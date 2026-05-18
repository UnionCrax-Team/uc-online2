S_API ISteamApps* S_CALLTYPE SteamApps()
{
	UCOLOG("[UCOnline2] SteamApps\r\n");
	return g_bClientReady ? g_ClientCtx.SteamApps() : nullptr;
}

S_API ISteamClient* S_CALLTYPE SteamClient()
{
	UCOLOG("[UCOnline2] SteamClient\r\n");
	return g_pSteamClientSafe;
}

S_API ISteamController* S_CALLTYPE SteamController()
{
	UCOLOG("[UCOnline2] SteamController\r\n");
	return g_bClientReady ? g_ClientCtx.SteamController() : nullptr;
}

S_API ISteamFriends* S_CALLTYPE SteamFriends()
{
	UCOLOG("[UCOnline2] SteamFriends\r\n");
	return g_bClientReady ? g_ClientCtx.SteamFriends() : nullptr;
}

S_API ISteamClient* S_CALLTYPE SteamGameServerSteamClient()
{
	UCOLOG("[UCOnline2] SteamGameServerSteamClient\r\n");
	return g_bServerReady ? g_ServerCtx.SteamClient() : nullptr;
}

S_API ISteamGameServer* S_CALLTYPE SteamGameServer()
{
	UCOLOG("[UCOnline2] SteamGameServer\r\n");
	return g_bServerReady ? g_ServerCtx.SteamGameServer() : nullptr;
}

S_API ISteamApps* S_CALLTYPE SteamGameServerApps()
{
	UCOLOG("[UCOnline2] SteamGameServerApps\r\n");
	return g_bServerReady ? g_ServerCtx.SteamApps() : nullptr;
}

S_API ISteamHTTP* S_CALLTYPE SteamGameServerHTTP()
{
	UCOLOG("[UCOnline2] SteamGameServerHTTP\r\n");
	return g_bServerReady ? g_ServerCtx.SteamHTTP() : nullptr;
}

S_API ISteamInventory* S_CALLTYPE SteamGameServerInventory()
{
	UCOLOG("[UCOnline2] SteamGameServerInventory\r\n");
	return g_bServerReady ? g_ServerCtx.SteamInventory() : nullptr;
}

S_API ISteamNetworking* S_CALLTYPE SteamGameServerNetworking()
{
	UCOLOG("[UCOnline2] SteamGameServerNetworking\r\n");
	return g_bServerReady ? g_ServerCtx.SteamGameServerNetworking() : nullptr;
}

S_API ISteamGameServerStats* S_CALLTYPE SteamGameServerStats()
{
	UCOLOG("[UCOnline2] SteamGameServerStats\r\n");
	return g_bServerReady ? g_ServerCtx.SteamGameServerStats() : nullptr;
}

S_API ISteamUGC* S_CALLTYPE SteamGameServerUGC()
{
	UCOLOG("[UCOnline2] SteamGameServerUGC\r\n");
	return g_bServerReady ? g_ServerCtx.SteamUGC() : nullptr;
}

S_API ISteamUtils* S_CALLTYPE SteamGameServerUtils()
{
	UCOLOG("[UCOnline2] SteamGameServerUtils\r\n");
	return g_bServerReady ? g_ServerCtx.SteamGameServerUtils() : nullptr;
}

S_API ISteamGameSearch* S_CALLTYPE SteamGameSearch()
{
	UCOLOG("[UCOnline2] SteamGameSearch\r\n");
	return g_bClientReady ? g_ClientCtx.SteamGameSearch() : nullptr;
}

S_API ISteamHTMLSurface* S_CALLTYPE SteamHTMLSurface()
{
	UCOLOG("[UCOnline2] SteamHTMLSurface\r\n");
	return g_bClientReady ? g_ClientCtx.SteamHTMLSurface() : nullptr;
}

S_API ISteamHTTP* S_CALLTYPE SteamHTTP()
{
	UCOLOG("[UCOnline2] SteamHTTP\r\n");
	return g_bClientReady ? g_ClientCtx.SteamHTTP() : nullptr;
}

