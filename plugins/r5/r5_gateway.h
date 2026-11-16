// ============================================================
// r5_gateway.h -- the Windrose coop-gateway redirect, shared by both builds.
//
// The same hook is needed in two different processes:
//   * the GAME, where it loads as a UCOnline2 plugin (r5.dll), and
//   * the DEDICATED SERVER, which links no steam_api64 at all and so takes a
//     version.dll proxy instead (r5_server/version.dll).
//
// Both hit the same wall -- the coop gateway refuses an emulated Steam ticket
// with 424 "Ticket for other app" -- and both are fixed the same way, so the
// logic lives here and only the entry point differs.
//
// UR5CaHttpClient::Init is 1534 bytes in every build seen so far (game and
// server, old and new), which is a useful check when re-deriving its address.
// ============================================================
#pragma once

#include <Windows.h>
#include <shlwapi.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "shlwapi.lib")

#include "../../include/MinHook.h"

// ------------------------------------------------------------
// Config ([R5] in union-crax.ini)
// ------------------------------------------------------------
struct R5Config
{
    uint64_t CaHttpInitRVA;         // UR5CaHttpClient::Init
    char     GatewayHostOverride[128];
    bool     bValid;
};
static R5Config g_Cfg = {};

static void IniCleanValue(char* v)
{
    if (!v) return;
    for (char* c = v; *c; ++c)
        if ((*c == '#' || *c == ';') && c > v && (c[-1] == ' ' || c[-1] == '\t')) { *c = '\0'; break; }
    size_t n = strlen(v);
    while (n && (v[n - 1] == ' ' || v[n - 1] == '\t' || v[n - 1] == '\r' || v[n - 1] == '\n')) v[--n] = '\0';
    char* q = v; while (*q == ' ' || *q == '\t') ++q;
    if (q != v) memmove(v, q, strlen(q) + 1);
}

static void LoadConfig()
{
    char ini[MAX_PATH] = {};
    if (!GetModuleFileNameA(nullptr, ini, MAX_PATH)) return;
    for (int i = (int)strlen(ini) - 1; i >= 0; --i)
        if (ini[i] == '\\' || ini[i] == '/') { ini[i] = 0; break; }
    strncat_s(ini, MAX_PATH, "\\union-crax.ini", _TRUNCATE);

    char buf[64] = {};
    GetPrivateProfileStringA("R5", "CaHttpInitRVA", "0", buf, sizeof(buf), ini);
    IniCleanValue(buf);
    g_Cfg.CaHttpInitRVA = (uint64_t)_strtoui64(buf, nullptr, 0);

    GetPrivateProfileStringA("R5", "GatewayHostOverride", "",
                             g_Cfg.GatewayHostOverride, sizeof(g_Cfg.GatewayHostOverride), ini);
    IniCleanValue(g_Cfg.GatewayHostOverride);

    // Opt-in twice over: an RVA is a raw address into THIS build, so it must
    // never be applied to a game it was not measured against.
    g_Cfg.bValid = (g_Cfg.CaHttpInitRVA != 0 && g_Cfg.GatewayHostOverride[0] != '\0');
}

// ------------------------------------------------------------
// Logging (host logger once available, %TEMP% before that)
// ------------------------------------------------------------
// Matches UCO_LogFn, declared locally so this header stands alone: the server
// proxy is not a UCOnline2 plugin and must not depend on the plugin ABI.
typedef void (*R5_LogFn)(const char* fmt, ...);
static R5_LogFn g_Log = nullptr;

static void LOG(const char* fmt, ...)
{
    char line[1024];
    va_list ap; va_start(ap, fmt);
    _vsnprintf_s(line, sizeof(line), _TRUNCATE, fmt, ap);
    va_end(ap);

    if (g_Log) { g_Log("%s", line); return; }

    char path[MAX_PATH] = {};
    if (GetTempPathA(MAX_PATH, path)) {
        strncat_s(path, MAX_PATH, "uc_online2.log", _TRUNCATE);
        FILE* f = nullptr;
        if (fopen_s(&f, path, "a") == 0 && f) { fprintf(f, "%s\n", line); fclose(f); }
    }
}

static bool IsReadable(const void* p, size_t bytes)
{
    if (!p) return false;
    MEMORY_BASIC_INFORMATION mbi = {};
    if (!VirtualQuery(p, &mbi, sizeof(mbi))) return false;
    if (mbi.State != MEM_COMMIT) return false;
    if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return false;
    const uint8_t* start = (const uint8_t*)mbi.BaseAddress;
    return ((const uint8_t*)p + bytes) <= (start + mbi.RegionSize);
}

// UE's FString: {Data, Num, Max}, Num counting the terminating NUL.
struct UeFString { const wchar_t* Data; int32_t Num; int32_t Max; };

// ------------------------------------------------------------
// The gateway host rewrite
// ------------------------------------------------------------
typedef void* (__fastcall* Fn_CaHttpInit)(void*, void*, void*, void*);
static Fn_CaHttpInit g_orig_CaHttpInit = nullptr;
static volatile LONG g_bRewritten = 0;

