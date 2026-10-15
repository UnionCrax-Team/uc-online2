// ============================================================
// coherence_universal -- get past coherence Cloud's platform login.
//
// THE WALL
// Games on the coherence SDK authenticate against coherence Cloud with a
// platform credential. On Steam that is POST /login/steam carrying a Steam
// auth session ticket, which coherence validates SERVER-SIDE against the
// publisher's own project config (their Steam publisher key + real AppId).
// An emulated ticket minted for AppId 480 is therefore rejected outright:
//
//     path=/login/steam method=POST statusCode=400
//     errorCode=InvalidCredentials
//     -> "Online Services are not available at the moment."
//
// That check is not weak; it is passing judgement on a credential we cannot
// forge, exactly like Photon's custom auth and EOS's STEAM_SESSION_TICKET.
//
// THE LEVER
// coherence's own AuthClient exposes anonymous login as a first-class API:
//
//     public Task<LoginResult> LoginWithSteam(string ticket, string identity, CancellationToken ct)
//     public Task<LoginResult> LoginAsGuest(CancellationToken ct)
//
// Same class, same return type. So we redirect one to the other and the game
// gets a valid coherence session with no platform credential involved. This is
// the same shape as the EOS_custom fix (swap a rejected STEAM_SESSION_TICKET
// for anonymous DEVICEID login) which works on Forever Skies and Palworld.
//
// WHAT THIS DOES NOT SOLVE
// Guest login must be ENABLED on the project being logged into. If the
// publisher disabled it, this fails the same way and you need [Coherence]
// ProjectId to point the client at your own coherence project instead --
// which also means only people running the same fix can see each other, the
// same trade as bringing your own Photon app.
//
// Verified against Vampire Survivors 1.15.114 (coherence SDK 1.6).
// ============================================================
#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "../../include/uco_plugin.h"
#include "../../include/MinHook.h"
#include "il2cpp_runtime.h"

static UCO_LogFn g_Log = nullptr;
#define LOG(...) do { if (g_Log) g_Log(__VA_ARGS__); } while (0)

// il2cpp_runtime.cpp logs through this; the host plugin supplies it.
extern "C" void IL2CPP_Log(const char* fmt, ...)
{
    if (!g_Log) return;
    char buf[1024];
    va_list ap; va_start(ap, fmt);
    _vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, ap);
    va_end(ap);
    g_Log("%s", buf);
}

static HANDLE g_hWatcher   = nullptr;
static volatile LONG g_bShutdown = 0;
static volatile LONG g_bInstalled = 0;

// ---- config ------------------------------------------------
static bool g_bForceGuest = true;
static char g_ProjectId[128] = {};
static char g_RuntimeKey[128] = {};
static bool g_bLocalMode = false;
static bool g_bLaunchRS  = true;

