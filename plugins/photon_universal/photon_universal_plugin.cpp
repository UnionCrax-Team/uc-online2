// ============================================================
// UCOnline2 -- Photon Universal Plugin
//
// One DLL that handles everything UCOnline2 currently supports
// in the Photon space:
//
//   * Photon Realtime / PUN  (IL2CPP and Mono backends)
//   * Photon Fusion 2        (IL2CPP)
//   * Photon Voice           (paired with Realtime/PUN)
//
// At init we auto-detect:
//   * Unity backend (Mono vs IL2CPP) via runtime DLL presence
//   * Photon flavor (Realtime/PUN vs Fusion) via metadata/assembly scan
// and install only the relevant module hooks.
//
// INI config (union-crax.ini next to the game exe):
//
//   [Realtime]
//   PhotonAppIdRealtime=<your Realtime app GUID>
//   PhotonAppIdVoice=<your Voice app GUID>      ; optional
//   ForcedAuthType=0
//   VerboseLog=1        ; optional: log EVERY Photon response + status change
//                       ; (failed connect/auth and dropped connections are
//                       ;  always logged, with or without it)
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
// Photon response / connection-state logging
//
// The hooks in each module only see what the client SENDS. When a connection
// fails, the reason comes back from the server: an OperationResponse with a
// non-zero ReturnCode and a DebugMessage ("Invalid AppId", "custom
// authentication failed", ...), or a StatusCode on the peer (timeout,
// server-side disconnect). Without those, a failed login shows up in the log
// as silence followed by the game giving up.
//
// So each module also hooks LoadBalancingClient.OnOperationResponse and
// OnStatusChanged. What gets written:
//   always      a FAILED connect/auth/join operation, and abnormal status
//               changes (timeouts, server disconnects, connect exceptions).
//               A few lines per session at most -- and exactly the lines a
//               report needs, so they don't wait on someone turning a flag on.
//   VerboseLog  ([Realtime]/[PUN]/[Fusion] VerboseLog=1) every operation
//               response and every status change, successful or not.
// ============================================================
static bool g_PhotonVerbose = false;

static const char* PhotonOpName(uint8_t op)
{
    switch (op) {
    case 217: return "GetGameList";
    case 218: return "ServerSettings";
    case 219: return "WebRpc";
    case 220: return "GetRegions";
    case 221: return "GetLobbyStats";
    case 222: return "FindFriends";
    case 225: return "JoinRandomGame";
    case 226: return "JoinGame";
    case 227: return "CreateGame";
    case 228: return "LeaveLobby";
    case 229: return "JoinLobby";
    case 230: return "Authenticate";
    case 231: return "AuthenticateOnce";
    case 248: return "ChangeGroups";
    case 250: return "ExchangeKeysForEncryption";
    case 251: return "GetProperties";
    case 252: return "SetProperties";
    case 253: return "RaiseEvent";
    case 254: return "Leave";
    case 255: return "Join";
    default:  return "?";
    }
}

// Every code and name in these tables was checked against the shipped
// PhotonRealtime / Photon3Unity3D metadata; anything else logs as "?" with
// its number.

// Photon.Realtime.ErrorCode
static const char* PhotonReturnCodeName(int rc)
{
    switch (rc) {
    case 0:     return "Ok";
    case -3:    return "OperationNotAllowedInCurrentState";
    case -2:    return "InvalidOperation";
    case -1:    return "InternalServerError";
    case 32767: return "InvalidAuthentication";
    case 32766: return "GameIdAlreadyExists";
    case 32765: return "GameFull";
    case 32764: return "GameClosed";
    case 32763: return "AlreadyMatched";
    case 32762: return "ServerFull";
    case 32761: return "UserBlocked";
    case 32760: return "NoRandomMatchFound";
    case 32758: return "GameDoesNotExist";
    case 32757: return "MaxCcuReached";
    case 32756: return "InvalidRegion";
    case 32755: return "CustomAuthenticationFailed";
    case 32753: return "AuthenticationTicketExpired";
    case 32752: return "PluginReportedError";
    case 32751: return "PluginMismatch";
    case 32750: return "JoinFailedPeerAlreadyJoined";
    case 32749: return "JoinFailedFoundInactiveJoiner";
    case 32748: return "JoinFailedWithRejoinerNotFound";
    case 32747: return "JoinFailedFoundExcludedUserId";
    case 32746: return "JoinFailedFoundActiveJoiner";
    case 32745: return "HttpLimitReached";
    case 32744: return "ExternalHttpCallFailed";
    case 32743: return "OperationLimitReached";
    case 32742: return "SlotError";
    case 32741: return "InvalidEncryptionParameters";
    default:    return "?";
    }
}

