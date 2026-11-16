// ============================================================
// UCOnline2 -- local ISteamRemoteStorage (Steam Cloud) emulation
//
// THE PROBLEM: real Steam binds ISteamRemoteStorage to the RUNNING app, which
// under UCO2 is the spoofed AppId (480/Spacewar). So every UCO2 game's cloud
// saves land in ONE shared bucket:
//
//   Steam\userdata\<account>\480\remote\
//       OutbreakSettings.data          <- game A
//       SaveData                       <- game B  (generic name: collision bait)
//       Saved\SaveGames\SaveData.dat   <- game C
//
// Two ways that bites. Locally, two games that both pick a common name overwrite
// each other. Worse, Spacewar's cloud really does sync, so on a second machine
// Steam pulls game A's "SaveData" down and game B reads it as its own.
//
// THE FIX ([Settings] LocalSaves, default ON) is two halves:
//
//   1. Report cloud as DISABLED (IsCloudEnabledForAccount/ForApp -> false).
//      Games that keep their own local save path -- most of them -- then use it,
//      and the save lands where the game's own docs/backups/save editors expect.
//      Nothing of ours is involved.
//   2. Serve the file API from a local folder for the games that DON'T have
//      another path. Plenty of titles use ISteamRemoteStorage AS their
//      filesystem, exactly as Valve's docs suggest; for those, "disable cloud"
//      changes nothing -- in real Steam that flag only stops the UPLOAD, writes
//      still go to userdata\<account>\<appid>\remote. So they need somewhere
//      else to land, and this is it:
//
//          <game folder>\uco_cloud\
//
// Existing saves are imported once from real Steam's 480 bucket on the first run
// (see ImportFromSteam) -- copied, never moved, so the originals stay put. That
// bucket is shared, so the import can pull in a file belonging to another game;
// harmless (this game only ever asks for its own) and better than stranding a save.
//
// Workshop/UGC is deliberately NOT emulated -- it is a different system that
// needs a real server, and a bad fake is worse than passthrough.
//
// Included into dllmain.cpp AFTER globals.h + callback_dispatcher.h + the SDK
// headers (relies on that include order, like api_flat.h). No SDK includes here.
// ============================================================
#pragma once
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <map>
#include <vector>
#include <string>
#include <mutex>

namespace UcoCloudEmu {

    static std::recursive_mutex s_lock;
    static bool s_enabled = false;
    static bool s_imported = false;
    static std::string s_root;                 // <game>\uco_cloud  (no trailing slash)

    // Enumeration cache: GetFileNameAndSize hands back a const char* the game may
    // hold onto, so the strings have to outlive the call.
    static std::vector<std::pair<std::string, int32>> s_listing;

    // Async reads: handle -> bytes waiting for FileReadAsyncComplete.
    static std::map<SteamAPICall_t, std::vector<uint8_t>> s_reads;
    // hCall -> { iCallback, serialized call result }, same scheme as the inventory
    // emu. High range so it never collides with a real Steam SteamAPICall_t.
    static std::map<SteamAPICall_t, std::pair<int, std::vector<uint8_t>>> s_calls;
    static SteamAPICall_t s_nextCall = 0x5100000000000000ULL;

    // Write streams: handle -> { absolute path, buffered bytes }.
    static std::map<UGCFileWriteStreamHandle_t, std::pair<std::string, std::vector<uint8_t>>> s_streams;
    static UGCFileWriteStreamHandle_t s_nextStream = 1;

    inline bool Enabled() { return s_enabled; }

    // --- path plumbing --------------------------------------------------------

    // Steam file names are relative and may contain subdirectories ("Saved/
    // SaveGames/x.dat"). Normalise to backslashes, refuse anything that climbs
    // out of the root, and return the absolute path.
    inline bool ResolvePath(const char* name, std::string& out) {
        if (!s_enabled || !name || !name[0]) return false;
        std::string rel(name);
        for (char& c : rel) if (c == '/') c = '\\';
        while (!rel.empty() && rel[0] == '\\') rel.erase(0, 1);
        if (rel.empty()) return false;
        // ".." anywhere is a traversal attempt; a game has no business doing it.
        if (rel == ".." || rel.find("..\\") != std::string::npos) return false;
        if (rel.size() >= 3 && rel.compare(rel.size() - 3, 3, "\\..") == 0) return false;
        if (rel.size() > 1 && rel[1] == ':') return false;      // absolute path
        out = s_root + "\\" + rel;
        return true;
    }

