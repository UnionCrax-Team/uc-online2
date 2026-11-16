#include <Windows.h>
#include <Shlwapi.h>
#include <DbgHelp.h>
#include <new.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>

#define STEAM_API_EXPORTS

#include "include/sdk/steam_api.h"
#include "include/sdk/steamclientpublic.h"
#include "include/sdk/steam_gameserver.h"
#include "include/sdk/steamtypes.h"

S_API ISteamClient* g_pSteamClientGameServer = nullptr;

#include <vector>

#include "include/registfuncs.h"
#include "include/callback_dispatcher.h"
#include "include/uco_plugin.h"
#include "include/globals.h"
#include "include/uc_loader.h"
#include "include/dump_handler.h"
#include "include/MinHook.h"

void InstallEarlyAppIdHook(ISteamUtils* pUtils);

#include "include/api/inventory_emu.h"
#include "include/api/remote_storage_emu.h"
#include "include/api/api_callbacks.h"
#include "include/api/api_client.h"
#include "include/api/api_interfaces.h"
#include "include/api/api_interfaces_v.h"
#include "include/api/api_gameserver.h"
#include "include/api/api_breakpad.h"
#include "include/api/api_shutdown.h"
#include "include/api/api_factory.h"
#include "include/api/api_flat.h"

// ============================================================
// Global variable definitions
// ============================================================

HMODULE g_ClientModule = nullptr;
HSteamPipe g_ClientPipe = 0;
HSteamUser g_ClientUser = 0;
ISteamClient* g_pSteamClient = nullptr;
ISteamClient* g_pSteamClientSafe = nullptr;
ISteamUtils* g_pUtilsForCallbacks = nullptr;
ISteamController* g_pControllerForCallbacks = nullptr;
ISteamInput* g_pInputForCallbacks = nullptr;
CSteamAPIContext g_ClientCtx;
bool g_bClientReady = false;
SRWLOCK g_CtxLock;

HMODULE g_ServerModule = nullptr;
HSteamPipe g_ServerPipe = 0;
HSteamUser g_ServerUser = 0;
ISteamClient* g_ServerClient = nullptr;
ISteamClient* g_pServerClient = nullptr;
ISteamClient* g_pSteamClientGameServer_Latest = nullptr;
ISteamGameServer* g_pGameServer = nullptr;
ISteamUtils* g_pServerUtils = nullptr;
CSteamGameServerAPIContext g_ServerCtx;
bool g_bServerReady = false;
EServerMode g_ServerMode = eServerModeInvalid;

bool g_bUsingBreakpad = false;
bool g_bFullDumps = false;
void* g_BreakpadCtx = nullptr;
PFNPreMinidumpCallback g_BreakpadCallback = nullptr;
char g_BreakpadVer[64] = { 0 };
char g_BreakpadTimestamp[64] = { 0 };
uint32 g_BreakpadAppID = 0;
uint64 g_BreakpadSteamID = 0;

// Default ON: route dispatch through CCallbackDispatcher::DispatchFrameSafe,
// whose try/catch (with /EHa) contains a faulting game callback instead of
// letting it kill the process. Games can still toggle this via
// SteamAPI_SetTryCatchCallbacks(). This is a safety net, not a substitute for
// the dispatcher's locking/lifetime fixes -- it just keeps a bad callback from
// being fatal (e.g. Forever Skies crashing from UE's online thread).
bool g_bTryCatch = true;
bool g_bVerboseLog = false;
int g_DispatchMode = 0;
char g_InstallPath[MAX_PATH] = { 0 };
bool g_bHaveInstallPath = false;
SRWLOCK g_CallbackLock;
uint32 g_ForcedAppId = 480;
uint32 g_OriginalAppId = 0;
uint32 g_SteamNetworkingAppId = 0;
bool   g_bRealAppIdEnv = false;   // [Settings] RealAppIdEnv: expose ogAppId in the SteamAppId env for a game's startup AppId check
// [Settings] EmulateTicket -- also gates auth-session-ticket emulation
// (see the ISteamUser hooks further down).
static bool g_bEmulateAuthTicket = false;

Fn_CreateInterface g_pfnCreateInterface = nullptr;
Fn_ReleaseThreadLocal g_pfnReleaseThreadLocal = nullptr;
Fn_IsKnownInterface g_pfnIsKnownInterface = nullptr;
Fn_NotifyMissing g_pfnNotifyMissing = nullptr;
Fn_BreakpadInit g_pfnBreakpadInit = nullptr;
Fn_BreakpadSetAppID g_pfnBreakpadSetAppID = nullptr;
Fn_BreakpadSetSteamID g_pfnBreakpadSetSteamID = nullptr;
Fn_BreakpadSetComment g_pfnBreakpadSetComment = nullptr;
Fn_BreakpadWriteDump g_pfnBreakpadWriteDump = nullptr;

// Starts at 1, and must never be 0 again. A game's interface accessor holds a
// static { pFn, counter, ptr } block that starts life all-zero, and
// SteamInternal_ContextInit only runs pFn when the block's counter differs from
// this one. Starting at 0 means a virgin block can compare equal on the very
// first call, so pFn never runs, ptr stays null, and the accessor hands the game
// a null interface -- which it then uses without checking. Rivals of Aether
// (GameMaker, all Steam access through SteamInternal_ContextInit) died exactly
// that way: a null deref in game code with every interface resolving fine.
uintp g_CtxCounter = 1;

// See [Settings] SteamClientVersion -- what the exported SteamClient()
// accessor returns for games built against an older Steamworks SDK.
ISteamClient* g_pSteamClientLegacy = nullptr;
char g_LegacyClientVersion[64] = { 0 };
int g_GameSteamUserVersion = 0;

// Forward declarations for SteamStub
static bool g_bSteamStubEnabled = false;
static void SteamStub_Init();

// ============================================================
// SteamInternal_ContextInit
// ============================================================

S_API void* S_CALLTYPE SteamInternal_ContextInit(void* pData)
{
	UCOLOG_HOT("[UCOnline2] SteamInternal_ContextInit");
	if (!pData) return nullptr;
	// Diagnostic: the accessor path is invisible in a normal log, so report
	// the first handful of context blocks and what they ended up holding.
	static LONG s_diagCount = 0;
	const bool diag = (InterlockedIncrement(&s_diagCount) <= 24);

	void** pArr = (void**)pData;
	uintp* pCounter = (uintp*)&pArr[1];
	char* pBase = (char*)pData;
	#if defined(_M_IX86)
		if (*pCounter != 0 && *pCounter == g_CtxCounter) return pBase + 8;
		AcquireSRWLockExclusive(&g_CtxLock);
		if (*pCounter == 0 || *pCounter != g_CtxCounter) { void(*pFn)(void*) = (void(*)(void*))pArr[0]; pFn(pBase + 8); *pCounter = g_CtxCounter; }
		ReleaseSRWLockExclusive(&g_CtxLock);
		if (diag) UCOLOG("[UCOnline2][diag] ContextInit pFn=%p -> iface=%p%s",
			pArr[0], *(void**)(pBase + 8), *(void**)(pBase + 8) ? "" : "   <<<< NULL");
		return pBase + 8;
	#elif defined(_M_AMD64)
		if (*pCounter != 0 && *pCounter == g_CtxCounter) return pBase + 16;
		AcquireSRWLockExclusive(&g_CtxLock);
		if (*pCounter == 0 || *pCounter != g_CtxCounter) { void(*pFn)(void*) = (void(*)(void*))pArr[0]; pFn(pBase + 16); *pCounter = g_CtxCounter; }
		ReleaseSRWLockExclusive(&g_CtxLock);
		return pBase + 16;
	#endif
}

// ============================================================
// Logging
// ============================================================


static void UCOLogImpl(const char* fmt, va_list args)
{
	char msg[2048] = { 0 };
	_vsnprintf_s(msg, sizeof(msg), _TRUNCATE, fmt, args);

	char logPath[MAX_PATH] = { 0 };
	DWORD len = GetTempPathA(MAX_PATH, logPath);
	if (len == 0 || len > (MAX_PATH - 25)) return;

	if (!PathAppendA(logPath, "uc_online2.log")) return;

	FILE* f = nullptr;
	if (fopen_s(&f, logPath, "ab") != 0 || !f) return;

	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);

	fprintf(f, "[%04u-%02u-%02u %02u:%02u:%02u.%03u] %s",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, msg);

	size_t msgLen = strlen(msg);
	if (msgLen == 0 || msg[msgLen - 1] != '\n')
		fputs("\r\n", f);

	fclose(f);
}

void UCOLOG(const char* fmt, ...)
{
	if (!fmt) return;
	va_list args;
	va_start(args, fmt);
	UCOLogImpl(fmt, args);
	va_end(args);
}

void UCOColor(WORD color, const char* text)
{
	(void)color;
    if (text && text[0])
        UCOLOG("%s", text);
}

// Low-level steamclient exports used by older Steam integrations.  FS25's
// working shim imports these directly, so the ISteamUser vtable hooks above
// cannot observe the host-side game-server handshake.  These hooks are
// deliberately pass-through: they provide visibility into the call path while
// preserving the real Steam client's return values and validation behavior.
using Fn_SteamGetGSHandle = void* (S_CALLTYPE*)(HSteamUser, HSteamPipe);
using Fn_SteamGSSendSteam3UserConnect = bool (S_CALLTYPE*)(void*, uint64, uint32, const void*, uint32);
using Fn_SteamInitiateGameConnection = int (S_CALLTYPE*)(HSteamUser, HSteamPipe, void*, int, uint64, int, uint32, uint16, bool);

static Fn_SteamGetGSHandle g_OriginalSteamGetGSHandle = nullptr;
static Fn_SteamGSSendSteam3UserConnect g_OriginalSteamGSSendSteam3UserConnect = nullptr;
static Fn_SteamInitiateGameConnection g_OriginalSteamInitiateGameConnection = nullptr;

static void* S_CALLTYPE Hooked_SteamGetGSHandle(HSteamUser hUser, HSteamPipe hPipe)
{
    void* handle = g_OriginalSteamGetGSHandle
        ? g_OriginalSteamGetGSHandle(hUser, hPipe)
        : nullptr;
    UCOLOG("[UCOnline2] steamclient Steam_GetGSHandle user=%u pipe=%u -> %p",
        (uint32)hUser, (uint32)hPipe, handle);
    return handle;
}

static bool S_CALLTYPE Hooked_SteamGSSendSteam3UserConnect(void* pGSHandle,
                                                            uint64 steamId,
                                                            uint32 ipPublic,
                                                            const void* pCookie,
                                                            uint32 cbCookie)
{
    const bool result = g_OriginalSteamGSSendSteam3UserConnect
        ? g_OriginalSteamGSSendSteam3UserConnect(pGSHandle, steamId, ipPublic, pCookie, cbCookie)
        : false;
    UCOLOG("[UCOnline2] steamclient Steam_GSSendSteam3UserConnect gs=%p steamid=%llu cookie=%u -> %s",
        pGSHandle, (unsigned long long)steamId, cbCookie, result ? "true" : "false");
    return result;
}

static int S_CALLTYPE Hooked_SteamInitiateGameConnection(HSteamUser hUser,
                                                          HSteamPipe hPipe,
                                                          void* pBlob,
                                                          int cbMaxBlob,
                                                          uint64 steamId,
                                                          int gameAppId,
                                                          uint32 ipServer,
                                                          uint16 portServer,
                                                          bool secure)
{
    const int result = g_OriginalSteamInitiateGameConnection
        ? g_OriginalSteamInitiateGameConnection(hUser, hPipe, pBlob, cbMaxBlob, steamId,
                                                 gameAppId, ipServer, portServer, secure)
        : 0;
    UCOLOG("[UCOnline2] steamclient Steam_InitiateGameConnection user=%u pipe=%u appid=%d server=%u:%u secure=%d -> %d bytes",
        (uint32)hUser, (uint32)hPipe, gameAppId, ipServer, (uint32)portServer,
        secure ? 1 : 0, result);
    return result;
}

static void UcoInstallSteamClientAuthHooks(HMODULE hSteamClient)
{
    if (!hSteamClient)
        return;

    static HMODULE s_hookedModule = nullptr;
    if (s_hookedModule == hSteamClient)
        return;

    MH_Initialize();

    struct Item { const char* name; void* detour; void** original; };
    Item items[] = {
        { "Steam_GetGSHandle", (void*)&Hooked_SteamGetGSHandle, (void**)&g_OriginalSteamGetGSHandle },
        { "Steam_GSSendSteam3UserConnect", (void*)&Hooked_SteamGSSendSteam3UserConnect, (void**)&g_OriginalSteamGSSendSteam3UserConnect },
        { "Steam_InitiateGameConnection", (void*)&Hooked_SteamInitiateGameConnection, (void**)&g_OriginalSteamInitiateGameConnection },
    };

    int installed = 0;
    for (const Item& item : items)
    {
        void* target = (void*)GetProcAddress(hSteamClient, item.name);
        if (!target)
        {
            UCOLOG("[UCOnline2] steamclient export missing: %s", item.name);
            continue;
        }

        MH_STATUS st = MH_CreateHook(target, item.detour, item.original);
        if (st == MH_ERROR_ALREADY_CREATED)
            st = MH_EnableHook(target);
        else if (st == MH_OK)
            st = MH_EnableHook(target);

        if (st == MH_OK)
            ++installed;
        else
            UCOLOG("[UCOnline2] steamclient hook failed: %s status=%d", item.name, st);
    }

    s_hookedModule = hSteamClient;
    UCOLOG("[UCOnline2] steamclient auth tracing: %d/3 hooks installed", installed);
}