// Launch the replication server the game already ships.
//
// coherence Cloud will not accept a login until the project has the game's
// schema registered, and schema upload is an editor-only operation -- there is
// no documented REST endpoint for it. The local path sidesteps that entirely:
// replication-server.exe takes the schema on the command line, so nothing has
// to be uploaded anywhere and no account is involved.
static void LaunchReplicationServer()
{
    char dir[MAX_PATH] = {};
    if (!GetModuleFileNameA(nullptr, dir, MAX_PATH)) return;
    if (char* s = strrchr(dir, '\\')) *s = '\0';

    char rs[MAX_PATH], schema[MAX_PATH];
    _snprintf_s(rs, sizeof(rs), _TRUNCATE,
        "%s\\VampireSurvivors_Data\\StreamingAssets\\replication-server.exe", dir);
    _snprintf_s(schema, sizeof(schema), _TRUNCATE,
        "%s\\VampireSurvivors_Data\\StreamingAssets\\combined.schema", dir);

    if (GetFileAttributesA(rs) == INVALID_FILE_ATTRIBUTES)
    {
        LOG("[Coherence] replication-server.exe not found at %s", rs);
        return;
    }

    // "rooms" is the mode the SDK's local-rooms path talks to; its default
    // ports (api 64001, udp 42001, signalling 42002, web 42003) are the same
    // ones RuntimeSettings carries, so no port juggling is needed.
    char cmd[MAX_PATH * 3];
    _snprintf_s(cmd, sizeof(cmd), _TRUNCATE,
        "\"%s\" rooms --schema \"%s\"", rs, schema);

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi = {};
    si.dwFlags     = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    if (CreateProcessA(nullptr, cmd, nullptr, nullptr, FALSE,
                       CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi))
    {
        LOG("[Coherence] replication server started (pid %lu)", pi.dwProcessId);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
    else
    {
        LOG("[Coherence] could not start the replication server (error %lu)", GetLastError());
    }
}

static const char* GetIniPath()
{
    static char path[MAX_PATH];
    static bool tried = false;
    if (tried) return path[0] ? path : nullptr;
    tried = true;

    char exeDir[MAX_PATH] = {};
    if (!GetModuleFileNameA(nullptr, exeDir, MAX_PATH)) return nullptr;
    char* slash = strrchr(exeDir, '\\');
    if (!slash) return nullptr;
    *slash = '\0';

    _snprintf_s(path, sizeof(path), _TRUNCATE, "%s\\union-crax.ini", exeDir);
    if (GetFileAttributesA(path) == INVALID_FILE_ATTRIBUTES) { path[0] = '\0'; return nullptr; }
    return path;
}

static void ReadIni()
{
    const char* ini = GetIniPath();
    if (!ini) { LOG("[Coherence] no union-crax.ini found -- using defaults"); return; }

    char buf[32] = {};
    GetPrivateProfileStringA("Coherence", "ForceGuestLogin", "true", buf, sizeof(buf), ini);
    // Windows does not strip inline comments, and a value of "true # foo"
    // must not read as false. Compare only the leading token.
    g_bForceGuest = (_strnicmp(buf, "true", 4) == 0 || buf[0] == '1' ||
                     _strnicmp(buf, "yes", 3) == 0 || _strnicmp(buf, "on", 2) == 0);

    GetPrivateProfileStringA("Coherence", "ProjectId", "", g_ProjectId, sizeof(g_ProjectId), ini);
    GetPrivateProfileStringA("Coherence", "RuntimeKey", "", g_RuntimeKey, sizeof(g_RuntimeKey), ini);

    char lb[32] = {};
    GetPrivateProfileStringA("Coherence", "LocalMode", "false", lb, sizeof(lb), ini);
    g_bLocalMode = (_strnicmp(lb, "true", 4) == 0 || lb[0] == '1' ||
                    _strnicmp(lb, "yes", 3) == 0 || _strnicmp(lb, "on", 2) == 0);
    GetPrivateProfileStringA("Coherence", "LaunchReplicationServer", "true", lb, sizeof(lb), ini);
    g_bLaunchRS = (_strnicmp(lb, "true", 4) == 0 || lb[0] == '1' ||
                   _strnicmp(lb, "yes", 3) == 0 || _strnicmp(lb, "on", 2) == 0);
    for (char* c = g_ProjectId; *c; ++c)
        if ((*c == '#' || *c == ';') && c > g_ProjectId && (c[-1] == ' ' || c[-1] == '\t')) { *c = '\0'; break; }
    for (char* c = g_RuntimeKey; *c; ++c)
        if ((*c == '#' || *c == ';') && c > g_RuntimeKey && (c[-1] == ' ' || c[-1] == '\t')) { *c = '\0'; break; }

    // Never log the key itself -- this file is routinely pasted into bug
    // reports, and it is the credential that identifies (and bills) a project.
    LOG("[Coherence] config: ForceGuestLogin=%d ProjectId=%s RuntimeKey=%s",
        g_bForceGuest ? 1 : 0,
        g_ProjectId[0]  ? g_ProjectId : "<unset, using the game's own>",
        g_RuntimeKey[0] ? "<set>" : "<unset, using the game's own>");
}

// ---- IL2CPP method ABI -------------------------------------
//
// An IL2CPP instance method compiles to
//     ret f(void* thisPtr, args..., const MethodInfo* method)
// CancellationToken is a struct with a single reference field, so it travels
// as one pointer-sized value and can be forwarded as an opaque void*.

typedef void* (*Fn_LoginWithSteam)(void* thiz, void* ticket, void* identity,
                                   void* ct, const MethodInfo* mi);
typedef void* (*Fn_LoginAsGuest)(void* thiz, void* ct, const MethodInfo* mi);
typedef void* (*Fn_GetProjectID)(void* thiz, const MethodInfo* mi);

// Static methods have NO `this` -- args then the MethodInfo*.
typedef void* (*Fn_CloudLoginWithSteam)(void* ticket, void* identity,
                                        void* ct, const MethodInfo* mi);
typedef void* (*Fn_CloudLoginAsGuest)(void* ct, const MethodInfo* mi);

static Fn_LoginWithSteam       g_RealLoginWithSteam      = nullptr;
static Fn_GetProjectID         g_RealGetProjectID        = nullptr;
static const MethodInfo*       g_GuestMethod             = nullptr;

static Fn_CloudLoginWithSteam  g_RealCloudLoginWithSteam = nullptr;
static const MethodInfo*       g_CloudGuestMethod        = nullptr;

// The one the game actually calls.
//
// AuthClient.LoginWithSteam is the low-level API, and hooking it installed
// cleanly but never fired: Vampire Survivors goes through the public static
// facade Coherence.Cloud.CoherenceCloud instead, which returns LoginOperation
// rather than Task<LoginResult>. That is the type CoherenceLoginModule holds,
// which is what gave it away. Both layers are hooked now -- whichever the game
// uses, the redirect happens, and the log says which one fired.
static void* Hooked_CloudLoginWithSteam(void* ticket, void* identity,
                                        void* ct, const MethodInfo* mi)
{
    if (g_CloudGuestMethod && g_CloudGuestMethod->methodPointer)
    {
        LOG("[Coherence] CoherenceCloud.LoginWithSteam intercepted -> LoginAsGuest");
        Fn_CloudLoginAsGuest guest = (Fn_CloudLoginAsGuest)g_CloudGuestMethod->methodPointer;
        return guest(ct, g_CloudGuestMethod);
    }
    LOG("[Coherence] CoherenceCloud.LoginWithSteam: no guest overload -- passing through");
    return g_RealCloudLoginWithSteam(ticket, identity, ct, mi);
}

static void* Hooked_LoginWithSteam(void* thiz, void* ticket, void* identity,
                                   void* ct, const MethodInfo* mi)
{
    if (g_GuestMethod && g_GuestMethod->methodPointer)
    {
        LOG("[Coherence] LoginWithSteam intercepted -> LoginAsGuest "
            "(the Steam ticket would be rejected: emulated tickets are minted "
            "for the spoofed AppId and coherence validates them server-side)");
        Fn_LoginAsGuest guest = (Fn_LoginAsGuest)g_GuestMethod->methodPointer;
        return guest(thiz, ct, g_GuestMethod);
    }

    // Never silently do nothing: if the guest path vanished in an update, say
    // so and let the original run so the failure is the game's, not ours.
    LOG("[Coherence] LoginWithSteam: LoginAsGuest unavailable -- passing through "
        "(expect InvalidCredentials)");
    return g_RealLoginWithSteam(thiz, ticket, identity, ct, mi);
}

// DO NOT HOOK THE GETTERS.
//
// get_RuntimeKey/get_ProjectID are one-line getters that load a string field
// and return it. Every such getter in the build compiles to byte-identical
// code, and the linker folds them into a SINGLE function (identical code
// folding). Hooking that address therefore hooks every string getter in the
// game at once: the first attempt made all of them return the runtime key, so
// Unity's own parameter names and asset paths became "fce1ea69...", and the
// game hung during addressable loading.
//
// Write the field instead. The offsets come from the il2cpp dump
// (runtimeKey +0x30, projectID +0x80) and are exact for this build; a wrong
// offset corrupts a neighbouring reference, so they are validated by reading
// the existing value back as a string before overwriting.
typedef void* (*Fn_GetInstance)(const MethodInfo* mi);

// Overwrite a managed string's CHARACTERS, in place.
//
// Replacing the RuntimeSettings field reference does not work: coherence
// captures the runtime key during CoherenceBridge init, which happens ~1s
// BEFORE this plugin loads (UCO2 loads plugins at SteamAPI_Init, after Unity
// has booted). Whatever we write to the field afterwards, the cached copy
// still holds the publisher's key -- proven by requests reaching their project
// 15s after we had rewritten the field repeatedly.
//
// The cache holds a REFERENCE to the same System.String, so editing that
// object's characters changes what every holder sees. Only safe when the
// lengths match exactly, which they do here (both keys are 32 hex chars) --
// a managed string's length is stored, not derived from a terminator, so
// writing a different length would corrupt it.
//
// Il2CppString layout: [0x00 object header][0x10 int32 length][0x14 UTF-16 chars]
static bool ReadIl2CppString(void* str, char* out, size_t outSize)
{
    if (!str || !out || outSize == 0) return false;
    const int32_t len = *(int32_t*)((uint8_t*)str + 0x10);
    if (len <= 0 || len > 4096) return false;
    const wchar_t* chars = (const wchar_t*)((uint8_t*)str + 0x14);
    const int n = WideCharToMultiByte(CP_UTF8, 0, chars, len, out, (int)outSize - 1, nullptr, nullptr);
    out[(n > 0) ? n : 0] = '\0';
    return n > 0;
}

static bool PatchStringInPlace(void* str, const char* utf8, const char* what)
{
    if (!str) return false;
    const int32_t len = *(int32_t*)((uint8_t*)str + 0x10);
    wchar_t wide[256] = {};
    const int n = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wide, 255) - 1;
    if (n <= 0) return false;

    if (n != len)
    {
        LOG("[Coherence] %s: length mismatch (existing %d chars, ours %d) -- refusing "
            "to patch in place; that would corrupt the string", what, (int)len, n);
        return false;
    }

    memcpy((uint8_t*)str + 0x14, wide, (size_t)n * sizeof(wchar_t));
    return true;
}

