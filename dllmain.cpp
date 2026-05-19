#include <Windows.h>
#include <Shlwapi.h>
#include <DbgHelp.h>
#include <new.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define STEAM_API_EXPORTS

#include "include/sdk/steam_api.h"
#include "include/sdk/steamclientpublic.h"
#include "include/sdk/steam_gameserver.h"

S_API ISteamClient* g_pSteamClientGameServer = nullptr;

#include "include/registfuncs.h"
#include "include/callback_dispatcher.h"
#include "include/globals.h"
#include "include/uc_loader.h"
#include "include/dump_handler.h"
#include "include/MinHook.h"

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

bool g_bTryCatch = false;
int g_DispatchMode = 0;
char g_InstallPath[MAX_PATH] = { 0 };
bool g_bHaveInstallPath = false;
SRWLOCK g_CallbackLock;
uint32 g_ForcedAppId = 480;
uint32 g_OriginalAppId = 0;

Fn_CreateInterface g_pfnCreateInterface = nullptr;
Fn_ReleaseThreadLocal g_pfnReleaseThreadLocal = nullptr;
Fn_IsKnownInterface g_pfnIsKnownInterface = nullptr;
Fn_NotifyMissing g_pfnNotifyMissing = nullptr;
Fn_BreakpadInit g_pfnBreakpadInit = nullptr;
Fn_BreakpadSetAppID g_pfnBreakpadSetAppID = nullptr;
Fn_BreakpadSetSteamID g_pfnBreakpadSetSteamID = nullptr;
Fn_BreakpadSetComment g_pfnBreakpadSetComment = nullptr;
Fn_BreakpadWriteDump g_pfnBreakpadWriteDump = nullptr;

uintp g_CtxCounter = 0;

// Forward declarations for SteamStub
static bool g_bSteamStubEnabled = false;
static void SteamStub_Init();

// ============================================================
// SteamInternal_ContextInit
// ============================================================

S_API void* S_CALLTYPE SteamInternal_ContextInit(void* pData)
{
	UCOLOG("[UCOnline2] SteamInternal_ContextInit");
	if (!pData) return nullptr;
	void** pArr = (void**)pData;
	uintp* pCounter = (uintp*)&pArr[1];
	char* pBase = (char*)pData;
	#if defined(_M_IX86)
		if (*pCounter == g_CtxCounter) return pBase + 8;
		AcquireSRWLockExclusive(&g_CtxLock);
		if (*pCounter != g_CtxCounter) { void(*pFn)(void*) = (void(*)(void*))pArr[0]; pFn(pBase + 8); *pCounter = g_CtxCounter; }
		ReleaseSRWLockExclusive(&g_CtxLock);
		return pBase + 8;
	#elif defined(_M_AMD64)
		if (*pCounter == g_CtxCounter) return pBase + 16;
		AcquireSRWLockExclusive(&g_CtxLock);
		if (*pCounter != g_CtxCounter) { void(*pFn)(void*) = (void(*)(void*))pArr[0]; pFn(pBase + 16); *pCounter = g_CtxCounter; }
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
		g_pSteamClientSafe = (ISteamClient*)g_pfnCreateInterface("SteamClient023", nullptr);
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
			HMODULE hLoaded = LoadLibraryExA(overlayPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (hLoaded)
			{
				UCOLOG("[UCOnline2] Loaded game overlay: %s", overlayPath);
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

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		UCOLOG("[UCOnline2] DllMain -> DLL_PROCESS_ATTACH");

		static CDLLLoader s_PluginLoader;
		s_PluginLoader.ReadConfig();
		g_ForcedAppId = s_PluginLoader.GetAppId();
		g_OriginalAppId = s_PluginLoader.GetOgAppId();

		SetAppIDEnv();
		WriteAppIDFile();

		// Load the game overlay early to ensure it can hook into graphics APIs
		LoadGameOverlay();

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
	}

	return TRUE;
}

// ============================================================
// CCallbackDispatcher
// ============================================================

static bool s_bDispatcherReady = false;

CCallbackDispatcher::CCallbackDispatcher()
{
	UCOColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY, "[UCOnline2] CCallbackDispatcher constructed\r\n");

	m_pfnBGetCallback = nullptr;
	m_pfnFreeLastCallback = nullptr;
	m_pfnGetAPICallResult = nullptr;
	m_CurrentUser = 0;
	m_ManualCbId = 0;
	m_ManualCbSize = 0;
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
	m_bProcessing = false;
	m_CallbackMap.clear();
	m_CallResultMap.clear();
	m_BufferMap.clear();
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
	m_bProcessing = false;
	m_CallbackMap.clear();
	m_CallResultMap.clear();
	m_BufferMap.clear();
}

