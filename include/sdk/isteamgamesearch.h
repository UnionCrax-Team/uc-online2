//====== Copyright Valve Corporation, All rights reserved. ====================
//
// Purpose: Steam Game Search interface
//
//=============================================================================

#ifndef ISTEAMGAMESEARCH_H
#define ISTEAMGAMESEARCH_H
#ifdef _WIN32
#pragma once
#endif

#include "steam_api_common.h"

//-----------------------------------------------------------------------------
// Purpose: interface for Game Search
//-----------------------------------------------------------------------------
class ISteamGameSearch
{
public:
	virtual ~ISteamGameSearch() {}

	// Search for a lobby matching the specified filters.  Results are returned in a LobbyMatchList_t callback.
	// nGameFilters should be 0 if there are no game-specific filters to apply.
	virtual void AddGameSearchParams( const char *pchKeyToFind, const char *pchValueToFind ) = 0;

	// Search for a lobby based on the game's own criteria, results in a LobbyMatchList_t callback
	virtual void SearchForLobbyWithGameFilters( CSteamID steamIDLobby, int nPlayerMin, int nPlayerMax, int nSearchSlots, int nPreferredSlots ) = 0;

	// Gets the total number of players in the specified lobby
	virtual uint32 GetTotalPlayersInLobby( CSteamID steamIDLobby ) = 0;

	// Returns the number of available lobby slots minus the number of players currently in the lobby
	virtual uint32 GetNumAvailableSlotsInLobby( CSteamID steamIDLobby ) = 0;

	// Submit a result for a single player or a team
	virtual void SubmitPlayerResult( uint64 ulSearchID, CSteamID steamIDPlayer, int nScore, EPlayerResultToken result ) = 0;

	// End the game search and return the players to the lobby
	virtual void EndGameSearch( uint64 ulSearchID ) = 0;

	// Sets the game's search tags.  These will be stored and can be used for filtering.
	virtual void SetGameTags( const char **pchGameTags, uint32 nGameTags ) = 0;

	// Gets the game's search tags
	virtual uint32 GetGameTags( char *pchGameTags, uint32 cchGameTags ) = 0;

	// Gets the number of players that have been found
	virtual uint32 GetNumPlayersSearching() = 0;

	// Gets the search results for a previous search
	virtual SteamAPICall_t RequestPlayersForLobby( uint64 ulSearchID ) = 0;

	// Returns true if the game is using the older ISteamGameSearch interface
	virtual bool IsGameSearchInProgress() = 0;

	// Gets the number of players in the game search results
	virtual uint32 GetPlayersInGameSearchResult( uint32 unSearchResultIndex ) = 0;

	// Gets the player at the specified index in the game search results
	virtual CSteamID GetPlayerInGameSearchResult( uint32 unSearchResultIndex, uint32 unPlayerIndex ) = 0;

	// Gets the custom data associated with the player
	virtual bool GetPlayerGameSearchResultData( uint32 unSearchResultIndex, uint32 unPlayerIndex, const char *pchKeyToFind, char *pchValueFound, uint32 cchValueFound ) = 0;

	// Gets the number of custom data entries for the player
	virtual uint32 GetPlayerGameSearchResultCount( uint32 unSearchResultIndex, uint32 unPlayerIndex ) = 0;
};

#define STEAMGAMESEARCH_INTERFACE_VERSION "STEAMGAMESEARCH_INTERFACE_VERSION001"

// Global accessor
inline ISteamGameSearch *SteamGameSearch();
STEAM_DEFINE_INTERFACE_ACCESSOR( ISteamGameSearch *, SteamGameSearch, STEAMGAMESEARCH_INTERFACE_VERSION );

#endif // ISTEAMGAMESEARCH_H
