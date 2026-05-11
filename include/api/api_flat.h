S_API void S_CALLTYPE SteamAPI_MatchMakingKeyValuePair_t_Construct(MatchMakingKeyValuePair_t* self)
{
	*self = MatchMakingKeyValuePair_t();
}
S_API bool S_CALLTYPE SteamAPI_SteamIPAddress_t_IsSet(SteamIPAddress_t* self)
{
	return self->IsSet();
}
S_API void S_CALLTYPE SteamAPI_servernetadr_t_Construct(servernetadr_t* self)
{
	*self = servernetadr_t();
}
S_API void S_CALLTYPE SteamAPI_servernetadr_t_Init(servernetadr_t* self, unsigned int ip, uint16 usQueryPort, uint16 usConnectionPort)
{
	return self->Init(ip, usQueryPort, usConnectionPort);
}
S_API uint16 S_CALLTYPE SteamAPI_servernetadr_t_GetQueryPort(servernetadr_t* self)
{
	return self->GetQueryPort();
}
S_API void S_CALLTYPE SteamAPI_servernetadr_t_SetQueryPort(servernetadr_t* self, uint16 usPort)
{
	return self->SetQueryPort(usPort);
}
S_API uint16 S_CALLTYPE SteamAPI_servernetadr_t_GetConnectionPort(servernetadr_t* self)
{
	return self->GetConnectionPort();
}
S_API void S_CALLTYPE SteamAPI_servernetadr_t_SetConnectionPort(servernetadr_t* self, uint16 usPort)
{
	return self->SetConnectionPort(usPort);
}
S_API uint32 S_CALLTYPE SteamAPI_servernetadr_t_GetIP(servernetadr_t* self)
{
	return self->GetIP();
}
S_API void S_CALLTYPE SteamAPI_servernetadr_t_SetIP(servernetadr_t* self, uint32 unIP)
{
	return self->SetIP(unIP);
}
S_API const char* S_CALLTYPE SteamAPI_servernetadr_t_GetConnectionAddressString(servernetadr_t* self)
{
	return self->GetConnectionAddressString();
}
S_API const char* S_CALLTYPE SteamAPI_servernetadr_t_GetQueryAddressString(servernetadr_t* self)
{
	return self->GetQueryAddressString();
}
S_API bool S_CALLTYPE SteamAPI_servernetadr_t_IsLessThan(servernetadr_t* self, const servernetadr_t& netadr)
{
	return self->operator<(netadr);
}
S_API void S_CALLTYPE SteamAPI_servernetadr_t_Assign(servernetadr_t* self, const servernetadr_t& that)
{
	return self->operator=(that);
}
S_API void S_CALLTYPE SteamAPI_gameserveritem_t_Construct(gameserveritem_t* self)
{
	*self = gameserveritem_t();
}
S_API const char* S_CALLTYPE SteamAPI_gameserveritem_t_GetName(gameserveritem_t* self)
{
	return self->GetName();
}
S_API void S_CALLTYPE SteamAPI_gameserveritem_t_SetName(gameserveritem_t* self, const char* pName)
{
	return self->SetName(pName);
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingConfigValue_t_SetInt32(SteamNetworkingConfigValue_t* self, ESteamNetworkingConfigValue eVal, int32_t data)
{
	return self->SetInt32(eVal, data);
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingConfigValue_t_SetInt64(SteamNetworkingConfigValue_t* self, ESteamNetworkingConfigValue eVal, int64_t data)
{
	return self->SetInt64(eVal, data);
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingConfigValue_t_SetFloat(SteamNetworkingConfigValue_t* self, ESteamNetworkingConfigValue eVal, float data)
{
	return self->SetFloat(eVal, data);
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingConfigValue_t_SetPtr(SteamNetworkingConfigValue_t* self, ESteamNetworkingConfigValue eVal, void* data)
{
	return self->SetPtr(eVal, data);
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingConfigValue_t_SetString(SteamNetworkingConfigValue_t* self, ESteamNetworkingConfigValue eVal, const char* data)
{
	return self->SetString(eVal, data);
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingIPAddr_Clear(SteamNetworkingIPAddr* self)
{
	return self->Clear();
}
S_API bool S_CALLTYPE SteamAPI_SteamNetworkingIPAddr_IsIPv6AllZeros(SteamNetworkingIPAddr* self)
{
	return self->IsIPv6AllZeros();
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingIPAddr_SetIPv6(SteamNetworkingIPAddr* self, const uint8* ipv6, uint16 nPort)
{
	self->SetIPv6(ipv6, nPort);
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingIPAddr_SetIPv4(SteamNetworkingIPAddr* self, uint32 nIP, uint16 nPort)
{
	return self->SetIPv4(nIP, nPort);
}
S_API bool S_CALLTYPE SteamAPI_SteamNetworkingIPAddr_IsIPv4(SteamNetworkingIPAddr* self)
{
	return self->IsIPv4();
}
S_API uint32 S_CALLTYPE SteamAPI_SteamNetworkingIPAddr_GetIPv4(SteamNetworkingIPAddr* self)
{
	return self->GetIPv4();
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingIPAddr_SetIPv6LocalHost(SteamNetworkingIPAddr* self, uint16 nPort)
{
	return self->SetIPv6LocalHost(nPort);
}
S_API bool S_CALLTYPE SteamAPI_SteamNetworkingIPAddr_IsLocalHost(SteamNetworkingIPAddr* self)
{
	return self->IsLocalHost();
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingIPAddr_ToString(SteamNetworkingIPAddr* self, char* buf, uint32 cbBuf, bool bWithPort)
{
	return self->ToString(buf, cbBuf, bWithPort);
}
S_API bool S_CALLTYPE SteamAPI_SteamNetworkingIPAddr_ParseString(SteamNetworkingIPAddr* self, const char* pszStr)
{
	return self->ParseString(pszStr);
}
S_API ESteamNetworkingFakeIPType S_CALLTYPE SteamAPI_SteamNetworkingIPAddr_GetFakeIPType(SteamNetworkingIPAddr* self)
{
	return self->GetFakeIPType();
}
S_API bool S_CALLTYPE SteamAPI_SteamNetworkingIPAddr_IsFakeIP(SteamNetworkingIPAddr* self)
{
	return self->IsFakeIP();
}
S_API bool S_CALLTYPE SteamAPI_SteamNetworkingIPAddr_IsEqualTo(SteamNetworkingIPAddr* self, const SteamNetworkingIPAddr& x)
{
	return self->operator==(x);
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingIdentity_Clear(SteamNetworkingIdentity* self)
{
	return self->Clear();
}
S_API bool S_CALLTYPE SteamAPI_SteamNetworkingIdentity_IsInvalid(SteamNetworkingIdentity* self)
{
	return self->IsInvalid();
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingIdentity_SetPSNID(SteamNetworkingIdentity* self, uint64 PSNID)
{
	return self->SetPSNID(PSNID);
}
S_API uint64 S_CALLTYPE SteamAPI_SteamNetworkingIdentity_GetPSNID(SteamNetworkingIdentity* self)
{
	return self->GetPSNID();
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingIdentity_SetSteamID(SteamNetworkingIdentity* self, uint64 steamID)
{
	return self->SetSteamID(steamID);
}
S_API uint64 S_CALLTYPE SteamAPI_SteamNetworkingIdentity_GetSteamID(SteamNetworkingIdentity* self)
{
	return self->GetSteamID().ConvertToUint64();
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingIdentity_SetSteamID64(SteamNetworkingIdentity* self, uint64 steamID)
{
	return self->SetSteamID64(steamID);
}
S_API uint64 S_CALLTYPE SteamAPI_SteamNetworkingIdentity_GetSteamID64(SteamNetworkingIdentity* self)
{
	return self->GetSteamID64();
}
S_API bool S_CALLTYPE SteamAPI_SteamNetworkingIdentity_SetXboxPairwiseID(SteamNetworkingIdentity* self, const char* pszString)
{
	return self->SetXboxPairwiseID(pszString);
}
S_API const char* S_CALLTYPE SteamAPI_SteamNetworkingIdentity_GetXboxPairwiseID(SteamNetworkingIdentity* self)
{
	return self->GetXboxPairwiseID();
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingIdentity_SetIPAddr(SteamNetworkingIdentity* self, const SteamNetworkingIPAddr& addr)
{
	return self->SetIPAddr(addr);
}
S_API const SteamNetworkingIPAddr* S_CALLTYPE SteamAPI_SteamNetworkingIdentity_GetIPAddr(SteamNetworkingIdentity* self)
{
	return self->GetIPAddr();
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingIdentity_SetIPv4Addr(SteamNetworkingIdentity* self, uint32 nIPv4, uint16 nPort)
{
	return self->SetIPv4Addr(nIPv4, nPort);
}
S_API uint32 S_CALLTYPE SteamAPI_SteamNetworkingIdentity_GetIPv4(SteamNetworkingIdentity* self)
{
	return self->GetIPv4();
}
S_API ESteamNetworkingFakeIPType S_CALLTYPE SteamAPI_SteamNetworkingIdentity_GetFakeIPType(SteamNetworkingIdentity* self)
{
	return self->GetFakeIPType();
}
S_API bool S_CALLTYPE SteamAPI_SteamNetworkingIdentity_IsFakeIP(SteamNetworkingIdentity* self)
{
	return self->IsFakeIP();
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingIdentity_SetLocalHost(SteamNetworkingIdentity* self)
{
	return self->SetLocalHost();
}
S_API bool S_CALLTYPE SteamAPI_SteamNetworkingIdentity_IsLocalHost(SteamNetworkingIdentity* self)
{
	return self->IsLocalHost();
}
S_API bool S_CALLTYPE SteamAPI_SteamNetworkingIdentity_SetGenericString(SteamNetworkingIdentity* self, const char* pszString)
{
	return self->SetGenericString(pszString);
}
S_API const char* S_CALLTYPE SteamAPI_SteamNetworkingIdentity_GetGenericString(SteamNetworkingIdentity* self)
{
	return self->GetGenericString();
}
S_API bool S_CALLTYPE SteamAPI_SteamNetworkingIdentity_SetGenericBytes(SteamNetworkingIdentity* self, const void* data, uint32 cbLen)
{
	return self->SetGenericBytes(data, cbLen);
}
S_API const uint8* S_CALLTYPE SteamAPI_SteamNetworkingIdentity_GetGenericBytes(SteamNetworkingIdentity* self, int& cbLen)
{
	return self->GetGenericBytes(cbLen);
}
S_API bool S_CALLTYPE SteamAPI_SteamNetworkingIdentity_IsEqualTo(SteamNetworkingIdentity* self, const SteamNetworkingIdentity& x)
{
	return self->operator==(x);
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingIdentity_ToString(SteamNetworkingIdentity* self, char* buf, uint32 cbBuf)
{
	return self->ToString(buf, cbBuf);
}
S_API bool S_CALLTYPE SteamAPI_SteamNetworkingIdentity_ParseString(SteamNetworkingIdentity* self, const char* pszStr)
{
	return self->ParseString(pszStr);
}
S_API void S_CALLTYPE SteamAPI_SteamNetworkingMessage_t_Release(SteamNetworkingMessage_t* self)
{
	return self->Release();
}
S_API const char* S_CALLTYPE SteamAPI_SteamNetworkingPOPIDRender_c_str(SteamNetworkingPOPIDRender* self)
{
	return self->c_str();
}
S_API const char* S_CALLTYPE SteamAPI_SteamNetworkingIdentityRender_c_str(SteamNetworkingIdentityRender* self)
{
	return self->c_str();
}
S_API const char* S_CALLTYPE SteamAPI_SteamNetworkingIPAddrRender_c_str(SteamNetworkingIPAddrRender* self)
{
	return self->c_str();
}
S_API HSteamPipe S_CALLTYPE SteamAPI_ISteamClient_CreateSteamPipe(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->CreateSteamPipe();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->CreateSteamPipe();
}
S_API bool S_CALLTYPE SteamAPI_ISteamClient_BReleaseSteamPipe(intptr_t instancePtr, HSteamPipe hSteamPipe)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->BReleaseSteamPipe(hSteamPipe);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->BReleaseSteamPipe(hSteamPipe);
}
S_API HSteamUser S_CALLTYPE SteamAPI_ISteamClient_ConnectToGlobalUser(intptr_t instancePtr, HSteamPipe hSteamPipe)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->ConnectToGlobalUser(hSteamPipe);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->ConnectToGlobalUser(hSteamPipe);
}
S_API HSteamUser S_CALLTYPE SteamAPI_ISteamClient_CreateLocalUser(intptr_t instancePtr, HSteamPipe * phSteamPipe, EAccountType eAccountType)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->CreateLocalUser(phSteamPipe, eAccountType);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->CreateLocalUser(phSteamPipe, eAccountType);
}
S_API void S_CALLTYPE SteamAPI_ISteamClient_ReleaseUser(intptr_t instancePtr, HSteamPipe hSteamPipe, HSteamUser hSteamUser)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->ReleaseUser(hSteamPipe, hSteamUser);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->ReleaseUser(hSteamPipe, hSteamUser);
}
S_API ISteamUser* S_CALLTYPE SteamAPI_ISteamClient_GetISteamUser(intptr_t instancePtr, HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamUser(hSteamUser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamUser(hSteamUser, hSteamPipe, pchVersion);
}
S_API ISteamGameServer* S_CALLTYPE SteamAPI_ISteamClient_GetISteamGameServer(intptr_t instancePtr, HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamGameServer(hSteamUser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamGameServer(hSteamUser, hSteamPipe, pchVersion);
}
S_API void S_CALLTYPE SteamAPI_ISteamClient_SetLocalIPBinding(intptr_t instancePtr, const struct SteamIPAddress_t& unIP, uint16 usPort)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->SetLocalIPBinding(unIP, usPort);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->SetLocalIPBinding(unIP, usPort);
}
S_API ISteamFriends* S_CALLTYPE SteamAPI_ISteamClient_GetISteamFriends(intptr_t instancePtr, HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamFriends(hSteamUser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamFriends(hSteamUser, hSteamPipe, pchVersion);
}
S_API ISteamUtils* S_CALLTYPE SteamAPI_ISteamClient_GetISteamUtils(intptr_t instancePtr, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamUtils(hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamUtils(hSteamPipe, pchVersion);
}
S_API ISteamMatchmaking* S_CALLTYPE SteamAPI_ISteamClient_GetISteamMatchmaking(intptr_t instancePtr, HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamMatchmaking(hSteamUser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamMatchmaking(hSteamUser, hSteamPipe, pchVersion);
}
S_API ISteamMatchmakingServers* S_CALLTYPE SteamAPI_ISteamClient_GetISteamMatchmakingServers(intptr_t instancePtr, HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamMatchmakingServers(hSteamUser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamMatchmakingServers(hSteamUser, hSteamPipe, pchVersion);
}
S_API void* S_CALLTYPE SteamAPI_ISteamClient_GetISteamGenericInterface(intptr_t instancePtr, HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamGenericInterface(hSteamUser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamGenericInterface(hSteamUser, hSteamPipe, pchVersion);
}
S_API ISteamUserStats* S_CALLTYPE SteamAPI_ISteamClient_GetISteamUserStats(intptr_t instancePtr, HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamUserStats(hSteamUser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamUserStats(hSteamUser, hSteamPipe, pchVersion);
}
S_API ISteamGameServerStats* S_CALLTYPE SteamAPI_ISteamClient_GetISteamGameServerStats(intptr_t instancePtr, HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamGameServerStats(hSteamuser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamGameServerStats(hSteamuser, hSteamPipe, pchVersion);
}
S_API ISteamApps* S_CALLTYPE SteamAPI_ISteamClient_GetISteamApps(intptr_t instancePtr, HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamApps(hSteamUser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamApps(hSteamUser, hSteamPipe, pchVersion);
}
S_API ISteamNetworking* S_CALLTYPE SteamAPI_ISteamClient_GetISteamNetworking(intptr_t instancePtr, HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamNetworking(hSteamUser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamNetworking(hSteamUser, hSteamPipe, pchVersion);
}
S_API ISteamRemoteStorage* S_CALLTYPE SteamAPI_ISteamClient_GetISteamRemoteStorage(intptr_t instancePtr, HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamRemoteStorage(hSteamuser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamRemoteStorage(hSteamuser, hSteamPipe, pchVersion);
}
S_API ISteamScreenshots* S_CALLTYPE SteamAPI_ISteamClient_GetISteamScreenshots(intptr_t instancePtr, HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamScreenshots(hSteamuser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamScreenshots(hSteamuser, hSteamPipe, pchVersion);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamClient_GetIPCCallCount(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetIPCCallCount();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetIPCCallCount();
}
S_API void S_CALLTYPE SteamAPI_ISteamClient_SetWarningMessageHook(intptr_t instancePtr, SteamAPIWarningMessageHook_t pFunction)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->SetWarningMessageHook(pFunction);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->SetWarningMessageHook(pFunction);
}
S_API bool S_CALLTYPE SteamAPI_ISteamClient_BShutdownIfAllPipesClosed(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->BShutdownIfAllPipesClosed();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->BShutdownIfAllPipesClosed();
}
S_API ISteamHTTP* S_CALLTYPE SteamAPI_ISteamClient_GetISteamHTTP(intptr_t instancePtr, HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamHTTP(hSteamuser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamHTTP(hSteamuser, hSteamPipe, pchVersion);
}
S_API ISteamController* S_CALLTYPE SteamAPI_ISteamClient_GetISteamController(intptr_t instancePtr, HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamController(hSteamUser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamController(hSteamUser, hSteamPipe, pchVersion);
}
S_API ISteamUGC* S_CALLTYPE SteamAPI_ISteamClient_GetISteamUGC(intptr_t instancePtr, HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamUGC(hSteamUser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamUGC(hSteamUser, hSteamPipe, pchVersion);
}
S_API ISteamMusic* S_CALLTYPE SteamAPI_ISteamClient_GetISteamMusic(intptr_t instancePtr, HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamMusic(hSteamuser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamMusic(hSteamuser, hSteamPipe, pchVersion);
}
S_API ISteamMusicRemote* S_CALLTYPE SteamAPI_ISteamClient_GetISteamMusicRemote(intptr_t instancePtr, HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion)
{
	return (ISteamMusicRemote*)SteamMusicRemote();
}
S_API ISteamHTMLSurface* S_CALLTYPE SteamAPI_ISteamClient_GetISteamHTMLSurface(intptr_t instancePtr, HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamHTMLSurface(hSteamuser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamHTMLSurface(hSteamuser, hSteamPipe, pchVersion);
}
S_API ISteamInventory* S_CALLTYPE SteamAPI_ISteamClient_GetISteamInventory(intptr_t instancePtr, HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamInventory(hSteamuser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamInventory(hSteamuser, hSteamPipe, pchVersion);
}
S_API ISteamVideo* S_CALLTYPE SteamAPI_ISteamClient_GetISteamVideo(intptr_t instancePtr, HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamVideo(hSteamuser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamVideo(hSteamuser, hSteamPipe, pchVersion);
}
S_API ISteamParentalSettings* S_CALLTYPE SteamAPI_ISteamClient_GetISteamParentalSettings(intptr_t instancePtr, HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamParentalSettings(hSteamuser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamParentalSettings(hSteamuser, hSteamPipe, pchVersion);
}
S_API ISteamInput* S_CALLTYPE SteamAPI_ISteamClient_GetISteamInput(intptr_t instancePtr, HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamInput(hSteamUser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamInput(hSteamUser, hSteamPipe, pchVersion);
}
S_API ISteamParties* S_CALLTYPE SteamAPI_ISteamClient_GetISteamParties(intptr_t instancePtr, HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char * pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamParties(hSteamUser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamParties(hSteamUser, hSteamPipe, pchVersion);
}
S_API ISteamRemotePlay* S_CALLTYPE SteamAPI_ISteamClient_GetISteamRemotePlay(intptr_t instancePtr, HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamClient() || g_bClientReady == false)
			return g_ServerCtx.SteamClient()->GetISteamRemotePlay(hSteamUser, hSteamPipe, pchVersion);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamClient()->GetISteamRemotePlay(hSteamUser, hSteamPipe, pchVersion);
}
S_API HSteamUser S_CALLTYPE SteamAPI_ISteamUser_GetHSteamUser(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->GetHSteamUser();
}
S_API bool S_CALLTYPE SteamAPI_ISteamUser_BLoggedOn(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->BLoggedOn();
}
S_API uint64 S_CALLTYPE SteamAPI_ISteamUser_GetSteamID(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->GetSteamID().ConvertToUint64();
}
S_API int S_CALLTYPE SteamAPI_ISteamUser_InitiateGameConnection(intptr_t instancePtr, void* pAuthBlob, int cbMaxAuthBlob, class CSteamID steamIDGameServer, uint32 unIPServer, uint16 usPortServer, bool bSecure)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->InitiateGameConnection_DEPRECATED(pAuthBlob, cbMaxAuthBlob, steamIDGameServer, unIPServer, usPortServer, bSecure);
}
S_API int S_CALLTYPE SteamAPI_ISteamUser_InitiateGameConnection_DEPRECATED(intptr_t instancePtr, void * pAuthBlob, int cbMaxAuthBlob, class CSteamID steamIDGameServer, uint32 unIPServer, uint16 usPortServer, bool bSecure)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->InitiateGameConnection_DEPRECATED(pAuthBlob, cbMaxAuthBlob, steamIDGameServer, unIPServer, usPortServer, bSecure);
}
S_API void S_CALLTYPE SteamAPI_ISteamUser_TerminateGameConnection(intptr_t instancePtr, uint32 unIPServer, uint16 usPortServer)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->TerminateGameConnection_DEPRECATED(unIPServer, usPortServer);
}
S_API void S_CALLTYPE SteamAPI_ISteamUser_TerminateGameConnection_DEPRECATED(intptr_t instancePtr, uint32 unIPServer, uint16 usPortServer)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->TerminateGameConnection_DEPRECATED(unIPServer, usPortServer);
}
S_API void S_CALLTYPE SteamAPI_ISteamUser_TrackAppUsageEvent(intptr_t instancePtr, class CGameID gameID, int eAppUsageEvent, const char * pchExtraInfo)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->TrackAppUsageEvent(gameID, eAppUsageEvent, pchExtraInfo);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUser_GetUserDataFolder(intptr_t instancePtr, char * pchBuffer, int cubBuffer)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->GetUserDataFolder(pchBuffer, cubBuffer);
}
S_API void S_CALLTYPE SteamAPI_ISteamUser_StartVoiceRecording(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->StartVoiceRecording();
}
S_API void S_CALLTYPE SteamAPI_ISteamUser_StopVoiceRecording(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->StopVoiceRecording();
}
S_API EVoiceResult S_CALLTYPE SteamAPI_ISteamUser_GetAvailableVoice(intptr_t instancePtr, uint32 * pcbCompressed, uint32 * pcbUncompressed_Deprecated, uint32 nUncompressedVoiceDesiredSampleRate_Deprecated)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->GetAvailableVoice(pcbCompressed, pcbUncompressed_Deprecated, nUncompressedVoiceDesiredSampleRate_Deprecated);
}
S_API EVoiceResult S_CALLTYPE SteamAPI_ISteamUser_GetVoice(intptr_t instancePtr, bool bWantCompressed, void * pDestBuffer, uint32 cbDestBufferSize, uint32 * nBytesWritten, bool bWantUncompressed_Deprecated, void * pUncompressedDestBuffer_Deprecated, uint32 cbUncompressedDestBufferSize_Deprecated, uint32 * nUncompressBytesWritten_Deprecated, uint32 nUncompressedVoiceDesiredSampleRate_Deprecated)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->GetVoice(bWantCompressed, pDestBuffer, cbDestBufferSize, nBytesWritten, bWantUncompressed_Deprecated, pUncompressedDestBuffer_Deprecated, cbUncompressedDestBufferSize_Deprecated, nUncompressBytesWritten_Deprecated, nUncompressedVoiceDesiredSampleRate_Deprecated);
}
S_API EVoiceResult S_CALLTYPE SteamAPI_ISteamUser_DecompressVoice(intptr_t instancePtr, const void * pCompressed, uint32 cbCompressed, void * pDestBuffer, uint32 cbDestBufferSize, uint32 * nBytesWritten, uint32 nDesiredSampleRate)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->DecompressVoice(pCompressed, cbCompressed, pDestBuffer, cbDestBufferSize, nBytesWritten, nDesiredSampleRate);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUser_GetVoiceOptimalSampleRate(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->GetVoiceOptimalSampleRate();
}
S_API HAuthTicket S_CALLTYPE SteamAPI_ISteamUser_GetAuthSessionTicket(intptr_t instancePtr, void* pTicket, int cbMaxTicket, uint32* pcbTicket, const SteamNetworkingIdentity* pSteamNetworkingIdentity)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->GetAuthSessionTicket(pTicket, cbMaxTicket, pcbTicket, pSteamNetworkingIdentity);
}
S_API HAuthTicket S_CALLTYPE SteamAPI_ISteamUser_GetAuthTicketForWebApi(intptr_t instancePtr, const char* pchIdentity)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->GetAuthTicketForWebApi(pchIdentity);
}
S_API EBeginAuthSessionResult S_CALLTYPE SteamAPI_ISteamUser_BeginAuthSession(intptr_t instancePtr, const void * pAuthTicket, int cbAuthTicket, class CSteamID steamID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->BeginAuthSession(pAuthTicket, cbAuthTicket, steamID);
}
S_API void S_CALLTYPE SteamAPI_ISteamUser_EndAuthSession(intptr_t instancePtr, class CSteamID steamID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->EndAuthSession(steamID);
}
S_API void S_CALLTYPE SteamAPI_ISteamUser_CancelAuthTicket(intptr_t instancePtr, HAuthTicket hAuthTicket)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->CancelAuthTicket(hAuthTicket);
}
S_API EUserHasLicenseForAppResult S_CALLTYPE SteamAPI_ISteamUser_UserHasLicenseForApp(intptr_t instancePtr, class CSteamID steamID, AppId_t appID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->UserHasLicenseForApp(steamID, appID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUser_BIsBehindNAT(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->BIsBehindNAT();
}
S_API void S_CALLTYPE SteamAPI_ISteamUser_AdvertiseGame(intptr_t instancePtr, class CSteamID steamIDGameServer, uint32 unIPServer, uint16 usPortServer)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->AdvertiseGame(steamIDGameServer, unIPServer, usPortServer);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUser_RequestEncryptedAppTicket(intptr_t instancePtr, void * pDataToInclude, int cbDataToInclude)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->RequestEncryptedAppTicket(pDataToInclude, cbDataToInclude);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUser_GetEncryptedAppTicket(intptr_t instancePtr, void * pTicket, int cbMaxTicket, uint32 * pcbTicket)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->GetEncryptedAppTicket(pTicket, cbMaxTicket, pcbTicket);
}
S_API int S_CALLTYPE SteamAPI_ISteamUser_GetGameBadgeLevel(intptr_t instancePtr, int nSeries, bool bFoil)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->GetGameBadgeLevel(nSeries, bFoil);
}
S_API int S_CALLTYPE SteamAPI_ISteamUser_GetPlayerSteamLevel(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->GetPlayerSteamLevel();
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUser_RequestStoreAuthURL(intptr_t instancePtr, const char * pchRedirectURL)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->RequestStoreAuthURL(pchRedirectURL);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUser_BIsPhoneVerified(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->BIsPhoneVerified();
}
S_API bool S_CALLTYPE SteamAPI_ISteamUser_BIsTwoFactorEnabled(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->BIsTwoFactorEnabled();
}
S_API bool S_CALLTYPE SteamAPI_ISteamUser_BIsPhoneIdentifying(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->BIsPhoneIdentifying();
}
S_API bool S_CALLTYPE SteamAPI_ISteamUser_BIsPhoneRequiringVerification(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->BIsPhoneRequiringVerification();
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUser_GetMarketEligibility(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->GetMarketEligibility();
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUser_GetDurationControl(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->GetDurationControl();
}
S_API bool S_CALLTYPE SteamAPI_ISteamUser_BSetDurationControlOnlineState(intptr_t instancePtr, EDurationControlOnlineState eNewState)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUser()->BSetDurationControlOnlineState(eNewState);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamFriends_GetPersonaName(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetPersonaName();
}
S_API EPersonaState S_CALLTYPE SteamAPI_ISteamFriends_GetPersonaState(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetPersonaState();
}
S_API int S_CALLTYPE SteamAPI_ISteamFriends_GetFriendCount(intptr_t instancePtr, int iFriendFlags)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendCount(iFriendFlags);
}
S_API uint64 S_CALLTYPE SteamAPI_ISteamFriends_GetFriendByIndex(intptr_t instancePtr, int iFriend, int iFriendFlags)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendByIndex(iFriend, iFriendFlags).ConvertToUint64();
}
S_API EFriendRelationship S_CALLTYPE SteamAPI_ISteamFriends_GetFriendRelationship(intptr_t instancePtr, class CSteamID steamIDFriend)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendRelationship(steamIDFriend);
}
S_API EPersonaState S_CALLTYPE SteamAPI_ISteamFriends_GetFriendPersonaState(intptr_t instancePtr, class CSteamID steamIDFriend)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendPersonaState(steamIDFriend);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamFriends_GetFriendPersonaName(intptr_t instancePtr, class CSteamID steamIDFriend)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendPersonaName(steamIDFriend);
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_GetFriendGamePlayed(intptr_t instancePtr, class CSteamID steamIDFriend, struct FriendGameInfo_t * pFriendGameInfo)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendGamePlayed(steamIDFriend, pFriendGameInfo);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamFriends_GetFriendPersonaNameHistory(intptr_t instancePtr, class CSteamID steamIDFriend, int iPersonaName)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendPersonaNameHistory(steamIDFriend, iPersonaName);
}
S_API int S_CALLTYPE SteamAPI_ISteamFriends_GetFriendSteamLevel(intptr_t instancePtr, class CSteamID steamIDFriend)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendSteamLevel(steamIDFriend);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamFriends_GetPlayerNickname(intptr_t instancePtr, class CSteamID steamIDPlayer)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetPlayerNickname(steamIDPlayer);
}
S_API int S_CALLTYPE SteamAPI_ISteamFriends_GetFriendsGroupCount(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendsGroupCount();
}
S_API FriendsGroupID_t S_CALLTYPE SteamAPI_ISteamFriends_GetFriendsGroupIDByIndex(intptr_t instancePtr, int iFG)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendsGroupIDByIndex(iFG);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamFriends_GetFriendsGroupName(intptr_t instancePtr, FriendsGroupID_t friendsGroupID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendsGroupName(friendsGroupID);
}
S_API int S_CALLTYPE SteamAPI_ISteamFriends_GetFriendsGroupMembersCount(intptr_t instancePtr, FriendsGroupID_t friendsGroupID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendsGroupMembersCount(friendsGroupID);
}
S_API void S_CALLTYPE SteamAPI_ISteamFriends_GetFriendsGroupMembersList(intptr_t instancePtr, FriendsGroupID_t friendsGroupID, class CSteamID * pOutSteamIDMembers, int nMembersCount)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendsGroupMembersList(friendsGroupID, pOutSteamIDMembers, nMembersCount);
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_HasFriend(intptr_t instancePtr, class CSteamID steamIDFriend, int iFriendFlags)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->HasFriend(steamIDFriend, iFriendFlags);
}
S_API int S_CALLTYPE SteamAPI_ISteamFriends_GetClanCount(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetClanCount();
}
S_API uint64 S_CALLTYPE SteamAPI_ISteamFriends_GetClanByIndex(intptr_t instancePtr, int iClan)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetClanByIndex(iClan).ConvertToUint64();
}
S_API const char* S_CALLTYPE SteamAPI_ISteamFriends_GetClanName(intptr_t instancePtr, class CSteamID steamIDClan)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetClanName(steamIDClan);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamFriends_GetClanTag(intptr_t instancePtr, class CSteamID steamIDClan)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetClanTag(steamIDClan);
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_GetClanActivityCounts(intptr_t instancePtr, class CSteamID steamIDClan, int * pnOnline, int * pnInGame, int * pnChatting)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetClanActivityCounts(steamIDClan, pnOnline, pnInGame, pnChatting);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamFriends_DownloadClanActivityCounts(intptr_t instancePtr, class CSteamID * psteamIDClans, int cClansToRequest)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->DownloadClanActivityCounts(psteamIDClans, cClansToRequest);
}
S_API int S_CALLTYPE SteamAPI_ISteamFriends_GetFriendCountFromSource(intptr_t instancePtr, class CSteamID steamIDSource)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendCountFromSource(steamIDSource);
}
S_API uint64 S_CALLTYPE SteamAPI_ISteamFriends_GetFriendFromSourceByIndex(intptr_t instancePtr, class CSteamID steamIDSource, int iFriend)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendFromSourceByIndex(steamIDSource, iFriend).ConvertToUint64();
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_IsUserInSource(intptr_t instancePtr, class CSteamID steamIDUser, class CSteamID steamIDSource)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->IsUserInSource(steamIDUser, steamIDSource);
}
S_API void S_CALLTYPE SteamAPI_ISteamFriends_SetInGameVoiceSpeaking(intptr_t instancePtr, class CSteamID steamIDUser, bool bSpeaking)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->SetInGameVoiceSpeaking(steamIDUser, bSpeaking);
}
S_API void S_CALLTYPE SteamAPI_ISteamFriends_ActivateGameOverlay(intptr_t instancePtr, const char * pchDialog)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->ActivateGameOverlay(pchDialog);
}
S_API void S_CALLTYPE SteamAPI_ISteamFriends_ActivateGameOverlayToUser(intptr_t instancePtr, const char * pchDialog, class CSteamID steamID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->ActivateGameOverlayToUser(pchDialog, steamID);
}
S_API void S_CALLTYPE SteamAPI_ISteamFriends_ActivateGameOverlayToWebPage(intptr_t instancePtr, const char * pchURL, EActivateGameOverlayToWebPageMode eMode)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->ActivateGameOverlayToWebPage(pchURL, eMode);
}
S_API void S_CALLTYPE SteamAPI_ISteamFriends_ActivateGameOverlayToStore(intptr_t instancePtr, AppId_t nAppID, EOverlayToStoreFlag eFlag)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->ActivateGameOverlayToStore(nAppID, eFlag);
}
S_API void S_CALLTYPE SteamAPI_ISteamFriends_SetPlayedWith(intptr_t instancePtr, class CSteamID steamIDUserPlayedWith)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->SetPlayedWith(steamIDUserPlayedWith);
}
S_API void S_CALLTYPE SteamAPI_ISteamFriends_ActivateGameOverlayInviteDialog(intptr_t instancePtr, class CSteamID steamIDLobby)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->ActivateGameOverlayInviteDialog(steamIDLobby);
}
S_API int S_CALLTYPE SteamAPI_ISteamFriends_GetSmallFriendAvatar(intptr_t instancePtr, class CSteamID steamIDFriend)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetSmallFriendAvatar(steamIDFriend);
}
S_API int S_CALLTYPE SteamAPI_ISteamFriends_GetMediumFriendAvatar(intptr_t instancePtr, class CSteamID steamIDFriend)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetMediumFriendAvatar(steamIDFriend);
}
S_API int S_CALLTYPE SteamAPI_ISteamFriends_GetLargeFriendAvatar(intptr_t instancePtr, class CSteamID steamIDFriend)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetLargeFriendAvatar(steamIDFriend);
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_RequestUserInformation(intptr_t instancePtr, class CSteamID steamIDUser, bool bRequireNameOnly)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->RequestUserInformation(steamIDUser, bRequireNameOnly);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamFriends_RequestClanOfficerList(intptr_t instancePtr, class CSteamID steamIDClan)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->RequestClanOfficerList(steamIDClan);
}
S_API uint64 S_CALLTYPE SteamAPI_ISteamFriends_GetClanOwner(intptr_t instancePtr, class CSteamID steamIDClan)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetClanOwner(steamIDClan).ConvertToUint64();
}
S_API int S_CALLTYPE SteamAPI_ISteamFriends_GetClanOfficerCount(intptr_t instancePtr, class CSteamID steamIDClan)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetClanOfficerCount(steamIDClan);
}
S_API uint64 S_CALLTYPE SteamAPI_ISteamFriends_GetClanOfficerByIndex(intptr_t instancePtr, class CSteamID steamIDClan, int iOfficer)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetClanOfficerByIndex(steamIDClan, iOfficer).ConvertToUint64();
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_SetRichPresence(intptr_t instancePtr, const char * pchKey, const char * pchValue)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->SetRichPresence(pchKey, pchValue);
}
S_API void S_CALLTYPE SteamAPI_ISteamFriends_ClearRichPresence(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->ClearRichPresence();
}
S_API const char* S_CALLTYPE SteamAPI_ISteamFriends_GetFriendRichPresence(intptr_t instancePtr, class CSteamID steamIDFriend, const char * pchKey)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendRichPresence(steamIDFriend, pchKey);
}
S_API int S_CALLTYPE SteamAPI_ISteamFriends_GetFriendRichPresenceKeyCount(intptr_t instancePtr, class CSteamID steamIDFriend)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendRichPresenceKeyCount(steamIDFriend);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamFriends_GetFriendRichPresenceKeyByIndex(intptr_t instancePtr, class CSteamID steamIDFriend, int iKey)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendRichPresenceKeyByIndex(steamIDFriend, iKey);
}
S_API void S_CALLTYPE SteamAPI_ISteamFriends_RequestFriendRichPresence(intptr_t instancePtr, class CSteamID steamIDFriend)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->RequestFriendRichPresence(steamIDFriend);
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_InviteUserToGame(intptr_t instancePtr, class CSteamID steamIDFriend, const char * pchConnectString)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->InviteUserToGame(steamIDFriend, pchConnectString);
}
S_API int S_CALLTYPE SteamAPI_ISteamFriends_GetCoplayFriendCount(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetCoplayFriendCount();
}
S_API uint64 S_CALLTYPE SteamAPI_ISteamFriends_GetCoplayFriend(intptr_t instancePtr, int iCoplayFriend)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetCoplayFriend(iCoplayFriend).ConvertToUint64();
}
S_API int S_CALLTYPE SteamAPI_ISteamFriends_GetFriendCoplayTime(intptr_t instancePtr, class CSteamID steamIDFriend)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendCoplayTime(steamIDFriend);
}
S_API AppId_t S_CALLTYPE SteamAPI_ISteamFriends_GetFriendCoplayGame(intptr_t instancePtr, class CSteamID steamIDFriend)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendCoplayGame(steamIDFriend);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamFriends_JoinClanChatRoom(intptr_t instancePtr, class CSteamID steamIDClan)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->JoinClanChatRoom(steamIDClan);
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_LeaveClanChatRoom(intptr_t instancePtr, class CSteamID steamIDClan)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->LeaveClanChatRoom(steamIDClan);
}
S_API int S_CALLTYPE SteamAPI_ISteamFriends_GetClanChatMemberCount(intptr_t instancePtr, class CSteamID steamIDClan)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetClanChatMemberCount(steamIDClan);
}
S_API uint64 S_CALLTYPE SteamAPI_ISteamFriends_GetChatMemberByIndex(intptr_t instancePtr, class CSteamID steamIDClan, int iUser)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetChatMemberByIndex(steamIDClan, iUser).ConvertToUint64();
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_SendClanChatMessage(intptr_t instancePtr, class CSteamID steamIDClanChat, const char * pchText)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->SendClanChatMessage(steamIDClanChat, pchText);
}
S_API int S_CALLTYPE SteamAPI_ISteamFriends_GetClanChatMessage(intptr_t instancePtr, class CSteamID steamIDClanChat, int iMessage, void * prgchText, int cchTextMax, EChatEntryType * peChatEntryType, class CSteamID * psteamidChatter)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetClanChatMessage(steamIDClanChat, iMessage, prgchText, cchTextMax, peChatEntryType, psteamidChatter);
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_IsClanChatAdmin(intptr_t instancePtr, class CSteamID steamIDClanChat, class CSteamID steamIDUser)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->IsClanChatAdmin(steamIDClanChat, steamIDUser);
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_IsClanChatWindowOpenInSteam(intptr_t instancePtr, class CSteamID steamIDClanChat)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->IsClanChatWindowOpenInSteam(steamIDClanChat);
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_OpenClanChatWindowInSteam(intptr_t instancePtr, class CSteamID steamIDClanChat)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->OpenClanChatWindowInSteam(steamIDClanChat);
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_CloseClanChatWindowInSteam(intptr_t instancePtr, class CSteamID steamIDClanChat)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->CloseClanChatWindowInSteam(steamIDClanChat);
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_SetListenForFriendsMessages(intptr_t instancePtr, bool bInterceptEnabled)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->SetListenForFriendsMessages(bInterceptEnabled);
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_ReplyToFriendMessage(intptr_t instancePtr, class CSteamID steamIDFriend, const char * pchMsgToSend)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->ReplyToFriendMessage(steamIDFriend, pchMsgToSend);
}
S_API int S_CALLTYPE SteamAPI_ISteamFriends_GetFriendMessage(intptr_t instancePtr, class CSteamID steamIDFriend, int iMessageID, void * pvData, int cubData, EChatEntryType * peChatEntryType)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFriendMessage(steamIDFriend, iMessageID, pvData, cubData, peChatEntryType);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamFriends_GetFollowerCount(intptr_t instancePtr, class CSteamID steamID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetFollowerCount(steamID);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamFriends_IsFollowing(intptr_t instancePtr, class CSteamID steamID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->IsFollowing(steamID);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamFriends_EnumerateFollowingList(intptr_t instancePtr, uint32 unStartIndex)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->EnumerateFollowingList(unStartIndex);
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_IsClanPublic(intptr_t instancePtr, class CSteamID steamIDClan)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->IsClanPublic(steamIDClan);
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_IsClanOfficialGameGroup(intptr_t instancePtr, class CSteamID steamIDClan)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->IsClanOfficialGameGroup(steamIDClan);
}
S_API int SteamAPI_ISteamFriends_GetNumChatsWithUnreadPriorityMessages(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetNumChatsWithUnreadPriorityMessages();
}
S_API void S_CALLTYPE SteamAPI_ISteamFriends_ActivateGameOverlayRemotePlayTogetherInviteDialog(intptr_t instancePtr, CSteamID steamIDLobby)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->ActivateGameOverlayRemotePlayTogetherInviteDialog(steamIDLobby);
}
S_API bool S_CALLTYPE SteamAPI_ISteamFriends_RegisterProtocolInOverlayBrowser(intptr_t instancePtr, const char* pchProtocol)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->RegisterProtocolInOverlayBrowser(pchProtocol);
}
S_API void SteamAPI_ISteamFriends_ActivateGameOverlayInviteDialogConnectString(intptr_t instancePtr, const char* pchConnectString)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->ActivateGameOverlayInviteDialogConnectString(pchConnectString);
}
S_API SteamAPICall_t SteamAPI_ISteamFriends_RequestEquippedProfileItems(intptr_t instancePtr, CSteamID steamID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->RequestEquippedProfileItems(steamID);
}
S_API bool SteamAPI_ISteamFriends_BHasEquippedProfileItem(intptr_t instancePtr, CSteamID steamID, ECommunityProfileItemType itemType)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->BHasEquippedProfileItem(steamID, itemType);
}
S_API const char* SteamAPI_ISteamFriends_GetProfileItemPropertyString(intptr_t instancePtr, CSteamID steamID, ECommunityProfileItemType itemType, ECommunityProfileItemProperty prop)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetProfileItemPropertyString(steamID, itemType, prop);
}
S_API uint32 SteamAPI_ISteamFriends_GetProfileItemPropertyUint(intptr_t instancePtr, CSteamID steamID, ECommunityProfileItemType itemType, ECommunityProfileItemProperty prop)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamFriends()->GetProfileItemPropertyUint(steamID, itemType, prop);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUtils_GetSecondsSinceAppActive(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetSecondsSinceAppActive();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetSecondsSinceAppActive();
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUtils_GetSecondsSinceComputerActive(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetSecondsSinceComputerActive();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetSecondsSinceComputerActive();
}
S_API EUniverse S_CALLTYPE SteamAPI_ISteamUtils_GetConnectedUniverse(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetConnectedUniverse();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetConnectedUniverse();
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUtils_GetServerRealTime(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetServerRealTime();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetServerRealTime();
}
S_API const char* S_CALLTYPE SteamAPI_ISteamUtils_GetIPCountry(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetIPCountry();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetIPCountry();
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_GetImageSize(intptr_t instancePtr, int iImage, uint32 * pnWidth, uint32 * pnHeight)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetImageSize(iImage, pnWidth, pnHeight);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetImageSize(iImage, pnWidth, pnHeight);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_GetImageRGBA(intptr_t instancePtr, int iImage, uint8 * pubDest, int nDestBufferSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetImageRGBA(iImage, pubDest, nDestBufferSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetImageRGBA(iImage, pubDest, nDestBufferSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_GetCSERIPPort(intptr_t instancePtr, uint32 * unIP, uint16 * usPort)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetCSERIPPort(unIP, usPort);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetCSERIPPort(unIP, usPort);
}
S_API uint8 S_CALLTYPE SteamAPI_ISteamUtils_GetCurrentBatteryPower(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetCurrentBatteryPower();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetCurrentBatteryPower();
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUtils_GetAppID(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetAppID();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetAppID();
}
S_API void S_CALLTYPE SteamAPI_ISteamUtils_SetOverlayNotificationPosition(intptr_t instancePtr, ENotificationPosition eNotificationPosition)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->SetOverlayNotificationPosition(eNotificationPosition);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->SetOverlayNotificationPosition(eNotificationPosition);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_IsAPICallCompleted(intptr_t instancePtr, SteamAPICall_t hSteamAPICall, bool * pbFailed)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->IsAPICallCompleted(hSteamAPICall, pbFailed);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->IsAPICallCompleted(hSteamAPICall, pbFailed);
}
S_API ESteamAPICallFailure S_CALLTYPE SteamAPI_ISteamUtils_GetAPICallFailureReason(intptr_t instancePtr, SteamAPICall_t hSteamAPICall)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetAPICallFailureReason(hSteamAPICall);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetAPICallFailureReason(hSteamAPICall);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_GetAPICallResult(intptr_t instancePtr, SteamAPICall_t hSteamAPICall, void * pCallback, int cubCallback, int iCallbackExpected, bool * pbFailed)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetAPICallResult(hSteamAPICall, pCallback, cubCallback, iCallbackExpected, pbFailed);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetAPICallResult(hSteamAPICall, pCallback, cubCallback, iCallbackExpected, pbFailed);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUtils_GetIPCCallCount(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetIPCCallCount();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetIPCCallCount();
}
S_API void S_CALLTYPE SteamAPI_ISteamUtils_SetWarningMessageHook(intptr_t instancePtr, SteamAPIWarningMessageHook_t pFunction)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->SetWarningMessageHook(pFunction);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->SetWarningMessageHook(pFunction);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_IsOverlayEnabled(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->IsOverlayEnabled();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->IsOverlayEnabled();
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_BOverlayNeedsPresent(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->BOverlayNeedsPresent();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->BOverlayNeedsPresent();
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUtils_CheckFileSignature(intptr_t instancePtr, const char * szFileName)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->CheckFileSignature(szFileName);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->CheckFileSignature(szFileName);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_ShowGamepadTextInput(intptr_t instancePtr, EGamepadTextInputMode eInputMode, EGamepadTextInputLineMode eLineInputMode, const char * pchDescription, uint32 unCharMax, const char * pchExistingText)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->ShowGamepadTextInput(eInputMode, eLineInputMode, pchDescription, unCharMax, pchExistingText);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->ShowGamepadTextInput(eInputMode, eLineInputMode, pchDescription, unCharMax, pchExistingText);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUtils_GetEnteredGamepadTextLength(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetEnteredGamepadTextLength();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetEnteredGamepadTextLength();
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_GetEnteredGamepadTextInput(intptr_t instancePtr, char * pchText, uint32 cchText)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetEnteredGamepadTextInput(pchText, cchText);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetEnteredGamepadTextInput(pchText, cchText);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamUtils_GetSteamUILanguage(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetSteamUILanguage();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetSteamUILanguage();
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_IsSteamRunningInVR(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->IsSteamRunningInVR();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->IsSteamRunningInVR();
}
S_API void S_CALLTYPE SteamAPI_ISteamUtils_SetOverlayNotificationInset(intptr_t instancePtr, int nHorizontalInset, int nVerticalInset)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->SetOverlayNotificationInset(nHorizontalInset, nVerticalInset);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->SetOverlayNotificationInset(nHorizontalInset, nVerticalInset);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_IsSteamInBigPictureMode(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->IsSteamInBigPictureMode();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->IsSteamInBigPictureMode();
}
S_API void S_CALLTYPE SteamAPI_ISteamUtils_StartVRDashboard(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->StartVRDashboard();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->StartVRDashboard();
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_IsVRHeadsetStreamingEnabled(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->IsVRHeadsetStreamingEnabled();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->IsVRHeadsetStreamingEnabled();
}
S_API void S_CALLTYPE SteamAPI_ISteamUtils_SetVRHeadsetStreamingEnabled(intptr_t instancePtr, bool bEnabled)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->SetVRHeadsetStreamingEnabled(bEnabled);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->SetVRHeadsetStreamingEnabled(bEnabled);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_IsSteamChinaLauncher(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->IsSteamChinaLauncher();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->IsSteamChinaLauncher();
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_InitFilterText(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->InitFilterText();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->InitFilterText();
}
S_API int S_CALLTYPE SteamAPI_ISteamUtils_FilterText(intptr_t instancePtr, ETextFilteringContext eContext, CSteamID sourceSteamID, const char* pchInputMessage, char* pchOutFilteredText, uint32 nByteSizeOutFilteredText)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->FilterText(eContext, sourceSteamID, pchInputMessage, pchOutFilteredText, nByteSizeOutFilteredText);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->FilterText(eContext, sourceSteamID, pchInputMessage, pchOutFilteredText, nByteSizeOutFilteredText);
}
S_API ESteamIPv6ConnectivityState S_CALLTYPE SteamAPI_ISteamUtils_GetIPv6ConnectivityState(intptr_t instancePtr, ESteamIPv6ConnectivityProtocol eProtocol)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->GetIPv6ConnectivityState(eProtocol);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->GetIPv6ConnectivityState(eProtocol);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_IsSteamRunningOnSteamDeck(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->IsSteamRunningOnSteamDeck();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->IsSteamRunningOnSteamDeck();
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_ShowFloatingGamepadTextInput(intptr_t instancePtr, EFloatingGamepadTextInputMode eKeyboardMode, int nTextFieldXPosition, int nTextFieldYPosition, int nTextFieldWidth, int nTextFieldHeight)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->ShowFloatingGamepadTextInput(eKeyboardMode, nTextFieldXPosition, nTextFieldYPosition, nTextFieldWidth, nTextFieldHeight);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->ShowFloatingGamepadTextInput(eKeyboardMode, nTextFieldXPosition, nTextFieldYPosition, nTextFieldWidth, nTextFieldHeight);
}
S_API void S_CALLTYPE SteamAPI_ISteamUtils_SetGameLauncherMode(intptr_t instancePtr, bool bLauncherMode)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->SetGameLauncherMode(bLauncherMode);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->SetGameLauncherMode(bLauncherMode);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_DismissFloatingGamepadTextInput(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->DismissFloatingGamepadTextInput();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->DismissFloatingGamepadTextInput();
}
S_API bool S_CALLTYPE SteamAPI_ISteamUtils_DismissGamepadTextInput(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerUtils() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerUtils()->DismissGamepadTextInput();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUtils()->DismissGamepadTextInput();
}
S_API int S_CALLTYPE SteamAPI_ISteamMatchmaking_GetFavoriteGameCount(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->GetFavoriteGameCount();
}
S_API bool S_CALLTYPE SteamAPI_ISteamMatchmaking_GetFavoriteGame(intptr_t instancePtr, int iGame, AppId_t * pnAppID, uint32 * pnIP, uint16 * pnConnPort, uint16 * pnQueryPort, uint32 * punFlags, uint32 * pRTime32LastPlayedOnServer)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->GetFavoriteGame(iGame, pnAppID, pnIP, pnConnPort, pnQueryPort, punFlags, pRTime32LastPlayedOnServer);
}
S_API int S_CALLTYPE SteamAPI_ISteamMatchmaking_AddFavoriteGame(intptr_t instancePtr, AppId_t nAppID, uint32 nIP, uint16 nConnPort, uint16 nQueryPort, uint32 unFlags, uint32 rTime32LastPlayedOnServer)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->AddFavoriteGame(nAppID, nIP, nConnPort, nQueryPort, unFlags, rTime32LastPlayedOnServer);
}
S_API bool S_CALLTYPE SteamAPI_ISteamMatchmaking_RemoveFavoriteGame(intptr_t instancePtr, AppId_t nAppID, uint32 nIP, uint16 nConnPort, uint16 nQueryPort, uint32 unFlags)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->RemoveFavoriteGame(nAppID, nIP, nConnPort, nQueryPort, unFlags);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamMatchmaking_RequestLobbyList(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->RequestLobbyList();
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmaking_AddRequestLobbyListStringFilter(intptr_t instancePtr, const char * pchKeyToMatch, const char * pchValueToMatch, ELobbyComparison eComparisonType)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->AddRequestLobbyListStringFilter(pchKeyToMatch, pchValueToMatch, eComparisonType);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmaking_AddRequestLobbyListNumericalFilter(intptr_t instancePtr, const char * pchKeyToMatch, int nValueToMatch, ELobbyComparison eComparisonType)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->AddRequestLobbyListNumericalFilter(pchKeyToMatch, nValueToMatch, eComparisonType);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmaking_AddRequestLobbyListNearValueFilter(intptr_t instancePtr, const char * pchKeyToMatch, int nValueToBeCloseTo)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->AddRequestLobbyListNearValueFilter(pchKeyToMatch, nValueToBeCloseTo);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmaking_AddRequestLobbyListFilterSlotsAvailable(intptr_t instancePtr, int nSlotsAvailable)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->AddRequestLobbyListFilterSlotsAvailable(nSlotsAvailable);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmaking_AddRequestLobbyListDistanceFilter(intptr_t instancePtr, ELobbyDistanceFilter eLobbyDistanceFilter)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->AddRequestLobbyListDistanceFilter(eLobbyDistanceFilter);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmaking_AddRequestLobbyListResultCountFilter(intptr_t instancePtr, int cMaxResults)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->AddRequestLobbyListResultCountFilter(cMaxResults);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmaking_AddRequestLobbyListCompatibleMembersFilter(intptr_t instancePtr, class CSteamID steamIDLobby)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->AddRequestLobbyListCompatibleMembersFilter(steamIDLobby);
}
S_API uint64 S_CALLTYPE SteamAPI_ISteamMatchmaking_GetLobbyByIndex(intptr_t instancePtr, int iLobby)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->GetLobbyByIndex(iLobby).ConvertToUint64();
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamMatchmaking_CreateLobby(intptr_t instancePtr, ELobbyType eLobbyType, int cMaxMembers)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->CreateLobby(eLobbyType, cMaxMembers);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamMatchmaking_JoinLobby(intptr_t instancePtr, class CSteamID steamIDLobby)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->JoinLobby(steamIDLobby);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmaking_LeaveLobby(intptr_t instancePtr, class CSteamID steamIDLobby)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->LeaveLobby(steamIDLobby);
}
S_API bool S_CALLTYPE SteamAPI_ISteamMatchmaking_InviteUserToLobby(intptr_t instancePtr, class CSteamID steamIDLobby, class CSteamID steamIDInvitee)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->InviteUserToLobby(steamIDLobby, steamIDInvitee);
}
S_API int S_CALLTYPE SteamAPI_ISteamMatchmaking_GetNumLobbyMembers(intptr_t instancePtr, class CSteamID steamIDLobby)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->GetNumLobbyMembers(steamIDLobby);
}
S_API uint64 S_CALLTYPE SteamAPI_ISteamMatchmaking_GetLobbyMemberByIndex(intptr_t instancePtr, class CSteamID steamIDLobby, int iMember)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->GetLobbyMemberByIndex(steamIDLobby, iMember).ConvertToUint64();
}
S_API const char* S_CALLTYPE SteamAPI_ISteamMatchmaking_GetLobbyData(intptr_t instancePtr, class CSteamID steamIDLobby, const char * pchKey)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->GetLobbyData(steamIDLobby, pchKey);
}
S_API bool S_CALLTYPE SteamAPI_ISteamMatchmaking_SetLobbyData(intptr_t instancePtr, class CSteamID steamIDLobby, const char * pchKey, const char * pchValue)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->SetLobbyData(steamIDLobby, pchKey, pchValue);
}
S_API int S_CALLTYPE SteamAPI_ISteamMatchmaking_GetLobbyDataCount(intptr_t instancePtr, class CSteamID steamIDLobby)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->GetLobbyDataCount(steamIDLobby);
}
S_API bool S_CALLTYPE SteamAPI_ISteamMatchmaking_GetLobbyDataByIndex(intptr_t instancePtr, class CSteamID steamIDLobby, int iLobbyData, char * pchKey, int cchKeyBufferSize, char * pchValue, int cchValueBufferSize)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->GetLobbyDataByIndex(steamIDLobby, iLobbyData, pchKey, cchKeyBufferSize, pchValue, cchValueBufferSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamMatchmaking_DeleteLobbyData(intptr_t instancePtr, class CSteamID steamIDLobby, const char * pchKey)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->DeleteLobbyData(steamIDLobby, pchKey);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamMatchmaking_GetLobbyMemberData(intptr_t instancePtr, class CSteamID steamIDLobby, class CSteamID steamIDUser, const char * pchKey)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->GetLobbyMemberData(steamIDLobby, steamIDUser, pchKey);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmaking_SetLobbyMemberData(intptr_t instancePtr, class CSteamID steamIDLobby, const char * pchKey, const char * pchValue)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->SetLobbyMemberData(steamIDLobby, pchKey, pchValue);
}
S_API bool S_CALLTYPE SteamAPI_ISteamMatchmaking_SendLobbyChatMsg(intptr_t instancePtr, class CSteamID steamIDLobby, const void * pvMsgBody, int cubMsgBody)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->SendLobbyChatMsg(steamIDLobby, pvMsgBody, cubMsgBody);
}
S_API int S_CALLTYPE SteamAPI_ISteamMatchmaking_GetLobbyChatEntry(intptr_t instancePtr, class CSteamID steamIDLobby, int iChatID, class CSteamID * pSteamIDUser, void * pvData, int cubData, EChatEntryType * peChatEntryType)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->GetLobbyChatEntry(steamIDLobby, iChatID, pSteamIDUser, pvData, cubData, peChatEntryType);
}
S_API bool S_CALLTYPE SteamAPI_ISteamMatchmaking_RequestLobbyData(intptr_t instancePtr, class CSteamID steamIDLobby)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->RequestLobbyData(steamIDLobby);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmaking_SetLobbyGameServer(intptr_t instancePtr, class CSteamID steamIDLobby, uint32 unGameServerIP, uint16 unGameServerPort, class CSteamID steamIDGameServer)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->SetLobbyGameServer(steamIDLobby, unGameServerIP, unGameServerPort, steamIDGameServer);
}
S_API bool S_CALLTYPE SteamAPI_ISteamMatchmaking_GetLobbyGameServer(intptr_t instancePtr, class CSteamID steamIDLobby, uint32 * punGameServerIP, uint16 * punGameServerPort, class CSteamID * psteamIDGameServer)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->GetLobbyGameServer(steamIDLobby, punGameServerIP, punGameServerPort, psteamIDGameServer);
}
S_API bool S_CALLTYPE SteamAPI_ISteamMatchmaking_SetLobbyMemberLimit(intptr_t instancePtr, class CSteamID steamIDLobby, int cMaxMembers)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->SetLobbyMemberLimit(steamIDLobby, cMaxMembers);
}
S_API int S_CALLTYPE SteamAPI_ISteamMatchmaking_GetLobbyMemberLimit(intptr_t instancePtr, class CSteamID steamIDLobby)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->GetLobbyMemberLimit(steamIDLobby);
}
S_API bool S_CALLTYPE SteamAPI_ISteamMatchmaking_SetLobbyType(intptr_t instancePtr, class CSteamID steamIDLobby, ELobbyType eLobbyType)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->SetLobbyType(steamIDLobby, eLobbyType);
}
S_API bool S_CALLTYPE SteamAPI_ISteamMatchmaking_SetLobbyJoinable(intptr_t instancePtr, class CSteamID steamIDLobby, bool bLobbyJoinable)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->SetLobbyJoinable(steamIDLobby, bLobbyJoinable);
}
S_API uint64 S_CALLTYPE SteamAPI_ISteamMatchmaking_GetLobbyOwner(intptr_t instancePtr, class CSteamID steamIDLobby)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->GetLobbyOwner(steamIDLobby).ConvertToUint64();
}
S_API bool S_CALLTYPE SteamAPI_ISteamMatchmaking_SetLobbyOwner(intptr_t instancePtr, class CSteamID steamIDLobby, class CSteamID steamIDNewOwner)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->SetLobbyOwner(steamIDLobby, steamIDNewOwner);
}
S_API bool S_CALLTYPE SteamAPI_ISteamMatchmaking_SetLinkedLobby(intptr_t instancePtr, class CSteamID steamIDLobby, class CSteamID steamIDLobbyDependent)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmaking()->SetLinkedLobby(steamIDLobby, steamIDLobbyDependent);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmakingServerListResponse_ServerResponded(intptr_t instancePtr, HServerListRequest hRequest, int iServer)
{
	if (g_bClientReady == false || instancePtr == 0)
		__debugbreak();
	ISteamMatchmakingServerListResponse *ServerListResponse = (ISteamMatchmakingServerListResponse*)instancePtr;
	return ServerListResponse->ServerResponded(hRequest, iServer);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmakingServerListResponse_ServerFailedToRespond(intptr_t instancePtr, HServerListRequest hRequest, int iServer)
{
	if (g_bClientReady == false || instancePtr == 0)
		__debugbreak();
	ISteamMatchmakingServerListResponse *ServerListResponse = (ISteamMatchmakingServerListResponse*)instancePtr;
	return ServerListResponse->ServerFailedToRespond(hRequest, iServer);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmakingServerListResponse_RefreshComplete(intptr_t instancePtr, HServerListRequest hRequest, EMatchMakingServerResponse response)
{
	if (g_bClientReady == false || instancePtr == 0)
		__debugbreak();
	ISteamMatchmakingServerListResponse *ServerListResponse = (ISteamMatchmakingServerListResponse*)instancePtr;
	return ServerListResponse->RefreshComplete(hRequest, response);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmakingPingResponse_ServerResponded(intptr_t instancePtr, class gameserveritem_t & server)
{
	if (g_bClientReady == false || instancePtr == 0)
		__debugbreak();
	ISteamMatchmakingPingResponse *PingResponse = (ISteamMatchmakingPingResponse*)instancePtr;
	return PingResponse->ServerResponded(server);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmakingPingResponse_ServerFailedToRespond(intptr_t instancePtr)
{
	if (g_bClientReady == false || instancePtr == 0)
		__debugbreak();
	ISteamMatchmakingPingResponse *PingResponse = (ISteamMatchmakingPingResponse*)instancePtr;
	return PingResponse->ServerFailedToRespond();
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmakingPlayersResponse_AddPlayerToList(intptr_t instancePtr, const char * pchName, int nScore, float flTimePlayed)
{
	if (g_bClientReady == false || instancePtr == 0)
		__debugbreak();
	ISteamMatchmakingPlayersResponse *PlayersResponse = (ISteamMatchmakingPlayersResponse*)instancePtr;
	return PlayersResponse->AddPlayerToList(pchName, nScore, flTimePlayed);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmakingPlayersResponse_PlayersFailedToRespond(intptr_t instancePtr)
{
	if (g_bClientReady == false || instancePtr == 0)
		__debugbreak();
	ISteamMatchmakingPlayersResponse *PlayersResponse = (ISteamMatchmakingPlayersResponse*)instancePtr;
	return PlayersResponse->PlayersFailedToRespond();
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmakingPlayersResponse_PlayersRefreshComplete(intptr_t instancePtr)
{
	if (g_bClientReady == false || instancePtr == 0)
		__debugbreak();
	ISteamMatchmakingPlayersResponse *PlayersResponse = (ISteamMatchmakingPlayersResponse*)instancePtr;
	return PlayersResponse->PlayersRefreshComplete();
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmakingRulesResponse_RulesResponded(intptr_t instancePtr, const char * pchRule, const char * pchValue)
{
	if (g_bClientReady == false || instancePtr == 0)
		__debugbreak();
	ISteamMatchmakingRulesResponse *RulesResponse = (ISteamMatchmakingRulesResponse*)instancePtr;
	return RulesResponse->RulesResponded(pchRule, pchValue);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmakingRulesResponse_RulesFailedToRespond(intptr_t instancePtr)
{
	if (g_bClientReady == false || instancePtr == 0)
		__debugbreak();
	ISteamMatchmakingRulesResponse *RulesResponse = (ISteamMatchmakingRulesResponse*)instancePtr;
	return RulesResponse->RulesFailedToRespond();
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmakingRulesResponse_RulesRefreshComplete(intptr_t instancePtr)
{
	if (g_bClientReady == false || instancePtr == 0)
		__debugbreak();
	ISteamMatchmakingRulesResponse *RulesResponse = (ISteamMatchmakingRulesResponse*)instancePtr;
	return RulesResponse->RulesRefreshComplete();
}
S_API HServerListRequest S_CALLTYPE SteamAPI_ISteamMatchmakingServers_RequestInternetServerList(intptr_t instancePtr, AppId_t iApp, struct MatchMakingKeyValuePair_t ** ppchFilters, uint32 nFilters, class ISteamMatchmakingServerListResponse * pRequestServersResponse)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->RequestInternetServerList(iApp, ppchFilters, nFilters, pRequestServersResponse);
}
S_API HServerListRequest S_CALLTYPE SteamAPI_ISteamMatchmakingServers_RequestLANServerList(intptr_t instancePtr, AppId_t iApp, class ISteamMatchmakingServerListResponse * pRequestServersResponse)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->RequestLANServerList(iApp, pRequestServersResponse);
}
S_API HServerListRequest S_CALLTYPE SteamAPI_ISteamMatchmakingServers_RequestFriendsServerList(intptr_t instancePtr, AppId_t iApp, struct MatchMakingKeyValuePair_t ** ppchFilters, uint32 nFilters, class ISteamMatchmakingServerListResponse * pRequestServersResponse)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->RequestFriendsServerList(iApp, ppchFilters, nFilters, pRequestServersResponse);
}
S_API HServerListRequest S_CALLTYPE SteamAPI_ISteamMatchmakingServers_RequestFavoritesServerList(intptr_t instancePtr, AppId_t iApp, struct MatchMakingKeyValuePair_t ** ppchFilters, uint32 nFilters, class ISteamMatchmakingServerListResponse * pRequestServersResponse)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->RequestFavoritesServerList(iApp, ppchFilters, nFilters, pRequestServersResponse);
}
S_API HServerListRequest S_CALLTYPE SteamAPI_ISteamMatchmakingServers_RequestHistoryServerList(intptr_t instancePtr, AppId_t iApp, struct MatchMakingKeyValuePair_t ** ppchFilters, uint32 nFilters, class ISteamMatchmakingServerListResponse * pRequestServersResponse)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->RequestHistoryServerList(iApp, ppchFilters, nFilters, pRequestServersResponse);
}
S_API HServerListRequest S_CALLTYPE SteamAPI_ISteamMatchmakingServers_RequestSpectatorServerList(intptr_t instancePtr, AppId_t iApp, struct MatchMakingKeyValuePair_t ** ppchFilters, uint32 nFilters, class ISteamMatchmakingServerListResponse * pRequestServersResponse)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->RequestSpectatorServerList(iApp, ppchFilters, nFilters, pRequestServersResponse);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmakingServers_ReleaseRequest(intptr_t instancePtr, HServerListRequest hServerListRequest)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->ReleaseRequest(hServerListRequest);
}
S_API gameserveritem_t* S_CALLTYPE SteamAPI_ISteamMatchmakingServers_GetServerDetails(intptr_t instancePtr, HServerListRequest hRequest, int iServer)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->GetServerDetails(hRequest, iServer);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmakingServers_CancelQuery(intptr_t instancePtr, HServerListRequest hRequest)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->CancelQuery(hRequest);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmakingServers_RefreshQuery(intptr_t instancePtr, HServerListRequest hRequest)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->RefreshQuery(hRequest);
}
S_API bool S_CALLTYPE SteamAPI_ISteamMatchmakingServers_IsRefreshing(intptr_t instancePtr, HServerListRequest hRequest)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->IsRefreshing(hRequest);
}
S_API int S_CALLTYPE SteamAPI_ISteamMatchmakingServers_GetServerCount(intptr_t instancePtr, HServerListRequest hRequest)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->GetServerCount(hRequest);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmakingServers_RefreshServer(intptr_t instancePtr, HServerListRequest hRequest, int iServer)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->RefreshServer(hRequest, iServer);
}
S_API HServerQuery S_CALLTYPE SteamAPI_ISteamMatchmakingServers_PingServer(intptr_t instancePtr, uint32 unIP, uint16 usPort, class ISteamMatchmakingPingResponse * pRequestServersResponse)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->PingServer(unIP, usPort, pRequestServersResponse);
}
S_API HServerQuery S_CALLTYPE SteamAPI_ISteamMatchmakingServers_PlayerDetails(intptr_t instancePtr, uint32 unIP, uint16 usPort, class ISteamMatchmakingPlayersResponse * pRequestServersResponse)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->PlayerDetails(unIP, usPort, pRequestServersResponse);
}
S_API HServerQuery S_CALLTYPE SteamAPI_ISteamMatchmakingServers_ServerRules(intptr_t instancePtr, uint32 unIP, uint16 usPort, class ISteamMatchmakingRulesResponse * pRequestServersResponse)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->ServerRules(unIP, usPort, pRequestServersResponse);
}
S_API void S_CALLTYPE SteamAPI_ISteamMatchmakingServers_CancelServerQuery(intptr_t instancePtr, HServerQuery hServerQuery)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMatchmakingServers()->CancelServerQuery(hServerQuery);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamParties_GetNumActiveBeacons(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParties()->GetNumActiveBeacons();
}
S_API PartyBeaconID_t S_CALLTYPE SteamAPI_ISteamParties_GetBeaconByIndex(intptr_t instancePtr, uint32 unIndex)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParties()->GetBeaconByIndex(unIndex);
}
S_API bool S_CALLTYPE SteamAPI_ISteamParties_GetBeaconDetails(intptr_t instancePtr, PartyBeaconID_t ulBeaconID, class CSteamID * pSteamIDBeaconOwner, struct SteamPartyBeaconLocation_t * pLocation, char * pchMetadata, int cchMetadata)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParties()->GetBeaconDetails(ulBeaconID, pSteamIDBeaconOwner, pLocation, pchMetadata, cchMetadata);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamParties_JoinParty(intptr_t instancePtr, PartyBeaconID_t ulBeaconID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParties()->JoinParty(ulBeaconID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamParties_GetNumAvailableBeaconLocations(intptr_t instancePtr, uint32 * puNumLocations)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParties()->GetNumAvailableBeaconLocations(puNumLocations);
}
S_API bool S_CALLTYPE SteamAPI_ISteamParties_GetAvailableBeaconLocations(intptr_t instancePtr, struct SteamPartyBeaconLocation_t * pLocationList, uint32 uMaxNumLocations)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParties()->GetAvailableBeaconLocations(pLocationList, uMaxNumLocations);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamParties_CreateBeacon(intptr_t instancePtr, uint32 unOpenSlots, struct SteamPartyBeaconLocation_t * pBeaconLocation, const char * pchConnectString, const char * pchMetadata)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParties()->CreateBeacon(unOpenSlots, pBeaconLocation, pchConnectString, pchMetadata);
}
S_API void S_CALLTYPE SteamAPI_ISteamParties_OnReservationCompleted(intptr_t instancePtr, PartyBeaconID_t ulBeacon, class CSteamID steamIDUser)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParties()->OnReservationCompleted(ulBeacon, steamIDUser);
}
S_API void S_CALLTYPE SteamAPI_ISteamParties_CancelReservation(intptr_t instancePtr, PartyBeaconID_t ulBeacon, class CSteamID steamIDUser)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParties()->CancelReservation(ulBeacon, steamIDUser);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamParties_ChangeNumOpenSlots(intptr_t instancePtr, PartyBeaconID_t ulBeacon, uint32 unOpenSlots)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParties()->ChangeNumOpenSlots(ulBeacon, unOpenSlots);
}
S_API bool S_CALLTYPE SteamAPI_ISteamParties_DestroyBeacon(intptr_t instancePtr, PartyBeaconID_t ulBeacon)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParties()->DestroyBeacon(ulBeacon);
}
S_API bool S_CALLTYPE SteamAPI_ISteamParties_GetBeaconLocationData(intptr_t instancePtr, struct SteamPartyBeaconLocation_t BeaconLocation, ESteamPartyBeaconLocationData eData, char * pchDataStringOut, int cchDataStringOut)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParties()->GetBeaconLocationData(BeaconLocation, eData, pchDataStringOut, cchDataStringOut);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_FileWrite(intptr_t instancePtr, const char * pchFile, const void * pvData, int32 cubData)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->FileWrite(pchFile, pvData, cubData);
}
S_API int32 S_CALLTYPE SteamAPI_ISteamRemoteStorage_FileRead(intptr_t instancePtr, const char * pchFile, void * pvData, int32 cubDataToRead)
{
	if (g_bClientReady == false)
		__debugbreak();

	int32 result = g_ClientCtx.SteamRemoteStorage()->FileRead(pchFile, pvData, cubDataToRead);

	// Fallback: try direct filesystem read (handles files not in Steam's VDF cloud manifest)
	if (result <= 0 && pchFile && pchFile[0])
	{
		// Get current SteamID for path construction
		uint64 steamID = 0;
		ISteamUser* pUser = g_ClientCtx.SteamUser();
		if (pUser)
			steamID = pUser->GetSteamID().ConvertToUint64();

		// Normalize path separators in the requested file path
		char normalizedFile[MAX_PATH] = {0};
		strcpy_s(normalizedFile, sizeof(normalizedFile), pchFile);
		for (char* p = normalizedFile; *p; p++)
			if (*p == '/') *p = '\\';

		// Try multiple fallback paths
		const int MAX_CANDIDATES = 8;
		char candidates[MAX_CANDIDATES][MAX_PATH * 2] = {{{0}}};
		int nCandidates = 0;

		// 1. Steam userdata path
		if (steamID != 0 && g_InstallPath[0] != '\0')
		{
			_snprintf_s(candidates[nCandidates++], MAX_PATH * 2, _TRUNCATE,
				"%s\\userdata\\%llu\\%u\\remote\\%s",
				g_InstallPath, steamID, g_ForcedAppId, normalizedFile);
		}

		// 2. APPDATA + SteamID folder
		const char* appData = getenv("APPDATA");
		if (appData && appData[0] && steamID != 0)
		{
			_snprintf_s(candidates[nCandidates++], MAX_PATH * 2, _TRUNCATE,
				"%s\\SlayTheSpire2\\steam\\%llu\\%s",
				appData, steamID, normalizedFile);
		}

		// 3. Current working directory
		{
			char cwd[MAX_PATH] = {0};
			if (GetCurrentDirectoryA(MAX_PATH, cwd) && cwd[0])
			{
				_snprintf_s(candidates[nCandidates++], MAX_PATH * 2, _TRUNCATE,
					"%s\\%s", cwd, normalizedFile);
			}
		}

		// 4. Module (DLL) directory - try to find the DLL's path
		{
			HMODULE hMod = GetModuleHandleA("steam_api64.dll");
			if (!hMod) hMod = GetModuleHandleA("steam_api.dll");
			if (hMod)
			{
				char modPath[MAX_PATH] = {0};
				GetModuleFileNameA(hMod, modPath, MAX_PATH);
				char* lastSlash = strrchr(modPath, '\\');
				if (lastSlash)
				{
					*lastSlash = '\0';
					_snprintf_s(candidates[nCandidates++], MAX_PATH * 2, _TRUNCATE,
						"%s\\%s", modPath, normalizedFile);
				}
			}
		}

		// 5. Game exe directory
		{
			char exePath[MAX_PATH] = {0};
			GetModuleFileNameA(NULL, exePath, MAX_PATH);
			char* lastSlash = strrchr(exePath, '\\');
			if (lastSlash)
			{
				*lastSlash = '\0';
				_snprintf_s(candidates[nCandidates++], MAX_PATH * 2, _TRUNCATE,
					"%s\\%s", exePath, normalizedFile);
			}
		}

		// Try each candidate path
		for (int i = 0; i < nCandidates; i++)
		{
			if (candidates[i][0] == '\0') continue;

			// Clean up any double backslashes
			for (char* p = candidates[i]; *p; p++)
				if (*p == '/' || *p == '\\')
				{
					char* next = p + 1;
					while (*next == '/' || *next == '\\') next++;
					if (next != p + 1)
						memmove(p + 1, next, strlen(next) + 1);
				}

			FILE* f = nullptr;
			fopen_s(&f, candidates[i], "rb");
			if (f)
			{
				fseek(f, 0, SEEK_END);
				int32 fileSize = (int32)ftell(f);
				fseek(f, 0, SEEK_SET);
				int32 readSize = (cubDataToRead < fileSize) ? cubDataToRead : fileSize;
				size_t bytesRead = fread(pvData, 1, readSize, f);
				fclose(f);
				if (bytesRead > 0)
					return (int32)bytesRead;
			}
		}
	}

	return result;
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_FileWriteAsync(intptr_t instancePtr, const char * pchFile, const void * pvData, uint32 cubData)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->FileWriteAsync(pchFile, pvData, cubData);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_FileReadAsync(intptr_t instancePtr, const char * pchFile, uint32 nOffset, uint32 cubToRead)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->FileReadAsync(pchFile, nOffset, cubToRead);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_FileReadAsyncComplete(intptr_t instancePtr, SteamAPICall_t hReadCall, void * pvBuffer, uint32 cubToRead)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->FileReadAsyncComplete(hReadCall, pvBuffer, cubToRead);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_FileForget(intptr_t instancePtr, const char * pchFile)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->FileForget(pchFile);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_FileDelete(intptr_t instancePtr, const char * pchFile)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->FileDelete(pchFile);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_FileShare(intptr_t instancePtr, const char * pchFile)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->FileShare(pchFile);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_SetSyncPlatforms(intptr_t instancePtr, const char * pchFile, ERemoteStoragePlatform eRemoteStoragePlatform)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->SetSyncPlatforms(pchFile, eRemoteStoragePlatform);
}
S_API UGCFileWriteStreamHandle_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_FileWriteStreamOpen(intptr_t instancePtr, const char * pchFile)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->FileWriteStreamOpen(pchFile);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_FileWriteStreamWriteChunk(intptr_t instancePtr, UGCFileWriteStreamHandle_t writeHandle, const void * pvData, int32 cubData)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->FileWriteStreamWriteChunk(writeHandle, pvData, cubData);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_FileWriteStreamClose(intptr_t instancePtr, UGCFileWriteStreamHandle_t writeHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->FileWriteStreamClose(writeHandle);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_FileWriteStreamCancel(intptr_t instancePtr, UGCFileWriteStreamHandle_t writeHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->FileWriteStreamCancel(writeHandle);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_FileExists(intptr_t instancePtr, const char * pchFile)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->FileExists(pchFile);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_FilePersisted(intptr_t instancePtr, const char * pchFile)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->FilePersisted(pchFile);
}
S_API int32 S_CALLTYPE SteamAPI_ISteamRemoteStorage_GetFileSize(intptr_t instancePtr, const char * pchFile)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->GetFileSize(pchFile);
}
S_API int64 S_CALLTYPE SteamAPI_ISteamRemoteStorage_GetFileTimestamp(intptr_t instancePtr, const char * pchFile)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->GetFileTimestamp(pchFile);
}
S_API ERemoteStoragePlatform S_CALLTYPE SteamAPI_ISteamRemoteStorage_GetSyncPlatforms(intptr_t instancePtr, const char * pchFile)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->GetSyncPlatforms(pchFile);
}
S_API int32 S_CALLTYPE SteamAPI_ISteamRemoteStorage_GetFileCount(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->GetFileCount();
}
S_API const char* S_CALLTYPE SteamAPI_ISteamRemoteStorage_GetFileNameAndSize(intptr_t instancePtr, int iFile, int32 * pnFileSizeInBytes)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->GetFileNameAndSize(iFile, pnFileSizeInBytes);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_GetQuota(intptr_t instancePtr, uint64 * pnTotalBytes, uint64 * puAvailableBytes)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->GetQuota(pnTotalBytes, puAvailableBytes);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_IsCloudEnabledForAccount(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->IsCloudEnabledForAccount();
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_IsCloudEnabledForApp(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->IsCloudEnabledForApp();
}
S_API void S_CALLTYPE SteamAPI_ISteamRemoteStorage_SetCloudEnabledForApp(intptr_t instancePtr, bool bEnabled)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->SetCloudEnabledForApp(bEnabled);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_UGCDownload(intptr_t instancePtr, UGCHandle_t hContent, uint32 unPriority)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->UGCDownload(hContent, unPriority);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_GetUGCDownloadProgress(intptr_t instancePtr, UGCHandle_t hContent, int32 * pnBytesDownloaded, int32 * pnBytesExpected)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->GetUGCDownloadProgress(hContent, pnBytesDownloaded, pnBytesExpected);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_GetUGCDetails(intptr_t instancePtr, UGCHandle_t hContent, AppId_t * pnAppID, char ** ppchName, int32 * pnFileSizeInBytes, class CSteamID * pSteamIDOwner)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->GetUGCDetails(hContent, pnAppID, ppchName, pnFileSizeInBytes, pSteamIDOwner);
}
S_API int32 S_CALLTYPE SteamAPI_ISteamRemoteStorage_UGCRead(intptr_t instancePtr, UGCHandle_t hContent, void * pvData, int32 cubDataToRead, uint32 cOffset, EUGCReadAction eAction)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->UGCRead(hContent, pvData, cubDataToRead, cOffset, eAction);
}
S_API int32 S_CALLTYPE SteamAPI_ISteamRemoteStorage_GetCachedUGCCount(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->GetCachedUGCCount();
}
S_API UGCHandle_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_GetCachedUGCHandle(intptr_t instancePtr, int32 iCachedContent)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->GetCachedUGCHandle(iCachedContent);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_PublishWorkshopFile(intptr_t instancePtr, const char * pchFile, const char * pchPreviewFile, AppId_t nConsumerAppId, const char * pchTitle, const char * pchDescription, ERemoteStoragePublishedFileVisibility eVisibility, struct SteamParamStringArray_t * pTags, EWorkshopFileType eWorkshopFileType)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->PublishWorkshopFile(pchFile, pchPreviewFile, nConsumerAppId, pchTitle, pchDescription, eVisibility, pTags, eWorkshopFileType);
}
S_API PublishedFileUpdateHandle_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_CreatePublishedFileUpdateRequest(intptr_t instancePtr, PublishedFileId_t unPublishedFileId)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->CreatePublishedFileUpdateRequest(unPublishedFileId);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_UpdatePublishedFileFile(intptr_t instancePtr, PublishedFileUpdateHandle_t updateHandle, const char * pchFile)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->UpdatePublishedFileFile(updateHandle, pchFile);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_UpdatePublishedFilePreviewFile(intptr_t instancePtr, PublishedFileUpdateHandle_t updateHandle, const char * pchPreviewFile)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->UpdatePublishedFilePreviewFile(updateHandle, pchPreviewFile);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_UpdatePublishedFileTitle(intptr_t instancePtr, PublishedFileUpdateHandle_t updateHandle, const char * pchTitle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->UpdatePublishedFileTitle(updateHandle, pchTitle);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_UpdatePublishedFileDescription(intptr_t instancePtr, PublishedFileUpdateHandle_t updateHandle, const char * pchDescription)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->UpdatePublishedFileDescription(updateHandle, pchDescription);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_UpdatePublishedFileVisibility(intptr_t instancePtr, PublishedFileUpdateHandle_t updateHandle, ERemoteStoragePublishedFileVisibility eVisibility)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->UpdatePublishedFileVisibility(updateHandle, eVisibility);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_UpdatePublishedFileTags(intptr_t instancePtr, PublishedFileUpdateHandle_t updateHandle, struct SteamParamStringArray_t * pTags)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->UpdatePublishedFileTags(updateHandle, pTags);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_CommitPublishedFileUpdate(intptr_t instancePtr, PublishedFileUpdateHandle_t updateHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->CommitPublishedFileUpdate(updateHandle);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_GetPublishedFileDetails(intptr_t instancePtr, PublishedFileId_t unPublishedFileId, uint32 unMaxSecondsOld)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->GetPublishedFileDetails(unPublishedFileId, unMaxSecondsOld);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_DeletePublishedFile(intptr_t instancePtr, PublishedFileId_t unPublishedFileId)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->DeletePublishedFile(unPublishedFileId);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_EnumerateUserPublishedFiles(intptr_t instancePtr, uint32 unStartIndex)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->EnumerateUserPublishedFiles(unStartIndex);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_SubscribePublishedFile(intptr_t instancePtr, PublishedFileId_t unPublishedFileId)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->SubscribePublishedFile(unPublishedFileId);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_EnumerateUserSubscribedFiles(intptr_t instancePtr, uint32 unStartIndex)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->EnumerateUserSubscribedFiles(unStartIndex);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_UnsubscribePublishedFile(intptr_t instancePtr, PublishedFileId_t unPublishedFileId)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->UnsubscribePublishedFile(unPublishedFileId);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_UpdatePublishedFileSetChangeDescription(intptr_t instancePtr, PublishedFileUpdateHandle_t updateHandle, const char * pchChangeDescription)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->UpdatePublishedFileSetChangeDescription(updateHandle, pchChangeDescription);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_GetPublishedItemVoteDetails(intptr_t instancePtr, PublishedFileId_t unPublishedFileId)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->GetPublishedItemVoteDetails(unPublishedFileId);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_UpdateUserPublishedItemVote(intptr_t instancePtr, PublishedFileId_t unPublishedFileId, bool bVoteUp)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->UpdateUserPublishedItemVote(unPublishedFileId, bVoteUp);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_GetUserPublishedItemVoteDetails(intptr_t instancePtr, PublishedFileId_t unPublishedFileId)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->GetUserPublishedItemVoteDetails(unPublishedFileId);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_EnumerateUserSharedWorkshopFiles(intptr_t instancePtr, class CSteamID steamId, uint32 unStartIndex, struct SteamParamStringArray_t * pRequiredTags, struct SteamParamStringArray_t * pExcludedTags)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->EnumerateUserSharedWorkshopFiles(steamId, unStartIndex, pRequiredTags, pExcludedTags);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_PublishVideo(intptr_t instancePtr, EWorkshopVideoProvider eVideoProvider, const char * pchVideoAccount, const char * pchVideoIdentifier, const char * pchPreviewFile, AppId_t nConsumerAppId, const char * pchTitle, const char * pchDescription, ERemoteStoragePublishedFileVisibility eVisibility, struct SteamParamStringArray_t * pTags)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->PublishVideo(eVideoProvider, pchVideoAccount, pchVideoIdentifier, pchPreviewFile, nConsumerAppId, pchTitle, pchDescription, eVisibility, pTags);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_SetUserPublishedFileAction(intptr_t instancePtr, PublishedFileId_t unPublishedFileId, EWorkshopFileAction eAction)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->SetUserPublishedFileAction(unPublishedFileId, eAction);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_EnumeratePublishedFilesByUserAction(intptr_t instancePtr, EWorkshopFileAction eAction, uint32 unStartIndex)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->EnumeratePublishedFilesByUserAction(eAction, unStartIndex);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_EnumeratePublishedWorkshopFiles(intptr_t instancePtr, EWorkshopEnumerationType eEnumerationType, uint32 unStartIndex, uint32 unCount, uint32 unDays, struct SteamParamStringArray_t * pTags, struct SteamParamStringArray_t * pUserTags)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->EnumeratePublishedWorkshopFiles(eEnumerationType, unStartIndex, unCount, unDays, pTags, pUserTags);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamRemoteStorage_UGCDownloadToLocation(intptr_t instancePtr, UGCHandle_t hContent, const char * pchLocation, uint32 unPriority)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->UGCDownloadToLocation(hContent, pchLocation, unPriority);
}
S_API int32 S_CALLTYPE SteamAPI_ISteamRemoteStorage_GetLocalFileChangeCount(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->GetLocalFileChangeCount();
}
S_API const char* S_CALLTYPE SteamAPI_ISteamRemoteStorage_GetLocalFileChange(intptr_t instancePtr, int iFile, ERemoteStorageLocalFileChange* pEChangeType, ERemoteStorageFilePathType* pEFilePathType)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->GetLocalFileChange(iFile, pEChangeType, pEFilePathType);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_BeginFileWriteBatch(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->BeginFileWriteBatch();
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemoteStorage_EndFileWriteBatch(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemoteStorage()->EndFileWriteBatch();
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUserStats_RequestCurrentStats(intptr_t instancePtr)
{
	// RequestCurrentStats was removed from SDK (managed by Steam client)
	// Return invalid to let the caller handle gracefully
	return k_uAPICallInvalid;
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_GetStatInt32(intptr_t instancePtr, const char * pchName, int32 * pData)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetStat(pchName, pData);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_GetStatFloat(intptr_t instancePtr, const char * pchName, float * pData)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetStat(pchName, pData);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_SetStatInt32(intptr_t instancePtr, const char * pchName, int32 nData)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->SetStat(pchName, nData);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_SetStatFloat(intptr_t instancePtr, const char * pchName, float fData)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->SetStat(pchName, fData);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_UpdateAvgRateStat(intptr_t instancePtr, const char * pchName, float flCountThisSession, double dSessionLength)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->UpdateAvgRateStat(pchName, flCountThisSession, dSessionLength);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_GetAchievement(intptr_t instancePtr, const char * pchName, bool * pbAchieved)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetAchievement(pchName, pbAchieved);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_SetAchievement(intptr_t instancePtr, const char * pchName)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->SetAchievement(pchName);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_ClearAchievement(intptr_t instancePtr, const char * pchName)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->ClearAchievement(pchName);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_GetAchievementAndUnlockTime(intptr_t instancePtr, const char * pchName, bool * pbAchieved, uint32 * punUnlockTime)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetAchievementAndUnlockTime(pchName, pbAchieved, punUnlockTime);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_StoreStats(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->StoreStats();
}
S_API int S_CALLTYPE SteamAPI_ISteamUserStats_GetAchievementIcon(intptr_t instancePtr, const char * pchName)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetAchievementIcon(pchName);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamUserStats_GetAchievementDisplayAttribute(intptr_t instancePtr, const char * pchName, const char * pchKey)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetAchievementDisplayAttribute(pchName, pchKey);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_IndicateAchievementProgress(intptr_t instancePtr, const char * pchName, uint32 nCurProgress, uint32 nMaxProgress)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->IndicateAchievementProgress(pchName, nCurProgress, nMaxProgress);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUserStats_GetNumAchievements(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetNumAchievements();
}
S_API const char* S_CALLTYPE SteamAPI_ISteamUserStats_GetAchievementName(intptr_t instancePtr, uint32 iAchievement)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetAchievementName(iAchievement);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUserStats_RequestUserStats(intptr_t instancePtr, class CSteamID steamIDUser)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->RequestUserStats(steamIDUser);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_GetUserStatInt32(intptr_t instancePtr, class CSteamID steamIDUser, const char * pchName, int32 * pData)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetUserStat(steamIDUser, pchName, pData);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_GetUserStatFloat(intptr_t instancePtr, class CSteamID steamIDUser, const char * pchName, float * pData)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetUserStat(steamIDUser, pchName, pData);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_GetUserAchievement(intptr_t instancePtr, class CSteamID steamIDUser, const char * pchName, bool * pbAchieved)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetUserAchievement(steamIDUser, pchName, pbAchieved);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_GetUserAchievementAndUnlockTime(intptr_t instancePtr, class CSteamID steamIDUser, const char * pchName, bool * pbAchieved, uint32 * punUnlockTime)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetUserAchievementAndUnlockTime(steamIDUser, pchName, pbAchieved, punUnlockTime);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_ResetAllStats(intptr_t instancePtr, bool bAchievementsToo)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->ResetAllStats(bAchievementsToo);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUserStats_FindOrCreateLeaderboard(intptr_t instancePtr, const char * pchLeaderboardName, ELeaderboardSortMethod eLeaderboardSortMethod, ELeaderboardDisplayType eLeaderboardDisplayType)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->FindOrCreateLeaderboard(pchLeaderboardName, eLeaderboardSortMethod, eLeaderboardDisplayType);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUserStats_FindLeaderboard(intptr_t instancePtr, const char * pchLeaderboardName)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->FindLeaderboard(pchLeaderboardName);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamUserStats_GetLeaderboardName(intptr_t instancePtr, SteamLeaderboard_t hSteamLeaderboard)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetLeaderboardName(hSteamLeaderboard);
}
S_API int S_CALLTYPE SteamAPI_ISteamUserStats_GetLeaderboardEntryCount(intptr_t instancePtr, SteamLeaderboard_t hSteamLeaderboard)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetLeaderboardEntryCount(hSteamLeaderboard);
}
S_API ELeaderboardSortMethod S_CALLTYPE SteamAPI_ISteamUserStats_GetLeaderboardSortMethod(intptr_t instancePtr, SteamLeaderboard_t hSteamLeaderboard)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetLeaderboardSortMethod(hSteamLeaderboard);
}
S_API ELeaderboardDisplayType S_CALLTYPE SteamAPI_ISteamUserStats_GetLeaderboardDisplayType(intptr_t instancePtr, SteamLeaderboard_t hSteamLeaderboard)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetLeaderboardDisplayType(hSteamLeaderboard);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUserStats_DownloadLeaderboardEntries(intptr_t instancePtr, SteamLeaderboard_t hSteamLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->DownloadLeaderboardEntries(hSteamLeaderboard, eLeaderboardDataRequest, nRangeStart, nRangeEnd);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUserStats_DownloadLeaderboardEntriesForUsers(intptr_t instancePtr, SteamLeaderboard_t hSteamLeaderboard, class CSteamID * prgUsers, int cUsers)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->DownloadLeaderboardEntriesForUsers(hSteamLeaderboard, prgUsers, cUsers);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_GetDownloadedLeaderboardEntry(intptr_t instancePtr, SteamLeaderboardEntries_t hSteamLeaderboardEntries, int index, struct LeaderboardEntry_t * pLeaderboardEntry, int32 * pDetails, int cDetailsMax)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetDownloadedLeaderboardEntry(hSteamLeaderboardEntries, index, pLeaderboardEntry, pDetails, cDetailsMax);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUserStats_UploadLeaderboardScore(intptr_t instancePtr, SteamLeaderboard_t hSteamLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int32 nScore, const int32 * pScoreDetails, int cScoreDetailsCount)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->UploadLeaderboardScore(hSteamLeaderboard, eLeaderboardUploadScoreMethod, nScore, pScoreDetails, cScoreDetailsCount);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUserStats_AttachLeaderboardUGC(intptr_t instancePtr, SteamLeaderboard_t hSteamLeaderboard, UGCHandle_t hUGC)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->AttachLeaderboardUGC(hSteamLeaderboard, hUGC);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUserStats_GetNumberOfCurrentPlayers(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetNumberOfCurrentPlayers();
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUserStats_RequestGlobalAchievementPercentages(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->RequestGlobalAchievementPercentages();
}
S_API int S_CALLTYPE SteamAPI_ISteamUserStats_GetMostAchievedAchievementInfo(intptr_t instancePtr, char * pchName, uint32 unNameBufLen, float * pflPercent, bool * pbAchieved)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetMostAchievedAchievementInfo(pchName, unNameBufLen, pflPercent, pbAchieved);
}
S_API int S_CALLTYPE SteamAPI_ISteamUserStats_GetNextMostAchievedAchievementInfo(intptr_t instancePtr, int iIteratorPrevious, char * pchName, uint32 unNameBufLen, float * pflPercent, bool * pbAchieved)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetNextMostAchievedAchievementInfo(iIteratorPrevious, pchName, unNameBufLen, pflPercent, pbAchieved);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_GetAchievementAchievedPercent(intptr_t instancePtr, const char * pchName, float * pflPercent)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetAchievementAchievedPercent(pchName, pflPercent);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUserStats_RequestGlobalStats(intptr_t instancePtr, int nHistoryDays)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->RequestGlobalStats(nHistoryDays);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_GetGlobalStatInt64(intptr_t instancePtr, const char * pchStatName, int64 * pData)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetGlobalStat(pchStatName, pData);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_GetGlobalStatDouble(intptr_t instancePtr, const char * pchStatName, double * pData)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetGlobalStat(pchStatName, pData);
}
S_API int32 S_CALLTYPE SteamAPI_ISteamUserStats_GetGlobalStatHistoryInt64(intptr_t instancePtr, const char * pchStatName, int64 * pData, uint32 cubData)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetGlobalStatHistory(pchStatName, pData, cubData);
}
S_API int32 S_CALLTYPE SteamAPI_ISteamUserStats_GetGlobalStatHistoryDouble(intptr_t instancePtr, const char * pchStatName, double * pData, uint32 cubData)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetGlobalStatHistory(pchStatName, pData, cubData);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_GetAchievementProgressLimitsInt32(intptr_t instancePtr, const char* pchName, int32* pnMinProgress, int32* pnMaxProgress)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetAchievementProgressLimits(pchName, pnMinProgress, pnMaxProgress);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUserStats_GetAchievementProgressLimitsFloat(intptr_t instancePtr, const char* pchName, float* pfMinProgress, float* pfMaxProgress)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUserStats()->GetAchievementProgressLimits(pchName, pfMinProgress, pfMaxProgress);
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_BIsSubscribed(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->BIsSubscribed();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->BIsSubscribed();
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_BIsLowViolence(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->BIsLowViolence();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->BIsLowViolence();
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_BIsCybercafe(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->BIsCybercafe();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->BIsCybercafe();
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_BIsVACBanned(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->BIsVACBanned();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->BIsVACBanned();
}
S_API const char* S_CALLTYPE SteamAPI_ISteamApps_GetCurrentGameLanguage(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->GetCurrentGameLanguage();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->GetCurrentGameLanguage();
}
S_API const char* S_CALLTYPE SteamAPI_ISteamApps_GetAvailableGameLanguages(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->GetAvailableGameLanguages();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->GetAvailableGameLanguages();
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_BIsSubscribedApp(intptr_t instancePtr, AppId_t appID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->BIsSubscribedApp(appID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->BIsSubscribedApp(appID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_BIsDlcInstalled(intptr_t instancePtr, AppId_t appID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->BIsDlcInstalled(appID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->BIsDlcInstalled(appID);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamApps_GetEarliestPurchaseUnixTime(intptr_t instancePtr, AppId_t nAppID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->GetEarliestPurchaseUnixTime(nAppID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->GetEarliestPurchaseUnixTime(nAppID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_BIsSubscribedFromFreeWeekend(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->BIsSubscribedFromFreeWeekend();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->BIsSubscribedFromFreeWeekend();
}
S_API int S_CALLTYPE SteamAPI_ISteamApps_GetDLCCount(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->GetDLCCount();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->GetDLCCount();
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_BGetDLCDataByIndex(intptr_t instancePtr, int iDLC, AppId_t * pAppID, bool * pbAvailable, char * pchName, int cchNameBufferSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->BGetDLCDataByIndex(iDLC, pAppID, pbAvailable, pchName, cchNameBufferSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->BGetDLCDataByIndex(iDLC, pAppID, pbAvailable, pchName, cchNameBufferSize);
}
S_API void S_CALLTYPE SteamAPI_ISteamApps_InstallDLC(intptr_t instancePtr, AppId_t nAppID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->InstallDLC(nAppID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->InstallDLC(nAppID);
}
S_API void S_CALLTYPE SteamAPI_ISteamApps_UninstallDLC(intptr_t instancePtr, AppId_t nAppID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->UninstallDLC(nAppID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->UninstallDLC(nAppID);
}
S_API void S_CALLTYPE SteamAPI_ISteamApps_RequestAppProofOfPurchaseKey(intptr_t instancePtr, AppId_t nAppID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->RequestAppProofOfPurchaseKey(nAppID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->RequestAppProofOfPurchaseKey(nAppID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_GetCurrentBetaName(intptr_t instancePtr, char * pchName, int cchNameBufferSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->GetCurrentBetaName(pchName, cchNameBufferSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->GetCurrentBetaName(pchName, cchNameBufferSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_MarkContentCorrupt(intptr_t instancePtr, bool bMissingFilesOnly)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->MarkContentCorrupt(bMissingFilesOnly);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->MarkContentCorrupt(bMissingFilesOnly);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamApps_GetInstalledDepots(intptr_t instancePtr, AppId_t appID, DepotId_t * pvecDepots, uint32 cMaxDepots)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->GetInstalledDepots(appID, pvecDepots, cMaxDepots);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->GetInstalledDepots(appID, pvecDepots, cMaxDepots);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamApps_GetAppInstallDir(intptr_t instancePtr, AppId_t appID, char * pchFolder, uint32 cchFolderBufferSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->GetAppInstallDir(appID, pchFolder, cchFolderBufferSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->GetAppInstallDir(appID, pchFolder, cchFolderBufferSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_BIsAppInstalled(intptr_t instancePtr, AppId_t appID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->BIsAppInstalled(appID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->BIsAppInstalled(appID);
}
S_API uint64 S_CALLTYPE SteamAPI_ISteamApps_GetAppOwner(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->GetAppOwner().ConvertToUint64();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->GetAppOwner().ConvertToUint64();
}
S_API const char* S_CALLTYPE SteamAPI_ISteamApps_GetLaunchQueryParam(intptr_t instancePtr, const char * pchKey)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->GetLaunchQueryParam(pchKey);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->GetLaunchQueryParam(pchKey);
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_GetDlcDownloadProgress(intptr_t instancePtr, AppId_t nAppID, uint64 * punBytesDownloaded, uint64 * punBytesTotal)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->GetDlcDownloadProgress(nAppID, punBytesDownloaded, punBytesTotal);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->GetDlcDownloadProgress(nAppID, punBytesDownloaded, punBytesTotal);
}
S_API int S_CALLTYPE SteamAPI_ISteamApps_GetAppBuildId(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->GetAppBuildId();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->GetAppBuildId();
}
S_API void S_CALLTYPE SteamAPI_ISteamApps_RequestAllProofOfPurchaseKeys(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->RequestAllProofOfPurchaseKeys();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->RequestAllProofOfPurchaseKeys();
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamApps_GetFileDetails(intptr_t instancePtr, const char * pszFileName)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->GetFileDetails(pszFileName);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->GetFileDetails(pszFileName);
}
S_API int S_CALLTYPE SteamAPI_ISteamApps_GetLaunchCommandLine(intptr_t instancePtr, char * pszCommandLine, int cubCommandLine)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->GetLaunchCommandLine(pszCommandLine, cubCommandLine);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->GetLaunchCommandLine(pszCommandLine, cubCommandLine);
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_BIsSubscribedFromFamilySharing(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->BIsSubscribedFromFamilySharing();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->BIsSubscribedFromFamilySharing();
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_BIsTimedTrial(intptr_t instancePtr, uint32* punSecondsAllowed, uint32* punSecondsPlayed)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->BIsTimedTrial(punSecondsAllowed, punSecondsPlayed);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->BIsTimedTrial(punSecondsAllowed, punSecondsPlayed);
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_SetDlcContext(intptr_t instancePtr, AppId_t nAppID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->SetDlcContext(nAppID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->SetDlcContext(nAppID);
}
S_API int S_CALLTYPE SteamAPI_ISteamApps_GetNumBetas(intptr_t instancePtr, int* pnAvailable, int* pnPrivate)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->GetNumBetas(pnAvailable, pnPrivate);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->GetNumBetas(pnAvailable, pnPrivate);
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_GetBetaInfo(intptr_t instancePtr, int iBetaIndex, uint32* punFlags, uint32* punBuildID, char* pchBetaName, int cchBetaName, char* pchDescription, int cchDescription)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->GetBetaInfo(iBetaIndex, punFlags, punBuildID, pchBetaName, cchBetaName, pchDescription, cchDescription);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->GetBetaInfo(iBetaIndex, punFlags, punBuildID, pchBetaName, cchBetaName, pchDescription, cchDescription);
}
S_API bool S_CALLTYPE SteamAPI_ISteamApps_SetActiveBeta(intptr_t instancePtr, const char* pchBetaName)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamApps() || g_bClientReady == false)
			return g_ServerCtx.SteamApps()->SetActiveBeta(pchBetaName);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamApps()->SetActiveBeta(pchBetaName);
}
S_API EResult S_CALLTYPE SteamAPI_ISteamNetworkingMessages_SendMessageToUser(intptr_t instancePtr, const SteamNetworkingIdentity& identityRemote, const void* pubData, uint32 cubData, int nSendFlags, int nRemoteChannel)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingMessages* pSteamNetworkingMessages = (ISteamNetworkingMessages*)pInterfaceAddress;
			return pSteamNetworkingMessages->SendMessageToUser(identityRemote, pubData, cubData, nSendFlags, nRemoteChannel);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
	ISteamNetworkingMessages* pSteamNetworkingMessages = (ISteamNetworkingMessages*)pInterfaceAddress;
	return pSteamNetworkingMessages->SendMessageToUser(identityRemote, pubData, cubData, nSendFlags, nRemoteChannel);
}
S_API int S_CALLTYPE SteamAPI_ISteamNetworkingMessages_ReceiveMessagesOnChannel(intptr_t instancePtr, int nLocalChannel, SteamNetworkingMessage_t** ppOutMessages, int nMaxMessages)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingMessages* pSteamNetworkingMessages = (ISteamNetworkingMessages*)pInterfaceAddress;
			return pSteamNetworkingMessages->ReceiveMessagesOnChannel(nLocalChannel, ppOutMessages, nMaxMessages);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
	ISteamNetworkingMessages* pSteamNetworkingMessages = (ISteamNetworkingMessages*)pInterfaceAddress;
	return pSteamNetworkingMessages->ReceiveMessagesOnChannel(nLocalChannel, ppOutMessages, nMaxMessages);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingMessages_AcceptSessionWithUser(intptr_t instancePtr, const SteamNetworkingIdentity& identityRemote)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingMessages* pSteamNetworkingMessages = (ISteamNetworkingMessages*)pInterfaceAddress;
			return pSteamNetworkingMessages->AcceptSessionWithUser(identityRemote);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
	ISteamNetworkingMessages* pSteamNetworkingMessages = (ISteamNetworkingMessages*)pInterfaceAddress;
	return pSteamNetworkingMessages->AcceptSessionWithUser(identityRemote);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingMessages_CloseSessionWithUser(intptr_t instancePtr, const SteamNetworkingIdentity& identityRemote)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingMessages* pSteamNetworkingMessages = (ISteamNetworkingMessages*)pInterfaceAddress;
			return pSteamNetworkingMessages->CloseSessionWithUser(identityRemote);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
	ISteamNetworkingMessages* pSteamNetworkingMessages = (ISteamNetworkingMessages*)pInterfaceAddress;
	return pSteamNetworkingMessages->CloseSessionWithUser(identityRemote);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingMessages_CloseChannelWithUser(intptr_t instancePtr, const SteamNetworkingIdentity& identityRemote, int nLocalChannel)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingMessages* pSteamNetworkingMessages = (ISteamNetworkingMessages*)pInterfaceAddress;
			return pSteamNetworkingMessages->CloseChannelWithUser(identityRemote, nLocalChannel);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
	ISteamNetworkingMessages* pSteamNetworkingMessages = (ISteamNetworkingMessages*)pInterfaceAddress;
	return pSteamNetworkingMessages->CloseChannelWithUser(identityRemote, nLocalChannel);
}
S_API ESteamNetworkingConnectionState S_CALLTYPE SteamAPI_ISteamNetworkingMessages_GetSessionConnectionInfo(intptr_t instancePtr, const SteamNetworkingIdentity& identityRemote, SteamNetConnectionInfo_t* pConnectionInfo, SteamNetConnectionRealTimeStatus_t* pQuickStatus)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingMessages* pSteamNetworkingMessages = (ISteamNetworkingMessages*)pInterfaceAddress;
			return pSteamNetworkingMessages->GetSessionConnectionInfo(identityRemote, pConnectionInfo, pQuickStatus);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
	ISteamNetworkingMessages* pSteamNetworkingMessages = (ISteamNetworkingMessages*)pInterfaceAddress;
	return pSteamNetworkingMessages->GetSessionConnectionInfo(identityRemote, pConnectionInfo, pQuickStatus);
}
S_API HSteamListenSocket S_CALLTYPE SteamAPI_ISteamNetworkingSockets_CreateListenSocketIP(intptr_t instancePtr, const SteamNetworkingIPAddr& localAddress, int nOptions, const SteamNetworkingConfigValue_t* pOptions)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->CreateListenSocketIP(localAddress, nOptions, pOptions);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->CreateListenSocketIP(localAddress, nOptions, pOptions);
}
S_API HSteamNetConnection S_CALLTYPE SteamAPI_ISteamNetworkingSockets_ConnectByIPAddress(intptr_t instancePtr, const SteamNetworkingIPAddr& address, int nOptions, const SteamNetworkingConfigValue_t* pOptions)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->ConnectByIPAddress(address, nOptions, pOptions);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->ConnectByIPAddress(address, nOptions, pOptions);
}
S_API HSteamListenSocket S_CALLTYPE SteamAPI_ISteamNetworkingSockets_CreateListenSocketP2P(intptr_t instancePtr, int nLocalVirtualPort, int nOptions, const SteamNetworkingConfigValue_t* pOptions)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->CreateListenSocketP2P(nLocalVirtualPort, nOptions, pOptions);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->CreateListenSocketP2P(nLocalVirtualPort, nOptions, pOptions);
}
S_API HSteamNetConnection S_CALLTYPE SteamAPI_ISteamNetworkingSockets_ConnectP2P(intptr_t instancePtr, const SteamNetworkingIdentity& identityRemote, int nRemoteVirtualPort, int nOptions, const SteamNetworkingConfigValue_t* pOptions)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->ConnectP2P(identityRemote, nRemoteVirtualPort, nOptions, pOptions);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->ConnectP2P(identityRemote, nRemoteVirtualPort, nOptions, pOptions);
}
S_API EResult S_CALLTYPE SteamAPI_ISteamNetworkingSockets_AcceptConnection(intptr_t instancePtr, HSteamNetConnection hConn)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->AcceptConnection(hConn);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->AcceptConnection(hConn);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingSockets_CloseConnection(intptr_t instancePtr, HSteamNetConnection hPeer, int nReason, const char* pszDebug, bool bEnableLinger)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->CloseConnection(hPeer, nReason, pszDebug, bEnableLinger);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->CloseConnection(hPeer, nReason, pszDebug, bEnableLinger);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingSockets_CloseListenSocket(intptr_t instancePtr, HSteamListenSocket hSocket)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->CloseListenSocket(hSocket);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->CloseListenSocket(hSocket);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingSockets_SetConnectionUserData(intptr_t instancePtr, HSteamNetConnection hPeer, int64 nUserData)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->SetConnectionUserData(hPeer, nUserData);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->SetConnectionUserData(hPeer, nUserData);
}
S_API int64 S_CALLTYPE SteamAPI_ISteamNetworkingSockets_GetConnectionUserData(intptr_t instancePtr, HSteamNetConnection hPeer)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->GetConnectionUserData(hPeer);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->GetConnectionUserData(hPeer);
}
S_API void S_CALLTYPE SteamAPI_ISteamNetworkingSockets_SetConnectionName(intptr_t instancePtr, HSteamNetConnection hPeer, const char* pszName)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->SetConnectionName(hPeer, pszName);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->SetConnectionName(hPeer, pszName);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingSockets_GetConnectionName(intptr_t instancePtr, HSteamNetConnection hPeer, char* pszName, int nMaxLen)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->GetConnectionName(hPeer, pszName, nMaxLen);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->GetConnectionName(hPeer, pszName, nMaxLen);
}
S_API EResult S_CALLTYPE SteamAPI_ISteamNetworkingSockets_SendMessageToConnection(intptr_t instancePtr, HSteamNetConnection hConn, const void* pData, uint32 cbData, int nSendFlags, int64* pOutMessageNumber)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->SendMessageToConnection(hConn, pData, cbData, nSendFlags, pOutMessageNumber);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->SendMessageToConnection(hConn, pData, cbData, nSendFlags, pOutMessageNumber);
}
S_API void S_CALLTYPE SteamAPI_ISteamNetworkingSockets_SendMessages(intptr_t instancePtr, int nMessages, SteamNetworkingMessage_t* const* pMessages, int64* pOutMessageNumberOrResult)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->SendMessages(nMessages, pMessages, pOutMessageNumberOrResult);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->SendMessages(nMessages, pMessages, pOutMessageNumberOrResult);
}
S_API EResult S_CALLTYPE SteamAPI_ISteamNetworkingSockets_FlushMessagesOnConnection(intptr_t instancePtr, HSteamNetConnection hConn)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->FlushMessagesOnConnection(hConn);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->FlushMessagesOnConnection(hConn);
}
S_API int S_CALLTYPE SteamAPI_ISteamNetworkingSockets_ReceiveMessagesOnConnection(intptr_t instancePtr, HSteamNetConnection hConn, SteamNetworkingMessage_t** ppOutMessages, int nMaxMessages)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->ReceiveMessagesOnConnection(hConn, ppOutMessages, nMaxMessages);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->ReceiveMessagesOnConnection(hConn, ppOutMessages, nMaxMessages);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingSockets_GetConnectionInfo(intptr_t instancePtr, HSteamNetConnection hConn, SteamNetConnectionInfo_t* pInfo)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->GetConnectionInfo(hConn, pInfo);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->GetConnectionInfo(hConn, pInfo);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingSockets_GetConnectionRealTimeStatus(intptr_t instancePtr, HSteamNetConnection hConn, SteamNetConnectionRealTimeStatus_t* pStatus, int nLanes, SteamNetConnectionRealTimeLaneStatus_t* pLanes)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->GetConnectionRealTimeStatus(hConn, pStatus, nLanes, pLanes);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->GetConnectionRealTimeStatus(hConn, pStatus, nLanes, pLanes);
}
S_API int S_CALLTYPE SteamAPI_ISteamNetworkingSockets_GetDetailedConnectionStatus(intptr_t instancePtr, HSteamNetConnection hConn, char* pszBuf, int cbBuf)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->GetDetailedConnectionStatus(hConn, pszBuf, cbBuf);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->GetDetailedConnectionStatus(hConn, pszBuf, cbBuf);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingSockets_GetListenSocketAddress(intptr_t instancePtr, HSteamListenSocket hSocket, SteamNetworkingIPAddr* address)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->GetListenSocketAddress(hSocket, address);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->GetListenSocketAddress(hSocket, address);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingSockets_CreateSocketPair(intptr_t instancePtr, HSteamNetConnection* pOutConnection1, HSteamNetConnection* pOutConnection2, bool bUseNetworkLoopback, const SteamNetworkingIdentity* pIdentity1, const SteamNetworkingIdentity* pIdentity2)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->CreateSocketPair(pOutConnection1, pOutConnection2, bUseNetworkLoopback, pIdentity1, pIdentity2);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->CreateSocketPair(pOutConnection1, pOutConnection2, bUseNetworkLoopback, pIdentity1, pIdentity2);
}
S_API EResult S_CALLTYPE SteamAPI_ISteamNetworkingSockets_ConfigureConnectionLanes(intptr_t instancePtr, HSteamNetConnection hConn, int nNumLanes, const int* pLanePriorities, const uint16* pLaneWeights)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->ConfigureConnectionLanes(hConn, nNumLanes, pLanePriorities, pLaneWeights);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->ConfigureConnectionLanes(hConn, nNumLanes, pLanePriorities, pLaneWeights);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingSockets_GetIdentity(intptr_t instancePtr, SteamNetworkingIdentity* pIdentity)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->GetIdentity(pIdentity);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->GetIdentity(pIdentity);
}
S_API ESteamNetworkingAvailability S_CALLTYPE SteamAPI_ISteamNetworkingSockets_InitAuthentication(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->InitAuthentication();
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->InitAuthentication();
}
S_API ESteamNetworkingAvailability S_CALLTYPE SteamAPI_ISteamNetworkingSockets_GetAuthenticationStatus(intptr_t instancePtr, SteamNetAuthenticationStatus_t* pDetails)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->GetAuthenticationStatus(pDetails);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->GetAuthenticationStatus(pDetails);
}
S_API HSteamNetPollGroup S_CALLTYPE SteamAPI_ISteamNetworkingSockets_CreatePollGroup(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->CreatePollGroup();
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->CreatePollGroup();
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingSockets_DestroyPollGroup(intptr_t instancePtr, HSteamNetPollGroup hPollGroup)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->DestroyPollGroup(hPollGroup);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->DestroyPollGroup(hPollGroup);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingSockets_SetConnectionPollGroup(intptr_t instancePtr, HSteamNetConnection hConn, HSteamNetPollGroup hPollGroup)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->SetConnectionPollGroup(hConn, hPollGroup);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->SetConnectionPollGroup(hConn, hPollGroup);
}
S_API int S_CALLTYPE SteamAPI_ISteamNetworkingSockets_ReceiveMessagesOnPollGroup(intptr_t instancePtr, HSteamNetPollGroup hPollGroup, SteamNetworkingMessage_t** ppOutMessages, int nMaxMessages)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->ReceiveMessagesOnPollGroup(hPollGroup, ppOutMessages, nMaxMessages);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->ReceiveMessagesOnPollGroup(hPollGroup, ppOutMessages, nMaxMessages);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingSockets_ReceivedRelayAuthTicket(intptr_t instancePtr, const void* pvTicket, int cbTicket, SteamDatagramRelayAuthTicket* pOutParsedTicket)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->ReceivedRelayAuthTicket(pvTicket, cbTicket, pOutParsedTicket);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->ReceivedRelayAuthTicket(pvTicket, cbTicket, pOutParsedTicket);
}
S_API int S_CALLTYPE SteamAPI_ISteamNetworkingSockets_FindRelayAuthTicketForServer(intptr_t instancePtr, const SteamNetworkingIdentity& identityGameServer, int nRemoteVirtualPort, SteamDatagramRelayAuthTicket* pOutParsedTicket)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->FindRelayAuthTicketForServer(identityGameServer, nRemoteVirtualPort, pOutParsedTicket);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->FindRelayAuthTicketForServer(identityGameServer, nRemoteVirtualPort, pOutParsedTicket);
}
S_API HSteamNetConnection S_CALLTYPE SteamAPI_ISteamNetworkingSockets_ConnectToHostedDedicatedServer(intptr_t instancePtr, const SteamNetworkingIdentity& identityTarget, int nRemoteVirtualPort, int nOptions, const SteamNetworkingConfigValue_t* pOptions)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->ConnectToHostedDedicatedServer(identityTarget, nRemoteVirtualPort, nOptions, pOptions);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->ConnectToHostedDedicatedServer(identityTarget, nRemoteVirtualPort, nOptions, pOptions);
}
S_API uint16 S_CALLTYPE SteamAPI_ISteamNetworkingSockets_GetHostedDedicatedServerPort(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->GetHostedDedicatedServerPort();
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->GetHostedDedicatedServerPort();
}
S_API SteamNetworkingPOPID S_CALLTYPE SteamAPI_ISteamNetworkingSockets_GetHostedDedicatedServerPOPID(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->GetHostedDedicatedServerPOPID();
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->GetHostedDedicatedServerPOPID();
}
S_API EResult S_CALLTYPE SteamAPI_ISteamNetworkingSockets_GetHostedDedicatedServerAddress(intptr_t instancePtr, SteamDatagramHostedAddress* pRouting)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->GetHostedDedicatedServerAddress(pRouting);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->GetHostedDedicatedServerAddress(pRouting);
}
S_API HSteamListenSocket S_CALLTYPE SteamAPI_ISteamNetworkingSockets_CreateHostedDedicatedServerListenSocket(intptr_t instancePtr, int nLocalVirtualPort, int nOptions, const SteamNetworkingConfigValue_t* pOptions)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->CreateHostedDedicatedServerListenSocket(nLocalVirtualPort, nOptions, pOptions);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->CreateHostedDedicatedServerListenSocket(nLocalVirtualPort, nOptions, pOptions);
}
S_API EResult S_CALLTYPE SteamAPI_ISteamNetworkingSockets_GetGameCoordinatorServerLogin(intptr_t instancePtr, SteamDatagramGameCoordinatorServerLogin* pLoginInfo, int* pcbSignedBlob, void* pBlob)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->GetGameCoordinatorServerLogin(pLoginInfo, pcbSignedBlob, pBlob);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->GetGameCoordinatorServerLogin(pLoginInfo, pcbSignedBlob, pBlob);
}
S_API HSteamNetConnection S_CALLTYPE SteamAPI_ISteamNetworkingSockets_ConnectP2PCustomSignaling(intptr_t instancePtr, ISteamNetworkingConnectionSignaling* pSignaling, const SteamNetworkingIdentity* pPeerIdentity, int nRemoteVirtualPort, int nOptions, const SteamNetworkingConfigValue_t* pOptions)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->ConnectP2PCustomSignaling(pSignaling, pPeerIdentity, nRemoteVirtualPort, nOptions, pOptions);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->ConnectP2PCustomSignaling(pSignaling, pPeerIdentity, nRemoteVirtualPort, nOptions, pOptions);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingSockets_ReceivedP2PCustomSignal(intptr_t instancePtr, const void* pMsg, int cbMsg, ISteamNetworkingSignalingRecvContext* pContext)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->ReceivedP2PCustomSignal(pMsg, cbMsg, pContext);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->ReceivedP2PCustomSignal(pMsg, cbMsg, pContext);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingSockets_GetCertificateRequest(intptr_t instancePtr, int* pcbBlob, void* pBlob, SteamNetworkingErrMsg& errMsg)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->GetCertificateRequest(pcbBlob, pBlob, errMsg);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->GetCertificateRequest(pcbBlob, pBlob, errMsg);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingSockets_SetCertificate(intptr_t instancePtr, const void* pCertificate, int cbCertificate, SteamNetworkingErrMsg& errMsg)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->SetCertificate(pCertificate, cbCertificate, errMsg);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->SetCertificate(pCertificate, cbCertificate, errMsg);
}
S_API void S_CALLTYPE SteamAPI_ISteamNetworkingSockets_ResetIdentity(intptr_t instancePtr, const SteamNetworkingIdentity* pIdentity)
{
	return;
}
S_API void S_CALLTYPE SteamAPI_ISteamNetworkingSockets_RunCallbacks(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->RunCallbacks();
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->RunCallbacks();
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingSockets_BeginAsyncRequestFakeIP(intptr_t instancePtr, int nNumPorts)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->BeginAsyncRequestFakeIP(nNumPorts);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->BeginAsyncRequestFakeIP(nNumPorts);
}
S_API void S_CALLTYPE SteamAPI_ISteamNetworkingSockets_GetFakeIP(intptr_t instancePtr, int idxFirstPort, SteamNetworkingFakeIPResult_t* pInfo)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->GetFakeIP(idxFirstPort, pInfo);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->GetFakeIP(idxFirstPort, pInfo);
}
S_API HSteamListenSocket S_CALLTYPE SteamAPI_ISteamNetworkingSockets_CreateListenSocketP2PFakeIP(intptr_t instancePtr, int idxFakePort, int nOptions, const SteamNetworkingConfigValue_t* pOptions)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->CreateListenSocketP2PFakeIP(idxFakePort, nOptions, pOptions);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->CreateListenSocketP2PFakeIP(idxFakePort, nOptions, pOptions);
}
S_API EResult S_CALLTYPE SteamAPI_ISteamNetworkingSockets_GetRemoteFakeIPForConnection(intptr_t instancePtr, HSteamNetConnection hConn, SteamNetworkingIPAddr* pOutAddr)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->GetRemoteFakeIPForConnection(hConn, pOutAddr);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->GetRemoteFakeIPForConnection(hConn, pOutAddr);
}
S_API ISteamNetworkingFakeUDPPort* S_CALLTYPE SteamAPI_ISteamNetworkingSockets_CreateFakeUDPPort(intptr_t instancePtr, int idxFakeServerPort)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
			return pSteamNetworkingSockets->CreateFakeUDPPort(idxFakeServerPort);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
	ISteamNetworkingSockets* pSteamNetworkingSockets = (ISteamNetworkingSockets*)pInterfaceAddress;
	return pSteamNetworkingSockets->CreateFakeUDPPort(idxFakeServerPort);
}
S_API SteamNetworkingMessage_t* S_CALLTYPE SteamAPI_ISteamNetworkingUtils_AllocateMessage(intptr_t instancePtr, int cbAllocateBuffer)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->AllocateMessage(cbAllocateBuffer);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->AllocateMessage(cbAllocateBuffer);
}
S_API void S_CALLTYPE SteamAPI_ISteamNetworkingUtils_InitRelayNetworkAccess(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->InitRelayNetworkAccess();
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->InitRelayNetworkAccess();
}
S_API ESteamNetworkingAvailability S_CALLTYPE SteamAPI_ISteamNetworkingUtils_GetRelayNetworkStatus(intptr_t instancePtr, SteamRelayNetworkStatus_t* pDetails)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->GetRelayNetworkStatus(pDetails);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->GetRelayNetworkStatus(pDetails);
}
S_API float S_CALLTYPE SteamAPI_ISteamNetworkingUtils_GetLocalPingLocation(intptr_t instancePtr, SteamNetworkPingLocation_t& result)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->GetLocalPingLocation(result);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->GetLocalPingLocation(result);
}
S_API int S_CALLTYPE SteamAPI_ISteamNetworkingUtils_EstimatePingTimeBetweenTwoLocations(intptr_t instancePtr, const SteamNetworkPingLocation_t& location1, const SteamNetworkPingLocation_t& location2)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->EstimatePingTimeBetweenTwoLocations(location1, location2);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->EstimatePingTimeBetweenTwoLocations(location1, location2);
}
S_API int S_CALLTYPE SteamAPI_ISteamNetworkingUtils_EstimatePingTimeFromLocalHost(intptr_t instancePtr, const SteamNetworkPingLocation_t& remoteLocation)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->EstimatePingTimeFromLocalHost(remoteLocation);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->EstimatePingTimeFromLocalHost(remoteLocation);
}
S_API void S_CALLTYPE SteamAPI_ISteamNetworkingUtils_ConvertPingLocationToString(intptr_t instancePtr, const SteamNetworkPingLocation_t& location, char* pszBuf, int cchBufSize)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->ConvertPingLocationToString(location, pszBuf, cchBufSize);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->ConvertPingLocationToString(location, pszBuf, cchBufSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_ParsePingLocationString(intptr_t instancePtr, const char* pszString, SteamNetworkPingLocation_t& result)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->ParsePingLocationString(pszString, result);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->ParsePingLocationString(pszString, result);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_CheckPingDataUpToDate(intptr_t instancePtr, float flMaxAgeSeconds)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->CheckPingDataUpToDate(flMaxAgeSeconds);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->CheckPingDataUpToDate(flMaxAgeSeconds);
}
S_API int S_CALLTYPE SteamAPI_ISteamNetworkingUtils_GetPingToDataCenter(intptr_t instancePtr, SteamNetworkingPOPID popID, SteamNetworkingPOPID* pViaRelayPoP)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->GetPingToDataCenter(popID, pViaRelayPoP);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->GetPingToDataCenter(popID, pViaRelayPoP);
}
S_API int S_CALLTYPE SteamAPI_ISteamNetworkingUtils_GetDirectPingToPOP(intptr_t instancePtr, SteamNetworkingPOPID popID)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->GetDirectPingToPOP(popID);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->GetDirectPingToPOP(popID);
}
S_API int S_CALLTYPE SteamAPI_ISteamNetworkingUtils_GetPOPCount(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->GetPOPCount();
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->GetPOPCount();
}
S_API int S_CALLTYPE SteamAPI_ISteamNetworkingUtils_GetPOPList(intptr_t instancePtr, SteamNetworkingPOPID* list, int nListSz)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->GetPOPList(list, nListSz);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->GetPOPList(list, nListSz);
}
S_API SteamNetworkingMicroseconds S_CALLTYPE SteamAPI_ISteamNetworkingUtils_GetLocalTimestamp(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->GetLocalTimestamp();
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->GetLocalTimestamp();
}
S_API void S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SetDebugOutputFunction(intptr_t instancePtr, ESteamNetworkingSocketsDebugOutputType eDetailLevel, FSteamNetworkingSocketsDebugOutput pfnFunc)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SetDebugOutputFunction(eDetailLevel, pfnFunc);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SetDebugOutputFunction(eDetailLevel, pfnFunc);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_IsFakeIPv4(intptr_t instancePtr, uint32 nIPv4)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->IsFakeIPv4(nIPv4);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->IsFakeIPv4(nIPv4);
}
S_API ESteamNetworkingFakeIPType S_CALLTYPE SteamAPI_ISteamNetworkingUtils_GetIPv4FakeIPType(intptr_t instancePtr, uint32 nIPv4)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->GetIPv4FakeIPType(nIPv4);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->GetIPv4FakeIPType(nIPv4);
}
S_API EResult S_CALLTYPE SteamAPI_ISteamNetworkingUtils_GetRealIdentityForFakeIP(intptr_t instancePtr, const SteamNetworkingIPAddr& fakeIP, SteamNetworkingIdentity* pOutRealIdentity)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->GetRealIdentityForFakeIP(fakeIP, pOutRealIdentity);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->GetRealIdentityForFakeIP(fakeIP, pOutRealIdentity);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SetGlobalConfigValueInt32(intptr_t instancePtr, ESteamNetworkingConfigValue eValue, int32 val)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SetGlobalConfigValueInt32(eValue, val);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SetGlobalConfigValueInt32(eValue, val);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SetGlobalConfigValueFloat(intptr_t instancePtr, ESteamNetworkingConfigValue eValue, float val)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SetGlobalConfigValueFloat(eValue, val);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SetGlobalConfigValueFloat(eValue, val);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SetGlobalConfigValueString(intptr_t instancePtr, ESteamNetworkingConfigValue eValue, const char* val)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SetGlobalConfigValueString(eValue, val);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SetGlobalConfigValueString(eValue, val);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SetGlobalConfigValuePtr(intptr_t instancePtr, ESteamNetworkingConfigValue eValue, void* val)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SetGlobalConfigValuePtr(eValue, val);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SetGlobalConfigValuePtr(eValue, val);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SetConnectionConfigValueInt32(intptr_t instancePtr, HSteamNetConnection hConn, ESteamNetworkingConfigValue eValue, int32 val)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SetConnectionConfigValueInt32(hConn, eValue, val);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SetConnectionConfigValueInt32(hConn, eValue, val);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SetConnectionConfigValueFloat(intptr_t instancePtr, HSteamNetConnection hConn, ESteamNetworkingConfigValue eValue, float val)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SetConnectionConfigValueFloat(hConn, eValue, val);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SetConnectionConfigValueFloat(hConn, eValue, val);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SetConnectionConfigValueString(intptr_t instancePtr, HSteamNetConnection hConn, ESteamNetworkingConfigValue eValue, const char* val)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SetConnectionConfigValueString(hConn, eValue, val);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SetConnectionConfigValueString(hConn, eValue, val);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SetGlobalCallback_SteamNetConnectionStatusChanged(intptr_t instancePtr, FnSteamNetConnectionStatusChanged fnCallback)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SetGlobalCallback_SteamNetConnectionStatusChanged(fnCallback);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SetGlobalCallback_SteamNetConnectionStatusChanged(fnCallback);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SetGlobalCallback_SteamNetAuthenticationStatusChanged(intptr_t instancePtr, FnSteamNetAuthenticationStatusChanged fnCallback)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SetGlobalCallback_SteamNetAuthenticationStatusChanged(fnCallback);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SetGlobalCallback_SteamNetAuthenticationStatusChanged(fnCallback);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SetGlobalCallback_SteamRelayNetworkStatusChanged(intptr_t instancePtr, FnSteamRelayNetworkStatusChanged fnCallback)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SetGlobalCallback_SteamRelayNetworkStatusChanged(fnCallback);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SetGlobalCallback_SteamRelayNetworkStatusChanged(fnCallback);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SetGlobalCallback_FakeIPResult(intptr_t instancePtr, FnSteamNetworkingFakeIPResult fnCallback)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SetGlobalCallback_FakeIPResult(fnCallback);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SetGlobalCallback_FakeIPResult(fnCallback);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SetGlobalCallback_MessagesSessionRequest(intptr_t instancePtr, FnSteamNetworkingMessagesSessionRequest fnCallback)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SetGlobalCallback_MessagesSessionRequest(fnCallback);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SetGlobalCallback_MessagesSessionRequest(fnCallback);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SetGlobalCallback_MessagesSessionFailed(intptr_t instancePtr, FnSteamNetworkingMessagesSessionFailed fnCallback)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SetGlobalCallback_MessagesSessionFailed(fnCallback);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SetGlobalCallback_MessagesSessionFailed(fnCallback);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SetConfigValue(intptr_t instancePtr, ESteamNetworkingConfigValue eValue, ESteamNetworkingConfigScope eScopeType, intptr_t scopeObj, ESteamNetworkingConfigDataType eDataType, const void* pArg)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SetConfigValue(eValue, eScopeType, scopeObj, eDataType, pArg);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SetConfigValue(eValue, eScopeType, scopeObj, eDataType, pArg);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SetConfigValueStruct(intptr_t instancePtr, const SteamNetworkingConfigValue_t& opt, ESteamNetworkingConfigScope eScopeType, intptr_t scopeObj)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SetConfigValueStruct(opt, eScopeType, scopeObj);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SetConfigValueStruct(opt, eScopeType, scopeObj);
}
S_API ESteamNetworkingGetConfigValueResult S_CALLTYPE SteamAPI_ISteamNetworkingUtils_GetConfigValue(intptr_t instancePtr, ESteamNetworkingConfigValue eValue, ESteamNetworkingConfigScope eScopeType, intptr_t scopeObj, ESteamNetworkingConfigDataType* pOutDataType, void* pResult, size_t* cbResult)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->GetConfigValue(eValue, eScopeType, scopeObj, pOutDataType, pResult, cbResult);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->GetConfigValue(eValue, eScopeType, scopeObj, pOutDataType, pResult, cbResult);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamNetworkingUtils_GetConfigValueInfo(intptr_t instancePtr, ESteamNetworkingConfigValue eValue, ESteamNetworkingConfigDataType* pOutDataType, ESteamNetworkingConfigScope* pOutScope)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->GetConfigValueInfo(eValue, pOutDataType, pOutScope);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->GetConfigValueInfo(eValue, pOutDataType, pOutScope);
}
S_API ESteamNetworkingConfigValue S_CALLTYPE SteamAPI_ISteamNetworkingUtils_IterateGenericEditableConfigValues(intptr_t instancePtr, ESteamNetworkingConfigValue eCurrent, bool bEnumerateDevVars)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->IterateGenericEditableConfigValues(eCurrent, bEnumerateDevVars);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->IterateGenericEditableConfigValues(eCurrent, bEnumerateDevVars);
}
S_API void S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SteamNetworkingIPAddr_ToString(intptr_t instancePtr, const SteamNetworkingIPAddr& addr, char* buf, uint32 cbBuf, bool bWithPort)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SteamNetworkingIPAddr_ToString(addr, buf, cbBuf, bWithPort);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SteamNetworkingIPAddr_ToString(addr, buf, cbBuf, bWithPort);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SteamNetworkingIPAddr_ParseString(intptr_t instancePtr, SteamNetworkingIPAddr* pAddr, const char* pszStr)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SteamNetworkingIPAddr_ParseString(pAddr, pszStr);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SteamNetworkingIPAddr_ParseString(pAddr, pszStr);
}
S_API ESteamNetworkingFakeIPType S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SteamNetworkingIPAddr_GetFakeIPType(intptr_t instancePtr, const SteamNetworkingIPAddr& addr)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SteamNetworkingIPAddr_GetFakeIPType(addr);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SteamNetworkingIPAddr_GetFakeIPType(addr);
}
S_API void S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SteamNetworkingIdentity_ToString(intptr_t instancePtr, const SteamNetworkingIdentity& identity, char* buf, uint32 cbBuf)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SteamNetworkingIdentity_ToString(identity, buf, cbBuf);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SteamNetworkingIdentity_ToString(identity, buf, cbBuf);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworkingUtils_SteamNetworkingIdentity_ParseString(intptr_t instancePtr, SteamNetworkingIdentity* pIdentity, const char* pszStr)
{
	if (g_bServerReady == true)
	{
		void* pInterfaceAddress = SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		if (instancePtr == (intptr_t)pInterfaceAddress || g_bClientReady == false)
		{
			ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
			return pSteamNetworkingUtils->SteamNetworkingIdentity_ParseString(pIdentity, pszStr);
		}
	}
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	ISteamNetworkingUtils* pSteamNetworkingUtils = (ISteamNetworkingUtils*)pInterfaceAddress;
	return pSteamNetworkingUtils->SteamNetworkingIdentity_ParseString(pIdentity, pszStr);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_SendP2PPacket(intptr_t instancePtr, class CSteamID steamIDRemote, const void * pubData, uint32 cubData, EP2PSend eP2PSendType, int nChannel)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->SendP2PPacket(steamIDRemote, pubData, cubData, eP2PSendType, nChannel);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->SendP2PPacket(steamIDRemote, pubData, cubData, eP2PSendType, nChannel);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_IsP2PPacketAvailable(intptr_t instancePtr, uint32 * pcubMsgSize, int nChannel)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->IsP2PPacketAvailable(pcubMsgSize, nChannel);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->IsP2PPacketAvailable(pcubMsgSize, nChannel);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_ReadP2PPacket(intptr_t instancePtr, void * pubDest, uint32 cubDest, uint32 * pcubMsgSize, class CSteamID * psteamIDRemote, int nChannel)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->ReadP2PPacket(pubDest, cubDest, pcubMsgSize, psteamIDRemote, nChannel);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->ReadP2PPacket(pubDest, cubDest, pcubMsgSize, psteamIDRemote, nChannel);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_AcceptP2PSessionWithUser(intptr_t instancePtr, class CSteamID steamIDRemote)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->AcceptP2PSessionWithUser(steamIDRemote);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->AcceptP2PSessionWithUser(steamIDRemote);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_CloseP2PSessionWithUser(intptr_t instancePtr, class CSteamID steamIDRemote)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->CloseP2PSessionWithUser(steamIDRemote);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->CloseP2PSessionWithUser(steamIDRemote);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_CloseP2PChannelWithUser(intptr_t instancePtr, class CSteamID steamIDRemote, int nChannel)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->CloseP2PChannelWithUser(steamIDRemote, nChannel);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->CloseP2PChannelWithUser(steamIDRemote, nChannel);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_GetP2PSessionState(intptr_t instancePtr, class CSteamID steamIDRemote, struct P2PSessionState_t * pConnectionState)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->GetP2PSessionState(steamIDRemote, pConnectionState);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->GetP2PSessionState(steamIDRemote, pConnectionState);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_AllowP2PPacketRelay(intptr_t instancePtr, bool bAllow)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->AllowP2PPacketRelay(bAllow);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->AllowP2PPacketRelay(bAllow);
}
S_API SNetListenSocket_t S_CALLTYPE SteamAPI_ISteamNetworking_CreateListenSocket(intptr_t instancePtr, int nVirtualP2PPort, struct SteamIPAddress_t nIP, uint16 nPort, bool bAllowUseOfPacketRelay)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->CreateListenSocket(nVirtualP2PPort, nIP, nPort, bAllowUseOfPacketRelay);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->CreateListenSocket(nVirtualP2PPort, nIP, nPort, bAllowUseOfPacketRelay);
}
S_API SNetSocket_t S_CALLTYPE SteamAPI_ISteamNetworking_CreateP2PConnectionSocket(intptr_t instancePtr, class CSteamID steamIDTarget, int nVirtualPort, int nTimeoutSec, bool bAllowUseOfPacketRelay)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->CreateP2PConnectionSocket(steamIDTarget, nVirtualPort, nTimeoutSec, bAllowUseOfPacketRelay);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->CreateP2PConnectionSocket(steamIDTarget, nVirtualPort, nTimeoutSec, bAllowUseOfPacketRelay);
}
S_API SNetSocket_t S_CALLTYPE SteamAPI_ISteamNetworking_CreateConnectionSocket(intptr_t instancePtr, struct SteamIPAddress_t nIP, uint16 nPort, int nTimeoutSec)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->CreateConnectionSocket(nIP, nPort, nTimeoutSec);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->CreateConnectionSocket(nIP, nPort, nTimeoutSec);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_DestroySocket(intptr_t instancePtr, SNetSocket_t hSocket, bool bNotifyRemoteEnd)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->DestroySocket(hSocket, bNotifyRemoteEnd);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->DestroySocket(hSocket, bNotifyRemoteEnd);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_DestroyListenSocket(intptr_t instancePtr, SNetListenSocket_t hSocket, bool bNotifyRemoteEnd)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->DestroyListenSocket(hSocket, bNotifyRemoteEnd);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->DestroyListenSocket(hSocket, bNotifyRemoteEnd);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_SendDataOnSocket(intptr_t instancePtr, SNetSocket_t hSocket, void * pubData, uint32 cubData, bool bReliable)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->SendDataOnSocket(hSocket, pubData, cubData, bReliable);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->SendDataOnSocket(hSocket, pubData, cubData, bReliable);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_IsDataAvailableOnSocket(intptr_t instancePtr, SNetSocket_t hSocket, uint32 * pcubMsgSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->IsDataAvailableOnSocket(hSocket, pcubMsgSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->IsDataAvailableOnSocket(hSocket, pcubMsgSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_RetrieveDataFromSocket(intptr_t instancePtr, SNetSocket_t hSocket, void * pubDest, uint32 cubDest, uint32 * pcubMsgSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->RetrieveDataFromSocket(hSocket, pubDest, cubDest, pcubMsgSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->RetrieveDataFromSocket(hSocket, pubDest, cubDest, pcubMsgSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_IsDataAvailable(intptr_t instancePtr, SNetListenSocket_t hListenSocket, uint32 * pcubMsgSize, SNetSocket_t * phSocket)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->IsDataAvailable(hListenSocket, pcubMsgSize, phSocket);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->IsDataAvailable(hListenSocket, pcubMsgSize, phSocket);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_RetrieveData(intptr_t instancePtr, SNetListenSocket_t hListenSocket, void * pubDest, uint32 cubDest, uint32 * pcubMsgSize, SNetSocket_t * phSocket)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->RetrieveData(hListenSocket, pubDest, cubDest, pcubMsgSize, phSocket);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->RetrieveData(hListenSocket, pubDest, cubDest, pcubMsgSize, phSocket);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_GetSocketInfo(intptr_t instancePtr, SNetSocket_t hSocket, class CSteamID * pSteamIDRemote, int * peSocketStatus, struct SteamIPAddress_t* punIPRemote, uint16 * punPortRemote)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->GetSocketInfo(hSocket, pSteamIDRemote, peSocketStatus, punIPRemote, punPortRemote);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->GetSocketInfo(hSocket, pSteamIDRemote, peSocketStatus, punIPRemote, punPortRemote);
}
S_API bool S_CALLTYPE SteamAPI_ISteamNetworking_GetListenSocketInfo(intptr_t instancePtr, SNetListenSocket_t hListenSocket, struct SteamIPAddress_t* pnIP, uint16 * pnPort)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->GetListenSocketInfo(hListenSocket, pnIP, pnPort);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->GetListenSocketInfo(hListenSocket, pnIP, pnPort);
}
S_API ESNetSocketConnectionType S_CALLTYPE SteamAPI_ISteamNetworking_GetSocketConnectionType(intptr_t instancePtr, SNetSocket_t hSocket)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->GetSocketConnectionType(hSocket);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->GetSocketConnectionType(hSocket);
}
S_API int S_CALLTYPE SteamAPI_ISteamNetworking_GetMaxPacketSize(intptr_t instancePtr, SNetSocket_t hSocket)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamGameServerNetworking() || g_bClientReady == false)
			return g_ServerCtx.SteamGameServerNetworking()->GetMaxPacketSize(hSocket);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamNetworking()->GetMaxPacketSize(hSocket);
}
S_API ScreenshotHandle S_CALLTYPE SteamAPI_ISteamScreenshots_WriteScreenshot(intptr_t instancePtr, void * pubRGB, uint32 cubRGB, int nWidth, int nHeight)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamScreenshots()->WriteScreenshot(pubRGB, cubRGB, nWidth, nHeight);
}
S_API ScreenshotHandle S_CALLTYPE SteamAPI_ISteamScreenshots_AddScreenshotToLibrary(intptr_t instancePtr, const char * pchFilename, const char * pchThumbnailFilename, int nWidth, int nHeight)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamScreenshots()->AddScreenshotToLibrary(pchFilename, pchThumbnailFilename, nWidth, nHeight);
}
S_API void S_CALLTYPE SteamAPI_ISteamScreenshots_TriggerScreenshot(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamScreenshots()->TriggerScreenshot();
}
S_API void S_CALLTYPE SteamAPI_ISteamScreenshots_HookScreenshots(intptr_t instancePtr, bool bHook)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamScreenshots()->HookScreenshots(bHook);
}
S_API bool S_CALLTYPE SteamAPI_ISteamScreenshots_SetLocation(intptr_t instancePtr, ScreenshotHandle hScreenshot, const char * pchLocation)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamScreenshots()->SetLocation(hScreenshot, pchLocation);
}
S_API bool S_CALLTYPE SteamAPI_ISteamScreenshots_TagUser(intptr_t instancePtr, ScreenshotHandle hScreenshot, class CSteamID steamID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamScreenshots()->TagUser(hScreenshot, steamID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamScreenshots_TagPublishedFile(intptr_t instancePtr, ScreenshotHandle hScreenshot, PublishedFileId_t unPublishedFileID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamScreenshots()->TagPublishedFile(hScreenshot, unPublishedFileID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamScreenshots_IsScreenshotsHooked(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamScreenshots()->IsScreenshotsHooked();
}
S_API ScreenshotHandle S_CALLTYPE SteamAPI_ISteamScreenshots_AddVRScreenshotToLibrary(intptr_t instancePtr, EVRScreenshotType eType, const char * pchFilename, const char * pchVRFilename)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamScreenshots()->AddVRScreenshotToLibrary(eType, pchFilename, pchVRFilename);
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusic_BIsEnabled(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMusic()->BIsEnabled();
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusic_BIsPlaying(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMusic()->BIsPlaying();
}
S_API AudioPlayback_Status S_CALLTYPE SteamAPI_ISteamMusic_GetPlaybackStatus(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMusic()->GetPlaybackStatus();
}
S_API void S_CALLTYPE SteamAPI_ISteamMusic_Play(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMusic()->Play();
}
S_API void S_CALLTYPE SteamAPI_ISteamMusic_Pause(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMusic()->Pause();
}
S_API void S_CALLTYPE SteamAPI_ISteamMusic_PlayPrevious(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMusic()->PlayPrevious();
}
S_API void S_CALLTYPE SteamAPI_ISteamMusic_PlayNext(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMusic()->PlayNext();
}
S_API void S_CALLTYPE SteamAPI_ISteamMusic_SetVolume(intptr_t instancePtr, float flVolume)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMusic()->SetVolume(flVolume);
}
S_API float S_CALLTYPE SteamAPI_ISteamMusic_GetVolume(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamMusic()->GetVolume();
}
S_API HTTPRequestHandle S_CALLTYPE SteamAPI_ISteamHTTP_CreateHTTPRequest(intptr_t instancePtr, EHTTPMethod eHTTPRequestMethod, const char * pchAbsoluteURL)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->CreateHTTPRequest(eHTTPRequestMethod, pchAbsoluteURL);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->CreateHTTPRequest(eHTTPRequestMethod, pchAbsoluteURL);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_SetHTTPRequestContextValue(intptr_t instancePtr, HTTPRequestHandle hRequest, uint64 ulContextValue)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->SetHTTPRequestContextValue(hRequest, ulContextValue);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->SetHTTPRequestContextValue(hRequest, ulContextValue);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_SetHTTPRequestNetworkActivityTimeout(intptr_t instancePtr, HTTPRequestHandle hRequest, uint32 unTimeoutSeconds)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->SetHTTPRequestNetworkActivityTimeout(hRequest, unTimeoutSeconds);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->SetHTTPRequestNetworkActivityTimeout(hRequest, unTimeoutSeconds);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_SetHTTPRequestHeaderValue(intptr_t instancePtr, HTTPRequestHandle hRequest, const char * pchHeaderName, const char * pchHeaderValue)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->SetHTTPRequestHeaderValue(hRequest, pchHeaderName, pchHeaderValue);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->SetHTTPRequestHeaderValue(hRequest, pchHeaderName, pchHeaderValue);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_SetHTTPRequestGetOrPostParameter(intptr_t instancePtr, HTTPRequestHandle hRequest, const char * pchParamName, const char * pchParamValue)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->SetHTTPRequestGetOrPostParameter(hRequest, pchParamName, pchParamValue);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->SetHTTPRequestGetOrPostParameter(hRequest, pchParamName, pchParamValue);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_SendHTTPRequest(intptr_t instancePtr, HTTPRequestHandle hRequest, SteamAPICall_t * pCallHandle)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->SendHTTPRequest(hRequest, pCallHandle);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->SendHTTPRequest(hRequest, pCallHandle);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_SendHTTPRequestAndStreamResponse(intptr_t instancePtr, HTTPRequestHandle hRequest, SteamAPICall_t * pCallHandle)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->SendHTTPRequestAndStreamResponse(hRequest, pCallHandle);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->SendHTTPRequestAndStreamResponse(hRequest, pCallHandle);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_DeferHTTPRequest(intptr_t instancePtr, HTTPRequestHandle hRequest)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->DeferHTTPRequest(hRequest);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->DeferHTTPRequest(hRequest);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_PrioritizeHTTPRequest(intptr_t instancePtr, HTTPRequestHandle hRequest)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->PrioritizeHTTPRequest(hRequest);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->PrioritizeHTTPRequest(hRequest);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_GetHTTPResponseHeaderSize(intptr_t instancePtr, HTTPRequestHandle hRequest, const char * pchHeaderName, uint32 * unResponseHeaderSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->GetHTTPResponseHeaderSize(hRequest, pchHeaderName, unResponseHeaderSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->GetHTTPResponseHeaderSize(hRequest, pchHeaderName, unResponseHeaderSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_GetHTTPResponseHeaderValue(intptr_t instancePtr, HTTPRequestHandle hRequest, const char * pchHeaderName, uint8 * pHeaderValueBuffer, uint32 unBufferSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->GetHTTPResponseHeaderValue(hRequest, pchHeaderName, pHeaderValueBuffer, unBufferSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->GetHTTPResponseHeaderValue(hRequest, pchHeaderName, pHeaderValueBuffer, unBufferSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_GetHTTPResponseBodySize(intptr_t instancePtr, HTTPRequestHandle hRequest, uint32 * unBodySize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->GetHTTPResponseBodySize(hRequest, unBodySize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->GetHTTPResponseBodySize(hRequest, unBodySize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_GetHTTPResponseBodyData(intptr_t instancePtr, HTTPRequestHandle hRequest, uint8 * pBodyDataBuffer, uint32 unBufferSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->GetHTTPResponseBodyData(hRequest, pBodyDataBuffer, unBufferSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->GetHTTPResponseBodyData(hRequest, pBodyDataBuffer, unBufferSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_GetHTTPStreamingResponseBodyData(intptr_t instancePtr, HTTPRequestHandle hRequest, uint32 cOffset, uint8 * pBodyDataBuffer, uint32 unBufferSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->GetHTTPStreamingResponseBodyData(hRequest, cOffset, pBodyDataBuffer, unBufferSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->GetHTTPStreamingResponseBodyData(hRequest, cOffset, pBodyDataBuffer, unBufferSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_ReleaseHTTPRequest(intptr_t instancePtr, HTTPRequestHandle hRequest)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->ReleaseHTTPRequest(hRequest);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->ReleaseHTTPRequest(hRequest);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_GetHTTPDownloadProgressPct(intptr_t instancePtr, HTTPRequestHandle hRequest, float * pflPercentOut)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->GetHTTPDownloadProgressPct(hRequest, pflPercentOut);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->GetHTTPDownloadProgressPct(hRequest, pflPercentOut);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_SetHTTPRequestRawPostBody(intptr_t instancePtr, HTTPRequestHandle hRequest, const char * pchContentType, uint8 * pubBody, uint32 unBodyLen)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->SetHTTPRequestRawPostBody(hRequest, pchContentType, pubBody, unBodyLen);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->SetHTTPRequestRawPostBody(hRequest, pchContentType, pubBody, unBodyLen);
}
S_API HTTPCookieContainerHandle S_CALLTYPE SteamAPI_ISteamHTTP_CreateCookieContainer(intptr_t instancePtr, bool bAllowResponsesToModify)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->CreateCookieContainer(bAllowResponsesToModify);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->CreateCookieContainer(bAllowResponsesToModify);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_ReleaseCookieContainer(intptr_t instancePtr, HTTPCookieContainerHandle hCookieContainer)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->ReleaseCookieContainer(hCookieContainer);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->ReleaseCookieContainer(hCookieContainer);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_SetCookie(intptr_t instancePtr, HTTPCookieContainerHandle hCookieContainer, const char * pchHost, const char * pchUrl, const char * pchCookie)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->SetCookie(hCookieContainer, pchHost, pchUrl, pchCookie);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->SetCookie(hCookieContainer, pchHost, pchUrl, pchCookie);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_SetHTTPRequestCookieContainer(intptr_t instancePtr, HTTPRequestHandle hRequest, HTTPCookieContainerHandle hCookieContainer)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->SetHTTPRequestCookieContainer(hRequest, hCookieContainer);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->SetHTTPRequestCookieContainer(hRequest, hCookieContainer);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_SetHTTPRequestUserAgentInfo(intptr_t instancePtr, HTTPRequestHandle hRequest, const char * pchUserAgentInfo)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->SetHTTPRequestUserAgentInfo(hRequest, pchUserAgentInfo);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->SetHTTPRequestUserAgentInfo(hRequest, pchUserAgentInfo);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_SetHTTPRequestRequiresVerifiedCertificate(intptr_t instancePtr, HTTPRequestHandle hRequest, bool bRequireVerifiedCertificate)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->SetHTTPRequestRequiresVerifiedCertificate(hRequest, bRequireVerifiedCertificate);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->SetHTTPRequestRequiresVerifiedCertificate(hRequest, bRequireVerifiedCertificate);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_SetHTTPRequestAbsoluteTimeoutMS(intptr_t instancePtr, HTTPRequestHandle hRequest, uint32 unMilliseconds)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->SetHTTPRequestAbsoluteTimeoutMS(hRequest, unMilliseconds);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->SetHTTPRequestAbsoluteTimeoutMS(hRequest, unMilliseconds);
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTTP_GetHTTPRequestWasTimedOut(intptr_t instancePtr, HTTPRequestHandle hRequest, bool * pbWasTimedOut)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamHTTP() || g_bClientReady == false)
			return g_ServerCtx.SteamHTTP()->GetHTTPRequestWasTimedOut(hRequest, pbWasTimedOut);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTTP()->GetHTTPRequestWasTimedOut(hRequest, pbWasTimedOut);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInput_Init(intptr_t instancePtr, bool bExplicitlyCallRunFrame)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->Init(bExplicitlyCallRunFrame);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInput_Shutdown(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->Shutdown();
}
S_API bool S_CALLTYPE SteamAPI_ISteamInput_SetInputActionManifestFilePath(intptr_t instancePtr, const char* pchInputActionManifestAbsolutePath)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->SetInputActionManifestFilePath(pchInputActionManifestAbsolutePath);
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_RunFrame(intptr_t instancePtr, bool bReservedValue)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->RunFrame(bReservedValue);
}
S_API bool SteamAPI_ISteamInput_BWaitForData(intptr_t instancePtr, bool bWaitForever, uint32 unTimeout)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->BWaitForData(bWaitForever, unTimeout);
}
S_API bool SteamAPI_ISteamInput_BNewDataAvailable(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->BNewDataAvailable();
}
S_API int S_CALLTYPE SteamAPI_ISteamInput_GetConnectedControllers(intptr_t instancePtr, InputHandle_t * handlesOut)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetConnectedControllers(handlesOut);
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_EnableDeviceCallbacks(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->EnableDeviceCallbacks();
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_EnableActionEventCallbacks(intptr_t instancePtr, SteamInputActionEventCallbackPointer pCallback)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->EnableActionEventCallbacks(pCallback);
}
S_API InputActionSetHandle_t S_CALLTYPE SteamAPI_ISteamInput_GetActionSetHandle(intptr_t instancePtr, const char * pszActionSetName)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetActionSetHandle(pszActionSetName);
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_ActivateActionSet(intptr_t instancePtr, InputHandle_t inputHandle, InputActionSetHandle_t actionSetHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->ActivateActionSet(inputHandle, actionSetHandle);
}
S_API InputActionSetHandle_t S_CALLTYPE SteamAPI_ISteamInput_GetCurrentActionSet(intptr_t instancePtr, InputHandle_t inputHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetCurrentActionSet(inputHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_ActivateActionSetLayer(intptr_t instancePtr, InputHandle_t inputHandle, InputActionSetHandle_t actionSetLayerHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->ActivateActionSetLayer(inputHandle, actionSetLayerHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_DeactivateActionSetLayer(intptr_t instancePtr, InputHandle_t inputHandle, InputActionSetHandle_t actionSetLayerHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->DeactivateActionSetLayer(inputHandle, actionSetLayerHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_DeactivateAllActionSetLayers(intptr_t instancePtr, InputHandle_t inputHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->DeactivateAllActionSetLayers(inputHandle);
}
S_API int S_CALLTYPE SteamAPI_ISteamInput_GetActiveActionSetLayers(intptr_t instancePtr, InputHandle_t inputHandle, InputActionSetHandle_t * handlesOut)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetActiveActionSetLayers(inputHandle, handlesOut);
}
S_API InputDigitalActionHandle_t S_CALLTYPE SteamAPI_ISteamInput_GetDigitalActionHandle(intptr_t instancePtr, const char * pszActionName)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetDigitalActionHandle(pszActionName);
}
S_API struct InputDigitalActionData_t S_CALLTYPE SteamAPI_ISteamInput_GetDigitalActionData(intptr_t instancePtr, InputHandle_t inputHandle, InputDigitalActionHandle_t digitalActionHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetDigitalActionData(inputHandle, digitalActionHandle);
}
S_API int S_CALLTYPE SteamAPI_ISteamInput_GetDigitalActionOrigins(intptr_t instancePtr, InputHandle_t inputHandle, InputActionSetHandle_t actionSetHandle, InputDigitalActionHandle_t digitalActionHandle, EInputActionOrigin * originsOut)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetDigitalActionOrigins(inputHandle, actionSetHandle, digitalActionHandle, originsOut);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamInput_GetStringForDigitalActionName(intptr_t instancePtr, InputDigitalActionHandle_t eActionHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetStringForDigitalActionName(eActionHandle);
}
S_API InputAnalogActionHandle_t S_CALLTYPE SteamAPI_ISteamInput_GetAnalogActionHandle(intptr_t instancePtr, const char * pszActionName)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetAnalogActionHandle(pszActionName);
}
S_API struct InputAnalogActionData_t S_CALLTYPE SteamAPI_ISteamInput_GetAnalogActionData(intptr_t instancePtr, InputHandle_t inputHandle, InputAnalogActionHandle_t analogActionHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetAnalogActionData(inputHandle, analogActionHandle);
}
S_API int S_CALLTYPE SteamAPI_ISteamInput_GetAnalogActionOrigins(intptr_t instancePtr, InputHandle_t inputHandle, InputActionSetHandle_t actionSetHandle, InputAnalogActionHandle_t analogActionHandle, EInputActionOrigin * originsOut)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetAnalogActionOrigins(inputHandle, actionSetHandle, analogActionHandle, originsOut);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamInput_GetGlyphPNGForActionOrigin(intptr_t instancePtr, EInputActionOrigin eOrigin, ESteamInputGlyphSize eSize, uint32 unFlags)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetGlyphPNGForActionOrigin(eOrigin, eSize, unFlags);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamInput_GetGlyphSVGForActionOrigin(intptr_t instancePtr, EInputActionOrigin eOrigin, uint32 unFlags)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetGlyphSVGForActionOrigin(eOrigin, unFlags);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamInput_GetGlyphForActionOrigin_Legacy(intptr_t instancePtr, EInputActionOrigin eOrigin)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetGlyphForActionOrigin_Legacy(eOrigin);
}
S_API const char * S_CALLTYPE SteamAPI_ISteamInput_GetGlyphForActionOrigin(intptr_t instancePtr, EInputActionOrigin eOrigin)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetGlyphForActionOrigin_Legacy(eOrigin);
}
S_API const char * S_CALLTYPE SteamAPI_ISteamInput_GetStringForActionOrigin(intptr_t instancePtr, EInputActionOrigin eOrigin)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetStringForActionOrigin(eOrigin);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamInput_GetStringForAnalogActionName(intptr_t instancePtr, InputAnalogActionHandle_t eActionHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetStringForAnalogActionName(eActionHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_StopAnalogActionMomentum(intptr_t instancePtr, InputHandle_t inputHandle, InputAnalogActionHandle_t eAction)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->StopAnalogActionMomentum(inputHandle, eAction);
}
S_API struct InputMotionData_t S_CALLTYPE SteamAPI_ISteamInput_GetMotionData(intptr_t instancePtr, InputHandle_t inputHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetMotionData(inputHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_TriggerVibration(intptr_t instancePtr, InputHandle_t inputHandle, unsigned short usLeftSpeed, unsigned short usRightSpeed)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->TriggerVibration(inputHandle, usLeftSpeed, usRightSpeed);
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_TriggerVibrationExtended(intptr_t instancePtr, InputHandle_t inputHandle, unsigned short usLeftSpeed, unsigned short usRightSpeed, unsigned short usLeftTriggerSpeed, unsigned short usRightTriggerSpeed)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->TriggerVibrationExtended(inputHandle, usLeftSpeed, usRightSpeed, usLeftTriggerSpeed, usRightTriggerSpeed);
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_TriggerSimpleHapticEvent(intptr_t instancePtr, InputHandle_t inputHandle, EControllerHapticLocation eHapticLocation, uint8 nIntensity, char nGainDB, uint8 nOtherIntensity, char nOtherGainDB)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->TriggerSimpleHapticEvent(inputHandle, eHapticLocation, nIntensity, nGainDB, nOtherIntensity, nOtherGainDB);
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_SetLEDColor(intptr_t instancePtr, InputHandle_t inputHandle, uint8 nColorR, uint8 nColorG, uint8 nColorB, unsigned int nFlags)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->SetLEDColor(inputHandle, nColorR, nColorG, nColorB, nFlags);
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_Legacy_TriggerHapticPulse(intptr_t instancePtr, InputHandle_t inputHandle, ESteamControllerPad eTargetPad, unsigned short usDurationMicroSec)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->Legacy_TriggerHapticPulse(inputHandle, eTargetPad, usDurationMicroSec);
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_TriggerHapticPulse(intptr_t instancePtr, InputHandle_t inputHandle, ESteamControllerPad eTargetPad, unsigned short usDurationMicroSec)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->Legacy_TriggerHapticPulse(inputHandle, eTargetPad, usDurationMicroSec);
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_Legacy_TriggerRepeatedHapticPulse(intptr_t instancePtr, InputHandle_t inputHandle, ESteamControllerPad eTargetPad, unsigned short usDurationMicroSec, unsigned short usOffMicroSec, unsigned short unRepeat, unsigned int nFlags)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->Legacy_TriggerRepeatedHapticPulse(inputHandle, eTargetPad, usDurationMicroSec, usOffMicroSec, unRepeat, nFlags);
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_TriggerRepeatedHapticPulse(intptr_t instancePtr, InputHandle_t inputHandle, ESteamControllerPad eTargetPad, unsigned short usDurationMicroSec, unsigned short usOffMicroSec, unsigned short unRepeat, unsigned int nFlags)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->Legacy_TriggerRepeatedHapticPulse(inputHandle, eTargetPad, usDurationMicroSec, usOffMicroSec, unRepeat, nFlags);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInput_ShowBindingPanel(intptr_t instancePtr, InputHandle_t inputHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->ShowBindingPanel(inputHandle);
}
S_API ESteamInputType S_CALLTYPE SteamAPI_ISteamInput_GetInputTypeForHandle(intptr_t instancePtr, InputHandle_t inputHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetInputTypeForHandle(inputHandle);
}
S_API InputHandle_t S_CALLTYPE SteamAPI_ISteamInput_GetControllerForGamepadIndex(intptr_t instancePtr, int nIndex)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetControllerForGamepadIndex(nIndex);
}
S_API int S_CALLTYPE SteamAPI_ISteamInput_GetGamepadIndexForController(intptr_t instancePtr, InputHandle_t ulinputHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetGamepadIndexForController(ulinputHandle);
}
S_API const char * S_CALLTYPE SteamAPI_ISteamInput_GetStringForXboxOrigin(intptr_t instancePtr, EXboxOrigin eOrigin)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetStringForXboxOrigin(eOrigin);
}
S_API const char * S_CALLTYPE SteamAPI_ISteamInput_GetGlyphForXboxOrigin(intptr_t instancePtr, EXboxOrigin eOrigin)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetGlyphForXboxOrigin(eOrigin);
}
S_API EInputActionOrigin S_CALLTYPE SteamAPI_ISteamInput_GetActionOriginFromXboxOrigin(intptr_t instancePtr, InputHandle_t inputHandle, EXboxOrigin eOrigin)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetActionOriginFromXboxOrigin(inputHandle, eOrigin);
}
S_API EInputActionOrigin S_CALLTYPE SteamAPI_ISteamInput_TranslateActionOrigin(intptr_t instancePtr, ESteamInputType eDestinationInputType, EInputActionOrigin eSourceOrigin)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->TranslateActionOrigin(eDestinationInputType, eSourceOrigin);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInput_GetDeviceBindingRevision(intptr_t instancePtr, InputHandle_t inputHandle, int* pMajor, int* pMinor)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetDeviceBindingRevision(inputHandle, pMajor, pMinor);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamInput_GetRemotePlaySessionID(intptr_t instancePtr, InputHandle_t inputHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetRemotePlaySessionID(inputHandle);
}
S_API uint16 S_CALLTYPE SteamAPI_ISteamInput_GetSessionInputConfigurationSettings(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->GetSessionInputConfigurationSettings();
}
S_API void S_CALLTYPE SteamAPI_ISteamInput_SetDualSenseTriggerEffect(intptr_t instancePtr, InputHandle_t inputHandle, const ScePadTriggerEffectParam* pParam)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInput()->SetDualSenseTriggerEffect(inputHandle, pParam);
}
S_API bool S_CALLTYPE SteamAPI_ISteamController_Init(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->Init();
}
S_API bool S_CALLTYPE SteamAPI_ISteamController_Shutdown(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->Shutdown();
}
S_API void S_CALLTYPE SteamAPI_ISteamController_RunFrame(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->RunFrame();
}
S_API int S_CALLTYPE SteamAPI_ISteamController_GetConnectedControllers(intptr_t instancePtr, ControllerHandle_t * handlesOut)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetConnectedControllers(handlesOut);
}
S_API ControllerActionSetHandle_t S_CALLTYPE SteamAPI_ISteamController_GetActionSetHandle(intptr_t instancePtr, const char * pszActionSetName)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetActionSetHandle(pszActionSetName);
}
S_API void S_CALLTYPE SteamAPI_ISteamController_ActivateActionSet(intptr_t instancePtr, ControllerHandle_t controllerHandle, ControllerActionSetHandle_t actionSetHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->ActivateActionSet(controllerHandle, actionSetHandle);
}
S_API ControllerActionSetHandle_t S_CALLTYPE SteamAPI_ISteamController_GetCurrentActionSet(intptr_t instancePtr, ControllerHandle_t controllerHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetCurrentActionSet(controllerHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamController_ActivateActionSetLayer(intptr_t instancePtr, ControllerHandle_t controllerHandle, ControllerActionSetHandle_t actionSetLayerHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->ActivateActionSetLayer(controllerHandle, actionSetLayerHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamController_DeactivateActionSetLayer(intptr_t instancePtr, ControllerHandle_t controllerHandle, ControllerActionSetHandle_t actionSetLayerHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->DeactivateActionSetLayer(controllerHandle, actionSetLayerHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamController_DeactivateAllActionSetLayers(intptr_t instancePtr, ControllerHandle_t controllerHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->DeactivateAllActionSetLayers(controllerHandle);
}
S_API int S_CALLTYPE SteamAPI_ISteamController_GetActiveActionSetLayers(intptr_t instancePtr, ControllerHandle_t controllerHandle, ControllerActionSetHandle_t * handlesOut)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetActiveActionSetLayers(controllerHandle, handlesOut);
}
S_API ControllerDigitalActionHandle_t S_CALLTYPE SteamAPI_ISteamController_GetDigitalActionHandle(intptr_t instancePtr, const char * pszActionName)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetDigitalActionHandle(pszActionName);
}
S_API struct InputDigitalActionData_t S_CALLTYPE SteamAPI_ISteamController_GetDigitalActionData(intptr_t instancePtr, ControllerHandle_t controllerHandle, ControllerDigitalActionHandle_t digitalActionHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetDigitalActionData(controllerHandle, digitalActionHandle);
}
S_API int S_CALLTYPE SteamAPI_ISteamController_GetDigitalActionOrigins(intptr_t instancePtr, ControllerHandle_t controllerHandle, ControllerActionSetHandle_t actionSetHandle, ControllerDigitalActionHandle_t digitalActionHandle, EControllerActionOrigin * originsOut)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetDigitalActionOrigins(controllerHandle, actionSetHandle, digitalActionHandle, originsOut);
}
S_API ControllerAnalogActionHandle_t S_CALLTYPE SteamAPI_ISteamController_GetAnalogActionHandle(intptr_t instancePtr, const char * pszActionName)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetAnalogActionHandle(pszActionName);
}
S_API struct InputAnalogActionData_t S_CALLTYPE SteamAPI_ISteamController_GetAnalogActionData(intptr_t instancePtr, ControllerHandle_t controllerHandle, ControllerAnalogActionHandle_t analogActionHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetAnalogActionData(controllerHandle, analogActionHandle);
}
S_API int S_CALLTYPE SteamAPI_ISteamController_GetAnalogActionOrigins(intptr_t instancePtr, ControllerHandle_t controllerHandle, ControllerActionSetHandle_t actionSetHandle, ControllerAnalogActionHandle_t analogActionHandle, EControllerActionOrigin * originsOut)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetAnalogActionOrigins(controllerHandle, actionSetHandle, analogActionHandle, originsOut);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamController_GetGlyphForActionOrigin(intptr_t instancePtr, EControllerActionOrigin eOrigin)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetGlyphForActionOrigin(eOrigin);
}
S_API const char* S_CALLTYPE SteamAPI_ISteamController_GetStringForActionOrigin(intptr_t instancePtr, EControllerActionOrigin eOrigin)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetStringForActionOrigin(eOrigin);
}
S_API void S_CALLTYPE SteamAPI_ISteamController_StopAnalogActionMomentum(intptr_t instancePtr, ControllerHandle_t controllerHandle, ControllerAnalogActionHandle_t eAction)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->StopAnalogActionMomentum(controllerHandle, eAction);
}
S_API struct InputMotionData_t S_CALLTYPE SteamAPI_ISteamController_GetMotionData(intptr_t instancePtr, ControllerHandle_t controllerHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetMotionData(controllerHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamController_TriggerHapticPulse(intptr_t instancePtr, ControllerHandle_t controllerHandle, ESteamControllerPad eTargetPad, unsigned short usDurationMicroSec)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->TriggerHapticPulse(controllerHandle, eTargetPad, usDurationMicroSec);
}
S_API void S_CALLTYPE SteamAPI_ISteamController_TriggerRepeatedHapticPulse(intptr_t instancePtr, ControllerHandle_t controllerHandle, ESteamControllerPad eTargetPad, unsigned short usDurationMicroSec, unsigned short usOffMicroSec, unsigned short unRepeat, unsigned int nFlags)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->TriggerRepeatedHapticPulse(controllerHandle, eTargetPad, usDurationMicroSec, usOffMicroSec, unRepeat, nFlags);
}
S_API void S_CALLTYPE SteamAPI_ISteamController_TriggerVibration(intptr_t instancePtr, ControllerHandle_t controllerHandle, unsigned short usLeftSpeed, unsigned short usRightSpeed)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->TriggerVibration(controllerHandle, usLeftSpeed, usRightSpeed);
}
S_API void S_CALLTYPE SteamAPI_ISteamController_SetLEDColor(intptr_t instancePtr, ControllerHandle_t controllerHandle, uint8 nColorR, uint8 nColorG, uint8 nColorB, unsigned int nFlags)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->SetLEDColor(controllerHandle, nColorR, nColorG, nColorB, nFlags);
}
S_API bool S_CALLTYPE SteamAPI_ISteamController_ShowBindingPanel(intptr_t instancePtr, ControllerHandle_t controllerHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->ShowBindingPanel(controllerHandle);
}
S_API ESteamInputType S_CALLTYPE SteamAPI_ISteamController_GetInputTypeForHandle(intptr_t instancePtr, ControllerHandle_t controllerHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetInputTypeForHandle(controllerHandle);
}
S_API ControllerHandle_t S_CALLTYPE SteamAPI_ISteamController_GetControllerForGamepadIndex(intptr_t instancePtr, int nIndex)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetControllerForGamepadIndex(nIndex);
}
S_API int S_CALLTYPE SteamAPI_ISteamController_GetGamepadIndexForController(intptr_t instancePtr, ControllerHandle_t ulControllerHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetGamepadIndexForController(ulControllerHandle);
}
S_API const char * S_CALLTYPE SteamAPI_ISteamController_GetStringForXboxOrigin(intptr_t instancePtr, EXboxOrigin eOrigin)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetStringForXboxOrigin(eOrigin);
}
S_API const char * S_CALLTYPE SteamAPI_ISteamController_GetGlyphForXboxOrigin(intptr_t instancePtr, EXboxOrigin eOrigin)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetGlyphForXboxOrigin(eOrigin);
}
S_API EControllerActionOrigin S_CALLTYPE SteamAPI_ISteamController_GetActionOriginFromXboxOrigin(intptr_t instancePtr, ControllerHandle_t controllerHandle, EXboxOrigin eOrigin)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetActionOriginFromXboxOrigin(controllerHandle, eOrigin);
}
S_API EControllerActionOrigin S_CALLTYPE SteamAPI_ISteamController_TranslateActionOrigin(intptr_t instancePtr, ESteamInputType eDestinationInputType, EControllerActionOrigin eSourceOrigin)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->TranslateActionOrigin(eDestinationInputType, eSourceOrigin);
}
S_API bool S_CALLTYPE SteamAPI_ISteamController_GetControllerBindingRevision(intptr_t instancePtr, ControllerHandle_t controllerHandle, int* pMajor, int* pMinor)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamController()->GetControllerBindingRevision(controllerHandle, pMajor, pMinor);
}
S_API UGCQueryHandle_t S_CALLTYPE SteamAPI_ISteamUGC_CreateQueryUserUGCRequest(intptr_t instancePtr, AccountID_t unAccountID, EUserUGCList eListType, EUGCMatchingUGCType eMatchingUGCType, EUserUGCListSortOrder eSortOrder, AppId_t nCreatorAppID, AppId_t nConsumerAppID, uint32 unPage)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->CreateQueryUserUGCRequest(unAccountID, eListType, eMatchingUGCType, eSortOrder, nCreatorAppID, nConsumerAppID, unPage);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->CreateQueryUserUGCRequest(unAccountID, eListType, eMatchingUGCType, eSortOrder, nCreatorAppID, nConsumerAppID, unPage);
}
S_API UGCQueryHandle_t S_CALLTYPE SteamAPI_ISteamUGC_CreateQueryAllUGCRequestPage(intptr_t instancePtr, EUGCQuery eQueryType, EUGCMatchingUGCType eMatchingeMatchingUGCTypeFileType, AppId_t nCreatorAppID, AppId_t nConsumerAppID, uint32 unPage)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->CreateQueryAllUGCRequest(eQueryType, eMatchingeMatchingUGCTypeFileType, nCreatorAppID, nConsumerAppID, unPage);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->CreateQueryAllUGCRequest(eQueryType, eMatchingeMatchingUGCTypeFileType, nCreatorAppID, nConsumerAppID, unPage);
}
S_API UGCQueryHandle_t SteamAPI_ISteamUGC_CreateQueryAllUGCRequestCursor(intptr_t instancePtr, EUGCQuery eQueryType, EUGCMatchingUGCType eMatchingeMatchingUGCTypeFileType, AppId_t nCreatorAppID, AppId_t nConsumerAppID, const char * pchCursor)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->CreateQueryAllUGCRequest(eQueryType, eMatchingeMatchingUGCTypeFileType, nCreatorAppID, nConsumerAppID, pchCursor);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->CreateQueryAllUGCRequest(eQueryType, eMatchingeMatchingUGCTypeFileType, nCreatorAppID, nConsumerAppID, pchCursor);
}
S_API UGCQueryHandle_t S_CALLTYPE SteamAPI_ISteamUGC_CreateQueryUGCDetailsRequest(intptr_t instancePtr, PublishedFileId_t * pvecPublishedFileID, uint32 unNumPublishedFileIDs)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->CreateQueryUGCDetailsRequest(pvecPublishedFileID, unNumPublishedFileIDs);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->CreateQueryUGCDetailsRequest(pvecPublishedFileID, unNumPublishedFileIDs);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_SendQueryUGCRequest(intptr_t instancePtr, UGCQueryHandle_t handle)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SendQueryUGCRequest(handle);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SendQueryUGCRequest(handle);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_GetQueryUGCResult(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 index, struct SteamUGCDetails_t * pDetails)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetQueryUGCResult(handle, index, pDetails);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetQueryUGCResult(handle, index, pDetails);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUGC_GetQueryUGCNumTags(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 index)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetQueryUGCNumTags(handle, index);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetQueryUGCNumTags(handle, index);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_GetQueryUGCTag(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 index, uint32 indexTag, char* pchValue, uint32 cchValueSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetQueryUGCTag(handle, index, indexTag, pchValue, cchValueSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetQueryUGCTag(handle, index, indexTag, pchValue, cchValueSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_GetQueryUGCTagDisplayName(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 index, uint32 indexTag, char* pchValue, uint32 cchValueSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetQueryUGCTagDisplayName(handle, index, indexTag, pchValue, cchValueSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetQueryUGCTagDisplayName(handle, index, indexTag, pchValue, cchValueSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_GetQueryUGCPreviewURL(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 index, char * pchURL, uint32 cchURLSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetQueryUGCPreviewURL(handle, index, pchURL, cchURLSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetQueryUGCPreviewURL(handle, index, pchURL, cchURLSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_GetQueryUGCMetadata(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 index, char * pchMetadata, uint32 cchMetadatasize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetQueryUGCMetadata(handle, index, pchMetadata, cchMetadatasize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetQueryUGCMetadata(handle, index, pchMetadata, cchMetadatasize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_GetQueryUGCChildren(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 index, PublishedFileId_t * pvecPublishedFileID, uint32 cMaxEntries)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetQueryUGCChildren(handle, index, pvecPublishedFileID, cMaxEntries);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetQueryUGCChildren(handle, index, pvecPublishedFileID, cMaxEntries);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_GetQueryUGCStatistic(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 index, EItemStatistic eStatType, uint64 * pStatValue)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetQueryUGCStatistic(handle, index, eStatType, pStatValue);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetQueryUGCStatistic(handle, index, eStatType, pStatValue);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUGC_GetQueryUGCNumAdditionalPreviews(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 index)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetQueryUGCNumAdditionalPreviews(handle, index);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetQueryUGCNumAdditionalPreviews(handle, index);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_GetQueryUGCAdditionalPreview(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 index, uint32 previewIndex, char * pchURLOrVideoID, uint32 cchURLSize, char * pchOriginalFileName, uint32 cchOriginalFileNameSize, EItemPreviewType * pPreviewType)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetQueryUGCAdditionalPreview(handle, index, previewIndex, pchURLOrVideoID, cchURLSize, pchOriginalFileName, cchOriginalFileNameSize, pPreviewType);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetQueryUGCAdditionalPreview(handle, index, previewIndex, pchURLOrVideoID, cchURLSize, pchOriginalFileName, cchOriginalFileNameSize, pPreviewType);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUGC_GetQueryUGCNumKeyValueTags(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 index)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetQueryUGCNumKeyValueTags(handle, index);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetQueryUGCNumKeyValueTags(handle, index);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_GetQueryUGCKeyValueTag(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 index, uint32 keyValueTagIndex, char * pchKey, uint32 cchKeySize, char * pchValue, uint32 cchValueSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetQueryUGCKeyValueTag(handle, index, keyValueTagIndex, pchKey, cchKeySize, pchValue, cchValueSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetQueryUGCKeyValueTag(handle, index, keyValueTagIndex, pchKey, cchKeySize, pchValue, cchValueSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_GetQueryFirstUGCKeyValueTag(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 index, const char* pchKey, char* pchValue, uint32 cchValueSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetQueryUGCKeyValueTag(handle, index, pchKey, pchValue, cchValueSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetQueryUGCKeyValueTag(handle, index, pchKey, pchValue, cchValueSize);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUGC_GetNumSupportedGameVersions(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 index)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetNumSupportedGameVersions(handle, index);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetNumSupportedGameVersions(handle, index);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_GetSupportedGameVersionData(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 index, uint32 versionIndex, char* pchGameBranchMin, char* pchGameBranchMax, uint32 cchGameBranchSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetSupportedGameVersionData(handle, index, versionIndex, pchGameBranchMin, pchGameBranchMax, cchGameBranchSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetSupportedGameVersionData(handle, index, versionIndex, pchGameBranchMin, pchGameBranchMax, cchGameBranchSize);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUGC_GetQueryUGCContentDescriptors(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 index, EUGCContentDescriptorID* pvecDescriptors, uint32 cMaxEntries)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetQueryUGCContentDescriptors(handle, index, pvecDescriptors, cMaxEntries);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetQueryUGCContentDescriptors(handle, index, pvecDescriptors, cMaxEntries);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_ReleaseQueryUGCRequest(intptr_t instancePtr, UGCQueryHandle_t handle)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->ReleaseQueryUGCRequest(handle);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->ReleaseQueryUGCRequest(handle);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_AddRequiredTag(intptr_t instancePtr, UGCQueryHandle_t handle, const char * pTagName)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->AddRequiredTag(handle, pTagName);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->AddRequiredTag(handle, pTagName);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_AddRequiredTagGroup(intptr_t instancePtr, UGCQueryHandle_t handle, const struct SteamParamStringArray_t* pTagGroups)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->AddRequiredTagGroup(handle, pTagGroups);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->AddRequiredTagGroup(handle, pTagGroups);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_AddExcludedTag(intptr_t instancePtr, UGCQueryHandle_t handle, const char * pTagName)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->AddExcludedTag(handle, pTagName);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->AddExcludedTag(handle, pTagName);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetReturnOnlyIDs(intptr_t instancePtr, UGCQueryHandle_t handle, bool bReturnOnlyIDs)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetReturnOnlyIDs(handle, bReturnOnlyIDs);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetReturnOnlyIDs(handle, bReturnOnlyIDs);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetReturnKeyValueTags(intptr_t instancePtr, UGCQueryHandle_t handle, bool bReturnKeyValueTags)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetReturnKeyValueTags(handle, bReturnKeyValueTags);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetReturnKeyValueTags(handle, bReturnKeyValueTags);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetReturnLongDescription(intptr_t instancePtr, UGCQueryHandle_t handle, bool bReturnLongDescription)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetReturnLongDescription(handle, bReturnLongDescription);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetReturnLongDescription(handle, bReturnLongDescription);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetReturnMetadata(intptr_t instancePtr, UGCQueryHandle_t handle, bool bReturnMetadata)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetReturnMetadata(handle, bReturnMetadata);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetReturnMetadata(handle, bReturnMetadata);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetReturnChildren(intptr_t instancePtr, UGCQueryHandle_t handle, bool bReturnChildren)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetReturnChildren(handle, bReturnChildren);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetReturnChildren(handle, bReturnChildren);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetReturnAdditionalPreviews(intptr_t instancePtr, UGCQueryHandle_t handle, bool bReturnAdditionalPreviews)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetReturnAdditionalPreviews(handle, bReturnAdditionalPreviews);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetReturnAdditionalPreviews(handle, bReturnAdditionalPreviews);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetReturnTotalOnly(intptr_t instancePtr, UGCQueryHandle_t handle, bool bReturnTotalOnly)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetReturnTotalOnly(handle, bReturnTotalOnly);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetReturnTotalOnly(handle, bReturnTotalOnly);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetReturnPlaytimeStats(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 unDays)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetReturnPlaytimeStats(handle, unDays);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetReturnPlaytimeStats(handle, unDays);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetLanguage(intptr_t instancePtr, UGCQueryHandle_t handle, const char * pchLanguage)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetLanguage(handle, pchLanguage);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetLanguage(handle, pchLanguage);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetAllowCachedResponse(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 unMaxAgeSeconds)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetAllowCachedResponse(handle, unMaxAgeSeconds);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetAllowCachedResponse(handle, unMaxAgeSeconds);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetAdminQuery(intptr_t instancePtr, UGCUpdateHandle_t handle, bool bAdminQuery)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetAdminQuery(handle, bAdminQuery);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetAdminQuery(handle, bAdminQuery);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetCloudFileNameFilter(intptr_t instancePtr, UGCQueryHandle_t handle, const char * pMatchCloudFileName)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetCloudFileNameFilter(handle, pMatchCloudFileName);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetCloudFileNameFilter(handle, pMatchCloudFileName);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetMatchAnyTag(intptr_t instancePtr, UGCQueryHandle_t handle, bool bMatchAnyTag)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetMatchAnyTag(handle, bMatchAnyTag);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetMatchAnyTag(handle, bMatchAnyTag);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetSearchText(intptr_t instancePtr, UGCQueryHandle_t handle, const char * pSearchText)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetSearchText(handle, pSearchText);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetSearchText(handle, pSearchText);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetRankedByTrendDays(intptr_t instancePtr, UGCQueryHandle_t handle, uint32 unDays)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetRankedByTrendDays(handle, unDays);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetRankedByTrendDays(handle, unDays);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetTimeCreatedDateRange(intptr_t instancePtr, UGCQueryHandle_t handle, RTime32 rtStart, RTime32 rtEnd)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetTimeCreatedDateRange(handle, rtStart, rtEnd);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetTimeCreatedDateRange(handle, rtStart, rtEnd);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetTimeUpdatedDateRange(intptr_t instancePtr, UGCQueryHandle_t handle, RTime32 rtStart, RTime32 rtEnd)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetTimeUpdatedDateRange(handle, rtStart, rtEnd);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetTimeUpdatedDateRange(handle, rtStart, rtEnd);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_AddRequiredKeyValueTag(intptr_t instancePtr, UGCQueryHandle_t handle, const char * pKey, const char * pValue)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->AddRequiredKeyValueTag(handle, pKey, pValue);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->AddRequiredKeyValueTag(handle, pKey, pValue);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_RequestUGCDetails(intptr_t instancePtr, PublishedFileId_t nPublishedFileID, uint32 unMaxAgeSeconds)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->RequestUGCDetails(nPublishedFileID, unMaxAgeSeconds);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->RequestUGCDetails(nPublishedFileID, unMaxAgeSeconds);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_CreateItem(intptr_t instancePtr, AppId_t nConsumerAppId, EWorkshopFileType eFileType)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->CreateItem(nConsumerAppId, eFileType);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->CreateItem(nConsumerAppId, eFileType);
}
S_API UGCUpdateHandle_t S_CALLTYPE SteamAPI_ISteamUGC_StartItemUpdate(intptr_t instancePtr, AppId_t nConsumerAppId, PublishedFileId_t nPublishedFileID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->StartItemUpdate(nConsumerAppId, nPublishedFileID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->StartItemUpdate(nConsumerAppId, nPublishedFileID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetItemTitle(intptr_t instancePtr, UGCUpdateHandle_t handle, const char * pchTitle)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetItemTitle(handle, pchTitle);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetItemTitle(handle, pchTitle);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetItemDescription(intptr_t instancePtr, UGCUpdateHandle_t handle, const char * pchDescription)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetItemDescription(handle, pchDescription);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetItemDescription(handle, pchDescription);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetItemUpdateLanguage(intptr_t instancePtr, UGCUpdateHandle_t handle, const char * pchLanguage)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetItemUpdateLanguage(handle, pchLanguage);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetItemUpdateLanguage(handle, pchLanguage);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetItemMetadata(intptr_t instancePtr, UGCUpdateHandle_t handle, const char * pchMetaData)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetItemMetadata(handle, pchMetaData);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetItemMetadata(handle, pchMetaData);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetItemVisibility(intptr_t instancePtr, UGCUpdateHandle_t handle, ERemoteStoragePublishedFileVisibility eVisibility)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetItemVisibility(handle, eVisibility);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetItemVisibility(handle, eVisibility);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetItemTags(intptr_t instancePtr, UGCUpdateHandle_t updateHandle, const struct SteamParamStringArray_t * pTags, bool bAllowAdminTags = false)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetItemTags(updateHandle, pTags, bAllowAdminTags);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetItemTags(updateHandle, pTags, bAllowAdminTags);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetItemContent(intptr_t instancePtr, UGCUpdateHandle_t handle, const char * pszContentFolder)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetItemContent(handle, pszContentFolder);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetItemContent(handle, pszContentFolder);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetItemPreview(intptr_t instancePtr, UGCUpdateHandle_t handle, const char * pszPreviewFile)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetItemPreview(handle, pszPreviewFile);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetItemPreview(handle, pszPreviewFile);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetAllowLegacyUpload(intptr_t instancePtr, UGCUpdateHandle_t handle, bool bAllowLegacyUpload)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetAllowLegacyUpload(handle, bAllowLegacyUpload);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetAllowLegacyUpload(handle, bAllowLegacyUpload);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_RemoveAllItemKeyValueTags(intptr_t instancePtr, UGCUpdateHandle_t handle)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->RemoveAllItemKeyValueTags(handle);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->RemoveAllItemKeyValueTags(handle);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_RemoveItemKeyValueTags(intptr_t instancePtr, UGCUpdateHandle_t handle, const char * pchKey)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->RemoveItemKeyValueTags(handle, pchKey);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->RemoveItemKeyValueTags(handle, pchKey);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_AddItemKeyValueTag(intptr_t instancePtr, UGCUpdateHandle_t handle, const char * pchKey, const char * pchValue)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->AddItemKeyValueTag(handle, pchKey, pchValue);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->AddItemKeyValueTag(handle, pchKey, pchValue);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_AddItemPreviewFile(intptr_t instancePtr, UGCUpdateHandle_t handle, const char * pszPreviewFile, EItemPreviewType type)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->AddItemPreviewFile(handle, pszPreviewFile, type);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->AddItemPreviewFile(handle, pszPreviewFile, type);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_AddItemPreviewVideo(intptr_t instancePtr, UGCUpdateHandle_t handle, const char * pszVideoID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->AddItemPreviewVideo(handle, pszVideoID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->AddItemPreviewVideo(handle, pszVideoID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_UpdateItemPreviewFile(intptr_t instancePtr, UGCUpdateHandle_t handle, uint32 index, const char * pszPreviewFile)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->UpdateItemPreviewFile(handle, index, pszPreviewFile);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->UpdateItemPreviewFile(handle, index, pszPreviewFile);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_UpdateItemPreviewVideo(intptr_t instancePtr, UGCUpdateHandle_t handle, uint32 index, const char * pszVideoID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->UpdateItemPreviewVideo(handle, index, pszVideoID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->UpdateItemPreviewVideo(handle, index, pszVideoID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_RemoveItemPreview(intptr_t instancePtr, UGCUpdateHandle_t handle, uint32 index)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->RemoveItemPreview(handle, index);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->RemoveItemPreview(handle, index);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_AddContentDescriptor(intptr_t instancePtr, UGCUpdateHandle_t handle, EUGCContentDescriptorID descid)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->AddContentDescriptor(handle, descid);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->AddContentDescriptor(handle, descid);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_RemoveContentDescriptor(intptr_t instancePtr, UGCUpdateHandle_t handle, EUGCContentDescriptorID descid)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->RemoveContentDescriptor(handle, descid);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->RemoveContentDescriptor(handle, descid);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetRequiredGameVersions(intptr_t instancePtr, UGCUpdateHandle_t handle, const char* pszGameBranchMin, const char* pszGameBranchMax)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetRequiredGameVersions(handle, pszGameBranchMin, pszGameBranchMax);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetRequiredGameVersions(handle, pszGameBranchMin, pszGameBranchMax);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_SubmitItemUpdate(intptr_t instancePtr, UGCUpdateHandle_t handle, const char * pchChangeNote)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SubmitItemUpdate(handle, pchChangeNote);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SubmitItemUpdate(handle, pchChangeNote);
}
S_API EItemUpdateStatus S_CALLTYPE SteamAPI_ISteamUGC_GetItemUpdateProgress(intptr_t instancePtr, UGCUpdateHandle_t handle, uint64 * punBytesProcessed, uint64 * punBytesTotal)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetItemUpdateProgress(handle, punBytesProcessed, punBytesTotal);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetItemUpdateProgress(handle, punBytesProcessed, punBytesTotal);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_SetUserItemVote(intptr_t instancePtr, PublishedFileId_t nPublishedFileID, bool bVoteUp)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetUserItemVote(nPublishedFileID, bVoteUp);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetUserItemVote(nPublishedFileID, bVoteUp);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_GetUserItemVote(intptr_t instancePtr, PublishedFileId_t nPublishedFileID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetUserItemVote(nPublishedFileID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetUserItemVote(nPublishedFileID);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_AddItemToFavorites(intptr_t instancePtr, AppId_t nAppId, PublishedFileId_t nPublishedFileID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->AddItemToFavorites(nAppId, nPublishedFileID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->AddItemToFavorites(nAppId, nPublishedFileID);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_RemoveItemFromFavorites(intptr_t instancePtr, AppId_t nAppId, PublishedFileId_t nPublishedFileID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->RemoveItemFromFavorites(nAppId, nPublishedFileID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->RemoveItemFromFavorites(nAppId, nPublishedFileID);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_SubscribeItem(intptr_t instancePtr, PublishedFileId_t nPublishedFileID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SubscribeItem(nPublishedFileID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SubscribeItem(nPublishedFileID);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_UnsubscribeItem(intptr_t instancePtr, PublishedFileId_t nPublishedFileID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->UnsubscribeItem(nPublishedFileID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->UnsubscribeItem(nPublishedFileID);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUGC_GetNumSubscribedItems(intptr_t instancePtr, bool bIncludeLocallyDisabled)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetNumSubscribedItems(bIncludeLocallyDisabled);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetNumSubscribedItems(bIncludeLocallyDisabled);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUGC_GetSubscribedItems(intptr_t instancePtr, PublishedFileId_t * pvecPublishedFileID, uint32 cMaxEntries, bool bIncludeLocallyDisabled)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetSubscribedItems(pvecPublishedFileID, cMaxEntries, bIncludeLocallyDisabled);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetSubscribedItems(pvecPublishedFileID, cMaxEntries, bIncludeLocallyDisabled);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUGC_GetItemState(intptr_t instancePtr, PublishedFileId_t nPublishedFileID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetItemState(nPublishedFileID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetItemState(nPublishedFileID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_GetItemInstallInfo(intptr_t instancePtr, PublishedFileId_t nPublishedFileID, uint64 * punSizeOnDisk, char * pchFolder, uint32 cchFolderSize, uint32 * punTimeStamp)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetItemInstallInfo(nPublishedFileID, punSizeOnDisk, pchFolder, cchFolderSize, punTimeStamp);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetItemInstallInfo(nPublishedFileID, punSizeOnDisk, pchFolder, cchFolderSize, punTimeStamp);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_GetItemDownloadInfo(intptr_t instancePtr, PublishedFileId_t nPublishedFileID, uint64 * punBytesDownloaded, uint64 * punBytesTotal)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetItemDownloadInfo(nPublishedFileID, punBytesDownloaded, punBytesTotal);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetItemDownloadInfo(nPublishedFileID, punBytesDownloaded, punBytesTotal);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_DownloadItem(intptr_t instancePtr, PublishedFileId_t nPublishedFileID, bool bHighPriority)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->DownloadItem(nPublishedFileID, bHighPriority);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->DownloadItem(nPublishedFileID, bHighPriority);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_BInitWorkshopForGameServer(intptr_t instancePtr, DepotId_t unWorkshopDepotID, const char * pszFolder)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->BInitWorkshopForGameServer(unWorkshopDepotID, pszFolder);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->BInitWorkshopForGameServer(unWorkshopDepotID, pszFolder);
}
S_API void S_CALLTYPE SteamAPI_ISteamUGC_SuspendDownloads(intptr_t instancePtr, bool bSuspend)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SuspendDownloads(bSuspend);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SuspendDownloads(bSuspend);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_StartPlaytimeTracking(intptr_t instancePtr, PublishedFileId_t * pvecPublishedFileID, uint32 unNumPublishedFileIDs)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->StartPlaytimeTracking(pvecPublishedFileID, unNumPublishedFileIDs);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->StartPlaytimeTracking(pvecPublishedFileID, unNumPublishedFileIDs);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_StopPlaytimeTracking(intptr_t instancePtr, PublishedFileId_t * pvecPublishedFileID, uint32 unNumPublishedFileIDs)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->StopPlaytimeTracking(pvecPublishedFileID, unNumPublishedFileIDs);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->StopPlaytimeTracking(pvecPublishedFileID, unNumPublishedFileIDs);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_StopPlaytimeTrackingForAllItems(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->StopPlaytimeTrackingForAllItems();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->StopPlaytimeTrackingForAllItems();
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_AddDependency(intptr_t instancePtr, PublishedFileId_t nParentPublishedFileID, PublishedFileId_t nChildPublishedFileID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->AddDependency(nParentPublishedFileID, nChildPublishedFileID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->AddDependency(nParentPublishedFileID, nChildPublishedFileID);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_RemoveDependency(intptr_t instancePtr, PublishedFileId_t nParentPublishedFileID, PublishedFileId_t nChildPublishedFileID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->RemoveDependency(nParentPublishedFileID, nChildPublishedFileID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->RemoveDependency(nParentPublishedFileID, nChildPublishedFileID);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_AddAppDependency(intptr_t instancePtr, PublishedFileId_t nPublishedFileID, AppId_t nAppID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->AddAppDependency(nPublishedFileID, nAppID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->AddAppDependency(nPublishedFileID, nAppID);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_RemoveAppDependency(intptr_t instancePtr, PublishedFileId_t nPublishedFileID, AppId_t nAppID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->RemoveAppDependency(nPublishedFileID, nAppID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->RemoveAppDependency(nPublishedFileID, nAppID);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_GetAppDependencies(intptr_t instancePtr, PublishedFileId_t nPublishedFileID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetAppDependencies(nPublishedFileID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetAppDependencies(nPublishedFileID);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_DeleteItem(intptr_t instancePtr, PublishedFileId_t nPublishedFileID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->DeleteItem(nPublishedFileID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->DeleteItem(nPublishedFileID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_ShowWorkshopEULA(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->ShowWorkshopEULA();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->ShowWorkshopEULA();
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamUGC_GetWorkshopEULAStatus(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetWorkshopEULAStatus();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetWorkshopEULAStatus();
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamUGC_GetUserContentDescriptorPreferences(intptr_t instancePtr, EUGCContentDescriptorID* pvecDescriptors, uint32 cMaxEntries)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->GetUserContentDescriptorPreferences(pvecDescriptors, cMaxEntries);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->GetUserContentDescriptorPreferences(pvecDescriptors, cMaxEntries);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetItemsDisabledLocally(intptr_t instancePtr, PublishedFileId_t* pvecPublishedFileIDs, uint32 unNumPublishedFileIDs, bool bDisabledLocally)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetItemsDisabledLocally(pvecPublishedFileIDs, unNumPublishedFileIDs, bDisabledLocally);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetItemsDisabledLocally(pvecPublishedFileIDs, unNumPublishedFileIDs, bDisabledLocally);
}
S_API bool S_CALLTYPE SteamAPI_ISteamUGC_SetSubscriptionsLoadOrder(intptr_t instancePtr, PublishedFileId_t* pvecPublishedFileIDs, uint32 unNumPublishedFileIDs)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamUGC() || g_bClientReady == false)
			return g_ServerCtx.SteamUGC()->SetSubscriptionsLoadOrder(pvecPublishedFileIDs, unNumPublishedFileIDs);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamUGC()->SetSubscriptionsLoadOrder(pvecPublishedFileIDs, unNumPublishedFileIDs);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_DestructISteamHTMLSurface(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->~ISteamHTMLSurface();
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTMLSurface_Init(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->Init();
}
S_API bool S_CALLTYPE SteamAPI_ISteamHTMLSurface_Shutdown(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->Shutdown();
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamHTMLSurface_CreateBrowser(intptr_t instancePtr, const char * pchUserAgent, const char * pchUserCSS)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->CreateBrowser(pchUserAgent, pchUserCSS);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_RemoveBrowser(intptr_t instancePtr, HHTMLBrowser unBrowserHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->RemoveBrowser(unBrowserHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_LoadURL(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, const char * pchURL, const char * pchPostData)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->LoadURL(unBrowserHandle, pchURL, pchPostData);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_SetSize(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, uint32 unWidth, uint32 unHeight)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->SetSize(unBrowserHandle, unWidth, unHeight);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_StopLoad(intptr_t instancePtr, HHTMLBrowser unBrowserHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->StopLoad(unBrowserHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_Reload(intptr_t instancePtr, HHTMLBrowser unBrowserHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->Reload(unBrowserHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_GoBack(intptr_t instancePtr, HHTMLBrowser unBrowserHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->GoBack(unBrowserHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_GoForward(intptr_t instancePtr, HHTMLBrowser unBrowserHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->GoForward(unBrowserHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_AddHeader(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, const char * pchKey, const char * pchValue)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->AddHeader(unBrowserHandle, pchKey, pchValue);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_ExecuteJavascript(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, const char * pchScript)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->ExecuteJavascript(unBrowserHandle, pchScript);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_MouseUp(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, ISteamHTMLSurface::EHTMLMouseButton eMouseButton)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->MouseUp(unBrowserHandle, eMouseButton);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_MouseDown(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, ISteamHTMLSurface::EHTMLMouseButton eMouseButton)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->MouseDown(unBrowserHandle, eMouseButton);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_MouseDoubleClick(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, ISteamHTMLSurface::EHTMLMouseButton eMouseButton)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->MouseDoubleClick(unBrowserHandle, eMouseButton);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_MouseMove(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, int x, int y)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->MouseMove(unBrowserHandle, x, y);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_MouseWheel(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, int32 nDelta)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->MouseWheel(unBrowserHandle, nDelta);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_KeyDown(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, uint32 nNativeKeyCode, ISteamHTMLSurface::EHTMLKeyModifiers eHTMLKeyModifiers, bool bIsSystemKey)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->KeyDown(unBrowserHandle, nNativeKeyCode, eHTMLKeyModifiers, bIsSystemKey);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_KeyUp(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, uint32 nNativeKeyCode, ISteamHTMLSurface::EHTMLKeyModifiers eHTMLKeyModifiers)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->KeyUp(unBrowserHandle, nNativeKeyCode, eHTMLKeyModifiers);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_KeyChar(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, uint32 cUnicodeChar, ISteamHTMLSurface::EHTMLKeyModifiers eHTMLKeyModifiers)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->KeyChar(unBrowserHandle, cUnicodeChar, eHTMLKeyModifiers);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_SetHorizontalScroll(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, uint32 nAbsolutePixelScroll)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->SetHorizontalScroll(unBrowserHandle, nAbsolutePixelScroll);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_SetVerticalScroll(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, uint32 nAbsolutePixelScroll)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->SetVerticalScroll(unBrowserHandle, nAbsolutePixelScroll);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_SetKeyFocus(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, bool bHasKeyFocus)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->SetKeyFocus(unBrowserHandle, bHasKeyFocus);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_ViewSource(intptr_t instancePtr, HHTMLBrowser unBrowserHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->ViewSource(unBrowserHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_CopyToClipboard(intptr_t instancePtr, HHTMLBrowser unBrowserHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->CopyToClipboard(unBrowserHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_PasteFromClipboard(intptr_t instancePtr, HHTMLBrowser unBrowserHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->PasteFromClipboard(unBrowserHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_Find(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, const char * pchSearchStr, bool bCurrentlyInFind, bool bReverse)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->Find(unBrowserHandle, pchSearchStr, bCurrentlyInFind, bReverse);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_StopFind(intptr_t instancePtr, HHTMLBrowser unBrowserHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->StopFind(unBrowserHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_GetLinkAtPosition(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, int x, int y)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->GetLinkAtPosition(unBrowserHandle, x, y);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_SetCookie(intptr_t instancePtr, const char * pchHostname, const char * pchKey, const char * pchValue, const char * pchPath, RTime32 nExpires, bool bSecure, bool bHTTPOnly)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->SetCookie(pchHostname, pchKey, pchValue, pchPath, nExpires, bSecure, bHTTPOnly);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_SetPageScaleFactor(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, float flZoom, int nPointX, int nPointY)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->SetPageScaleFactor(unBrowserHandle, flZoom, nPointX, nPointY);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_SetBackgroundMode(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, bool bBackgroundMode)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->SetBackgroundMode(unBrowserHandle, bBackgroundMode);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_SetDPIScalingFactor(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, float flDPIScaling)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->SetDPIScalingFactor(unBrowserHandle, flDPIScaling);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_OpenDeveloperTools(intptr_t instancePtr, HHTMLBrowser unBrowserHandle)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->OpenDeveloperTools(unBrowserHandle);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_AllowStartRequest(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, bool bAllowed)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->AllowStartRequest(unBrowserHandle, bAllowed);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_JSDialogResponse(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, bool bResult)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->JSDialogResponse(unBrowserHandle, bResult);
}
S_API void S_CALLTYPE SteamAPI_ISteamHTMLSurface_FileLoadDialogResponse(intptr_t instancePtr, HHTMLBrowser unBrowserHandle, const char** pchSelectedFiles)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamHTMLSurface()->FileLoadDialogResponse(unBrowserHandle, pchSelectedFiles);
}
S_API EResult S_CALLTYPE SteamAPI_ISteamInventory_GetResultStatus(intptr_t instancePtr, SteamInventoryResult_t resultHandle)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->GetResultStatus(resultHandle);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->GetResultStatus(resultHandle);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_GetResultItems(intptr_t instancePtr, SteamInventoryResult_t resultHandle, struct SteamItemDetails_t * pOutItemsArray, uint32 * punOutItemsArraySize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->GetResultItems(resultHandle, pOutItemsArray, punOutItemsArraySize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->GetResultItems(resultHandle, pOutItemsArray, punOutItemsArraySize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_GetResultItemProperty(intptr_t instancePtr, SteamInventoryResult_t resultHandle, uint32 unItemIndex, const char * pchPropertyName, char * pchValueBuffer, uint32 * punValueBufferSizeOut)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->GetResultItemProperty(resultHandle, unItemIndex, pchPropertyName, pchValueBuffer, punValueBufferSizeOut);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->GetResultItemProperty(resultHandle, unItemIndex, pchPropertyName, pchValueBuffer, punValueBufferSizeOut);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamInventory_GetResultTimestamp(intptr_t instancePtr, SteamInventoryResult_t resultHandle)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->GetResultTimestamp(resultHandle);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->GetResultTimestamp(resultHandle);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_CheckResultSteamID(intptr_t instancePtr, SteamInventoryResult_t resultHandle, class CSteamID steamIDExpected)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->CheckResultSteamID(resultHandle, steamIDExpected);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->CheckResultSteamID(resultHandle, steamIDExpected);
}
S_API void S_CALLTYPE SteamAPI_ISteamInventory_DestroyResult(intptr_t instancePtr, SteamInventoryResult_t resultHandle)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->DestroyResult(resultHandle);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->DestroyResult(resultHandle);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_GetAllItems(intptr_t instancePtr, SteamInventoryResult_t * pResultHandle)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->GetAllItems(pResultHandle);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->GetAllItems(pResultHandle);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_GetItemsByID(intptr_t instancePtr, SteamInventoryResult_t * pResultHandle, const SteamItemInstanceID_t * pInstanceIDs, uint32 unCountInstanceIDs)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->GetItemsByID(pResultHandle, pInstanceIDs, unCountInstanceIDs);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->GetItemsByID(pResultHandle, pInstanceIDs, unCountInstanceIDs);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_SerializeResult(intptr_t instancePtr, SteamInventoryResult_t resultHandle, void * pOutBuffer, uint32 * punOutBufferSize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->SerializeResult(resultHandle, pOutBuffer, punOutBufferSize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->SerializeResult(resultHandle, pOutBuffer, punOutBufferSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_DeserializeResult(intptr_t instancePtr, SteamInventoryResult_t * pOutResultHandle, const void * pBuffer, uint32 unBufferSize, bool bRESERVED_MUST_BE_FALSE)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->DeserializeResult(pOutResultHandle, pBuffer, unBufferSize, bRESERVED_MUST_BE_FALSE);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->DeserializeResult(pOutResultHandle, pBuffer, unBufferSize, bRESERVED_MUST_BE_FALSE);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_GenerateItems(intptr_t instancePtr, SteamInventoryResult_t * pResultHandle, const SteamItemDef_t * pArrayItemDefs, const uint32 * punArrayQuantity, uint32 unArrayLength)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->GenerateItems(pResultHandle, pArrayItemDefs, punArrayQuantity, unArrayLength);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->GenerateItems(pResultHandle, pArrayItemDefs, punArrayQuantity, unArrayLength);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_GrantPromoItems(intptr_t instancePtr, SteamInventoryResult_t * pResultHandle)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->GrantPromoItems(pResultHandle);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->GrantPromoItems(pResultHandle);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_AddPromoItem(intptr_t instancePtr, SteamInventoryResult_t * pResultHandle, SteamItemDef_t itemDef)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->AddPromoItem(pResultHandle, itemDef);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->AddPromoItem(pResultHandle, itemDef);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_AddPromoItems(intptr_t instancePtr, SteamInventoryResult_t * pResultHandle, const SteamItemDef_t * pArrayItemDefs, uint32 unArrayLength)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->AddPromoItems(pResultHandle, pArrayItemDefs, unArrayLength);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->AddPromoItems(pResultHandle, pArrayItemDefs, unArrayLength);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_ConsumeItem(intptr_t instancePtr, SteamInventoryResult_t * pResultHandle, SteamItemInstanceID_t itemConsume, uint32 unQuantity)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->ConsumeItem(pResultHandle, itemConsume, unQuantity);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->ConsumeItem(pResultHandle, itemConsume, unQuantity);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_ExchangeItems(intptr_t instancePtr, SteamInventoryResult_t * pResultHandle, const SteamItemDef_t * pArrayGenerate, const uint32 * punArrayGenerateQuantity, uint32 unArrayGenerateLength, const SteamItemInstanceID_t * pArrayDestroy, const uint32 * punArrayDestroyQuantity, uint32 unArrayDestroyLength)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->ExchangeItems(pResultHandle, pArrayGenerate, punArrayGenerateQuantity, unArrayGenerateLength, pArrayDestroy, punArrayDestroyQuantity, unArrayDestroyLength);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->ExchangeItems(pResultHandle, pArrayGenerate, punArrayGenerateQuantity, unArrayGenerateLength, pArrayDestroy, punArrayDestroyQuantity, unArrayDestroyLength);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_TransferItemQuantity(intptr_t instancePtr, SteamInventoryResult_t * pResultHandle, SteamItemInstanceID_t itemIdSource, uint32 unQuantity, SteamItemInstanceID_t itemIdDest)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->TransferItemQuantity(pResultHandle, itemIdSource, unQuantity, itemIdDest);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->TransferItemQuantity(pResultHandle, itemIdSource, unQuantity, itemIdDest);
}
S_API void S_CALLTYPE SteamAPI_ISteamInventory_SendItemDropHeartbeat(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->SendItemDropHeartbeat();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->SendItemDropHeartbeat();
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_TriggerItemDrop(intptr_t instancePtr, SteamInventoryResult_t * pResultHandle, SteamItemDef_t dropListDefinition)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->TriggerItemDrop(pResultHandle, dropListDefinition);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->TriggerItemDrop(pResultHandle, dropListDefinition);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_TradeItems(intptr_t instancePtr, SteamInventoryResult_t * pResultHandle, class CSteamID steamIDTradePartner, const SteamItemInstanceID_t * pArrayGive, const uint32 * pArrayGiveQuantity, uint32 nArrayGiveLength, const SteamItemInstanceID_t * pArrayGet, const uint32 * pArrayGetQuantity, uint32 nArrayGetLength)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->TradeItems(pResultHandle, steamIDTradePartner, pArrayGive, pArrayGiveQuantity, nArrayGiveLength, pArrayGet, pArrayGetQuantity, nArrayGetLength);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->TradeItems(pResultHandle, steamIDTradePartner, pArrayGive, pArrayGiveQuantity, nArrayGiveLength, pArrayGet, pArrayGetQuantity, nArrayGetLength);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_LoadItemDefinitions(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->LoadItemDefinitions();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->LoadItemDefinitions();
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_GetItemDefinitionIDs(intptr_t instancePtr, SteamItemDef_t * pItemDefIDs, uint32 * punItemDefIDsArraySize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->GetItemDefinitionIDs(pItemDefIDs, punItemDefIDsArraySize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->GetItemDefinitionIDs(pItemDefIDs, punItemDefIDsArraySize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_GetItemDefinitionProperty(intptr_t instancePtr, SteamItemDef_t iDefinition, const char * pchPropertyName, char * pchValueBuffer, uint32 * punValueBufferSizeOut)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->GetItemDefinitionProperty(iDefinition, pchPropertyName, pchValueBuffer, punValueBufferSizeOut);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->GetItemDefinitionProperty(iDefinition, pchPropertyName, pchValueBuffer, punValueBufferSizeOut);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamInventory_RequestEligiblePromoItemDefinitionsIDs(intptr_t instancePtr, class CSteamID steamID)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->RequestEligiblePromoItemDefinitionsIDs(steamID);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->RequestEligiblePromoItemDefinitionsIDs(steamID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_GetEligiblePromoItemDefinitionIDs(intptr_t instancePtr, class CSteamID steamID, SteamItemDef_t * pItemDefIDs, uint32 * punItemDefIDsArraySize)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->GetEligiblePromoItemDefinitionIDs(steamID, pItemDefIDs, punItemDefIDsArraySize);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->GetEligiblePromoItemDefinitionIDs(steamID, pItemDefIDs, punItemDefIDsArraySize);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamInventory_StartPurchase(intptr_t instancePtr, const SteamItemDef_t * pArrayItemDefs, const uint32 * punArrayQuantity, uint32 unArrayLength)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->StartPurchase(pArrayItemDefs, punArrayQuantity, unArrayLength);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->StartPurchase(pArrayItemDefs, punArrayQuantity, unArrayLength);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamInventory_RequestPrices(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->RequestPrices();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->RequestPrices();
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamInventory_GetNumItemsWithPrices(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->GetNumItemsWithPrices();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->GetNumItemsWithPrices();
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_GetItemsWithPrices(intptr_t instancePtr, SteamItemDef_t *pArrayItemDefs, uint64 *pCurrentPrices, uint64 *pBasePrices, uint32 unArrayLength)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->GetItemsWithPrices(pArrayItemDefs, pCurrentPrices, pBasePrices, unArrayLength);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->GetItemsWithPrices(pArrayItemDefs, pCurrentPrices, pBasePrices, unArrayLength);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_GetItemPrice(intptr_t instancePtr, SteamItemDef_t iDefinition, uint64 *pCurrentPrice, uint64 *pBasePrice)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->GetItemPrice(iDefinition, pCurrentPrice, pBasePrice);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->GetItemPrice(iDefinition, pCurrentPrice, pBasePrice);
}
S_API SteamInventoryUpdateHandle_t S_CALLTYPE SteamAPI_ISteamInventory_StartUpdateProperties(intptr_t instancePtr)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->StartUpdateProperties();
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->StartUpdateProperties();
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_RemoveProperty(intptr_t instancePtr, SteamInventoryUpdateHandle_t handle, SteamItemInstanceID_t nItemID, const char * pchPropertyName)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->RemoveProperty(handle, nItemID, pchPropertyName);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->RemoveProperty(handle, nItemID, pchPropertyName);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_SetPropertyString(intptr_t instancePtr, SteamInventoryUpdateHandle_t handle, SteamItemInstanceID_t nItemID, const char * pchPropertyName, const char * pchPropertyValue)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->SetProperty(handle, nItemID, pchPropertyName, pchPropertyValue);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->SetProperty(handle, nItemID, pchPropertyName, pchPropertyValue);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_SetPropertyBool(intptr_t instancePtr, SteamInventoryUpdateHandle_t handle, SteamItemInstanceID_t nItemID, const char * pchPropertyName, bool bValue)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->SetProperty(handle, nItemID, pchPropertyName, bValue);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->SetProperty(handle, nItemID, pchPropertyName, bValue);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_SetPropertyInt64(intptr_t instancePtr, SteamInventoryUpdateHandle_t handle, SteamItemInstanceID_t nItemID, const char * pchPropertyName, int64 nValue)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->SetProperty(handle, nItemID, pchPropertyName, nValue);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->SetProperty(handle, nItemID, pchPropertyName, nValue);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_SetPropertyFloat(intptr_t instancePtr, SteamInventoryUpdateHandle_t handle, SteamItemInstanceID_t nItemID, const char * pchPropertyName, float flValue)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->SetProperty(handle, nItemID, pchPropertyName, flValue);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->SetProperty(handle, nItemID, pchPropertyName, flValue);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_SubmitUpdateProperties(intptr_t instancePtr, SteamInventoryUpdateHandle_t handle, SteamInventoryResult_t * pResultHandle)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->SubmitUpdateProperties(handle, pResultHandle);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->SubmitUpdateProperties(handle, pResultHandle);
}
S_API bool S_CALLTYPE SteamAPI_ISteamInventory_InspectItem(intptr_t instancePtr, SteamInventoryResult_t* pResultHandle, const char* pchItemToken)
{
	if (g_bServerReady == true)
	{
		if (instancePtr == (intptr_t)g_ServerCtx.SteamInventory() || g_bClientReady == false)
			return g_ServerCtx.SteamInventory()->InspectItem(pResultHandle, pchItemToken);
	}
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamInventory()->InspectItem(pResultHandle, pchItemToken);
}
S_API void S_CALLTYPE SteamAPI_ISteamTimeline_SetTimelineTooltip(intptr_t instancePtr, const char* pchDescription, float flTimeDelta)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->SetTimelineTooltip(pchDescription, flTimeDelta);
}
S_API void S_CALLTYPE SteamAPI_ISteamTimeline_ClearTimelineTooltip(intptr_t instancePtr, float flTimeDelta)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->ClearTimelineTooltip(flTimeDelta);
}
S_API void S_CALLTYPE SteamAPI_ISteamTimeline_SetTimelineGameMode(intptr_t instancePtr, ETimelineGameMode eMode)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->SetTimelineGameMode(eMode);
}
S_API TimelineEventHandle_t S_CALLTYPE SteamAPI_ISteamTimeline_AddInstantaneousTimelineEvent(intptr_t instancePtr, const char* pchTitle, const char* pchDescription, const char* pchIcon, uint32 unIconPriority, float flStartOffsetSeconds, ETimelineEventClipPriority ePossibleClip)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->AddInstantaneousTimelineEvent(pchTitle, pchDescription, pchIcon, unIconPriority, flStartOffsetSeconds, ePossibleClip);
}
S_API TimelineEventHandle_t S_CALLTYPE SteamAPI_ISteamTimeline_AddRangeTimelineEvent(intptr_t instancePtr, const char* pchTitle, const char* pchDescription, const char* pchIcon, uint32 unIconPriority, float flStartOffsetSeconds, float flDuration, ETimelineEventClipPriority ePossibleClip)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->AddRangeTimelineEvent(pchTitle, pchDescription, pchIcon, unIconPriority, flStartOffsetSeconds, flDuration, ePossibleClip);
}
S_API TimelineEventHandle_t S_CALLTYPE SteamAPI_ISteamTimeline_StartRangeTimelineEvent(intptr_t instancePtr, const char* pchTitle, const char* pchDescription, const char* pchIcon, uint32 unPriority, float flStartOffsetSeconds, ETimelineEventClipPriority ePossibleClip)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->StartRangeTimelineEvent(pchTitle, pchDescription, pchIcon, unPriority, flStartOffsetSeconds, ePossibleClip);
}
S_API void S_CALLTYPE SteamAPI_ISteamTimeline_UpdateRangeTimelineEvent(intptr_t instancePtr, TimelineEventHandle_t ulEvent, const char* pchTitle, const char* pchDescription, const char* pchIcon, uint32 unPriority, ETimelineEventClipPriority ePossibleClip)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->UpdateRangeTimelineEvent(ulEvent, pchTitle, pchDescription, pchIcon, unPriority, ePossibleClip);
}
S_API void S_CALLTYPE SteamAPI_ISteamTimeline_EndRangeTimelineEvent(intptr_t instancePtr, TimelineEventHandle_t ulEvent, float flEndOffsetSeconds)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->EndRangeTimelineEvent(ulEvent, flEndOffsetSeconds);
}
S_API void S_CALLTYPE SteamAPI_ISteamTimeline_RemoveTimelineEvent(intptr_t instancePtr, TimelineEventHandle_t ulEvent)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->RemoveTimelineEvent(ulEvent);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamTimeline_DoesEventRecordingExist(intptr_t instancePtr, TimelineEventHandle_t ulEvent)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->DoesEventRecordingExist(ulEvent);
}
S_API void S_CALLTYPE SteamAPI_ISteamTimeline_StartGamePhase(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->StartGamePhase();
}
S_API void S_CALLTYPE SteamAPI_ISteamTimeline_EndGamePhase(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->EndGamePhase();
}
S_API void S_CALLTYPE SteamAPI_ISteamTimeline_SetGamePhaseID(intptr_t instancePtr, const char* pchPhaseID)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->SetGamePhaseID(pchPhaseID);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamTimeline_DoesGamePhaseRecordingExist(intptr_t instancePtr, const char* pchPhaseID)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->DoesGamePhaseRecordingExist(pchPhaseID);
}
S_API void S_CALLTYPE SteamAPI_ISteamTimeline_AddGamePhaseTag(intptr_t instancePtr, const char* pchTagName, const char* pchTagIcon, const char* pchTagGroup, uint32 unPriority)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->AddGamePhaseTag(pchTagName, pchTagIcon, pchTagGroup, unPriority);
}
S_API void S_CALLTYPE SteamAPI_ISteamTimeline_SetGamePhaseAttribute(intptr_t instancePtr, const char* pchAttributeGroup, const char* pchAttributeValue, uint32 unPriority)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->SetGamePhaseAttribute(pchAttributeGroup, pchAttributeValue, unPriority);
}
S_API void S_CALLTYPE SteamAPI_ISteamTimeline_OpenOverlayToGamePhase(intptr_t instancePtr, const char* pchPhaseID)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->OpenOverlayToGamePhase(pchPhaseID);
}
S_API void S_CALLTYPE SteamAPI_ISteamTimeline_OpenOverlayToTimelineEvent(intptr_t instancePtr, const TimelineEventHandle_t ulEvent)
{
	if (g_bClientReady == false)
		__debugbreak();
	void* pInterfaceAddress = SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
	ISteamTimeline* pSteamTimeline = (ISteamTimeline*)pInterfaceAddress;
	return pSteamTimeline->OpenOverlayToTimelineEvent(ulEvent);
}
S_API void S_CALLTYPE SteamAPI_ISteamVideo_GetVideoURL(intptr_t instancePtr, AppId_t unVideoAppID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamVideo()->GetVideoURL(unVideoAppID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamVideo_IsBroadcasting(intptr_t instancePtr, int * pnNumViewers)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamVideo()->IsBroadcasting(pnNumViewers);
}
S_API void S_CALLTYPE SteamAPI_ISteamVideo_GetOPFSettings(intptr_t instancePtr, AppId_t unVideoAppID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamVideo()->GetOPFSettings(unVideoAppID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamVideo_GetOPFStringForApp(intptr_t instancePtr, AppId_t unVideoAppID, char * pchBuffer, int32 * pnBufferSize)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamVideo()->GetOPFStringForApp(unVideoAppID, pchBuffer, pnBufferSize);
}
S_API bool S_CALLTYPE SteamAPI_ISteamParentalSettings_BIsParentalLockEnabled(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParentalSettings()->BIsParentalLockEnabled();
}
S_API bool S_CALLTYPE SteamAPI_ISteamParentalSettings_BIsParentalLockLocked(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParentalSettings()->BIsParentalLockLocked();
}
S_API bool S_CALLTYPE SteamAPI_ISteamParentalSettings_BIsAppBlocked(intptr_t instancePtr, AppId_t nAppID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParentalSettings()->BIsAppBlocked(nAppID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamParentalSettings_BIsAppInBlockList(intptr_t instancePtr, AppId_t nAppID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParentalSettings()->BIsAppInBlockList(nAppID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamParentalSettings_BIsFeatureBlocked(intptr_t instancePtr, EParentalFeature eFeature)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParentalSettings()->BIsFeatureBlocked(eFeature);
}
S_API bool S_CALLTYPE SteamAPI_ISteamParentalSettings_BIsFeatureInBlockList(intptr_t instancePtr, EParentalFeature eFeature)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamParentalSettings()->BIsFeatureInBlockList(eFeature);
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamRemotePlay_GetSessionCount(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemotePlay()->GetSessionCount();
}
S_API RemotePlaySessionID_t S_CALLTYPE SteamAPI_ISteamRemotePlay_GetSessionID(intptr_t instancePtr, int iSessionIndex)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemotePlay()->GetSessionID(iSessionIndex);
}
S_API uint64 S_CALLTYPE SteamAPI_ISteamRemotePlay_GetSessionSteamID(intptr_t instancePtr, RemotePlaySessionID_t unSessionID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemotePlay()->GetSessionSteamID(unSessionID).ConvertToUint64();
}
S_API const char* S_CALLTYPE SteamAPI_ISteamRemotePlay_GetSessionClientName(intptr_t instancePtr, RemotePlaySessionID_t unSessionID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemotePlay()->GetSessionClientName(unSessionID);
}
S_API ESteamDeviceFormFactor S_CALLTYPE SteamAPI_ISteamRemotePlay_GetSessionClientFormFactor(intptr_t instancePtr, RemotePlaySessionID_t unSessionID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemotePlay()->GetSessionClientFormFactor(unSessionID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemotePlay_BGetSessionClientResolution(intptr_t instancePtr, RemotePlaySessionID_t unSessionID, int* pnResolutionX, int* pnResolutionY)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemotePlay()->BGetSessionClientResolution(unSessionID, pnResolutionX, pnResolutionY);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemotePlay_ShowRemotePlayTogetherUI(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemotePlay()->ShowRemotePlayTogetherUI();
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemotePlay_BSendRemotePlayTogetherInvite(intptr_t instancePtr, class CSteamID steamIDFriend)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemotePlay()->BSendRemotePlayTogetherInvite(steamIDFriend);
}
S_API bool S_CALLTYPE SteamAPI_ISteamRemotePlay_BEnableRemotePlayTogetherDirectInput(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemotePlay()->BEnableRemotePlayTogetherDirectInput();
}
S_API void S_CALLTYPE SteamAPI_ISteamRemotePlay_DisableRemotePlayTogetherDirectInput(intptr_t instancePtr)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemotePlay()->DisableRemotePlayTogetherDirectInput();
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamRemotePlay_GetInput(intptr_t instancePtr, RemotePlayInput_t* pInput, uint32 unMaxEvents)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemotePlay()->GetInput(pInput, unMaxEvents);
}
S_API void S_CALLTYPE SteamAPI_ISteamRemotePlay_SetMouseVisibility(intptr_t instancePtr, RemotePlaySessionID_t unSessionID, bool bVisible)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemotePlay()->SetMouseVisibility(unSessionID, bVisible);
}
S_API void S_CALLTYPE SteamAPI_ISteamRemotePlay_SetMousePosition(intptr_t instancePtr, RemotePlaySessionID_t unSessionID, float flNormalizedX, float flNormalizedY)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemotePlay()->SetMousePosition(unSessionID, flNormalizedX, flNormalizedY);
}
S_API RemotePlayCursorID_t S_CALLTYPE SteamAPI_ISteamRemotePlay_CreateMouseCursor(intptr_t instancePtr, int nWidth, int nHeight, int nHotX, int nHotY, const void* pBGRA, int nPitch)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemotePlay()->CreateMouseCursor(nWidth, nHeight, nHotX, nHotY, pBGRA, nPitch);
}
S_API void S_CALLTYPE SteamAPI_ISteamRemotePlay_SetMouseCursor(intptr_t instancePtr, RemotePlaySessionID_t unSessionID, RemotePlayCursorID_t unCursorID)
{
	if (g_bClientReady == false)
		__debugbreak();
	return g_ClientCtx.SteamRemotePlay()->SetMouseCursor(unSessionID, unCursorID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServer_InitGameServer(intptr_t instancePtr, uint32 unIP, uint16 usGamePort, uint16 usQueryPort, uint32 unFlags, AppId_t nGameAppId, const char * pchVersionString)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->InitGameServer(unIP, usGamePort, usQueryPort, unFlags, nGameAppId, pchVersionString);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetProduct(intptr_t instancePtr, const char * pszProduct)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetProduct(pszProduct);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetGameDescription(intptr_t instancePtr, const char * pszGameDescription)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetGameDescription(pszGameDescription);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetModDir(intptr_t instancePtr, const char * pszModDir)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetModDir(pszModDir);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetDedicatedServer(intptr_t instancePtr, bool bDedicated)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetDedicatedServer(bDedicated);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_LogOn(intptr_t instancePtr, const char * pszToken)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->LogOn(pszToken);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_LogOnAnonymous(intptr_t instancePtr)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->LogOnAnonymous();
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_LogOff(intptr_t instancePtr)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->LogOff();
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServer_BLoggedOn(intptr_t instancePtr)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->BLoggedOn();
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServer_BSecure(intptr_t instancePtr)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->BSecure();
}
S_API uint64 S_CALLTYPE SteamAPI_ISteamGameServer_GetSteamID(intptr_t instancePtr)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->GetSteamID().ConvertToUint64();
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServer_WasRestartRequested(intptr_t instancePtr)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->WasRestartRequested();
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetMaxPlayerCount(intptr_t instancePtr, int cPlayersMax)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetMaxPlayerCount(cPlayersMax);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetBotPlayerCount(intptr_t instancePtr, int cBotplayers)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetBotPlayerCount(cBotplayers);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetServerName(intptr_t instancePtr, const char * pszServerName)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetServerName(pszServerName);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetMapName(intptr_t instancePtr, const char * pszMapName)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetMapName(pszMapName);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetPasswordProtected(intptr_t instancePtr, bool bPasswordProtected)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetPasswordProtected(bPasswordProtected);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetSpectatorPort(intptr_t instancePtr, uint16 unSpectatorPort)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetSpectatorPort(unSpectatorPort);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetSpectatorServerName(intptr_t instancePtr, const char * pszSpectatorServerName)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetSpectatorServerName(pszSpectatorServerName);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_ClearAllKeyValues(intptr_t instancePtr)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->ClearAllKeyValues();
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetKeyValue(intptr_t instancePtr, const char * pKey, const char * pValue)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetKeyValue(pKey, pValue);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetGameTags(intptr_t instancePtr, const char * pchGameTags)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetGameTags(pchGameTags);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetGameData(intptr_t instancePtr, const char * pchGameData)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetGameData(pchGameData);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetRegion(intptr_t instancePtr, const char * pszRegion)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetRegion(pszRegion);
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServer_SendUserConnectAndAuthenticate(intptr_t instancePtr, uint32 unIPClient, const void* pvAuthBlob, uint32 cubAuthBlobSize, class CSteamID* pSteamIDUser)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SendUserConnectAndAuthenticate_DEPRECATED(unIPClient, pvAuthBlob, cubAuthBlobSize, pSteamIDUser);
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServer_SendUserConnectAndAuthenticate_DEPRECATED(intptr_t instancePtr, uint32 unIPClient, const void * pvAuthBlob, uint32 cubAuthBlobSize, class CSteamID * pSteamIDUser)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SendUserConnectAndAuthenticate_DEPRECATED(unIPClient, pvAuthBlob, cubAuthBlobSize, pSteamIDUser);
}
S_API uint64 S_CALLTYPE SteamAPI_ISteamGameServer_CreateUnauthenticatedUserConnection(intptr_t instancePtr)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->CreateUnauthenticatedUserConnection().ConvertToUint64();
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SendUserDisconnect_DEPRECATED(intptr_t instancePtr, class CSteamID steamIDUser)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SendUserDisconnect_DEPRECATED(steamIDUser);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SendUserDisconnect(intptr_t instancePtr, class CSteamID steamIDUser)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SendUserDisconnect_DEPRECATED(steamIDUser);
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServer_BUpdateUserData(intptr_t instancePtr, class CSteamID steamIDUser, const char * pchPlayerName, uint32 uScore)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->BUpdateUserData(steamIDUser, pchPlayerName, uScore);
}
S_API HAuthTicket S_CALLTYPE SteamAPI_ISteamGameServer_GetAuthSessionTicket(intptr_t instancePtr, void* pTicket, int cbMaxTicket, uint32* pcbTicket, const SteamNetworkingIdentity* pSnid)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->GetAuthSessionTicket(pTicket, cbMaxTicket, pcbTicket, pSnid);
}
S_API EBeginAuthSessionResult S_CALLTYPE SteamAPI_ISteamGameServer_BeginAuthSession(intptr_t instancePtr, const void * pAuthTicket, int cbAuthTicket, class CSteamID steamID)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->BeginAuthSession(pAuthTicket, cbAuthTicket, steamID);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_EndAuthSession(intptr_t instancePtr, class CSteamID steamID)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->EndAuthSession(steamID);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_CancelAuthTicket(intptr_t instancePtr, HAuthTicket hAuthTicket)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->CancelAuthTicket(hAuthTicket);
}
S_API EUserHasLicenseForAppResult S_CALLTYPE SteamAPI_ISteamGameServer_UserHasLicenseForApp(intptr_t instancePtr, class CSteamID steamID, AppId_t appID)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->UserHasLicenseForApp(steamID, appID);
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServer_RequestUserGroupStatus(intptr_t instancePtr, class CSteamID steamIDUser, class CSteamID steamIDGroup)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->RequestUserGroupStatus(steamIDUser, steamIDGroup);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_GetGameplayStats(intptr_t instancePtr)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->GetGameplayStats();
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamGameServer_GetServerReputation(intptr_t instancePtr)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->GetServerReputation();
}
S_API struct SteamIPAddress_t S_CALLTYPE SteamAPI_ISteamGameServer_GetPublicIP(intptr_t instancePtr)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->GetPublicIP();
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServer_HandleIncomingPacket(intptr_t instancePtr, const void * pData, int cbData, uint32 srcIP, uint16 srcPort)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->HandleIncomingPacket(pData, cbData, srcIP, srcPort);
}
S_API int S_CALLTYPE SteamAPI_ISteamGameServer_GetNextOutgoingPacket(intptr_t instancePtr, void * pOut, int cbMaxOut, uint32 * pNetAdr, uint16 * pPort)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->GetNextOutgoingPacket(pOut, cbMaxOut, pNetAdr, pPort);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetAdvertiseServerActive(intptr_t instancePtr, bool bActive)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetAdvertiseServerActive(bActive);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_EnableHeartbeats(intptr_t instancePtr, bool bActive)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetAdvertiseServerActive(bActive);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_SetHeartbeatInterval(intptr_t instancePtr, int iHeartbeatInterval)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->SetMasterServerHeartbeatInterval_DEPRECATED(iHeartbeatInterval);
}
S_API void S_CALLTYPE SteamAPI_ISteamGameServer_ForceHeartbeat(intptr_t instancePtr)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->ForceMasterServerHeartbeat_DEPRECATED();
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamGameServer_AssociateWithClan(intptr_t instancePtr, class CSteamID steamIDClan)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->AssociateWithClan(steamIDClan);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamGameServer_ComputeNewPlayerCompatibility(intptr_t instancePtr, class CSteamID steamIDNewPlayer)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_pGameServer->ComputeNewPlayerCompatibility(steamIDNewPlayer);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamGameServerStats_RequestUserStats(intptr_t instancePtr, class CSteamID steamIDUser)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_ServerCtx.SteamGameServerStats()->RequestUserStats(steamIDUser);
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServerStats_GetUserStatInt32(intptr_t instancePtr, class CSteamID steamIDUser, const char * pchName, int32 * pData)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_ServerCtx.SteamGameServerStats()->GetUserStat(steamIDUser, pchName, pData);
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServerStats_GetUserStatFloat(intptr_t instancePtr, class CSteamID steamIDUser, const char * pchName, float * pData)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_ServerCtx.SteamGameServerStats()->GetUserStat(steamIDUser, pchName, pData);
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServerStats_GetUserAchievement(intptr_t instancePtr, class CSteamID steamIDUser, const char * pchName, bool * pbAchieved)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_ServerCtx.SteamGameServerStats()->GetUserAchievement(steamIDUser, pchName, pbAchieved);
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServerStats_SetUserStatInt32(intptr_t instancePtr, class CSteamID steamIDUser, const char * pchName, int32 nData)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_ServerCtx.SteamGameServerStats()->SetUserStat(steamIDUser, pchName, nData);
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServerStats_SetUserStatFloat(intptr_t instancePtr, class CSteamID steamIDUser, const char * pchName, float fData)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_ServerCtx.SteamGameServerStats()->SetUserStat(steamIDUser, pchName, fData);
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServerStats_UpdateUserAvgRateStat(intptr_t instancePtr, class CSteamID steamIDUser, const char * pchName, float flCountThisSession, double dSessionLength)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_ServerCtx.SteamGameServerStats()->UpdateUserAvgRateStat(steamIDUser, pchName, flCountThisSession, dSessionLength);
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServerStats_SetUserAchievement(intptr_t instancePtr, class CSteamID steamIDUser, const char * pchName)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_ServerCtx.SteamGameServerStats()->SetUserAchievement(steamIDUser, pchName);
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameServerStats_ClearUserAchievement(intptr_t instancePtr, class CSteamID steamIDUser, const char * pchName)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_ServerCtx.SteamGameServerStats()->ClearUserAchievement(steamIDUser, pchName);
}
S_API SteamAPICall_t S_CALLTYPE SteamAPI_ISteamGameServerStats_StoreUserStats(intptr_t instancePtr, class CSteamID steamIDUser)
{
	if (g_bServerReady == false)
		__debugbreak();
	return g_ServerCtx.SteamGameServerStats()->StoreUserStats(steamIDUser);
}

// ============================================================
// ISteamGameSearch - flat C API exports
// ============================================================

S_API intptr_t S_CALLTYPE SteamAPI_ISteamClient_GetISteamGameSearch(intptr_t instancePtr, HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion)
{
	return (intptr_t)g_ClientCtx.SteamGameSearch();
}

S_API void S_CALLTYPE SteamAPI_ISteamGameSearch_AddGameSearchParams(intptr_t instancePtr, const char *pchKeyToFind, const char *pchValueToFind)
{
	if (g_bClientReady)
		g_ClientCtx.SteamGameSearch()->AddGameSearchParams(pchKeyToFind, pchValueToFind);
}

S_API void S_CALLTYPE SteamAPI_ISteamGameSearch_SearchForLobbyWithGameFilters(intptr_t instancePtr, class CSteamID steamIDLobby, int nPlayerMin, int nPlayerMax, int nSearchSlots, int nPreferredSlots)
{
	if (g_bClientReady)
		g_ClientCtx.SteamGameSearch()->SearchForLobbyWithGameFilters(steamIDLobby, nPlayerMin, nPlayerMax, nSearchSlots, nPreferredSlots);
}

S_API uint32 S_CALLTYPE SteamAPI_ISteamGameSearch_GetTotalPlayersInLobby(intptr_t instancePtr, class CSteamID steamIDLobby)
{
	if (g_bClientReady)
		return g_ClientCtx.SteamGameSearch()->GetTotalPlayersInLobby(steamIDLobby);
	return 0;
}

S_API uint32 S_CALLTYPE SteamAPI_ISteamGameSearch_GetNumAvailableSlotsInLobby(intptr_t instancePtr, class CSteamID steamIDLobby)
{
	if (g_bClientReady)
		return g_ClientCtx.SteamGameSearch()->GetNumAvailableSlotsInLobby(steamIDLobby);
	return 0;
}

S_API void S_CALLTYPE SteamAPI_ISteamGameSearch_SubmitPlayerResult(intptr_t instancePtr, uint64 ulSearchID, class CSteamID steamIDPlayer, int nScore, int nResult)
{
	if (g_bClientReady)
		g_ClientCtx.SteamGameSearch()->SubmitPlayerResult(ulSearchID, steamIDPlayer, nScore, nResult);
}

S_API void S_CALLTYPE SteamAPI_ISteamGameSearch_EndGameSearch(intptr_t instancePtr, uint64 ulSearchID)
{
	if (g_bClientReady)
		g_ClientCtx.SteamGameSearch()->EndGameSearch(ulSearchID);
}

S_API void S_CALLTYPE SteamAPI_ISteamGameSearch_SetGameTags(intptr_t instancePtr, const char **pchGameTags, uint32 nGameTags)
{
	if (g_bClientReady)
		g_ClientCtx.SteamGameSearch()->SetGameTags(pchGameTags, nGameTags);
}

S_API uint32 S_CALLTYPE SteamAPI_ISteamGameSearch_GetGameTags(intptr_t instancePtr, char *pchGameTags, uint32 cchGameTags)
{
	if (g_bClientReady)
		return g_ClientCtx.SteamGameSearch()->GetGameTags(pchGameTags, cchGameTags);
	return 0;
}

S_API uint32 S_CALLTYPE SteamAPI_ISteamGameSearch_GetNumPlayersSearching(intptr_t instancePtr)
{
	if (g_bClientReady)
		return g_ClientCtx.SteamGameSearch()->GetNumPlayersSearching();
	return 0;
}

S_API uint64 S_CALLTYPE SteamAPI_ISteamGameSearch_RequestPlayersForLobby(intptr_t instancePtr, uint64 ulSearchID)
{
	if (g_bClientReady)
		return g_ClientCtx.SteamGameSearch()->RequestPlayersForLobby(ulSearchID);
	return 0;
}

S_API bool S_CALLTYPE SteamAPI_ISteamGameSearch_IsGameSearchInProgress(intptr_t instancePtr)
{
	if (g_bClientReady)
		return g_ClientCtx.SteamGameSearch()->IsGameSearchInProgress();
	return false;
}

S_API uint32 S_CALLTYPE SteamAPI_ISteamGameSearch_GetPlayersInGameSearchResult(intptr_t instancePtr, uint32 unSearchResultIndex)
{
	if (g_bClientReady)
		return g_ClientCtx.SteamGameSearch()->GetPlayersInGameSearchResult(unSearchResultIndex);
	return 0;
}

S_API void S_CALLTYPE SteamAPI_ISteamGameSearch_GetPlayerInGameSearchResult(intptr_t instancePtr, uint32 unSearchResultIndex, uint32 unPlayerIndex, uint64* pOutSteamID)
{
	if (g_bClientReady && pOutSteamID)
		*pOutSteamID = g_ClientCtx.SteamGameSearch()->GetPlayerInGameSearchResult(unSearchResultIndex, unPlayerIndex).ConvertToUint64();
}

S_API bool S_CALLTYPE SteamAPI_ISteamGameSearch_GetPlayerGameSearchResultData(intptr_t instancePtr, uint32 unSearchResultIndex, uint32 unPlayerIndex, const char *pchKeyToFind, char *pchValueFound, uint32 cchValueFound)
{
	if (g_bClientReady)
		return g_ClientCtx.SteamGameSearch()->GetPlayerGameSearchResultData(unSearchResultIndex, unPlayerIndex, pchKeyToFind, pchValueFound, cchValueFound);
	return false;
}

S_API uint32 S_CALLTYPE SteamAPI_ISteamGameSearch_GetPlayerGameSearchResultCount(intptr_t instancePtr, uint32 unSearchResultIndex, uint32 unPlayerIndex)
{
	if (g_bClientReady)
		return g_ClientCtx.SteamGameSearch()->GetPlayerGameSearchResultCount(unSearchResultIndex, unPlayerIndex);
	return 0;
}

// ============================================================
// ISteamMusicRemote flat API exports (stub - all return false/0)
// ============================================================
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_RegisterSteamMusicRemote(intptr_t instancePtr, const char* pchName)
{
	if (g_bClientReady)
		return SteamMusicRemote()->RegisterSteamMusicRemote(pchName);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_DeregisterSteamMusicRemote(intptr_t instancePtr)
{
	if (g_bClientReady)
		return SteamMusicRemote()->DeregisterSteamMusicRemote();
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_BIsCurrentMusicRemote(intptr_t instancePtr)
{
	if (g_bClientReady)
		return SteamMusicRemote()->BIsCurrentMusicRemote();
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_BActivationSuccess(intptr_t instancePtr, bool bValue)
{
	if (g_bClientReady)
		return SteamMusicRemote()->BActivationSuccess(bValue);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_SetDisplayName(intptr_t instancePtr, const char* pchDisplayName)
{
	if (g_bClientReady)
		return SteamMusicRemote()->SetDisplayName(pchDisplayName);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_SetPNGIcon_64x64(intptr_t instancePtr, void* pvBuffer, uint32 cbBuffer)
{
	if (g_bClientReady)
		return SteamMusicRemote()->SetPNGIcon_64x64(pvBuffer, cbBuffer);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_EnablePlayPrevious(intptr_t instancePtr, bool bValue)
{
	if (g_bClientReady)
		return SteamMusicRemote()->EnablePlayPrevious(bValue);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_EnablePlayNext(intptr_t instancePtr, bool bValue)
{
	if (g_bClientReady)
		return SteamMusicRemote()->EnablePlayNext(bValue);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_EnableShuffled(intptr_t instancePtr, bool bValue)
{
	if (g_bClientReady)
		return SteamMusicRemote()->EnableShuffled(bValue);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_EnableLooped(intptr_t instancePtr, bool bValue)
{
	if (g_bClientReady)
		return SteamMusicRemote()->EnableLooped(bValue);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_EnableQueue(intptr_t instancePtr, bool bValue)
{
	if (g_bClientReady)
		return SteamMusicRemote()->EnableQueue(bValue);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_EnablePlaylists(intptr_t instancePtr, bool bValue)
{
	if (g_bClientReady)
		return SteamMusicRemote()->EnablePlaylists(bValue);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_UpdatePlaybackStatus(intptr_t instancePtr, int nStatus)
{
	if (g_bClientReady)
		return SteamMusicRemote()->UpdatePlaybackStatus(nStatus);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_UpdateShuffled(intptr_t instancePtr, bool bValue)
{
	if (g_bClientReady)
		return SteamMusicRemote()->UpdateShuffled(bValue);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_UpdateLooped(intptr_t instancePtr, bool bValue)
{
	if (g_bClientReady)
		return SteamMusicRemote()->UpdateLooped(bValue);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_UpdateVolume(intptr_t instancePtr, float flVolume)
{
	if (g_bClientReady)
		return SteamMusicRemote()->UpdateVolume(flVolume);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_CurrentEntryWillChange(intptr_t instancePtr)
{
	if (g_bClientReady)
		return SteamMusicRemote()->CurrentEntryWillChange();
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_CurrentEntryIsAvailable(intptr_t instancePtr, bool bAvailable)
{
	if (g_bClientReady)
		return SteamMusicRemote()->CurrentEntryIsAvailable(bAvailable);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_UpdateCurrentEntryText(intptr_t instancePtr, const char* pchText)
{
	if (g_bClientReady)
		return SteamMusicRemote()->UpdateCurrentEntryText(pchText);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_UpdateCurrentEntryElapsedSeconds(intptr_t instancePtr, int nValue)
{
	if (g_bClientReady)
		return SteamMusicRemote()->UpdateCurrentEntryElapsedSeconds(nValue);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_UpdateCurrentEntryCoverArt(intptr_t instancePtr, void* pvBuffer, uint32 cbBuffer)
{
	if (g_bClientReady)
		return SteamMusicRemote()->UpdateCurrentEntryCoverArt(pvBuffer, cbBuffer);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_CurrentEntryDidChange(intptr_t instancePtr)
{
	if (g_bClientReady)
		return SteamMusicRemote()->CurrentEntryDidChange();
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_QueueWillChange(intptr_t instancePtr)
{
	if (g_bClientReady)
		return SteamMusicRemote()->QueueWillChange();
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_ResetQueueEntries(intptr_t instancePtr)
{
	if (g_bClientReady)
		return SteamMusicRemote()->ResetQueueEntries();
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_SetQueueEntry(intptr_t instancePtr, int nID, int nPosition, const char* pchEntryText)
{
	if (g_bClientReady)
		return SteamMusicRemote()->SetQueueEntry(nID, nPosition, pchEntryText);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_SetCurrentQueueEntry(intptr_t instancePtr, int nID)
{
	if (g_bClientReady)
		return SteamMusicRemote()->SetCurrentQueueEntry(nID);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_QueueDidChange(intptr_t instancePtr)
{
	if (g_bClientReady)
		return SteamMusicRemote()->QueueDidChange();
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_PlaylistWillChange(intptr_t instancePtr)
{
	if (g_bClientReady)
		return SteamMusicRemote()->PlaylistWillChange();
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_ResetPlaylistEntries(intptr_t instancePtr)
{
	if (g_bClientReady)
		return SteamMusicRemote()->ResetPlaylistEntries();
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_SetPlaylistEntry(intptr_t instancePtr, int nID, int nPosition, const char* pchEntryText)
{
	if (g_bClientReady)
		return SteamMusicRemote()->SetPlaylistEntry(nID, nPosition, pchEntryText);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_SetCurrentPlaylistEntry(intptr_t instancePtr, int nID)
{
	if (g_bClientReady)
		return SteamMusicRemote()->SetCurrentPlaylistEntry(nID);
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamMusicRemote_PlaylistDidChange(intptr_t instancePtr)
{
	if (g_bClientReady)
		return SteamMusicRemote()->PlaylistDidChange();
	return false;
}

// ============================================================
// ISteamGameSearch old-version flat API exports (stub)
// These existed in older Steam SDK but were removed/replaced.
// ============================================================
S_API bool S_CALLTYPE SteamAPI_ISteamGameSearch_SearchForGameSolo(intptr_t instancePtr, class CSteamID, int, int, int, int)
{
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameSearch_SearchForGameWithLobby(intptr_t instancePtr, class CSteamID, int, int, int, int)
{
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameSearch_AcceptGame(intptr_t instancePtr)
{
	return false;
}
S_API void S_CALLTYPE SteamAPI_ISteamGameSearch_DeclineGame(intptr_t instancePtr)
{
}
S_API void S_CALLTYPE SteamAPI_ISteamGameSearch_HostConfirmGameStart(intptr_t instancePtr, uint64)
{
}
S_API void S_CALLTYPE SteamAPI_ISteamGameSearch_CancelRequestPlayersForGame(intptr_t instancePtr)
{
}
S_API void S_CALLTYPE SteamAPI_ISteamGameSearch_SetGameHostParams(intptr_t instancePtr, const char*, const char*)
{
}
S_API void S_CALLTYPE SteamAPI_ISteamGameSearch_SetConnectionDetails(intptr_t instancePtr, const char*, const char*)
{
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameSearch_RequestPlayersForGame(intptr_t instancePtr, uint64)
{
	return false;
}
S_API bool S_CALLTYPE SteamAPI_ISteamGameSearch_RetrieveConnectionDetails(intptr_t instancePtr, uint64, char*, uint32)
{
	return false;
}
S_API void S_CALLTYPE SteamAPI_ISteamGameSearch_EndGame(intptr_t instancePtr, uint64, uint64)
{
}

// ============================================================
// Missing ISteamFriends exports (old SDK methods, use stubs)
// ============================================================
S_API void S_CALLTYPE SteamAPI_ISteamFriends_SetPersonaName(intptr_t instancePtr, const char* pchPersonaName)
{
	// SetPersonaName removed in newer SDK, no-op stub
}
S_API uint32 S_CALLTYPE SteamAPI_ISteamFriends_GetUserRestrictions(intptr_t instancePtr)
{
	// GetUserRestrictions removed in newer SDK, return no restrictions
	return 0;
}

// ============================================================
// Missing exports from original Steam SDK
// ============================================================
S_API ISteamClient* S_CALLTYPE SteamAPI_SteamClient_v019()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamClient_v019\r\n");
	return g_bClientReady ? g_ClientCtx.SteamClient() : nullptr;
}