static bool WriteStringField(void* obj, uint32_t offset, const char* utf8, const char* what)
{
    if (!obj) return false;

    // Sanity-check: the slot should already hold a readable managed string.
    // If it does not, the offset is wrong for this build and writing would
    // scribble on something else.
    void* existing = *(void**)((uint8_t*)obj + offset);
    char probe[256] = {};
    if (!existing || !IL2CPP_DescribeObject((Il2CppObject*)existing, probe, sizeof(probe)))
    {
        LOG("[Coherence] %s: slot +0x%X does not hold a string -- refusing to write "
            "(offset is wrong for this build)", what, offset);
        return false;
    }

    void* s = IL2CPP_StringNew(utf8);
    if (!s)
    {
        LOG("[Coherence] %s: could not allocate the replacement string", what);
        return false;
    }

    *(void**)((uint8_t*)obj + offset) = s;
    LOG("[Coherence] %s: replaced (was \"%s\")", what, probe);
    return true;
}

// ---- Vampire Survivors login bridge ------------------------
//
// Hooking coherence's own login APIs installed cleanly but never fired --
// neither AuthClient.LoginWithSteam nor the CoherenceCloud static facade is on
// the path this game takes, and dump.cs shows signatures but not bodies, so
// there is no way to read off which callee it does use.
//
// Rather than guess a fourth time, replace the game's own single entry point:
//
//     VampireSurvivors.CoherenceLoginModule.Login(Action<bool> onComplete)
//
// and drive its own private completion handler with a guest LoginOperation.
// Whatever Login called internally becomes irrelevant, because it no longer
// runs. VS-specific by nature; it no-ops on any other game.
typedef void (*Fn_VSLogin)(void* onComplete, const MethodInfo* mi);
typedef void (*Fn_VSOnCompleteLogin)(void* loginOp, void* onComplete, const MethodInfo* mi);

