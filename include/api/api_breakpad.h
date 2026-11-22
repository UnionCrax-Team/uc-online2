#if defined(_DEBUG) && defined(_WIN32)
CDumpHandler* g_pDumpHandler = nullptr;
#endif

void UpdateMinidumpSteamID(uint64 sid)
{
	UCOLOG("[UCOnline2] UpdateMinidumpSteamID -> %lld\r\n", sid);

	if (sid == 0) return;

	if (g_pfnBreakpadSetSteamID)
		UCOLOG("[UCOnline2] Caching SteamID %lld (breakpad loaded: yes)\r\n", sid);
	else
		UCOLOG("[UCOnline2] Caching SteamID %lld (breakpad loaded: no)\r\n", sid);

	g_BreakpadSteamID = sid;

	if (g_pfnBreakpadSetSteamID)
		g_pfnBreakpadSetSteamID(sid);
}

void LoadBreakpadSymbols(HMODULE hMod)
{
	UCOLOG("[UCOnline2] LoadBreakpadSymbols\r\n");

	if (!g_bUsingBreakpad)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] LoadBreakpad: UseBreakpadCrashHandler not called\r\n");
		return;
	}

	HMODULE hTarget = hMod;
	if (!hTarget)
	{
		hTarget = g_ClientModule;
		if (!hTarget) hTarget = g_ServerModule;

		if (!hTarget)
		{
			UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] LoadBreakpad: no module loaded\r\n");
			return;
		}
	}

	UCOLOG("[UCOnline2] Loading breakpad functions from steamclient\r\n");

	g_pfnBreakpadWriteDump = (Fn_BreakpadWriteDump)GetProcAddress(hTarget, "Breakpad_SteamWriteMiniDumpUsingExceptionInfoWithBuildId");
	if (!g_pfnBreakpadWriteDump) return;

	g_pfnBreakpadSetComment = (Fn_BreakpadSetComment)GetProcAddress(hTarget, "Breakpad_SteamWriteMiniDumpSetComment");
	if (!g_pfnBreakpadSetComment) return;

	g_pfnBreakpadSetSteamID = (Fn_BreakpadSetSteamID)GetProcAddress(hTarget, "Breakpad_SteamSetSteamID");
	if (!g_pfnBreakpadSetSteamID) return;

	g_pfnBreakpadSetAppID = (Fn_BreakpadSetAppID)GetProcAddress(hTarget, "Breakpad_SteamSetAppID");
	if (!g_pfnBreakpadSetAppID) return;

	g_pfnBreakpadInit = (Fn_BreakpadInit)GetProcAddress(hTarget, "Breakpad_SteamMiniDumpInit");
	if (!g_pfnBreakpadInit) return;

	UCOLOG("[UCOnline2] Initializing breakpad minidump system\r\n");
	g_pfnBreakpadInit(g_BreakpadAppID, g_BreakpadVer, g_BreakpadTimestamp, g_bFullDumps, g_BreakpadCtx, g_BreakpadCallback);

	if (g_BreakpadSteamID != 0)
		UpdateMinidumpSteamID(g_BreakpadSteamID);
}

S_API void S_CALLTYPE SteamAPI_SetBreakpadAppID(uint32 appId)
{
	UCOLOG("[UCOnline2] SteamAPI_SetBreakpadAppID -> %u\r\n", appId);

	if (g_BreakpadAppID != appId)
	{
		UCOLOG("[UCOnline2] Setting breakpad AppID: %u\r\n", appId);
		g_BreakpadAppID = appId;
	}

	if (!g_pfnBreakpadSetAppID)
		LoadBreakpadSymbols(nullptr);

	if (g_pfnBreakpadSetAppID)
		g_pfnBreakpadSetAppID(g_BreakpadAppID);
}

S_API void S_CALLTYPE SteamAPI_SetMiniDumpComment(const char* comment)
{
	UCOLOG("[UCOnline2] SteamAPI_SetMiniDumpComment\r\n");

	if (comment && strlen(comment) > 0)
	{
		#if defined(_DEBUG) && defined(_WIN32)
		if (g_pDumpHandler && g_pDumpHandler->IsReady())
		{
			g_pDumpHandler->ClearComment();

			int wLen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, comment, -1, nullptr, 0);
			if (wLen > 0)
			{
				wchar_t* wComment = new wchar_t[wLen]();
				if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, comment, -1, wComment, wLen) > 0)
				{
					g_pDumpHandler->SetComment(wComment);
				}
				delete[] wComment;
			}
		}
		#endif

		if (!g_pfnBreakpadSetComment)
			LoadBreakpadSymbols(nullptr);

		if (g_pfnBreakpadSetComment)
			g_pfnBreakpadSetComment(comment);
	}
}

S_API void S_CALLTYPE SteamAPI_UseBreakpadCrashHandler(const char* ver, const char* date, const char* time, bool bFull, void* ctx, PFNPreMinidumpCallback cb)
{
	UCOLOG("[UCOnline2] SteamAPI_UseBreakpadCrashHandler\r\n");

	SecureZeroMemory(g_BreakpadVer, sizeof(g_BreakpadVer));
	SecureZeroMemory(g_BreakpadTimestamp, sizeof(g_BreakpadTimestamp));
	g_BreakpadCtx = nullptr;
	g_BreakpadCallback = nullptr;

	g_bUsingBreakpad = true;
	g_bFullDumps = bFull;

	if (ver && strlen(ver) > 0)
		_snprintf_s(g_BreakpadVer, sizeof(g_BreakpadVer), _TRUNCATE, "%s", ver);

	g_BreakpadCtx = ctx;
	g_BreakpadCallback = cb;

	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	_snprintf_s(g_BreakpadTimestamp, sizeof(g_BreakpadTimestamp), _TRUNCATE, "%04d%02d%02d%02d%02d%02d",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

S_API void S_CALLTYPE SteamAPI_WriteMiniDump(uint32 code, void* info, uint32 buildId)
{
	UCOLOG("[UCOnline2] SteamAPI_WriteMiniDump\r\n");

	if (info)
	{
		#if defined(_DEBUG) && defined(_WIN32)
		if (g_pDumpHandler && g_pDumpHandler->IsReady())
		{
			g_pDumpHandler->WriteDump(code, (_EXCEPTION_POINTERS*)info);
			g_pDumpHandler->ClearComment();
		}
		#endif

		if (!g_pfnBreakpadWriteDump)
			LoadBreakpadSymbols(nullptr);

		if (g_pfnBreakpadWriteDump)
			g_pfnBreakpadWriteDump(code, info, buildId);
	}
}
