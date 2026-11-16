// ============================================================
// UCOnline2 plugin -- EOS_custom
//
// Lets an EOS game authenticate against Epic's REAL backend using
// YOUR own free Epic app, via an anonymous EOS Connect Device ID
// login. Co-op then runs on Epic's own relay -- no LAN, no VPN and
// no EOS emulator.
//
// THE PROBLEM
//   These games log into EOS with a Steam session ticket. Epic
//   validates that ticket server-side against Steam, which always
//   fails for an emulated ticket. From the game's own UE log:
//     External credential 'STEAM_SESSION_TICKET' failed to
//     authenticate with EOS Connect:
//     EOS_Connect_ExternalTokenValidationFailed
//   With no EOS identity there is no ProductUserId, so the session
//   layer refuses to host:
//     HostingPlayerNum provided to CreateSession does not have
//     online identity.
//   No session is created, so nothing is advertised to Steam: the
//   "Join Game" button never appears and joiners find nothing.
//
// THE FIX -- two hooks on the genuine Epic EOSSDK-Win64-Shipping.dll
//   1. EOS_Platform_Create: rewrite ProductId / SandboxId /
//      DeploymentId / ClientId / ClientSecret to OUR Epic app so
//      every player lands in the same session pool.
//   2. EOS_Connect_Login: replace the doomed STEAM_SESSION_TICKET
//      credential (type 18) with DEVICEID_ACCESS_TOKEN (type 10).
//      Device ID login is anonymous -- there is no external platform
//      for Epic to validate -- so it succeeds and yields a real
//      ProductUserId. It also REQUIRES UserLoginInfo.DisplayName.
//   Device ID login needs a device id to exist first, so we call
//   EOS_Connect_CreateDeviceId once on the first attempt and let the
//   game's own login retry loop succeed on a later pass.
//
// The EOS SDK can load from a subdir (Forever Skies:
// ProjectZeppelin\Binaries\Win64\RedpointEOS\EOSSDK-Win64-Shipping.dll)
// and typically loads BEFORE UCO_PluginInit runs, so the watcher
// thread starts from DllMain, waits for the module, resolves the
// exports and installs the hooks. Until the host hands us its
// logger, a fallback writes straight to %TEMP%\uc_online2.log.
//
// The EOS struct layouts below are trimmed to the fields we touch and
// pinned to the EOS SDK 1.15.x ABI (Forever Skies ships 1.15.5). Only
// leading fields are used, which are stable across minor versions.
//
// MinHook statically linked (same as the other UCOnline2 plugins).
// ============================================================
#include <Windows.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "shlwapi.lib")

#include "../../include/MinHook.h"
#include "../../include/uco_plugin.h"
#include "eos_baked_creds.h"   // default Epic app creds (baked at release; empty in source)

// ------------------------------------------------------------
// Minimal EOS types (SDK 1.15.x). Only the fields we read or rewrite.
// ------------------------------------------------------------
typedef void* EOS_HPlatform;
typedef void* EOS_HConnect;
typedef int32_t EOS_EResult;

// EOS_ELoginCredentialType / EOS_EExternalCredentialType values we
// might see (from eos_connect_types.h / eos_common.h, 1.15.x):
//   EOS_ECT_EPIC                       = 0
//   EOS_ECT_STEAM_APP_TICKET           = 1
//   EOS_ECT_PSN_ID_TOKEN               = 2
//   EOS_ECT_XBL_XSTS_TOKEN             = 3
//   EOS_ECT_DISCORD_ACCESS_TOKEN       = 4
//   EOS_ECT_GOG_SESSION_TICKET         = 5
//   EOS_ECT_NINTENDO_ID_TOKEN          = 6
//   EOS_ECT_NINTENDO_NSA_ID_TOKEN      = 7
//   EOS_ECT_UPLAY_ACCESS_TOKEN         = 8
//   EOS_ECT_OPENID_ACCESS_TOKEN        = 9
//   EOS_ECT_DEVICEID_ACCESS_TOKEN      = 10
//   EOS_ECT_APPLE_ID_TOKEN             = 11
//   EOS_ECT_GOOGLE_ID_TOKEN            = 12
//   EOS_ECT_OCULUS_USERID_NONCE        = 13
//   EOS_ECT_ITCHIO_JWT                 = 14
//   EOS_ECT_ITCHIO_KEY                 = 15
//   EOS_ECT_EPIC_ID_TOKEN              = 16
//   EOS_ECT_AMAZON_ACCESS_TOKEN        = 17
//   EOS_ECT_STEAM_SESSION_TICKET       = 18
static const char* ExternalCredTypeName(int t)
{
    switch (t) {
        case 0:  return "EPIC";
        case 1:  return "STEAM_APP_TICKET";
        case 2:  return "PSN_ID_TOKEN";
        case 3:  return "XBL_XSTS_TOKEN";
        case 4:  return "DISCORD_ACCESS_TOKEN";
        case 5:  return "GOG_SESSION_TICKET";
        case 6:  return "NINTENDO_ID_TOKEN";
        case 7:  return "NINTENDO_NSA_ID_TOKEN";
        case 8:  return "UPLAY_ACCESS_TOKEN";
        case 9:  return "OPENID_ACCESS_TOKEN";
        case 10: return "DEVICEID_ACCESS_TOKEN";
        case 11: return "APPLE_ID_TOKEN";
        case 12: return "GOOGLE_ID_TOKEN";
        case 13: return "OCULUS_USERID_NONCE";
        case 14: return "ITCHIO_JWT";
        case 15: return "ITCHIO_KEY";
        case 16: return "EPIC_ID_TOKEN";
        case 17: return "AMAZON_ACCESS_TOKEN";
        case 18: return "STEAM_SESSION_TICKET";
        default: return "UNKNOWN";
    }
}

// EOS_Connect_Credentials (eos_connect_types.h, ApiVersion 1)
//   int32 ApiVersion;
//   const char* Token;
//   EOS_EExternalCredentialType Type;
struct EOS_Connect_Credentials {
    int32_t     ApiVersion;
    const char* Token;
    int32_t     Type;
};

// EOS_Connect_UserLoginInfo (optional; used for DeviceID display name).
// Declared at API version 2 -- the layout as of SDK 1.19.1, which appended
// NsaIdToken. We BUILD this struct rather than only reading it, so it must be
// at least as large as the version we advertise in ApiVersion: the SDK reads
// exactly the fields that version promises. Declaring v1 while echoing a game's
// ApiVersion=2 would have the SDK read NsaIdToken past the end of the object
// and dereference whatever followed.
#define EOSC_USERLOGININFO_MAX_SUPPORTED 2
struct EOS_Connect_UserLoginInfo {
    int32_t     ApiVersion;
    const char* DisplayName;
    const char* NsaIdToken;      // v2+; always null for us (Nintendo only)
};

// EOS_Connect_LoginOptions
//   int32 ApiVersion;
//   const EOS_Connect_Credentials* Credentials;
//   const EOS_Connect_UserLoginInfo* UserLoginInfo;
struct EOS_Connect_LoginOptions {
    int32_t                            ApiVersion;
    const EOS_Connect_Credentials*     Credentials;
    const EOS_Connect_UserLoginInfo*   UserLoginInfo;
};