    inline void EnsureParentDirs(const std::string& full) {
        size_t pos = s_root.size();
        for (;;) {
            size_t slash = full.find('\\', pos + 1);
            if (slash == std::string::npos) break;
            CreateDirectoryA(full.substr(0, slash).c_str(), nullptr);
            pos = slash;
        }
    }

    inline bool ReadWholeFile(const std::string& path, std::vector<uint8_t>& out) {
        FILE* f = nullptr; fopen_s(&f, path.c_str(), "rb");
        if (!f) return false;
        fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
        if (n < 0) { fclose(f); return false; }
        out.resize((size_t)n);
        size_t got = n ? fread(out.data(), 1, (size_t)n, f) : 0;
        fclose(f);
        out.resize(got);
        return true;
    }

    inline bool WriteWholeFile(const std::string& path, const void* data, size_t n) {
        EnsureParentDirs(path);
        FILE* f = nullptr; fopen_s(&f, path.c_str(), "wb");
        if (!f) return false;
        bool ok = (n == 0) || (fwrite(data, 1, n, f) == n);
        fclose(f);
        return ok;
    }

    // --- setup ----------------------------------------------------------------

    // iniPath is <game>\union-crax.ini; the store goes beside it. Deliberately NOT
    // keyed by Steam account: the point is to look like a game that keeps saves in
    // its own folder, and those are not per-Steam-account either.
    inline void Init(const char* iniPath) {
        if (!iniPath || !iniPath[0]) return;
        std::string dir(iniPath);
        size_t slash = dir.find_last_of("\\/");
        dir = (slash == std::string::npos) ? std::string(".") : dir.substr(0, slash);
        std::lock_guard<std::recursive_mutex> g(s_lock);
        s_root = dir + "\\uco_cloud";
        CreateDirectoryA(s_root.c_str(), nullptr);
        s_enabled = true;
        UCOLOG("[UCOnline2] LocalSaves: Steam Cloud reported OFF; ISteamRemoteStorage "
            "served from %s", s_root.c_str());
    }

    // Copy anything already sitting in the spoofed app's cloud bucket into the
    // local store, so an existing save is not stranded the first time LocalSaves
    // is on. Never deletes from Steam -- if this misfires the original is there.
    //
    // Called by UcoInstallRemoteStorageHooks BEFORE it hooks the interface, and
    // through RAW VTABLE SLOTS rather than the C++ type: the object may be an
    // older interface version whose layout does not match our SDK header, and
    // once the hooks are on, reading through it would just read this store back.
    typedef int32       (S_CALLTYPE *PfnGetFileCount)(void*);
    typedef const char* (S_CALLTYPE *PfnGetFileNameAndSize)(void*, int, int32*);
    typedef int32       (S_CALLTYPE *PfnFileRead)(void*, const char*, void*, int32);

    inline void ImportFromSteam(void* pIface, PfnGetFileCount pfnCount,
        PfnGetFileNameAndSize pfnNameAndSize, PfnFileRead pfnRead)
    {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        if (!s_enabled || s_imported) return;
        s_imported = true;
        if (!pIface || !pfnCount || !pfnNameAndSize || !pfnRead) return;

        int32 count = pfnCount(pIface);
        if (count <= 0) return;

        int copied = 0, skipped = 0;
        for (int32 i = 0; i < count; ++i) {
            int32 size = 0;
            const char* name = pfnNameAndSize(pIface, (int)i, &size);
            if (!name || !name[0] || size < 0) continue;
            std::string full;
            if (!ResolvePath(name, full)) continue;
            if (GetFileAttributesA(full.c_str()) != INVALID_FILE_ATTRIBUTES) { ++skipped; continue; }
            std::vector<uint8_t> buf((size_t)size);
            int32 got = size ? pfnRead(pIface, name, buf.data(), size) : 0;
            if (got < 0) continue;
            if (WriteWholeFile(full, buf.data(), (size_t)got)) {
                ++copied;
                UCOLOG("[UCOnline2] LocalSaves: imported '%s' (%d bytes) from the "
                    "spoofed app's cloud folder", name, got);
            }
        }
        if (copied || skipped)
            UCOLOG("[UCOnline2] LocalSaves: import done -- %d copied, %d already local "
                "(originals left untouched in the folder for the spoofed AppId)",
                copied, skipped);
    }

    // --- file operations ------------------------------------------------------

