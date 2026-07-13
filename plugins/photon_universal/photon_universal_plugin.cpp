// ============================================================
// UCOnline2 -- Photon Universal Plugin
//
// One DLL that handles everything UCOnline2 currently supports
// in the Photon space:
//
//   * Photon Realtime / PUN  (IL2CPP and Mono backends)
//   * Photon Fusion 2        (IL2CPP)
//   * Photon Voice           (paired with Realtime/PUN)
//   * Unity Services auth bypass  (SignInWithSteam -> anonymous)
//   * Phasmophobia SteamAuth gate NOP (game-specific)
//
// At init we auto-detect:
//   * Unity backend (Mono vs IL2CPP) via runtime DLL presence
//   * Photon flavor (Realtime/PUN vs Fusion) via metadata/assembly scan
//   * Phasmo byte signature in GameAssembly.dll
// and install only the relevant module hooks.
//
// INI config (union-crax.ini next to the game exe):
//
//   [Realtime]
//   PhotonAppIdRealtime=<your Realtime app GUID>
//   PhotonAppIdVoice=<your Voice app GUID>      ; optional
//   ForcedAuthType=0
//
//   [Fusion]
//   PhotonAppIdFusion=<your Fusion app GUID>
//   ForcedAuthType=0
//
// You only need to populate the section(s) matching the game's
// Photon flavor. Mega plugin reads both; the inactive one is
// just ignored.
//
// MinHook is statically linked.
// ============================================================
#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../../include/MinHook.h"
#include "../../include/uco_plugin.h"

#include "il2cpp_runtime.h"
#include "mono_runtime.h"

// ============================================================
// SHARED INFRA
// ============================================================
static UCO_LogFn g_Log              = nullptr;
static uint32_t  g_ForcedAppId      = 480;
static uint32_t  g_OriginalAppId    = 0;
static volatile LONG g_bShutdown    = 0;
static HANDLE    g_hWatcherThread   = nullptr;

#define LOG(...) do { if (g_Log) g_Log(__VA_ARGS__); } while (0)

// MONO_Log and IL2CPP_Log are extern "C" hooks the runtime helpers
// call to surface diagnostics. Both forward to g_Log.
extern "C" void MONO_Log(const char* fmt, ...)
{
    if (!g_Log) return;
    char buf[1024];
    va_list ap; va_start(ap, fmt);
    _vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, ap);
    va_end(ap);
    g_Log("%s", buf);
}
extern "C" void IL2CPP_Log(const char* fmt, ...)
{
    if (!g_Log) return;
    char buf[1024];
    va_list ap; va_start(ap, fmt);
    _vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, ap);
    va_end(ap);
    g_Log("%s", buf);
}

// Resolve <game>\union-crax.ini path once.
static const char* GetIniPath()
{
    static char path[MAX_PATH] = {};
    static bool computed = false;
    if (computed) return path[0] ? path : nullptr;
    computed = true;
    char exeDir[MAX_PATH] = {};
    DWORD len = GetModuleFileNameA(nullptr, exeDir, MAX_PATH);
    if (len == 0) return nullptr;
    for (int i = (int)len - 1; i >= 0; --i) {
        if (exeDir[i] == '\\' || exeDir[i] == '/') { exeDir[i] = 0; break; }
    }
    int n = _snprintf_s(path, sizeof(path), _TRUNCATE, "%s\\union-crax.ini", exeDir);
    if (n <= 0) { path[0] = 0; return nullptr; }
    return path;
}

