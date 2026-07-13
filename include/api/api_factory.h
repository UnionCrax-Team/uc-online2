S_API void* S_CALLTYPE SteamInternal_CreateInterface(const char* ver)
{
	if (ver)
	{
		UCOLOG("[UCOnline2] SteamInternal_CreateInterface -> %s\r\n", ver);

		HMODULE hMod = g_ClientModule;
		if (g_ServerModule) hMod = g_ServerModule;

		if (hMod)
		{
			// For SteamClient* version requests, prefer our cached
			// SteamClient (acquired via STEAMCLIENT_INTERFACE_VERSION
			// during init) over whatever real Steam returns. Real
			// Steam's older SteamClient* versions (e.g. SteamClient021)
			// often come back as deprecated stubs that return null for
			// all GetISteamUser/Friends/etc calls -- which makes
			// Steamworks.NET 20.x's CSteamAPIContext.Init() fail. The
			// ISteamClient vtable is additive across versions so the
			// newer cached interface is safe to hand to older callers.
			//
			// Asked-for current version, or non-SteamClient interfaces,
			// pass through to real Steam unchanged.
			if (g_pSteamClient && _strnicmp(ver, "SteamClient", 11) == 0
			    && strcmp(ver, STEAMCLIENT_INTERFACE_VERSION) != 0)
			{
				UCOLOG("[UCOnline2] CreateInterface: substituting cached %s "
				       "for requested %s (vtable-compatible)\r\n",
				       STEAMCLIENT_INTERFACE_VERSION, ver);
				return g_pSteamClient;
			}

			g_pfnCreateInterface = (Fn_CreateInterface)GetProcAddress(hMod, "CreateInterface");
			if (g_pfnCreateInterface)
			{
				void* result = g_pfnCreateInterface(ver, nullptr);
				if (result) return result;

				// Last-ditch: any SteamClient* still unresolved falls
				// back to our cached one.
				if (g_pSteamClient && _strnicmp(ver, "SteamClient", 11) == 0)
				{
					UCOLOG("[UCOnline2] CreateInterface: real Steam returned null for %s; "
					       "falling back to cached SteamClient\r\n", ver);
					return g_pSteamClient;
				}
			}
		}
	}

	UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] CreateInterface: returning null\r\n");
	return nullptr;
}

S_API void* S_CALLTYPE SteamGameServerInternal_CreateInterface(const char* iface)
{
	if (iface)
	{
		UCOLOG("[UCOnline2] SteamGameServerInternal_CreateInterface -> %s\r\n", iface);

		if (g_ServerModule)
		{
			g_pfnCreateInterface = (Fn_CreateInterface)GetProcAddress(g_ServerModule, "CreateInterface");
			if (g_pfnCreateInterface)
				return g_pfnCreateInterface(iface, nullptr);
		}
	}

	UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] GameServerCreateInterface: returning null\r\n");
	return nullptr;
}

S_API void* S_CALLTYPE SteamInternal_FindOrCreateUserInterface(HSteamUser hUser, const char* ver)
{
	if (ver)
	{
		UCOLOG("[UCOnline2] FindOrCreateUserInterface -> %s\r\n", ver);

		if (g_pSteamClient && g_ClientPipe != 0)
		{
			void* pIface = g_pSteamClient->GetISteamGenericInterface(hUser, g_ClientPipe, ver);
			if (pIface)
				return pIface;

			// Steamworks.NET 20.x CSteamAPIContext.Init() null-checks
			// every interface and returns false if ANY is missing.
			// Some newer interfaces (Timeline, NetworkingMessages,
			// etc.) may not exist on the user's Steam client version.
			// Return a zero-filled stub so the null-check passes;
			// games that never call methods on the missing interface
			// work fine, games that do will crash at the call site.
			static char s_StubVtable[4096] = {};
			UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY,
				"[UCOnline2] FindOrCreateUserInterface: real Steam returned null for ");
			UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, ver);
			UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY,
				"; returning stub to prevent init failure\r\n");
			WarnMissingInterface(g_ClientPipe, ver);
			return (void*)s_StubVtable;
		}

		char msg[MAX_PATH] = { 0 };
		_snprintf_s(msg, MAX_PATH, _TRUNCATE, "[UCOnline2] Tried to access %s before SteamAPI_Init\r\n", ver);
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, msg);
	}

	UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] FindOrCreateUserInterface: returning null\r\n");
	return nullptr;
}

S_API void* S_CALLTYPE SteamInternal_FindOrCreateGameServerInterface(HSteamUser hUser, const char* ver)
{
	if (ver)
	{
		UCOLOG("[UCOnline2] FindOrCreateGameServerInterface -> %s\r\n", ver);

		if (g_ServerClient && g_ServerPipe != 0)
		{
			void* pIface = g_ServerClient->GetISteamGenericInterface(hUser, g_ServerPipe, ver);
			if (pIface)
				return pIface;

			// Same stub logic as the user-interface path above.
			static char s_ServerStubVtable[4096] = {};
			UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY,
				"[UCOnline2] FindOrCreateGameServerInterface: real Steam returned null for ");
			UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, ver);
			UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY,
				"; returning stub to prevent init failure\r\n");
			WarnMissingInterface(g_ServerPipe, ver);
			return (void*)s_ServerStubVtable;
		}

		char msg[MAX_PATH] = { 0 };
		_snprintf_s(msg, MAX_PATH, _TRUNCATE, "[UCOnline2] Tried to access %s before GameServer_Init\r\n", ver);
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, msg);
	}

	UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] FindOrCreateGameServerInterface: returning null\r\n");
	return nullptr;
}