static Fn_VSLogin        g_RealVSLogin      = nullptr;
static const MethodInfo* g_VSOnCompleteMi   = nullptr;

static void Hooked_VSLogin(void* onComplete, const MethodInfo* mi)
{
    if (g_CloudGuestMethod && g_CloudGuestMethod->methodPointer &&
        g_VSOnCompleteMi && g_VSOnCompleteMi->methodPointer)
    {
        LOG("[Coherence] CoherenceLoginModule.Login -> guest login");

        // default(CancellationToken) is a struct whose single reference field
        // is null, so a zero pointer-sized value is exactly "no token".
        Fn_CloudLoginAsGuest guest = (Fn_CloudLoginAsGuest)g_CloudGuestMethod->methodPointer;
        void* op = guest(nullptr, g_CloudGuestMethod);
        if (!op)
        {
            LOG("[Coherence] LoginAsGuest returned null -- falling back to the game's login");
            g_RealVSLogin(onComplete, mi);
            return;
        }

        ((Fn_VSOnCompleteLogin)g_VSOnCompleteMi->methodPointer)(op, onComplete, g_VSOnCompleteMi);
        return;
    }

    LOG("[Coherence] VS login bridge not fully resolved -- running the original");
    g_RealVSLogin(onComplete, mi);
}

