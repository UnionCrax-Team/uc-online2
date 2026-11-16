// ============================================================
// UCOnline2 -- playfab_universal Plugin
//
// Game-agnostic PlayFab redirect for Unity/Mono games running
// under UCOnline2. Points a game's PlayFab traffic at a title
// YOU control and logs in without Steam ownership validation.
//
// WHY THIS IS NEEDED
//   A game's own PlayFab title only accepts LoginWithSteam, which
//   makes PlayFab ask Steam "does this ticket own AppId X?". Under
//   UCOnline2 the ticket is minted for the spoofed AppId 480, so
//   that check fails and multiplayer never initialises. You cannot
//   fix this on the publisher's title -- but PlayFab titles are
//   free, so you point the game at your own and log in with an
//   anonymous CustomID instead. Same escape hatch as EOS_custom.
//
// MODULES (all optional, all driven from union-crax.ini)
//   1. TITLE REDIRECT (Mono)
//      PlayFab.PlayFabSettings.TitleId -> [PlayFab]TitleId.
//      Set at the source, so it is transport-agnostic.
//      (WinHTTP hooking does NOT reach this: UnityPlayer only
//      imports WinHttpGetIEProxyConfigForCurrentUser -- Unity's
//      UnityWebRequest has its own HTTP stack.)
//
//   2. LOGIN SWITCH (Mono, PlayFabUnityHttp.MakeApiCall)
//      Rewrites the outgoing request mid-flight:
//        URL   /Client/LoginWithSteam -> /Client/LoginWithCustomID
//        body  -> {"TitleId":"<ours>","CustomId":"<id>","CreateAccount":true}
//      LoginResult is identical either way, so the game's own
//      success handler runs unchanged and gets a real EntityToken.
//      This hook also acts as a safety net for module 1: any
//      *.playfabapi.com URL still pointing at another title has its
//      host rewritten here, even if set_TitleId landed late.
//
//   3. NATIVE ENDPOINT REDIRECT (WinHTTP)
//      PlayFab Party (PartyWin32.dll / PartyWin.dll) is native and
//      never sees the C# TitleId, so it POSTs to the ORIGINAL title
//      with OUR entity token -> "InvalidAPIEndpoint". Rewriting the
//      WinHttpConnect host fixes it; both hosts share the
//      *.playfabapi.com wildcard cert so TLS still validates.
//
//   4. OFFLINE-GATE UNLOCK (Mono, optional)
//      Force a static bool (e.g. Raft_Network.SignedIntoPlayfab)
//      true so the multiplayer UI is usable before login resolves.
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
#include <set>
#include <map>

#include "../../include/MinHook.h"
#include "../../include/uco_plugin.h"
#include "mono_runtime.h"

static UCO_LogFn      g_Log            = nullptr;
static volatile LONG  g_bShutdown      = 0;
static HANDLE         g_hWatcherThread = nullptr;
static ISteamUser*    g_pSteamUser     = nullptr;

// ---- config ------------------------------------------------
static char    g_TitleIdA[64]    = {};   // our PlayFab TitleId
static wchar_t g_TitleIdW[64]    = {};   // wide, for the WinHTTP host rewrite
static char    g_CustomIdA[128]  = {};   // literal CustomId; empty = use Steam ID
static char    g_LoginEndpoints[256] = {};  // comma-separated, e.g. "LoginWithSteam"
static bool    g_bNativeRedirect = true;    // module 3 on/off
static bool    g_bKeepGameTitle  = false;   // [PlayFab] KeepGameTitle: anonymous login
                                            // on the game's OWN title, no redirect.
                                            // WINS even if TitleId is filled in.
static char    g_GameTitleId[64] = {};      // game's own title, captured at runtime from
                                            // the first login URL (KeepGameTitle mode)

static char    g_GateAssembly[64]  = {};
static char    g_GateNamespace[64] = {};
static char    g_GateClass[64]     = {};
static char    g_GateField[64]     = {};

static bool    g_bVerbose = false;   // [PlayFab] VerboseLog: log every request/rewrite,
                                     // not just the first of each kind.

#define LOG(...)  do { if (g_Log) g_Log(__VA_ARGS__); } while (0)
#define VLOG(...) do { if (g_bVerbose && g_Log) g_Log(__VA_ARGS__); } while (0)
// Log the first of a kind always; log every subsequent one only under VerboseLog.
#define LOG_FIRST_OR_VERBOSE(flag, ...) \
    do { if (g_bVerbose || InterlockedExchange(&(flag), 1) == 0) LOG(__VA_ARGS__); } while (0)

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

// The identity we present to our own PlayFab title. Stable per Steam
// account, so a player keeps the same PlayFab entity across sessions.
static const char* GetCustomId()
{
    static char id[128] = {};
    if (id[0]) return id;
    if (g_CustomIdA[0]) {
        strncpy_s(id, sizeof(id), g_CustomIdA, _TRUNCATE);
    } else {
        _snprintf_s(id, sizeof(id), _TRUNCATE, "%llu",
                    (unsigned long long)GetRealSteamID());
    }
    return id;
}

// Should the plugin act on login calls at all? True when we have our own title
// OR when KeepGameTitle asks us to anonymise the login on the game's own title.
static bool PfActive()        { return g_TitleIdA[0] != 0 || g_bKeepGameTitle; }
// Should we REDIRECT the title/host to ours? Only when we have a title AND are
// not in KeepGameTitle mode (which deliberately keeps the game's title).
static bool PfRedirectTitle() { return g_TitleIdA[0] != 0 && !g_bKeepGameTitle; }

// The TitleId to write into the CustomId login body: ours when redirecting, else
// the game's own (captured from the login URL under KeepGameTitle).
static const char* BodyTitleId()
{
    if (g_bKeepGameTitle && g_GameTitleId[0]) return g_GameTitleId;
    return g_TitleIdA;
}

// Capture "<title>" from https://<title>.playfabapi.com/... once, so KeepGameTitle
// can echo the game's real title back in the login body.
static void CaptureGameTitleFromUrl(const char* url)
{
    if (!g_bKeepGameTitle || g_GameTitleId[0] || !url) return;
    const char* dom = strstr(url, ".playfabapi.com");
    const char* schemeEnd = strstr(url, "://");
    const char* hostStart = schemeEnd ? schemeEnd + 3 : url;
    if (!dom || hostStart >= dom) return;
    size_t n = (size_t)(dom - hostStart);
    if (n == 0 || n >= sizeof(g_GameTitleId)) return;
    memcpy(g_GameTitleId, hostStart, n);
    g_GameTitleId[n] = 0;
    LOG("[PlayFab] KeepGameTitle: keeping the game's own title %s (no redirect)", g_GameTitleId);
}

// ------------------------------------------------------------
// MODULE 1: PlayFab TitleId redirect (managed set_TitleId)
// ------------------------------------------------------------
typedef void (*Fn_setTitleId)(void* /*MonoString*/);
static volatile LONG g_TitleIdRedirected = 0;