S_API ISteamInput* S_CALLTYPE SteamInput()
{
	UCOLOG("[UCOnline2] SteamInput\r\n");
	return g_bClientReady ? g_ClientCtx.SteamInput() : nullptr;
}

S_API ISteamInventory* S_CALLTYPE SteamInventory()
{
	UCOLOG("[UCOnline2] SteamInventory\r\n");
	return g_bClientReady ? g_ClientCtx.SteamInventory() : nullptr;
}

S_API ISteamMatchmaking* S_CALLTYPE SteamMatchmaking()
{
	UCOLOG("[UCOnline2] SteamMatchmaking\r\n");
	return g_bClientReady ? g_ClientCtx.SteamMatchmaking() : nullptr;
}

S_API ISteamMatchmakingServers* S_CALLTYPE SteamMatchmakingServers()
{
	UCOLOG("[UCOnline2] SteamMatchmakingServers\r\n");
	return g_bClientReady ? g_ClientCtx.SteamMatchmakingServers() : nullptr;
}

S_API ISteamMusic* S_CALLTYPE SteamMusic()
{
	UCOLOG("[UCOnline2] SteamMusic\r\n");
	return g_bClientReady ? g_ClientCtx.SteamMusic() : nullptr;
}

S_API ISteamMusicRemote* S_CALLTYPE SteamMusicRemote()
{
	UCOLOG("[UCOnline2] SteamMusicRemote\r\n");
	return &s_MusicRemoteStub;
}

S_API ISteamNetworking* S_CALLTYPE SteamNetworking()
{
	UCOLOG("[UCOnline2] SteamNetworking\r\n");
	return g_bClientReady ? g_ClientCtx.SteamNetworking() : nullptr;
}

S_API ISteamParentalSettings* S_CALLTYPE SteamParentalSettings()
{
	UCOLOG("[UCOnline2] SteamParentalSettings\r\n");
	return g_bClientReady ? g_ClientCtx.SteamParentalSettings() : nullptr;
}

S_API ISteamParties* S_CALLTYPE SteamParties()
{
	UCOLOG("[UCOnline2] SteamParties\r\n");
	return g_bClientReady ? g_ClientCtx.SteamParties() : nullptr;
}

S_API ISteamRemotePlay* S_CALLTYPE SteamRemotePlay()
{
	UCOLOG("[UCOnline2] SteamRemotePlay\r\n");
	return g_bClientReady ? g_ClientCtx.SteamRemotePlay() : nullptr;
}

S_API ISteamRemoteStorage* S_CALLTYPE SteamRemoteStorage()
{
	UCOLOG("[UCOnline2] SteamRemoteStorage\r\n");
	return g_bClientReady ? g_ClientCtx.SteamRemoteStorage() : nullptr;
}

S_API ISteamScreenshots* S_CALLTYPE SteamScreenshots()
{
	UCOLOG("[UCOnline2] SteamScreenshots\r\n");
	return g_bClientReady ? g_ClientCtx.SteamScreenshots() : nullptr;
}

S_API ISteamUGC* S_CALLTYPE SteamUGC()
{
	UCOLOG("[UCOnline2] SteamUGC\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUGC() : nullptr;
}

S_API ISteamUser* S_CALLTYPE SteamUser()
{
	UCOLOG("[UCOnline2] SteamUser\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUser() : nullptr;
}

S_API ISteamUserStats* S_CALLTYPE SteamUserStats()
{
	UCOLOG("[UCOnline2] SteamUserStats\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUserStats() : nullptr;
}

S_API ISteamUtils* S_CALLTYPE SteamUtils()
{
	UCOLOG("[UCOnline2] SteamUtils\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUtils() : nullptr;
}

S_API ISteamVideo* S_CALLTYPE SteamVideo()
{
	UCOLOG("[UCOnline2] SteamVideo\r\n");
	return g_bClientReady ? g_ClientCtx.SteamVideo() : nullptr;
}