void CCallbackDispatcher::ExecuteCallResult(HSteamPipe hPipe, SteamAPICall_t hCall, CCallbackBase* pCb)
{
	UCOLOG("[UCOnline2] ExecuteCallResult -> call=%lld cb=%d size=%d\r\n", hCall, pCb->GetICallback(), pCb->GetCallbackSizeBytes());

	BYTE* pBuffer = new BYTE[pCb->GetCallbackSizeBytes()]();
	bool bFailed = false;

	bool bResult = m_pfnGetAPICallResult(hPipe, hCall, pBuffer, pCb->GetCallbackSizeBytes(), pCb->GetICallback(), &bFailed);

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
}

void CCallbackDispatcher::DispatchFrame(HSteamPipe hPipe, bool bServer)
{
	if (!m_pfnBGetCallback || !m_pfnFreeLastCallback || !m_pfnGetAPICallResult)
		return;

	CallbackMsg_t msg = { 0 };
	if (!m_pfnBGetCallback(hPipe, &msg))
		return;

	UCOLOG("[UCOnline2] Callback received -> %d\r\n", msg.m_iCallback);
	m_CurrentUser = msg.m_hSteamUser;

	if (msg.m_iCallback == SteamAPICallCompleted_t::k_iCallback && msg.m_cubParam == sizeof(SteamAPICallCompleted_t))
	{
		SteamAPICallCompleted_t* pCompleted = (SteamAPICallCompleted_t*)msg.m_pubParam;

		if (!m_CallResultMap.empty())
		{
			for (auto it = m_CallResultMap.begin(); it != m_CallResultMap.end(); ++it)
			{
				if (pCompleted->m_hAsyncCall == it->first &&
					pCompleted->m_iCallback == it->second->GetICallback() &&
					pCompleted->m_cubParam == (uint32)it->second->GetCallbackSizeBytes())
				{
					ExecuteCallResult(hPipe, it->first, it->second);
				}
			}
		}
	}
	else
	{
		if (!m_CallbackMap.empty())
		{
			for (auto it = m_CallbackMap.begin(); it != m_CallbackMap.end(); ++it)
			{
				CCallbackBase* pCb = it->second;

				if (it->first == msg.m_iCallback && (pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsRegistered))
				{
					if (msg.m_hSteamUser == g_ServerUser && (pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsGameServer) && bServer)
					{
						UCOLOG("[UCOnline2] Server callback -> %d flags=%d\r\n", msg.m_iCallback, pCb->m_nCallbackFlags);
						pCb->Run(msg.m_pubParam);
						break;
					}
					else if (msg.m_hSteamUser == g_ClientUser && !(pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsGameServer) && !bServer)
					{
						UCOLOG("[UCOnline2] Client callback -> %d flags=%d\r\n", msg.m_iCallback, pCb->m_nCallbackFlags);

						bool bSkip = false;

						if (msg.m_iCallback == HTML_NeedsPaint_t::k_iCallback && msg.m_cubParam == sizeof(HTML_NeedsPaint_t))
						{
							HTML_NeedsPaint_t* pPaint = (HTML_NeedsPaint_t*)msg.m_pubParam;
							if (pPaint->unWide == 1 || pPaint->unTall == 1)
								bSkip = true;
						}

						if (!bSkip)
							pCb->Run(msg.m_pubParam);

						break;
					}
				}
			}
		}
	}

	UCOLOG("[UCOnline2] Freeing callback -> %d\r\n", msg.m_iCallback);
	SecureZeroMemory(msg.m_pubParam, msg.m_cubParam);
	m_pfnFreeLastCallback(hPipe);
}