static void TryRedirectTitleId()
{
    if (!PfRedirectTitle()) return;   // KeepGameTitle keeps the game's own title
    if (InterlockedCompareExchange(&g_TitleIdRedirected, 0, 0)) return;
    void* fn = MONO_FindMethodPtr("PlayFab", "PlayFab", "PlayFabSettings", "set_TitleId", 1);
    if (!fn) return;
    MonoString* s = MONO_StringNew(g_TitleIdA);
    if (!s) return;
    ((Fn_setTitleId)fn)(s);
    InterlockedExchange(&g_TitleIdRedirected, 1);
    LOG("[PlayFab] PlayFabSettings.TitleId -> %s", g_TitleIdA);
}

// ------------------------------------------------------------
// MODULE 2: login switch + host safety net
// (PlayFabUnityHttp.MakeApiCall)
// ------------------------------------------------------------
typedef void(__fastcall* Fn_MakeApiCall)(void* pThis, void* reqContainer);
static Fn_MakeApiCall g_origMakeApiCall   = nullptr;
static int            g_offFullUrl        = -1;
static int            g_offPayload        = -1;
static int            g_offApiEndpoint    = -1;
static volatile LONG  g_LoginSwitchLogged = 0;
static volatile LONG  g_HostFixLogged     = 0;

// True if `url` names one of the platform logins we redirect. Kept to an
// explicit list so ordinary API calls are never touched.
static const char* MatchLoginEndpoint(const char* url)
{
    const char* list = g_LoginEndpoints[0] ? g_LoginEndpoints : "LoginWithSteam";
    char buf[256];
    strncpy_s(buf, sizeof(buf), list, _TRUNCATE);

    char* ctx = nullptr;
    for (char* tok = strtok_s(buf, ",; \t", &ctx); tok; tok = strtok_s(nullptr, ",; \t", &ctx)) {
        // The result points into `url` (the caller's buffer), not into `buf`,
        // so it stays valid after this function returns.
        const char* hit = strstr(url, tok);
        if (hit) return hit;
    }
    return nullptr;
}

// Rewrite the <title> in https://<title>.playfabapi.com/... to ours.
// Returns true if `out` was written.
static bool RewriteHost(const char* url, char* out, size_t outSize)
{
    if (!PfRedirectTitle()) return false;   // never rewrite the host under KeepGameTitle
    const char* dom = strstr(url, ".playfabapi.com");
    if (!dom) return false;

    const char* schemeEnd = strstr(url, "://");
    const char* hostStart = schemeEnd ? schemeEnd + 3 : url;
    if (hostStart > dom) return false;

    size_t hostLen = (size_t)(dom - hostStart);
    if (hostLen == strlen(g_TitleIdA) &&
        _strnicmp(hostStart, g_TitleIdA, hostLen) == 0)
        return false;   // already ours

    _snprintf_s(out, outSize, _TRUNCATE, "%.*s%s%s",
                (int)(hostStart - url), url, g_TitleIdA, dom);
    return true;
}

static void __fastcall Hooked_MakeApiCall(void* pThis, void* reqContainer)
{
    if (reqContainer && g_offFullUrl >= 0 && PfActive()) {
        MonoString* urlObj = *(MonoString**)((uint8_t*)reqContainer + g_offFullUrl);
        char url[512] = {};
        if (urlObj && MONO_StringToUtf8(urlObj, url, sizeof(url))) {

            CaptureGameTitleFromUrl(url);
            const char* hit = MatchLoginEndpoint(url);
            if (hit) {
                // -- platform login -> LoginWithCustomID --
                // Length of the matched endpoint name, so the tail is preserved.
                size_t pre    = (size_t)(hit - url);
                size_t nameLen = 0;
                while (hit[nameLen] && hit[nameLen] != '?' && hit[nameLen] != '/')
                    ++nameLen;

                char newUrl[512] = {};
                _snprintf_s(newUrl, sizeof(newUrl), _TRUNCATE, "%.*s%s%s",
                            (int)pre, url, "LoginWithCustomID", hit + nameLen);

                // Point it at our title too, in case module 1 lost the race.
                char fixedUrl[512] = {};
                const char* finalUrl = RewriteHost(newUrl, fixedUrl, sizeof(fixedUrl))
                                     ? fixedUrl : newUrl;

                MonoString* nu = MONO_StringNew(finalUrl);
                if (nu) *(void**)((uint8_t*)reqContainer + g_offFullUrl) = nu;

                if (g_offApiEndpoint >= 0) {
                    MonoString* ep = MONO_StringNew("/Client/LoginWithCustomID");
                    if (ep) *(void**)((uint8_t*)reqContainer + g_offApiEndpoint) = ep;
                }

                const char* cid = GetCustomId();
                char body[320] = {};
                int blen = _snprintf_s(body, sizeof(body), _TRUNCATE,
                    "{\"TitleId\":\"%s\",\"CustomId\":\"%s\",\"CreateAccount\":true}",
                    BodyTitleId(), cid);
                if (blen > 0 && g_offPayload >= 0) {
                    MonoObject* arr = MONO_NewByteArray(body, blen);
                    if (arr) *(void**)((uint8_t*)reqContainer + g_offPayload) = arr;
                }

                LOG_FIRST_OR_VERBOSE(g_LoginSwitchLogged,
                    "[PlayFab] login switched -> LoginWithCustomID (CustomId=%s, body=%d bytes)",
                        cid, blen);
            } else {
                // -- safety net: any other call still aimed at the old title --
                char fixedUrl[512] = {};
                if (RewriteHost(url, fixedUrl, sizeof(fixedUrl))) {
                    MonoString* nu = MONO_StringNew(fixedUrl);
                    if (nu) *(void**)((uint8_t*)reqContainer + g_offFullUrl) = nu;
                    if (InterlockedExchange(&g_HostFixLogged, 1) == 0)
                        LOG("[PlayFab] late host rewrite -> %s (set_TitleId lost the race)",
                            fixedUrl);
                }
            }
        }
    }
    if (g_origMakeApiCall) g_origMakeApiCall(pThis, reqContainer);
}

// ------------------------------------------------------------
// MODULE 3: native Party endpoint redirect (WinHTTP)
// ------------------------------------------------------------
typedef HINTERNET (WINAPI* Fn_WinHttpConnect)(HINTERNET, LPCWSTR, INTERNET_PORT, DWORD);
static Fn_WinHttpConnect g_origWinHttpConnect = nullptr;
static volatile LONG     g_WinHttpRedirLogged = 0;

static HINTERNET WINAPI Hooked_WinHttpConnect(HINTERNET hSession, LPCWSTR pswzServerName,
                                              INTERNET_PORT nServerPort, DWORD dwReserved)
{
    if (pswzServerName && g_TitleIdW[0] && !g_bKeepGameTitle && wcsstr(pswzServerName, L".playfabapi.com")) {
        wchar_t newName[128] = {};
        _snwprintf_s(newName, _countof(newName), _TRUNCATE, L"%s.playfabapi.com", g_TitleIdW);
        if (_wcsicmp(pswzServerName, newName) != 0) {
            LOG_FIRST_OR_VERBOSE(g_WinHttpRedirLogged,
                "[PlayFab] WinHttpConnect redirect %ls -> %ls", pswzServerName, newName);
            return g_origWinHttpConnect(hSession, newName, nServerPort, dwReserved);
        }
    }
    return g_origWinHttpConnect(hSession, pswzServerName, nServerPort, dwReserved);
}

