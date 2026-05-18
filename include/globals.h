/**
 *  All of the interfaces, exports found in the real dlls, namespaces,
 *  functions, you name it. This is kinda necessary, and also really
 *  touchy. So try to be careful here, if you even look at it sideways
 *  it'll take that as a sign to just throw the whole build process lol.
 *
 *  ~veeλnti<3 2026
 */

#pragma once

#include <Windows.h>

#define STEAM_API_EXPORTS
#include "include/sdk/steam_api.h"
#include "include/sdk/steamclientpublic.h"
#include "include/sdk/steam_gameserver.h"
#include "include/sdk/isteamapplist.h"

const uint32 k_unServerFlagNone    = 0x00;
const uint32 k_unServerFlagSecure  = 0x01;
const uint32 k_unServerFlagPrivate = 0x02;

class ISteamGameSearch 
{ 
public: 
	virtual ~ISteamGameSearch() {} 
	virtual void AddGameSearchParams(const char*, const char*) = 0;
	virtual void SearchForLobbyWithGameFilters(class CSteamID, int, int, int, int) = 0;
	virtual uint32 GetTotalPlayersInLobby(class CSteamID) = 0;
	virtual uint32 GetNumAvailableSlotsInLobby(class CSteamID) = 0;
	virtual void SubmitPlayerResult(uint64, class CSteamID, int, int) = 0;
	virtual void EndGameSearch(uint64) = 0;
	virtual void SetGameTags(const char**, uint32) = 0;
	virtual uint32 GetGameTags(char*, uint32) = 0;
	virtual uint32 GetNumPlayersSearching() = 0;
	virtual uint64 RequestPlayersForLobby(uint64) = 0;
	virtual bool IsGameSearchInProgress() = 0;
	virtual uint32 GetPlayersInGameSearchResult(uint32) = 0;
	virtual class CSteamID GetPlayerInGameSearchResult(uint32, uint32) = 0;
	virtual bool GetPlayerGameSearchResultData(uint32, uint32, const char*, char*, uint32) = 0;
	virtual uint32 GetPlayerGameSearchResultCount(uint32, uint32) = 0;
};

class CSteamGameSearchStub : public ISteamGameSearch
{
public:
	virtual void AddGameSearchParams( const char *, const char * ) override {}
	virtual void SearchForLobbyWithGameFilters( CSteamID, int, int, int, int ) override {}
	virtual uint32 GetTotalPlayersInLobby( CSteamID ) override { return 0; }
	virtual uint32 GetNumAvailableSlotsInLobby( CSteamID ) override { return 0; }
	virtual void SubmitPlayerResult( uint64, CSteamID, int, int ) override {}
	virtual void EndGameSearch( uint64 ) override {}
	virtual void SetGameTags( const char **, uint32 ) override {}
	virtual uint32 GetGameTags( char *, uint32 ) override { return 0; }
	virtual uint32 GetNumPlayersSearching() override { return 0; }
	virtual SteamAPICall_t RequestPlayersForLobby( uint64 ) override { return k_uAPICallInvalid; }
	virtual bool IsGameSearchInProgress() override { return false; }
	virtual uint32 GetPlayersInGameSearchResult( uint32 ) override { return 0; }
	virtual  CSteamID GetPlayerInGameSearchResult( uint32, uint32 ) override { return k_steamIDNil; }
	virtual bool GetPlayerGameSearchResultData( uint32, uint32, const char *, char *, uint32 ) override { return false; }
	virtual uint32 GetPlayerGameSearchResultCount( uint32, uint32 ) override { return 0; }
};

static CSteamGameSearchStub s_GameSearchStub;

