// ============================================================
// UCOnline2 -- raft_mp Plugin (Raft current build, 1.1.01)
//
// Makes current (post-crossplay) Raft's PlayFab-gated multiplayer
// usable under UCOnline2 by pointing it at a PlayFab title YOU
// control and logging in without Steam validation. Modules:
//
//   1. OFFLINE-GATE UNLOCK (Mono, Raft_Network.Update hook)
//      - force Raft_Network.SignedIntoPlayfab = true
//      - write the real Steam ID into Raft_Network.localSteamID
//        (Network_UserId == struct{ulong}; single 8-byte write)
//
//   2. PLAYFAB TITLE REDIRECT (Mono)
//      - set PlayFab.PlayFabSettings.TitleId to our title (ini
//        [Playfab]TitleId). Transport-agnostic; done at the source.
//        (WinHTTP hooking does NOT work -- UnityPlayer only imports
//        WinHttpGetIEProxyConfigForCurrentUser; UnityWebRequest uses
//        Unity's own HTTP stack.)
//
//   3. LOGIN SWITCH (Mono, PlayFabUnityHttp.MakeApiCall hook)
//      - LoginWithSteam can't succeed on our title (no Steam add-on
//        / publisher key -> "SteamNotEnabledForTitle"). Rewrite the
//        outgoing request: URL  /Client/LoginWithSteam -> LoginWithCustomID,
//        body -> {"TitleId":"<ours>","CustomId":"<steamID>","CreateAccount":true}.
//        Response type (LoginResult) is identical, so the game's
//        OnSteamLoginSuccess/UserSignedIn run unchanged and we get a
//        real EntityToken (which PlayFab Party then uses).
//
// Mono-only. MinHook statically linked.
// ============================================================
#include <Windows.h>
#include <winhttp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>

#include "../../include/MinHook.h"
#include "../../include/uco_plugin.h"
#include "mono_runtime.h"

static UCO_LogFn      g_Log            = nullptr;
static volatile LONG  g_bShutdown      = 0;
static HANDLE         g_hWatcherThread = nullptr;
static ISteamUser*    g_pSteamUser     = nullptr;
static char           g_TitleIdA[64]   = {};   // our PlayFab TitleId (ini)
static wchar_t        g_TitleIdW[64]   = {};   // wide, for WinHTTP host rewrite

#define LOG(...) do { if (g_Log) g_Log(__VA_ARGS__); } while (0)

extern "C" void MONO_Log(const char* fmt, ...)
{
    if (!g_Log) return;
    char buf[1024];
    va_list ap; va_start(ap, fmt);
    _vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, ap);
    va_end(ap);
    g_Log("%s", buf);
}

static const char* GetIniPath()
{
    static char path[MAX_PATH] = {};
    static bool computed = false;
    if (computed) return path[0] ? path : nullptr;
    computed = true;
    char exeDir[MAX_PATH] = {};
    DWORD len = GetModuleFileNameA(nullptr, exeDir, MAX_PATH);
    if (len == 0) return nullptr;
    for (int i = (int)len - 1; i >= 0; --i)
        if (exeDir[i] == '\\' || exeDir[i] == '/') { exeDir[i] = 0; break; }
    int n = _snprintf_s(path, sizeof(path), _TRUNCATE, "%s\\union-crax.ini", exeDir);
    if (n <= 0) { path[0] = 0; return nullptr; }
    return path;
}

typedef uint64_t (*Fn_GetSteamID)(ISteamUser*);
static uint64_t GetRealSteamID()
{
    if (!g_pSteamUser) return 0;
    HMODULE h = GetModuleHandleA("steam_api64.dll");
    if (!h) return 0;
    Fn_GetSteamID f = (Fn_GetSteamID)GetProcAddress(h, "SteamAPI_ISteamUser_GetSteamID");
    if (!f) return 0;
    return f(g_pSteamUser);
}

// ------------------------------------------------------------
// MODULE 2: PlayFab TitleId redirect (managed set_TitleId)
// ------------------------------------------------------------
typedef void (*Fn_setTitleId)(void* /*MonoString*/);
static volatile LONG g_TitleIdRedirected = 0;