// ------------------------------------------------------------
// MODULE 4: offline-gate unlock (optional static bool force)
// ------------------------------------------------------------
static MonoClass*    g_GateKlass     = nullptr;
static volatile LONG g_GateLogged    = 0;

static bool GateConfigured() { return g_GateClass[0] && g_GateField[0]; }

static void ApplyGate()
{
    if (!GateConfigured()) return;
    if (!g_GateKlass) {
        g_GateKlass = MONO_FindClass(g_GateAssembly[0] ? g_GateAssembly : nullptr,
                                     g_GateNamespace, g_GateClass);
        if (!g_GateKlass) return;
    }
    uint8_t trueVal = 1;
    if (MONO_SetStaticFieldByName(g_GateKlass, g_GateField, &trueVal)) {
        if (InterlockedExchange(&g_GateLogged, 1) == 0)
            LOG("[PlayFab] gate forced: %s.%s = true", g_GateClass, g_GateField);
    }
}

// ------------------------------------------------------------
// Install (idempotent per-hook; quiet retry until JIT-ready)
// ------------------------------------------------------------
static volatile LONG g_LoginHookDone   = 0;
static volatile LONG g_WinHttpHookDone = 0;

// ------------------------------------------------------------
// MODULE 5: native libHttpClient login rewrite (No Man's Sky etc.)
//
// Native PlayFab games use the C SDK over libHttpClient, so the Mono login swap
// (Module 2) never runs for them. The host rewrite (Module 3) sends their calls
// to our title, but LoginWithSteam there still fails -- our title cannot validate
// a Steam ticket minted under the 480 spoof. So do the SAME swap at the
// libHttpClient layer: when a call's URL is a platform login, rewrite the
// endpoint to LoginWithCustomID and replace the ticket body with a CustomId body.
// Host is left alone -- Module 3 rewrites it at connect time.
//
// libHttpClient.Win32.dll exports HCHttpCallRequestSetUrl and
// HCHttpCallRequestSetRequestBody{Bytes,String}. URL is set before the body, so
// we tag the call handle on SetUrl and swap its body on the following SetBody.
// (x64: all calling conventions collapse to one ABI, so no __stdcall needed.)
// ------------------------------------------------------------
// The PlayFab C SDK does not use SetRequestBodyBytes/String for the login body --
// it uses the STREAMING SetRequestBodyReadFunction (confirmed: the request went to
// LoginWithCustomID but the body still carried the SteamTicket). So we hook that
// too, and detect the login by BODY CONTENT ("SteamTicket") rather than only by
// tagging the handle on SetUrl, which is order-fragile.
typedef int32_t (*Fn_HcSetUrl)(void* call, const char* method, const char* url);
typedef int32_t (*Fn_HcSetBodyBytes)(void* call, const uint8_t* body, uint32_t size);
typedef int32_t (*Fn_HcSetBodyString)(void* call, const char* body);
// HCHttpCallRequestBodyReadFunction: HRESULT(call, size_t offset, size_t avail,
//   void* ctx, uint8_t* dest, size_t* written)
typedef int32_t (*Fn_HcBodyRead)(void* call, size_t offset, size_t avail,
                                 void* ctx, uint8_t* dest, size_t* written);
typedef int32_t (*Fn_HcSetBodyRead)(void* call, Fn_HcBodyRead readFn,
                                    size_t bodySize, void* ctx);
static Fn_HcSetUrl        g_origHcSetUrl        = nullptr;
static Fn_HcSetBodyBytes  g_origHcSetBodyBytes  = nullptr;
static Fn_HcSetBodyString g_origHcSetBodyString = nullptr;
static Fn_HcSetBodyRead   g_origHcSetBodyRead   = nullptr;
static volatile LONG      g_HcHookDone          = 0;
static volatile LONG      g_HcRedirLogged       = 0;
static volatile LONG      g_HcBodyLogged        = 0;
static CRITICAL_SECTION   g_HcLock;
static std::set<void*>    g_LoginCalls;   // handles tagged as a login on SetUrl
// The forged CustomId body is identical for every login this session, so a single
// buffer served by our read function is safe.
static char               g_LoginBody[256]  = {};
static uint32_t           g_LoginBodyLen    = 0;

static bool BodyIsSteamLogin(const void* p, size_t n)
{
    static const char needle[] = "SteamTicket";
    const size_t nl = sizeof(needle) - 1;
    if (!p || n < nl) return false;
    const char* b = (const char*)p;
    for (size_t i = 0; i + nl <= n; ++i)
        if (memcmp(b + i, needle, nl) == 0) return true;
    return false;
}

// Write "<prefix>LoginWithCustomID<suffix>" -- preserves host + query, only the
// endpoint name changes. Returns false if url is not a configured platform login.
static bool RewriteLoginUrl(const char* url, char* out, size_t outSize)
{
    const char* list = g_LoginEndpoints[0] ? g_LoginEndpoints : "LoginWithSteam";
    char buf[256];
    strncpy_s(buf, sizeof(buf), list, _TRUNCATE);
    char* ctx = nullptr;
    for (char* tok = strtok_s(buf, ",; \t", &ctx); tok; tok = strtok_s(nullptr, ",; \t", &ctx)) {
        const char* hit = strstr(url, tok);
        if (hit) {
            _snprintf_s(out, outSize, _TRUNCATE, "%.*sLoginWithCustomID%s",
                        (int)(hit - url), url, hit + strlen(tok));
            return true;
        }
    }
    return false;
}

static void MakeCustomIdBody(char* out, size_t outSize)
{
    _snprintf_s(out, outSize, _TRUNCATE,
                "{\"TitleId\":\"%s\",\"CustomId\":\"%s\",\"CreateAccount\":true}",
                BodyTitleId(), GetCustomId());
}

static void EnsureLoginBody()
{
    if (g_LoginBodyLen == 0) {
        MakeCustomIdBody(g_LoginBody, sizeof(g_LoginBody));
        g_LoginBodyLen = (uint32_t)strlen(g_LoginBody);
    }
}

static void LogBodySwapOnce(const char* how)
{
    LOG_FIRST_OR_VERBOSE(g_HcBodyLogged,
        "[PlayFab] native HC body swap (%s): SteamTicket -> CustomId (%u bytes)",
            how, g_LoginBodyLen);
}

// Our replacement read function: serves the CustomId body from g_LoginBody.
static int32_t OurBodyRead(void* /*call*/, size_t offset, size_t avail,
                           void* /*ctx*/, uint8_t* dest, size_t* written)
{
    size_t remaining = (offset < g_LoginBodyLen) ? (g_LoginBodyLen - offset) : 0;
    size_t n = remaining < avail ? remaining : avail;
    if (n && dest) memcpy(dest, g_LoginBody + offset, n);
    if (written) *written = n;
    return 0;   // S_OK
}

// erase-and-test: true exactly once, for the SetBody that follows a login SetUrl.
static bool TakeLoginCall(void* call)
{
    EnterCriticalSection(&g_HcLock);
    bool r = g_LoginCalls.erase(call) != 0;
    LeaveCriticalSection(&g_HcLock);
    return r;
}