// EOS_Platform_Options -- we only read the leading id string fields.
// Layout (1.15.x, x64), offsets validated by ApiVersion sanity check:
//   int32 ApiVersion;            (+0)
//   [4 pad]                      (+4)
//   void* Reserved;              (+8)
//   const char* ProductId;       (+16)
//   const char* SandboxId;       (+24)
//   EOS_Platform_ClientCredentials ClientCredentials; { const char* ClientId; const char* ClientSecret; }  (+32, +40)
//   EOS_Bool bIsServer;          (+48)
//   const char* EncryptionKey;   (+56)
//   const char* OverrideCountryCode; (+64)
//   const char* OverrideLocaleCode;  (+72)
//   const char* DeploymentId;    (+80)
// We read these by offset defensively (only if ApiVersion looks sane).
struct EOS_Platform_ClientCredentials {
    const char* ClientId;
    const char* ClientSecret;
};

// ------------------------------------------------------------
// Config: our own Epic app, from union-crax.ini [EOS].
// These replace whatever the game was built with, so both players
// end up on OUR deployment instead of the publisher's.
// ------------------------------------------------------------
struct EosConfig
{
    char ProductId[64];
    char SandboxId[64];
    char DeploymentId[64];
    char ClientId[128];
    char ClientSecret[256];
    char DisplayName[64];   // required by Device ID login
    bool bKeepGameApp;      // [EOS] KeepGameApp: Device ID login on the game's OWN
                            // Epic app, no redirect. WINS even if the app ids below
                            // are filled in.
    bool bVerboseLog;       // [EOS] VerboseLog: route the EOS SDK's full verbose log
                            // into the host log. Off by default (warnings+errors only).
    bool bNoPresence;       // [EOS] NoPresence: for games whose co-op rides PRESENCE /
                            // integrated-platform lobbies, which an anonymous Device ID
                            // (Connect-only) login can't satisfy. Two effects:
                            //   1. force bPresenceEnabled=false on every lobby create/join
                            //   2. NULL the platform's IntegratedPlatformOptionsContainer
                            //      (the SDK's documented off switch) so the Steam<->EOS
                            //      identity validation doesn't reject the anon user.
    bool bUsingBaked;       // true when the redirect creds came from the baked-in
                            // default Epic app rather than the user's own ini block.
    bool bValid;
};
static EosConfig g_Cfg = {};

static void LoadEosConfig()
{
    char ini[MAX_PATH] = {};
    DWORD len = GetModuleFileNameA(nullptr, ini, MAX_PATH);
    if (!len) return;
    for (int i = (int)len - 1; i >= 0; --i)
        if (ini[i] == '\\' || ini[i] == '/') { ini[i] = 0; break; }
    strncat_s(ini, MAX_PATH, "\\union-crax.ini", _TRUNCATE);

    GetPrivateProfileStringA("EOS", "ProductId",    "", g_Cfg.ProductId,    sizeof(g_Cfg.ProductId),    ini);
    GetPrivateProfileStringA("EOS", "SandboxId",    "", g_Cfg.SandboxId,    sizeof(g_Cfg.SandboxId),    ini);
    GetPrivateProfileStringA("EOS", "DeploymentId", "", g_Cfg.DeploymentId, sizeof(g_Cfg.DeploymentId), ini);
    GetPrivateProfileStringA("EOS", "ClientId",     "", g_Cfg.ClientId,     sizeof(g_Cfg.ClientId),     ini);
    GetPrivateProfileStringA("EOS", "ClientSecret", "", g_Cfg.ClientSecret, sizeof(g_Cfg.ClientSecret), ini);
    GetPrivateProfileStringA("EOS", "DisplayName",  "Player", g_Cfg.DisplayName, sizeof(g_Cfg.DisplayName), ini);

    // [EOS] KeepGameApp=1 -> Device ID login on the game's OWN Epic app, no
    // redirect. Deliberately checked BEFORE (and independent of) the app ids, so
    // it wins even when they are filled in.
    g_Cfg.bKeepGameApp = GetPrivateProfileIntA("EOS", "KeepGameApp", 0, ini) != 0;

    // [EOS] VerboseLog=1 -> full EOS SDK verbose log. Off by default; we still
    // surface warnings+errors so real failures are visible without the spam.
    g_Cfg.bVerboseLog  = GetPrivateProfileIntA("EOS", "VerboseLog", 0, ini) != 0;

    // [EOS] NoPresence=1 -> force non-presence lobbies (see field comment).
    g_Cfg.bNoPresence  = GetPrivateProfileIntA("EOS", "NoPresence", 0, ini) != 0;

    // Own-app override vs baked default. If the user filled a COMPLETE [EOS] redirect
    // block, use it (bring-your-own Epic app). Otherwise fall back to the creds baked
    // into this DLL at release time (the project's default Epic app) so the redirect
    // works with no Epic app of your own. All-or-nothing: a partial ini block does NOT
    // get mixed with baked creds -- we take the full baked set instead.
    bool iniComplete = g_Cfg.ProductId[0] && g_Cfg.SandboxId[0] && g_Cfg.DeploymentId[0] &&
                       g_Cfg.ClientId[0] && g_Cfg.ClientSecret[0];
    if (!iniComplete && UCO_EOS_BAKED_PRODUCTID[0] && UCO_EOS_BAKED_CLIENTSECRET[0]) {
        strncpy_s(g_Cfg.ProductId,    UCO_EOS_BAKED_PRODUCTID,    _TRUNCATE);
        strncpy_s(g_Cfg.SandboxId,    UCO_EOS_BAKED_SANDBOXID,    _TRUNCATE);
        strncpy_s(g_Cfg.DeploymentId, UCO_EOS_BAKED_DEPLOYMENTID, _TRUNCATE);
        strncpy_s(g_Cfg.ClientId,     UCO_EOS_BAKED_CLIENTID,     _TRUNCATE);
        strncpy_s(g_Cfg.ClientSecret, UCO_EOS_BAKED_CLIENTSECRET, _TRUNCATE);
        g_Cfg.bUsingBaked = true;
    }

    g_Cfg.bValid = g_Cfg.ProductId[0] && g_Cfg.SandboxId[0] && g_Cfg.DeploymentId[0] &&
                   g_Cfg.ClientId[0] && g_Cfg.ClientSecret[0];
}

// ------------------------------------------------------------
// Plugin state
// ------------------------------------------------------------
static UCO_LogFn g_Log = nullptr;

// Fallback logger: the EOS calls we care about happen during game
// startup, BEFORE the host hands us ctx->Log. Until then we append
// to %TEMP%\uc_online2.log ourselves so nothing is lost.
static void FallbackLog(const char* fmt, va_list ap)
{
    char path[MAX_PATH];
    if (!GetTempPathA(MAX_PATH, path)) return;
    strncat_s(path, MAX_PATH, "uc_online2.log", _TRUNCATE);

    FILE* f = nullptr;
    if (fopen_s(&f, path, "a") != 0 || !f) return;

    SYSTEMTIME st; GetLocalTime(&st);
    fprintf(f, "[%04d-%02d-%02d %02d:%02d:%02d.%03d] ",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    vfprintf(f, fmt, ap);
    fputs("\n", f);
    fclose(f);
}

static void EosLog(const char* fmt, ...)
{
    va_list ap; va_start(ap, fmt);
    if (g_Log) {
        char buf[1024];
        _vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, ap);
        g_Log("%s", buf);
    } else {
        FallbackLog(fmt, ap);
    }
    va_end(ap);
}

#define LOG(...) EosLog(__VA_ARGS__)

static HANDLE        g_hWatcher   = nullptr;
static volatile LONG g_bShutdown  = 0;

// Set once EOS_Platform_Create has actually been redirected to our Epic app.
// The login rewrite is gated on this: swapping in a Device ID login while the
// platform still points at the PUBLISHER'S deployment would just create an
// anonymous account on their backend, which is both useless and rude.
static volatile LONG g_bPlatformRedirected = 0;