// ============================================================
// InitSteamClient // I seriously broke this later on in the
//				   // releases, I am SO SORRY ya'll!!
// ============================================================

void* InitSteamClient(HMODULE* phMod, bool bLocal, const char* iface)
{
	g_pUtilsForCallbacks = nullptr;
	g_pControllerForCallbacks = nullptr;
	g_pInputForCallbacks = nullptr;

	if (!phMod || !iface) return nullptr;

	*phMod = nullptr;

	char dllPath[MAX_PATH] = { 0 };
	const char* installDir = SteamAPI_GetSteamInstallPath();

	if (_stricmp(installDir, "UCOnline2_InvalidPath") == 0)
	{
		if (!bLocal) return nullptr;
	}
	else
	{
		#if defined(_M_IX86)
			_snprintf_s(dllPath, MAX_PATH, _TRUNCATE, "%s\\steamclient.dll", installDir);
		#elif defined(_M_AMD64)
			_snprintf_s(dllPath, MAX_PATH, _TRUNCATE, "%s\\steamclient64.dll", installDir);
		#endif
	}

	if (SteamAPI_IsSteamRunning())
	{
		*phMod = LoadLibraryExA(dllPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (!*phMod)
			UCOColor(0, "[UCOnline2] Failed to load steamclient DLL");
	}
	else
	{
		UCOColor(0, "[UCOnline2] Steam is not running");
	}

	if (!*phMod)
	{
		if (!bLocal)
		{
			UCOColor(0, "[UCOnline2] No Steam instance and bLocal is false");
			return nullptr;
		}

		#if defined(_M_IX86)
			*phMod = LoadLibraryW(L"steamclient.dll");
		#elif defined(_M_AMD64)
			*phMod = LoadLibraryW(L"steamclient64.dll");
		#endif

		if (!*phMod)
		{
			UCOColor(0, "[UCOnline2] Cannot find steamclient DLL");
			return nullptr;
		}
	}

	g_pfnCreateInterface = (Fn_CreateInterface)GetProcAddress(*phMod, "CreateInterface");

	if (g_pfnCreateInterface)
	{
		UcoInstallSteamClientAuthHooks(*phMod);
		g_pSteamClientSafe = (ISteamClient*)g_pfnCreateInterface("SteamClient023", nullptr);
		if (g_LegacyClientVersion[0])
		{
			g_pSteamClientLegacy = (ISteamClient*)g_pfnCreateInterface(g_LegacyClientVersion, nullptr);
			UCOLOG("[UCOnline2] SteamClient() accessor pinned to %s -> %p",
				g_LegacyClientVersion, g_pSteamClientLegacy);
		}
		g_pfnReleaseThreadLocal = (Fn_ReleaseThreadLocal)GetProcAddress(*phMod, "Steam_ReleaseThreadLocalMemory");
		g_CtxCounter++;

		return g_pfnCreateInterface(iface, nullptr);
	}
	else
	{
		UCOColor(0, "[UCOnline2] CreateInterface not found in steamclient");
		FreeLibrary(*phMod);
		*phMod = nullptr;
	}

	return nullptr;
}


// ============================================================
// LoadGameOverlay
// ============================================================

// Small helper so the overlay log line reads clearly when something is unset.
static const char* GetEnvOrDash(const char* name)
{
	static char bufs[4][MAX_PATH];
	static int  turn = 0;
	char* b = bufs[turn++ & 3];
	const DWORD n = GetEnvironmentVariableA(name, b, MAX_PATH);
	if (n == 0 || n >= MAX_PATH) { b[0] = '-'; b[1] = '\0'; }
	return b;
}

static void LoadGameOverlay()
{
#if defined(_WIN32)
	#if defined(_M_IX86)
		HMODULE hOverlay = GetModuleHandleW(L"GameOverlayRenderer.dll");
	#elif defined(_M_AMD64)
		HMODULE hOverlay = GetModuleHandleW(L"GameOverlayRenderer64.dll");
	#endif

	if (g_ForcedAppId != 769 && !hOverlay)
	{
		const char* installPath = SteamAPI_GetSteamInstallPath();
		if (_stricmp(installPath, "UCOnline2_InvalidPath") != 0)
		{
			char overlayPath[MAX_PATH] = { 0 };
			#if defined(_M_IX86)
				_snprintf_s(overlayPath, MAX_PATH, _TRUNCATE, "%s\\GameOverlayRenderer.dll", installPath);
			#elif defined(_M_AMD64)
				_snprintf_s(overlayPath, MAX_PATH, _TRUNCATE, "%s\\GameOverlayRenderer64.dll", installPath);
			#endif
			// Ensure steamclient is loaded first so the overlay has its dependencies
			// satisfied and can hook into the process properly.
			char steamClientPath[MAX_PATH] = { 0 };
			#if defined(_M_IX86)
				_snprintf_s(steamClientPath, MAX_PATH, _TRUNCATE, "%s\\steamclient.dll", installPath);
			#elif defined(_M_AMD64)
				_snprintf_s(steamClientPath, MAX_PATH, _TRUNCATE, "%s\\steamclient64.dll", installPath);
			#endif
			HMODULE hSteam = nullptr;
			// Try to get an existing handle first
			#if defined(_M_IX86)
				hSteam = GetModuleHandleW(L"steamclient.dll");
			#elif defined(_M_AMD64)
				hSteam = GetModuleHandleW(L"steamclient64.dll");
			#endif
			if (!hSteam)
			{
				hSteam = LoadLibraryExA(steamClientPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
				if (hSteam)
				{
					#ifdef _DEBUG
					UCOLOG("[UCOnline2] Preloaded steamclient for overlay: %s", steamClientPath);
					#endif
				}
				else
				{
					#ifdef _DEBUG
					UCOLOG("[UCOnline2] Failed to preload steamclient: %s (error %lu)", steamClientPath, GetLastError());
					#endif
				}
			}

			HMODULE hLoaded = LoadLibraryExA(overlayPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (hLoaded)
			{
				// Not debug-only: when the overlay misbehaves this line, and
				// what follows it, is the whole diagnosis.
				UCOLOG("[UCOnline2] Loaded game overlay: %s", overlayPath);
				UCOLOG("[UCOnline2]   env: SteamGameId=%s SteamClientLaunch=%s SteamPath=%s user=%s",
					GetEnvOrDash("SteamGameId"), GetEnvOrDash("SteamClientLaunch"),
					GetEnvOrDash("SteamPath"), GetEnvOrDash("SteamAppUser"));
			}
			else
			{
				UCOLOG("[UCOnline2] Failed to load game overlay: %s (error %lu)", overlayPath, GetLastError());
			}
		}
	}
#endif // _WIN32
}

// ============================================================
// DllMain
// ============================================================

// File-scope so the SteamAPI_Init path in api_client.h can call
// InitPlugins() on it, and DLL_PROCESS_DETACH can call
// ShutdownPlugins().
CDLLLoader s_PluginLoader;

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		UCOLOG("[UCOnline2] DllMain -> DLL_PROCESS_ATTACH");

		s_PluginLoader.ReadConfig();
		g_ForcedAppId = s_PluginLoader.GetAppId();
		g_OriginalAppId = s_PluginLoader.GetOgAppId();
		if (s_PluginLoader.GetSDREnabled())
		{
			g_SteamNetworkingAppId = g_OriginalAppId;
			if (g_SteamNetworkingAppId == 0)
				UCOLOG("[UCOnline2] SDR=yes ignored: ogAppId is required");
		}

		g_bRealAppIdEnv = s_PluginLoader.GetRealAppIdEnv();
		if (g_bRealAppIdEnv && g_OriginalAppId == 0)
		{
			UCOLOG("[UCOnline2] RealAppIdEnv=yes ignored: ogAppId is required");
			g_bRealAppIdEnv = false;
		}

		s_PluginLoader.GetClientVersion(g_LegacyClientVersion, sizeof(g_LegacyClientVersion));
		if (g_LegacyClientVersion[0])
			UCOLOG("[UCOnline2] SteamClient() accessor will be pinned to %s", g_LegacyClientVersion);

		SetAppIDEnv();
		WriteAppIDFile();

		// Load the game overlay early to ensure it can hook into graphics APIs.
		// [Settings] LoadOverlay=no opts out: the renderer hooks D3D/DXGI, and a
		// game that objects to that has no other way to keep it out.
		if (s_PluginLoader.GetLoadOverlay())
			LoadGameOverlay();
		else
			UCOLOG("[UCOnline2] LoadOverlay=no -- not loading GameOverlayRenderer");

		// [Settings] RealAppIdEnv: some games read the SteamAppId ENV at startup and
		// quit if it isn't their real AppId (Valheim's SteamManager: "Invalid APPID"
		// -> Application.Quit(), BEFORE it ever calls SteamAPI_Init). Expose ogAppId in
		// the env now for that early read; SteamAPI_Init re-stamps the forced (spoof)
		// id before it connects to Steam, so ownership still rides the free AppId. Done
		// AFTER the overlay load so the overlay/relay context stays on the forced id.
		if (g_bRealAppIdEnv)
		{
			SetSteamAppEnvironment(g_OriginalAppId);
			UCOLOG("[UCOnline2] RealAppIdEnv: SteamAppId env -> %u for the game's startup "
				"check (the Steam connection still uses %u)", g_OriginalAppId, g_ForcedAppId);
		}

		char dllPath[MAX_PATH] = { 0 };
		DWORD len = GetModuleFileNameA(hModule, dllPath, sizeof(dllPath));

		if (len == 0)
			return FALSE;

		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			return FALSE;

		UCOLOG("[UCOnline2] DLL Path: %s", dllPath);

		// The actual likelihood of this being used or even working is low, as it wouldn't even work if it were named anything else.
		// However, I'm positive that compiling without this will cause it to scream at me and I no no like that. It make me maaaad.
		#if defined(_M_IX86)
			if (!StrStrIA(dllPath, "steam_api.dll"))
				UCOLOG("[UCOnline2] Warning: not named steam_api.dll");
		#elif defined(_M_AMD64)
			if (!StrStrIA(dllPath, "steam_api64.dll"))
				UCOLOG("[UCOnline2] Warning: not named steam_api64.dll");
		#endif

		UCOLOG("[UCOnline2] PID: %lu", GetCurrentProcessId());
		UCOLOG("[UCOnline2] Thread: %lu", GetCurrentThreadId());

		InitializeSRWLock(&g_CtxLock);
		InitializeSRWLock(&g_CallbackLock);

		// Read before anything chatty runs, so the hot-path traces honour it.
		g_bVerboseLog = s_PluginLoader.GetVerboseLog();
		if (g_bVerboseLog)
			UCOLOG("[UCOnline2] VerboseLog enabled -- per-frame callback traces will be logged");

		#ifdef _DEBUG
		g_pDumpHandler = new CDumpHandler();
		#endif

		s_PluginLoader.LoadPlugins();

		UCOLOG("[UCOnline2] %zu plugin(s) loaded", s_PluginLoader.LoadedCount());

		g_bSteamStubEnabled = s_PluginLoader.GetSteamStubEnabled();
		if (g_bSteamStubEnabled)
		{
			SteamStub_Init();
		}

		// Local ISteamInventory emulation so in-game purchases/grants succeed
		// and persist (real Steam has no items for the spoofed AppId).
		if (s_PluginLoader.GetInventoryAutoGrant())
			UcoInvEmu::Init(s_PluginLoader.GetIniPath(), s_PluginLoader.GetOgAppId());

		// Local Steam Cloud: keep saves out of the spoofed AppId's shared
		// 480/remote bucket, where UCO2 games collide with each other.
		if (s_PluginLoader.GetLocalSaves())
			UcoCloudEmu::Init(s_PluginLoader.GetIniPath());

		// DLC ownership: [DLC] UnlockAll + named entries, plus the legacy
		// [Settings] UnlockDLC list. Loaded before any game code can ask.
		UcoDlcStore::Load(s_PluginLoader.GetIniPath(), s_PluginLoader.GetOgAppId());
		UCOLOG("[UCOnline2] DLC store: UnlockAll=%d, %d entr%s",
			UcoDlcStore::UnlockAll() ? 1 : 0, UcoDlcStore::Count(),
			UcoDlcStore::Count() == 1 ? "y" : "ies");

		const bool passthroughTicket = s_PluginLoader.GetPassthroughTicketEnabled();
		const bool emulateTicket = s_PluginLoader.GetEmulateTicketEnabled();
		g_bEmulateAuthTicket = emulateTicket && !passthroughTicket;
		if (passthroughTicket) {
			UCOLOG("[UCOnline2] PassthroughTicket enabled -- using genuine Steam tickets%s",
				emulateTicket ? " (overrides EmulateTicket)" : "");
		}
		if (g_bEmulateAuthTicket) {
			uint32 emulatedAppId = s_PluginLoader.GetOgAppId();
			if (emulatedAppId == 0) emulatedAppId = s_PluginLoader.GetAppId();
			CSteamUserStub::SetEmulatedApp(emulatedAppId);
		}
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		UCOLOG("[UCOnline2] DllMain -> DLL_PROCESS_DETACH");
		s_PluginLoader.ShutdownPlugins();
		if (g_bSteamStubEnabled)
		{
			MH_DisableHook(reinterpret_cast<LPVOID*>(GetTickCount));
			MH_Uninitialize();
		}
	}

	return TRUE;
}

// ============================================================
// CCallbackDispatcher
// ============================================================

static bool s_bDispatcherReady = false;

// ============================================================
// Callback patcher registry
//
// Plugins (see include/uco_plugin.h) can register a callback
// patcher for a specific iCallback. Patchers run -- in
// registration order -- on every matching callback before it
// is dispatched to the game's CCallback.
//
// The registry replaces the previous hard-coded auth callback
// patching. Per-game auth behavior now lives in a plugin.
// ============================================================
struct CallbackPatcherEntry
{
    int                    iCallback;
    UCO_CallbackPatcherFn  fn;
};
static std::vector<CallbackPatcherEntry> g_CallbackPatchers;
static SRWLOCK g_CallbackPatcherLock = SRWLOCK_INIT;

void UCO_RegisterCallbackPatcher(int iCallback, UCO_CallbackPatcherFn fn)
{
    if (!fn) return;
    AcquireSRWLockExclusive(&g_CallbackPatcherLock);
    g_CallbackPatchers.push_back({ iCallback, fn });
    ReleaseSRWLockExclusive(&g_CallbackPatcherLock);
    UCOLOG("[UCOnline2] Callback patcher registered for iCallback=%d", iCallback);
}

// Steam reports stats and achievement callbacks against the app the SESSION is
// running as -- our spoofed AppId -- but the game compares them against the id
// it got from GetAppID, which we answer with ogAppId. The two disagree, the
// game decides the callback is for some other game, and drops it.
//
// Rivals of Aether sat printing "Steam Stats not ready." forever because of
// this, and gates its online menu on stats being ready, so multiplayer was
// unreachable while every interface looked healthy.
//
// These three all begin with a CGameID, whose low 32 bits are the AppId, so the
// rewrite is the same for each. Only the low half is touched: the high bits
// carry the id TYPE (app / mod / shortcut) and must survive.
static void PatchCallbackGameId(int iCallback, uint8* pBuf, uint32 cbBuf)
{
    if (!g_OriginalAppId || g_OriginalAppId == g_ForcedAppId) return;
    if (!pBuf || cbBuf < sizeof(uint64)) return;

    switch (iCallback)
    {
        case UserStatsReceived_t::k_iCallback:        // 1101
        case UserStatsStored_t::k_iCallback:          // 1102
        case UserAchievementStored_t::k_iCallback:    // 1103
            break;
        default:
            return;
    }

    uint64 id = 0;
    memcpy(&id, pBuf, sizeof(id));
    if ((uint32)id != g_ForcedAppId) return;         // already right, or not ours

    const uint64 fixed = (id & ~0xFFFFFFFFULL) | (uint64)g_OriginalAppId;
    memcpy(pBuf, &fixed, sizeof(fixed));
    UCOLOG("[UCOnline2] callback %d: GameID %u -> %u (so the game accepts it)",
        iCallback, g_ForcedAppId, g_OriginalAppId);
}

static void RunCallbackPatchers(int iCallback, uint8* pBuf, uint32 cbBuf)
{
    if (!pBuf || cbBuf == 0) return;
    PatchCallbackGameId(iCallback, pBuf, cbBuf);
    AcquireSRWLockShared(&g_CallbackPatcherLock);
    // Iterate by index in case a patcher misbehaves and re-enters.
    size_t n = g_CallbackPatchers.size();
    for (size_t i = 0; i < n; i++)
    {
        const auto& e = g_CallbackPatchers[i];
        if (e.iCallback == iCallback)
            e.fn(pBuf, cbBuf);
    }
    ReleaseSRWLockShared(&g_CallbackPatcherLock);
}

CCallbackDispatcher::CCallbackDispatcher()
{
	UCOColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY, "[UCOnline2] CCallbackDispatcher constructed\r\n");

	// Must exist before any thread can reach the maps.
	InitializeCriticalSection(&m_MapLock);

	m_pfnBGetCallback = nullptr;
	m_pfnFreeLastCallback = nullptr;
	m_pfnGetAPICallResult = nullptr;
	m_CurrentUser = 0;
	m_ManualCbId = 0;
	m_ManualCbSize = 0;
	m_ManualSyntheticData.clear();
	m_bManualSyntheticActive = false;
	m_bProcessing = false;
	m_CallbackMap.clear();
	m_CallResultMap.clear();
	m_BufferMap.clear();
	s_bDispatcherReady = true;
}