// music
class ISteamMusicRemote
{
public:
	virtual ~ISteamMusicRemote() {}
	virtual bool RegisterSteamMusicRemote(const char*) = 0;
	virtual bool DeregisterSteamMusicRemote() = 0;
	virtual bool BIsCurrentMusicRemote() = 0;
	virtual bool BActivationSuccess(bool) = 0;
	virtual bool SetDisplayName(const char*) = 0;
	virtual bool SetPNGIcon_64x64(void*, uint32) = 0;
	virtual bool EnablePlayPrevious(bool) = 0;
	virtual bool EnablePlayNext(bool) = 0;
	virtual bool EnableShuffled(bool) = 0;
	virtual bool EnableLooped(bool) = 0;
	virtual bool EnableQueue(bool) = 0;
	virtual bool EnablePlaylists(bool) = 0;
	virtual bool UpdatePlaybackStatus(int) = 0;
	virtual bool UpdateShuffled(bool) = 0;
	virtual bool UpdateLooped(bool) = 0;
	virtual bool UpdateVolume(float) = 0;
	virtual bool CurrentEntryWillChange() = 0;
	virtual bool CurrentEntryIsAvailable(bool) = 0;
	virtual bool UpdateCurrentEntryText(const char*) = 0;
	virtual bool UpdateCurrentEntryElapsedSeconds(int) = 0;
	virtual bool UpdateCurrentEntryCoverArt(void*, uint32) = 0;
	virtual bool CurrentEntryDidChange() = 0;
	virtual bool QueueWillChange() = 0;
	virtual bool ResetQueueEntries() = 0;
	virtual bool SetQueueEntry(int, int, const char*) = 0;
	virtual bool SetCurrentQueueEntry(int) = 0;
	virtual bool QueueDidChange() = 0;
	virtual bool PlaylistWillChange() = 0;
	virtual bool ResetPlaylistEntries() = 0;
	virtual bool SetPlaylistEntry(int, int, const char*) = 0;
	virtual bool SetCurrentPlaylistEntry(int) = 0;
	virtual bool PlaylistDidChange() = 0;
};

class CSteamMusicRemoteStub : public ISteamMusicRemote
{
public:
	virtual bool RegisterSteamMusicRemote(const char*) override { return false; }
	virtual bool DeregisterSteamMusicRemote() override { return false; }
	virtual bool BIsCurrentMusicRemote() override { return false; }
	virtual bool BActivationSuccess(bool) override { return false; }
	virtual bool SetDisplayName(const char*) override { return false; }
	virtual bool SetPNGIcon_64x64(void*, uint32) override { return false; }
	virtual bool EnablePlayPrevious(bool) override { return false; }
	virtual bool EnablePlayNext(bool) override { return false; }
	virtual bool EnableShuffled(bool) override { return false; }
	virtual bool EnableLooped(bool) override { return false; }
	virtual bool EnableQueue(bool) override { return false; }
	virtual bool EnablePlaylists(bool) override { return false; }
	virtual bool UpdatePlaybackStatus(int) override { return false; }
	virtual bool UpdateShuffled(bool) override { return false; }
	virtual bool UpdateLooped(bool) override { return false; }
	virtual bool UpdateVolume(float) override { return false; }
	virtual bool CurrentEntryWillChange() override { return false; }
	virtual bool CurrentEntryIsAvailable(bool) override { return false; }
	virtual bool UpdateCurrentEntryText(const char*) override { return false; }
	virtual bool UpdateCurrentEntryElapsedSeconds(int) override { return false; }
	virtual bool UpdateCurrentEntryCoverArt(void*, uint32) override { return false; }
	virtual bool CurrentEntryDidChange() override { return false; }
	virtual bool QueueWillChange() override { return false; }
	virtual bool ResetQueueEntries() override { return false; }
	virtual bool SetQueueEntry(int, int, const char*) override { return false; }
	virtual bool SetCurrentQueueEntry(int) override { return false; }
	virtual bool QueueDidChange() override { return false; }
	virtual bool PlaylistWillChange() override { return false; }
	virtual bool ResetPlaylistEntries() override { return false; }
	virtual bool SetPlaylistEntry(int, int, const char*) override { return false; }
	virtual bool SetCurrentPlaylistEntry(int) override { return false; }
	virtual bool PlaylistDidChange() override { return false; }
};

static CSteamMusicRemoteStub s_MusicRemoteStub;

class CSteamAPIContext
{
public:
	ISteamClient* m_pSteamClient;
	ISteamUser* m_pSteamUser;
	ISteamFriends* m_pSteamFriends;
	ISteamUtils* m_pSteamUtils;
	ISteamMatchmaking* m_pSteamMatchmaking;
	ISteamMatchmakingServers* m_pSteamMatchmakingServers;
	ISteamUserStats* m_pSteamUserStats;
	ISteamApps* m_pSteamApps;
	ISteamNetworking* m_pSteamNetworking;
	ISteamRemoteStorage* m_pSteamRemoteStorage;
	ISteamScreenshots* m_pSteamScreenshots;
	ISteamHTTP* m_pSteamHTTP;
	ISteamController* m_pSteamController;
	ISteamUGC* m_pSteamUGC;
	ISteamMusic* m_pSteamMusic;
	ISteamGameSearch* m_pSteamGameSearch;
	ISteamParties* m_pSteamParties;
	ISteamRemotePlay* m_pSteamRemotePlay;
	ISteamTimeline* m_pSteamTimeline;
	ISteamInventory* m_pSteamInventory;
	ISteamHTMLSurface* m_pSteamHTMLSurface;
	ISteamVideo* m_pSteamVideo;
	ISteamParentalSettings* m_pSteamParentalSettings;
	ISteamInput* m_pSteamInput;
	ISteamAppList* m_pSteamAppList;