// ============================================================
// MODULE: Realtime / PUN (IL2CPP backend)
//
// Hooks LoadBalancingPeer.OpAuthenticate / OpAuthenticateOnce,
// PhotonPeer.SendOperation, and AuthenticationValues.set_AuthType.
// Per-peer classifier routes the first peer to Realtime AppId
// and the second distinct peer to Voice AppId.
// ============================================================
namespace ModRealtimeIL2CPP {

static char  g_AppIdUtf8[64]      = {};
static void* g_AppIdString        = nullptr;
static bool  g_AppIdPatchEnabled  = false;
static char  g_VoiceAppIdUtf8[64] = {};
static void* g_VoiceAppIdString   = nullptr;
static bool  g_VoiceAppIdEnabled  = false;
static unsigned int g_ForcedAuthType = 0;
static const size_t kOffsetAuthType = 0x10;

struct PeerSlot { void* pThis; int product; };
static PeerSlot g_Peers[4] = {};
static int      g_PeerCount = 0;
static CRITICAL_SECTION g_PeerCs;
static bool             g_PeerCsInit = false;

static int ClassifyPeer(void* pThis)
{
    if (!pThis) return 0;
    EnterCriticalSection(&g_PeerCs);
    int product = 0;
    bool found = false;
    for (int i = 0; i < g_PeerCount; ++i) {
        if (g_Peers[i].pThis == pThis) { product = g_Peers[i].product; found = true; break; }
    }
    if (!found) {
        product = (g_PeerCount == 0) ? 0 : 1;
        if (g_PeerCount < (int)(sizeof(g_Peers)/sizeof(g_Peers[0]))) {
            g_Peers[g_PeerCount++] = { pThis, product };
            LOG("[Realtime] new peer %p classified as %s", pThis,
                product == 0 ? "Realtime" : "Voice");
        }
    }
    LeaveCriticalSection(&g_PeerCs);
    return product;
}

static void EnsureStrings()
{
    if (g_AppIdPatchEnabled && !g_AppIdString) {
        g_AppIdString = IL2CPP_StringNew(g_AppIdUtf8);
    }
    if (g_VoiceAppIdEnabled && !g_VoiceAppIdString) {
        g_VoiceAppIdString = IL2CPP_StringNew(g_VoiceAppIdUtf8);
    }
}

static void* PickAppIdString(void* pThis, const char** outName)
{
    EnsureStrings();
    int product = ClassifyPeer(pThis);
    void* r = (product == 1) ? g_VoiceAppIdString : g_AppIdString;
    if (outName) *outName = (product == 1) ? "Voice" : "Realtime";
    return r;
}
static const char* PickAppIdUtf8(void* pThis, const char** outName)
{
    int product = ClassifyPeer(pThis);
    if (outName) *outName = (product == 1) ? "Voice" : "Realtime";
    if (product == 1) return g_VoiceAppIdEnabled ? g_VoiceAppIdUtf8 : nullptr;
    return g_AppIdPatchEnabled ? g_AppIdUtf8 : nullptr;
}

static void PatchAuthType(void* authValues, const char* sender)
{
    if (!authValues) return;
    unsigned char* p = (unsigned char*)authValues + kOffsetAuthType;
    unsigned char prev = *p;
    *p = (unsigned char)(g_ForcedAuthType & 0xFF);
    LOG("[Realtime] %s: authValues.authType %u -> %u", sender, prev, g_ForcedAuthType);
}

typedef void (__fastcall *Fn_SetAuthType)(void* pThis, unsigned int value);
static Fn_SetAuthType g_pfnOrigSetAuthType = nullptr;
static void __fastcall Hooked_SetAuthType(void* pThis, unsigned int v) {
    g_pfnOrigSetAuthType(pThis, g_ForcedAuthType);
}

typedef bool (__fastcall *Fn_OpAuth)(void*, void*, void*, void*, void*, bool);
typedef bool (__fastcall *Fn_OpAuthOnce)(void*, void*, void*, void*, void*, int, int);
static Fn_OpAuth     g_pfnOrigOpAuth     = nullptr;
static Fn_OpAuthOnce g_pfnOrigOpAuthOnce = nullptr;

static bool __fastcall Hooked_OpAuth(void* pThis, void* appId, void* ver, void* auth, void* region, bool lobby) {
    const char* name = "Realtime";
    void* replace = PickAppIdString(pThis, &name);
    if (replace && appId != replace) {
        LOG("[Realtime] OpAuth (%s peer): appId arg %p -> %p", name, appId, replace);
        appId = replace;
    }
    PatchAuthType(auth, "OpAuth");
    return g_pfnOrigOpAuth(pThis, appId, ver, auth, region, lobby);
}
static bool __fastcall Hooked_OpAuthOnce(void* pThis, void* appId, void* ver, void* auth, void* region, int enc, int proto) {
    const char* name = "Realtime";
    void* replace = PickAppIdString(pThis, &name);
    if (replace && appId != replace) {
        LOG("[Realtime] OpAuthOnce (%s peer): appId arg %p -> %p", name, appId, replace);
        appId = replace;
    }
    PatchAuthType(auth, "OpAuthOnce");
    return g_pfnOrigOpAuthOnce(pThis, appId, ver, auth, region, enc, proto);
}

typedef bool (__fastcall *Fn_SendOp)(void* pThis, uint8_t op, void* params, void* opts, void* a5, void* a6);
static Fn_SendOp g_pfnOrigSendOp = nullptr;
static bool __fastcall Hooked_SendOp(void* pThis, uint8_t op, void* params, void* opts, void* a5, void* a6) {
    bool isAuth = (op == 220 || op == 226 || op == 230 || op == 231);
    if (isAuth && params) {
        const char* name = "Realtime";
        const char* userId = PickAppIdUtf8(pThis, &name);
        if (userId && userId[0]) {
            if (IL2CPP_DictByteStringSetItem((Il2CppObject*)params, 224, userId))
                LOG("[Realtime] SendOp op=%u (%s peer): params[224] AppId -> %s", op, name, userId);
        }
        if (g_ForcedAuthType <= 255) {
            IL2CPP_DictByteByteSetItem((Il2CppObject*)params, 217, (uint8_t)(g_ForcedAuthType & 0xFF));
        }
    }
    return g_pfnOrigSendOp(pThis, op, params, opts, a5, a6);
}

static bool TryInstall()
{
    if (!IL2CPP_IsReady()) return false;
    // Realtime classes live in Photon.Realtime image. If not found,
    // this game isn't a PUN/Realtime IL2CPP game.
    if (!IL2CPP_FindClass("Photon.Realtime", "Photon.Realtime", "LoadBalancingPeer")) return false;

    if (!g_PeerCsInit) { InitializeCriticalSection(&g_PeerCs); g_PeerCsInit = true; }

    void* fn;
    fn = IL2CPP_FindMethodPtr("Photon.Realtime", "Photon.Realtime", "AuthenticationValues", "set_AuthType", 1);
    if (fn) {
        if (MH_CreateHook(fn, (void*)&Hooked_SetAuthType, (void**)&g_pfnOrigSetAuthType) == MH_OK)
            { MH_EnableHook(fn); LOG("[Realtime] set_AuthType hook @ %p", fn); }
    }
    fn = IL2CPP_FindMethodPtr("Photon.Realtime", "Photon.Realtime", "LoadBalancingPeer", "OpAuthenticate", -1);
    if (fn) {
        if (MH_CreateHook(fn, (void*)&Hooked_OpAuth, (void**)&g_pfnOrigOpAuth) == MH_OK)
            { MH_EnableHook(fn); LOG("[Realtime] OpAuthenticate hook @ %p", fn); }
    }
    fn = IL2CPP_FindMethodPtr("Photon.Realtime", "Photon.Realtime", "LoadBalancingPeer", "OpAuthenticateOnce", -1);
    if (fn) {
        if (MH_CreateHook(fn, (void*)&Hooked_OpAuthOnce, (void**)&g_pfnOrigOpAuthOnce) == MH_OK)
            { MH_EnableHook(fn); LOG("[Realtime] OpAuthenticateOnce hook @ %p", fn); }
    }
    fn = IL2CPP_FindMethodPtr("Photon3Unity3D", "ExitGames.Client.Photon", "PhotonPeer", "SendOperation", -1);
    if (!fn)
        fn = IL2CPP_FindMethodPtr("Photon3Unity3D", "ExitGames.Client.Photon", "PeerBase", "SendOperation", -1);
    if (fn) {
        if (MH_CreateHook(fn, (void*)&Hooked_SendOp, (void**)&g_pfnOrigSendOp) == MH_OK)
            { MH_EnableHook(fn); LOG("[Realtime] SendOperation hook @ %p", fn); }
    }
    LOG("[Realtime] IL2CPP module active");
    return true;
}

static void ReadIni(const char* ini)
{
    GetPrivateProfileStringA("Realtime", "PhotonAppIdRealtime", "", g_AppIdUtf8, sizeof(g_AppIdUtf8), ini);
    if (!g_AppIdUtf8[0]) GetPrivateProfileStringA("PUN", "PhotonAppIdRealtime", "", g_AppIdUtf8, sizeof(g_AppIdUtf8), ini);
    g_AppIdPatchEnabled = (g_AppIdUtf8[0] != 0);
    GetPrivateProfileStringA("Realtime", "PhotonAppIdVoice", "", g_VoiceAppIdUtf8, sizeof(g_VoiceAppIdUtf8), ini);
    if (!g_VoiceAppIdUtf8[0]) GetPrivateProfileStringA("PUN", "PhotonAppIdVoice", "", g_VoiceAppIdUtf8, sizeof(g_VoiceAppIdUtf8), ini);
    g_VoiceAppIdEnabled = (g_VoiceAppIdUtf8[0] != 0);
    char buf[8] = {};
    GetPrivateProfileStringA("Realtime", "ForcedAuthType", "", buf, sizeof(buf), ini);
    if (!buf[0]) GetPrivateProfileStringA("PUN", "ForcedAuthType", "0", buf, sizeof(buf), ini);
    g_ForcedAuthType = (unsigned int)strtoul(buf, nullptr, 10);
    if (g_AppIdPatchEnabled)
        LOG("[Realtime] IL2CPP: Realtime AppId=%s Voice=%s AuthType=%u",
            g_AppIdUtf8, g_VoiceAppIdUtf8[0] ? g_VoiceAppIdUtf8 : "(none)", g_ForcedAuthType);
}
} // namespace ModRealtimeIL2CPP