static int32_t Hooked_HcSetUrl(void* call, const char* method, const char* url)
{
    if (url) CaptureGameTitleFromUrl(url);
    if (url && PfActive() && MatchLoginEndpoint(url)) {
        char newUrl[600];
        if (RewriteLoginUrl(url, newUrl, sizeof(newUrl))) {
            EnterCriticalSection(&g_HcLock);
            g_LoginCalls.insert(call);
            LeaveCriticalSection(&g_HcLock);
            LOG_FIRST_OR_VERBOSE(g_HcRedirLogged,
                "[PlayFab] native HC login: %s -> LoginWithCustomID (CustomId=%s)",
                    url, GetCustomId());
            return g_origHcSetUrl(call, method, newUrl);
        }
    }
    return g_origHcSetUrl(call, method, url);
}

static int32_t Hooked_HcSetBodyBytes(void* call, const uint8_t* body, uint32_t size)
{
    if (PfActive() && (TakeLoginCall(call) || BodyIsSteamLogin(body, size))) {
        (void)TakeLoginCall(call);   // clear the tag if content matched
        EnsureLoginBody();
        LogBodySwapOnce("bytes");
        return g_origHcSetBodyBytes(call, (const uint8_t*)g_LoginBody, g_LoginBodyLen);
    }
    return g_origHcSetBodyBytes(call, body, size);
}

static int32_t Hooked_HcSetBodyString(void* call, const char* body)
{
    if (PfActive() && (TakeLoginCall(call) || (body && strstr(body, "SteamTicket")))) {
        (void)TakeLoginCall(call);
        EnsureLoginBody();
        LogBodySwapOnce("string");
        return g_origHcSetBodyString(call, g_LoginBody);
    }
    return g_origHcSetBodyString(call, body);
}

// The path the PlayFab C SDK actually uses. Swap the whole streaming body for our
// CustomId body: replace both the read function and the declared body size (which
// libHttpClient uses for Content-Length), so the size stays consistent.
static int32_t Hooked_HcSetBodyRead(void* call, Fn_HcBodyRead readFn,
                                    size_t bodySize, void* ctx)
{
    if (PfActive() && TakeLoginCall(call)) {
        EnsureLoginBody();
        LogBodySwapOnce("readfn");
        return g_origHcSetBodyRead(call, &OurBodyRead, g_LoginBodyLen, nullptr);
    }
    return g_origHcSetBodyRead(call, readFn, bodySize, ctx);
}

// Mono-independent, like the WinHTTP redirect. No-op on Mono games (the DLL is
// simply absent).
static void InstallHcLoginRewrite()
{
    if (!g_TitleIdW[0] && !g_bKeepGameTitle) return;   // login swap still wanted under KeepGameTitle
    if (InterlockedCompareExchange(&g_HcHookDone, 0, 0)) return;
    HMODULE h = GetModuleHandleW(L"libHttpClient.Win32.dll");
    if (!h) return;   // native PlayFab SDK not loaded (yet, or a Mono game)
    void* pu = (void*)GetProcAddress(h, "HCHttpCallRequestSetUrl");
    if (!pu) return;
    if (MH_CreateHook(pu, (void*)&Hooked_HcSetUrl, (void**)&g_origHcSetUrl) != MH_OK) return;
    MH_EnableHook(pu);
    void* pb = (void*)GetProcAddress(h, "HCHttpCallRequestSetRequestBodyBytes");
    if (pb && MH_CreateHook(pb, (void*)&Hooked_HcSetBodyBytes, (void**)&g_origHcSetBodyBytes) == MH_OK)
        MH_EnableHook(pb);
    void* ps = (void*)GetProcAddress(h, "HCHttpCallRequestSetRequestBodyString");
    if (ps && MH_CreateHook(ps, (void*)&Hooked_HcSetBodyString, (void**)&g_origHcSetBodyString) == MH_OK)
        MH_EnableHook(ps);
    // The one the PlayFab C SDK actually uses for the login body.
    void* pr = (void*)GetProcAddress(h, "HCHttpCallRequestSetRequestBodyReadFunction");
    if (pr && MH_CreateHook(pr, (void*)&Hooked_HcSetBodyRead, (void**)&g_origHcSetBodyRead) == MH_OK)
        MH_EnableHook(pr);
    InterlockedExchange(&g_HcHookDone, 1);
    LOG("[PlayFab] libHttpClient login-rewrite hooks installed (SetUrl@%p bytes@%p str@%p readfn@%p)",
        pu, pb, ps, pr);
}

// MODULE 6: native libcurl login rewrite (UE5 / static OpenSSL games)
//
// UE5's FHttp runs on statically-linked libcurl over static OpenSSL, so it has no
// exports and never touches the Mono, WinHTTP, or libHttpClient paths above. Hook
// curl_easy_setopt and do the SAME swap at the request-config layer:
//   CURLOPT_URL         host -> our title, /Client/LoginWithSteam -> LoginWithCustomID
//   CURLOPT_POSTFIELDS  SteamTicket body -> {"TitleId","CustomId","CreateAccount"}
//   CURLOPT_POSTFIELDSIZE -> our body length (kept consistent with the swapped body)
// UE sets the URL before the body, so we tag the handle on the login URL and swap
// the body that follows; a stale tag is cleared whenever a handle's URL is re-set.
//
// curl_easy_setopt has no symbol, so we LOCATE it the version-robust way: scan the
// exe's .text for `mov edx, CURLOPT_URL(0x2712)` sites and resolve the call target
// each one shares. A UE monolith links libcurl more than once, so we hook every
// setopt-shaped target (variadic prologue) we find.
#define UCO_CURLOPT_URL             10002
#define UCO_CURLOPT_POSTFIELDS      10015
#define UCO_CURLOPT_COPYPOSTFIELDS  10165
#define UCO_CURLOPT_HTTPHEADER      10023
#define UCO_CURLOPT_WRITEFUNCTION   20011
#define UCO_CURLOPT_WRITEDATA       10001
#define UCO_CURLOPT_POSTFIELDSIZE      60

typedef intptr_t (*Fn_CurlSetopt)(void*, int, void*);   // variadic; 3rd arg lands in R8
static Fn_CurlSetopt    g_origCurlSetopt[4] = {};
static int              g_curlHooks    = 0;
static volatile LONG    g_CurlHookDone  = 0;
static volatile LONG    g_CurlLoggedUrl = 0, g_CurlLoggedBody = 0, g_CurlCLFixed = 0;
static CRITICAL_SECTION g_CurlLock;
static std::set<void*>  g_CurlLoginHandles;   // handles whose next body must be swapped

// Response capture for login handles: wrap the game's write callback so we can log
// PlayFab's reply (why it accepted/rejected our CustomId login) then forward it.
typedef size_t (*Fn_CurlWrite)(char*, size_t, size_t, void*);
struct CurlWriteCtx { Fn_CurlWrite origFn; void* origData; };
static std::map<void*, CurlWriteCtx> g_CurlWriteCtx;   // per login handle
struct CurlSlist { char* data; CurlSlist* next; };     // curl_slist for header logging

static size_t OurCurlWrite(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    CurlWriteCtx* c = (CurlWriteCtx*)userdata;
    size_t n = size * nmemb;
    static volatile LONG s_resp = 0;
    if (g_bVerbose || InterlockedIncrement(&s_resp) <= 4) {
        char buf[600]; size_t m = n < sizeof(buf) - 1 ? n : sizeof(buf) - 1;
        memcpy(buf, ptr, m); buf[m] = 0;
        LOG("[PlayFab] libcurl login RESPONSE (%zu bytes): %s", n, buf);
    }
    return (c && c->origFn) ? c->origFn(ptr, size, nmemb, c->origData) : n;
}