static void TryRedirectTitleId()
{
    if (!g_TitleIdA[0]) return;
    if (InterlockedCompareExchange(&g_TitleIdRedirected, 0, 0)) return;
    void* fn = MONO_FindMethodPtr("PlayFab", "PlayFab", "PlayFabSettings", "set_TitleId", 1);
    if (!fn) return;
    MonoString* s = MONO_StringNew(g_TitleIdA);
    if (!s) return;
    ((Fn_setTitleId)fn)(s);
    InterlockedExchange(&g_TitleIdRedirected, 1);
    LOG("[RaftRedirect] PlayFabSettings.TitleId -> %s", g_TitleIdA);
}

// ------------------------------------------------------------
// MODULE 1: offline-gate unlock (Raft_Network.Update hook)
// ------------------------------------------------------------
typedef void(__fastcall* Fn_Update)(void* pThis);
static Fn_Update     g_origUpdate          = nullptr;
static MonoClass*    g_RaftNetworkClass    = nullptr;
static int           g_localSteamIDOffset  = -1;
static volatile LONG g_AppliedLogged       = 0;

static void __fastcall Hooked_Update(void* pThis)
{
    if (pThis && g_RaftNetworkClass) {
        // Clear the offline gate so the multiplayer UI is usable even before
        // PlayFab login finishes. Once LoginWithCustomID succeeds the game's
        // OnPlayFabSignedIn sets this true anyway.
        uint8_t trueVal = 1;
        MONO_SetStaticFieldByName(g_RaftNetworkClass, "SignedIntoPlayfab", &trueVal);

        // IMPORTANT: do NOT override localSteamID. The current build identifies
        // players over the PlayFab Party network by their PlayFab entity ID
        // (TitleAccountId), which OnPlayFabSignedIn assigns to localSteamID on a
        // successful login. Forcing it to the raw Steam ID made the joiner fail
        // to find its own local player in the received world (NRE in
        // GameManager.OnWorldRecieved -> SelfDisconnect). Login works now, so
        // we leave localSteamID entirely to the game.

        if (InterlockedExchange(&g_AppliedLogged, 1) == 0)
            LOG("[RaftUnlock] applied: SignedIntoPlayfab=true (localSteamID left to OnPlayFabSignedIn)");

        TryRedirectTitleId();
    }
    if (g_origUpdate) g_origUpdate(pThis);
}

// ------------------------------------------------------------
// MODULE 3: login switch (PlayFabUnityHttp.MakeApiCall hook)
// ------------------------------------------------------------
typedef void(__fastcall* Fn_MakeApiCall)(void* pThis, void* reqContainer);
static Fn_MakeApiCall g_origMakeApiCall  = nullptr;
static int            g_offFullUrl       = -1;
static int            g_offPayload       = -1;
static int            g_offApiEndpoint   = -1;
static volatile LONG  g_LoginSwitchLogged = 0;

static void __fastcall Hooked_MakeApiCall(void* pThis, void* reqContainer)
{
    if (reqContainer && g_offFullUrl >= 0) {
        MonoString* urlObj = *(MonoString**)((uint8_t*)reqContainer + g_offFullUrl);
        char url[512] = {};
        if (urlObj && MONO_StringToUtf8(urlObj, url, sizeof(url))) {
            const char* hit = strstr(url, "LoginWithSteam");
            if (hit) {
                // 1) URL: LoginWithSteam -> LoginWithCustomID
                size_t pre = (size_t)(hit - url);
                char newUrl[512] = {};
                _snprintf_s(newUrl, sizeof(newUrl), _TRUNCATE, "%.*s%s%s",
                            (int)pre, url, "LoginWithCustomID",
                            hit + (sizeof("LoginWithSteam") - 1));
                MonoString* nu = MONO_StringNew(newUrl);
                if (nu) *(void**)((uint8_t*)reqContainer + g_offFullUrl) = nu;
                if (g_offApiEndpoint >= 0) {
                    MonoString* ep = MONO_StringNew("/Client/LoginWithCustomID");
                    if (ep) *(void**)((uint8_t*)reqContainer + g_offApiEndpoint) = ep;
                }
                // 2) Body: LoginWithCustomID payload
                uint64_t sid = GetRealSteamID();
                char body[256] = {};
                int blen = _snprintf_s(body, sizeof(body), _TRUNCATE,
                    "{\"TitleId\":\"%s\",\"CustomId\":\"%llu\",\"CreateAccount\":true}",
                    g_TitleIdA, (unsigned long long)sid);
                if (blen > 0 && g_offPayload >= 0) {
                    MonoObject* arr = MONO_NewByteArray(body, blen);
                    if (arr) *(void**)((uint8_t*)reqContainer + g_offPayload) = arr;
                }
                if (InterlockedExchange(&g_LoginSwitchLogged, 1) == 0)
                    LOG("[RaftLogin] LoginWithSteam -> LoginWithCustomID (CustomId=%llu, body=%d bytes)",
                        (unsigned long long)sid, blen);
            }
        }
    }
    if (g_origMakeApiCall) g_origMakeApiCall(pThis, reqContainer);
}