// ============================================================
// MODULE: Realtime / PUN (Mono backend)
//
// Same shape as the IL2CPP module, different runtime helpers.
// ============================================================
namespace ModRealtimeMono {

static char        g_AppIdUtf8[64]      = {};
static MonoString* g_AppIdString        = nullptr;
static bool        g_AppIdPatchEnabled  = false;
static char        g_VoiceAppIdUtf8[64] = {};
static MonoString* g_VoiceAppIdString   = nullptr;
static bool        g_VoiceAppIdEnabled  = false;
static unsigned int g_ForcedAuthType = 0;
static const size_t kOffsetAuthType = 0x10;

struct PeerSlot { void* pThis; int product; };
static PeerSlot         g_Peers[4]  = {};
static int              g_PeerCount = 0;
static CRITICAL_SECTION g_PeerCs;
static bool             g_PeerCsInit = false;

static int ClassifyPeer(void* pThis)
{
    if (!pThis) return 0;
    EnterCriticalSection(&g_PeerCs);
    int product = 0;
    bool found = false;
    for (int i = 0; i < g_PeerCount; ++i) {
        if (g_Peers[i].pThis == pThis) { product = g_Peers[i].product; found = true; break; }
    }
    if (!found) {
        product = (g_PeerCount == 0) ? 0 : 1;
        if (g_PeerCount < (int)(sizeof(g_Peers)/sizeof(g_Peers[0]))) {
            g_Peers[g_PeerCount++] = { pThis, product };
            LOG("[Realtime/Mono] new peer %p classified as %s", pThis,
                product == 0 ? "Realtime" : "Voice");
        }
    }
    LeaveCriticalSection(&g_PeerCs);
    return product;
}

static void EnsureStrings()
{
    if (g_AppIdPatchEnabled && !g_AppIdString) g_AppIdString = MONO_StringNew(g_AppIdUtf8);
    if (g_VoiceAppIdEnabled && !g_VoiceAppIdString) g_VoiceAppIdString = MONO_StringNew(g_VoiceAppIdUtf8);
}
static MonoString* PickAppIdString(void* pThis, const char** outName)
{
    EnsureStrings();
    int product = ClassifyPeer(pThis);
    if (outName) *outName = (product == 1) ? "Voice" : "Realtime";
    if (product == 1) return g_VoiceAppIdEnabled ? g_VoiceAppIdString : nullptr;
    return g_AppIdPatchEnabled ? g_AppIdString : nullptr;
}
static const char* PickAppIdUtf8(void* pThis, const char** outName)
{
    int product = ClassifyPeer(pThis);
    if (outName) *outName = (product == 1) ? "Voice" : "Realtime";
    if (product == 1) return g_VoiceAppIdEnabled ? g_VoiceAppIdUtf8 : nullptr;
    return g_AppIdPatchEnabled ? g_AppIdUtf8 : nullptr;
}

static void PatchAuthType(void* authValues, const char* sender)
{
    if (!authValues) return;
    unsigned char* p = (unsigned char*)authValues + kOffsetAuthType;
    unsigned char prev = *p;
    *p = (unsigned char)(g_ForcedAuthType & 0xFF);
    LOG("[Realtime/Mono] %s: authValues.authType %u -> %u", sender, prev, g_ForcedAuthType);
}

typedef void (__fastcall *Fn_SetAuthType)(void* pThis, unsigned int value);
static Fn_SetAuthType g_pfnOrigSetAuthType = nullptr;
static void __fastcall Hooked_SetAuthType(void* pThis, unsigned int v) {
    g_pfnOrigSetAuthType(pThis, g_ForcedAuthType);
}

typedef bool (__fastcall *Fn_OpAuth)(void*, void*, void*, void*, void*, bool);
typedef bool (__fastcall *Fn_OpAuthOnce)(void*, void*, void*, void*, void*, int, int);
static Fn_OpAuth     g_pfnOrigOpAuth     = nullptr;
static Fn_OpAuthOnce g_pfnOrigOpAuthOnce = nullptr;
static bool __fastcall Hooked_OpAuth(void* pThis, void* appId, void* ver, void* auth, void* region, bool lobby) {
    const char* name = "Realtime";
    MonoString* replace = PickAppIdString(pThis, &name);
    if (replace && appId != replace) {
        LOG("[Realtime/Mono] OpAuth (%s peer): appId arg %p -> %p", name, appId, replace);
        appId = replace;
    }
    PatchAuthType(auth, "OpAuth");
    return g_pfnOrigOpAuth(pThis, appId, ver, auth, region, lobby);
}
static bool __fastcall Hooked_OpAuthOnce(void* pThis, void* appId, void* ver, void* auth, void* region, int enc, int proto) {
    const char* name = "Realtime";
    MonoString* replace = PickAppIdString(pThis, &name);
    if (replace && appId != replace) {
        LOG("[Realtime/Mono] OpAuthOnce (%s peer): appId arg %p -> %p", name, appId, replace);
        appId = replace;
    }
    PatchAuthType(auth, "OpAuthOnce");
    return g_pfnOrigOpAuthOnce(pThis, appId, ver, auth, region, enc, proto);
}

typedef bool (__fastcall *Fn_SendOp)(void* pThis, uint8_t op, void* params, void* opts, void* a5, void* a6);
static Fn_SendOp g_pfnOrigSendOp = nullptr;
static bool __fastcall Hooked_SendOp(void* pThis, uint8_t op, void* params, void* opts, void* a5, void* a6) {
    bool isAuth = (op == 220 || op == 226 || op == 230 || op == 231);
    if (isAuth && params) {
        const char* name = "Realtime";
        const char* userId = PickAppIdUtf8(pThis, &name);
        if (userId && userId[0]) {
            if (MONO_DictByteStringSetItem((MonoObject*)params, 224, userId))
                LOG("[Realtime/Mono] SendOp op=%u (%s peer): params[224] AppId -> %s", op, name, userId);
        }
        if (g_ForcedAuthType <= 255) {
            MONO_DictByteByteSetItem((MonoObject*)params, 217, (uint8_t)(g_ForcedAuthType & 0xFF));
        }
    }
    return g_pfnOrigSendOp(pThis, op, params, opts, a5, a6);
}

static bool TryInstall()
{
    if (!MONO_IsReady()) return false;
    if (!MONO_FindClass("Photon.Realtime", "Photon.Realtime", "LoadBalancingPeer")) return false;

    if (!g_PeerCsInit) { InitializeCriticalSection(&g_PeerCs); g_PeerCsInit = true; }

    void* fn;
    fn = MONO_FindMethodPtr("Photon.Realtime", "Photon.Realtime", "AuthenticationValues", "set_AuthType", 1);
    if (fn && MH_CreateHook(fn, (void*)&Hooked_SetAuthType, (void**)&g_pfnOrigSetAuthType) == MH_OK)
        { MH_EnableHook(fn); LOG("[Realtime/Mono] set_AuthType hook @ %p", fn); }
    fn = MONO_FindMethodPtr("Photon.Realtime", "Photon.Realtime", "LoadBalancingPeer", "OpAuthenticate", -1);
    if (fn && MH_CreateHook(fn, (void*)&Hooked_OpAuth, (void**)&g_pfnOrigOpAuth) == MH_OK)
        { MH_EnableHook(fn); LOG("[Realtime/Mono] OpAuthenticate hook @ %p", fn); }
    fn = MONO_FindMethodPtr("Photon.Realtime", "Photon.Realtime", "LoadBalancingPeer", "OpAuthenticateOnce", -1);
    if (fn && MH_CreateHook(fn, (void*)&Hooked_OpAuthOnce, (void**)&g_pfnOrigOpAuthOnce) == MH_OK)
        { MH_EnableHook(fn); LOG("[Realtime/Mono] OpAuthenticateOnce hook @ %p", fn); }
    fn = MONO_FindMethodPtr("Photon3Unity3D", "ExitGames.Client.Photon", "PhotonPeer", "SendOperation", -1);
    if (!fn) fn = MONO_FindMethodPtr("Photon3Unity3D", "ExitGames.Client.Photon", "PeerBase", "SendOperation", -1);
    if (fn && MH_CreateHook(fn, (void*)&Hooked_SendOp, (void**)&g_pfnOrigSendOp) == MH_OK)
        { MH_EnableHook(fn); LOG("[Realtime/Mono] SendOperation hook @ %p", fn); }
    LOG("[Realtime/Mono] module active");
    return true;
}

static void ReadIni(const char* ini)
{
    GetPrivateProfileStringA("Realtime", "PhotonAppIdRealtime", "", g_AppIdUtf8, sizeof(g_AppIdUtf8), ini);
    if (!g_AppIdUtf8[0]) GetPrivateProfileStringA("PUN", "PhotonAppIdRealtime", "", g_AppIdUtf8, sizeof(g_AppIdUtf8), ini);
    g_AppIdPatchEnabled = (g_AppIdUtf8[0] != 0);
    GetPrivateProfileStringA("Realtime", "PhotonAppIdVoice", "", g_VoiceAppIdUtf8, sizeof(g_VoiceAppIdUtf8), ini);
    if (!g_VoiceAppIdUtf8[0]) GetPrivateProfileStringA("PUN", "PhotonAppIdVoice", "", g_VoiceAppIdUtf8, sizeof(g_VoiceAppIdUtf8), ini);
    g_VoiceAppIdEnabled = (g_VoiceAppIdUtf8[0] != 0);
    char buf[8] = {};
    GetPrivateProfileStringA("Realtime", "ForcedAuthType", "", buf, sizeof(buf), ini);
    if (!buf[0]) GetPrivateProfileStringA("PUN", "ForcedAuthType", "0", buf, sizeof(buf), ini);
    g_ForcedAuthType = (unsigned int)strtoul(buf, nullptr, 10);
    if (g_AppIdPatchEnabled)
        LOG("[Realtime/Mono] Realtime AppId=%s Voice=%s AuthType=%u",
            g_AppIdUtf8, g_VoiceAppIdUtf8[0] ? g_VoiceAppIdUtf8 : "(none)", g_ForcedAuthType);
}
} // namespace ModRealtimeMono


