static bool s_bDispatcherInited = false;
CCallbackDispatcher* GetDispatcher();

// Forward declaration for GetSteamPathFromRegistry (defined in dllmain.cpp)
bool GetSteamPathFromRegistry(char* outPath, size_t pathSize);

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

	UCOLOG("[UCOnline2] CountRegisteredCallbacks -> %d = %u\r\n", iCallbackId, count);
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
	UCOLOG("[UCOnline2] GetHSteamPipe -> %u", g_ClientPipe);
	return g_ClientPipe;
}

S_API HSteamUser S_CALLTYPE GetHSteamUser()
{
	UCOLOG("[UCOnline2] GetHSteamUser -> %u", g_ClientUser);
	return g_ClientUser;
}

S_API HSteamPipe S_CALLTYPE SteamAPI_GetHSteamPipe()
{
	UCOLOG("[UCOnline2] SteamAPI_GetHSteamPipe -> %u", g_ClientPipe);
	return g_ClientPipe;
}

S_API HSteamUser S_CALLTYPE SteamAPI_GetHSteamUser()
{
	UCOLOG("[UCOnline2] SteamAPI_GetHSteamUser -> %u", g_ClientUser);
	return g_ClientUser;
}

S_API const char* S_CALLTYPE SteamAPI_GetSteamInstallPath()
{
    UCOLOG("[UCOnline2] SteamAPI_GetSteamInstallPath called");

    if (g_bHaveInstallPath)
    {
        UCOLOG("[UCOnline2] Steam Install Path (cached) --> %s", g_InstallPath);
        return g_InstallPath;
    }

    // Try to get Steam path from registry instead of complex process detection
    if (GetSteamPathFromRegistry(g_InstallPath, MAX_PATH))
    {
        g_bHaveInstallPath = true;
        UCOLOG("[UCOnline2] Steam Install Path --> %s", g_InstallPath);
        return g_InstallPath;
    }

    UCOLOG("[UCOnline2] Returning invalid path: UCOnline2_InvalidPath");
    return "UCOnline2_InvalidPath";
}