// ---- install -----------------------------------------------
//
// AuthClient lives in Coherence.Runtime; RuntimeSettings is reachable from
// several images depending on build, so try each rather than pinning one.
static Il2CppClass* FindIn(const char* const* images, int n,
                           const char* ns, const char* cls)
{
    for (int i = 0; i < n; ++i)
        if (Il2CppClass* k = IL2CPP_FindClass(images[i], ns, cls))
            return k;
    return nullptr;
}

// Re-applied on every watcher tick, not written once.
//
// Writing the key a single time is a race we lose intermittently: the value is
// captured somewhere at construction, and RuntimeSettings.Instance has a
// setter, so the object we patched can be replaced afterwards. One run reached
// our project, the next went to the publisher's with the same code and no
// config change. Re-asserting it each tick makes the outcome deterministic
// regardless of which instance is current or when it appeared.
static bool g_bSettingsLogged = false;

static void ApplySettings()
{
    if (!g_ProjectId[0] && !g_RuntimeKey[0] && !g_bLocalMode) return;

    static const char* kSettingsImages2[] = { "Coherence.Toolkit", "Coherence.Runtime",
                                              "Coherence.Common", "Coherence" };
    Il2CppClass* rs = FindIn(kSettingsImages2, 4, "Coherence", "RuntimeSettings");
    if (!rs) return;

    const MethodInfo* inst = IL2CPP_FindMethod(rs, "get_Instance", 0);
    void* settings = (inst && inst->methodPointer)
        ? ((Fn_GetInstance)inst->methodPointer)(inst) : nullptr;
    if (!settings) return;

    // Only write when it differs, so this stays cheap and silent once correct.
    bool changed = false;
    if (g_RuntimeKey[0])
    {
        void* cur = *(void**)((uint8_t*)settings + 0x30);
        char probe[256] = {};
        if (cur && ReadIl2CppString(cur, probe, sizeof(probe)) &&
            strcmp(probe, g_RuntimeKey) != 0)
        {
            // In place, so the copy coherence cached at startup changes too.
            if (PatchStringInPlace(cur, g_RuntimeKey, "RuntimeKey"))
            {
                LOG("[Coherence] RuntimeKey patched in place (was \"%s\")", probe);
                changed = true;
            }
            else
            {
                changed |= WriteStringField(settings, 0x30, g_RuntimeKey, "RuntimeKey");
            }
        }
    }
    if (g_ProjectId[0])
    {
        void* cur = *(void**)((uint8_t*)settings + 0x80);
        char probe[256] = {};
        if (cur) IL2CPP_DescribeObject((Il2CppObject*)cur, probe, sizeof(probe));
        if (!strstr(probe, g_ProjectId))
            changed |= WriteStringField(settings, 0x80, g_ProjectId, "ProjectID");
    }
    if (g_bLocalMode)
    {
        volatile uint8_t* flag = (uint8_t*)settings + 0x74;
        if (*flag != 1)
        {
            *flag = 1;
            LOG("[Coherence] localDevelopmentMode -> 1");
            if (g_bLaunchRS && !g_bSettingsLogged) LaunchReplicationServer();
            changed = true;
        }
    }
    if (changed && !g_bSettingsLogged)
    {
        g_bSettingsLogged = true;
        LOG("[Coherence] settings applied; will be re-asserted if the game overwrites them");
    }
}