CCallbackDispatcher::~CCallbackDispatcher()
{
	UCOColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY, "[UCOnline2] CCallbackDispatcher destroyed\r\n");

	s_bDispatcherReady = false;
	m_pfnBGetCallback = nullptr;
	m_pfnFreeLastCallback = nullptr;
	m_pfnGetAPICallResult = nullptr;
	m_CurrentUser = 0;
	m_ManualCbId = 0;
	m_ManualCbSize = 0;
	m_ManualSyntheticData.clear();
	m_bManualSyntheticActive = false;
	m_bProcessing = false;
	{
		CDispatcherLock lock(&m_MapLock);
		m_CallbackMap.clear();
		m_CallResultMap.clear();
		m_BufferMap.clear();
	}
	DeleteCriticalSection(&m_MapLock);
}

void CCallbackDispatcher::Shutdown()
{
	UCOColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY, "[UCOnline2] CCallbackDispatcher shutdown\r\n");

	s_bDispatcherReady = false;
	m_pfnBGetCallback = nullptr;
	m_pfnFreeLastCallback = nullptr;
	m_pfnGetAPICallResult = nullptr;
	m_CurrentUser = 0;
	m_ManualCbId = 0;
	m_ManualCbSize = 0;
	m_ManualSyntheticData.clear();
	m_bManualSyntheticActive = false;
	m_bProcessing = false;
	{
		CDispatcherLock lock(&m_MapLock);
		m_CallbackMap.clear();
		m_CallResultMap.clear();
		m_BufferMap.clear();
	}
}

void CCallbackDispatcher::ExecuteCallResult(HSteamPipe hPipe, SteamAPICall_t hCall, CCallbackBase* pCb)
{
	if (!pCb)
		return;

	UCOLOG_HOT("[UCOnline2] ExecuteCallResult -> call=%lld cb=%d size=%d\r\n", hCall, pCb->GetICallback(), pCb->GetCallbackSizeBytes());

	BYTE* pBuffer = new BYTE[pCb->GetCallbackSizeBytes()]();
	bool bFailed = false;

	bool bResult;
	if (UcoInvEmu::IsOurCall(hCall))
		bResult = UcoInvEmu::GetAPICallResult(hCall, pBuffer, pCb->GetCallbackSizeBytes(), pCb->GetICallback(), &bFailed);
	else if (UcoCloudEmu::IsOurCall(hCall))
		bResult = UcoCloudEmu::GetAPICallResult(hCall, pBuffer, pCb->GetCallbackSizeBytes(), pCb->GetICallback(), &bFailed);
	else
		bResult = m_pfnGetAPICallResult(hPipe, hCall, pBuffer, pCb->GetCallbackSizeBytes(), pCb->GetICallback(), &bFailed);

	if (bResult && !bFailed)
	{
		size_t countBefore = m_CallbackMap.size();

		pCb->Run(pBuffer, bFailed, hCall);

		if (countBefore != m_CallbackMap.size())
		{
			UCOColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY, "[UCOnline2] Callback map changed during CallResult execution\r\n");

			m_CallbackMap.erase(LobbyEnter_t::k_iCallback);
			pCb->Run(pBuffer);
		}

		m_BufferMap.insert(std::make_pair(hCall, pBuffer));
	}
	else
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] GetAPICallResult failed\r\n");
		delete[] pBuffer;
	}

	// Retire the one-shot registration. The SDK makes the DISPATCHER own this:
	//     CCallResult<T,P>::Run(...) { m_hAPICall = k_uAPICallInvalid; // caller unregisters for us
	// (see include/sdk/steam_api_internal.h). Because Run() clears the handle,
	// the game's ~CCallResult() -> Cancel() sees an invalid handle and never
	// calls SteamAPI_UnregisterCallResult. If we don't drop the entry here it
	// lingers in m_CallResultMap and becomes a DANGLING pointer the moment the
	// game frees the object -- the next SteamAPICallCompleted_t match loop then
	// calls its virtuals (GetICallback / GetCallbackSizeBytes) on freed memory.
	{
		CDispatcherLock lock(&m_MapLock);
		m_CallResultMap.erase(hCall);
	}
}

// ------------------------------------------------------------
// Synthetic callbacks.
//
// Emulating an API call is only half the job: games that call an async Steam
// function register its response callback and block on it. Real Steam will
// never send a response for a call we answered ourselves, so we have to deliver
// it. This is the only path by which a callback we invented reaches the game.
//
// Delivery happens on whichever thread drives SteamAPI_RunCallbacks, under the
// same lock and the same matching rules as a real callback, so a game cannot
// tell the difference.
// ------------------------------------------------------------
void CCallbackDispatcher::PostCallback(int iCallback, const void* pvData, size_t cubData,
                                       HSteamUser user, bool bServer, unsigned delayMs)
{
	CDispatcherLock lock(&m_MapLock);
	UcoPendingCallback pending;
	pending.iCallback = iCallback;
	pending.user      = user;
	pending.bServer   = bServer;
	pending.dueTick   = GetTickCount64() + delayMs;
	if (pvData && cubData)
		pending.data.assign((const uint8_t*)pvData, (const uint8_t*)pvData + cubData);
	m_Synthetic.push_back(pending);
	UCOLOG("[UCOnline2] queued synthetic callback -> %d size=%zu delay=%ums",
		iCallback, cubData, delayMs);
}

bool CCallbackDispatcher::TryGetSyntheticForManual(bool bServer, CallbackMsg_t* pMsg)
{
	if (!pMsg)
		return false;

	CDispatcherLock lock(&m_MapLock);
	if (m_bManualSyntheticActive || m_Synthetic.empty())
		return false;

	const ULONGLONG now = GetTickCount64();
	for (size_t i = 0; i < m_Synthetic.size(); ++i)
	{
		UcoPendingCallback& pending = m_Synthetic[i];
		if (pending.bServer != bServer || now < pending.dueTick)
			continue;

		UcoPendingCallback fire = pending;
		m_Synthetic.erase(m_Synthetic.begin() + (ptrdiff_t)i);

		m_ManualSyntheticData = fire.data;
		if (!m_ManualSyntheticData.empty())
			RunCallbackPatchers(fire.iCallback, m_ManualSyntheticData.data(), (uint32)m_ManualSyntheticData.size());

		pMsg->m_hSteamUser = fire.user;
		pMsg->m_iCallback = fire.iCallback;
		pMsg->m_pubParam = m_ManualSyntheticData.empty() ? nullptr : m_ManualSyntheticData.data();
		pMsg->m_cubParam = (int)m_ManualSyntheticData.size();

		m_bManualSyntheticActive = true;
		UCOLOG("[UCOnline2] manual synthetic callback -> %d size=%d",
			pMsg->m_iCallback, pMsg->m_cubParam);
		return true;
	}

	return false;
}

bool CCallbackDispatcher::FreeManualSynthetic()
{
	CDispatcherLock lock(&m_MapLock);
	if (!m_bManualSyntheticActive)
		return false;

	m_ManualSyntheticData.clear();
	m_bManualSyntheticActive = false;
	return true;
}

