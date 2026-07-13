static bool s_bDispatcherInited = false;
CCallbackDispatcher* GetDispatcher();

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

static void SetAppIDEnv()
{
	char szApp[16] = { 0 };
	char szGame[32] = { 0 };
	char szOverlayGame[32] = { 0 };

	_snprintf_s(szApp, sizeof(szApp), _TRUNCATE, "%u", g_ForcedAppId);
	_snprintf_s(szGame, sizeof(szGame), _TRUNCATE, "%llu", CGameID(g_ForcedAppId).ToUint64());

	uint32 overlayAppId = (g_OriginalAppId != 0) ? g_OriginalAppId : g_ForcedAppId;
	_snprintf_s(szOverlayGame, sizeof(szOverlayGame), _TRUNCATE, "%llu", CGameID(overlayAppId).ToUint64());

	SetEnvironmentVariableA("SteamAppId", szApp);
	SetEnvironmentVariableA("SteamGameId", szGame);
	SetEnvironmentVariableA("SteamOverlayGameId", szOverlayGame);
}

static void WriteAppIDFile()
{
	char buf[16] = { 0 };
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "%u\n", g_ForcedAppId);

	FILE* f = nullptr;
	if (fopen_s(&f, "steam_appid.txt", "wb") == 0 && f)
	{
		fwrite(buf, 1, strlen(buf), f);
		fclose(f);
	}

	char exePath[MAX_PATH] = { 0 };
	if (GetModuleFileNameA(nullptr, exePath, MAX_PATH) != 0)
	{
		if (PathRemoveFileSpecA(exePath))
		{
			char fullPath[MAX_PATH] = { 0 };
			_snprintf_s(fullPath, sizeof(fullPath), _TRUNCATE, "%s\\steam_appid.txt", exePath);

			f = nullptr;
			if (fopen_s(&f, fullPath, "wb") == 0 && f)
			{
				fwrite(buf, 1, strlen(buf), f);
				fclose(f);
			}
		}
	}
}

S_API HSteamPipe S_CALLTYPE GetHSteamPipe()
{
	// Hot path (called per-frame for callback dispatch) -- not logged to avoid flooding the log.
	return g_ClientPipe;
}

S_API HSteamUser S_CALLTYPE GetHSteamUser()
{
	UCOLOG("[UCOnline2] GetHSteamUser\r\n");
	return g_ClientUser;
}

S_API HSteamPipe S_CALLTYPE SteamAPI_GetHSteamPipe()
{
	// Hot path (called per-frame for callback dispatch) -- not logged to avoid flooding the log.
	return g_ClientPipe;
}

S_API HSteamUser S_CALLTYPE SteamAPI_GetHSteamUser()
{
	UCOLOG("[UCOnline2] SteamAPI_GetHSteamUser\r\n");
	return g_ClientUser;
}

S_API const char* S_CALLTYPE SteamAPI_GetSteamInstallPath()
{
	UCOLOG("[UCOnline2] SteamAPI_GetSteamInstallPath");

	if (g_bHaveInstallPath)
		return g_InstallPath;

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
				LSTATUS GetDllString = ERROR_SUCCESS;

				#if defined(_M_IX86)
					GetDllString = GetRegistryString("Software\\Valve\\Steam\\ActiveProcess", "SteamClientDll", g_InstallPath, MAX_PATH);
				#elif defined(_M_AMD64)
					GetDllString = GetRegistryString("Software\\Valve\\Steam\\ActiveProcess", "SteamClientDll64", g_InstallPath, MAX_PATH);
				#endif

				if (GetDllString == ERROR_SUCCESS)
				{
					BOOL FixPath = PathRemoveFileSpecA(g_InstallPath);

					if (FixPath == TRUE)
					{
						g_bHaveInstallPath = true;

						UCOLOG("[UCOnline2] Steam Install Path --> %s\r\n", g_InstallPath);

						return g_InstallPath;
					}
					else
					{
						UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] PathRemoveFileSpecA Failed (SteamAPI_GetSteamInstallPath)!\r\n");
					}
				}
				else
				{
					UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Unable to get steamclient(64).dll path (SteamAPI_GetSteamInstallPath)!\r\n");
				}
			}
			else
			{
				UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] GetExitCodeProcess Failed (SteamAPI_GetSteamInstallPath)!\r\n");
			}
		}
		else
		{
			UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] OpenProcess Failed (SteamAPI_GetSteamInstallPath)!\r\n");
		}
	}
	else
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Unable to get the PID of the Steam process (SteamAPI_GetSteamInstallPath)!\r\n");
	}

	return "UCOnline2_InvalidPath";
}

