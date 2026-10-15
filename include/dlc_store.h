// ============================================================
// dlc_store.h -- the single source of truth for DLC ownership.
//
// WHY THIS EXISTS
// DLC unlocking used to live in two places that disagreed:
//
//   * CSteamAppsStub (globals.h) -- only reached when Steam is absent
//     (GetStubbedLol). Its BIsSubscribedApp/BIsDlcInstalled walked the
//     configured id list and then returned true unconditionally anyway, so
//     the list did nothing and every id was "owned".
//   * A single MinHook on ISteamApps::BIsSubscribedApp (dllmain.cpp) for the
//     normal PASSTHROUGH path. Everything else -- BIsDlcInstalled, GetDLCCount,
//     BGetDLCDataByIndex -- went to real Steam, which answers for the SPOOFED
//     AppId (480) and therefore reports no DLC at all.
//
// That combination is why unlocking was hit-and-miss: games that ask
// "do I own <id>?" worked, and games that ENUMERATE their DLC through
// GetDLCCount/BGetDLCDataByIndex saw an empty list, because nothing answered
// those. Both paths now come through here.
//
// SEMANTICS
// The edge cases below are not invented -- they match Goldberg/gbe_fork's
// Steam_Apps, which encodes behaviour learned from real games:
//   * appId 0 is false everywhere (a game that gets true here can hang on its
//     loading screen).
//   * UINT32_MAX is true for ownership but false for "installed", which is what
//     real Steam does.
//   * The running app reports its OWN id as an installed DLC. Age of Empires 2
//     DE relies on this -- without it, it loads only one game mode.
//
// ENUMERATION NEEDS NAMES
// UnlockAll can answer any "do I own this?" question without knowing the ids,
// but it cannot invent a LIST. GetDLCCount/BGetDLCDataByIndex can only report
// DLC that was named explicitly in the ini, so games that build their DLC menu
// from the API need real entries, not just UnlockAll.
// ============================================================
#pragma once

#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>

struct UcoDlcEntry
{
    uint32_t    appId    = 0;
    bool        available = true;
    std::string name;
};

class UcoDlcStore
{
public:
    // Parses [DLC] from union-crax.ini:
    //
    //   [DLC]
    //   UnlockAll=1              ; answer "owned" for any id we're asked about
    //   2280850=Deluxe Edition   ; named entries are what enumeration can report
    //
    // The legacy [Settings] UnlockDLC=a,b,c list is still honoured and merged in
    // (unnamed, so those entries enumerate as "DLC <id>").
    static void Load(const char* iniPath, uint32_t ownAppId)
    {
        s_ownAppId = ownAppId;
        s_entries.clear();
        if (!iniPath || !iniPath[0]) return;

        // Read as a STRING, not with GetPrivateProfileInt.
        //
        // GetPrivateProfileInt only understands digits, so "UnlockAll=true" --
        // the spelling the README documents and patch.bat writes -- parsed as
        // the default, 0. The feature silently did nothing for every user who
        // followed the documentation; it only ever appeared to work when a
        // game also had explicit <appid>=<name> entries doing the real work.
        //
        // Accept the same truthy values as the rest of the ini, and still take
        // 1/0 so existing configs keep working.
        {
            char raw[64] = { 0 };
            GetPrivateProfileStringA("DLC", "UnlockAll", "", raw, sizeof(raw), iniPath);

            // Windows does not strip inline comments, so "true ; why" arrives
            // whole. Cut at a comment marker that follows whitespace.
            for (char* c = raw; *c; ++c) {
                if ((*c == '#' || *c == ';') && c > raw && (c[-1] == ' ' || c[-1] == '\t')) { *c = '\0'; break; }
            }
            char* p = raw;
            while (*p == ' ' || *p == '\t') ++p;
            size_t len = strlen(p);
            while (len && (p[len-1] == ' ' || p[len-1] == '\t')) p[--len] = '\0';

            s_unlockAll = (_stricmp(p, "true") == 0) || (_stricmp(p, "yes") == 0) ||
                          (_stricmp(p, "on")   == 0) || (_stricmp(p, "1")   == 0);
        }

        // Every key in [DLC] except UnlockAll is "<appid>=<name>". Passing a null
        // key name yields the section's key names as a double-null-terminated
        // block.
        std::vector<char> keys(16384);
        const DWORD n = GetPrivateProfileStringA("DLC", nullptr, "", keys.data(),
                                                 (DWORD)keys.size(), iniPath);
        if (n > 0) {
            for (const char* k = keys.data(); *k; k += strlen(k) + 1) {
                if (_stricmp(k, "UnlockAll") == 0) continue;
                const uint32_t id = (uint32_t)strtoul(k, nullptr, 0);
                if (!id) continue;                       // not an appid key
                char name[192] = {};
                GetPrivateProfileStringA("DLC", k, "", name, sizeof(name), iniPath);
                Add(id, name[0] ? name : nullptr);
            }
        }

        // Legacy list. Kept working so existing setups don't silently regress.
        char legacy[2048] = {};
        GetPrivateProfileStringA("Settings", "UnlockDLC", "", legacy, sizeof(legacy), iniPath);
        if (legacy[0]) {
            char* ctx = nullptr;
            for (char* tok = strtok_s(legacy, ",", &ctx); tok; tok = strtok_s(nullptr, ",", &ctx)) {
                while (*tok == ' ' || *tok == '\t') ++tok;
                const uint32_t id = (uint32_t)strtoul(tok, nullptr, 0);
                if (id) Add(id, nullptr);
            }
        }
    }