// Rewrite a *.playfabapi.com URL for curl: host -> our title (when redirecting) and,
// if it is a platform login, endpoint -> LoginWithCustomID. Returns whether it
// changed; sets *isLogin when the login endpoint was converted.
static bool RewriteCurlUrl(const char* url, char* out, size_t outSize, bool* isLogin)
{
    *isLogin = false;
    char tmp[700];
    strncpy_s(tmp, sizeof(tmp), url, _TRUNCATE);

    if (MatchLoginEndpoint(tmp)) {
        char sw[700];
        if (RewriteLoginUrl(tmp, sw, sizeof(sw))) { strncpy_s(tmp, sizeof(tmp), sw, _TRUNCATE); *isLogin = true; }
    }

    bool hostChanged = false;
    if (PfRedirectTitle()) {
        const char* dom = strstr(tmp, ".playfabapi.com");
        const char* sch = strstr(tmp, "://");
        if (dom && sch && sch < dom) {
            const char* hostStart = sch + 3;
            size_t hostLen = (size_t)(dom - hostStart);
            if (!(hostLen == strlen(g_TitleIdA) && _strnicmp(hostStart, g_TitleIdA, hostLen) == 0)) {
                _snprintf_s(out, outSize, _TRUNCATE, "%.*s%s%s",
                            (int)(hostStart - tmp), tmp, g_TitleIdA, dom);
                hostChanged = true;
            }
        }
    }
    if (!hostChanged) strncpy_s(out, outSize, tmp, _TRUNCATE);
    return *isLogin || hostChanged;
}

// Preserved-body path: transform the game's ORIGINAL LoginWithSteam body into a
// LoginWithCustomID body, keeping InfoRequestParameters/CreateAccount/etc so the
// login response still carries the player data the game asked for (a stripped body
// yields no InfoResultPayload, and some SDKs then treat the login as incomplete and
// loop). Swap "SteamTicket":"..." for "CustomId":"...", and the TitleId for ours.
static char          g_CurlLoginBody[8192] = {};
static uint32_t      g_CurlLoginBodyLen    = 0;
static volatile LONG g_CurlBodyBuilt       = 0;
static char*         g_CurlCLNode          = nullptr;   // in-flight login's Content-Length slist->data
// UE streams the request body via a read callback rather than POSTFIELDS, so we
// capture the callback + its context + size, then slurp the real body ourselves.
typedef size_t (*Fn_CurlRead)(char*, size_t, size_t, void*);
static Fn_CurlRead   g_CurlReadFn     = nullptr;
static void*         g_CurlReadData   = nullptr;
static long          g_CurlUploadSize = 0;
static void FixCurlContentLength(char* node);   // defined below; used by ApplyCurlLoginBody

static int BuildLoginBodyFromOriginal(const char* orig, char* out, size_t outSize)
{
    if (!orig) return 0;
    const char* stk = strstr(orig, "\"SteamTicket\"");
    if (!stk) return 0;
    const char* colon = strchr(stk, ':');   if (!colon) return 0;
    const char* q1 = strchr(colon, '\"');   if (!q1) return 0;   // value opening quote
    const char* q2 = strchr(q1 + 1, '\"');  if (!q2) return 0;   // value closing quote (base64: no escaped quotes)
    int pre = (int)(stk - orig);
    int n = _snprintf_s(out, outSize, _TRUNCATE, "%.*s\"CustomId\":\"%s\"%s",
                        pre, orig, GetCustomId(), q2 + 1);
    if (n <= 0) return 0;
    if (PfRedirectTitle() && g_TitleIdA[0]) {
        char* t = strstr(out, "\"TitleId\"");
        char* c = t ? strchr(t, ':') : nullptr;
        char* v1 = c ? strchr(c, '\"') : nullptr;
        char* v2 = v1 ? strchr(v1 + 1, '\"') : nullptr;
        if (v1 && v2) {
            char tail[4096]; strncpy_s(tail, sizeof(tail), v2, _TRUNCATE);   // closing quote + rest
            int pre2 = (int)(v1 + 1 - out);
            _snprintf_s(out + pre2, outSize - pre2, _TRUNCATE, "%s%s", g_TitleIdA, tail);
        }
    }
    return (int)strlen(out);
}

// Build g_CurlLoginBody from the game's real login body (from POSTFIELDS or slurped
// from the stream). Falls back to the minimal CustomId body if it isn't a steam
// login body. One-shot: the transformed body is identical for every login.
static void SetCurlLoginBodyFrom(const char* orig)
{
    if (g_CurlBodyBuilt) return;
    char built[8192];
    int n = BuildLoginBodyFromOriginal(orig, built, sizeof(built));
    if (n <= 0) { MakeCustomIdBody(built, sizeof(built)); n = (int)strlen(built); }
    if (n > (int)sizeof(g_CurlLoginBody) - 1) n = (int)sizeof(g_CurlLoginBody) - 1;
    memcpy(g_CurlLoginBody, built, n); g_CurlLoginBody[n] = 0; g_CurlLoginBodyLen = (uint32_t)n;
    g_CurlBodyBuilt = 1;
}