void CCallbackDispatcher::DispatchFrameSafe(HSteamPipe hPipe, bool bServer)
{
	try
	{
		if (!m_pfnBGetCallback || !m_pfnFreeLastCallback || !m_pfnGetAPICallResult)
			return;

		CallbackMsg_t msg = { 0 };
		if (!m_pfnBGetCallback(hPipe, &msg))
			return;

		UCOLOG("[UCOnline2] Callback (safe) -> %d\r\n", msg.m_iCallback);
		m_CurrentUser = msg.m_hSteamUser;

		if (msg.m_iCallback == SteamAPICallCompleted_t::k_iCallback && msg.m_cubParam == sizeof(SteamAPICallCompleted_t))
		{
			SteamAPICallCompleted_t* pCompleted = (SteamAPICallCompleted_t*)msg.m_pubParam;

			if (!m_CallResultMap.empty())
			{
				for (auto it = m_CallResultMap.begin(); it != m_CallResultMap.end(); ++it)
				{
					if (pCompleted->m_hAsyncCall == it->first &&
						pCompleted->m_iCallback == it->second->GetICallback() &&
						pCompleted->m_cubParam == (uint32)it->second->GetCallbackSizeBytes())
					{
						ExecuteCallResult(hPipe, it->first, it->second);
					}
				}
			}
		}
		else
		{
			if (!m_CallbackMap.empty())
			{
				for (auto it = m_CallbackMap.begin(); it != m_CallbackMap.end(); ++it)
				{
					CCallbackBase* pCb = it->second;

					if (it->first == msg.m_iCallback && (pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsRegistered))
					{
						if (msg.m_hSteamUser == g_ServerUser && (pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsGameServer) && bServer)
						{
							UCOLOG("[UCOnline2] Server callback (safe) -> %d flags=%d\r\n", msg.m_iCallback, pCb->m_nCallbackFlags);
							pCb->Run(msg.m_pubParam);
							break;
						}
						else if (msg.m_hSteamUser == g_ClientUser && !(pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsGameServer) && !bServer)
						{
							UCOLOG("[UCOnline2] Client callback (safe) -> %d flags=%d\r\n", msg.m_iCallback, pCb->m_nCallbackFlags);
							pCb->Run(msg.m_pubParam);
							break;
						}
					}
				}
			}
		}

		UCOLOG("[UCOnline2] Freeing callback (safe) -> %d\r\n", msg.m_iCallback);
		SecureZeroMemory(msg.m_pubParam, msg.m_cubParam);
		m_pfnFreeLastCallback(hPipe);
	}
	catch (...)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Exception in callback dispatch\r\n");
	}
}

void CCallbackDispatcher::Add(CCallbackBase* pCb, int iCallback)
{
	if (!pCb)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Add: null callback ptr\r\n");
		return;
	}

	if (pCb->GetCallbackSizeBytes() == 0)
		return;

	UCOLOG("[UCOnline2] Register callback -> %d size=%d flags=%d\r\n", iCallback, pCb->GetCallbackSizeBytes(), pCb->m_nCallbackFlags);

	pCb->m_nCallbackFlags |= pCb->k_ECallbackFlagsRegistered;
	pCb->m_iCallback = iCallback;
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

	if (!(pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsRegistered))
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Remove: not registered\r\n");
		return;
	}

	if (m_CallbackMap.empty())
		return;

	UCOLOG("[UCOnline2] Unregister callback -> %d flags=%d\r\n", pCb->GetICallback(), pCb->m_nCallbackFlags);
	pCb->m_nCallbackFlags &= ~pCb->k_ECallbackFlagsRegistered;

	for (auto it = m_CallbackMap.begin(); it != m_CallbackMap.end(); ++it)
	{
		if (it->second == pCb)
		{
			m_CallbackMap.erase(it);
			break;
		}
	}
}