// ============================================================
// MODULE: Photon Fusion 2 (IL2CPP)
//
// Different ScriptableObject (PhotonAppSettings), different
// namespace (Fusion.Photon.Realtime). Single AppId slot:
// AppIdFusion. We override via the get_Global hook which
// returns the singleton with our AppId stamped in.
// ============================================================
namespace ModFusion {

static char  g_AppIdUtf8[64]      = {};
static void* g_AppIdString        = nullptr;
static bool  g_AppIdPatchEnabled  = false;
static unsigned int g_ForcedAuthType = 0;
static const size_t kOffsetAppSettings_AppIdFusion = 0x18;
static const size_t kOffsetAuthType                = 0x10;

typedef void* (__fastcall *Fn_GetGlobal)();
static Fn_GetGlobal g_pfnOrigGetGlobal = nullptr;
static void* __fastcall Hooked_GetGlobal()
{
    void* settings = g_pfnOrigGetGlobal();
    if (!settings || !g_AppIdPatchEnabled) return settings;
    if (!g_AppIdString) g_AppIdString = IL2CPP_StringNew(g_AppIdUtf8);
    if (!g_AppIdString) return settings;
    // settings is a wrapper; the actual AppSettings is at settings+0x10 conventionally.
    void** pAppSettings = (void**)((char*)settings + 0x10);
    void* appSettings = *pAppSettings;
    if (!appSettings) return settings;
    void** pAppId = (void**)((char*)appSettings + kOffsetAppSettings_AppIdFusion);
    if (*pAppId != g_AppIdString) {
        void* old = *pAppId;
        *pAppId = g_AppIdString;
        LOG("[Fusion] AppIdFusion patched in singleton (was %p)", old);
    }
    return settings;
}

typedef void (__fastcall *Fn_SetAuthType)(void* pThis, unsigned int value);
static Fn_SetAuthType g_pfnOrigSetAuthType = nullptr;
static void __fastcall Hooked_SetAuthType(void* pThis, unsigned int v) {
    g_pfnOrigSetAuthType(pThis, g_ForcedAuthType);
}

typedef bool (__fastcall *Fn_OpAuth)(void*, void*, void*, void*, void*, bool);
typedef bool (__fastcall *Fn_OpAuthOnce)(void*, void*, void*, void*, void*, int, int);
static Fn_OpAuth     g_pfnOrigOpAuth     = nullptr;
static Fn_OpAuthOnce g_pfnOrigOpAuthOnce = nullptr;

static void PatchAuthType(void* authValues, const char* sender) {
    if (!authValues) return;
    unsigned char* p = (unsigned char*)authValues + kOffsetAuthType;
    unsigned char prev = *p;
    *p = (unsigned char)(g_ForcedAuthType & 0xFF);
    LOG("[Fusion] %s: authType %u -> %u", sender, prev, g_ForcedAuthType);
}
static bool __fastcall Hooked_OpAuth(void* pThis, void* appId, void* ver, void* auth, void* region, bool lobby) {
    PatchAuthType(auth, "OpAuth");
    return g_pfnOrigOpAuth(pThis, appId, ver, auth, region, lobby);
}
static bool __fastcall Hooked_OpAuthOnce(void* pThis, void* appId, void* ver, void* auth, void* region, int enc, int proto) {
    PatchAuthType(auth, "OpAuthOnce");
    return g_pfnOrigOpAuthOnce(pThis, appId, ver, auth, region, enc, proto);
}

static bool TryInstall()
{
    if (!IL2CPP_IsReady()) return false;
    if (!IL2CPP_FindClass("Fusion.Realtime", "Fusion.Photon.Realtime", "PhotonAppSettings")) return false;

    void* fn;
    if (g_AppIdPatchEnabled) {
        fn = IL2CPP_FindMethodPtr("Fusion.Realtime", "Fusion.Photon.Realtime", "PhotonAppSettings", "get_Global", 0);
        if (fn && MH_CreateHook(fn, (void*)&Hooked_GetGlobal, (void**)&g_pfnOrigGetGlobal) == MH_OK)
            { MH_EnableHook(fn); LOG("[Fusion] PhotonAppSettings.get_Global hook @ %p", fn); }
    }
    fn = IL2CPP_FindMethodPtr("Fusion.Realtime", "Fusion.Photon.Realtime", "AuthenticationValues", "set_AuthType", 1);
    if (fn && MH_CreateHook(fn, (void*)&Hooked_SetAuthType, (void**)&g_pfnOrigSetAuthType) == MH_OK)
        { MH_EnableHook(fn); LOG("[Fusion] set_AuthType hook @ %p", fn); }
    fn = IL2CPP_FindMethodPtr("Fusion.Realtime", "Fusion.Photon.Realtime", "LoadBalancingPeer", "OpAuthenticate", -1);
    if (fn && MH_CreateHook(fn, (void*)&Hooked_OpAuth, (void**)&g_pfnOrigOpAuth) == MH_OK)
        { MH_EnableHook(fn); LOG("[Fusion] OpAuthenticate hook @ %p", fn); }
    fn = IL2CPP_FindMethodPtr("Fusion.Realtime", "Fusion.Photon.Realtime", "LoadBalancingPeer", "OpAuthenticateOnce", -1);
    if (fn && MH_CreateHook(fn, (void*)&Hooked_OpAuthOnce, (void**)&g_pfnOrigOpAuthOnce) == MH_OK)
        { MH_EnableHook(fn); LOG("[Fusion] OpAuthenticateOnce hook @ %p", fn); }
    LOG("[Fusion] module active");
    return true;
}

static void ReadIni(const char* ini)
{
    GetPrivateProfileStringA("Fusion", "PhotonAppIdFusion", "", g_AppIdUtf8, sizeof(g_AppIdUtf8), ini);
    g_AppIdPatchEnabled = (g_AppIdUtf8[0] != 0);
    char buf[8] = {};
    GetPrivateProfileStringA("Fusion", "ForcedAuthType", "0", buf, sizeof(buf), ini);
    g_ForcedAuthType = (unsigned int)strtoul(buf, nullptr, 10);
    if (g_AppIdPatchEnabled)
        LOG("[Fusion] AppId=%s AuthType=%u", g_AppIdUtf8, g_ForcedAuthType);
}
} // namespace ModFusion