// ExitGames.Client.Photon.StatusCode
static const char* PhotonStatusName(int status)
{
    switch (status) {
    case 1022: return "SecurityExceptionOnConnect";
    case 1023: return "ExceptionOnConnect";
    case 1024: return "Connect";
    case 1025: return "Disconnect";
    case 1026: return "Exception";
    case 1030: return "SendError";
    case 1039: return "ExceptionOnReceive";
    case 1040: return "TimeoutDisconnect";
    case 1041: return "DisconnectByServerTimeout";
    case 1042: return "DisconnectByServerUserLimit";
    case 1043: return "DisconnectByServerLogic";
    case 1044: return "DisconnectByServerReasonUnknown";
    case 1048: return "EncryptionEstablished";
    case 1049: return "EncryptionFailedToEstablish";
    case 1050: return "ServerAddressInvalid";
    case 1051: return "DnsExceptionOnConnect";
    default:   return "?";
    }
}

// Photon.Realtime.ServerConnection
static const char* PhotonServerName(int server)
{
    switch (server) {
    case 0:  return "MasterServer";
    case 1:  return "GameServer";
    case 2:  return "NameServer";
    default: return "server ?";
    }
}

// Operations whose failure explains a connection that never comes up.
// JoinRandomGame is left out on purpose: "no random match" is routine
// matchmaking, not a fault, and would just be noise.
static bool IsConnectOp(uint8_t op)
{
    return op == 220 || op == 226 || op == 227 || op == 229 || op == 230 || op == 231;
}

static bool IsAbnormalStatus(int status)
{
    switch (status) {
    case 1022: case 1023: case 1026: case 1030: case 1039: case 1040:
    case 1041: case 1042: case 1043: case 1044: case 1049: case 1050: case 1051:
        return true;
    default:
        return false;
    }
}

static void LogOpResponse(const char* tag, const char* peer, int server,
                          uint8_t op, int rc, const char* debugMessage)
{
    bool failed = (rc != 0);
    if (!g_PhotonVerbose && !(failed && IsConnectOp(op)))
        return;
    if (!failed) {
        LOG("%s response (%s peer, %s): op=%u %s -> Ok",
            tag, peer, PhotonServerName(server), op, PhotonOpName(op));
        return;
    }
    LOG("%s response (%s peer, %s): op=%u %s FAILED -> ReturnCode %d %s%s%s%s",
        tag, peer, PhotonServerName(server), op, PhotonOpName(op),
        rc, PhotonReturnCodeName(rc),
        (debugMessage && debugMessage[0]) ? ": \"" : "",
        (debugMessage && debugMessage[0]) ? debugMessage : "",
        (debugMessage && debugMessage[0]) ? "\"" : "");
}

static void LogStatusChange(const char* tag, const char* peer, int server, int status)
{
    bool abnormal = IsAbnormalStatus(status);
    if (!abnormal && !g_PhotonVerbose)
        return;
    LOG("%s status (%s peer, %s): %d %s%s", tag, peer, PhotonServerName(server),
        status, PhotonStatusName(status), abnormal ? "  <-- connection problem" : "");
}