// Drive the game's own read callback to copy the streamed request body out. SEH-
// guarded: the callback is UE's, so bail to the fallback body if it misbehaves.
static uint32_t SlurpStream(Fn_CurlRead fn, void* data, long size, char* dst, uint32_t dstCap)
{
    uint32_t got = 0;
    __try {
        while (got + 1 < dstCap && (long)got < size) {
            size_t want = dstCap - 1 - got;
            if (want > (size_t)(size - (long)got)) want = (size_t)(size - (long)got);
            size_t r = fn(dst + got, 1, want, data);
            if (r == 0 || r >= 0x10000000) break;   // EOF / CURL_READFUNC_ABORT / _PAUSE
            got += (uint32_t)r;
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
    dst[got] = 0;
    return got;
}

// Slurp the streamed body, then build the transformed login body from it.
static bool BuildCurlBodyFromStream()
{
    if (g_CurlBodyBuilt) return true;
    if (!g_CurlReadFn || !g_CurlReadData || g_CurlUploadSize <= 0 || g_CurlUploadSize > 60000) return false;
    char* orig = (char*)malloc((size_t)g_CurlUploadSize + 1);
    if (!orig) return false;
    uint32_t got = SlurpStream(g_CurlReadFn, g_CurlReadData, g_CurlUploadSize, orig, (uint32_t)g_CurlUploadSize + 1);
    bool ok = false;
    if (got && strstr(orig, "\"SteamTicket\"")) {
        VLOG("[PlayFab] libcurl streamed login body (%u bytes): %.900s", got, orig);
        SetCurlLoginBodyFrom(orig);
        ok = (g_CurlBodyBuilt != 0);
    }
    free(orig);
    return ok;
}

// Override the request with our prepared login body (POSTFIELDS beats the stream).
static void ApplyCurlLoginBody(Fn_CurlSetopt orig, void* h)
{
    orig(h, UCO_CURLOPT_POSTFIELDS, (void*)g_CurlLoginBody);
    orig(h, UCO_CURLOPT_POSTFIELDSIZE, (void*)(intptr_t)g_CurlLoginBodyLen);
    FixCurlContentLength(g_CurlCLNode);
}

// Overwrite a "Content-Length: N" slist entry in place with our body length (always
// the shorter string, so it fits). No-op until the body length is known.
static void FixCurlContentLength(char* node)
{
    if (!node || !g_CurlLoginBodyLen) return;
    char fixed[32];
    int n = _snprintf_s(fixed, sizeof(fixed), _TRUNCATE, "Content-Length: %u", g_CurlLoginBodyLen);
    if (n > 0 && (size_t)n <= strlen(node)) {
        memcpy(node, fixed, n); node[n] = 0;
        LOG_FIRST_OR_VERBOSE(g_CurlCLFixed,
            "[PlayFab] libcurl fixed Content-Length -> %u (SDK had set the SteamTicket-body length)", g_CurlLoginBodyLen);
    }
}

static intptr_t CurlSetoptCommon(int idx, void* h, int opt, void* param)
{
    Fn_CurlSetopt orig = g_origCurlSetopt[idx];
    if (PfActive() && h) {
        // Diagnostic (verbose): trace every option set on a login handle AFTER its
        // URL is tagged, so we can see exactly how UE hands curl the request body
        // (POSTFIELDS vs the streaming READFUNCTION/READDATA path).
        if (g_bVerbose) {
            bool marked; EnterCriticalSection(&g_CurlLock); marked = g_CurlLoginHandles.count(h) != 0; LeaveCriticalSection(&g_CurlLock);
            if (marked) {
                static volatile LONG s_trace = 0;
                if (InterlockedIncrement(&s_trace) <= 80) {
                    const bool strOpt = (opt == 10002 || opt == 10015 || opt == 10165 || opt == 10036);
                    if (strOpt && param) VLOG("[PlayFab] login setopt(%d) = \"%.80s\"", opt, (const char*)param);
                    else                 VLOG("[PlayFab] login setopt(%d) = %p  [20012=READFN 10009=READDATA 46=UPLOAD 47=POST 14/115/30115=INFILESIZE 60=POSTFIELDSIZE]", opt, param);
                }
            }
        }
        if (opt == UCO_CURLOPT_URL && param) {
            const char* url = (const char*)param;
            // Diagnostic: surface the PlayFab URLs (all of them) plus the first
            // dozen others, so we can see which setopt copy carries which traffic.
            // VerboseLog logs every URL on every hook.
            static volatile LONG s_dbg = 0;
            if (g_bVerbose || strstr(url, "playfab") || InterlockedIncrement(&s_dbg) <= 12)
                LOG("[PlayFab] libcurl[hook %d] CURLOPT_URL: %.190s", idx, url);
            // Any new URL clears a stale login tag; re-added only for a fresh login.
            EnterCriticalSection(&g_CurlLock); g_CurlLoginHandles.erase(h); LeaveCriticalSection(&g_CurlLock);
            if (strstr(url, ".playfabapi.com")) {
                CaptureGameTitleFromUrl(url);
                char nu[700]; bool isLogin = false;
                if (RewriteCurlUrl(url, nu, sizeof(nu), &isLogin)) {
                    if (isLogin) {
                        EnterCriticalSection(&g_CurlLock); g_CurlLoginHandles.insert(h); LeaveCriticalSection(&g_CurlLock);
                        // fresh request: forget the previous login's per-request captures
                        g_CurlCLNode = nullptr; g_CurlReadFn = nullptr; g_CurlReadData = nullptr; g_CurlUploadSize = 0;
                    }
                    LOG_FIRST_OR_VERBOSE(g_CurlLoggedUrl, "[PlayFab] libcurl URL rewrite: %s -> %s%s",
                                         url, nu, isLogin ? "  (login -> CustomID)" : "");
                    return orig(h, opt, (void*)nu);   // curl copies the URL string
                }
                VLOG("[PlayFab] libcurl playfab URL (no rewrite needed): %s", url);
            }
        }
        else if (opt == UCO_CURLOPT_POSTFIELDS || opt == UCO_CURLOPT_COPYPOSTFIELDS) {
            bool swap;
            EnterCriticalSection(&g_CurlLock); swap = g_CurlLoginHandles.count(h) != 0; LeaveCriticalSection(&g_CurlLock);
            bool byContent = false;
            if (!swap && param) { byContent = BodyIsSteamLogin(param, strlen((const char*)param)); swap = byContent; }
            if (swap && param) {
                // Non-streaming: the real body is right here -> transform it directly.
                VLOG("[PlayFab] libcurl original login body (%d bytes): %.900s",
                     (int)strlen((const char*)param), (const char*)param);
                SetCurlLoginBodyFrom((const char*)param);
                LOG_FIRST_OR_VERBOSE(g_CurlLoggedBody,
                    "[PlayFab] libcurl body swap%s: SteamTicket -> CustomId (%u bytes)",
                    byContent ? " (by content)" : "", g_CurlLoginBodyLen);
                ApplyCurlLoginBody(orig, h);
                return orig(h, opt, (void*)g_CurlLoginBody);
            }
            // swap && !param: UE streams the body (handled when READFUNCTION is set).
        }
        else if (opt == UCO_CURLOPT_POSTFIELDSIZE) {
            bool login;
            EnterCriticalSection(&g_CurlLock); login = g_CurlLoginHandles.count(h) != 0; LeaveCriticalSection(&g_CurlLock);
            if (login) {
                g_CurlUploadSize = (long)(intptr_t)param;   // remember the stream length for slurping
                if (g_CurlLoginBodyLen) {                    // body already prepared -> use our size
                    VLOG("[PlayFab] libcurl POSTFIELDSIZE swap -> %u", g_CurlLoginBodyLen);
                    return orig(h, opt, (void*)(intptr_t)g_CurlLoginBodyLen);
                }
            }
        }
        else if (opt == 10009 /*CURLOPT_READDATA*/) {
            bool login;
            EnterCriticalSection(&g_CurlLock); login = g_CurlLoginHandles.count(h) != 0; LeaveCriticalSection(&g_CurlLock);
            if (login) g_CurlReadData = param;
        }
        else if (opt == 20012 /*CURLOPT_READFUNCTION*/) {
            bool login;
            EnterCriticalSection(&g_CurlLock); login = g_CurlLoginHandles.count(h) != 0; LeaveCriticalSection(&g_CurlLock);
            if (login) {
                g_CurlReadFn = (Fn_CurlRead)param;
                // We now have readfn + readdata + size: slurp the streamed body,
                // transform it (keeps InfoRequestParameters), and override the stream.
                if (!g_CurlBodyBuilt && !BuildCurlBodyFromStream()) SetCurlLoginBodyFrom(nullptr);
                if (g_CurlBodyBuilt) {
                    LOG_FIRST_OR_VERBOSE(g_CurlLoggedBody,
                        "[PlayFab] libcurl streamed body -> CustomId (%u bytes, InfoRequestParameters preserved)",
                        g_CurlLoginBodyLen);
                    intptr_t r = orig(h, opt, param);   // let the readfn be set (POSTFIELDS overrides it)
                    ApplyCurlLoginBody(orig, h);
                    return r;
                }
            }
        }
        else if (opt == UCO_CURLOPT_HTTPHEADER && param) {
            bool login;
            EnterCriticalSection(&g_CurlLock); login = g_CurlLoginHandles.count(h) != 0; LeaveCriticalSection(&g_CurlLock);
            if (login) {
                for (CurlSlist* s = (CurlSlist*)param; s; s = s->next) {
                    if (!s->data) continue;
                    VLOG("[PlayFab] libcurl login request header: %s", s->data);
                    // Remember the manual Content-Length node; POSTFIELDS fixes it once
                    // the swapped body's length is known (headers are set before the body).
                    if (_strnicmp(s->data, "Content-Length:", 15) == 0) {
                        g_CurlCLNode = s->data;
                        FixCurlContentLength(s->data);   // already-known length (later logins) -> fix now
                    }
                }
            }
        }
        else if ((opt == UCO_CURLOPT_WRITEFUNCTION || opt == UCO_CURLOPT_WRITEDATA) && g_bVerbose) {
            // Diagnostic only (verbose): wrap the response callback to log PlayFab's
            // reply. Kept off the normal path -- it assumes UE sets both WRITEFUNCTION
            // and WRITEDATA (which FCurlHttpRequest does).
            bool login;
            EnterCriticalSection(&g_CurlLock); login = g_CurlLoginHandles.count(h) != 0; LeaveCriticalSection(&g_CurlLock);
            if (login) {
                EnterCriticalSection(&g_CurlLock);
                CurlWriteCtx* c = &g_CurlWriteCtx[h];
                LeaveCriticalSection(&g_CurlLock);
                if (opt == UCO_CURLOPT_WRITEFUNCTION) { c->origFn = (Fn_CurlWrite)param; return orig(h, opt, (void*)&OurCurlWrite); }
                c->origData = param; return orig(h, opt, (void*)c);
            }
        }
    }
    return orig(h, opt, param);
}
static intptr_t Hooked_CurlSetopt0(void* h, int o, void* p) { return CurlSetoptCommon(0, h, o, p); }
static intptr_t Hooked_CurlSetopt1(void* h, int o, void* p) { return CurlSetoptCommon(1, h, o, p); }
static intptr_t Hooked_CurlSetopt2(void* h, int o, void* p) { return CurlSetoptCommon(2, h, o, p); }
static intptr_t Hooked_CurlSetopt3(void* h, int o, void* p) { return CurlSetoptCommon(3, h, o, p); }
static void* const kCurlDetours[4] = {
    (void*)&Hooked_CurlSetopt0, (void*)&Hooked_CurlSetopt1,
    (void*)&Hooked_CurlSetopt2, (void*)&Hooked_CurlSetopt3
};

// setopt is variadic, so its prologue homes the integer arg registers into the
// shadow space -- MSVC does this either via rax (mov rax,rsp; mov [rax+x],edx) or
// straight to the stack (mov [rsp+x],edx). Match the opening of either form.
static bool CurlPrologueLooksVariadic(const uint8_t* p)
{
    if (p[0] == 0x48 && p[1] == 0x8B && p[2] == 0xC4) return true;   // mov rax, rsp
    if (p[0] == 0x89 && p[1] == 0x54 && p[2] == 0x24) return true;   // mov [rsp+imm8], edx
    return false;
}

static void InstallCurlLoginRewrite()
{
    if (InterlockedCompareExchange(&g_CurlHookDone, 0, 0)) return;
    if (!g_TitleIdA[0] && !g_bKeepGameTitle) { InterlockedExchange(&g_CurlHookDone, 1); return; }

    HMODULE base = GetModuleHandleW(nullptr);
    if (!base) return;
    const uint8_t* b = (const uint8_t*)base;
    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)b;
    IMAGE_NT_HEADERS* nt  = (IMAGE_NT_HEADERS*)(b + dos->e_lfanew);
    IMAGE_SECTION_HEADER* sec = IMAGE_FIRST_SECTION(nt);
    const uint8_t* text = nullptr; size_t textSize = 0;
    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i)
        if (memcmp(sec[i].Name, ".text", 5) == 0) { text = b + sec[i].VirtualAddress; textSize = sec[i].Misc.VirtualSize; break; }
    if (!text) { InterlockedExchange(&g_CurlHookDone, 1); return; }

    struct { void* addr; int cnt; } cand[8] = {};
    int nc = 0;
    for (size_t i = 0; i + 5 < textSize; ++i) {
        if (!(text[i] == 0xBA && text[i+1] == 0x12 && text[i+2] == 0x27 && text[i+3] == 0x00 && text[i+4] == 0x00))
            continue;   // mov edx, CURLOPT_URL(0x2712)
        for (size_t j = i + 5; j < i + 48 && j + 5 < textSize; ++j) {
            if (text[j] != 0xE8) continue;   // call rel32
            int32_t rel; memcpy(&rel, text + j + 1, 4);
            const uint8_t* tgt = text + j + 5 + rel;
            if (tgt < text || tgt >= text + textSize) break;
            int k; for (k = 0; k < nc; ++k) if (cand[k].addr == (void*)tgt) { cand[k].cnt++; break; }
            if (k == nc && nc < 8) { cand[nc].addr = (void*)tgt; cand[nc].cnt = 1; nc++; }
            break;
        }
    }
    for (int k = 0; k < nc && g_curlHooks < 4; ++k) {
        if (cand[k].cnt < 2 || !CurlPrologueLooksVariadic((const uint8_t*)cand[k].addr)) continue;
        int idx = g_curlHooks;
        if (MH_CreateHook(cand[k].addr, kCurlDetours[idx], (void**)&g_origCurlSetopt[idx]) == MH_OK &&
            MH_EnableHook(cand[k].addr) == MH_OK) {
            LOG("[PlayFab] libcurl curl_easy_setopt hook %d @ %p (%d URL sites)", idx, cand[k].addr, cand[k].cnt);
            g_curlHooks++;
        }
    }
    if (g_curlHooks == 0)
        LOG("[PlayFab] libcurl: no curl_easy_setopt found in exe (not a UE/libcurl game) -- module idle");
    InterlockedExchange(&g_CurlHookDone, 1);
}

