static bool s_bDispatcherInited = false;
CCallbackDispatcher* GetDispatcher();

// Forward declaration for GetSteamPathFromRegistry (defined in dllmain.cpp)
bool GetSteamPathFromRegistry(char* outPath, size_t pathSize);

// DEBUG helper
#define DEBUG_WRITE(msg) do { \
    FILE* _df = nullptr; \
    fopen_s(&_df, "C:\\Users\\cools\\Desktop\\uc_online2_debug.txt", "ab"); \
    if (_df) { fprintf(_df, msg); fclose(_df); } \
} while(0)

uint32 CountRegisteredCallbacks(int iCallbackId)
{
	uint32 count = 0;

	if (s_bDispatcherInited)
	{
		CCallbackDispatcher* pDisp = GetDispatcher();

		if (!pDisp->m_CallbackMap.empty())
		{
			for (auto it = pDisp->m_CallbackMap.begin(); it != pDisp->m_CallbackMap.end(); ++it)
			{
				if (it->first == iCallbackId)
					count++;
			}
		}
	}

	return count;
}

static void WarnMissingInterface(HSteamPipe hPipe, const char* iface)
{
	HMODULE hMod = g_ClientModule;
	if (g_ServerModule) hMod = g_ServerModule;

	g_pfnNotifyMissing = (Fn_NotifyMissing)GetProcAddress(hMod, "Steam_NotifyMissingInterface");
	if (g_pfnNotifyMissing)
		g_pfnNotifyMissing(hPipe, iface);
}

extern uint32 g_ForcedAppId;
extern uint32 g_OriginalAppId;
static bool s_bAnonUser = false;

S_API HSteamPipe S_CALLTYPE GetHSteamPipe()
{
	return g_ClientPipe;
}

S_API HSteamUser S_CALLTYPE GetHSteamUser()
{
	return g_ClientUser;
}

S_API HSteamPipe S_CALLTYPE SteamAPI_GetHSteamPipe()
{
	return g_ClientPipe;
}

S_API HSteamUser S_CALLTYPE SteamAPI_GetHSteamUser()
{
	return g_ClientUser;
}

S_API const char* S_CALLTYPE SteamAPI_GetSteamInstallPath()
{
    if (g_bHaveInstallPath)
    {
        return g_InstallPath;
    }

    if (GetSteamPathFromRegistry(g_InstallPath, MAX_PATH))
    {
        g_bHaveInstallPath = true;
        return g_InstallPath;
    }

    return "UCOnline2_InvalidPath";
}

