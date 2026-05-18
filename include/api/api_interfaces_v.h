S_API ISteamUser* S_CALLTYPE SteamAPI_SteamUser_v023()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamUser_v023\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUser() : nullptr;
}

S_API ISteamUserStats* S_CALLTYPE SteamAPI_SteamUserStats_v013()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamUserStats_v013\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUserStats() : nullptr;
}

S_API ISteamUtils* S_CALLTYPE SteamAPI_SteamUtils_v010()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamUtils_v010\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUtils() : nullptr;
}

S_API ISteamUGC* S_CALLTYPE SteamAPI_SteamUGC_v021()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamUGC_v021\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUGC() : nullptr;
}

S_API ISteamFriends* S_CALLTYPE SteamAPI_SteamFriends_v018()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamFriends_v018\r\n");
	return g_bClientReady ? g_ClientCtx.SteamFriends() : nullptr;
}

S_API ISteamUtils* S_CALLTYPE SteamAPI_SteamGameServerUtils_v010()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerUtils_v010\r\n");
	return g_bServerReady ? g_ServerCtx.SteamGameServerUtils() : nullptr;
}

S_API ISteamUGC* S_CALLTYPE SteamAPI_SteamGameServerUGC_v021()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerUGC_v021\r\n");
	return g_bServerReady ? g_ServerCtx.SteamUGC() : nullptr;
}

S_API ISteamApps* S_CALLTYPE SteamAPI_SteamGameServerApps_v008()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerApps_v008\r\n");
	return g_bServerReady ? g_ServerCtx.SteamApps() : nullptr;
}

S_API ISteamNetworking* S_CALLTYPE SteamAPI_SteamGameServerNetworking_v006()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerNetworking_v006\r\n");
	return g_bServerReady ? g_ServerCtx.SteamGameServerNetworking() : nullptr;
}

S_API void* S_CALLTYPE SteamAPI_SteamGameServerNetworkingMessages_SteamAPI_v002()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerNetworkingMessages_SteamAPI_v002\r\n");
	return SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
}

S_API void* S_CALLTYPE SteamAPI_SteamGameServerNetworkingSockets_SteamAPI_v012()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerNetworkingSockets_SteamAPI_v012\r\n");
	return SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
}

S_API ISteamHTTP* S_CALLTYPE SteamAPI_SteamGameServerHTTP_v003()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerHTTP_v003\r\n");
	return g_bServerReady ? g_ServerCtx.SteamHTTP() : nullptr;
}

S_API ISteamInventory* S_CALLTYPE SteamAPI_SteamGameServerInventory_v003()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerInventory_v003\r\n");
	return g_bServerReady ? g_ServerCtx.SteamInventory() : nullptr;
}

S_API ISteamGameServer* S_CALLTYPE SteamAPI_SteamGameServer_v015()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServer_v015\r\n");
	return g_bServerReady ? g_pGameServer : nullptr;
}

S_API ISteamGameServerStats* S_CALLTYPE SteamAPI_SteamGameServerStats_v001()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerStats_v001\r\n");
	return g_bServerReady ? g_ServerCtx.SteamGameServerStats() : nullptr;
}

S_API ISteamMatchmaking* S_CALLTYPE SteamAPI_SteamMatchmaking_v009()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamMatchmaking_v009\r\n");
	return g_bClientReady ? g_ClientCtx.SteamMatchmaking() : nullptr;
}

S_API ISteamMatchmakingServers* S_CALLTYPE SteamAPI_SteamMatchmakingServers_v002()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamMatchmakingServers_v002\r\n");
	return g_bClientReady ? g_ClientCtx.SteamMatchmakingServers() : nullptr;
}

S_API ISteamMusic* S_CALLTYPE SteamAPI_SteamMusic_v001()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamMusic_v001\r\n");
	return g_bClientReady ? g_ClientCtx.SteamMusic() : nullptr;
}

S_API ISteamParties* S_CALLTYPE SteamAPI_SteamParties_v002()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamParties_v002\r\n");
	return g_bClientReady ? g_ClientCtx.SteamParties() : nullptr;
}

S_API ISteamParentalSettings* S_CALLTYPE SteamAPI_SteamParentalSettings_v001()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamParentalSettings_v001\r\n");
	return g_bClientReady ? g_ClientCtx.SteamParentalSettings() : nullptr;
}

S_API ISteamRemoteStorage* S_CALLTYPE SteamAPI_SteamRemoteStorage_v016()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamRemoteStorage_v016\r\n");
	return g_bClientReady ? g_ClientCtx.SteamRemoteStorage() : nullptr;
}

S_API ISteamRemotePlay* S_CALLTYPE SteamAPI_SteamRemotePlay_v003()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamRemotePlay_v003\r\n");
	return g_bClientReady ? g_ClientCtx.SteamRemotePlay() : nullptr;
}