// Whether Connect_Login should swap in a Device ID login. Set when we redirect
// to our own app (the classic path) OR when [EOS] KeepGameApp deliberately keeps
// the game's own app and only anonymises the login. Connect_Login gates on THIS.
static volatile LONG g_bDoDeviceIdLogin = 0;

typedef EOS_HPlatform (EOS_CALL_PLACEHOLDER)(void);
typedef EOS_HPlatform (__cdecl* Fn_EOS_Platform_Create)(const void* Options);
typedef void          (__cdecl* Fn_EOS_Connect_Login)(EOS_HConnect Handle, const void* Options,
                                                      void* ClientData, void* CompletionDelegate);

static Fn_EOS_Platform_Create g_orig_Platform_Create = nullptr;
static Fn_EOS_Connect_Login   g_orig_Connect_Login   = nullptr;

// ------------------------------------------------------------
// EOS SDK logging bridge.
//
// Shipping UE builds compile out UE_LOG and print nothing to stdout, so when a
// game stalls mid-login we are blind to the EOS side: did EOS_Platform_Create
// return a valid handle? did the game query Connect? what EResult came back?
// Routing the SDK's OWN log (EOS_Logging_SetCallback) into the host log answers
// all of that, and is reusable for every EOS title.
//
// EOS_Logging_SetCallback must run AFTER EOS_Initialize, so we arm it once from
// inside Hooked_Platform_Create (by then Initialize has run) rather than at
// hook-install time.
struct EOS_LogMessage { const char* Category; const char* Message; int32_t Level; };
typedef void        (__cdecl* EOS_LogMessageFunc)(const EOS_LogMessage*);
typedef EOS_EResult (__cdecl* Fn_EOS_Logging_SetCallback)(EOS_LogMessageFunc);
typedef EOS_EResult (__cdecl* Fn_EOS_Logging_SetLogLevel)(int32_t Category, int32_t Level);
static Fn_EOS_Logging_SetCallback g_pfn_SetLogCallback = nullptr;
static Fn_EOS_Logging_SetLogLevel g_pfn_SetLogLevel    = nullptr;

static const char* EosLogLevelName(int32_t lvl)
{
    switch (lvl) {
        case 100: return "Fatal";   case 200: return "Error";
        case 300: return "Warning"; case 400: return "Info";
        case 500: return "Verbose"; case 600: return "VeryVerbose";
        default:  return "?";
    }
}

// EosLog (defined above) uses the host logger, which is file-backed and safe to
// call from the SDK's logging thread(s).
static void __cdecl UcoEosLogSink(const EOS_LogMessage* m)
{
    if (!m) return;
    EosLog("[EOS-SDK][%s][%s] %s",
        m->Category ? m->Category : "?",
        EosLogLevelName(m->Level),
        m->Message ? m->Message : "");
}

static const char* SafeStr(const char* s) { return s ? s : "(null)"; }

// ------------------------------------------------------------
// Layout validation.
//
// We locate the id fields in EOS_Platform_Options by fixed offsets,
// which only holds for the genuine Epic SDK. If someone has an EOS
// EMULATOR dll in place instead (Nemirtingas et al), its options
// struct is laid out completely differently and those offsets read
// garbage -- previously faulting with EXCEPTION_ACCESS_VIOLATION on
// an address like 0x0000000100000001 (two adjacent 32-bit fields
// read as one pointer). The same would happen if a future SDK ever
// reordered the struct.
//
// So: prove the layout before trusting it. A real EOS id is a
// readable 32-char lowercase hex string, which is a strong enough
// fingerprint that a wrong layout will essentially never pass.
// If validation fails we skip the redirect and say why, instead of
// crashing the game.
// ------------------------------------------------------------
static bool IsReadable(const void* p, size_t bytes)
{
    if (!p) return false;
    MEMORY_BASIC_INFORMATION mbi = {};
    if (!VirtualQuery(p, &mbi, sizeof(mbi))) return false;
    if (mbi.State != MEM_COMMIT) return false;
    if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return false;
    // Ensure the whole span sits inside this region.
    const uint8_t* end   = (const uint8_t*)p + bytes;
    const uint8_t* limit = (const uint8_t*)mbi.BaseAddress + mbi.RegionSize;
    return end <= limit;
}

// True if `s` points at a NUL-terminated 32-char hex string (an EOS id).
static bool LooksLikeEosId(const char* s)
{
    if (!IsReadable(s, 33)) return false;
    for (int i = 0; i < 32; ++i) {
        char c = s[i];
        bool hex = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
        if (!hex) return false;
    }
    return s[32] == '\0';
}

// Diagnostic: scan the options block for pointers to EOS ids, so we can see the
// REAL layout when the fixed offsets do not match (a newer SDK, an EAC platform,
// or a wrapper). Logs the first few calls only.
static void DumpEosOptions(const uint8_t* p)
{
    static volatile LONG s_dumped = 0;
    if (InterlockedIncrement(&s_dumped) > 4) return;
    LOG("[EOSAuth] Platform_Create options scan (ApiVersion=%d):",
        IsReadable(p, 4) ? *(const int32_t*)p : -1);
    int found = 0;
    for (int off = 8; off <= 128; off += 8) {
        const char** pp = (const char**)(p + off);
        if (!IsReadable(pp, sizeof(void*))) continue;
        if (LooksLikeEosId(*pp)) { LOG("[EOSAuth]   +0x%02X -> EOS id %s", off, *pp); ++found; }
    }
    if (!found)
        LOG("[EOSAuth]   no EOS ids found in the options -- likely a null/anti-cheat platform.");
}

// ------------------------------------------------------------
// [EOS] NoPresence, part 2 -- disable the EOS Integrated Platform.
//
// Presence-off on the lobby (part 1) is not enough for a Steam<->EOS integrated
// game (StarRupture): even a non-presence CreateLobby still consults the
// Integrated Platform's identity, which validates that the local user is a real
// linked platform (Steam) user. Our anonymous Device ID PUID has no linked Steam
// account, so:
//   LogEOSIntegratedPlatform  ValidateUserPlatformLoginStatus() Specified local user ... is invalid.
//   LogEOSLobby               Cannot create Lobby, user permissions do not allow it.
//
// EOS_Platform_Options documents the fix directly: setting
// IntegratedPlatformOptionsContainerHandle to NULL disables the integrated
// platform for the host, which removes that identity validation. We NULL it in
// place before EOS_Platform_Create runs.
//
// Offset (x64): the handle sits at +120 in EOS_Platform_Options and is stable for
// platform-options ApiVersion >= 12 (the version that introduced the field); the
// preceding fields are unchanged since then. We only touch it once the id fields
// at +16/+80 prove this is the genuine Epic layout.
//   ApiVersion(0) Reserved(8) ProductId(16) SandboxId(24) ClientCredentials(32,40)
//   bIsServer(48) EncryptionKey(56) OverrideCountryCode(64) OverrideLocaleCode(72)
//   DeploymentId(80) Flags(88) CacheDirectory(96) TickBudget(104) RTCOptions(112)
//   IntegratedPlatformOptionsContainerHandle(120)
// ------------------------------------------------------------
static const size_t EOS_PLATFORM_IPCONTAINER_OFFSET = 120;