S_API ESteamAPIInitResult S_CALLTYPE SteamInternal_SteamAPI_Init(const char* pszVersions, SteamErrMsg* pOutErr)
{
	SetAppIDEnv();
	WriteAppIDFile();

	if (g_pSteamClient)
	{
		DEBUG_WRITE("[DEBUG] Already initialized, returning OK\n");
		return k_ESteamAPIInitResult_OK;
	}

	DEBUG_WRITE("[DEBUG] Calling InitSteamClient...\n");
	g_pSteamClient = static_cast<ISteamClient*>(InitSteamClient(&g_ClientModule, s_bAnonUser, STEAMCLIENT_INTERFACE_VERSION));
	if (!g_pSteamClient)
	{
		DEBUG_WRITE("[DEBUG] InitSteamClient FAILED\n");
		SteamAPI_Shutdown();
		return k_ESteamAPIInitResult_FailedGeneric;
	}
	DEBUG_WRITE("[DEBUG] InitSteamClient OK\n");

	SetAppIDEnv();

	if (!s_bAnonUser)
	{
		DEBUG_WRITE("[DEBUG] Creating SteamPipe...\n");
		g_ClientPipe = g_pSteamClient->CreateSteamPipe();
		if (g_ClientPipe == 0)
		{
			DEBUG_WRITE("[DEBUG] CreateSteamPipe FAILED\n");
			SteamAPI_Shutdown();
			return k_ESteamAPIInitResult_NoSteamClient;
		}
		DEBUG_WRITE("[DEBUG] CreateSteamPipe OK\n");

		DEBUG_WRITE("[DEBUG] ConnectToGlobalUser...\n");
		g_ClientUser = g_pSteamClient->ConnectToGlobalUser(g_ClientPipe);
	}
	else
	{
		DEBUG_WRITE("[DEBUG] CreateLocalUser...\n");
		g_ClientUser = g_pSteamClient->CreateLocalUser(&g_ClientPipe, k_EAccountTypeAnonUser);
	}

	if (g_ClientUser == 0)
	{
		DEBUG_WRITE("[DEBUG] ConnectToGlobalUser/CreateLocalUser FAILED\n");
		SteamAPI_Shutdown();
		return k_ESteamAPIInitResult_NoSteamClient;
	}
	DEBUG_WRITE("[DEBUG] ConnectToGlobalUser/CreateLocalUser OK\n");

	if (pszVersions)
	{
		DEBUG_WRITE("[DEBUG] Version checking enabled\n");
		HMODULE hMod = g_ClientModule;
		if (g_ServerModule) hMod = g_ServerModule;

		g_pfnIsKnownInterface = (Fn_IsKnownInterface)GetProcAddress(hMod, "Steam_IsKnownInterface");
		if (!g_pfnIsKnownInterface)
		{
			DEBUG_WRITE("[DEBUG] Steam_IsKnownInterface NOT FOUND -> VERSION MISMATCH\n");
			SteamAPI_Shutdown();
			return k_ESteamAPIInitResult_VersionMismatch;
		}
		DEBUG_WRITE("[DEBUG] Steam_IsKnownInterface found OK\n");
	}
	else
	{
		DEBUG_WRITE("[DEBUG] Version checking disabled (pszVersions is null)\n");
	}

	DEBUG_WRITE("[DEBUG] Getting ISteamUtils...\n");
	ISteamUtils* pUtils = (ISteamUtils*)g_pSteamClient->GetISteamUtils(g_ClientPipe, STEAMUTILS_INTERFACE_VERSION);
	if (!pUtils)
	{
		DEBUG_WRITE("[DEBUG] GetISteamUtils FAILED\n");
		WarnMissingInterface(g_ClientPipe, STEAMUTILS_INTERFACE_VERSION);
		SteamAPI_Shutdown();
		return k_ESteamAPIInitResult_VersionMismatch;
	}
	DEBUG_WRITE("[DEBUG] GetISteamUtils OK\n");

	uint32 reportedID = pUtils->GetAppID();
	DEBUG_WRITE("[DEBUG] Steam reported AppID: ");
	// Write AppID as text
	{
		FILE* _df = nullptr;
		fopen_s(&_df, "C:\\Users\\cools\\Desktop\\uc_online2_debug.txt", "ab");
		if (_df) { fprintf(_df, "%u\n", reportedID); fclose(_df); }
	}

	SetAppIDEnv();
	SteamAPI_SetBreakpadAppID(g_ForcedAppId);
	Steam_RegisterInterfaceFuncs(g_ClientModule);
	LoadBreakpadSymbols(g_ClientModule);
	g_pSteamClient->Set_SteamAPI_CCheckCallbackRegisteredInProcess(CountRegisteredCallbacks);

	DEBUG_WRITE("[DEBUG] Getting ISteamUser...\n");
	ISteamUser* pUser = (ISteamUser*)g_pSteamClient->GetISteamUser(g_ClientUser, g_ClientPipe, STEAMUSER_INTERFACE_VERSION);
	if (!pUser)
	{
		DEBUG_WRITE("[DEBUG] GetISteamUser FAILED\n");
		WarnMissingInterface(g_ClientPipe, STEAMUSER_INTERFACE_VERSION);
		SteamAPI_Shutdown();
		return k_ESteamAPIInitResult_VersionMismatch;
	}
	DEBUG_WRITE("[DEBUG] GetISteamUser OK\n");

	uint64 sid = pUser->GetSteamID().ConvertToUint64();
	bool bLogged = pUser->BLoggedOn();
	{
		FILE* _df = nullptr;
		fopen_s(&_df, "C:\\Users\\cools\\Desktop\\uc_online2_debug.txt", "ab");
		if (_df) { fprintf(_df, "[DEBUG] SteamUser - SID: %llu, LoggedOn: %s\n", sid, bLogged ? "yes" : "no"); fclose(_df); }
	}

	UpdateMinidumpSteamID(sid);

	DEBUG_WRITE("[DEBUG] Calling g_ClientCtx.Init()...\n");
	g_bClientReady = g_ClientCtx.Init();
	{
		FILE* _df = nullptr;
		fopen_s(&_df, "C:\\Users\\cools\\Desktop\\uc_online2_debug.txt", "ab");
		if (_df) { fprintf(_df, "[DEBUG] g_ClientCtx.Init() = %s\n", g_bClientReady ? "success" : "failed"); fclose(_df); }
	}

	if (g_bClientReady)
	{
		DEBUG_WRITE("[DEBUG] SteamAPI_Init SUCCESS: installing BIsSubscribedApp hook\n");
		InstallBIsSubscribedAppHook();
		DEBUG_WRITE("[DEBUG] BIsSubscribedApp hook installed\n");
		return k_ESteamAPIInitResult_OK;
	}
	else
	{
		DEBUG_WRITE("[DEBUG] g_ClientCtx.Init FAILED -> Shutdown\n");
		SteamAPI_Shutdown();
		return k_ESteamAPIInitResult_VersionMismatch;
	}

	DEBUG_WRITE("[DEBUG] SteamAPI_Init falling through to generic failure\n");
	SteamAPI_Shutdown();
	return k_ESteamAPIInitResult_FailedGeneric;
}