void CCallbackDispatcher::RemoveCallResult(CCallbackBase* pCb, SteamAPICall_t hCall)
{
	if (!pCb || hCall == k_uAPICallInvalid)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] RemoveCallResult: invalid args\r\n");
		return;
	}

	if (!(pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsRegistered))
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] RemoveCallResult: not registered\r\n");
		return;
	}

	UCOLOG("[UCOnline2] Unregister call result -> %lld cb=%d flags=%d\r\n", hCall, pCb->GetICallback(), pCb->m_nCallbackFlags);
	pCb->m_nCallbackFlags &= ~pCb->k_ECallbackFlagsRegistered;

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
#include <intrin.h>
#include "include/MinHook.h"
#include <atomic>

static std::atomic<uint32_t> g_SteamStubCount{ 0 };
static constexpr uint32_t STEAM_STUB_MAX_COUNT = 1;
static constexpr uint8_t STEAM_STUB_SIGNATURE[] = { 0x44, 0x0F, 0xB6, 0xF8, 0x3C, 0x30, 0x0F, 0x84 };

typedef DWORD(WINAPI* GetTickCount_t)(void);
static GetTickCount_t g_OrigGetTickCount = nullptr;

static uint8_t* SteamStub_FindSignature(uint8_t* start, uint8_t* end, const uint8_t* sig, size_t sigLen)
{
	for (uint8_t* p = start; p < end - sigLen; ++p)
	{
		bool match = true;
		for (size_t i = 0; i < sigLen; ++i)
		{
			if (p[i] != sig[i])
			{
				match = false;
				break;
			}
		}
		if (match)
			return p;
	}
	return nullptr;
}

static DWORD WINAPI SteamStub_HookGetTickCount(void)
{
	uint8_t* returnAddr = reinterpret_cast<uint8_t*>(_ReturnAddress());

	uint8_t* start = returnAddr;
	uint8_t* end = start + 128;

	DWORD oldProtect = 0;
	if (!VirtualProtect(start, static_cast<SIZE_T>(end - start), PAGE_EXECUTE_READWRITE, &oldProtect))
	{
		return g_OrigGetTickCount();
	}

	uint8_t* found = SteamStub_FindSignature(start, end, STEAM_STUB_SIGNATURE, sizeof(STEAM_STUB_SIGNATURE));
	if (found)
	{
		found[6] = 0x90;
		found[7] = 0xE9;

		uint32_t count = g_SteamStubCount.fetch_add(1, std::memory_order_seq_cst) + 1;
		if (count >= STEAM_STUB_MAX_COUNT)
		{
			MH_DisableHook(reinterpret_cast<LPVOID*>(GetTickCount));
		}
	}

	VirtualProtect(start, static_cast<SIZE_T>(end - start), oldProtect, &oldProtect);

	return g_OrigGetTickCount();
}

static void SteamStub_Init()
{
	if (MH_Initialize() != MH_OK)
		return;

	void* pTarget = reinterpret_cast<void*>(GetTickCount);

	if (MH_CreateHook(pTarget, SteamStub_HookGetTickCount, reinterpret_cast<LPVOID*>(&g_OrigGetTickCount)) != MH_OK)
		return;

	if (MH_EnableHook(pTarget) != MH_OK)
		return;

	UCOLOG("[UCOnline2] SteamStub hook initialized");
}