static void StripIntegratedPlatform(const void* Options)
{
    if (!Options) return;
    uint8_t* p = (uint8_t*)Options;
    if (!IsReadable(p, EOS_PLATFORM_IPCONTAINER_OFFSET + sizeof(void*))) {
        LOG("[EOSAuth] NoPresence: platform options not readable -- not stripping integrated platform.");
        return;
    }
    int32_t apiver = *(const int32_t*)p;
    if (apiver < 12) {
        LOG("[EOSAuth] NoPresence: platform ApiVersion=%d < 12 has no integrated-platform field -- nothing to strip.", apiver);
        return;
    }
    // Prove the genuine Epic layout before writing at +120 (same fingerprint the
    // redirect path uses): real EOS ids at their expected offsets.
    const char** ppProductId    = (const char**)(p + 16);
    const char** ppDeploymentId = (const char**)(p + 80);
    if (!LooksLikeEosId(*ppProductId) || !LooksLikeEosId(*ppDeploymentId)) {
        LOG("[EOSAuth] NoPresence: platform options don't match the Epic layout -- NOT stripping integrated platform.");
        return;
    }
    void** ppIP = (void**)(p + EOS_PLATFORM_IPCONTAINER_OFFSET);
    if (*ppIP) {
        LOG("[EOSAuth] NoPresence: disabling EOS Integrated Platform (container was %p) so the "
            "anonymous Device ID user isn't rejected by platform-login validation.", *ppIP);
        *ppIP = nullptr;   // SDK: NULL == integrated platform disabled for the host
    } else {
        LOG("[EOSAuth] NoPresence: integrated platform already disabled (container null).");
    }
}

// ------------------------------------------------------------
// Hooked EOS_Platform_Create -- rewrite the product/sandbox/deployment/
// client ids the game initializes with to point at OUR Epic app.
// ------------------------------------------------------------
static EOS_HPlatform __cdecl Hooked_Platform_Create(const void* Options)
{
    // Arm EOS SDK logging exactly once. By the time the game reaches
    // Platform_Create, EOS_Initialize has run, so SetCallback is now valid.
    static LONG s_logArmed = 0;
    if (InterlockedCompareExchange(&s_logArmed, 1, 0) == 0) {
        if (g_pfn_SetLogCallback) {
            g_pfn_SetLogCallback(&UcoEosLogSink);
            if (g_pfn_SetLogLevel)
                g_pfn_SetLogLevel(0x7fffffff /*EOS_LC_ALL_CATEGORIES*/,
                                  g_Cfg.bVerboseLog ? 500 /*EOS_LOG_Verbose*/ : 300 /*EOS_LOG_Warning*/);
            LOG("[EOSAuth] EOS SDK logging routed into host log (%s).",
                g_Cfg.bVerboseLog ? "verbose" : "warnings+errors -- set [EOS] VerboseLog=1 for full");
        } else {
            LOG("[EOSAuth] EOS_Logging_SetCallback not exported -- SDK internals stay invisible.");
        }
    }

    // [EOS] NoPresence, part 2: disable the integrated platform (Steam<->EOS
    // identity bridge) so it doesn't reject the anonymous Device ID user during
    // lobby creation. Done here, before either mode's path, so it applies whether
    // we redirect or KeepGameApp. Self-validates the layout before writing.
    if (g_Cfg.bNoPresence)
        StripIntegratedPlatform(Options);

    // KeepGameApp: skip the redirect entirely -- keep the game's OWN Epic app and
    // only anonymise the login (Device ID). Checked BEFORE the app ids so it wins
    // even when they are filled in.
    if (g_Cfg.bKeepGameApp) {
        InterlockedExchange(&g_bDoDeviceIdLogin, 1);
        static LONG s_keepLogged = 0;
        if (InterlockedCompareExchange(&s_keepLogged, 1, 0) == 0)
            LOG("[EOSAuth] KeepGameApp: NOT redirecting -- Device ID login on the game's own "
                "EOS app (DisplayName=%s).", g_Cfg.DisplayName);
        return g_orig_Platform_Create ? g_orig_Platform_Create(Options) : nullptr;
    }

    if (Options) {
        uint8_t* p = (uint8_t*)Options;   // writable: we redirect the ids in place

        // Validate the options block itself before touching any field.
        if (!IsReadable(p, 88)) {
            LOG("[EOSAuth] EOS_Platform_Create: options block not readable -- skipping redirect.");
            return g_orig_Platform_Create ? g_orig_Platform_Create(Options) : nullptr;
        }

        int32_t apiver = *(const int32_t*)(p + 0);
        DumpEosOptions(p);   // diagnostic: where do the EOS ids actually live?

        const char** ppProductId    = (const char**)(p + 16);
        const char** ppSandboxId    = (const char**)(p + 24);
        EOS_Platform_ClientCredentials* cc = (EOS_Platform_ClientCredentials*)(p + 32);
        const char** ppDeploymentId = (const char**)(p + 80);

        // Prove the layout: all three ids must look like real EOS ids. If not,
        // we're almost certainly not looking at the genuine Epic SDK's struct.
        // ProductId + DeploymentId at their expected offsets are proof enough of
        // the genuine EOS layout. SandboxId is NOT required: some games (NMRiH2)
        // pass it null, and demanding it made the whole redirect bail. We overwrite
        // it with ours below regardless.
        const bool bLayoutOK = LooksLikeEosId(*ppProductId)
                            && LooksLikeEosId(*ppDeploymentId);

        if (!bLayoutOK) {
            LOG("[EOSAuth] EOS_Platform_Create (ApiVersion=%d): options do NOT match the expected "
                "Epic SDK layout -- NOT redirecting (the game will use its own EOS app).", apiver);
            LOG("[EOSAuth]   Most likely an EOS EMULATOR dll (e.g. Nemirtingas) is installed instead "
                "of the genuine Epic EOSSDK-Win64-Shipping.dll. Restore the original to use this plugin.");
            LOG("[EOSAuth]   (If you ARE on the genuine SDK, it may have changed layout -- the offsets "
                "in this plugin would need updating.)");
            return g_orig_Platform_Create ? g_orig_Platform_Create(Options) : nullptr;
        }

        LOG("[EOSAuth] EOS_Platform_Create (ApiVersion=%d) game values:", apiver);
        LOG("[EOSAuth]   ProductId=%s SandboxId=%s DeploymentId=%s",
            SafeStr(*ppProductId), SafeStr(*ppSandboxId), SafeStr(*ppDeploymentId));
        LOG("[EOSAuth]   ClientId=%s", SafeStr(cc ? cc->ClientId : nullptr));

        if (g_Cfg.bValid) {
            // Point the platform at OUR Epic app. Both players must use the
            // same ids or they end up in different session pools.
            *ppProductId    = g_Cfg.ProductId;
            *ppSandboxId    = g_Cfg.SandboxId;
            *ppDeploymentId = g_Cfg.DeploymentId;
            if (cc) {
                cc->ClientId     = g_Cfg.ClientId;
                cc->ClientSecret = g_Cfg.ClientSecret;
            }
            InterlockedExchange(&g_bPlatformRedirected, 1);
            InterlockedExchange(&g_bDoDeviceIdLogin, 1);
            LOG("[EOSAuth]   -> REDIRECTED to Product=%s Sandbox=%s Deployment=%s Client=%s",
                g_Cfg.ProductId, g_Cfg.SandboxId, g_Cfg.DeploymentId, g_Cfg.ClientId);
        } else {
            LOG("[EOSAuth]   -> NOT redirected: [EOS] section in union-crax.ini is incomplete");
        }
    } else {
        LOG("[EOSAuth] EOS_Platform_Create: Options=null");
    }
    return g_orig_Platform_Create ? g_orig_Platform_Create(Options) : nullptr;
}

// ------------------------------------------------------------
// Hooked EOS_Connect_Login -- the important one. Swap the game's
// external platform credential for an anonymous Device ID login.
// ------------------------------------------------------------
static const int EOS_ECT_DEVICEID_ACCESS_TOKEN = 10;

// EOS_Connect_CreateDeviceIdOptions (eos_connect_types.h, ApiVersion 1)
struct EOS_Connect_CreateDeviceIdOptions
{
    int32_t     ApiVersion;
    const char* DeviceModel;
};