// ------------------------------------------------------------
// IL2CPP response logger. Shared by the Realtime and Fusion modules: both
// run a LoadBalancingClient over the same ExitGames/Photon.Client layer, and a
// game only ever ships one of them, so the first module to activate installs
// it. IL2CPP methods take a trailing MethodInfo*, which is passed straight
// through.
// ------------------------------------------------------------
namespace RespLogIL2CPP {

typedef const char* (*Fn_PeerName)(void* peer);

static bool        g_Installed = false;
static const char* g_Tag       = "[Realtime]";
static Fn_PeerName g_PeerName  = nullptr;
static int g_OffRespOp = -1, g_OffRespRc = -1, g_OffRespMsg = -1;
static int g_OffClientServer = -1, g_OffClientPeer = -1;

static const char* ClientPeerName(void* client)
{
    if (!client || g_OffClientPeer < 0 || !g_PeerName) return "?";
    return g_PeerName(*(void**)((char*)client + g_OffClientPeer));
}
static int ClientServer(void* client)
{
    if (!client || g_OffClientServer < 0) return -1;
    return *(int*)((char*)client + g_OffClientServer);
}

typedef void (__fastcall *Fn_OnOpResponse)(void* pThis, void* resp, const void* method);
typedef void (__fastcall *Fn_OnStatusChanged)(void* pThis, int status, const void* method);
static Fn_OnOpResponse    g_pfnOrigOnOpResponse    = nullptr;
static Fn_OnStatusChanged g_pfnOrigOnStatusChanged = nullptr;

static void __fastcall Hooked_OnOpResponse(void* pThis, void* resp, const void* method)
{
    // Log before the game handles it: a failed auth usually disconnects inside
    // the original, and the line has to make it out first.
    if (resp && g_OffRespOp >= 0 && g_OffRespRc >= 0) {
        uint8_t op = *(uint8_t*)((char*)resp + g_OffRespOp);
        int     rc = *(int16_t*)((char*)resp + g_OffRespRc);
        char msg[512] = {};
        if (rc != 0 && g_OffRespMsg >= 0)
            IL2CPP_StringToUtf8(*(Il2CppObject**)((char*)resp + g_OffRespMsg), msg, sizeof(msg));
        LogOpResponse(g_Tag, ClientPeerName(pThis), ClientServer(pThis), op, rc, msg);
    }
    g_pfnOrigOnOpResponse(pThis, resp, method);
}

static void __fastcall Hooked_OnStatusChanged(void* pThis, int status, const void* method)
{
    LogStatusChange(g_Tag, ClientPeerName(pThis), ClientServer(pThis), status);
    g_pfnOrigOnStatusChanged(pThis, status, method);
}

static Il2CppClass* FindFirstClass(const char* image, const char* const* namespaces,
                                   const char* const* names)
{
    for (int n = 0; namespaces[n]; ++n)
        for (int c = 0; names[c]; ++c)
            if (Il2CppClass* k = IL2CPP_FindClass(image, namespaces[n], names[c]))
                return k;
    return nullptr;
}

static bool HookMethod(Il2CppClass* klass, const char* method, void* detour, void** original)
{
    const MethodInfo* mi = IL2CPP_FindMethod(klass, method, 1);
    void* fn = mi ? mi->methodPointer : nullptr;
    if (!fn) return false;
    if (MH_CreateHook(fn, detour, original) != MH_OK) return false;
    return MH_EnableHook(fn) == MH_OK;
}

// clientImage/clientNamespace locate LoadBalancingClient for this flavour.
static void Install(const char* tag, const char* clientImage, const char* clientNamespace,
                    Fn_PeerName peerName)
{
    if (g_Installed) return;
    g_Installed = true;
    g_Tag = tag;
    g_PeerName = peerName;

    // Realtime 4 lives in ExitGames.Client.Photon; Realtime 5 renamed it Photon.Client.
    static const char* const kRespNs[]     = { "ExitGames.Client.Photon", "Photon.Client", nullptr };
    static const char* const kRespName[]   = { "OperationResponse", nullptr };
    const char* const        kClientNs[]   = { clientNamespace, nullptr };
    static const char* const kClientName[] = { "LoadBalancingClient", "RealtimeClient", nullptr };

    Il2CppClass* resp   = FindFirstClass("Photon3Unity3D", kRespNs, kRespName);
    Il2CppClass* client = FindFirstClass(clientImage, kClientNs, kClientName);
    if (!resp || !client) {
        LOG("%s response logging unavailable (OperationResponse=%p LoadBalancingClient=%p)",
            tag, (void*)resp, (void*)client);
        return;
    }

    g_OffRespOp       = IL2CPP_GetFieldOffset(resp, "OperationCode");
    g_OffRespRc       = IL2CPP_GetFieldOffset(resp, "ReturnCode");
    g_OffRespMsg      = IL2CPP_GetFieldOffset(resp, "DebugMessage");
    g_OffClientServer = IL2CPP_GetFieldOffset(client, "<Server>k__BackingField");
    g_OffClientPeer   = IL2CPP_GetFieldOffset(client, "<LoadBalancingPeer>k__BackingField");

    bool onResp = (g_OffRespOp >= 0 && g_OffRespRc >= 0) &&
        HookMethod(client, "OnOperationResponse", (void*)&Hooked_OnOpResponse,
                   (void**)&g_pfnOrigOnOpResponse);
    bool onStatus = HookMethod(client, "OnStatusChanged", (void*)&Hooked_OnStatusChanged,
                               (void**)&g_pfnOrigOnStatusChanged);

    LOG("%s response logging: OnOperationResponse=%s OnStatusChanged=%s (%s)", tag,
        onResp ? "hooked" : "NOT hooked", onStatus ? "hooked" : "NOT hooked",
        g_PhotonVerbose ? "VerboseLog: every response" : "failures only; VerboseLog=1 for all");
}

} // namespace RespLogIL2CPP