    inline bool FileExists(const char* name) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        std::string full;
        if (!ResolvePath(name, full)) return false;
        DWORD attr = GetFileAttributesA(full.c_str());
        return attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY);
    }

    inline bool FileWrite(const char* name, const void* data, int32 cub) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        std::string full;
        if (!ResolvePath(name, full) || cub < 0) return false;
        return WriteWholeFile(full, data, (size_t)cub);
    }

    inline int32 FileRead(const char* name, void* data, int32 cubToRead) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        std::string full;
        if (!ResolvePath(name, full) || !data || cubToRead <= 0) return 0;
        std::vector<uint8_t> buf;
        if (!ReadWholeFile(full, buf)) return 0;
        int32 n = (int32)buf.size() < cubToRead ? (int32)buf.size() : cubToRead;
        if (n > 0) memcpy(data, buf.data(), (size_t)n);
        return n;
    }

    inline bool FileDelete(const char* name) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        std::string full;
        if (!ResolvePath(name, full)) return false;
        return DeleteFileA(full.c_str()) != 0;
    }

    // FileForget means "stop syncing, keep the local copy". With nothing syncing,
    // that is a no-op that succeeds.
    inline bool FileForget(const char* name) { return FileExists(name); }

    inline int32 GetFileSize(const char* name) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        std::string full;
        if (!ResolvePath(name, full)) return 0;
        WIN32_FILE_ATTRIBUTE_DATA fad{};
        if (!GetFileAttributesExA(full.c_str(), GetFileExInfoStandard, &fad)) return 0;
        return (int32)fad.nFileSizeLow;
    }

    inline int64 GetFileTimestamp(const char* name) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        std::string full;
        if (!ResolvePath(name, full)) return 0;
        WIN32_FILE_ATTRIBUTE_DATA fad{};
        if (!GetFileAttributesExA(full.c_str(), GetFileExInfoStandard, &fad)) return 0;
        ULARGE_INTEGER ul{};
        ul.LowPart = fad.ftLastWriteTime.dwLowDateTime;
        ul.HighPart = fad.ftLastWriteTime.dwHighDateTime;
        // FILETIME (100ns ticks since 1601) -> unix seconds.
        return (int64)((ul.QuadPart - 116444736000000000ULL) / 10000000ULL);
    }

    // --- enumeration ----------------------------------------------------------

    inline void ScanInto(const std::string& dir, const std::string& prefix) {
        WIN32_FIND_DATAA ffd{};
        HANDLE h = FindFirstFileA((dir + "\\*").c_str(), &ffd);
        if (h == INVALID_HANDLE_VALUE) return;
        do {
            if (strcmp(ffd.cFileName, ".") == 0 || strcmp(ffd.cFileName, "..") == 0) continue;
            std::string rel = prefix.empty() ? std::string(ffd.cFileName)
                                             : prefix + "/" + ffd.cFileName;
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                ScanInto(dir + "\\" + ffd.cFileName, rel);
            else
                s_listing.emplace_back(rel, (int32)ffd.nFileSizeLow);
        } while (FindNextFileA(h, &ffd));
        FindClose(h);
    }

    inline int32 GetFileCount() {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        s_listing.clear();
        if (s_enabled) ScanInto(s_root, "");
        return (int32)s_listing.size();
    }

    inline const char* GetFileNameAndSize(int iFile, int32* pnFileSizeInBytes) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        if (iFile < 0 || (size_t)iFile >= s_listing.size()) {
            if (pnFileSizeInBytes) *pnFileSizeInBytes = 0;
            return "";
        }
        if (pnFileSizeInBytes) *pnFileSizeInBytes = s_listing[iFile].second;
        return s_listing[iFile].first.c_str();
    }

    // --- cloud state ----------------------------------------------------------

    // The disk is the quota. Report a round 1 GiB so a game that sanity-checks
    // free space before saving is satisfied.
    inline bool GetQuota(uint64* pnTotal, uint64* puAvailable) {
        const uint64 oneGiB = 1024ULL * 1024ULL * 1024ULL;
        if (pnTotal) *pnTotal = oneGiB;
        if (puAvailable) *puAvailable = oneGiB;
        return true;
    }

    // Half the fix: a game that has its own local save path takes it when cloud
    // reads as off, and never touches this emulation at all.
    inline bool IsCloudEnabled() { return false; }
    inline bool FilePersisted(const char* name) { return FileExists(name); }

    // --- async ----------------------------------------------------------------

    inline bool IsOurCall(SteamAPICall_t h) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        return s_calls.find(h) != s_calls.end();
    }

    inline SteamAPICall_t PostCallResult(int iCallback, const void* res, int cub) {
        SteamAPICall_t hCall = s_nextCall++;
        std::vector<uint8_t> data((size_t)cub);
        memcpy(data.data(), res, (size_t)cub);
        s_calls[hCall] = { iCallback, std::move(data) };

        SteamAPICallCompleted_t done{};
        done.m_hAsyncCall = hCall;
        done.m_iCallback = iCallback;
        done.m_cubParam = (uint32)cub;
        GetDispatcher()->PostCallback(SteamAPICallCompleted_t::k_iCallback, &done,
            sizeof(done), g_ClientUser, false, 10);
        return hCall;
    }

    inline SteamAPICall_t FileWriteAsync(const char* name, const void* data, uint32 cub) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        bool ok = FileWrite(name, data, (int32)cub);
        RemoteStorageFileWriteAsyncComplete_t res{};
        res.m_eResult = ok ? k_EResultOK : k_EResultFail;
        return PostCallResult(RemoteStorageFileWriteAsyncComplete_t::k_iCallback, &res, sizeof(res));
    }

    inline SteamAPICall_t FileReadAsync(const char* name, uint32 nOffset, uint32 cubToRead) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        std::string full;
        std::vector<uint8_t> buf;
        bool ok = ResolvePath(name, full) && ReadWholeFile(full, buf);

        std::vector<uint8_t> slice;
        if (ok && nOffset <= buf.size()) {
            size_t avail = buf.size() - nOffset;
            size_t n = cubToRead < avail ? cubToRead : avail;
            slice.assign(buf.begin() + nOffset, buf.begin() + nOffset + n);
        } else {
            ok = false;
        }

        RemoteStorageFileReadAsyncComplete_t res{};
        res.m_eResult = ok ? k_EResultOK : k_EResultFileNotFound;
        res.m_nOffset = nOffset;
        res.m_cubRead = (uint32)slice.size();
        SteamAPICall_t hCall = PostCallResult(
            RemoteStorageFileReadAsyncComplete_t::k_iCallback, &res, sizeof(res));
        // The struct carries its own handle, so that field can only be filled in
        // once PostCallResult has allocated one.
        auto it = s_calls.find(hCall);
        if (it != s_calls.end())
            reinterpret_cast<RemoteStorageFileReadAsyncComplete_t*>(
                it->second.second.data())->m_hFileReadAsync = hCall;
        if (ok) s_reads[hCall] = std::move(slice);
        return hCall;
    }

    inline bool FileReadAsyncComplete(SteamAPICall_t hReadCall, void* pvBuffer, uint32 cubToRead) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        auto it = s_reads.find(hReadCall);
        if (it == s_reads.end() || !pvBuffer) return false;
        uint32 n = (uint32)it->second.size() < cubToRead ? (uint32)it->second.size() : cubToRead;
        if (n) memcpy(pvBuffer, it->second.data(), n);
        s_reads.erase(it);
        return true;
    }

    inline bool IsAPICallCompleted(SteamAPICall_t, bool* pbFailed) {
        if (pbFailed) *pbFailed = false;
        return true;   // ours complete immediately
    }

    inline bool GetAPICallResult(SteamAPICall_t hCall, void* pBuf, int cubBuf, int, bool* pbFailed) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        auto it = s_calls.find(hCall);
        if (it == s_calls.end()) return false;
        if (pbFailed) *pbFailed = false;
        int n = (int)it->second.second.size();
        if (pBuf && cubBuf >= n) memcpy(pBuf, it->second.second.data(), n);
        return true;
    }

    // --- write streams --------------------------------------------------------

    inline UGCFileWriteStreamHandle_t FileWriteStreamOpen(const char* name) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        std::string full;
        if (!ResolvePath(name, full)) return k_UGCFileStreamHandleInvalid;
        UGCFileWriteStreamHandle_t h = s_nextStream++;
        s_streams[h] = { full, {} };
        return h;
    }

    inline bool FileWriteStreamWriteChunk(UGCFileWriteStreamHandle_t h, const void* data, int32 cub) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        auto it = s_streams.find(h);
        if (it == s_streams.end() || cub < 0) return false;
        const uint8_t* p = (const uint8_t*)data;
        it->second.second.insert(it->second.second.end(), p, p + cub);
        return true;
    }

    inline bool FileWriteStreamClose(UGCFileWriteStreamHandle_t h) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        auto it = s_streams.find(h);
        if (it == s_streams.end()) return false;
        bool ok = WriteWholeFile(it->second.first, it->second.second.data(), it->second.second.size());
        s_streams.erase(it);
        return ok;
    }

    inline bool FileWriteStreamCancel(UGCFileWriteStreamHandle_t h) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        return s_streams.erase(h) != 0;
    }
}