typedef void (__cdecl* Fn_EOS_Connect_CreateDeviceId)(EOS_HConnect Handle, const void* Options,
                                                     void* ClientData, void* CompletionDelegate);
static Fn_EOS_Connect_CreateDeviceId g_pfn_CreateDeviceId = nullptr;
static volatile LONG g_bDeviceIdRequested = 0;
static volatile LONG g_bDeviceIdReady     = 0;

// A login we deferred until the device id exists, so we can re-issue it from
// the CreateDeviceId callback with the game's original ClientData/delegate.
struct PendingLogin
{
    EOS_HConnect Handle;
    void*        ClientData;
    void*        CompletionDelegate;
    bool         bValid;
};
static PendingLogin g_Pending = {};

static void IssueDeviceIdLogin(const PendingLogin& p);   // fwd

// EOS_Success (0) or EOS_DuplicateNotAllowed both mean "a device id now exists".
// The result code is the first field of the callback data struct.
static void __cdecl OnCreateDeviceIdCb(const void* Data)
{
    int32_t rc = (Data && IsReadable(Data, sizeof(int32_t))) ? *(const int32_t*)Data : -1;
    LOG("[EOSAuth] CreateDeviceId completed (ResultCode=%d) -- device id now exists.", rc);

    InterlockedExchange(&g_bDeviceIdReady, 1);

    // Now -- and only now -- run the login we held back.
    if (g_Pending.bValid) {
        PendingLogin p = g_Pending;
        g_Pending.bValid = false;
        LOG("[EOSAuth] Re-issuing the deferred Device ID login now that the device id exists.");
        IssueDeviceIdLogin(p);
    }
}

// Device ID login fails with EOS_NotFound unless a device id already exists.
//
// The first version of this fired CreateDeviceId and let the original login
// continue in the same tick, relying on the game to retry. That worked only by
// luck: the login was submitted ~2ms BEFORE the device id existed. Forever
// Skies happened to retry login every ~4s; Palworld only ever attempts it once.
//
// So instead we DEFER: hold the first login, create the device id, and re-issue
// the login from that callback. Ordering is then guaranteed rather than raced.
static void RequestDeviceId(EOS_HConnect Handle)
{
    if (!g_pfn_CreateDeviceId || !Handle) return;
    if (InterlockedExchange(&g_bDeviceIdRequested, 1) != 0) return;   // once only

    EOS_Connect_CreateDeviceIdOptions opts = {};
    opts.ApiVersion  = 1;
    opts.DeviceModel = "PC Windows 64-bit";

    LOG("[EOSAuth] Requesting EOS_Connect_CreateDeviceId (required before Device ID login)...");
    g_pfn_CreateDeviceId(Handle, &opts, nullptr, (void*)&OnCreateDeviceIdCb);
}

// Storage for the rewritten options. The SDK copies what it needs during the
// call, but keep these alive for the call's duration regardless.
static EOS_Connect_Credentials   g_DevCreds    = {};
static EOS_Connect_UserLoginInfo g_DevLoginInf = {};
static EOS_Connect_LoginOptions  g_DevLoginOpts = {};

// Struct versions copied from the game's own call, so the options we build are
// ABI-compatible with whatever EOS SDK this game ships.
static int32_t g_LoginOptsApiVersion = 2;
static int32_t g_CredsApiVersion     = 1;
static int32_t g_UserInfoApiVersion  = 1;

// Build a Device ID login and call the real EOS_Connect_Login, preserving the
// game's original ClientData/CompletionDelegate so its callback still fires.
static void IssueDeviceIdLogin(const PendingLogin& p)
{
    if (!g_orig_Connect_Login || !p.bValid) return;

    g_DevCreds.ApiVersion = g_CredsApiVersion;
    g_DevCreds.Token      = nullptr;                      // must be null for Device ID
    g_DevCreds.Type       = EOS_ECT_DEVICEID_ACCESS_TOKEN;

    // Device ID login REQUIRES UserLoginInfo with a DisplayName.
    // Never advertise a version newer than the layout declared above, even if
    // the game asked for one -- the SDK would read fields we don't have.
    g_DevLoginInf.ApiVersion  = (g_UserInfoApiVersion > EOSC_USERLOGININFO_MAX_SUPPORTED)
                                ? EOSC_USERLOGININFO_MAX_SUPPORTED : g_UserInfoApiVersion;
    g_DevLoginInf.DisplayName = g_Cfg.DisplayName;
    g_DevLoginInf.NsaIdToken  = nullptr;

    g_DevLoginOpts.ApiVersion    = g_LoginOptsApiVersion;
    g_DevLoginOpts.Credentials   = &g_DevCreds;
    g_DevLoginOpts.UserLoginInfo = &g_DevLoginInf;

    LOG("[EOSAuth] -> EOS_Connect_Login as DEVICEID_ACCESS_TOKEN (DisplayName=%s)", g_Cfg.DisplayName);
    g_orig_Connect_Login(p.Handle, &g_DevLoginOpts, p.ClientData, p.CompletionDelegate);
}

static void __cdecl Hooked_Connect_Login(EOS_HConnect Handle, const void* Options,
                                        void* ClientData, void* CompletionDelegate)
{
    if (Options) {
        EOS_Connect_LoginOptions* o = (EOS_Connect_LoginOptions*)Options;   // writable

        // Act when Device ID mode is armed (g_bDoDeviceIdLogin) -- set either by
        // the redirect to our own app, or by [EOS] KeepGameApp keeping the game's
        // app and only anonymising the login. Validate before dereferencing, in
        // case this isn't the layout we expect (EOS emulator / newer SDK).
        if (!InterlockedCompareExchange(&g_bDoDeviceIdLogin, 0, 0)) {
            LOG("[EOSAuth] EOS_Connect_Login: Device ID mode not armed -- passing through untouched.");
            if (g_orig_Connect_Login) g_orig_Connect_Login(Handle, Options, ClientData, CompletionDelegate);
            return;
        }
        if (!IsReadable(o, sizeof(EOS_Connect_LoginOptions)) ||
            (o->Credentials && !IsReadable(o->Credentials, sizeof(EOS_Connect_Credentials)))) {
            LOG("[EOSAuth] EOS_Connect_Login: options not readable as expected -- passing through untouched.");
            if (g_orig_Connect_Login) g_orig_Connect_Login(Handle, Options, ClientData, CompletionDelegate);
            return;
        }

        int t = o->Credentials ? o->Credentials->Type : -1;

        LOG("[EOSAuth] EOS_Connect_Login: OptionsApiVersion=%d Credentials.Type=%d (%s) Token=%s",
            o->ApiVersion, t, ExternalCredTypeName(t),
            (o->Credentials && o->Credentials->Token) ? "(present)" : "(null)");

        // An external platform credential (e.g. STEAM_SESSION_TICKET) is
        // validated server-side by Epic against that platform. An EMULATED
        // Steam ticket always fails there -- the game log shows
        //   External credential 'STEAM_SESSION_TICKET' failed to authenticate
        //   with EOS Connect: EOS_Connect_ExternalTokenValidationFailed
        // which leaves the player with no ProductUserId, so Redpoint then
        // refuses CreateSession ("does not have online identity") and no
        // session/lobby is ever advertised.
        //
        // Device ID login is anonymous: no external platform, no identity
        // provider, nothing for Epic to validate against Steam. It yields a
        // real ProductUserId on our own deployment, which is all the game
        // needs to create and advertise a session.
        if (t != EOS_ECT_DEVICEID_ACCESS_TOKEN) {   // Device ID mode already armed above
            // Remember the struct versions the game used so our re-issued call
            // stays ABI-compatible with whatever SDK this is.
            g_LoginOptsApiVersion = o->ApiVersion;
            g_CredsApiVersion     = o->Credentials   ? o->Credentials->ApiVersion   : 1;
            g_UserInfoApiVersion  = o->UserLoginInfo ? o->UserLoginInfo->ApiVersion : 1;

            PendingLogin p = { Handle, ClientData, CompletionDelegate, true };

            if (InterlockedCompareExchange(&g_bDeviceIdReady, 0, 0)) {
                // Device id already exists (a later login attempt) -- issue now.
                LOG("[EOSAuth]   -> DEVICEID login (device id already exists)");
                IssueDeviceIdLogin(p);
                return;   // we issued the real call; don't also forward the original
            }

            // First attempt: hold this login, create the device id, and let the
            // CreateDeviceId callback re-issue it. Ordering is then guaranteed
            // instead of raced (the old code forwarded the login ~2ms before the
            // device id existed and relied on the game retrying).
            g_Pending = p;
            LOG("[EOSAuth]   -> DEFERRING login until the device id exists");
            RequestDeviceId(Handle);
            return;   // deliberately do NOT forward the doomed Steam-ticket login
        }
    } else {
        LOG("[EOSAuth] EOS_Connect_Login: Options=null");
    }
    if (g_orig_Connect_Login)
        g_orig_Connect_Login(Handle, Options, ClientData, CompletionDelegate);
}