// ============================================================
// Player display name -> Photon custom-auth params[216]
//
// Photon forwards ClientAuthenticationParams (param code 216) to
// the configured Custom Authentication URL as a query string. Our
// permissive Cloudflare Worker echoes ?name=... back as the auth
// Nickname, which PUN uses for the local player. Without this the
// Worker returns a constant "Player" for everyone.
//
// We source the name from the real Steam persona (UCOnline2 proxies
// ISteamFriends straight through to real Steam), or from an explicit
// [Realtime] Nickname= override. Built once at init; empty disables
// the injection so the Worker just keeps its old constant.
// ============================================================
static char g_LoginAuthParam[192] = {};   // e.g. "name=John%20Doe", or empty

static void BuildLoginAuthParam(ISteamFriends* pFriends, const char* iniOverride)
{
    char nameBuf[160] = {};
    const char* source = "";
    if (iniOverride && iniOverride[0]) {
        strncpy_s(nameBuf, sizeof(nameBuf), iniOverride, _TRUNCATE);
        source = "ini override";
    } else if (pFriends) {
        // GetPersonaName() is vtable index 0 on ISteamFriends.
        // x64 __thiscall == __fastcall (this in RCX).
        typedef const char* (__fastcall *Fn_GetPersonaName)(void*);
        void** vtbl = *(void***)pFriends;
        const char* n = (vtbl && vtbl[0]) ? ((Fn_GetPersonaName)vtbl[0])(pFriends) : nullptr;
        if (n && n[0]) { strncpy_s(nameBuf, sizeof(nameBuf), n, _TRUNCATE); source = "Steam persona"; }
    }
    if (!nameBuf[0]) {
        LOG("[Universal] no Steam persona name / Nickname override; params[216] passthrough disabled");
        return;
    }

    // URL-encode the name (RFC 3986 unreserved set stays literal).
    char enc[176]; size_t o = 0;
    for (const unsigned char* p = (const unsigned char*)nameBuf; *p && o + 3 < sizeof(enc); ++p) {
        unsigned char c = *p;
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
            enc[o++] = (char)c;
        } else {
            static const char* hex = "0123456789ABCDEF";
            enc[o++] = '%'; enc[o++] = hex[c >> 4]; enc[o++] = hex[c & 0xF];
        }
    }
    enc[o] = 0;
    _snprintf_s(g_LoginAuthParam, sizeof(g_LoginAuthParam), _TRUNCATE, "name=%s", enc);
    LOG("[Universal] Photon Nickname from %s '%s' -> params[216]=%s", source, nameBuf, g_LoginAuthParam);
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
        // Only the Authenticate ops carry custom-auth params to the Worker.
        if ((op == 230 || op == 231) && g_LoginAuthParam[0]) {
            if (IL2CPP_DictByteStringSetItem((Il2CppObject*)params, 216, g_LoginAuthParam))
                LOG("[Realtime] SendOp op=%u: params[216] auth-params -> %s", op, g_LoginAuthParam);
        }
    }
    return g_pfnOrigSendOp(pThis, op, params, opts, a5, a6);
}

// Name an already-classified peer without classifying a new one (used by the
// response logger: a response must never decide which peer is Realtime/Voice).
static const char* LookupPeerName(void* peer)
{
    if (!peer || !g_PeerCsInit) return "?";
    const char* name = "?";
    EnterCriticalSection(&g_PeerCs);
    for (int i = 0; i < g_PeerCount; ++i)
        if (g_Peers[i].pThis == peer) { name = g_Peers[i].product == 1 ? "Voice" : "Realtime"; break; }
    LeaveCriticalSection(&g_PeerCs);
    return name;
}