// Deliver one callback while holding m_MapLock. Most callback types retain the
// historical first-registration behavior. Some games register a new scoped
// networking handler when a host or join operation begins while older managed
// wrappers remain registered. The newest 1221 handler owns the active operation.
bool CCallbackDispatcher::DispatchToTarget(int iCallback, void* pvData, uint32 cubData,
                                           HSteamUser user, bool bServer)
{
	CCallbackBase* target = nullptr;
	// Request-scoped networking and ticket listeners are commonly registered
	// alongside an older long-lived wrapper. The listener created for the active
	// request is the newest one; delivering only to the stale first registration
	// leaves the caller waiting even though Steam returned a successful result.
	const bool preferNewest =
		iCallback == SteamNetConnectionStatusChangedCallback_t::k_iCallback ||
		iCallback == GetAuthSessionTicketResponse_t::k_iCallback ||
		iCallback == GetTicketForWebApiResponse_t::k_iCallback;
	auto range = m_CallbackMap.equal_range(iCallback);
	for (auto it = range.first; it != range.second; ++it)
	{
		CCallbackBase* pCb = it->second;
		if (!pCb)
			continue;

		const bool registered =
			(pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsRegistered) != 0;
		const bool gameServer =
			(pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsGameServer) != 0;
		if (registered && ((bServer && gameServer && user == g_ServerUser) ||
			(!bServer && !gameServer && user == g_ClientUser)))
		{
			if (!target || preferNewest)
				target = pCb;
		}
	}

	if (!target)
		return false;

	target->Run(pvData);

	return true;
}

void CCallbackDispatcher::DrainSynthetic(bool bServer)
{
	// Snapshot the due entries under the lock, then dispatch them still holding
	// it -- same discipline as DispatchFrame, so a callback cannot be destroyed
	// mid-Run() by a concurrent unregister.
	CDispatcherLock lock(&m_MapLock);
	if (m_Synthetic.empty())
		return;

	const ULONGLONG now = GetTickCount64();
	for (size_t i = 0; i < m_Synthetic.size(); )
	{
		UcoPendingCallback& p = m_Synthetic[i];
		if (p.bServer != bServer || now < p.dueTick)
		{
			++i;
			continue;
		}

		UcoPendingCallback fire = p;                 // copy: the vector is edited below
		m_Synthetic.erase(m_Synthetic.begin() + (ptrdiff_t)i);

		const bool ran = DispatchToTarget(fire.iCallback, fire.data.empty() ? nullptr : fire.data.data(),
		                                  (uint32)fire.data.size(), fire.user, fire.bServer);
		UCOLOG("[UCOnline2] synthetic callback -> %d %s",
			fire.iCallback, ran ? "delivered" : "DROPPED (nothing registered for it)");
	}
}

void CCallbackDispatcher::DispatchFrame(HSteamPipe hPipe, bool bServer)
{
	// Before the early return below: Steam having nothing pending must not stop
	// our own queued callbacks from being delivered.
	DrainSynthetic(bServer);

	if (!m_pfnBGetCallback || !m_pfnFreeLastCallback || !m_pfnGetAPICallResult)
		return;

	CallbackMsg_t msg = { 0 };
	if (!m_pfnBGetCallback(hPipe, &msg))
		return;

	UCOLOG_HOT("[UCOnline2] Callback received -> %d\r\n", msg.m_iCallback);
	m_CurrentUser = msg.m_hSteamUser;

	// The lock is held for the whole dispatch, INCLUDING across Run().
	// That is deliberate and is what keeps the callback object alive: a game's
	// CCallback destructor calls SteamAPI_UnregisterCallback -> Remove(), which
	// needs this same lock, so an object cannot be destroyed while we are inside
	// its Run(). Releasing the lock first would reopen a use-after-free window.
	// The CRITICAL_SECTION is recursive, so a callback that registers or
	// unregisters others from inside Run() (same thread) still works.
	CDispatcherLock lock(&m_MapLock);

	if (msg.m_iCallback == SteamAPICallCompleted_t::k_iCallback && msg.m_cubParam == sizeof(SteamAPICallCompleted_t))
	{
		SteamAPICallCompleted_t* pCompleted = (SteamAPICallCompleted_t*)msg.m_pubParam;

		// Snapshot first: ExecuteCallResult re-enters and edits m_CallResultMap
		// (it retires the fired entry), which would invalidate a live iterator.
		std::vector<std::pair<SteamAPICall_t, CCallbackBase*> > matches;
		for (auto it = m_CallResultMap.begin(); it != m_CallResultMap.end(); ++it)
		{
			if (!it->second)
				continue;

			if (pCompleted->m_hAsyncCall == it->first &&
				pCompleted->m_iCallback == it->second->GetICallback() &&
				pCompleted->m_cubParam == (uint32)it->second->GetCallbackSizeBytes())
			{
				matches.push_back(*it);
			}
		}

		for (size_t i = 0; i < matches.size(); ++i)
			ExecuteCallResult(hPipe, matches[i].first, matches[i].second);
	}
	else
	{
		bool bSkip = false;
		if (!bServer)
		{
			if (msg.m_iCallback == HTML_NeedsPaint_t::k_iCallback &&
				msg.m_cubParam == sizeof(HTML_NeedsPaint_t))
			{
				HTML_NeedsPaint_t* pPaint = (HTML_NeedsPaint_t*)msg.m_pubParam;
				bSkip = pPaint->unWide == 1 || pPaint->unTall == 1;
			}
			RunCallbackPatchers(msg.m_iCallback, msg.m_pubParam, msg.m_cubParam);
		}

		if (!bSkip)
			DispatchToTarget(msg.m_iCallback, msg.m_pubParam, msg.m_cubParam,
				msg.m_hSteamUser, bServer);
	}

	UCOLOG_HOT("[UCOnline2] Freeing callback -> %d\r\n", msg.m_iCallback);
	SecureZeroMemory(msg.m_pubParam, msg.m_cubParam);
	m_pfnFreeLastCallback(hPipe);
}

void CCallbackDispatcher::DispatchFrameSafe(HSteamPipe hPipe, bool bServer)
{
	try { DrainSynthetic(bServer); } catch (...) { }

	if (!m_pfnBGetCallback || !m_pfnFreeLastCallback || !m_pfnGetAPICallResult)
		return;

	CallbackMsg_t msg = { 0 };
	if (!m_pfnBGetCallback(hPipe, &msg))
		return;

	// The callback MUST be freed whether or not dispatch throws. If we skipped
	// the free on the exception path, BGetCallback would keep handing back the
	// same message forever -- turning one bad callback into an endless fault
	// loop (a hang instead of a crash).
	bool bDispatched = false;

	try
	{
		UCOLOG_HOT("[UCOnline2] Callback (safe) -> %d\r\n", msg.m_iCallback);
		m_CurrentUser = msg.m_hSteamUser;

		// Same locking/snapshot discipline as DispatchFrame -- see the comment
		// on m_MapLock. Held across Run() so a concurrent unregister+destroy
		// cannot free the callback out from under us.
		CDispatcherLock lock(&m_MapLock);

		if (msg.m_iCallback == SteamAPICallCompleted_t::k_iCallback && msg.m_cubParam == sizeof(SteamAPICallCompleted_t))
		{
			SteamAPICallCompleted_t* pCompleted = (SteamAPICallCompleted_t*)msg.m_pubParam;

			std::vector<std::pair<SteamAPICall_t, CCallbackBase*> > matches;
			for (auto it = m_CallResultMap.begin(); it != m_CallResultMap.end(); ++it)
			{
				if (!it->second)
					continue;

				if (pCompleted->m_hAsyncCall == it->first &&
					pCompleted->m_iCallback == it->second->GetICallback() &&
					pCompleted->m_cubParam == (uint32)it->second->GetCallbackSizeBytes())
				{
					matches.push_back(*it);
				}
			}

			for (size_t i = 0; i < matches.size(); ++i)
				ExecuteCallResult(hPipe, matches[i].first, matches[i].second);
		}
		else
		{
			if (!bServer)
				RunCallbackPatchers(msg.m_iCallback, msg.m_pubParam, msg.m_cubParam);
			DispatchToTarget(msg.m_iCallback, msg.m_pubParam, msg.m_cubParam,
				msg.m_hSteamUser, bServer);
		}

		bDispatched = true;
	}
	catch (...)
	{
		// Contained: a faulting game callback no longer kills the process.
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Exception in callback dispatch -- callback skipped\r\n");
	}

	UCOLOG_HOT("[UCOnline2] Freeing callback (safe) -> %d (dispatched=%d)\r\n", msg.m_iCallback, (int)bDispatched);
	SecureZeroMemory(msg.m_pubParam, msg.m_cubParam);
	m_pfnFreeLastCallback(hPipe);
}

void CCallbackDispatcher::Add(CCallbackBase* pCb, int iCallback)
{
	if (!pCb)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Add: null callback ptr\r\n");
		return;
	}

	// Do NOT call pCb->GetCallbackSizeBytes() here. Real Steam does not invoke a
	// callback's virtuals at registration time -- it just links it in and reads
	// the size later, at dispatch. Some wrappers route that virtual straight into
	// managed code: Facepunch.Steamworks' FileDetailsResult_t faults inside its
	// managed StructSize/OnGetSize when called re-entrantly from within the
	// RegisterCallbacks loop, crashing the game (e.g. Risk of Rain 2). Just
	// register the callback; the size is read safely when the callback fires.
	UCOLOG("[UCOnline2] Register callback -> %d flags=%d\r\n", iCallback, pCb->m_nCallbackFlags);

	pCb->m_nCallbackFlags |= pCb->k_ECallbackFlagsRegistered;
	pCb->m_iCallback = iCallback;

	CDispatcherLock lock(&m_MapLock);
	m_CallbackMap.insert(std::make_pair(iCallback, pCb));
}

void CCallbackDispatcher::AddCallResult(CCallbackBase* pCb, SteamAPICall_t hCall)
{
	if (!pCb || hCall == k_uAPICallInvalid)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] AddCallResult: invalid args\r\n");
		return;
	}

	if (pCb->GetICallback() == 0 || pCb->GetCallbackSizeBytes() == 0)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] AddCallResult: zero callback\r\n");
		return;
	}

	UCOLOG("[UCOnline2] Register call result -> %lld cb=%d size=%d\r\n", hCall, pCb->GetICallback(), pCb->GetCallbackSizeBytes());

	pCb->m_nCallbackFlags |= pCb->k_ECallbackFlagsRegistered;

	CDispatcherLock lock(&m_MapLock);
	auto existing = m_CallResultMap.find(hCall);
	if (existing == m_CallResultMap.end())
	{
		m_CallResultMap.insert(std::make_pair(hCall, pCb));
	}
	else
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] AddCallResult: already registered\r\n");
		existing->second = pCb;
	}
}

void CCallbackDispatcher::Remove(CCallbackBase* pCb)
{
	if (!pCb)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Remove: null callback ptr\r\n");
		return;
	}

	// Deliberately NO early-return on the registered flag. This runs from the
	// game's CCallback destructor, so bailing out would leave a pointer to an
	// object that is about to be freed sitting in the map -- a later dispatch
	// would then call Run()/virtuals on freed memory. The entry must go
	// regardless of the flag's state.
	CDispatcherLock lock(&m_MapLock);

	UCOLOG("[UCOnline2] Unregister callback -> %d flags=%d\r\n", pCb->GetICallback(), pCb->m_nCallbackFlags);
	pCb->m_nCallbackFlags &= ~pCb->k_ECallbackFlagsRegistered;

	// Erase EVERY entry for this callback (it can be registered under more than
	// one key); any straggler is a dangling pointer waiting to crash us.
	for (auto it = m_CallbackMap.begin(); it != m_CallbackMap.end(); )
	{
		if (it->second == pCb)
			it = m_CallbackMap.erase(it);
		else
			++it;
	}

	// It may also still be sitting in the call-result map, e.g. the game frees
	// a CCallResult whose API call never completed.
	for (auto it = m_CallResultMap.begin(); it != m_CallResultMap.end(); )
	{
		if (it->second == pCb)
			it = m_CallResultMap.erase(it);
		else
			++it;
	}
}

void CCallbackDispatcher::RemoveCallResult(CCallbackBase* pCb, SteamAPICall_t hCall)
{
	if (!pCb || hCall == k_uAPICallInvalid)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] RemoveCallResult: invalid args\r\n");
		return;
	}

	// As in Remove(): never early-return on the flag, or the map keeps a pointer
	// to an object the game is about to destroy.
	UCOLOG("[UCOnline2] Unregister call result -> %lld cb=%d flags=%d\r\n", hCall, pCb->GetICallback(), pCb->m_nCallbackFlags);
	pCb->m_nCallbackFlags &= ~pCb->k_ECallbackFlagsRegistered;

	CDispatcherLock lock(&m_MapLock);

	// This erase was missing entirely: the call-result entry was never removed,
	// so m_CallResultMap kept the pointer forever and it dangled as soon as the
	// game freed the CCallResult.
	m_CallResultMap.erase(hCall);

	// The same object may be registered under other handles too.
	for (auto it = m_CallResultMap.begin(); it != m_CallResultMap.end(); )
	{
		if (it->second == pCb)
			it = m_CallResultMap.erase(it);
		else
			++it;
	}

	auto bufIt = m_BufferMap.find(hCall);
	if (bufIt != m_BufferMap.end())
	{
		UCOColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY, "[UCOnline2] Freeing call result buffer\r\n");
		delete[] bufIt->second;
		m_BufferMap.erase(bufIt);
	}
}