// ============================================================
// MODULE: Unity Services Auth (IL2CPP)
//
// Generic for any IL2CPP game that uses Unity Gaming Services:
// SignInWithSteamAsync -> SignInAnonymouslyAsync, plus stubs
// for LinkWithSteamAsync / UpdatePlayerNameAsync /
// RefreshAccessTokenAsync / HandleSignInRefreshRequestAsync.
// ============================================================
namespace ModUnityAuth {

typedef void* (__fastcall *Fn_Task)(void* pThis, void* a1, void* a2, void* a3);
static Fn_Task g_pfnSignInAnon          = nullptr;
static Fn_Task g_pfnOrigSignInSteam     = nullptr;
static Fn_Task g_pfnOrigLinkSteam       = nullptr;
static Fn_Task g_pfnOrigUpdatePlayer    = nullptr;
static Fn_Task g_pfnOrigRefreshToken    = nullptr;
static Fn_Task g_pfnOrigHandleSignIn    = nullptr;

typedef void* (__fastcall *Fn_GetCompleted)();
static Fn_GetCompleted g_pfnTaskCompleted = nullptr;

static void* __fastcall Hooked_SignInWithSteam(void* pThis, void* a1, void* a2, void* a3) {
    LOG("[Auth] SignInWithSteam intercepted -> SignInAnonymouslyAsync");
    if (g_pfnSignInAnon) return g_pfnSignInAnon(pThis, nullptr, nullptr, nullptr);
    return nullptr;
}
static void* __fastcall Hooked_LinkSteam(void* pThis, void* a1, void* a2, void* a3) {
    LOG("[Auth] LinkWithSteam intercepted -> CompletedTask");
    if (g_pfnTaskCompleted) return g_pfnTaskCompleted();
    return nullptr;
}
static void* __fastcall Hooked_UpdatePlayer(void* pThis, void* a1, void* a2, void* a3) {
    LOG("[Auth] UpdatePlayerName intercepted -> CompletedTask");
    if (g_pfnTaskCompleted) return g_pfnTaskCompleted();
    return nullptr;
}
static void* __fastcall Hooked_RefreshToken(void* pThis, void* a1, void* a2, void* a3) {
    LOG("[Auth] RefreshAccessToken intercepted -> CompletedTask");
    if (g_pfnTaskCompleted) return g_pfnTaskCompleted();
    return nullptr;
}
static void* __fastcall Hooked_HandleSignInRefresh(void* pThis, void* a1, void* a2, void* a3) {
    LOG("[Auth] HandleSignInRefreshRequest intercepted -> CompletedTask");
    if (g_pfnTaskCompleted) return g_pfnTaskCompleted();
    return nullptr;
}

static bool TryInstall()
{
    if (!IL2CPP_IsReady()) return false;
    if (!IL2CPP_FindClass("Unity.Services.Authentication", "Unity.Services.Authentication", "AuthenticationServiceInternal"))
        return false;

    g_pfnTaskCompleted = (Fn_GetCompleted)IL2CPP_FindMethodPtr("mscorlib", "System.Threading.Tasks", "Task", "get_CompletedTask", 0);
    g_pfnSignInAnon    = (Fn_Task)IL2CPP_FindMethodPtr(nullptr, "Unity.Services.Authentication", "AuthenticationServiceInternal", "SignInAnonymouslyAsync", -1);

    void* fn;
    fn = IL2CPP_FindMethodPtr(nullptr, "Unity.Services.Authentication", "AuthenticationServiceInternal", "SignInWithSteamAsync", -1);
    if (fn && MH_CreateHook(fn, (void*)&Hooked_SignInWithSteam, (void**)&g_pfnOrigSignInSteam) == MH_OK)
        { MH_EnableHook(fn); LOG("[Auth] SignInWithSteamAsync hook @ %p", fn); }
    fn = IL2CPP_FindMethodPtr(nullptr, "Unity.Services.Authentication", "AuthenticationServiceInternal", "LinkWithSteamAsync", -1);
    if (fn && MH_CreateHook(fn, (void*)&Hooked_LinkSteam, (void**)&g_pfnOrigLinkSteam) == MH_OK)
        { MH_EnableHook(fn); LOG("[Auth] LinkWithSteamAsync hook @ %p", fn); }
    fn = IL2CPP_FindMethodPtr(nullptr, "Unity.Services.Authentication", "AuthenticationServiceInternal", "UpdatePlayerNameAsync", -1);
    if (fn && MH_CreateHook(fn, (void*)&Hooked_UpdatePlayer, (void**)&g_pfnOrigUpdatePlayer) == MH_OK)
        { MH_EnableHook(fn); LOG("[Auth] UpdatePlayerNameAsync hook @ %p", fn); }
    fn = IL2CPP_FindMethodPtr(nullptr, "Unity.Services.Authentication", "AuthenticationServiceInternal", "RefreshAccessTokenAsync", -1);
    if (fn && MH_CreateHook(fn, (void*)&Hooked_RefreshToken, (void**)&g_pfnOrigRefreshToken) == MH_OK)
        { MH_EnableHook(fn); LOG("[Auth] RefreshAccessTokenAsync hook @ %p", fn); }
    fn = IL2CPP_FindMethodPtr(nullptr, "Unity.Services.Authentication", "AuthenticationServiceInternal", "HandleSignInRefreshRequestAsync", -1);
    if (fn && MH_CreateHook(fn, (void*)&Hooked_HandleSignInRefresh, (void**)&g_pfnOrigHandleSignIn) == MH_OK)
        { MH_EnableHook(fn); LOG("[Auth] HandleSignInRefreshRequestAsync hook @ %p", fn); }
    LOG("[Auth] Unity Services auth module active");
    return true;
}
} // namespace ModUnityAuth