S_API ISteamApps* S_CALLTYPE SteamAPI_SteamApps_v008()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamApps_v008\r\n");
	return g_bClientReady ? g_ClientCtx.SteamApps() : nullptr;
}

S_API ISteamNetworking* S_CALLTYPE SteamAPI_SteamNetworking_v006()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamNetworking_v006\r\n");
	return g_bClientReady ? g_ClientCtx.SteamNetworking() : nullptr;
}

S_API void* S_CALLTYPE SteamAPI_SteamNetworkingSockets_SteamAPI_v012()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamNetworkingSockets_SteamAPI_v012\r\n");
	return SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
}

S_API void* S_CALLTYPE SteamAPI_SteamNetworkingUtils_SteamAPI_v004()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamNetworkingUtils_SteamAPI_v004\r\n");

	void* pResult = SteamInternal_FindOrCreateUserInterface(0, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	if (!pResult)
		pResult = SteamInternal_FindOrCreateGameServerInterface(0, STEAMNETWORKINGUTILS_INTERFACE_VERSION);

	return pResult;
}

S_API void* S_CALLTYPE SteamAPI_SteamNetworkingMessages_SteamAPI_v002()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamNetworkingMessages_SteamAPI_v002\r\n");
	return SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
}

S_API ISteamScreenshots* S_CALLTYPE SteamAPI_SteamScreenshots_v003()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamScreenshots_v003\r\n");
	return g_bClientReady ? g_ClientCtx.SteamScreenshots() : nullptr;
}

S_API void* S_CALLTYPE SteamAPI_SteamTimeline_v004()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamTimeline_v004\r\n");

	if (!g_bClientReady) return nullptr;
	return SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
}

S_API ISteamHTTP* S_CALLTYPE SteamAPI_SteamHTTP_v003()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamHTTP_v003\r\n");
	return g_bClientReady ? g_ClientCtx.SteamHTTP() : nullptr;
}

S_API ISteamHTMLSurface* S_CALLTYPE SteamAPI_SteamHTMLSurface_v005()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamHTMLSurface_v005\r\n");
	return g_bClientReady ? g_ClientCtx.SteamHTMLSurface() : nullptr;
}

S_API ISteamInput* S_CALLTYPE SteamAPI_SteamInput_v006()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamInput_v006\r\n");
	return g_bClientReady ? g_ClientCtx.SteamInput() : nullptr;
}

S_API ISteamInventory* S_CALLTYPE SteamAPI_SteamInventory_v003()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamInventory_v003\r\n");
	return g_bClientReady ? g_ClientCtx.SteamInventory() : nullptr;
}

S_API ISteamController* S_CALLTYPE SteamAPI_SteamController_v008()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamController_v008\r\n");
	return g_bClientReady ? g_ClientCtx.SteamController() : nullptr;
}

S_API ISteamVideo* S_CALLTYPE SteamAPI_SteamVideo_v007()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamVideo_v007\r\n");
	return g_bClientReady ? g_ClientCtx.SteamVideo() : nullptr;
}
// ============================================================
// Old-version aliases for compatibility with older Steamworks.NET
// ============================================================

S_API ISteamGameSearch* S_CALLTYPE SteamAPI_SteamGameSearch_v001()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameSearch_v001\r\n");
	return g_bClientReady ? g_ClientCtx.SteamGameSearch() : nullptr;
}

S_API ISteamMusicRemote* S_CALLTYPE SteamAPI_SteamMusicRemote_v001()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamMusicRemote_v001\r\n");
	return &s_MusicRemoteStub;
}

S_API void* S_CALLTYPE SteamAPI_SteamTimeline_v001()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamTimeline_v001\r\n");
	if (!g_bClientReady) return nullptr;
	return SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
}

S_API ISteamFriends* S_CALLTYPE SteamAPI_SteamFriends_v017()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamFriends_v017\r\n");
	return g_bClientReady ? g_ClientCtx.SteamFriends() : nullptr;
}

S_API ISteamUserStats* S_CALLTYPE SteamAPI_SteamUserStats_v012()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamUserStats_v012\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUserStats() : nullptr;
}

S_API ISteamUGC* S_CALLTYPE SteamAPI_SteamUGC_v020()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamUGC_v020\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUGC() : nullptr;
}

S_API ISteamUGC* S_CALLTYPE SteamAPI_SteamGameServerUGC_v020()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerUGC_v020\r\n");
	return g_bServerReady ? g_ServerCtx.SteamUGC() : nullptr;
}

S_API ISteamRemotePlay* S_CALLTYPE SteamAPI_SteamRemotePlay_v002()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamRemotePlay_v002\r\n");
	return g_bClientReady ? g_ClientCtx.SteamRemotePlay() : nullptr;
}
