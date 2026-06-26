#pragma once

#include "common.h"

// Steam helper functions to fetch user persona name and steam ID.

// Steam ID is entirely optional. Only exists because I decided to do
// things exactly the way OnlineFix does it, that's all.

// Disclaimer: AI generated code

typedef int      HSteamPipe;
typedef int      HSteamUser;
typedef uint64_t CSteamID;

typedef void*      (__cdecl *SteamClient_t)  (void);
typedef HSteamPipe (__cdecl *GetHSteamPipe_t)(void);
typedef HSteamUser (__cdecl *GetHSteamUser_t)(void);

// ----------- ISteamClient

typedef struct {
    void* stub0; /* CreateSteamPipe       */
    void* stub1; /* BReleaseSteamPipe     */
    void* stub2; /* ConnectToGlobalUser   */
    void* stub3; /* CreateLocalUser       */
    void* stub4; /* ReleaseUser           */
    void* (__cdecl *GetISteamUser)   (void* self, HSteamUser hUser, HSteamPipe hPipe, const char* pchVersion);
    void* stub6; /* GetISteamGameServer   */
    void* stub7; /* SetLocalIPBinding     */
    void* (__cdecl *GetISteamFriends)(void* self, HSteamUser hUser, HSteamPipe hPipe, const char* pchVersion);
} ISteamClient_vt;

typedef struct { ISteamClient_vt* vt; } ISteamClient;

// ----------- ISteamUser

typedef struct {
    void*     stub0; /* GetHSteamUser  */
    void*     stub1; /* BLoggedOn      */
    void (__cdecl *GetSteamID)(void* self, CSteamID *id);
} ISteamUser_vt;

typedef struct { ISteamUser_vt* vt; } ISteamUser;

// ----------- ISteamFriends

typedef struct {
    const char* (__cdecl *GetPersonaName)(void* self);
} ISteamFriends_vt;

typedef struct { ISteamFriends_vt* vt; } ISteamFriends;

// -------------------------

static ISteamClient *Steam_GetSteamClient(HSteamPipe *hPipe, HSteamUser *hUser) {
    HMODULE hAPI = GetModuleHandleA("steam_api64.dll");
    if (!hAPI) {
        LogText("Failed to load Steam API. Error : %lu", GetLastError());
        return NULL;
    }

    SteamClient_t fnSteamClient = (SteamClient_t) GetProcAddress(hAPI, "SteamClient");
    GetHSteamPipe_t fnGetHSteamPipe = (GetHSteamPipe_t) GetProcAddress(hAPI, "GetHSteamPipe");
    GetHSteamUser_t fnGetHSteamUser = (GetHSteamUser_t) GetProcAddress(hAPI, "GetHSteamUser");
    if (!fnSteamClient || !fnGetHSteamPipe || !fnGetHSteamUser) {
        LogText("Failed while loading functions from Steam API. Error : %lu", GetLastError());
        return NULL;
    }

    ISteamClient* pClient = (ISteamClient*) fnSteamClient();
    *hPipe = fnGetHSteamPipe();
    *hUser = fnGetHSteamUser();
    if (!pClient || !*hPipe || !*hUser) {
        LogText("Steam API invalid. Client: %p | Pipe: %d | User: %d", pClient, *hPipe, *hUser);
        return NULL;
    };
    return pClient;
}

static const char* Steam_GetPersonaName() {
    HSteamPipe hPipe;
    HSteamUser hUser;
    ISteamClient *pClient = Steam_GetSteamClient(&hPipe, &hUser);
    if (!pClient) return "YesYes";

    ISteamFriends* pFriends = (ISteamFriends*) pClient->vt->GetISteamFriends(pClient, hUser, hPipe, "SteamFriends005");
    if (!pFriends) {
        LogText("SteamClient returned NULL ISteamFriends");
        return "YesYes";
    }
    return pFriends->vt->GetPersonaName(pFriends);
}

static CSteamID Steam_GetSteamID(void) {
    HSteamPipe hPipe;
    HSteamUser hUser;
    ISteamClient *pClient = Steam_GetSteamClient(&hPipe, &hUser);
    if (!pClient) return 1337;

    ISteamUser* pUser = (ISteamUser*) pClient->vt->GetISteamUser(pClient, hUser, hPipe, "SteamUser012");
    if (!pUser) {
        LogText("SteamClient returned NULL ISteamUser");
        return 1337;
    }
    CSteamID id;
    pUser->vt->GetSteamID(pUser, &id);
    return id;
}