// ------------------------------------------------------------
// MODULE 4: native Party endpoint redirect (WinHTTP)
// PartyWin32.dll (native, User-Agent PlayFabParty/1.0) POSTs to
// <RaftTitle>.playfabapi.com/Party/RequestParty using OUR entity
// token -> PlayFab rejects "InvalidAPIEndpoint, use <ourTitle>...".
// The C# PlayFabSettings.TitleId redirect doesn't reach the native
// lib. PartyWin32 uses WinHTTP, so rewrite the WinHttpConnect host
// (*.playfabapi.com -> <ourTitle>.playfabapi.com). Length-independent;
// both share the *.playfabapi.com wildcard cert so TLS still validates.
// ------------------------------------------------------------
typedef HINTERNET (WINAPI* Fn_WinHttpConnect)(HINTERNET, LPCWSTR, INTERNET_PORT, DWORD);
static Fn_WinHttpConnect g_origWinHttpConnect = nullptr;
static volatile LONG     g_WinHttpRedirLogged = 0;

static HINTERNET WINAPI Hooked_WinHttpConnect(HINTERNET hSession, LPCWSTR pswzServerName,
                                              INTERNET_PORT nServerPort, DWORD dwReserved)
{
    if (pswzServerName && g_TitleIdW[0] && wcsstr(pswzServerName, L".playfabapi.com")) {
        wchar_t newName[128] = {};
        _snwprintf_s(newName, _countof(newName), _TRUNCATE, L"%s.playfabapi.com", g_TitleIdW);
        if (_wcsicmp(pswzServerName, newName) != 0) {
            if (InterlockedExchange(&g_WinHttpRedirLogged, 1) == 0)
                LOG("[RaftParty] WinHttpConnect redirect %ls -> %ls", pswzServerName, newName);
            return g_origWinHttpConnect(hSession, newName, nServerPort, dwReserved);
        }
    }
    return g_origWinHttpConnect(hSession, pswzServerName, nServerPort, dwReserved);
}

// ------------------------------------------------------------
// Install (idempotent per-hook; quiet retry until JIT-ready)
// ------------------------------------------------------------
static volatile LONG g_UpdateHookDone = 0;
static volatile LONG g_LoginHookDone  = 0;
static volatile LONG g_WinHttpHookDone = 0;