	bool Init()
	{
		HSteamUser hUser = SteamAPI_GetHSteamUser();
		HSteamPipe hPipe = SteamAPI_GetHSteamPipe();
		if (hPipe == 0) return false;

		m_pSteamClient = (ISteamClient*)SteamInternal_CreateInterface(STEAMCLIENT_INTERFACE_VERSION);
		m_pSteamUser = (ISteamUser*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMUSER_INTERFACE_VERSION);
		m_pSteamFriends = (ISteamFriends*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMFRIENDS_INTERFACE_VERSION);
		m_pSteamUtils = (ISteamUtils*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMUTILS_INTERFACE_VERSION);
		m_pSteamMatchmaking = (ISteamMatchmaking*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMMATCHMAKING_INTERFACE_VERSION);
		m_pSteamMatchmakingServers = (ISteamMatchmakingServers*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMMATCHMAKINGSERVERS_INTERFACE_VERSION);
		m_pSteamUserStats = (ISteamUserStats*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMUSERSTATS_INTERFACE_VERSION);
		m_pSteamApps = (ISteamApps*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMAPPS_INTERFACE_VERSION);
		m_pSteamNetworking = (ISteamNetworking*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMNETWORKING_INTERFACE_VERSION);
		m_pSteamRemoteStorage = (ISteamRemoteStorage*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMREMOTESTORAGE_INTERFACE_VERSION);
		m_pSteamScreenshots = (ISteamScreenshots*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMSCREENSHOTS_INTERFACE_VERSION);
		m_pSteamHTTP = (ISteamHTTP*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMHTTP_INTERFACE_VERSION);
		m_pSteamController = (ISteamController*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMCONTROLLER_INTERFACE_VERSION);
		m_pSteamUGC = (ISteamUGC*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMUGC_INTERFACE_VERSION);
		m_pSteamMusic = (ISteamMusic*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMMUSIC_INTERFACE_VERSION);
		m_pSteamGameSearch = nullptr;
		m_pSteamParties = (ISteamParties*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMPARTIES_INTERFACE_VERSION);
		m_pSteamRemotePlay = (ISteamRemotePlay*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMREMOTEPLAY_INTERFACE_VERSION);
		m_pSteamTimeline = (ISteamTimeline*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMTIMELINE_INTERFACE_VERSION);
		m_pSteamInventory = (ISteamInventory*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMINVENTORY_INTERFACE_VERSION);
		m_pSteamHTMLSurface = (ISteamHTMLSurface*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMHTMLSURFACE_INTERFACE_VERSION);
		m_pSteamVideo = (ISteamVideo*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMVIDEO_INTERFACE_VERSION);
		m_pSteamParentalSettings = (ISteamParentalSettings*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMPARENTALSETTINGS_INTERFACE_VERSION);
		m_pSteamInput = (ISteamInput*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMINPUT_INTERFACE_VERSION);
		m_pSteamAppList = (ISteamAppList*)SteamInternal_FindOrCreateUserInterface(hUser, STEAMAPPLIST_INTERFACE_VERSION);

		return m_pSteamUser != nullptr && m_pSteamUtils != nullptr;
	}

	void Clear()
	{
		m_pSteamClient = nullptr; m_pSteamUser = nullptr; m_pSteamFriends = nullptr; m_pSteamUtils = nullptr;
		m_pSteamMatchmaking = nullptr; m_pSteamMatchmakingServers = nullptr;
		m_pSteamUserStats = nullptr; m_pSteamApps = nullptr; m_pSteamNetworking = nullptr;
		m_pSteamRemoteStorage = nullptr; m_pSteamScreenshots = nullptr; m_pSteamHTTP = nullptr;
		m_pSteamController = nullptr; m_pSteamUGC = nullptr; m_pSteamMusic = nullptr;
		m_pSteamGameSearch = nullptr; m_pSteamParties = nullptr;
		m_pSteamRemotePlay = nullptr; m_pSteamTimeline = nullptr;
		m_pSteamInventory = nullptr; m_pSteamHTMLSurface = nullptr;
		m_pSteamVideo = nullptr; m_pSteamParentalSettings = nullptr;
		m_pSteamInput = nullptr; m_pSteamAppList = nullptr;
	}