// ============================================================
// MODULE: Phasmophobia SteamAuth gate NOP (IL2CPP, game-specific)
//
// NOPs the je at GameAssembly+0xEAF7CE that gates the "Failed to
// get Steam account information" error branch. Byte-verifies
// before patching so non-Phasmo or new-build games are skipped
// cleanly.
// ============================================================
namespace ModPhasmoGate {

static bool TryInstall()
{
    HMODULE ga = GetModuleHandleA("GameAssembly.dll");
    if (!ga) return false;
    const uintptr_t kPatchRva = 0xEAF7CE;
    uint8_t* site = (uint8_t*)ga + kPatchRva;
    const uint8_t kExpected[6] = { 0x0F, 0x84, 0xC3, 0x06, 0x00, 0x00 };
    if (memcmp(site, kExpected, 6) != 0) {
        // Not Phasmo build 23249745 — silent skip (most games hit this branch).
        return false;
    }
    DWORD oldProt = 0;
    if (!VirtualProtect(site, 6, PAGE_EXECUTE_READWRITE, &oldProt)) {
        LOG("[Phasmo] VirtualProtect failed GLE=%lu", GetLastError());
        return false;
    }
    for (int i = 0; i < 6; ++i) site[i] = 0x90;
    DWORD tmp = 0;
    VirtualProtect(site, 6, oldProt, &tmp);
    FlushInstructionCache(GetCurrentProcess(), site, 6);
    LOG("[Phasmo] SteamAccountGate NOPed at GameAssembly+0x%llx", (unsigned long long)kPatchRva);
    return true;
}
} // namespace ModPhasmoGate