static bool InstallHook(void* target, void* detour, void** original,
                        const char* label)
{
    if (!target) {
        LOG("[Realtime] %s method not found", label);
        return false;
    }

    MH_STATUS status = MH_CreateHook(target, detour, original);
    if (status != MH_OK) {
        LOG("[Realtime] %s hook create failed: %s", label,
            MH_StatusToString(status));
        return false;
    }

    status = MH_EnableHook(target);
    if (status != MH_OK) {
        LOG("[Realtime] %s hook enable failed: %s", label,
            MH_StatusToString(status));
        MH_RemoveHook(target);
        return false;
    }

    LOG("[Realtime] %s hook @ %p", label, target);
    return true;
}

static bool TryInstall()
{
    if (!IL2CPP_IsReady()) return false;
    // Current PUN builds use PhotonRealtime.dll. The runtime helper still
    // falls back across all loaded images, which keeps older assembly naming
    // compatible, but asking for the real image first avoids needless misses.
    if (!IL2CPP_FindClass("PhotonRealtime", "Photon.Realtime", "LoadBalancingPeer")) {
        static bool loggedMissingPeer = false;
        if (!loggedMissingPeer) {
            LOG("[Realtime] Photon.Realtime.LoadBalancingPeer not available");
            loggedMissingPeer = true;
        }
        return false;
    }

    if (!g_PeerCsInit) { InitializeCriticalSection(&g_PeerCs); g_PeerCsInit = true; }

    int installed = 0;
    void* fn = IL2CPP_FindMethodPtr("PhotonRealtime", "Photon.Realtime",
        "AuthenticationValues", "set_AuthType", 1);
    if (InstallHook(fn, (void*)&Hooked_SetAuthType,
        (void**)&g_pfnOrigSetAuthType, "set_AuthType")) ++installed;

    fn = IL2CPP_FindMethodPtr("PhotonRealtime", "Photon.Realtime",
        "LoadBalancingPeer", "OpAuthenticate", -1);
    if (InstallHook(fn, (void*)&Hooked_OpAuth,
        (void**)&g_pfnOrigOpAuth, "OpAuthenticate")) ++installed;

    fn = IL2CPP_FindMethodPtr("PhotonRealtime", "Photon.Realtime",
        "LoadBalancingPeer", "OpAuthenticateOnce", -1);
    if (InstallHook(fn, (void*)&Hooked_OpAuthOnce,
        (void**)&g_pfnOrigOpAuthOnce, "OpAuthenticateOnce")) ++installed;

    fn = IL2CPP_FindMethodPtr("Photon3Unity3D", "ExitGames.Client.Photon", "PhotonPeer", "SendOperation", -1);
    if (!fn)
        fn = IL2CPP_FindMethodPtr("Photon3Unity3D", "ExitGames.Client.Photon", "PeerBase", "SendOperation", -1);
    if (InstallHook(fn, (void*)&Hooked_SendOp,
        (void**)&g_pfnOrigSendOp, "SendOperation")) ++installed;

    if (installed == 0) return false;
    RespLogIL2CPP::Install("[Realtime]", "PhotonRealtime", "Photon.Realtime", &LookupPeerName);
    LOG("[Realtime] IL2CPP module active (%d hooks)", installed);
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

// Real offset of AuthenticationValues.authType, resolved from metadata in
// TryInstall (-1 until known). Replaces the old hardcoded 0x10 guess, which
// on Mono's field layout landed on a reference field and crashed the GC.
static int g_MonoAuthTypeOffset = -1;
static void PatchAuthType(void* authValues, const char* sender)
{
    if (!authValues || g_MonoAuthTypeOffset < 0) return;   // never write at a guessed offset
    unsigned char* p = (unsigned char*)authValues + g_MonoAuthTypeOffset;
    unsigned char prev = *p;
    *p = (unsigned char)(g_ForcedAuthType & 0xFF);
    LOG("[Realtime/Mono] %s: authValues.authType(@0x%x) %u -> %u", sender, g_MonoAuthTypeOffset, prev, g_ForcedAuthType);
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
        // Only the Authenticate ops carry custom-auth params to the Worker.
        if ((op == 230 || op == 231) && g_LoginAuthParam[0]) {
            if (MONO_DictByteStringSetItem((MonoObject*)params, 216, g_LoginAuthParam))
                LOG("[Realtime/Mono] SendOp op=%u: params[216] auth-params -> %s", op, g_LoginAuthParam);
        }
    }
    return g_pfnOrigSendOp(pThis, op, params, opts, a5, a6);
}