// The native WinHTTP host-rewrite needs NO Mono runtime -- it hooks winhttp.dll
// directly. It MUST live outside the Mono gate: native PlayFab games (No Man's
// Sky) have no Mono, so MONO_TryInit() fails, and when this was below that gate
// the redirect was structurally unreachable for exactly the native games it
// exists to serve (the plugin logged "init ... nativeRedirect=1" but never
// installed the hook).
static void InstallNativeRedirect()
{
    if (!g_bNativeRedirect || !g_TitleIdW[0] || g_bKeepGameTitle) return;   // no host redirect under KeepGameTitle
    if (InterlockedCompareExchange(&g_WinHttpHookDone, 0, 0)) return;
    HMODULE hWin = GetModuleHandleW(L"winhttp.dll");
    if (!hWin) return;   // not loaded yet -- watcher retries
    void* pc = (void*)GetProcAddress(hWin, "WinHttpConnect");
    if (pc && MH_CreateHook(pc, (void*)&Hooked_WinHttpConnect,
                            (void**)&g_origWinHttpConnect) == MH_OK) {
        MH_EnableHook(pc);
        InterlockedExchange(&g_WinHttpHookDone, 1);
        LOG("[PlayFab] WinHttpConnect hook @ %p (redirect *.playfabapi.com -> %s.playfabapi.com)",
            pc, g_TitleIdA);
    }
}