CCallbackDispatcher* GetDispatcher()
{
	static CCallbackDispatcher instance;
	return &instance;
}

// ============================================================
// CDumpHandler (_DEBUG only)
// ============================================================

#ifdef _DEBUG

#include <DbgHelp.h>

CDumpHandler::CDumpHandler()
{
	m_Comment.clear();
	m_hDbgHelp = nullptr;
	m_pfnWriteDump = nullptr;
	m_bReady = false;

	m_hDbgHelp = LoadLibraryExW(L"DbgHelp.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (!m_hDbgHelp)
		return;

	FARPROC fp = GetProcAddress(m_hDbgHelp, "MiniDumpWriteDump");
	if (!fp)
	{
		FreeLibrary(m_hDbgHelp);
		m_hDbgHelp = nullptr;
		return;
	}

	m_pfnWriteDump = (Fn_MiniDumpWriteDump)fp;

	InitializeSRWLock(&m_Lock);
	m_bReady = true;
}

CDumpHandler::~CDumpHandler()
{
	m_pfnWriteDump = nullptr;
	m_Comment.clear();

	if (m_hDbgHelp)
	{
		FreeLibrary(m_hDbgHelp);
		m_hDbgHelp = nullptr;
	}

	m_bReady = false;
}

bool CDumpHandler::IsReady()
{
	return m_bReady;
}

void CDumpHandler::SetComment(const wchar_t* comment)
{
	m_Comment.assign(comment);
}

size_t CDumpHandler::GetCommentByteSize()
{
	if (m_Comment.empty()) return 0;
	return m_Comment.length() * sizeof(wchar_t);
}

const wchar_t* CDumpHandler::GetComment()
{
	return m_Comment.c_str();
}

void CDumpHandler::ClearComment()
{
	m_Comment.clear();
}

void CDumpHandler::WriteDump(DWORD exceptionCode, _EXCEPTION_POINTERS* pExceptionInfo)
{
	if (!m_hDbgHelp || !m_pfnWriteDump)
		return;

	HANDLE hProcess = GetCurrentProcess();
	DWORD pid = GetCurrentProcessId();

	AcquireSRWLockExclusive(&m_Lock);

	MINIDUMP_EXCEPTION_INFORMATION excInfo = { 0 };
	excInfo.ThreadId = GetCurrentThreadId();
	excInfo.ExceptionPointers = pExceptionInfo;
	excInfo.ClientPointers = FALSE;

	MINIDUMP_TYPE dumpType = (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithHandleData | MiniDumpWithUnloadedModules |
		MiniDumpWithProcessThreadData | MiniDumpWithFullMemoryInfo | MiniDumpWithThreadInfo);

	if (GetCommentByteSize() != 0)
	{
		MINIDUMP_USER_STREAM streams[2] = { 0 };

		streams[0].Type = CommentStreamW;
		streams[0].BufferSize = (ULONG)GetCommentByteSize();
		streams[0].Buffer = (LPVOID)GetComment();

		MINIDUMP_EXCEPTION_STREAM excStream = { 0 };
		excStream.ExceptionRecord.ExceptionCode = exceptionCode;

		streams[1].Type = ExceptionStream;
		streams[1].BufferSize = sizeof(MINIDUMP_EXCEPTION_STREAM);
		streams[1].Buffer = &excStream;

		MINIDUMP_USER_STREAM_INFORMATION streamInfo = { 0 };
		streamInfo.UserStreamCount = 2;
		streamInfo.UserStreamArray = streams;

		m_pfnWriteDump(hProcess, pid, nullptr, dumpType, &excInfo, &streamInfo, nullptr);
	}
	else
	{
		MINIDUMP_USER_STREAM streams[1] = { 0 };

		MINIDUMP_EXCEPTION_STREAM excStream = { 0 };
		excStream.ExceptionRecord.ExceptionCode = exceptionCode;

		streams[0].Type = ExceptionStream;
		streams[0].BufferSize = sizeof(MINIDUMP_EXCEPTION_STREAM);
		streams[0].Buffer = &excStream;

		MINIDUMP_USER_STREAM_INFORMATION streamInfo = { 0 };
		streamInfo.UserStreamCount = 1;
		streamInfo.UserStreamArray = streams;

		m_pfnWriteDump(hProcess, pid, nullptr, dumpType, &excInfo, &streamInfo, nullptr);
	}

	ReleaseSRWLockExclusive(&m_Lock);
}
#endif

/**
 *  SteamStubbed, credits to DenuvoSanctuary for the original code for this.
 *  Originally written in Rust, rewritten here in C++ to integrate it.
 */
#include "include/steamstub_hook.h"

// The bypass mechanics live in the shared header (also used by the early-load
// proxy, version.dll). Route its log lines through UCOLOG.
static void SteamStub_LogCb(const char* m) { UCOLOG("[UCOnline2] %s", m); }

// ============================================================
// Generic Steam-side spoof hooks
//
// These are kept in core because they apply uniformly to any
// ogAppId-spoofed setup -- they don't carry per-game logic.
// Game-specific behaviors (auth ticket synthesis, EOS bypass,
// etc.) live in plugins; see include/uco_plugin.h and the
// reference Outbound plugin under plugins/outbound/.
// ============================================================

typedef uint32 (S_CALLTYPE *Fn_GetAppID)(void* pThis);
typedef bool   (S_CALLTYPE *Fn_BIsSubscribedApp)(void* pThis, AppId_t appID);
typedef bool   (S_CALLTYPE *Fn_BIsDlcInstalled)(void* pThis, AppId_t appID);
typedef uint32 (S_CALLTYPE *Fn_GetEarliestPurchase)(void* pThis, AppId_t appID);
typedef int    (S_CALLTYPE *Fn_GetDLCCount)(void* pThis);
typedef bool   (S_CALLTYPE *Fn_BGetDLCDataByIndex)(void* pThis, int iDLC, AppId_t* pAppID,
                                                   bool* pbAvailable, char* pchName, int cchName);

static Fn_GetAppID            g_pfnOriginalGetAppID           = nullptr;
static void*                  g_pHookedGetAppIDTarget          = nullptr;
static Fn_BIsSubscribedApp    g_pfnOriginalBIsSubscribedApp   = nullptr;
static Fn_BIsDlcInstalled     g_pfnOriginalBIsDlcInstalled    = nullptr;
static Fn_GetEarliestPurchase g_pfnOriginalGetEarliestPurchase = nullptr;
static Fn_GetDLCCount         g_pfnOriginalGetDLCCount        = nullptr;
static Fn_BGetDLCDataByIndex  g_pfnOriginalBGetDLCDataByIndex = nullptr;
static bool                g_bGetAppIDLoggedFirst        = false;
static bool                g_bSubscribedLoggedFirst      = false;
static bool                g_bDlcLoggedFirst             = false;
static bool                g_bDlcCountLoggedFirst        = false;

// Many games gate multiplayer behind "do you actually own this AppId?"
// via ISteamApps::BIsSubscribedApp(GetAppID()). Real Steam answers
// false because the user owns Spacewar (480), not the real AppId.
// Return true for the ogAppId.
static bool S_CALLTYPE Hooked_BIsSubscribedApp(void* pThis, AppId_t appID)
{
    bool original = g_pfnOriginalBIsSubscribedApp(pThis, appID);
    if (original) return true;

    // Real Steam answers for the SPOOFED AppId, so it says false both for the
    // game itself and for any DLC. Defer to the store for both.
    if (g_OriginalAppId != 0 && appID == g_OriginalAppId)
    {
        if (!g_bSubscribedLoggedFirst)
        {
            UCOLOG("[UCOnline2] BIsSubscribedApp(%u) hook returning true (Steam says false)", appID);
            g_bSubscribedLoggedFirst = true;
        }
        return true;
    }
    return UcoDlcStore::IsOwned((uint32)appID);
}

// The DLC surface below is the reason unlocking used to be unreliable: only
// BIsSubscribedApp was hooked, so a game asking "do I own <id>?" was answered,
// while a game ENUMERATING its DLC through GetDLCCount/BGetDLCDataByIndex got
// real Steam's answer for AppId 480 -- i.e. no DLC at all.
static bool S_CALLTYPE Hooked_BIsDlcInstalled(void* pThis, AppId_t appID)
{
    if (g_pfnOriginalBIsDlcInstalled && g_pfnOriginalBIsDlcInstalled(pThis, appID))
        return true;

    const bool ours = UcoDlcStore::IsInstalled((uint32)appID);
    if (ours && !g_bDlcLoggedFirst)
    {
        UCOLOG("[UCOnline2] BIsDlcInstalled(%u) hook returning true (Steam says false)", appID);
        g_bDlcLoggedFirst = true;
    }
    return ours;
}

// Some games treat a purchase time of 0 as "not owned", so answering the
// ownership question alone isn't enough. Backdated by four days.
static uint32 S_CALLTYPE Hooked_GetEarliestPurchaseUnixTime(void* pThis, AppId_t appID)
{
    uint32 original = g_pfnOriginalGetEarliestPurchase
                    ? g_pfnOriginalGetEarliestPurchase(pThis, appID) : 0;
    if (original) return original;
    if (!UcoDlcStore::IsOwned((uint32)appID)) return 0;
    return (uint32)(time(nullptr) - 4 * 24 * 60 * 60);
}

// Enumeration. UnlockAll can answer any ownership question without knowing the
// ids, but it cannot invent a LIST -- only DLC named in the ini can be reported
// here, which is why [DLC] entries matter for games that build a DLC menu.
static int S_CALLTYPE Hooked_GetDLCCount(void* pThis)
{
    const int original = g_pfnOriginalGetDLCCount ? g_pfnOriginalGetDLCCount(pThis) : 0;
    const int ours = UcoDlcStore::Count();
    if (ours <= 0) return original;

    if (!g_bDlcCountLoggedFirst)
    {
        UCOLOG("[UCOnline2] GetDLCCount hook returning %d configured DLC (Steam reports %d)",
            ours, original);
        g_bDlcCountLoggedFirst = true;
    }
    return ours;
}

static bool S_CALLTYPE Hooked_BGetDLCDataByIndex(void* pThis, int iDLC, AppId_t* pAppID,
                                                 bool* pbAvailable, char* pchName, int cchName)
{
    if (UcoDlcStore::Count() > 0)
        return UcoDlcStore::Get(iDLC, (uint32_t*)pAppID, pbAvailable, pchName, cchName);
    return g_pfnOriginalBGetDLCDataByIndex
         ? g_pfnOriginalBGetDLCDataByIndex(pThis, iDLC, pAppID, pbAvailable, pchName, cchName)
         : false;
}

// Make ISteamUtils::GetAppID() report ogAppId so the rest of the
// game stack agrees on the "real" AppId (matches the way OnlineFix
// exposes RealAppId via the same interface).
static uint32 S_CALLTYPE Hooked_GetAppID(void* pThis)
{
    if (g_OriginalAppId == 0 || g_OriginalAppId == g_ForcedAppId)
        return g_pfnOriginalGetAppID ? g_pfnOriginalGetAppID(pThis) : g_ForcedAppId;

    if (!g_bGetAppIDLoggedFirst)
    {
        UCOLOG("[UCOnline2] GetAppID hook returning ogAppId=%u",
            g_OriginalAppId);
        g_bGetAppIDLoggedFirst = true;
    }
    return g_OriginalAppId;
}

void InstallEarlyAppIdHook(ISteamUtils* pUtils)
{
    if (!pUtils || g_OriginalAppId == 0 || g_OriginalAppId == g_ForcedAppId)
        return;

    void** utilsVT = *reinterpret_cast<void***>(pUtils);
    void* target = utilsVT[9];
    if (g_pHookedGetAppIDTarget == target && g_pfnOriginalGetAppID)
        return;

    MH_Initialize();
    MH_STATUS status = MH_CreateHook(target, &Hooked_GetAppID,
        reinterpret_cast<void**>(&g_pfnOriginalGetAppID));
    if (status != MH_OK)
    {
        UCOLOG("[UCOnline2] MH_CreateHook failed for early GetAppID: %d", status);
        return;
    }

    status = MH_EnableHook(target);
    if (status == MH_OK || status == MH_ERROR_ENABLED)
    {
        g_pHookedGetAppIDTarget = target;
        UCOLOG("[UCOnline2] GetAppID hook installed early (will return %u)",
            g_OriginalAppId);
    }
    else
    {
        UCOLOG("[UCOnline2] MH_EnableHook failed for early GetAppID: %d", status);
    }
}


// ============================================================
// Auth session ticket emulation ([Settings] EmulateTicket).
//
// WHY
// Games that gate multiplayer on Steam identity call
// ISteamUser::GetAuthSessionTicket, send the blob to the host, and the host
// calls BeginAuthSession on it. Passed through, real Steam mints that ticket
// under the SPOOFED AppId (480), so it does not describe the game being played
// and the peer rejects it. In a log this shows up as the game registering
// GetAuthSessionTicketResponse_t (callback 163) over and over -- asking for a
// ticket, not getting a usable one, retrying.
//
// This is emulatable precisely BECAUSE the check is peer-side rather than
// Valve-side: both players run the same emulator, so one side mints the ticket
// and the other accepts it. Contrast a publisher backend that asks Steam to
// validate server-side -- that is unforgeable and no local emulation helps.
//
// The blob is shaped so a peer running this same code can sanity-check it: the
// first fields are a magic value, the ogAppId and our SteamID.
// ============================================================
// No original function pointers here on purpose: these detours fully replace
// the originals and are only installed when emulation is on, so nothing ever
// calls through. That is what lets several ISteamUser versions be hooked
// without a trampoline set per version.
static volatile LONG g_NextAuthTicket = 0;

// The detours fully REPLACE the originals -- they are only installed when
// emulation is on, and never call through. That is what makes it safe to hook
// several ISteamUser objects (the game can hold more than one interface
// version) without a trampoline per version.
// ------------------------------------------------------------
// Building a REAL Steam auth session ticket.
//
// The first attempt at this emitted 64 bytes of our own invention -- a magic
// value, an AppId, a SteamID. That can only ever satisfy another copy of this
// emulator, because it is not a Steam ticket and no real parser accepts it.
// Anything that inspects the ticket (a game server, a publisher backend)
// rejects it immediately, which is exactly what Farming Simulator 25 did.
//
// What actually works is emitting the genuine STRUCTURE. Servers that do not
// call Valve to verify -- which is most of them -- parse the blob locally for
// the SteamID, AppId and entitlements, and a structurally correct ticket
// satisfies them. The 128-byte signature is left zeroed: we obviously cannot
// forge Valve's, and anything that checks it is out of reach by definition.
//
// Layout below is the documented Steam app-ticket format (cross-checked against
// gbe_fork's auth.cpp, which is the reference implementation):
//
//   GC blob        24 bytes   len(20), GCToken, SteamID, generated-date
//   session blob   28 bytes   len(24), 1, 2, ExternalIP, InternalIP,
//                             TimeSinceStartup, TicketGeneratedCount
//   uint32                    remaining length (4 + ticket + signature)
//   uint32                    ticket layout length (ticket blob + 4)
//   ticket blob    42+ bytes  Version(4), SteamID, AppId, ExternalIP,
//                             InternalIP, zero, generated, expires,
//                             licenses[], DLCs[], padding
//   signature     128 bytes   zeroed
//
// With no licences that totals 230 bytes; Steam itself returns 234, the
// difference being a single licence entry, which is a useful size check.
//
// The DLC section is populated from the configured [DLC] list. Some games read
// their entitlements from the ticket rather than from BIsDlcInstalled, so
// answering only the API call can leave a game insisting it has no licence for
// content it was just told it owns.
// ------------------------------------------------------------
namespace uco_ticket {

struct Writer
{
    uint8_t* p;
    uint8_t* end;
    bool     ok;

    Writer(void* buf, int cap)
        : p((uint8_t*)buf), end((uint8_t*)buf + cap), ok(true) {}

    void u16(uint16_t v) { put(&v, sizeof(v)); }
    void u32(uint32_t v) { put(&v, sizeof(v)); }
    void u64(uint64_t v) { put(&v, sizeof(v)); }
    void zero(size_t n)  { if (!room(n)) return; memset(p, 0, n); p += n; }

    size_t written(void* base) const { return (size_t)(p - (uint8_t*)base); }

private:
    bool room(size_t n) { if (p + n > end) { ok = false; return false; } return true; }
    void put(const void* v, size_t n) { if (!room(n)) return; memcpy(p, v, n); p += n; }
};

static const uint32_t GC_LEN       = 20;
static const uint32_t SESSION_LEN  = 24;
static const uint32_t SIG_LEN      = 128;
static const uint32_t TICKET_VER   = 4;

} // namespace uco_ticket

static uint32 UcoBuildSteamAuthTicket(void* pTicket, int cbMaxTicket,
                                      uint32 appId, uint64 steamId)
{
    using namespace uco_ticket;

    const uint32_t now = (uint32_t)time(nullptr);
    const uint32_t startupSecs = (uint32_t)(GetTickCount64() / 1000ULL);
    const uint32_t seq = (uint32_t)InterlockedIncrement(&g_NextAuthTicket);

    // Size the ticket blob first: the two length prefixes have to describe it.
    const int dlcCount = UcoDlcStore::Count();
    size_t ticketBlob = 42;                       // fixed fields, incl. both counts + padding
    ticketBlob += 4;                              // one licence entry
    for (int i = 0; i < dlcCount; ++i)
        ticketBlob += 4 + 2 + 4;                  // DLC AppId + licence count + one licence

    const size_t total = 24 + 28 + 4 + 4 + ticketBlob + SIG_LEN;
    if (cbMaxTicket < (int)total)
        return 0;

    Writer w(pTicket, cbMaxTicket);
    memset(pTicket, 0, total);

    // --- GC blob ---
    w.u32(GC_LEN);
    w.u64(((uint64_t)seq << 32) ^ (steamId + now));   // GCToken: unique per ticket
    w.u64(steamId);
    w.u32(now);

    // --- session blob ---
    w.u32(SESSION_LEN);
    w.u32(1);
    w.u32(2);
    w.u32(0);                                     // ExternalIP (unknown/unused)
    w.u32(0);                                     // InternalIP
    w.u32(startupSecs);
    w.u32(seq);

    // --- length prefixes ---
    w.u32((uint32_t)(4 + ticketBlob + SIG_LEN));  // remaining
    w.u32((uint32_t)(ticketBlob + 4));            // ticket layout length

    // --- ticket blob ---
    w.u32(TICKET_VER);
    w.u64(steamId);
    w.u32(appId);
    w.u32(0);                                     // ExternalIP
    w.u32(0);                                     // InternalIP
    w.u32(0);                                     // AlwaysZero / ownership flags
    w.u32(now);                                   // generated
    w.u32(now + 24 * 60 * 60);                    // expires

    w.u16(1);                                     // licence count
    w.u32(0);                                     // licence id (not validated locally)

    w.u16((uint16_t)dlcCount);
    for (int i = 0; i < dlcCount; ++i)
    {
        uint32_t dlcAppId = 0;
        bool avail = false;
        char name[8] = {};
        UcoDlcStore::Get(i, &dlcAppId, &avail, name, 0);
        w.u32(dlcAppId);
        w.u16(1);                                 // one licence for this DLC
        w.u32(0);
    }

    w.u16(0);                                     // padding
    w.zero(SIG_LEN);                              // signature we cannot forge

    if (!w.ok)
        return 0;
    return (uint32)total;
}

static HAuthTicket UcoEmitAuthTicket(void* pTicket, int cbMaxTicket, uint32* pcbTicket)
{
    uint64 steamId = 0;
    if (g_ClientCtx.SteamUser())
        steamId = g_ClientCtx.SteamUser()->GetSteamID().ConvertToUint64();

    const uint32 appId = g_OriginalAppId ? g_OriginalAppId : g_ForcedAppId;

    const uint32 cb = pTicket ? UcoBuildSteamAuthTicket(pTicket, cbMaxTicket, appId, steamId) : 0;
    if (!cb)
    {
        UCOLOG("[UCOnline2] GetAuthSessionTicket: buffer too small (%d) -- returning invalid handle",
            cbMaxTicket);
        if (pcbTicket) *pcbTicket = 0;
        return k_HAuthTicketInvalid;
    }
    if (pcbTicket) *pcbTicket = cb;

    const HAuthTicket handle = (HAuthTicket)InterlockedCompareExchange(&g_NextAuthTicket, 0, 0);

    GetAuthSessionTicketResponse_t resp = {};
    resp.m_hAuthTicket = handle;
    resp.m_eResult     = k_EResultOK;
    if (GetDispatcher())
        GetDispatcher()->PostCallback(GetAuthSessionTicketResponse_t::k_iCallback,
            &resp, sizeof(resp), g_ClientUser, false, 10);

    UCOLOG("[UCOnline2] GetAuthSessionTicket emulated -> handle=%u appid=%u size=%u (%d DLC entitlement(s))",
        handle, appId, cb, UcoDlcStore::Count());
    return handle;
}

// SteamUser021 takes three arguments; 022 and later added the networking
// identity. Same body, two entry points -- calling through the wrong arity
// would corrupt the call.
static HAuthTicket S_CALLTYPE Hooked_GetAuthSessionTicket3(void* pThis, void* pTicket,
                                                           int cbMaxTicket, uint32* pcbTicket)
{
    (void)pThis;
    return UcoEmitAuthTicket(pTicket, cbMaxTicket, pcbTicket);
}

static HAuthTicket S_CALLTYPE Hooked_GetAuthSessionTicket4(void* pThis, void* pTicket,
                                                           int cbMaxTicket, uint32* pcbTicket,
                                                           const void* pIdentity)
{
    (void)pThis; (void)pIdentity;
    return UcoEmitAuthTicket(pTicket, cbMaxTicket, pcbTicket);
}

// GetAuthTicketForWebApi (SteamUser023+) is the WebApi-flavoured sibling of
// GetAuthSessionTicket. Unlike the session call it does NOT fill a caller
// buffer -- it returns only a handle and delivers the ticket bytes INLINE in
// GetTicketForWebApiResponse_t (callback 168).
//
// Games that authenticate to a secondary backend (EOS/Redpoint, a publisher
// web service) with a Steam WebApi ticket gate their whole online flow on this
// callback: Pizza House Simulator registers cb168, waits, retries, and never
// reaches EOS_Connect_Login when it never arrives. When a plugin swaps the
// downstream EOS login to Device ID the ticket CONTENT is irrelevant -- we only
// have to make the callback fire so the game advances past the Steam gate. Same
// locally-parseable structure as the session ticket; the signature we cannot
// forge stays zeroed (anything that verifies it server-side is out of reach by
// definition, exactly as for GetAuthSessionTicket).
static HAuthTicket S_CALLTYPE Hooked_GetAuthTicketForWebApi(void* pThis, const char* pchIdentity)
{
    (void)pThis;

    uint64 steamId = 0;
    if (g_ClientCtx.SteamUser())
        steamId = g_ClientCtx.SteamUser()->GetSteamID().ConvertToUint64();
    const uint32 appId = g_OriginalAppId ? g_OriginalAppId : g_ForcedAppId;

    GetTicketForWebApiResponse_t resp = {};
    const uint32 cb = UcoBuildSteamAuthTicket(resp.m_rgubTicket,
                                              GetTicketForWebApiResponse_t::k_nCubTicketMaxLength,
                                              appId, steamId);
    const HAuthTicket handle = (HAuthTicket)InterlockedCompareExchange(&g_NextAuthTicket, 0, 0);

    resp.m_hAuthTicket = handle;
    resp.m_eResult     = k_EResultOK;
    resp.m_cubTicket   = (int)cb;
    if (GetDispatcher())
        GetDispatcher()->PostCallback(GetTicketForWebApiResponse_t::k_iCallback,
            &resp, sizeof(resp), g_ClientUser, false, 10);

    UCOLOG("[UCOnline2] GetAuthTicketForWebApi emulated -> handle=%u appid=%u size=%u identity=%s",
        handle, appId, cb, pchIdentity ? pchIdentity : "(null)");
    return handle;
}

// Legacy client/server authentication uses this older entry point instead of
// GetAuthSessionTicket.  Its payload is still handed to the game server as an
// opaque auth blob, so use the same locally parseable ticket structure.  The
// endpoint arguments are retained in the signature for ABI compatibility.
static int S_CALLTYPE Hooked_InitiateGameConnection(void* pThis, void* pAuthBlob,
                                                    int cbMaxAuthBlob, CSteamID steamIDGameServer,
                                                    uint32 unIPServer, uint16 usPortServer,
                                                    bool bSecure)
{
    (void)pThis;
    (void)steamIDGameServer;
    (void)unIPServer;
    (void)usPortServer;
    (void)bSecure;

    uint64 steamId = 0;
    if (g_ClientCtx.SteamUser())
        steamId = g_ClientCtx.SteamUser()->GetSteamID().ConvertToUint64();

    const uint32 appId = g_OriginalAppId ? g_OriginalAppId : g_ForcedAppId;
    const uint32 cb = pAuthBlob
        ? UcoBuildSteamAuthTicket(pAuthBlob, cbMaxAuthBlob, appId, steamId)
        : 0;
    if (!cb)
    {
        UCOLOG("[UCOnline2] InitiateGameConnection emulated -> buffer too small (%d)",
            cbMaxAuthBlob);
        return 0;
    }

    UCOLOG("[UCOnline2] InitiateGameConnection emulated -> %u bytes appid=%u", cb, appId);
    return (int)cb;
}

static void S_CALLTYPE Hooked_TerminateGameConnection(void* pThis,
                                                       uint32 unIPServer, uint16 usPortServer)
{
    (void)pThis;
    UCOLOG("[UCOnline2] TerminateGameConnection emulated for %u:%u", unIPServer, usPortServer);
}

static EBeginAuthSessionResult S_CALLTYPE Hooked_BeginAuthSession(void* pThis, const void* pAuthTicket,
                                                                  int cbAuthTicket, CSteamID steamID)
{
    // Accept, then answer the ValidateAuthTicketResponse_t the caller waits on;
    // returning OK alone leaves a host stuck on "authenticating".
    ValidateAuthTicketResponse_t resp = {};
    resp.m_SteamID              = steamID;
    resp.m_eAuthSessionResponse = k_EAuthSessionResponseOK;
    resp.m_OwnerSteamID         = steamID;
    if (GetDispatcher())
        GetDispatcher()->PostCallback(ValidateAuthTicketResponse_t::k_iCallback,
            &resp, sizeof(resp), g_ClientUser, false, 10);

    UCOLOG("[UCOnline2] BeginAuthSession emulated -> OK for %llu (%d byte ticket)",
        (unsigned long long)steamID.ConvertToUint64(), cbAuthTicket);
    return k_EBeginAuthSessionResultOK;
}

static void S_CALLTYPE Hooked_EndAuthSession(void* pThis, CSteamID steamID)
{
    UCOLOG("[UCOnline2] EndAuthSession emulated for %llu",
        (unsigned long long)steamID.ConvertToUint64());
}

static void S_CALLTYPE Hooked_CancelAuthTicket(void* pThis, HAuthTicket hAuthTicket)
{
    UCOLOG("[UCOnline2] CancelAuthTicket emulated for handle=%u", hAuthTicket);
}


// ------------------------------------------------------------
// Per-interface auth hook installation.
//
// WHY THIS IS NOT A ONE-LINER
// We ask Steam for SteamUser023, but a game may ask for an older version --
// Farming Simulator 25 uses SteamUser021. Steam returns a SEPARATE adapter
// object per version, each with its own vtable, so hooks installed on our
// object are never reached by the game. The symptom is silent: the hooks report
// as installed and simply never fire.
//
// Worse, the layout MOVES between versions, so hooking the game's object with
// our version's indices would detour the wrong functions entirely:
//
//   version  Initiate  Terminate  GetAuthSessionTicket   Begin  End  Cancel
//   021      3         4          13 (three args)        14     15   16
//   022      3         4          13 (four args)         14     15   16
//   023      3         4          13 (four args)         15     16   17
//
// 023 inserted GetAuthTicketForWebApi at 14 and pushed the rest down; 021
// predates the networking-identity argument. Using 023 indices on a 021 object
// would put a CancelAuthTicket detour on UserHasLicenseForApp.
//
// So: resolve the layout from the version string, and refuse to guess for
// versions not listed. A missed hook is a bug report; a wrong hook is a crash.
// ------------------------------------------------------------
struct UcoUserAuthLayout
{
    int initiate;
    int terminate;
    int  getTicket;
    int  webapi;                 // GetAuthTicketForWebApi; -1 on versions that lack it
    int  begin;
    int  end;
    int  cancel;
    bool ticketTakesIdentity;
};

static bool UcoUserAuthLayoutFor(const char* ver, UcoUserAuthLayout& out)
{
    if (!ver || _strnicmp(ver, "SteamUser", 9) != 0)
        return false;
    const int n = atoi(ver + 9);
    // GetAuthTicketForWebApi was inserted at index 14 in SteamUser023, pushing
    // Begin/End/Cancel down one; 021/022 predate it (webapi = -1).
    if (n == 21) { out.initiate = 3; out.terminate = 4; out.getTicket = 13; out.webapi = -1; out.begin = 14; out.end = 15; out.cancel = 16; out.ticketTakesIdentity = false; return true; }
    if (n == 22) { out.initiate = 3; out.terminate = 4; out.getTicket = 13; out.webapi = -1; out.begin = 14; out.end = 15; out.cancel = 16; out.ticketTakesIdentity = true;  return true; }
    if (n >= 23) { out.initiate = 3; out.terminate = 4; out.getTicket = 13; out.webapi = 14; out.begin = 15; out.end = 16; out.cancel = 17; out.ticketTakesIdentity = true;  return true; }
    return false;   // 020 and older: layouts not verified, so do not touch them
}

void UcoInstallUserAuthHooks(void* pIface, const char* ver)
{
    if (!g_bEmulateAuthTicket || !pIface || !ver)
        return;

    UcoUserAuthLayout L = {};
    if (!UcoUserAuthLayoutFor(ver, L))
    {
        if (_strnicmp(ver, "SteamUser", 9) == 0)
            UCOLOG("[UCOnline2] %s: auth vtable layout unknown -- NOT hooking "
                   "(guessing an index would detour the wrong function)", ver);
        return;
    }

    // One object may be handed out repeatedly; hook each vtable once.
    static void* s_seen[8] = {};
    static int   s_seenCount = 0;
    void** vt = *reinterpret_cast<void***>(pIface);
    for (int i = 0; i < s_seenCount; ++i)
        if (s_seen[i] == (void*)vt)
            return;
    if (s_seenCount < (int)(sizeof(s_seen) / sizeof(s_seen[0])))
        s_seen[s_seenCount++] = (void*)vt;

    MH_Initialize();

    void* getTicketDetour = L.ticketTakesIdentity
                          ? (void*)&Hooked_GetAuthSessionTicket4
                          : (void*)&Hooked_GetAuthSessionTicket3;

    struct Item { int index; void* detour; const char* name; };
    const Item items[] = {
        { L.initiate, (void*)&Hooked_InitiateGameConnection, "InitiateGameConnection" },
        { L.terminate, (void*)&Hooked_TerminateGameConnection, "TerminateGameConnection" },
        { L.getTicket, getTicketDetour,             "GetAuthSessionTicket" },
        { L.webapi,    (void*)&Hooked_GetAuthTicketForWebApi, "GetAuthTicketForWebApi" },
        { L.begin,     (void*)&Hooked_BeginAuthSession, "BeginAuthSession" },
        { L.end,       (void*)&Hooked_EndAuthSession,   "EndAuthSession" },
        { L.cancel,    (void*)&Hooked_CancelAuthTicket, "CancelAuthTicket" },
    };

    int ok = 0, applicable = 0;
    for (const Item& it : items)
    {
        if (it.index < 0)                          // method absent in this version
            continue;
        ++applicable;
        void* target = vt[it.index];
        void* dummyOriginal = nullptr;             // detours never call through
        MH_STATUS st = MH_CreateHook(target, it.detour, &dummyOriginal);
        if (st == MH_ERROR_ALREADY_CREATED)        // shared impl across versions
        {
            ++ok;
            continue;
        }
        if (st == MH_OK && MH_EnableHook(target) == MH_OK)
            ++ok;
        else
            UCOLOG("[UCOnline2] %s: failed to hook %s (vtable[%d]): %d",
                ver, it.name, it.index, st);
    }

    UCOLOG("[UCOnline2] auth ticket emulation: %d/%d hooks installed on %s%s",
        ok, applicable, ver, L.ticketTakesIdentity ? "" : " (3-arg ticket variant)");
}

// ------------------------------------------------------------
// ISteamRemoteStorage vtable hooks -- the [Settings] LocalSaves path for games
// that use the C++ interface directly instead of the flat SteamAPI_* exports.
//
// Same hazard as the ISteamUser auth hooks: a game may ask for an older
// interface version than the one we hold, and Steam hands out a SEPARATE object
// per version. The layouts, read off the shipped headers for 010-016:
//
//   version  FileWrite  async trio  FileExists  IsCloudEnabledForApp
//   <=012    0          absent      10          19
//   >=013    0          2,3,4       13          22
//
// 013 inserted FileWriteAsync/FileReadAsync/FileReadAsyncComplete after
// FileRead and pushed everything below down by three; 013/014/015/016 are
// otherwise identical through SetCloudEnabledForApp. Versions outside that
// range are not hooked -- a missed hook means saves keep going to the shared
// bucket, a wrong hook detours some unrelated method.
//
// The detours never call the original: with LocalSaves on, the local store IS
// the storage. UGC/workshop slots are left alone and still reach real Steam.
// ------------------------------------------------------------
static bool S_CALLTYPE Hooked_RS_FileWrite(void*, const char* f, const void* p, int32 n)
{ return UcoCloudEmu::FileWrite(f, p, n); }
static int32 S_CALLTYPE Hooked_RS_FileRead(void*, const char* f, void* p, int32 n)
{ return UcoCloudEmu::FileRead(f, p, n); }
static SteamAPICall_t S_CALLTYPE Hooked_RS_FileWriteAsync(void*, const char* f, const void* p, uint32 n)
{ return UcoCloudEmu::FileWriteAsync(f, p, n); }
static SteamAPICall_t S_CALLTYPE Hooked_RS_FileReadAsync(void*, const char* f, uint32 off, uint32 n)
{ return UcoCloudEmu::FileReadAsync(f, off, n); }
static bool S_CALLTYPE Hooked_RS_FileReadAsyncComplete(void*, SteamAPICall_t h, void* p, uint32 n)
{ return UcoCloudEmu::FileReadAsyncComplete(h, p, n); }
static bool S_CALLTYPE Hooked_RS_FileForget(void*, const char* f)
{ return UcoCloudEmu::FileForget(f); }
static bool S_CALLTYPE Hooked_RS_FileDelete(void*, const char* f)
{ return UcoCloudEmu::FileDelete(f); }
static bool S_CALLTYPE Hooked_RS_SetSyncPlatforms(void*, const char*, ERemoteStoragePlatform)
{ return true; }
static UGCFileWriteStreamHandle_t S_CALLTYPE Hooked_RS_FileWriteStreamOpen(void*, const char* f)
{ return UcoCloudEmu::FileWriteStreamOpen(f); }
static bool S_CALLTYPE Hooked_RS_FileWriteStreamWriteChunk(void*, UGCFileWriteStreamHandle_t h, const void* p, int32 n)
{ return UcoCloudEmu::FileWriteStreamWriteChunk(h, p, n); }
static bool S_CALLTYPE Hooked_RS_FileWriteStreamClose(void*, UGCFileWriteStreamHandle_t h)
{ return UcoCloudEmu::FileWriteStreamClose(h); }
static bool S_CALLTYPE Hooked_RS_FileWriteStreamCancel(void*, UGCFileWriteStreamHandle_t h)
{ return UcoCloudEmu::FileWriteStreamCancel(h); }
static bool S_CALLTYPE Hooked_RS_FileExists(void*, const char* f)
{ return UcoCloudEmu::FileExists(f); }
static bool S_CALLTYPE Hooked_RS_FilePersisted(void*, const char* f)
{ return UcoCloudEmu::FilePersisted(f); }
static int32 S_CALLTYPE Hooked_RS_GetFileSize(void*, const char* f)
{ return UcoCloudEmu::GetFileSize(f); }
static int64 S_CALLTYPE Hooked_RS_GetFileTimestamp(void*, const char* f)
{ return UcoCloudEmu::GetFileTimestamp(f); }
static ERemoteStoragePlatform S_CALLTYPE Hooked_RS_GetSyncPlatforms(void*, const char*)
{ return k_ERemoteStoragePlatformAll; }
static int32 S_CALLTYPE Hooked_RS_GetFileCount(void*)
{ return UcoCloudEmu::GetFileCount(); }
static const char* S_CALLTYPE Hooked_RS_GetFileNameAndSize(void*, int i, int32* pSize)
{ return UcoCloudEmu::GetFileNameAndSize(i, pSize); }
static bool S_CALLTYPE Hooked_RS_GetQuota(void*, uint64* pTotal, uint64* pAvail)
{ return UcoCloudEmu::GetQuota(pTotal, pAvail); }
static bool S_CALLTYPE Hooked_RS_IsCloudEnabled(void*)
{ return UcoCloudEmu::IsCloudEnabled(); }
// Deliberately does nothing: forwarding would flip the real Steam Cloud setting
// for Spacewar, outside this game and permanently.
static void S_CALLTYPE Hooked_RS_SetCloudEnabledForApp(void*, bool) {}

struct UcoRemoteStorageLayout
{
    int write, read, writeAsync, readAsync, readAsyncDone;
    int forget, del, setSync, streamOpen, streamChunk, streamClose, streamCancel;
    int exists, persisted, size, timestamp, getSync, count, nameAndSize;
    int quota, cloudAccount, cloudApp, setCloudApp;
};

static bool UcoRemoteStorageLayoutFor(const char* ver, UcoRemoteStorageLayout& out)
{
    const char* kPrefix = "STEAMREMOTESTORAGE_INTERFACE_VERSION";
    const size_t kPrefixLen = 36;
    if (!ver || _strnicmp(ver, kPrefix, kPrefixLen) != 0)
        return false;
    const int n = atoi(ver + kPrefixLen);
    if (n >= 13)
    {
        out = { 0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12,
                13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 };
        return true;
    }
    if (n >= 10)
    {
        // No async trio; everything after FileRead sits three slots higher.
        out = { 0, 1, -1, -1, -1, 2, 3, 5, 6, 7, 8, 9,
                10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
        return true;
    }
    return false;   // 009 and older: layouts not verified, so do not touch them
}

void UcoInstallRemoteStorageHooks(void* pIface, const char* ver)
{
    if (!UcoCloudEmu::Enabled() || !pIface || !ver)
        return;

#if defined(_M_IX86)
    // These interface methods are __thiscall on x86 (this in ECX) but the
    // detours above are __cdecl, so every argument would land one slot off --
    // see the same open bug on the DLC/auth hooks. A wrong bool there is a
    // cosmetic glitch; here it would write a corrupt save, so 32-bit games get
    // only the flat-export path (which passes instancePtr explicitly and is
    // unaffected). That covers Steamworks.NET and most 32-bit Unity titles.
    (void)ver;
    UCOLOG("[UCOnline2] LocalSaves: skipping RemoteStorage vtable hooks on x86 "
           "(calling convention); the flat SteamAPI_* exports still go local");
    return;
#else

    UcoRemoteStorageLayout L = {};
    if (!UcoRemoteStorageLayoutFor(ver, L))
    {
        if (_strnicmp(ver, "STEAMREMOTESTORAGE", 18) == 0)
            UCOLOG("[UCOnline2] %s: RemoteStorage vtable layout unknown -- NOT hooking "
                   "(saves through this interface keep going to the spoofed AppId)", ver);
        return;
    }

    // One object may be handed out repeatedly; hook each vtable once.
    static void* s_seen[8] = {};
    static int   s_seenCount = 0;
    void** vt = *reinterpret_cast<void***>(pIface);
    for (int i = 0; i < s_seenCount; ++i)
        if (s_seen[i] == (void*)vt)
            return;
    if (s_seenCount < (int)(sizeof(s_seen) / sizeof(s_seen[0])))
        s_seen[s_seenCount++] = (void*)vt;

    // Pull any existing save out of the spoofed app's cloud bucket FIRST --
    // after the hooks are on, this interface just reads the local store back.
    UcoCloudEmu::ImportFromSteam(pIface,
        (UcoCloudEmu::PfnGetFileCount)vt[L.count],
        (UcoCloudEmu::PfnGetFileNameAndSize)vt[L.nameAndSize],
        (UcoCloudEmu::PfnFileRead)vt[L.read]);

    MH_Initialize();

    struct Item { int index; void* detour; const char* name; };
    const Item items[] = {
        { L.write,        (void*)&Hooked_RS_FileWrite,                 "FileWrite" },
        { L.read,         (void*)&Hooked_RS_FileRead,                  "FileRead" },
        { L.writeAsync,   (void*)&Hooked_RS_FileWriteAsync,            "FileWriteAsync" },
        { L.readAsync,    (void*)&Hooked_RS_FileReadAsync,             "FileReadAsync" },
        { L.readAsyncDone,(void*)&Hooked_RS_FileReadAsyncComplete,     "FileReadAsyncComplete" },
        { L.forget,       (void*)&Hooked_RS_FileForget,                "FileForget" },
        { L.del,          (void*)&Hooked_RS_FileDelete,                "FileDelete" },
        { L.setSync,      (void*)&Hooked_RS_SetSyncPlatforms,          "SetSyncPlatforms" },
        { L.streamOpen,   (void*)&Hooked_RS_FileWriteStreamOpen,       "FileWriteStreamOpen" },
        { L.streamChunk,  (void*)&Hooked_RS_FileWriteStreamWriteChunk, "FileWriteStreamWriteChunk" },
        { L.streamClose,  (void*)&Hooked_RS_FileWriteStreamClose,      "FileWriteStreamClose" },
        { L.streamCancel, (void*)&Hooked_RS_FileWriteStreamCancel,     "FileWriteStreamCancel" },
        { L.exists,       (void*)&Hooked_RS_FileExists,                "FileExists" },
        { L.persisted,    (void*)&Hooked_RS_FilePersisted,             "FilePersisted" },
        { L.size,         (void*)&Hooked_RS_GetFileSize,               "GetFileSize" },
        { L.timestamp,    (void*)&Hooked_RS_GetFileTimestamp,          "GetFileTimestamp" },
        { L.getSync,      (void*)&Hooked_RS_GetSyncPlatforms,          "GetSyncPlatforms" },
        { L.count,        (void*)&Hooked_RS_GetFileCount,              "GetFileCount" },
        { L.nameAndSize,  (void*)&Hooked_RS_GetFileNameAndSize,        "GetFileNameAndSize" },
        { L.quota,        (void*)&Hooked_RS_GetQuota,                  "GetQuota" },
        { L.cloudAccount, (void*)&Hooked_RS_IsCloudEnabled,            "IsCloudEnabledForAccount" },
        { L.cloudApp,     (void*)&Hooked_RS_IsCloudEnabled,            "IsCloudEnabledForApp" },
        { L.setCloudApp,  (void*)&Hooked_RS_SetCloudEnabledForApp,     "SetCloudEnabledForApp" },
    };

    int ok = 0, applicable = 0;
    for (const Item& it : items)
    {
        if (it.index < 0)                          // method absent in this version
            continue;
        ++applicable;
        void* target = vt[it.index];
        void* dummyOriginal = nullptr;             // detours never call through
        MH_STATUS st = MH_CreateHook(target, it.detour, &dummyOriginal);
        if (st == MH_ERROR_ALREADY_CREATED)        // shared impl across versions
        {
            ++ok;
            continue;
        }
        if (st == MH_OK && MH_EnableHook(target) == MH_OK)
            ++ok;
        else
            UCOLOG("[UCOnline2] %s: failed to hook %s (vtable[%d]): %d",
                ver, it.name, it.index, st);
    }

    UCOLOG("[UCOnline2] LocalSaves: %d/%d RemoteStorage hooks installed on %s",
        ok, applicable, ver);
#endif
}

void InstallSteamSpoofHooks()
{
    if (!g_bClientReady)
    {
        UCOLOG("[UCOnline2] Cannot install spoof hooks: client not ready");
        return;
    }

    // Only the AppId spoof needs ogAppId. DLC unlocking and ticket emulation do
    // not, and used to be skipped along with it by a single early return -- so
    // configuring [DLC] without an ogAppId silently did nothing at all.
    const bool bSpoofAppId = (g_OriginalAppId != 0 && g_OriginalAppId != g_ForcedAppId);
    if (!bSpoofAppId)
        UCOLOG("[UCOnline2] No usable ogAppId: AppId spoof skipped "
               "(DLC and ticket hooks are unaffected)");

    MH_Initialize();

    // ISteamUtils vtable: [9] = GetAppID.
    //   0:GetSecondsSinceAppActive  1:GetSecondsSinceComputerActive
    //   2:GetConnectedUniverse  3:GetServerRealTime  4:GetIPCountry
    //   5:GetImageSize  6:GetImageRGBA  7:GetCSERIPPort (private but
    //   present in vtable)  8:GetCurrentBatteryPower  9:GetAppID
    if (bSpoofAppId)
        InstallEarlyAppIdHook(g_ClientCtx.SteamUtils());

    // ISteamApps vtable, in declaration order from include/sdk/isteamapps.h.
    // Hooking by index is safe here because WE pin the interface version:
    // api_client.h requests STEAMAPPS_INTERFACE_VERSION008 explicitly, so the
    // layout cannot shift under us. If that version string ever changes, these
    // indices must be re-derived from the matching header.
    //
    //   0:BIsSubscribed          1:BIsLowViolence     2:BIsCybercafe
    //   3:BIsVACBanned           4:GetCurrentGameLanguage
    //   5:GetAvailableGameLanguages                   6:BIsSubscribedApp
    //   7:BIsDlcInstalled        8:GetEarliestPurchaseUnixTime
    //   9:BIsSubscribedFromFreeWeekend               10:GetDLCCount
    //  11:BGetDLCDataByIndex    12:InstallDLC        13:UninstallDLC
    // ...and that pin is exactly what [Settings] Client breaks. A game pinned to
    // an old SteamClient asks for old interfaces too (Rivals of Aether takes
    // STEAMAPPS_INTERFACE_VERSION007), which is a different object with a
    // different layout from the 008 we hook by index. These are inline hooks on
    // the function bodies, so the old-version caller still lands in them, and we
    // then hand a v007 `this` to the v008 implementation -- which faults inside
    // steamclient. Skip the hooks in that case: losing DLC unlocking is a far
    // better outcome than crashing the game mid-session.
    if (g_LegacyClientVersion[0] && (bSpoofAppId || UcoDlcStore::Active()))
    {
        UCOLOG("[UCOnline2] ISteamApps DLC hooks skipped: Client=%s means the game "
               "uses an older ISteamApps whose vtable does not match ours",
               g_LegacyClientVersion);
    }
    else if ((bSpoofAppId || UcoDlcStore::Active()) && g_ClientCtx.SteamApps())
    {
        void** appsVT = *reinterpret_cast<void***>(g_ClientCtx.SteamApps());

        struct DlcHook { int index; void* detour; void** original; const char* name; };
        const DlcHook hooks[] = {
            { 6,  &Hooked_BIsSubscribedApp,          (void**)&g_pfnOriginalBIsSubscribedApp,    "BIsSubscribedApp" },
            { 7,  &Hooked_BIsDlcInstalled,           (void**)&g_pfnOriginalBIsDlcInstalled,     "BIsDlcInstalled" },
            { 8,  &Hooked_GetEarliestPurchaseUnixTime,(void**)&g_pfnOriginalGetEarliestPurchase, "GetEarliestPurchaseUnixTime" },
            { 10, &Hooked_GetDLCCount,               (void**)&g_pfnOriginalGetDLCCount,         "GetDLCCount" },
            { 11, &Hooked_BGetDLCDataByIndex,        (void**)&g_pfnOriginalBGetDLCDataByIndex,  "BGetDLCDataByIndex" },
        };

        int installed = 0;
        for (const DlcHook& h : hooks)
        {
            void* target = appsVT[h.index];
            MH_STATUS s = MH_CreateHook(target, h.detour, h.original);
            if (s == MH_OK && MH_EnableHook(target) == MH_OK)
                ++installed;
            else
                UCOLOG("[UCOnline2] failed to hook ISteamApps::%s (vtable[%d]): %d",
                    h.name, h.index, s);
        }

        UCOLOG("[UCOnline2] ISteamApps DLC hooks: %d/%d installed (UnlockAll=%d, %d configured DLC)",
            installed, (int)(sizeof(hooks) / sizeof(hooks[0])),
            UcoDlcStore::UnlockAll() ? 1 : 0, UcoDlcStore::Count());
    }

    // ISteamUser auth hooks are installed per-interface, not here -- see
    // UcoInstallUserAuthHooks. The game frequently uses a DIFFERENT interface
    // version than the one we hold, and each version is a separate object with
    // its own vtable, so hooking only ours silently misses every call.
    if (g_bEmulateAuthTicket && g_ClientCtx.SteamUser())
        UcoInstallUserAuthHooks(g_ClientCtx.SteamUser(), STEAMUSER_INTERFACE_VERSION);

    // Same story for ISteamRemoteStorage: a game may take a different version
    // than ours (hooked in the interface factory), but it may equally use the
    // one we hold, so cover that object too. Installing twice is a no-op.
    if (UcoCloudEmu::Enabled() && g_ClientCtx.SteamRemoteStorage())
        UcoInstallRemoteStorageHooks(g_ClientCtx.SteamRemoteStorage(),
            STEAMREMOTESTORAGE_INTERFACE_VERSION);
}

static void SteamStub_Init()
{
	// If the early-load proxy (version.dll) already armed the bypass before the
	// exe entry point -- the only path that works for Unity games that P/Invoke
	// steam_api64 lazily -- don't hook GetTickCount a second time.
	char probe[8] = {};
	if (GetEnvironmentVariableA("UCO2_STEAMSTUB_HOOKED", probe, sizeof(probe)) > 0)
	{
		UCOLOG("[UCOnline2] SteamStub already armed by the early proxy; skipping");
		return;
	}

	if (UcoSteamStub::Install(&SteamStub_LogCb))
		UCOLOG("[UCOnline2] SteamStub hook initialized");
}
