// ============================================================
// UCOnline2 plugin ABI (v1)
//
// A "plugin" is a Windows DLL placed in the folder named by
// `PluginsFolder=` in union-crax.ini. Plugins exist to hold
// per-game customization (EOS hooks, ticket synthesis, IL2CPP
// patches, mock backends, etc.) that does NOT belong in the
// generic Steam-wrapper core.
//
// Lifecycle
//   1. DLL_PROCESS_ATTACH on the host (UCOnline2)
//   2. Host calls LoadLibraryExA on every *.dll in PluginsFolder
//      (alphabetical order). Plugin's DllMain runs here. Note
//      that ISteam interfaces are NOT yet available.
//   3. Game calls SteamAPI_Init -> host populates g_ClientCtx.
//   4. Host calls UCO_PluginInit(ctx) on every plugin that
//      exports it. This is where plugins should install hooks
//      and spin up watchers (e.g. for EOSSDK loading later).
//   5. Game runs.
//   6. Game calls SteamAPI_Shutdown (or host detaches) -> host
//      calls UCO_PluginShutdown() on every plugin in REVERSE
//      load order, then FreeLibrary.
//
// A plugin that exports neither symbol is still loaded (so
// classic side-effect-from-DllMain plugins keep working) but
// gets no init/shutdown calls.
//
// Stability: ApiVersion bumps when the context struct layout
// changes. Plugins MUST check ctx->ApiVersion and return
// non-zero from UCO_PluginInit if the host is too new/old.
// ============================================================
#pragma once

#include <stdint.h>

// Forward declarations -- plugins include the Steam SDK
// themselves if they want to call methods through these
// pointers. We keep this header SDK-independent so a plugin
// can be built without dragging in UCOnline2's SDK copy.
class ISteamClient;
class ISteamUser;
class ISteamUtils;
class ISteamApps;
class ISteamFriends;
class ISteamMatchmaking;

#define UCO_PLUGIN_API_VERSION 1

typedef void (*UCO_LogFn)(const char* fmt, ...);

// Called by the host for every received Steam callback before it
// is dispatched to the game's CCallback. Patchers may mutate the
// buffer in-place (e.g. force m_eResult = OK on auth callbacks).
typedef void (*UCO_CallbackPatcherFn)(uint8_t* buf, uint32_t cbBuf);

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UCO_PluginContext
{
    // Always-present header
    uint32_t  ApiVersion;        // == UCO_PLUGIN_API_VERSION
    uint32_t  StructSize;        // == sizeof(UCO_PluginContext)

    // AppId config (from union-crax.ini)
    uint32_t  ForcedAppId;       // [Settings]AppId   (Steam-side spoof, e.g. 480)
    uint32_t  OriginalAppId;     // [Settings]ogAppId (real game AppId, 0 if unset)

    // Resolved Steam interfaces. Will be non-null when
    // UCO_PluginInit is called (SteamAPI_Init has succeeded).
    ISteamClient*       pSteamClient;
    ISteamUser*         pSteamUser;
    ISteamUtils*        pSteamUtils;
    ISteamApps*         pSteamApps;
    ISteamFriends*      pSteamFriends;
    ISteamMatchmaking*  pSteamMatchmaking;

    // Local Steam handles
    int32_t   HSteamUser;
    int32_t   HSteamPipe;

    // ---- Host services ---------------------------------------------------

    // Append a line to %TEMP%\uc_online2.log. printf-style.
    UCO_LogFn  Log;

    // Register a function that will be called on every Steam
    // callback whose iCallback matches. Patchers run before the
    // callback reaches the game. Multiple patchers may register
    // for the same iCallback; they run in registration order.
    void (*RegisterCallbackPatcher)(int iCallback, UCO_CallbackPatcherFn fn);
} UCO_PluginContext;

// Plugin exports (both optional):
//
//   int  UCO_PluginInit(const UCO_PluginContext* ctx);
//   void UCO_PluginShutdown(void);
//
// UCO_PluginInit should return 0 on success. Non-zero values
// are logged by the host but otherwise non-fatal.
typedef int  (__cdecl *UCO_PluginInit_Fn)(const UCO_PluginContext* ctx);
typedef void (__cdecl *UCO_PluginShutdown_Fn)(void);

#ifdef __cplusplus
}
#endif