// ============================================================
// ORCHESTRATOR
// ============================================================
static volatile LONG g_RealtimeIL2CPP_Done = 0;
static volatile LONG g_RealtimeMono_Done   = 0;
static volatile LONG g_Fusion_Done         = 0;
static volatile LONG g_UnityAuth_Done      = 0;
static volatile LONG g_PhasmoGate_Done     = 0;

static void RunDetectionPass()
{
    // Phasmo gate is byte-pattern — try at any time once GameAssembly is loaded.
    if (!InterlockedCompareExchange(&g_PhasmoGate_Done, 0, 0)) {
        if (ModPhasmoGate::TryInstall())
            InterlockedExchange(&g_PhasmoGate_Done, 1);
    }
    // IL2CPP-based modules
    if (IL2CPP_TryInit()) {
        if (!InterlockedCompareExchange(&g_RealtimeIL2CPP_Done, 0, 0))
            if (ModRealtimeIL2CPP::TryInstall()) InterlockedExchange(&g_RealtimeIL2CPP_Done, 1);
        if (!InterlockedCompareExchange(&g_Fusion_Done, 0, 0))
            if (ModFusion::TryInstall())         InterlockedExchange(&g_Fusion_Done, 1);
        if (!InterlockedCompareExchange(&g_UnityAuth_Done, 0, 0))
            if (ModUnityAuth::TryInstall())      InterlockedExchange(&g_UnityAuth_Done, 1);
    }
    // Mono-based module
    if (MONO_TryInit()) {
        if (!InterlockedCompareExchange(&g_RealtimeMono_Done, 0, 0))
            if (ModRealtimeMono::TryInstall()) InterlockedExchange(&g_RealtimeMono_Done, 1);
    }
}