// ------------------------------------------------------------
// [EOS] NoPresence -- force non-presence lobbies.
//
// Some Steam+EOS games (StarRupture) create PRESENCE-enabled EOS lobbies.
// Presence is an EPIC-ACCOUNT concept: it lets the Social Overlay advertise
// "Join Game" through the user's Epic social graph and the integrated (Steam)
// platform. Our anonymous Device ID login is Connect-ONLY -- it yields a
// ProductUserId but no Epic account and no linked external account -- so the SDK
// refuses to advertise presence and then refuses the lobby outright:
//   LogEOSLobby            Lobby will be created, but user lacks permission to advertise presence.
//   LogEOSIntegratedPlatform  ValidateUserPlatformLoginStatus() Specified local user ... is invalid.
//   LogEOSLobby            Cannot create Lobby, user permissions do not allow it.
//
// Forcing bPresenceEnabled=false drops the lobby off the presence / integrated-
// platform path, so a Connect-only user can create and join it. Trade-off: the
// Steam "Join Game" overlay is presence-driven, so with presence off joining may
// rely on the game's own in-game invite / lobby-code flow instead.
//
// bPresenceEnabled sits at +24 (x64) in ALL of CreateLobby / JoinLobby /
// JoinLobbyById options, and every field ahead of it exists since each struct's
// v1, so the offset is version-robust for any ApiVersion >= 2:
//   CreateLobby:    ApiVersion(0) LocalUserId(8) MaxLobbyMembers(16) PermissionLevel(20) bPresenceEnabled(24)
//   JoinLobby:      ApiVersion(0) LobbyDetailsHandle(8) LocalUserId(16) bPresenceEnabled(24)
//   JoinLobbyById:  ApiVersion(0) LobbyId(8) LocalUserId(16) bPresenceEnabled(24)
// ------------------------------------------------------------
static const size_t EOS_LOBBY_PRESENCE_OFFSET = 24;

typedef void (__cdecl* Fn_EOS_Lobby_Op)(void* Handle, const void* Options,
                                        void* ClientData, void* CompletionDelegate);
static Fn_EOS_Lobby_Op g_orig_CreateLobby   = nullptr;
static Fn_EOS_Lobby_Op g_orig_JoinLobby     = nullptr;
static Fn_EOS_Lobby_Op g_orig_JoinLobbyById = nullptr;

// Zero bPresenceEnabled in a lobby options struct, after proving the layout so we
// never write into an unexpected struct (an EOS emulator / a future SDK reorder).
static void ForcePresenceOff(const void* Options, const char* which)
{
    if (!Options) return;
    uint8_t* p = (uint8_t*)Options;   // writable: the game's own options block
    if (!IsReadable(p, EOS_LOBBY_PRESENCE_OFFSET + sizeof(int32_t))) {
        LOG("[EOSAuth] %s: options not readable -- leaving presence untouched.", which);
        return;
    }
    int32_t  apiver    = *(const int32_t*)p;
    int32_t* pPresence = (int32_t*)(p + EOS_LOBBY_PRESENCE_OFFSET);
    // Sanity: a plausible ApiVersion and a real EOS_Bool (0/1) currently at +24.
    if (apiver < 1 || apiver > 64 || (*pPresence != 0 && *pPresence != 1)) {
        LOG("[EOSAuth] %s: unexpected layout (ApiVersion=%d, presence=%d) -- NOT touching it.",
            which, apiver, *pPresence);
        return;
    }
    if (*pPresence != 0) {
        *pPresence = 0;   // EOS_FALSE
        LOG("[EOSAuth] %s: forced bPresenceEnabled=false (NoPresence).", which);
    }
    // Already false -> nothing to do, stay silent (avoids per-call spam).
}

static void __cdecl Hooked_CreateLobby(void* H, const void* O, void* CD, void* CB)
{
    ForcePresenceOff(O, "EOS_Lobby_CreateLobby");
    if (g_orig_CreateLobby) g_orig_CreateLobby(H, O, CD, CB);
}
static void __cdecl Hooked_JoinLobby(void* H, const void* O, void* CD, void* CB)
{
    ForcePresenceOff(O, "EOS_Lobby_JoinLobby");
    if (g_orig_JoinLobby) g_orig_JoinLobby(H, O, CD, CB);
}
static void __cdecl Hooked_JoinLobbyById(void* H, const void* O, void* CD, void* CB)
{
    ForcePresenceOff(O, "EOS_Lobby_JoinLobbyById");
    if (g_orig_JoinLobbyById) g_orig_JoinLobbyById(H, O, CD, CB);
}

// Install the three lobby hooks (only when [EOS] NoPresence is set). Any that the
// SDK doesn't export are simply skipped.
static void InstallNoPresenceHooks(HMODULE hEos)
{
    struct { const char* name; Fn_EOS_Lobby_Op hook; Fn_EOS_Lobby_Op* orig; } lobbies[] = {
        { "EOS_Lobby_CreateLobby",   &Hooked_CreateLobby,   &g_orig_CreateLobby },
        { "EOS_Lobby_JoinLobby",     &Hooked_JoinLobby,     &g_orig_JoinLobby },
        { "EOS_Lobby_JoinLobbyById", &Hooked_JoinLobbyById, &g_orig_JoinLobbyById },
    };
    int hooked = 0;
    for (auto& L : lobbies) {
        void* pfn = (void*)GetProcAddress(hEos, L.name);
        if (!pfn) { LOG("[EOSAuth] NoPresence: %s not exported -- skipping.", L.name); continue; }
        if (MH_CreateHook(pfn, (void*)L.hook, (void**)L.orig) == MH_OK &&
            MH_EnableHook(pfn) == MH_OK)
            ++hooked;
        else
            LOG("[EOSAuth] NoPresence: hook %s failed.", L.name);
    }
    LOG("[EOSAuth] NoPresence armed: %d/3 lobby entry points hooked "
        "(forcing bPresenceEnabled=false).", hooked);
}