    static void Add(uint32_t appId, const char* name)
    {
        if (!appId) return;
        for (auto& e : s_entries) {
            if (e.appId == appId) {                      // last name wins
                if (name && name[0]) e.name = name;
                return;
            }
        }
        UcoDlcEntry e;
        e.appId = appId;
        e.available = true;
        if (name && name[0]) {
            e.name = name;
        } else {
            char buf[32];
            sprintf_s(buf, sizeof(buf), "DLC %u", appId);
            e.name = buf;
        }
        s_entries.push_back(e);
    }

    // ISteamApps::BIsSubscribedApp semantics.
    static bool IsOwned(uint32_t appId)
    {
        if (appId == 0)          return false;           // real Steam: false
        if (appId == UINT32_MAX) return true;            // real Steam: true
        if (s_ownAppId && appId == s_ownAppId) return true;
        if (Listed(appId))       return true;
        return s_unlockAll;
    }

    // ISteamApps::BIsDlcInstalled semantics. Deliberately differs from IsOwned
    // at both ends -- see the header comment.
    static bool IsInstalled(uint32_t appId)
    {
        if (appId == 0)          return false;
        if (appId == UINT32_MAX) return false;
        if (s_ownAppId && appId == s_ownAppId) return true;   // AoE2 DE relies on this
        if (Listed(appId))       return true;
        return s_unlockAll;
    }

    static int Count() { return (int)s_entries.size(); }

    static bool Get(int index, uint32_t* pAppId, bool* pAvailable,
                    char* pchName, int cchName)
    {
        if (index < 0 || index >= (int)s_entries.size()) return false;
        const UcoDlcEntry& e = s_entries[(size_t)index];
        if (pAppId)     *pAppId = e.appId;
        if (pAvailable) *pAvailable = e.available;
        if (pchName && cchName > 0) {
            memset(pchName, 0, (size_t)cchName);
            strncpy_s(pchName, (size_t)cchName, e.name.c_str(), _TRUNCATE);
        }
        return true;
    }

    static bool UnlockAll()  { return s_unlockAll; }
    static bool Active()     { return s_unlockAll || !s_entries.empty(); }
    static uint32_t OwnAppId() { return s_ownAppId; }

private:
    static bool Listed(uint32_t appId)
    {
        for (const auto& e : s_entries)
            if (e.appId == appId) return e.available;
        return false;
    }

    static std::vector<UcoDlcEntry> s_entries;
    static bool     s_unlockAll;
    static uint32_t s_ownAppId;
};

// Single translation unit (dllmain.cpp), same as the rest of these headers.
std::vector<UcoDlcEntry> UcoDlcStore::s_entries;
bool     UcoDlcStore::s_unlockAll = false;
uint32_t UcoDlcStore::s_ownAppId  = 0;