	ISteamClient* SteamClient() { return m_pSteamClient; }
	ISteamUser* SteamUser() { return m_pSteamUser; }
	ISteamFriends* SteamFriends() { return m_pSteamFriends; }
	ISteamUtils* SteamUtils() { return m_pSteamUtils; }
	ISteamMatchmaking* SteamMatchmaking() { return m_pSteamMatchmaking; }
	ISteamMatchmakingServers* SteamMatchmakingServers() { return m_pSteamMatchmakingServers; }
	ISteamUserStats* SteamUserStats() { return m_pSteamUserStats; }
	ISteamApps* SteamApps() { return m_pSteamApps; }
	ISteamNetworking* SteamNetworking() { return m_pSteamNetworking; }
	ISteamRemoteStorage* SteamRemoteStorage() { return m_pSteamRemoteStorage; }
	ISteamScreenshots* SteamScreenshots() { return m_pSteamScreenshots; }
	ISteamHTTP* SteamHTTP() { return m_pSteamHTTP; }
	ISteamController* SteamController() { return m_pSteamController; }
	ISteamUGC* SteamUGC() { return m_pSteamUGC; }
	ISteamMusic* SteamMusic() { return m_pSteamMusic; }
	ISteamGameSearch* SteamGameSearch() { return m_pSteamGameSearch; }
	ISteamParties* SteamParties() { return m_pSteamParties; }
	ISteamRemotePlay* SteamRemotePlay() { return m_pSteamRemotePlay; }
	ISteamInventory* SteamInventory() { return m_pSteamInventory; }
	ISteamHTMLSurface* SteamHTMLSurface() { return m_pSteamHTMLSurface; }
	ISteamVideo* SteamVideo() { return m_pSteamVideo; }
	ISteamParentalSettings* SteamParentalSettings() { return m_pSteamParentalSettings; }
	ISteamInput* SteamInput() { return m_pSteamInput; }
	ISteamTimeline* SteamTimeline() { return m_pSteamTimeline; }
	ISteamAppList* SteamAppList() { return m_pSteamAppList; }

};

class CSteamGameServerAPIContext
{
public:
	ISteamClient* m_pSteamClient;
	ISteamGameServer* m_pSteamGameServer;
	ISteamUtils* m_pSteamUtils;
	ISteamNetworking* m_pSteamNetworking;
	ISteamGameServerStats* m_pSteamGameServerStats;
	ISteamHTTP* m_pSteamHTTP;
	ISteamInventory* m_pSteamInventory;
	ISteamUGC* m_pSteamUGC;
	ISteamApps* m_pSteamApps;

	bool Init()
	{
		HSteamUser hUser = SteamGameServer_GetHSteamUser();
		HSteamPipe hPipe = SteamGameServer_GetHSteamPipe();
		if (hPipe == 0) return false;

		m_pSteamClient = (ISteamClient*)SteamInternal_FindOrCreateGameServerInterface(hUser, STEAMCLIENT_INTERFACE_VERSION);
		m_pSteamGameServer = (ISteamGameServer*)SteamInternal_FindOrCreateGameServerInterface(hUser, STEAMGAMESERVER_INTERFACE_VERSION);
		m_pSteamUtils = (ISteamUtils*)SteamInternal_FindOrCreateGameServerInterface(hUser, STEAMUTILS_INTERFACE_VERSION);
		m_pSteamNetworking = (ISteamNetworking*)SteamInternal_FindOrCreateGameServerInterface(hUser, STEAMNETWORKING_INTERFACE_VERSION);
		m_pSteamGameServerStats = (ISteamGameServerStats*)SteamInternal_FindOrCreateGameServerInterface(hUser, STEAMGAMESERVERSTATS_INTERFACE_VERSION);
		m_pSteamHTTP = (ISteamHTTP*)SteamInternal_FindOrCreateGameServerInterface(hUser, STEAMHTTP_INTERFACE_VERSION);
		m_pSteamInventory = (ISteamInventory*)SteamInternal_FindOrCreateGameServerInterface(hUser, STEAMINVENTORY_INTERFACE_VERSION);
		m_pSteamUGC = (ISteamUGC*)SteamInternal_FindOrCreateGameServerInterface(hUser, STEAMUGC_INTERFACE_VERSION);
		m_pSteamApps = (ISteamApps*)SteamInternal_FindOrCreateGameServerInterface(hUser, STEAMAPPS_INTERFACE_VERSION);

		return m_pSteamGameServer != nullptr && m_pSteamUtils != nullptr;
	}

	void Clear()
	{
		m_pSteamClient = nullptr; m_pSteamGameServer = nullptr; m_pSteamUtils = nullptr;
		m_pSteamNetworking = nullptr; m_pSteamGameServerStats = nullptr;
		m_pSteamHTTP = nullptr; m_pSteamInventory = nullptr; m_pSteamUGC = nullptr; m_pSteamApps = nullptr;
	}

