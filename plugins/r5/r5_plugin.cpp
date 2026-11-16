// ============================================================
// UCOnline2 plugin -- r5 (Windrose)
//
// Points Windrose's coop gateway at a host you control, so the client's
// authorisation succeeds and multiplayer can proceed.
//
// THE PROBLEM
//   Windrose's coop backend authorises the client before anything online is
//   allowed -- joining included:
//       POST https://r5coopapigateway-<region>-release.windrose.support
//            /api/v1/Auth/AuthenticateClientBySteam
//   and it asks STEAM to validate the supplied ticket against Windrose's real
//   AppId. A ticket minted under the spoofed AppId can never pass that, so the
//   gateway answers
//       424 {"errorcode":102,"errordesc":"Ticket for other app"}
//   and every online path stays shut. That check is server-side and there is
//   nothing to forge client-side.
//
// THE FIX
//   Don't fight the check -- don't reach it. UR5CaHttpClient::Init sets the
//   gateway host for each request; rewrite it there to a host you control,
//   which answers the auth route with a synthetic "IsOk" reply. Nothing is ever
//   sent to the publisher.
//
//   The host must have a valid certificate, because the client still speaks
//   ordinary HTTPS to it. A Cloudflare Worker is ideal: no TLS to terminate, no
//   socket redirect, no DNS hooking. See tools/eos-app-site/worker.js.
//
// WHY THE HOSTNAME AND NOT THE URL
//   The field is the HOSTNAME ONLY. The client composes
//       https:// + <host> + :443 + /api/v1 + <route>
//   around it, so writing a full URL produces
//       https://https://your.host/api/v1:443/api/v1/Auth/...
//   Write just the host, and keep it SHORTER than the original
//   ("r5coopapigateway-kr-release.windrose.support", 44 chars) -- the rewrite is
//   in place, into the game's own buffer.
//
// WHY HERE AND NOT AT REPLY TIME
//   The host is re-selected per attempt from the region pinger, so rewriting a
//   cached copy after a failed reply is too late: the retry just picks another
//   region. Init is where each request's endpoint is actually set.
//
// BUILD-SPECIFIC
//   R5CaHttpInitRVA must match the exact executable. To re-derive it on a new
//   build, find the ANSI literal "UR5CaHttpClient::Init" (UE logs __FUNCTION__),
//   take the function that references it, and ignore the two lambda variants --
//   the real one is ~1534 bytes and its literal has no ::<lambda_N> suffix.
// ============================================================
#include "../../include/uco_plugin.h"

#include "r5_gateway.h"

static HANDLE g_hWatcher = nullptr;
static volatile LONG g_bShutdown = 0;

static DWORD WINAPI WatcherProc(LPVOID)
{
    // Just off the loader lock. The gateway is not touched until the game makes
    // its first request, which is far later than this.
    InstallHook();
    return 0;
}

// ------------------------------------------------------------
// Plugin entry points
// ------------------------------------------------------------
extern "C" __declspec(dllexport) int UCO_PluginInit(const UCO_PluginContext* ctx)
{
    if (ctx && ctx->ApiVersion == UCO_PLUGIN_API_VERSION && ctx->Log)
        g_Log = ctx->Log;
    LOG("[R5] plugin init (host logger attached).");
    return 0;
}

extern "C" __declspec(dllexport) void UCO_PluginShutdown(void)
{
    InterlockedExchange(&g_bShutdown, 1);
    LOG("[R5] plugin shutdown.");
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        LoadConfig();
        if (!g_Cfg.bValid) {
            LOG("[R5] not configured ([R5] CaHttpInitRVA / GatewayHostOverride) -- idle. "
                "This plugin only ever acts on the build its RVA was measured against.");
            return TRUE;
        }
        LOG("[R5] config: CaHttpInitRVA=0x%llX GatewayHostOverride=%s",
            (unsigned long long)g_Cfg.CaHttpInitRVA, g_Cfg.GatewayHostOverride);
        g_hWatcher = CreateThread(nullptr, 0, WatcherProc, nullptr, 0, nullptr);
    }
    return TRUE;
}