static bool TryRewriteEndpoint(void* p, const char* where)
{
    UeFString* fs = (UeFString*)p;
    if (!p || !IsReadable(fs, sizeof(*fs))) return false;
    if (!fs->Data || fs->Num < 8 || fs->Num > 512) return false;
    if (!IsReadable(fs->Data, (size_t)fs->Num * 2)) return false;

    char narrow[512] = {};
    WideCharToMultiByte(CP_UTF8, 0, fs->Data, fs->Num - 1, narrow, sizeof(narrow) - 1, nullptr, nullptr);
    if (!StrStrIA(narrow, "windrose.support")) return false;   // only ever the gateway

    wchar_t repl[192] = {};
    const int n = MultiByteToWideChar(CP_UTF8, 0, g_Cfg.GatewayHostOverride, -1, repl, 192) - 1;
    if (n <= 0 || n + 1 > fs->Num) {
        if (InterlockedExchange(&g_bRewritten, 1) == 0)
            LOG("[R5] gateway host override \"%s\" (%d chars) does not fit the game's "
                "buffer (%d) -- pick a shorter hostname.",
                g_Cfg.GatewayHostOverride, n, fs->Num - 1);
        return false;
    }

    // In place, into the game's own allocation: shorter only, so nothing is
    // reallocated and the game still owns and frees it.
    memcpy((void*)fs->Data, repl, (size_t)(n + 1) * sizeof(wchar_t));
    fs->Num = n + 1;

    if (InterlockedExchange(&g_bRewritten, 1) == 0)
        LOG("[R5] coop gateway host rewritten (%s): \"%s\" -> \"%s\"",
            where, narrow, g_Cfg.GatewayHostOverride);
    return true;
}

static void* __fastcall Hooked_CaHttpInit(void* a1, void* a2, void* a3, void* a4)
{
    // Deliberately a SEARCH rather than a fixed offset: the endpoint may arrive
    // as an argument or already live in the object, and the layout moves
    // between builds. Matching on "windrose.support" means we only ever touch
    // the string we mean to, whatever its offset turns out to be.
    void* args[3] = { a2, a3, a4 };
    bool done = false;
    for (int i = 0; i < 3 && !done; ++i)
        done = TryRewriteEndpoint(args[i], "arg");
    if (!done && a1 && IsReadable(a1, 0x200)) {
        for (int off = 0; off < 0x200 && !done; off += 8)
            done = TryRewriteEndpoint((uint8_t*)a1 + off, "field");
    }
    return g_orig_CaHttpInit ? g_orig_CaHttpInit(a1, a2, a3, a4) : nullptr;
}

// ------------------------------------------------------------
// Install
//
// From the watcher thread, never DllMain: MH_EnableHook suspends every thread
// in the process, and doing that while DllMain holds the loader lock deadlocks
// the moment a suspended thread wants that lock. That cost two frozen startups
// on this game before it was understood.
// ------------------------------------------------------------
static volatile LONG g_bInstalled = 0;

static void InstallHook()
{
    if (!g_Cfg.bValid) return;
    if (InterlockedExchange(&g_bInstalled, 1) != 0) return;

    uint8_t* base = (uint8_t*)GetModuleHandleW(nullptr);
    if (!base) return;

    const IMAGE_DOS_HEADER* dos = (const IMAGE_DOS_HEADER*)base;
    if (!IsReadable(dos, sizeof(*dos)) || dos->e_magic != IMAGE_DOS_SIGNATURE) return;
    const IMAGE_NT_HEADERS64* nt = (const IMAGE_NT_HEADERS64*)(base + dos->e_lfanew);
    if (!IsReadable(nt, sizeof(*nt)) || nt->Signature != IMAGE_NT_SIGNATURE) return;
    const uint64_t imageSize = nt->OptionalHeader.SizeOfImage;

    if (g_Cfg.CaHttpInitRVA >= imageSize) {
        LOG("[R5] CaHttpInitRVA 0x%llX is outside the image (size 0x%llX) -- wrong build? "
            "Not hooking.", (unsigned long long)g_Cfg.CaHttpInitRVA,
            (unsigned long long)imageSize);
        return;
    }

    MH_STATUS s = MH_Initialize();
    if (s != MH_OK && s != MH_ERROR_ALREADY_INITIALIZED) {
        LOG("[R5] MH_Initialize failed: %d", s);
        return;
    }

    void* target = (void*)(base + g_Cfg.CaHttpInitRVA);
    s = MH_CreateHook(target, (void*)&Hooked_CaHttpInit, (void**)&g_orig_CaHttpInit);
    if (s == MH_OK && MH_EnableHook(target) == MH_OK)
        LOG("[R5] UR5CaHttpClient::Init hooked @ %p -- coop gateway will be sent to \"%s\".",
            target, g_Cfg.GatewayHostOverride);
    else
        LOG("[R5] FAILED to hook UR5CaHttpClient::Init @ %p: %d", target, s);
}