S_API ESteamAPIInitResult S_CALLTYPE SteamInternal_SteamAPI_Init(const char* pszVersions, SteamErrMsg* pOutErr)
{
	UCOLOG("[UCOnline2] SteamAPI_Init\r\n");
	SetAppIDEnv();
	WriteAppIDFile();
	UCOLOG("[UCOnline2] AppID forced to %u", g_ForcedAppId);

	if (g_pSteamClient)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Init already called, use Shutdown first\r\n");
		return k_ESteamAPIInitResult_OK;
	}

	g_pSteamClient = static_cast<ISteamClient*>(InitSteamClient(&g_ClientModule, s_bAnonUser, STEAMCLIENT_INTERFACE_VERSION));

	if (!g_pSteamClient)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Failed to get SteamClient\r\n");
		SteamAPI_Shutdown();
		return k_ESteamAPIInitResult_FailedGeneric;
	}

	SetAppIDEnv();

	if (!s_bAnonUser)
	{
		g_ClientPipe = g_pSteamClient->CreateSteamPipe();
		if (g_ClientPipe == 0)
		{
			UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] CreateSteamPipe failed\r\n");
			SteamAPI_Shutdown();
			return k_ESteamAPIInitResult_NoSteamClient;
		}

		g_ClientUser = g_pSteamClient->ConnectToGlobalUser(g_ClientPipe);
		UCOLOG("[UCOnline2] ConnectToGlobalUser -> %u\r\n", (uint32)g_ClientUser);
	}
	else
	{
		g_ClientUser = g_pSteamClient->CreateLocalUser(&g_ClientPipe, k_EAccountTypeAnonUser);
		UCOLOG("[UCOnline2] CreateLocalUser -> %u\r\n", (uint32)g_ClientUser);
	}

	if (g_ClientUser != 0)
	{
		if (pszVersions)
		{
			HMODULE hMod = g_ClientModule;
			if (g_ServerModule) hMod = g_ServerModule;

			g_pfnIsKnownInterface = (Fn_IsKnownInterface)GetProcAddress(hMod, "Steam_IsKnownInterface");
			// Steam_IsKnownInterface is optional. If it's absent
			// (some older steamclient builds don't expose it), don't
			// fail init -- just skip the validation. Steamworks.NET
			// 20.x passes pszVersions on every init and would
			// otherwise hard-fail here on perfectly viable Steam
			// installs.
			if (!g_pfnIsKnownInterface)
			{
				UCOLOG("[UCOnline2] Steam_IsKnownInterface not exported by steamclient; "
				       "skipping interface-version pre-validation\r\n");
			}
		}

		ISteamUtils* pUtils = (ISteamUtils*)g_pSteamClient->GetISteamUtils(g_ClientPipe, STEAMUTILS_INTERFACE_VERSION);
		if (pUtils)
		{
			uint32 reportedID = pUtils->GetAppID();

			if (reportedID == 0)
				UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Steam reported AppID 0, forcing 480\r\n");

			SetAppIDEnv();
			SteamAPI_SetBreakpadAppID(g_ForcedAppId);
			Steam_RegisterInterfaceFuncs(g_ClientModule);
			LoadBreakpadSymbols(g_ClientModule);

			g_pSteamClient->Set_SteamAPI_CCheckCallbackRegisteredInProcess(CountRegisteredCallbacks);

			ISteamUser* pUser = (ISteamUser*)g_pSteamClient->GetISteamUser(g_ClientUser, g_ClientPipe, STEAMUSER_INTERFACE_VERSION);
			if (pUser)
			{
				uint64 sid = pUser->GetSteamID().ConvertToUint64();
				bool bLogged = pUser->BLoggedOn();
				UpdateMinidumpSteamID(sid);

				UCOLOG("[UCOnline2] SteamUser OK - SID=%llu Logged=%s\r\n", sid, bLogged ? "yes" : "no");
			}
			else
			{
				WarnMissingInterface(g_ClientPipe, STEAMUSER_INTERFACE_VERSION);
				SteamAPI_Shutdown();
				return k_ESteamAPIInitResult_VersionMismatch;
			}

			g_bClientReady = g_ClientCtx.Init();

			if (g_bClientReady)
			{
				// Diagnostic: probe each interface Steamworks.NET 20.x
				// CSteamAPIContext.Init() asks for, log any nullptr.
				// Identifies which lookup makes the game-side init fail.
				struct IfaceProbe { const char* name; void* result; };
				ISteamClient* sc = g_pSteamClient;
				IfaceProbe probes[] = {
					{ "SteamUser023",                          sc->GetISteamUser(g_ClientUser, g_ClientPipe, "SteamUser023") },
					{ "SteamFriends018",                       sc->GetISteamFriends(g_ClientUser, g_ClientPipe, "SteamFriends018") },
					{ "SteamUtils010",                         sc->GetISteamUtils(g_ClientPipe, "SteamUtils010") },
					{ "SteamMatchMaking009",                   sc->GetISteamMatchmaking(g_ClientUser, g_ClientPipe, "SteamMatchMaking009") },
					{ "SteamMatchMakingServers002",            sc->GetISteamMatchmakingServers(g_ClientUser, g_ClientPipe, "SteamMatchMakingServers002") },
					{ "STEAMUSERSTATS_INTERFACE_VERSION013",   sc->GetISteamUserStats(g_ClientUser, g_ClientPipe, "STEAMUSERSTATS_INTERFACE_VERSION013") },
					{ "STEAMAPPS_INTERFACE_VERSION008",        sc->GetISteamApps(g_ClientUser, g_ClientPipe, "STEAMAPPS_INTERFACE_VERSION008") },
					{ "SteamNetworking006",                    sc->GetISteamNetworking(g_ClientUser, g_ClientPipe, "SteamNetworking006") },
					{ "STEAMREMOTESTORAGE_INTERFACE_VERSION016", sc->GetISteamRemoteStorage(g_ClientUser, g_ClientPipe, "STEAMREMOTESTORAGE_INTERFACE_VERSION016") },
					{ "STEAMSCREENSHOTS_INTERFACE_VERSION003", sc->GetISteamScreenshots(g_ClientUser, g_ClientPipe, "STEAMSCREENSHOTS_INTERFACE_VERSION003") },
					{ "SteamMatchGameSearch001",               sc->GetISteamGenericInterface(g_ClientUser, g_ClientPipe, "SteamMatchGameSearch001") },
					{ "STEAMHTTP_INTERFACE_VERSION003",        sc->GetISteamHTTP(g_ClientUser, g_ClientPipe, "STEAMHTTP_INTERFACE_VERSION003") },
					{ "STEAMUGC_INTERFACE_VERSION021",         sc->GetISteamUGC(g_ClientUser, g_ClientPipe, "STEAMUGC_INTERFACE_VERSION021") },
					{ "STEAMMUSIC_INTERFACE_VERSION001",       sc->GetISteamMusic(g_ClientUser, g_ClientPipe, "STEAMMUSIC_INTERFACE_VERSION001") },
					{ "STEAMMUSICREMOTE_INTERFACE_VERSION001", sc->GetISteamGenericInterface(g_ClientUser, g_ClientPipe, "STEAMMUSICREMOTE_INTERFACE_VERSION001") },
					{ "STEAMHTMLSURFACE_INTERFACE_VERSION_005",sc->GetISteamHTMLSurface(g_ClientUser, g_ClientPipe, "STEAMHTMLSURFACE_INTERFACE_VERSION_005") },
					{ "STEAMINVENTORY_INTERFACE_V003",         sc->GetISteamInventory(g_ClientUser, g_ClientPipe, "STEAMINVENTORY_INTERFACE_V003") },
					{ "STEAMVIDEO_INTERFACE_V007",             sc->GetISteamVideo(g_ClientUser, g_ClientPipe, "STEAMVIDEO_INTERFACE_V007") },
					{ "STEAMPARENTALSETTINGS_INTERFACE_VERSION001", sc->GetISteamParentalSettings(g_ClientUser, g_ClientPipe, "STEAMPARENTALSETTINGS_INTERFACE_VERSION001") },
					{ "SteamInput006",                         sc->GetISteamInput(g_ClientUser, g_ClientPipe, "SteamInput006") },
					{ "SteamParties002",                       sc->GetISteamParties(g_ClientUser, g_ClientPipe, "SteamParties002") },
					{ "STEAMREMOTEPLAY_INTERFACE_VERSION003",  sc->GetISteamRemotePlay(g_ClientUser, g_ClientPipe, "STEAMREMOTEPLAY_INTERFACE_VERSION003") },
					// Final 4: Steamworks.NET 20.x CSteamAPIContext.Init()
					// also probes these via FindOrCreateUserInterface.
					// If any returns null, managed Init() returns false.
					{ "SteamNetworkingUtils004",               sc->GetISteamGenericInterface(g_ClientUser, g_ClientPipe, "SteamNetworkingUtils004") },
					{ "SteamNetworkingSockets012",             sc->GetISteamGenericInterface(g_ClientUser, g_ClientPipe, "SteamNetworkingSockets012") },
					{ "SteamNetworkingMessages002",            sc->GetISteamGenericInterface(g_ClientUser, g_ClientPipe, "SteamNetworkingMessages002") },
					{ "STEAMTIMELINE_INTERFACE_V004",          sc->GetISteamGenericInterface(g_ClientUser, g_ClientPipe, "STEAMTIMELINE_INTERFACE_V004") },
				};
				for (size_t i = 0; i < sizeof(probes)/sizeof(probes[0]); ++i) {
					if (probes[i].result == nullptr)
						UCOLOG("[UCOnline2] probe FAIL: %s -> nullptr\r\n", probes[i].name);
				}

				InstallSteamSpoofHooks();

				// Build the plugin context now that Steam interfaces
				// are resolved, then let each plugin install its
				// game-specific hooks.
				UCO_PluginContext ctx = {};
				ctx.ApiVersion       = UCO_PLUGIN_API_VERSION;
				ctx.StructSize       = sizeof(UCO_PluginContext);
				ctx.ForcedAppId      = g_ForcedAppId;
				ctx.OriginalAppId    = g_OriginalAppId;
				ctx.pSteamClient     = g_ClientCtx.SteamClient();
				ctx.pSteamUser       = g_ClientCtx.SteamUser();
				ctx.pSteamUtils      = g_ClientCtx.SteamUtils();
				ctx.pSteamApps       = g_ClientCtx.SteamApps();
				ctx.pSteamFriends    = g_ClientCtx.SteamFriends();
				ctx.pSteamMatchmaking= g_ClientCtx.SteamMatchmaking();
				ctx.HSteamUser       = g_ClientUser;
				ctx.HSteamPipe       = g_ClientPipe;
				ctx.Log              = &UCOLOG;
				ctx.RegisterCallbackPatcher = &UCO_RegisterCallbackPatcher;
				s_PluginLoader.InitPlugins(&ctx);

				return k_ESteamAPIInitResult_OK;
			}

			if (!g_bClientReady)
			{
				UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] g_ClientCtx.Init failed\r\n");
				SteamAPI_Shutdown();
				return k_ESteamAPIInitResult_VersionMismatch;
			}

			return k_ESteamAPIInitResult_OK;
		}
		else
		{
			UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Failed to get ISteamUtils\r\n");
			WarnMissingInterface(g_ClientPipe, STEAMUTILS_INTERFACE_VERSION);
			SteamAPI_Shutdown();
			return k_ESteamAPIInitResult_VersionMismatch;
		}
	}
	else
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Failed to connect/create user\r\n");
		SteamAPI_Shutdown();
		return k_ESteamAPIInitResult_NoSteamClient;
	}

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
	UCOLOG("[UCOnline2] SteamAPI_RestartAppIfNecessary\r\n");
	SetAppIDEnv();
	return false;
}