// ------------------------------------------------------------
// Locating the EOS SDK.
//
// "EOSSDK-Win64-Shipping.dll" is only the Redpoint/UE convention.
// Games rename it, ship it under a wrapper, or load a 32-bit build,
// and matching on the name alone silently hooks nothing at all.
//
// So identify it by CAPABILITY instead: walk the loaded modules and
// take the first one that actually exports EOS_Platform_Create. That
// is what we need from it, and no non-EOS module exports it.
// ------------------------------------------------------------
static HMODULE FindEosModule(char* nameOut, size_t nameOutSize)
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (snap == INVALID_HANDLE_VALUE) return nullptr;

    HMODULE found = nullptr;
    MODULEENTRY32 me = {};
    me.dwSize = sizeof(me);

    if (Module32First(snap, &me)) {
        do {
            HMODULE h = me.hModule;
            if (!h) continue;
            if (GetProcAddress(h, "EOS_Platform_Create")) {
                found = h;
                if (nameOut && nameOutSize) {
                    // szModule is TCHAR; the project builds Unicode.
                    WideCharToMultiByte(CP_UTF8, 0, me.szModule, -1,
                                        nameOut, (int)nameOutSize, nullptr, nullptr);
                }
                break;
            }
        } while (Module32Next(snap, &me));
    }

    CloseHandle(snap);
    return found;
}

// Diagnosis aid for the give-up path: list anything EOS-shaped that IS
// loaded, so a game that hides the SDK somewhere unexpected is obvious.
static void LogEosLookingModules()
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (snap == INVALID_HANDLE_VALUE) return;

    MODULEENTRY32 me = {};
    me.dwSize = sizeof(me);
    int hits = 0;

    if (Module32First(snap, &me)) {
        do {
            char name[MAX_PATH] = {};
            WideCharToMultiByte(CP_UTF8, 0, me.szModule, -1, name, sizeof(name), nullptr, nullptr);
            if (StrStrIA(name, "eos") || StrStrIA(name, "epic")) {
                LOG("[EOSAuth]   loaded but no EOS_Platform_Create export: %s", name);
                ++hits;
            }
        } while (Module32Next(snap, &me));
    }
    if (!hits)
        LOG("[EOSAuth]   no EOS/Epic-looking module is loaded in this process at all.");

    CloseHandle(snap);
}

// Diagnostic: list EVERY module exporting EOS_Platform_Create. Games with
// EasyAntiCheat can load a SECOND EOS SDK (EAC's) alongside the game's, and if we
// hook the wrong one our redirect never touches the game's platform.
static void LogAllEosModules()
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (snap == INVALID_HANDLE_VALUE) return;
    MODULEENTRY32 me = {};
    me.dwSize = sizeof(me);
    int n = 0;
    if (Module32First(snap, &me)) {
        do {
            FARPROC pc = me.hModule ? GetProcAddress(me.hModule, "EOS_Platform_Create") : nullptr;
            if (!pc) continue;
            char name[MAX_PATH] = {};
            WideCharToMultiByte(CP_UTF8, 0, me.szModule, -1, name, sizeof(name), nullptr, nullptr);
            LOG("[EOSAuth]   EOS module #%d: %s (Platform_Create @ %p, base %p)",
                ++n, name, (void*)pc, (void*)me.hModule);
        } while (Module32Next(snap, &me));
    }
    if (n > 1)
        LOG("[EOSAuth]   NOTE: %d EOS SDKs loaded -- only the first is currently hooked.", n);
    CloseHandle(snap);
}

// ------------------------------------------------------------
// LoadLibrary trap.
//
// Forever Skies 1.17.1 (Redpoint OSS) DYNAMICALLY loads the EOS SDK and creates
// the platform ~400ms into startup -- BEFORE a CreateThread'd watcher can even
// be scheduled (measured: the platform booted 54ms before the watcher thread
// armed the trap). A polling watcher therefore always loses the race, the
// platform boots with the GAME's own Epic app, our redirect never runs, and the
// Steam ticket is rejected exactly as if the plugin weren't there.
//
// So we hook LoadLibrary and arm it SYNCHRONOUSLY from DllMain (see DllMain),
// which is ~400ms ahead of the SDK load. The moment a module that exports
// EOS_Platform_Create finishes loading, we install our EOS hooks -- before
// control returns to the game, so its subsequent EOS_Platform_Create is ours.
// We hook AFTER calling the original LoadLibrary (loader lock already released),
// so the MinHook thread-freeze there is safe. The watcher thread stays on as a
// pure fallback poll for games the trap doesn't cover.
// ------------------------------------------------------------
static bool InstallHooks();               // fwd
static volatile LONG g_bEosHooked = 0;    // set once EOS_Platform_Create is hooked
static CRITICAL_SECTION g_InstallCs;      // serialises InstallHooks (trap vs. watcher)
static volatile LONG g_bInstallCsReady = 0;

typedef HMODULE (WINAPI* Fn_LoadLibraryExW)(LPCWSTR, HANDLE, DWORD);
typedef HMODULE (WINAPI* Fn_LoadLibraryW)(LPCWSTR);
static Fn_LoadLibraryExW g_orig_LoadLibraryExW = nullptr;
static Fn_LoadLibraryW   g_orig_LoadLibraryW   = nullptr;

static void TryHookAfterLoad(HMODULE m)
{
    if (!m) return;
    if (InterlockedCompareExchange(&g_bEosHooked, 0, 0)) return;
    // Cheap: did the module that just loaded bring in EOS_Platform_Create?
    if (GetProcAddress(m, "EOS_Platform_Create")) {
        LOG("[EOSAuth] LoadLibrary trap CAUGHT the EOS SDK load; hooking before the game can use it.");
        InstallHooks();
    }
}

static HMODULE WINAPI Hooked_LoadLibraryExW(LPCWSTR name, HANDLE hFile, DWORD flags)
{
    HMODULE m = g_orig_LoadLibraryExW(name, hFile, flags);
    TryHookAfterLoad(m);
    return m;
}
static HMODULE WINAPI Hooked_LoadLibraryW(LPCWSTR name)
{
    HMODULE m = g_orig_LoadLibraryW(name);
    TryHookAfterLoad(m);
    return m;
}

static void InstallLoadLibraryTrap()
{
    MH_STATUS s = MH_Initialize();
    if (s != MH_OK && s != MH_ERROR_ALREADY_INITIALIZED) return;
    HMODULE k32 = GetModuleHandleW(L"kernel32.dll");
    if (!k32) return;
    void* pExW = (void*)GetProcAddress(k32, "LoadLibraryExW");
    void* pW   = (void*)GetProcAddress(k32, "LoadLibraryW");
    if (pExW && MH_CreateHook(pExW, (void*)&Hooked_LoadLibraryExW,
                              (void**)&g_orig_LoadLibraryExW) == MH_OK)
        MH_EnableHook(pExW);
    if (pW && MH_CreateHook(pW, (void*)&Hooked_LoadLibraryW,
                            (void**)&g_orig_LoadLibraryW) == MH_OK)
        MH_EnableHook(pW);
    LOG("[EOSAuth] LoadLibrary trap armed (catches a late/dynamic EOS SDK load in time).");
}