static bool TryInstall()
{
    if (!IL2CPP_TryInit()) return false;

    static const char* kAuthImages[]     = { "Coherence.Runtime", "Coherence", "Coherence.Cloud" };
    static const char* kSettingsImages[] = { "Coherence.Toolkit", "Coherence.Runtime",
                                             "Coherence.Common", "Coherence" };

    Il2CppClass* authCls = FindIn(kAuthImages, 3, "Coherence.Cloud", "AuthClient");
    if (!authCls) return false;   // types not loaded yet; the watcher retries

    const MethodInfo* steamMi = IL2CPP_FindMethod(authCls, "LoginWithSteam", 3);
    g_GuestMethod             = IL2CPP_FindMethod(authCls, "LoginAsGuest",   1);

    if (!steamMi || !steamMi->methodPointer)
    {
        LOG("[Coherence] AuthClient found but LoginWithSteam did not resolve");
        IL2CPP_DumpClassMethods(authCls, "AuthClient");
        return false;
    }
    if (!g_GuestMethod || !g_GuestMethod->methodPointer)
    {
        LOG("[Coherence] AuthClient.LoginAsGuest not found -- cannot redirect. "
            "Set [Coherence] ProjectId and use your own project instead.");
        IL2CPP_DumpClassMethods(authCls, "AuthClient");
        return false;
    }

    if (g_bForceGuest)
    {
        if (MH_CreateHook(steamMi->methodPointer, (LPVOID)&Hooked_LoginWithSteam,
                          (LPVOID*)&g_RealLoginWithSteam) == MH_OK &&
            MH_EnableHook(steamMi->methodPointer) == MH_OK)
            LOG("[Coherence] hooked AuthClient::LoginWithSteam -> LoginAsGuest");
        else
            LOG("[Coherence] FAILED to hook AuthClient::LoginWithSteam");
    }
    else
    {
        LOG("[Coherence] ForceGuestLogin=false -- leaving the Steam login alone");
    }

    // The static facade. Its LoginAsGuest has two overloads -- take the
    // 1-argument one (CancellationToken); the other wants LoginAsGuestOptions
    // we have no way to construct.
    if (g_bForceGuest)
    {
        if (Il2CppClass* cloudCls = FindIn(kAuthImages, 3, "Coherence.Cloud", "CoherenceCloud"))
        {
            const MethodInfo* cSteam = IL2CPP_FindMethod(cloudCls, "LoginWithSteam", 3);
            g_CloudGuestMethod        = IL2CPP_FindMethod(cloudCls, "LoginAsGuest",   1);

            if (cSteam && cSteam->methodPointer && g_CloudGuestMethod &&
                g_CloudGuestMethod->methodPointer &&
                MH_CreateHook(cSteam->methodPointer, (LPVOID)&Hooked_CloudLoginWithSteam,
                              (LPVOID*)&g_RealCloudLoginWithSteam) == MH_OK &&
                MH_EnableHook(cSteam->methodPointer) == MH_OK)
                LOG("[Coherence] hooked CoherenceCloud::LoginWithSteam -> LoginAsGuest");
            else
                LOG("[Coherence] could not hook CoherenceCloud::LoginWithSteam "
                    "(steam=%p guest=%p)", (void*)cSteam, (void*)g_CloudGuestMethod);
        }
        else
        {
            LOG("[Coherence] CoherenceCloud facade not found -- only the "
                "AuthClient hook is active");
        }
    }

    ApplySettings();

    // Game-specific login bridge. Absent on non-VS titles, which is fine.
    if (g_bForceGuest)
    {
        static const char* kVSImages[] = { "VampireSurvivors.Runtime", "Assembly-CSharp" };
        if (Il2CppClass* lm = FindIn(kVSImages, 2, "VampireSurvivors", "CoherenceLoginModule"))
        {
            const MethodInfo* loginMi = IL2CPP_FindMethod(lm, "Login", 1);
            g_VSOnCompleteMi          = IL2CPP_FindMethod(lm, "OnCompleteLogin", 2);

            if (loginMi && loginMi->methodPointer && g_VSOnCompleteMi &&
                g_CloudGuestMethod &&
                MH_CreateHook(loginMi->methodPointer, (LPVOID)&Hooked_VSLogin,
                              (LPVOID*)&g_RealVSLogin) == MH_OK &&
                MH_EnableHook(loginMi->methodPointer) == MH_OK)
                LOG("[Coherence] hooked CoherenceLoginModule::Login -> guest login");
            else
                LOG("[Coherence] VS bridge not installed (login=%p onComplete=%p guest=%p)",
                    (void*)loginMi, (void*)g_VSOnCompleteMi, (void*)g_CloudGuestMethod);
        }
    }

    return true;
}