S_API bool S_CALLTYPE SteamAPI_Init()
{
	s_bAnonUser = false;
	return SteamInternal_SteamAPI_Init(nullptr, nullptr) == k_ESteamAPIInitResult_OK;
}

S_API ESteamAPIInitResult S_CALLTYPE SteamAPI_InitFlat(SteamErrMsg* pOutErr)
{
	s_bAnonUser = false;
	return SteamInternal_SteamAPI_Init(nullptr, nullptr);
}

S_API bool S_CALLTYPE SteamAPI_InitAnonymousUser()
{
	s_bAnonUser = true;
	bool result = SteamInternal_SteamAPI_Init(nullptr, nullptr) == k_ESteamAPIInitResult_OK;
	s_bAnonUser = false;
	return result;
}

S_API bool S_CALLTYPE SteamAPI_InitSafe()
{
	s_bAnonUser = false;

	bool bOk = SteamAPI_Init();
	if (bOk && g_pSteamClient)
		return true;

	return false;
}

S_API bool S_CALLTYPE SteamAPI_IsSteamRunning()
{
	DWORD ActiveProcessPID = 0;
	LSTATUS GetPID = GetRegistryDWORD("Software\\Valve\\Steam\\ActiveProcess", "pid", ActiveProcessPID);

	if (GetPID == ERROR_SUCCESS && ActiveProcessPID != 0)
	{
		HANDLE hPIDProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, ActiveProcessPID);

		if (hPIDProcess != nullptr)
		{
			DWORD ExitCode = 0;
			BOOL bExitCode = GetExitCodeProcess(hPIDProcess, &ExitCode);

			CloseHandle(hPIDProcess);

			if (bExitCode == TRUE && ExitCode == 259)
			{
				return true;
			}
		}
	}

	return false;
}

S_API void S_CALLTYPE SteamAPI_ReleaseCurrentThreadMemory()
{
	if (g_pfnReleaseThreadLocal)
		g_pfnReleaseThreadLocal(0);
}

S_API bool S_CALLTYPE SteamAPI_RestartAppIfNecessary(uint32 appId)
{
	SetAppIDEnv();
	return false;
}
