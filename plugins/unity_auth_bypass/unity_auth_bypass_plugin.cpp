// ============================================================
// UCOnline2 plugin -- Unity Gaming Services auth bypass
//
// Many modern Unity games gate multiplayer behind Unity
// Gaming Services (UGS) authentication. The typical flow is:
//
//   AuthenticationService.Instance.SignInWithSteamAsync(ticket)
//
// internally:
//   1. Fetch a Steam auth ticket via ISteamUser
//   2. POST { ticket, appId } to Unity's UGS backend
//   3. UGS asks Steam Web API "is this ticket entitled to
//      AppId X?"
//   4. If yes -> AuthenticationService.IsSignedIn = true,
//      PlayerInfo populated, game proceeds.
//   5. If no  -> SignInFailed event, game shows an error like
//      "Failed to get Steam account information" or "Not
//      signed into Unity Services" and refuses to start
//      multiplayer.
//
// With UCOnline2 spoofing AppId = 480 (Spacewar), step 3
// always fails: the ticket is for app 480 but UGS asks Steam
// about the real AppId.
//
// This plugin sidesteps the problem by hooking
// SignInWithSteamAsync and redirecting every call to
// SignInAnonymouslyAsync. UGS still creates a real, valid
// player session -- it just isn't tied to a Steam account.
// The game's "IsSignedIn" / "PlayerInfo" checks pass and the
// multiplayer flow proceeds normally.
//
// MinHook is statically linked into this DLL.
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

// ============================================================
// Plugin-local state
// ============================================================
static UCO_LogFn g_Log              = nullptr;
static volatile LONG g_bShutdown    = 0;
static HANDLE    g_hWatcherThread   = nullptr;

#define LOG(...) do { if (g_Log) g_Log(__VA_ARGS__); } while (0)

extern "C" void IL2CPP_Log(const char* fmt, ...)
{
    if (!g_Log) return;
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    _vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, ap);
    va_end(ap);
    g_Log("%s", buf);
}

// ============================================================
// Hook: AuthenticationServiceInternal.SignInWithSteamAsync
//
// Generic signature covering every overload we've seen in the
// wild (1 / 2 / 3 explicit args after `this`):
//
//   Task SignInWithSteamAsync(string ticket)
//   Task SignInWithSteamAsync(string ticket, SignInOptions options)
//   Task SignInWithSteamAsync(string ticket, string identity, SignInOptions options)
//
// We declare 3 explicit args after `this`. Extras passed by
// the game land in R8/R9/stack and are simply ignored, which
// is safe under x64 __fastcall. We then forward to
// SignInAnonymouslyAsync, returning its Task object so the
// caller's `await` resolves normally with a real signed-in
// state.
// ============================================================
typedef void* (__fastcall *Fn_SignInTask)(void* pThis, void* a1, void* a2, void* a3);

static Fn_SignInTask g_pfnSignInAnonymous       = nullptr;
static Fn_SignInTask g_pfnOrigSignInWithSteam   = nullptr;

static void* __fastcall Hooked_SignInWithSteam(void* pThis, void* ticket, void* a2, void* a3)
{
    LOG("[Auth] SignInWithSteamAsync intercepted -> SignInAnonymouslyAsync (this=%p)", pThis);
    if (!g_pfnSignInAnonymous)
    {
        LOG("[Auth] WARNING: SignInAnonymouslyAsync not resolved, falling through");
        return g_pfnOrigSignInWithSteam(pThis, ticket, a2, a3);
    }
    // SignInAnonymouslyAsync is typically:
    //   Task SignInAnonymouslyAsync()
    //   Task SignInAnonymouslyAsync(SignInOptions options)
    // Passing nullptr for the options arg works for both.
    return g_pfnSignInAnonymous(pThis, nullptr, nullptr, nullptr);
}

static bool InstallIl2CppHooks()
{
    if (!IL2CPP_IsReady()) return false;

    // Resolve SignInAnonymouslyAsync first; if it's missing
    // there's no point hooking SignInWithSteamAsync.
    g_pfnSignInAnonymous = (Fn_SignInTask)IL2CPP_FindMethodPtr(
        nullptr, "Unity.Services.Authentication",
        "AuthenticationServiceInternal", "SignInAnonymouslyAsync", -1);
    if (!g_pfnSignInAnonymous)
    {
        LOG("[Auth] could not find SignInAnonymouslyAsync -- bypass disabled");
        return false;
    }
    LOG("[Auth] SignInAnonymouslyAsync at %p", g_pfnSignInAnonymous);

    void* steamFn = IL2CPP_FindMethodPtr(
        nullptr, "Unity.Services.Authentication",
        "AuthenticationServiceInternal", "SignInWithSteamAsync", -1);
    if (!steamFn)
    {
        LOG("[Auth] could not find SignInWithSteamAsync -- game probably "
            "doesn't use UGS Steam sign-in; nothing to bypass");
        return false;
    }

    if (MH_CreateHook(steamFn, (void*)&Hooked_SignInWithSteam,
                      (void**)&g_pfnOrigSignInWithSteam) != MH_OK ||
        MH_EnableHook(steamFn) != MH_OK)
    {
        LOG("[Auth] failed to install SignInWithSteamAsync hook at %p", steamFn);
        return false;
    }
    LOG("[Auth] SignInWithSteamAsync hook installed at %p", steamFn);

    return true;
}

static DWORD WINAPI WatcherProc(LPVOID)
{
    for (int i = 0; i < 600 && InterlockedCompareExchange(&g_bShutdown, 0, 0) == 0; ++i)
    {
        if (IL2CPP_TryInit())
        {
            InstallIl2CppHooks();
            return 0;
        }
        Sleep(200);
    }
    if (!InterlockedCompareExchange(&g_bShutdown, 0, 0))
        LOG("[Auth] GameAssembly.dll never resolved -- giving up on IL2CPP hooks");
    return 0;
}

// ============================================================
// Plugin ABI entry points
// ============================================================
extern "C" __declspec(dllexport) int __cdecl UCO_PluginInit(const UCO_PluginContext* ctx)
{
    if (!ctx) return 1;
    if (ctx->ApiVersion != UCO_PLUGIN_API_VERSION) return 2;

    g_Log = ctx->Log;
    LOG("[Auth] plugin v1 init: AppId=%u ogAppId=%u",
        ctx->ForcedAppId, ctx->OriginalAppId);

    if (MH_Initialize() != MH_OK)
    {
        // Already initialized by another plugin -- that's fine.
        LOG("[Auth] MH_Initialize returned non-OK (already inited?)");
    }

    g_hWatcherThread = CreateThread(nullptr, 0, WatcherProc, nullptr, 0, nullptr);
    return 0;
}

extern "C" __declspec(dllexport) void __cdecl UCO_PluginShutdown(void)
{
    InterlockedExchange(&g_bShutdown, 1);
    if (g_hWatcherThread)
    {
        WaitForSingleObject(g_hWatcherThread, 1000);
        CloseHandle(g_hWatcherThread);
        g_hWatcherThread = nullptr;
    }
    // Don't MH_DisableHook(ALL) here -- another plugin may share
    // the MinHook instance. Disable only what we own.
    LOG("[Auth] plugin shutdown");
}

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) { return TRUE; }