// --- inbound: responses + status changes (see "Photon response logging") ---

// Name an already-classified peer without classifying a new one: a response
// must never be the thing that decides which peer is Realtime and which Voice.
static const char* LookupPeerName(void* peer)
{
    if (!peer || !g_PeerCsInit) return "?";
    const char* name = "?";
    EnterCriticalSection(&g_PeerCs);
    for (int i = 0; i < g_PeerCount; ++i)
        if (g_Peers[i].pThis == peer) { name = g_Peers[i].product == 1 ? "Voice" : "Realtime"; break; }
    LeaveCriticalSection(&g_PeerCs);
    return name;
}

static int g_OffRespOp = -1, g_OffRespRc = -1, g_OffRespMsg = -1;
static int g_OffClientServer = -1, g_OffClientPeer = -1;

static const char* ClientPeerName(void* client)
{
    if (!client || g_OffClientPeer < 0) return "?";
    return LookupPeerName(*(void**)((char*)client + g_OffClientPeer));
}
static int ClientServer(void* client)
{
    if (!client || g_OffClientServer < 0) return -1;
    return *(int*)((char*)client + g_OffClientServer);
}

typedef void (__fastcall *Fn_OnOpResponse)(void* pThis, void* resp);
typedef void (__fastcall *Fn_OnStatusChanged)(void* pThis, int status);
static Fn_OnOpResponse    g_pfnOrigOnOpResponse    = nullptr;
static Fn_OnStatusChanged g_pfnOrigOnStatusChanged = nullptr;

static void __fastcall Hooked_OnOpResponse(void* pThis, void* resp)
{
    // Log before the game handles it: a failed auth usually disconnects inside
    // the original, and the line has to make it out first.
    if (resp && g_OffRespOp >= 0 && g_OffRespRc >= 0) {
        uint8_t op = *(uint8_t*)((char*)resp + g_OffRespOp);
        int     rc = *(int16_t*)((char*)resp + g_OffRespRc);
        char msg[512] = {};
        if (rc != 0 && g_OffRespMsg >= 0)
            MONO_StringToUtf8(*(MonoObject**)((char*)resp + g_OffRespMsg), msg, sizeof(msg));
        LogOpResponse("[Realtime/Mono]", ClientPeerName(pThis), ClientServer(pThis), op, rc, msg);
    }
    g_pfnOrigOnOpResponse(pThis, resp);
}

static void __fastcall Hooked_OnStatusChanged(void* pThis, int status)
{
    LogStatusChange("[Realtime/Mono]", ClientPeerName(pThis), ClientServer(pThis), status);
    g_pfnOrigOnStatusChanged(pThis, status);
}

static void InstallResponseLogging()
{
    // Realtime 4 lives in ExitGames.Client.Photon; Realtime 5 renamed it Photon.Client.
    MonoClass* resp = MONO_FindClass("Photon3Unity3D", "ExitGames.Client.Photon", "OperationResponse");
    if (!resp) resp = MONO_FindClass(nullptr, "Photon.Client", "OperationResponse");
    MonoClass* client = MONO_FindClass("Photon.Realtime", "Photon.Realtime", "LoadBalancingClient");
    if (!client) client = MONO_FindClass("Photon.Realtime", "Photon.Realtime", "RealtimeClient");
    if (!resp || !client) {
        LOG("[Realtime/Mono] response logging unavailable (OperationResponse=%p LoadBalancingClient=%p)",
            (void*)resp, (void*)client);
        return;
    }

    g_OffRespOp       = MONO_GetFieldOffset(resp, "OperationCode");
    g_OffRespRc       = MONO_GetFieldOffset(resp, "ReturnCode");
    g_OffRespMsg      = MONO_GetFieldOffset(resp, "DebugMessage");
    g_OffClientServer = MONO_GetFieldOffset(client, "<Server>k__BackingField");
    g_OffClientPeer   = MONO_GetFieldOffset(client, "<LoadBalancingPeer>k__BackingField");

    bool onResp = false, onStatus = false;
    void* fn = (g_OffRespOp >= 0 && g_OffRespRc >= 0)
        ? MONO_GetMethodNativePtr(MONO_FindMethod(client, "OnOperationResponse", 1)) : nullptr;
    if (fn && MH_CreateHook(fn, (void*)&Hooked_OnOpResponse, (void**)&g_pfnOrigOnOpResponse) == MH_OK)
        onResp = (MH_EnableHook(fn) == MH_OK);
    fn = MONO_GetMethodNativePtr(MONO_FindMethod(client, "OnStatusChanged", 1));
    if (fn && MH_CreateHook(fn, (void*)&Hooked_OnStatusChanged, (void**)&g_pfnOrigOnStatusChanged) == MH_OK)
        onStatus = (MH_EnableHook(fn) == MH_OK);

    LOG("[Realtime/Mono] response logging: OnOperationResponse=%s OnStatusChanged=%s (%s)",
        onResp ? "hooked" : "NOT hooked", onStatus ? "hooked" : "NOT hooked",
        g_PhotonVerbose ? "VerboseLog: every response" : "failures only; VerboseLog=1 for all");
}