static bool TryInstallAll()
{
    if (!MONO_TryInit()) return false;
    g_RaftNetworkClass = MONO_FindClass("Assembly-CSharp", "", "Raft_Network");
    if (!g_RaftNetworkClass) return false;

    // -- Update hook (offline unlock + redirect) --
    if (!InterlockedCompareExchange(&g_UpdateHookDone, 0, 0)) {
        g_localSteamIDOffset = MONO_GetFieldOffset(g_RaftNetworkClass, "localSteamID");
        void* fn = MONO_FindMethodPtr("Assembly-CSharp", "", "Raft_Network", "Update", 0);
        if (fn && MH_CreateHook(fn, (void*)&Hooked_Update, (void**)&g_origUpdate) == MH_OK) {
            MH_EnableHook(fn);
            InterlockedExchange(&g_UpdateHookDone, 1);
            LOG("[RaftUnlock] Raft_Network.Update hook @ %p (localSteamID off 0x%X)",
                fn, (unsigned)g_localSteamIDOffset);
        }
    }

    // -- Login switch hook --
    if (!InterlockedCompareExchange(&g_LoginHookDone, 0, 0)) {
        MonoClass* crc = MONO_FindClass("PlayFab", "PlayFab.Internal", "CallRequestContainer");
        void* mac = MONO_FindMethodPtr("PlayFab", "PlayFab.Internal", "PlayFabUnityHttp", "MakeApiCall", 1);
        if (crc && mac) {
            g_offFullUrl     = MONO_GetFieldOffset(crc, "FullUrl");
            g_offPayload     = MONO_GetFieldOffset(crc, "Payload");
            g_offApiEndpoint = MONO_GetFieldOffset(crc, "ApiEndpoint");
            if (g_offFullUrl >= 0 && g_offPayload >= 0 &&
                MH_CreateHook(mac, (void*)&Hooked_MakeApiCall, (void**)&g_origMakeApiCall) == MH_OK) {
                MH_EnableHook(mac);
                InterlockedExchange(&g_LoginHookDone, 1);
                LOG("[RaftLogin] PlayFabUnityHttp.MakeApiCall hook @ %p (FullUrl off 0x%X, Payload off 0x%X)",
                    mac, (unsigned)g_offFullUrl, (unsigned)g_offPayload);
            }
        }
    }

    // -- Native Party endpoint redirect (WinHTTP) --
    if (g_TitleIdW[0] && !InterlockedCompareExchange(&g_WinHttpHookDone, 0, 0)) {
        HMODULE hWin = GetModuleHandleW(L"winhttp.dll");
        if (hWin) {
            void* pc = (void*)GetProcAddress(hWin, "WinHttpConnect");
            if (pc && MH_CreateHook(pc, (void*)&Hooked_WinHttpConnect,
                                    (void**)&g_origWinHttpConnect) == MH_OK) {
                MH_EnableHook(pc);
                InterlockedExchange(&g_WinHttpHookDone, 1);
                LOG("[RaftParty] WinHttpConnect hook @ %p (redirect *.playfabapi.com -> %s.playfabapi.com)",
                    pc, g_TitleIdA);
            }
        }
    }

    return InterlockedCompareExchange(&g_UpdateHookDone, 0, 0) &&
           InterlockedCompareExchange(&g_LoginHookDone, 0, 0) &&
           (!g_TitleIdW[0] || InterlockedCompareExchange(&g_WinHttpHookDone, 0, 0));
}

static DWORD WINAPI WatcherProc(LPVOID)
{
    for (int i = 0; i < 1500 && InterlockedCompareExchange(&g_bShutdown, 0, 0) == 0; ++i) {
        if (TryInstallAll()) return 0;
        Sleep(200);
    }
    LOG("[RaftUnlock] watcher stopped (UpdateHook=%ld LoginHook=%ld)",
        InterlockedCompareExchange(&g_UpdateHookDone, 0, 0),
        InterlockedCompareExchange(&g_LoginHookDone, 0, 0));
    return 0;
}

extern "C" __declspec(dllexport) int __cdecl UCO_PluginInit(const UCO_PluginContext* ctx)
{
    if (!ctx) return 1;
    if (ctx->ApiVersion != UCO_PLUGIN_API_VERSION) return 2;
    g_Log        = ctx->Log;
    g_pSteamUser = ctx->pSteamUser;

    const char* ini = GetIniPath();
    if (ini) {
        GetPrivateProfileStringA("Playfab", "TitleId", "", g_TitleIdA, sizeof(g_TitleIdA), ini);
        if (!g_TitleIdA[0]) GetPrivateProfileStringA("Playfab", "PlayFabTitleId", "", g_TitleIdA, sizeof(g_TitleIdA), ini);
        if (!g_TitleIdA[0]) GetPrivateProfileStringA("Raft",    "PlayFabTitleId", "", g_TitleIdA, sizeof(g_TitleIdA), ini);
        if (g_TitleIdA[0])
            MultiByteToWideChar(CP_ACP, 0, g_TitleIdA, -1, g_TitleIdW, _countof(g_TitleIdW));
    }

    LOG("[RaftUnlock] init: AppId=%u ogAppId=%u pSteamUser=%p PlayFabTitleId=%s",
        ctx->ForcedAppId, ctx->OriginalAppId, (void*)g_pSteamUser,
        g_TitleIdA[0] ? g_TitleIdA : "(none - redirect/login disabled)");

    if (MH_Initialize() != MH_OK)
        LOG("[RaftUnlock] MH_Initialize non-OK (already inited?)");

    g_hWatcherThread = CreateThread(nullptr, 0, WatcherProc, nullptr, 0, nullptr);
    return 0;
}

extern "C" __declspec(dllexport) void __cdecl UCO_PluginShutdown(void)
{
    InterlockedExchange(&g_bShutdown, 1);
    if (g_hWatcherThread) {
        WaitForSingleObject(g_hWatcherThread, 1000);
        CloseHandle(g_hWatcherThread);
        g_hWatcherThread = nullptr;
    }
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    LOG("[RaftUnlock] shutdown");
}

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) { return TRUE; }