static DWORD WINAPI WatcherProc(LPVOID)
{
    // Poll up to ~2 minutes for runtime + Photon classes to be loaded.
    for (int i = 0; i < 600 && InterlockedCompareExchange(&g_bShutdown, 0, 0) == 0; ++i) {
        RunDetectionPass();
        // If everything that wanted to activate has activated, we can stop.
        // But we don't know what "should" activate, so just keep polling
        // until shutdown or timeout. Cheap.
        Sleep(200);
    }
    return 0;
}

extern "C" __declspec(dllexport) int __cdecl UCO_PluginInit(const UCO_PluginContext* ctx)
{
    if (!ctx) return 1;
    if (ctx->ApiVersion != UCO_PLUGIN_API_VERSION) return 2;
    g_Log           = ctx->Log;
    g_ForcedAppId   = ctx->ForcedAppId;
    g_OriginalAppId = ctx->OriginalAppId;

    LOG("[Universal] photon_universal plugin init: AppId=%u ogAppId=%u",
        g_ForcedAppId, g_OriginalAppId);

    const char* ini = GetIniPath();
    if (ini) {
        ModRealtimeIL2CPP::ReadIni(ini);
        ModRealtimeMono::ReadIni(ini);
        ModFusion::ReadIni(ini);
    } else {
        LOG("[Universal] no union-crax.ini found");
    }

    if (MH_Initialize() != MH_OK)
        LOG("[Universal] MH_Initialize non-OK (already inited?)");

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
    LOG("[Universal] plugin shutdown");
}

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) { return TRUE; }