static bool TryInstall()
{
    if (!MONO_IsReady()) return false;
    if (!MONO_FindClass("Photon.Realtime", "Photon.Realtime", "LoadBalancingPeer")) return false;

    // Resolve the REAL offset of AuthenticationValues.authType rather than
    // guessing 0x10: Mono's auto field layout often puts the property backing
    // fields (references) before the enum, so a raw write at 0x10 would smash a
    // string pointer and crash the GC (mono_validate_string_pointer). If we
    // can't resolve it we simply skip the object-field force -- params[217] on
    // the wire and the set_AuthType hook already force Custom auth.
    if (g_MonoAuthTypeOffset < 0)
    {
        MonoClass* avc = MONO_FindClass("Photon.Realtime", "Photon.Realtime", "AuthenticationValues");
        if (avc) g_MonoAuthTypeOffset = MONO_GetFieldOffset(avc, "authType");
        LOG("[Realtime/Mono] AuthenticationValues.authType field offset = 0x%x", g_MonoAuthTypeOffset);
    }

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
    InstallResponseLogging();
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

// Fusion runs a single client, so there is nothing to tell apart.
static const char* FusionPeerName(void*) { return "Fusion"; }

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
    RespLogIL2CPP::Install("[Fusion]", "Fusion.Realtime", "Fusion.Photon.Realtime", &FusionPeerName);
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
// ORCHESTRATOR
// ============================================================
static volatile LONG g_RealtimeIL2CPP_Done = 0;
static volatile LONG g_RealtimeMono_Done   = 0;
static volatile LONG g_Fusion_Done         = 0;

static void RunDetectionPass()
{

    // IL2CPP-based modules
    if (IL2CPP_TryInit()) {
        if (!InterlockedCompareExchange(&g_RealtimeIL2CPP_Done, 0, 0))
            if (ModRealtimeIL2CPP::TryInstall()) InterlockedExchange(&g_RealtimeIL2CPP_Done, 1);
        if (!InterlockedCompareExchange(&g_Fusion_Done, 0, 0))
            if (ModFusion::TryInstall())         InterlockedExchange(&g_Fusion_Done, 1);
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
    char nickOverride[160] = {};
    if (ini) {
        // VerboseLog=1 under whichever section this game's flavour uses: log every
        // Photon response and status change, not just the failures.
        char verbose[8] = {};
        GetPrivateProfileStringA("Realtime", "VerboseLog", "", verbose, sizeof(verbose), ini);
        if (!verbose[0]) GetPrivateProfileStringA("PUN", "VerboseLog", "", verbose, sizeof(verbose), ini);
        if (!verbose[0]) GetPrivateProfileStringA("Fusion", "VerboseLog", "", verbose, sizeof(verbose), ini);
        g_PhotonVerbose = !strcmp(verbose, "1") || !_stricmp(verbose, "true") ||
                          !_stricmp(verbose, "yes") || !_stricmp(verbose, "on");
        ModRealtimeIL2CPP::ReadIni(ini);
        ModRealtimeMono::ReadIni(ini);
        ModFusion::ReadIni(ini);
        GetPrivateProfileStringA("Realtime", "Nickname", "", nickOverride, sizeof(nickOverride), ini);
        if (!nickOverride[0]) GetPrivateProfileStringA("PUN", "Nickname", "", nickOverride, sizeof(nickOverride), ini);
    } else {
        LOG("[Universal] no union-crax.ini found");
    }

    // Feed the player's display name into Photon custom-auth params[216].
    // Default: real Steam persona (proxied through). Override: [Realtime] Nickname=.
    BuildLoginAuthParam(ctx->pSteamFriends, nickOverride);

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