S_API ESteamAPIInitResult S_CALLTYPE SteamInternal_SteamAPI_Init(const char* pszVersions, SteamErrMsg* pOutErr)
{
	UCOLOG("[UCOnline2] SteamAPI_Init called with pszVersions: %s", pszVersions ? pszVersions : "null");
	SetAppIDEnv();
	WriteAppIDFile();
	UCOLOG("[UCOnline2] AppID forced to %u", g_ForcedAppId);

	if (g_pSteamClient)
	{
		UCOLOG("[UCOnline2] Init already called, SteamClient: 0x%p, use Shutdown first", g_pSteamClient);
		return k_ESteamAPIInitResult_OK;
	}

	g_pSteamClient = static_cast<ISteamClient*>(InitSteamClient(&g_ClientModule, s_bAnonUser, STEAMCLIENT_INTERFACE_VERSION));
	UCOLOG("[UCOnline2] InitSteamClient returned SteamClient: 0x%p", g_pSteamClient);

	if (!g_pSteamClient)
	{
		UCOLOG("[UCOnline2] Failed to get SteamClient");
		SteamAPI_Shutdown();
		return k_ESteamAPIInitResult_FailedGeneric;
	}

	SetAppIDEnv();
	UCOLOG("[UCOnline2] SetAppIDEnv called after SteamClient acquisition");

	if (!s_bAnonUser)
	{
		UCOLOG("[UCOnline2] Creating global Steam user (non-anonymous)");
		g_ClientPipe = g_pSteamClient->CreateSteamPipe();
		UCOLOG("[UCOnline2] CreateSteamPipe returned: %u", g_ClientPipe);
		
		if (g_ClientPipe == 0)
		{
			UCOLOG("[UCOnline2] CreateSteamPipe failed");
			SteamAPI_Shutdown();
			return k_ESteamAPIInitResult_NoSteamClient;
		}

		g_ClientUser = g_pSteamClient->ConnectToGlobalUser(g_ClientPipe);
		UCOLOG("[UCOnline2] ConnectToGlobalUser returned: %u", (uint32)g_ClientUser);
	}
	else
	{
		UCOLOG("[UCOnline2] Creating local anonymous Steam user");
		g_ClientUser = g_pSteamClient->CreateLocalUser(&g_ClientPipe, k_EAccountTypeAnonUser);
		UCOLOG("[UCOnline2] CreateLocalUser returned: %u", (uint32)g_ClientUser);
	}

	UCOLOG("[UCOnline2] Final ClientUser: %u", (uint32)g_ClientUser);

	if (g_ClientUser != 0)
	{
		UCOLOG("[UCOnline2] Valid client user obtained, proceeding with interface initialization");
		
		if (pszVersions)
		{
			UCOLOG("[UCOnline2] Version checking enabled");
			HMODULE hMod = g_ClientModule;
			if (g_ServerModule) hMod = g_ServerModule;

			g_pfnIsKnownInterface = (Fn_IsKnownInterface)GetProcAddress(hMod, "Steam_IsKnownInterface");
			if (!g_pfnIsKnownInterface)
			{
				UCOLOG("[UCOnline2] Steam_IsKnownInterface function not found");
				SteamAPI_Shutdown();
				return k_ESteamAPIInitResult_VersionMismatch;
			}
			else
			{
				UCOLOG("[UCOnline2] Steam_IsKnownInterface function found: 0x%p", g_pfnIsKnownInterface);
			}
		}
		else
		{
			UCOLOG("[UCOnline2] Version checking disabled (pszVersions is null)");
		}

		ISteamUtils* pUtils = (ISteamUtils*)g_pSteamClient->GetISteamUtils(g_ClientPipe, STEAMUTILS_INTERFACE_VERSION);
		UCOLOG("[UCOnline2] GetISteamUtils returned: 0x%p", pUtils);
		
		if (pUtils)
		{
			uint32 reportedID = pUtils->GetAppID();
			UCOLOG("[UCOnline2] Steam Utils reported AppID: %u", reportedID);

			if (reportedID == 0)
				UCOLOG("[UCOnline2] Steam reported AppID 0, forcing to %u", g_ForcedAppId);

			SetAppIDEnv();
			UCOLOG("[UCOnline2] SetAppIDEnv called after Utils acquisition");
			
			SteamAPI_SetBreakpadAppID(g_ForcedAppId);
			UCOLOG("[UCOnline2] Set Breakpad AppID to: %u", g_ForcedAppId);
			
			Steam_RegisterInterfaceFuncs(g_ClientModule);
			UCOLOG("[UCOnline2] Registered interface functions with client module");
			
			LoadBreakpadSymbols(g_ClientModule);
			UCOLOG("[UCOnline2] Loaded breakpad symbols");

			g_pSteamClient->Set_SteamAPI_CCheckCallbackRegisteredInProcess(CountRegisteredCallbacks);
			UCOLOG("[UCOnline2] Set callback registration function");

			ISteamUser* pUser = (ISteamUser*)g_pSteamClient->GetISteamUser(g_ClientUser, g_ClientPipe, STEAMUSER_INTERFACE_VERSION);
			UCOLOG("[UCOnline2] GetISteamUser returned: 0x%p", pUser);
			
			if (pUser)
			{
				uint64 sid = pUser->GetSteamID().ConvertToUint64();
				bool bLogged = pUser->BLoggedOn();
				UCOLOG("[UCOnline2] SteamUser details - SID: %llu, LoggedOn: %s", sid, bLogged ? "yes" : "no");
				
				UpdateMinidumpSteamID(sid);

				if (pUtils)
				{
					g_bClientReady = g_ClientCtx.Init();
					UCOLOG("[UCOnline2] Client context init result: %s", g_bClientReady ? "success" : "failed");
				}
				else
				{
					g_bClientReady = false;
					UCOLOG("[UCOnline2] Client context init skipped (no Utils)");
				}

				if (g_bClientReady)
				{
					UCOLOG("[UCOnline2] SteamAPI_Init successful");

					// Hook BIsSubscribedApp to always return true
					// Fixes games with hardcoded AppID subscription checks (e.g. Godot games)
					InstallBIsSubscribedAppHook();

					return k_ESteamAPIInitResult_OK;
				}
				else
				{
					UCOLOG("[UCOnline2] g_ClientCtx.Init failed");
					SteamAPI_Shutdown();
					return k_ESteamAPIInitResult_VersionMismatch;
				}
			}
			else
			{
				UCOLOG("[UCOnline2] Failed to get ISteamUser interface");
				WarnMissingInterface(g_ClientPipe, STEAMUSER_INTERFACE_VERSION);
				SteamAPI_Shutdown();
				return k_ESteamAPIInitResult_VersionMismatch;
			}
		}
		else
		{
			UCOLOG("[UCOnline2] Failed to get ISteamUtils interface");
			WarnMissingInterface(g_ClientPipe, STEAMUTILS_INTERFACE_VERSION);
			SteamAPI_Shutdown();
			return k_ESteamAPIInitResult_VersionMismatch;
		}
	}
	else
	{
		UCOLOG("[UCOnline2] Failed to create/connect Steam user (got 0)");
		SteamAPI_Shutdown();
		return k_ESteamAPIInitResult_NoSteamClient;
	}

	UCOLOG("[UCOnline2] SteamAPI_Init falling through to generic failure");
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
	UCOLOG("[UCOnline2] SteamAPI_InitSafe\r\n");
	s_bAnonUser = false;

	bool bOk = SteamAPI_Init();
	if (bOk && g_pSteamClient)
		return true;

	UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] SteamAPI_InitSafe failed\r\n");
	return false;
}

S_API bool S_CALLTYPE SteamAPI_IsSteamRunning()
{
	UCOLOG("[UCOnline2] SteamAPI_IsSteamRunning");

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
			else
			{
				UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] GetExitCodeProcess Failed (SteamAPI_IsSteamRunning)!\r\n");
			}
		}
		else
		{
			UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] OpenProcess Failed (SteamAPI_IsSteamRunning)!\r\n");
		}
	}
	else
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Unable to get the PID of the Steam process (SteamAPI_IsSteamRunning)!\r\n");
	}

	return false;
}

S_API void S_CALLTYPE SteamAPI_ReleaseCurrentThreadMemory()
{
	UCOLOG("[UCOnline2] SteamAPI_ReleaseCurrentThreadMemory\r\n");
	if (g_pfnReleaseThreadLocal)
		g_pfnReleaseThreadLocal(0);
}

S_API bool S_CALLTYPE SteamAPI_RestartAppIfNecessary(uint32 appId)
{
	UCOLOG("[UCOnline2] SteamAPI_RestartAppIfNecessary called with appId: %u", appId);
	SetAppIDEnv();
	UCOLOG("[UCOnline2] SetAppIDEnv called, returning false (no restart needed)");
	return false;
}