static bool TryInstallAll()
{
    // Mono-independent native paths first: host redirect + libHttpClient login
    // rewrite + libcurl login rewrite. All no-op on games that lack their target
    // (no winhttp / no libHttpClient / no libcurl in the exe).
    InstallNativeRedirect();
    InstallHcLoginRewrite();
    InstallCurlLoginRewrite();
    const bool winhttpReady = (!g_bNativeRedirect || !g_TitleIdW[0] || g_bKeepGameTitle ||
                               InterlockedCompareExchange(&g_WinHttpHookDone, 0, 0) != 0);
    // Only wait on the libHttpClient hook if that DLL is actually present (native
    // PlayFab game). Mono games never load it, so do not block on it there.
    const bool hcReady = (GetModuleHandleW(L"libHttpClient.Win32.dll") == nullptr) ||
                         (InterlockedCompareExchange(&g_HcHookDone, 0, 0) != 0);
    // The libcurl scan is one-shot against the exe's .text (present from the start),
    // so it resolves on the first pass -- ready once it has run.
    const bool curlReady = (InterlockedCompareExchange(&g_CurlHookDone, 0, 0) != 0);
    const bool nativeReady = winhttpReady && hcReady && curlReady;

    // Everything below is Mono-only. A native game (NMS) has no Mono runtime, so
    // once the native redirect is in there is nothing more to do -- report ready
    // instead of spinning "not ready" forever waiting for a Mono that never loads.
    if (!MONO_TryInit())
        return nativeReady;

    TryRedirectTitleId();

    // -- login switch / host safety net --
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
                LOG("[PlayFab] PlayFabUnityHttp.MakeApiCall hook @ %p (FullUrl off 0x%X, Payload off 0x%X)",
                    mac, (unsigned)g_offFullUrl, (unsigned)g_offPayload);
            }
        }
    }

    // (native redirect already installed by InstallNativeRedirect() above)

    ApplyGate();

    bool loginReady   = InterlockedCompareExchange(&g_LoginHookDone, 0, 0) != 0;
    // nativeReady already computed above (before the Mono gate).
    bool titleReady   = (!g_TitleIdA[0] || g_bKeepGameTitle ||
                         InterlockedCompareExchange(&g_TitleIdRedirected, 0, 0) != 0);
    bool gateReady    = (!GateConfigured() || g_GateKlass != nullptr);

    return loginReady && nativeReady && titleReady && gateReady;
}

static DWORD WINAPI WatcherProc(LPVOID)
{
    bool installed = false;
    // Poll fast at first: the TitleId redirect has to land before the game's
    // first PlayFab call. (The MakeApiCall host rewrite covers us if it doesn't.)
    for (int i = 0; i < 1500 && InterlockedCompareExchange(&g_bShutdown, 0, 0) == 0; ++i) {
        if (TryInstallAll()) { installed = true; break; }
        Sleep(i < 100 ? 50 : 200);
    }

    if (!installed) {
        LOG("[PlayFab] watcher gave up (LoginHook=%ld TitleRedirect=%ld WinHttpHook=%ld)",
            InterlockedCompareExchange(&g_LoginHookDone, 0, 0),
            InterlockedCompareExchange(&g_TitleIdRedirected, 0, 0),
            InterlockedCompareExchange(&g_WinHttpHookDone, 0, 0));
        return 0;
    }

    // The gate is a static the game rewrites every frame, so it has to be
    // re-forced for as long as the process lives. Everything else is one-shot.
    while (GateConfigured() && InterlockedCompareExchange(&g_bShutdown, 0, 0) == 0) {
        ApplyGate();
        Sleep(200);
    }
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
        // [PlayFab] is the documented section. Section/key lookup is
        // case-insensitive, so [Playfab] works too; [Raft] is read last
        // so existing raft_mp installs keep working unchanged.
        GetPrivateProfileStringA("PlayFab", "TitleId", "", g_TitleIdA, sizeof(g_TitleIdA), ini);
        if (!g_TitleIdA[0]) GetPrivateProfileStringA("PlayFab", "PlayFabTitleId", "", g_TitleIdA, sizeof(g_TitleIdA), ini);
        if (!g_TitleIdA[0]) GetPrivateProfileStringA("Raft",    "PlayFabTitleId", "", g_TitleIdA, sizeof(g_TitleIdA), ini);
        if (g_TitleIdA[0])
            MultiByteToWideChar(CP_ACP, 0, g_TitleIdA, -1, g_TitleIdW, _countof(g_TitleIdW));

        GetPrivateProfileStringA("PlayFab", "CustomId",       "",               g_CustomIdA,     sizeof(g_CustomIdA),     ini);
        GetPrivateProfileStringA("PlayFab", "LoginEndpoints", "LoginWithSteam", g_LoginEndpoints, sizeof(g_LoginEndpoints), ini);
        g_bNativeRedirect = GetPrivateProfileIntA("PlayFab", "RedirectNativeHttp", 1, ini) != 0;
        // Anonymous login on the game's OWN title (no redirect). Read AFTER TitleId
        // and independent of it, so it wins even when TitleId is filled in.
        g_bKeepGameTitle  = GetPrivateProfileIntA("PlayFab", "KeepGameTitle", 0, ini) != 0;
        g_bVerbose        = GetPrivateProfileIntA("PlayFab", "VerboseLog", 0, ini) != 0;

        GetPrivateProfileStringA("PlayFab", "GateAssembly",  "", g_GateAssembly,  sizeof(g_GateAssembly),  ini);
        GetPrivateProfileStringA("PlayFab", "GateNamespace", "", g_GateNamespace, sizeof(g_GateNamespace), ini);
        GetPrivateProfileStringA("PlayFab", "GateClass",     "", g_GateClass,     sizeof(g_GateClass),     ini);
        GetPrivateProfileStringA("PlayFab", "GateField",     "", g_GateField,     sizeof(g_GateField),     ini);
    }

    LOG("[PlayFab] init: AppId=%u ogAppId=%u pSteamUser=%p TitleId=%s KeepGameTitle=%d logins=%s nativeRedirect=%d verbose=%d gate=%s",
        ctx->ForcedAppId, ctx->OriginalAppId, (void*)g_pSteamUser,
        g_TitleIdA[0] ? g_TitleIdA : (g_bKeepGameTitle ? "(game's own)" : "(none - plugin idle)"),
        g_bKeepGameTitle ? 1 : 0,
        g_LoginEndpoints, g_bNativeRedirect ? 1 : 0, g_bVerbose ? 1 : 0,
        GateConfigured() ? g_GateClass : "(none)");

    if (!PfActive()) {
        LOG("[PlayFab] no [PlayFab]TitleId and KeepGameTitle off -- nothing to do");
        return 0;
    }

    if (MH_Initialize() != MH_OK)
        LOG("[PlayFab] MH_Initialize non-OK (already inited?)");

    InitializeCriticalSection(&g_HcLock);   // guards g_LoginCalls (Module 5)
    InitializeCriticalSection(&g_CurlLock); // guards g_CurlLoginHandles (Module 6)
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
    LOG("[PlayFab] shutdown");
}

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) { return TRUE; }