static DWORD WINAPI WatcherProc(LPVOID)
{
    // coherence types are loaded lazily, well after our DllMain. Poll for ~2
    // minutes, then stop -- a game that has not touched coherence by then is
    // not going to.
    for (int i = 0; i < 600 && InterlockedCompareExchange(&g_bShutdown, 0, 0) == 0; ++i)
    {
        if (InterlockedCompareExchange(&g_bInstalled, 0, 0) == 0)
        {
            if (TryInstall()) InterlockedExchange(&g_bInstalled, 1);
        }
        else
        {
            // Hooks are permanent; the settings are not. Keep asserting them.
            ApplySettings();
        }
        Sleep(200);
    }
    if (InterlockedCompareExchange(&g_bInstalled, 0, 0) == 0)
        LOG("[Coherence] gave up: Coherence.Cloud.AuthClient never appeared "
            "(is this actually a coherence game?)");
    return 0;
}

extern "C" __declspec(dllexport) int __cdecl UCO_PluginInit(const UCO_PluginContext* ctx)
{
    if (!ctx) return 1;
    if (ctx->ApiVersion != UCO_PLUGIN_API_VERSION) return 2;
    g_Log = ctx->Log;

    LOG("[Coherence] coherence_universal init: AppId=%u ogAppId=%u",
        ctx->ForcedAppId, ctx->OriginalAppId);
    ReadIni();

    if (MH_Initialize() != MH_OK)
        LOG("[Coherence] MH_Initialize non-OK (already initialised?)");

    // Never install hooks from DllMain/init: MH_EnableHook suspends every
    // thread and deadlocks against the loader lock. The watcher does it.
    g_hWatcher = CreateThread(nullptr, 0, WatcherProc, nullptr, 0, nullptr);
    return 0;
}

extern "C" __declspec(dllexport) void __cdecl UCO_PluginShutdown(void)
{
    InterlockedExchange(&g_bShutdown, 1);
    if (g_hWatcher)
    {
        WaitForSingleObject(g_hWatcher, 1000);
        CloseHandle(g_hWatcher);
        g_hWatcher = nullptr;
    }
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    LOG("[Coherence] plugin shutdown");
}