// ------------------------------------------------------------
// Watcher: wait for the EOS SDK module, resolve exports, hook them.
// ------------------------------------------------------------
static bool InstallHooksLocked()
{
    if (InterlockedCompareExchange(&g_bEosHooked, 0, 0)) return true; // already done
    char eosName[MAX_PATH] = {};
    HMODULE hEos = FindEosModule(eosName, sizeof(eosName));
    if (!hEos) return false; // not loaded yet

    LogAllEosModules();   // diagnostic: how many EOS SDKs are loaded, which is ours?

    void* pPlatformCreate = (void*)GetProcAddress(hEos, "EOS_Platform_Create");
    void* pConnectLogin   = (void*)GetProcAddress(hEos, "EOS_Connect_Login");

    // Called (not hooked) to create the device id before Device ID login.
    g_pfn_CreateDeviceId = (Fn_EOS_Connect_CreateDeviceId)GetProcAddress(hEos, "EOS_Connect_CreateDeviceId");
    if (!g_pfn_CreateDeviceId)
        LOG("[EOSAuth] WARNING: EOS_Connect_CreateDeviceId not exported; Device ID login will fail with EOS_NotFound");

    // Resolved here, ARMED later (from Hooked_Platform_Create, once EOS_Initialize
    // has run) so the SDK's own log flows into the host log.
    g_pfn_SetLogCallback = (Fn_EOS_Logging_SetCallback)GetProcAddress(hEos, "EOS_Logging_SetCallback");
    g_pfn_SetLogLevel    = (Fn_EOS_Logging_SetLogLevel)GetProcAddress(hEos, "EOS_Logging_SetLogLevel");
    if (!pPlatformCreate || !pConnectLogin) {
        LOG("[EOSAuth] EOS module loaded but exports missing "
            "(Platform_Create=%p Connect_Login=%p). Is this the real Epic SDK?",
            pPlatformCreate, pConnectLogin);
        return true; // stop retrying; wrong DLL
    }

    MH_STATUS s = MH_Initialize();
    if (s != MH_OK && s != MH_ERROR_ALREADY_INITIALIZED) {
        LOG("[EOSAuth] MH_Initialize failed: %d", s);
        return true;
    }

    if (MH_CreateHook(pPlatformCreate, (void*)&Hooked_Platform_Create,
                      (void**)&g_orig_Platform_Create) == MH_OK)
        MH_EnableHook(pPlatformCreate);
    else
        LOG("[EOSAuth] hook EOS_Platform_Create failed");

    if (MH_CreateHook(pConnectLogin, (void*)&Hooked_Connect_Login,
                      (void**)&g_orig_Connect_Login) == MH_OK)
        MH_EnableHook(pConnectLogin);
    else
        LOG("[EOSAuth] hook EOS_Connect_Login failed");

    // [EOS] NoPresence: also hook the lobby create/join entry points so we can
    // force bPresenceEnabled=false (for games whose lobbies demand presence, which
    // an anonymous Device ID login can't advertise).
    if (g_Cfg.bNoPresence)
        InstallNoPresenceHooks(hEos);

    InterlockedExchange(&g_bEosHooked, 1);
    LOG("[EOSAuth] hooks installed in %s (Platform_Create @ %p, Connect_Login @ %p).",
        eosName[0] ? eosName : "(unnamed module)", pPlatformCreate, pConnectLogin);
    return true;
}

// The trap (loader thread) and the fallback watcher can both reach here at the
// same instant; without serialisation the second one races past the g_bEosHooked
// check and logs spurious "hook ... failed" (MinHook rejecting an already-created
// hook). Serialise so exactly one caller does the install.
static bool InstallHooks()
{
    if (InterlockedCompareExchange(&g_bEosHooked, 0, 0)) return true; // fast path
    if (!InterlockedCompareExchange(&g_bInstallCsReady, 0, 0))
        return InstallHooksLocked(); // CS not up yet (shouldn't happen); best-effort
    EnterCriticalSection(&g_InstallCs);
    bool r = InstallHooksLocked();
    LeaveCriticalSection(&g_InstallCs);
    return r;
}

static DWORD WINAPI WatcherProc(LPVOID)
{
    // The LoadLibrary trap (armed in DllMain) is the primary mechanism and
    // catches the SDK the instant it loads. This poll is only a fallback for a
    // game that brings the SDK in by a path the trap doesn't see, or that has
    // it already loaded before we get here.
    if (InstallHooks()) return 0;

    // Poll up to ~3 minutes for the EOS SDK to load (subdir load can be late).
    for (int i = 0; i < 900 && InterlockedCompareExchange(&g_bShutdown, 0, 0) == 0; ++i) {
        if (InstallHooks()) return 0;
        Sleep(200);
    }
    LOG("[EOSAuth] no module exporting EOS_Platform_Create appeared in this process; nothing hooked.");
    LogEosLookingModules();
    LOG("[EOSAuth]   If the game runs its online layer in a SEPARATE process, this plugin "
        "is in the wrong one -- it only sees the exe that loaded UCOnline2's steam_api64.dll.");
    return 0;
}

// ------------------------------------------------------------
// UCO plugin entry points
//
// TIMING NOTE: the game creates its EOS platform and does its
// initial Connect login during STARTUP -- often before the host
// calls UCO_PluginInit (which waits for SteamAPI_Init). If we only
// started watching here we'd miss both calls entirely. So the
// watcher is started from DllMain (earliest possible moment, when
// UCOnline2's steam_api64.dll is loaded) and UCO_PluginInit just
// upgrades us to the host's logger once it's available.
// ------------------------------------------------------------
extern "C" __declspec(dllexport) int UCO_PluginInit(const UCO_PluginContext* ctx)
{
    if (!ctx || ctx->ApiVersion != UCO_PLUGIN_API_VERSION) return 1;
    g_Log = ctx->Log;
    LOG("[EOSAuth] plugin init (host logger attached).");
    return 0;
}

extern "C" __declspec(dllexport) void UCO_PluginShutdown(void)
{
    InterlockedExchange(&g_bShutdown, 1);
    if (g_hWatcher) {
        WaitForSingleObject(g_hWatcher, 1000);
        CloseHandle(g_hWatcher);
        g_hWatcher = nullptr;
    }
    MH_DisableHook(MH_ALL_HOOKS);
    LOG("[EOSAuth] plugin shutdown.");
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        InitializeCriticalSection(&g_InstallCs);
        InterlockedExchange(&g_bInstallCsReady, 1);
        // Start watching for the EOS SDK IMMEDIATELY -- the game's
        // EOS_Platform_Create / first EOS_Connect_Login happen during
        // startup, before UCO_PluginInit would ever run.
        LoadEosConfig();
        LOG("[EOSAuth] DllMain -- config %s (Product=%s Client=%s DisplayName=%s); watching for EOS SDK.",
            g_Cfg.bValid ? (g_Cfg.bUsingBaked ? "OK (baked-in default Epic app)" : "OK (your own Epic app from ini)")
                         : "INCOMPLETE (no ini creds and no baked default -- set [EOS] in union-crax.ini)",
            g_Cfg.ProductId[0] ? g_Cfg.ProductId : "(unset)",
            g_Cfg.ClientId[0]  ? g_Cfg.ClientId  : "(unset)",
            g_Cfg.DisplayName);
        if (g_Cfg.bKeepGameApp || g_Cfg.bNoPresence)
            LOG("[EOSAuth] DllMain -- flags: KeepGameApp=%d NoPresence=%d.",
                g_Cfg.bKeepGameApp ? 1 : 0, g_Cfg.bNoPresence ? 1 : 0);
        // Arm the LoadLibrary trap RIGHT HERE, synchronously. Forever Skies
        // 1.17.1 dynamic-loads the EOS SDK and creates the platform ~400ms into
        // startup -- earlier than a freshly-CreateThread'd watcher can even get
        // scheduled (measured: SDK booted 54ms BEFORE the thread armed the trap).
        // Arming from DllMain (loader lock held, but MinHook only patches
        // kernel32 code + suspends threads -- it never takes the loader lock)
        // guarantees the trap is live long before the SDK loads. The EOS hooks
        // themselves are still installed later from the trap callback, after the
        // real LoadLibrary has returned and released the lock.
        InstallLoadLibraryTrap();
        g_hWatcher = CreateThread(nullptr, 0, WatcherProc, nullptr, 0, nullptr);
    }
    return TRUE;
}