	ISteamClient* SteamClient() { return m_pSteamClient; }
	ISteamGameServer* SteamGameServer() { return m_pSteamGameServer; }
	ISteamUtils* SteamGameServerUtils() { return m_pSteamUtils; }
	ISteamNetworking* SteamGameServerNetworking() { return m_pSteamNetworking; }
	ISteamGameServerStats* SteamGameServerStats() { return m_pSteamGameServerStats; }
	ISteamHTTP* SteamHTTP() { return m_pSteamHTTP; }
	ISteamInventory* SteamInventory() { return m_pSteamInventory; }
	ISteamUGC* SteamUGC() { return m_pSteamUGC; }
	ISteamApps* SteamApps() { return m_pSteamApps; }
};

// ============================================================

extern HMODULE g_ClientModule;
extern HSteamPipe g_ClientPipe;
extern HSteamUser g_ClientUser;
extern ISteamClient* g_pSteamClient;
extern ISteamClient* g_pSteamClientSafe;
extern ISteamUtils* g_pUtilsForCallbacks;
extern ISteamController* g_pControllerForCallbacks;
extern ISteamInput* g_pInputForCallbacks;
extern CSteamAPIContext g_ClientCtx;
extern bool g_bClientReady;
extern SRWLOCK g_CtxLock;

extern HMODULE g_ServerModule;
extern HSteamPipe g_ServerPipe;
extern HSteamUser g_ServerUser;
extern ISteamClient* g_ServerClient;
extern ISteamClient* g_pServerClient;
extern ISteamClient* g_pSteamClientGameServer_Latest;
extern ISteamGameServer* g_pGameServer;
extern ISteamUtils* g_pServerUtils;
extern CSteamGameServerAPIContext g_ServerCtx;
extern bool g_bServerReady;
extern EServerMode g_ServerMode;

extern bool g_bUsingBreakpad;
extern bool g_bFullDumps;
extern void* g_BreakpadCtx;
extern PFNPreMinidumpCallback g_BreakpadCallback;
extern char g_BreakpadVer[64];
extern char g_BreakpadTimestamp[64];
extern uint32 g_BreakpadAppID;
extern uint64 g_BreakpadSteamID;

extern bool g_bTryCatch;
extern int g_DispatchMode;
extern char g_InstallPath[MAX_PATH];
extern bool g_bHaveInstallPath;
extern SRWLOCK g_CallbackLock;
extern uint32 g_ForcedAppId;

typedef void* (S_CALLTYPE* Fn_CreateInterface)(const char* pName, int* pReturnCode);
extern Fn_CreateInterface g_pfnCreateInterface;

typedef void (S_CALLTYPE* Fn_ReleaseThreadLocal)(int bThreadExit);
extern Fn_ReleaseThreadLocal g_pfnReleaseThreadLocal;

typedef bool (S_CALLTYPE* Fn_IsKnownInterface)(const char* Interface);
extern Fn_IsKnownInterface g_pfnIsKnownInterface;

typedef void (S_CALLTYPE* Fn_NotifyMissing)(HSteamPipe hPipe, const char* Interface);
extern Fn_NotifyMissing g_pfnNotifyMissing;

typedef void (S_CALLTYPE* Fn_BreakpadInit)(uint32 AppID, const char* ver, const char* date, bool bFull, void* ctx, PFNPreMinidumpCallback cb);
extern Fn_BreakpadInit g_pfnBreakpadInit;

typedef void (S_CALLTYPE* Fn_BreakpadSetAppID)(uint32 AppID);
extern Fn_BreakpadSetAppID g_pfnBreakpadSetAppID;

typedef void (S_CALLTYPE* Fn_BreakpadSetSteamID)(uint64 SteamID);
extern Fn_BreakpadSetSteamID g_pfnBreakpadSetSteamID;

typedef void (S_CALLTYPE* Fn_BreakpadSetComment)(const char* Comment);
extern Fn_BreakpadSetComment g_pfnBreakpadSetComment;

typedef void (S_CALLTYPE* Fn_BreakpadWriteDump)(uint32 Code, void* Info, uint32 BuildID);
extern Fn_BreakpadWriteDump g_pfnBreakpadWriteDump;

extern uintp g_CtxCounter;

void UCOLOG(const char* fmt, ...);
void UCOColor(WORD color, const char* text);
void* InitSteamClient(HMODULE* phModule, bool bLocal, const char* iface);
void LoadBreakpadSymbols(HMODULE hMod);
void UpdateMinidumpSteamID(uint64 sid);
void InstallBIsSubscribedAppHook();
