#define ORIGINAL_DLL "EOSSDK-Win64-Shipping.yes"

#pragma comment(linker, "/export:EOS_Achievements_AddNotifyAchievementsUnlocked=" ORIGINAL_DLL ".EOS_Achievements_AddNotifyAchievementsUnlocked,@1")
#pragma comment(linker, "/export:EOS_Achievements_AddNotifyAchievementsUnlockedV2=" ORIGINAL_DLL ".EOS_Achievements_AddNotifyAchievementsUnlockedV2,@2")
#pragma comment(linker, "/export:EOS_Achievements_CopyAchievementDefinitionByAchievementId=" ORIGINAL_DLL ".EOS_Achievements_CopyAchievementDefinitionByAchievementId,@3")
#pragma comment(linker, "/export:EOS_Achievements_CopyAchievementDefinitionByIndex=" ORIGINAL_DLL ".EOS_Achievements_CopyAchievementDefinitionByIndex,@4")
#pragma comment(linker, "/export:EOS_Achievements_CopyAchievementDefinitionV2ByAchievementId=" ORIGINAL_DLL ".EOS_Achievements_CopyAchievementDefinitionV2ByAchievementId,@5")
#pragma comment(linker, "/export:EOS_Achievements_CopyAchievementDefinitionV2ByIndex=" ORIGINAL_DLL ".EOS_Achievements_CopyAchievementDefinitionV2ByIndex,@6")
#pragma comment(linker, "/export:EOS_Achievements_CopyPlayerAchievementByAchievementId=" ORIGINAL_DLL ".EOS_Achievements_CopyPlayerAchievementByAchievementId,@7")
#pragma comment(linker, "/export:EOS_Achievements_CopyPlayerAchievementByIndex=" ORIGINAL_DLL ".EOS_Achievements_CopyPlayerAchievementByIndex,@8")
#pragma comment(linker, "/export:EOS_Achievements_CopyUnlockedAchievementByAchievementId=" ORIGINAL_DLL ".EOS_Achievements_CopyUnlockedAchievementByAchievementId,@9")
#pragma comment(linker, "/export:EOS_Achievements_CopyUnlockedAchievementByIndex=" ORIGINAL_DLL ".EOS_Achievements_CopyUnlockedAchievementByIndex,@10")
#pragma comment(linker, "/export:EOS_Achievements_DefinitionV2_Release=" ORIGINAL_DLL ".EOS_Achievements_DefinitionV2_Release,@11")
#pragma comment(linker, "/export:EOS_Achievements_Definition_Release=" ORIGINAL_DLL ".EOS_Achievements_Definition_Release,@12")
#pragma comment(linker, "/export:EOS_Achievements_GetAchievementDefinitionCount=" ORIGINAL_DLL ".EOS_Achievements_GetAchievementDefinitionCount,@13")
#pragma comment(linker, "/export:EOS_Achievements_GetPlayerAchievementCount=" ORIGINAL_DLL ".EOS_Achievements_GetPlayerAchievementCount,@14")
#pragma comment(linker, "/export:EOS_Achievements_GetUnlockedAchievementCount=" ORIGINAL_DLL ".EOS_Achievements_GetUnlockedAchievementCount,@15")
#pragma comment(linker, "/export:EOS_Achievements_PlayerAchievement_Release=" ORIGINAL_DLL ".EOS_Achievements_PlayerAchievement_Release,@16")
#pragma comment(linker, "/export:EOS_Achievements_QueryDefinitions=" ORIGINAL_DLL ".EOS_Achievements_QueryDefinitions,@17")
#pragma comment(linker, "/export:EOS_Achievements_QueryPlayerAchievements=" ORIGINAL_DLL ".EOS_Achievements_QueryPlayerAchievements,@18")
#pragma comment(linker, "/export:EOS_Achievements_RemoveNotifyAchievementsUnlocked=" ORIGINAL_DLL ".EOS_Achievements_RemoveNotifyAchievementsUnlocked,@19")
#pragma comment(linker, "/export:EOS_Achievements_UnlockAchievements=" ORIGINAL_DLL ".EOS_Achievements_UnlockAchievements,@20")
#pragma comment(linker, "/export:EOS_Achievements_UnlockedAchievement_Release=" ORIGINAL_DLL ".EOS_Achievements_UnlockedAchievement_Release,@21")
#pragma comment(linker, "/export:EOS_ActiveSession_CopyInfo=" ORIGINAL_DLL ".EOS_ActiveSession_CopyInfo,@22")
#pragma comment(linker, "/export:EOS_ActiveSession_GetRegisteredPlayerByIndex=" ORIGINAL_DLL ".EOS_ActiveSession_GetRegisteredPlayerByIndex,@23")
#pragma comment(linker, "/export:EOS_ActiveSession_GetRegisteredPlayerCount=" ORIGINAL_DLL ".EOS_ActiveSession_GetRegisteredPlayerCount,@24")
#pragma comment(linker, "/export:EOS_ActiveSession_Info_Release=" ORIGINAL_DLL ".EOS_ActiveSession_Info_Release,@25")
#pragma comment(linker, "/export:EOS_ActiveSession_Release=" ORIGINAL_DLL ".EOS_ActiveSession_Release,@26")
#pragma comment(linker, "/export:EOS_AntiCheatClient_AddExternalIntegrityCatalog=" ORIGINAL_DLL ".EOS_AntiCheatClient_AddExternalIntegrityCatalog,@27")
#pragma comment(linker, "/export:EOS_AntiCheatClient_AddNotifyClientIntegrityViolated=" ORIGINAL_DLL ".EOS_AntiCheatClient_AddNotifyClientIntegrityViolated,@28")
#pragma comment(linker, "/export:EOS_AntiCheatClient_AddNotifyMessageToPeer=" ORIGINAL_DLL ".EOS_AntiCheatClient_AddNotifyMessageToPeer,@29")
#pragma comment(linker, "/export:EOS_AntiCheatClient_AddNotifyMessageToServer=" ORIGINAL_DLL ".EOS_AntiCheatClient_AddNotifyMessageToServer,@30")
#pragma comment(linker, "/export:EOS_AntiCheatClient_AddNotifyPeerActionRequired=" ORIGINAL_DLL ".EOS_AntiCheatClient_AddNotifyPeerActionRequired,@31")
#pragma comment(linker, "/export:EOS_AntiCheatClient_AddNotifyPeerAuthStatusChanged=" ORIGINAL_DLL ".EOS_AntiCheatClient_AddNotifyPeerAuthStatusChanged,@32")
#pragma comment(linker, "/export:EOS_AntiCheatClient_BeginSession=EOS_AntiCheatClient_BeginSession,@33")
// #pragma comment(linker, "/export:EOS_AntiCheatClient_EndSession=" ORIGINAL_DLL ".EOS_AntiCheatClient_EndSession,@34")
#pragma comment(linker, "/export:EOS_AntiCheatClient_GetProtectMessageOutputLength=" ORIGINAL_DLL ".EOS_AntiCheatClient_GetProtectMessageOutputLength,@35")
#pragma comment(linker, "/export:EOS_AntiCheatClient_PollStatus=EOS_AntiCheatClient_PollStatus,@36")
// #pragma comment(linker, "/export:EOS_AntiCheatClient_ProtectMessage=" ORIGINAL_DLL ".EOS_AntiCheatClient_ProtectMessage,@37")
#pragma comment(linker, "/export:EOS_AntiCheatClient_ReceiveMessageFromPeer=" ORIGINAL_DLL ".EOS_AntiCheatClient_ReceiveMessageFromPeer,@38")
#pragma comment(linker, "/export:EOS_AntiCheatClient_ReceiveMessageFromServer=" ORIGINAL_DLL ".EOS_AntiCheatClient_ReceiveMessageFromServer,@39")
#pragma comment(linker, "/export:EOS_AntiCheatClient_RegisterPeer=" ORIGINAL_DLL ".EOS_AntiCheatClient_RegisterPeer,@40")
#pragma comment(linker, "/export:EOS_AntiCheatClient_RemoveNotifyClientIntegrityViolated=" ORIGINAL_DLL ".EOS_AntiCheatClient_RemoveNotifyClientIntegrityViolated,@41")
#pragma comment(linker, "/export:EOS_AntiCheatClient_RemoveNotifyMessageToPeer=" ORIGINAL_DLL ".EOS_AntiCheatClient_RemoveNotifyMessageToPeer,@42")
#pragma comment(linker, "/export:EOS_AntiCheatClient_RemoveNotifyMessageToServer=" ORIGINAL_DLL ".EOS_AntiCheatClient_RemoveNotifyMessageToServer,@43")
#pragma comment(linker, "/export:EOS_AntiCheatClient_RemoveNotifyPeerActionRequired=" ORIGINAL_DLL ".EOS_AntiCheatClient_RemoveNotifyPeerActionRequired,@44")
#pragma comment(linker, "/export:EOS_AntiCheatClient_RemoveNotifyPeerAuthStatusChanged=" ORIGINAL_DLL ".EOS_AntiCheatClient_RemoveNotifyPeerAuthStatusChanged,@45")
#pragma comment(linker, "/export:EOS_AntiCheatClient_UnprotectMessage=" ORIGINAL_DLL ".EOS_AntiCheatClient_UnprotectMessage,@46")
#pragma comment(linker, "/export:EOS_AntiCheatClient_UnregisterPeer=" ORIGINAL_DLL ".EOS_AntiCheatClient_UnregisterPeer,@47")
#pragma comment(linker, "/export:EOS_AntiCheatServer_AddNotifyClientActionRequired=" ORIGINAL_DLL ".EOS_AntiCheatServer_AddNotifyClientActionRequired,@48")
#pragma comment(linker, "/export:EOS_AntiCheatServer_AddNotifyClientAuthStatusChanged=" ORIGINAL_DLL ".EOS_AntiCheatServer_AddNotifyClientAuthStatusChanged,@49")
#pragma comment(linker, "/export:EOS_AntiCheatServer_AddNotifyMessageToClient=" ORIGINAL_DLL ".EOS_AntiCheatServer_AddNotifyMessageToClient,@50")
#pragma comment(linker, "/export:EOS_AntiCheatServer_BeginSession=" ORIGINAL_DLL ".EOS_AntiCheatServer_BeginSession,@51")
#pragma comment(linker, "/export:EOS_AntiCheatServer_EndSession=" ORIGINAL_DLL ".EOS_AntiCheatServer_EndSession,@52")
#pragma comment(linker, "/export:EOS_AntiCheatServer_GetProtectMessageOutputLength=" ORIGINAL_DLL ".EOS_AntiCheatServer_GetProtectMessageOutputLength,@53")
#pragma comment(linker, "/export:EOS_AntiCheatServer_LogEvent=" ORIGINAL_DLL ".EOS_AntiCheatServer_LogEvent,@54")
#pragma comment(linker, "/export:EOS_AntiCheatServer_LogGameRoundEnd=" ORIGINAL_DLL ".EOS_AntiCheatServer_LogGameRoundEnd,@55")
#pragma comment(linker, "/export:EOS_AntiCheatServer_LogGameRoundStart=" ORIGINAL_DLL ".EOS_AntiCheatServer_LogGameRoundStart,@56")
#pragma comment(linker, "/export:EOS_AntiCheatServer_LogPlayerDespawn=" ORIGINAL_DLL ".EOS_AntiCheatServer_LogPlayerDespawn,@57")
#pragma comment(linker, "/export:EOS_AntiCheatServer_LogPlayerRevive=" ORIGINAL_DLL ".EOS_AntiCheatServer_LogPlayerRevive,@58")
#pragma comment(linker, "/export:EOS_AntiCheatServer_LogPlayerSpawn=" ORIGINAL_DLL ".EOS_AntiCheatServer_LogPlayerSpawn,@59")
#pragma comment(linker, "/export:EOS_AntiCheatServer_LogPlayerTakeDamage=" ORIGINAL_DLL ".EOS_AntiCheatServer_LogPlayerTakeDamage,@60")
#pragma comment(linker, "/export:EOS_AntiCheatServer_LogPlayerTick=" ORIGINAL_DLL ".EOS_AntiCheatServer_LogPlayerTick,@61")
#pragma comment(linker, "/export:EOS_AntiCheatServer_LogPlayerUseAbility=" ORIGINAL_DLL ".EOS_AntiCheatServer_LogPlayerUseAbility,@62")
#pragma comment(linker, "/export:EOS_AntiCheatServer_LogPlayerUseWeapon=" ORIGINAL_DLL ".EOS_AntiCheatServer_LogPlayerUseWeapon,@63")
#pragma comment(linker, "/export:EOS_AntiCheatServer_ProtectMessage=" ORIGINAL_DLL ".EOS_AntiCheatServer_ProtectMessage,@64")
#pragma comment(linker, "/export:EOS_AntiCheatServer_ReceiveMessageFromClient=" ORIGINAL_DLL ".EOS_AntiCheatServer_ReceiveMessageFromClient,@65")
#pragma comment(linker, "/export:EOS_AntiCheatServer_RegisterClient=" ORIGINAL_DLL ".EOS_AntiCheatServer_RegisterClient,@66")
#pragma comment(linker, "/export:EOS_AntiCheatServer_RegisterEvent=" ORIGINAL_DLL ".EOS_AntiCheatServer_RegisterEvent,@67")
#pragma comment(linker, "/export:EOS_AntiCheatServer_RemoveNotifyClientActionRequired=" ORIGINAL_DLL ".EOS_AntiCheatServer_RemoveNotifyClientActionRequired,@68")
#pragma comment(linker, "/export:EOS_AntiCheatServer_RemoveNotifyClientAuthStatusChanged=" ORIGINAL_DLL ".EOS_AntiCheatServer_RemoveNotifyClientAuthStatusChanged,@69")
#pragma comment(linker, "/export:EOS_AntiCheatServer_RemoveNotifyMessageToClient=" ORIGINAL_DLL ".EOS_AntiCheatServer_RemoveNotifyMessageToClient,@70")
#pragma comment(linker, "/export:EOS_AntiCheatServer_SetClientDetails=" ORIGINAL_DLL ".EOS_AntiCheatServer_SetClientDetails,@71")
#pragma comment(linker, "/export:EOS_AntiCheatServer_SetClientNetworkState=" ORIGINAL_DLL ".EOS_AntiCheatServer_SetClientNetworkState,@72")
#pragma comment(linker, "/export:EOS_AntiCheatServer_SetGameSessionId=" ORIGINAL_DLL ".EOS_AntiCheatServer_SetGameSessionId,@73")
#pragma comment(linker, "/export:EOS_AntiCheatServer_UnprotectMessage=" ORIGINAL_DLL ".EOS_AntiCheatServer_UnprotectMessage,@74")
#pragma comment(linker, "/export:EOS_AntiCheatServer_UnregisterClient=" ORIGINAL_DLL ".EOS_AntiCheatServer_UnregisterClient,@75")
#pragma comment(linker, "/export:EOS_Audio_CreateNewInputStream=" ORIGINAL_DLL ".EOS_Audio_CreateNewInputStream,@76")
#pragma comment(linker, "/export:EOS_Audio_CreateNewOutputStream=" ORIGINAL_DLL ".EOS_Audio_CreateNewOutputStream,@77")
#pragma comment(linker, "/export:EOS_Audio_DestroyInputStream=" ORIGINAL_DLL ".EOS_Audio_DestroyInputStream,@78")
#pragma comment(linker, "/export:EOS_Audio_DestroyOutputStream=" ORIGINAL_DLL ".EOS_Audio_DestroyOutputStream,@79")
#pragma comment(linker, "/export:EOS_Audio_EnableCommunicationsModeOutputDevices=" ORIGINAL_DLL ".EOS_Audio_EnableCommunicationsModeOutputDevices,@80")
#pragma comment(linker, "/export:EOS_Audio_GetInputDeviceInfo=" ORIGINAL_DLL ".EOS_Audio_GetInputDeviceInfo,@81")
#pragma comment(linker, "/export:EOS_Audio_GetInputStreamInfo=" ORIGINAL_DLL ".EOS_Audio_GetInputStreamInfo,@82")
#pragma comment(linker, "/export:EOS_Audio_GetOutputDeviceInfo=" ORIGINAL_DLL ".EOS_Audio_GetOutputDeviceInfo,@83")
#pragma comment(linker, "/export:EOS_Audio_GetOutputStreamInfo=" ORIGINAL_DLL ".EOS_Audio_GetOutputStreamInfo,@84")
#pragma comment(linker, "/export:EOS_Audio_IsInputStreamDeviceDisconnected=" ORIGINAL_DLL ".EOS_Audio_IsInputStreamDeviceDisconnected,@85")
#pragma comment(linker, "/export:EOS_Audio_IsInputStreamSilent=" ORIGINAL_DLL ".EOS_Audio_IsInputStreamSilent,@86")
#pragma comment(linker, "/export:EOS_Audio_QueryInputDevices=" ORIGINAL_DLL ".EOS_Audio_QueryInputDevices,@87")
#pragma comment(linker, "/export:EOS_Audio_QueryOutputDevices=" ORIGINAL_DLL ".EOS_Audio_QueryOutputDevices,@88")
#pragma comment(linker, "/export:EOS_Audio_RegisterUser=" ORIGINAL_DLL ".EOS_Audio_RegisterUser,@89")
#pragma comment(linker, "/export:EOS_Audio_RemoveNotifyDevicesChanged=" ORIGINAL_DLL ".EOS_Audio_RemoveNotifyDevicesChanged,@90")
#pragma comment(linker, "/export:EOS_Audio_SetFeatureEnabledForInputStream=" ORIGINAL_DLL ".EOS_Audio_SetFeatureEnabledForInputStream,@91")
#pragma comment(linker, "/export:EOS_Audio_SetNotifyDevicesChanged=" ORIGINAL_DLL ".EOS_Audio_SetNotifyDevicesChanged,@92")
#pragma comment(linker, "/export:EOS_Audio_StartInputStream=" ORIGINAL_DLL ".EOS_Audio_StartInputStream,@93")
#pragma comment(linker, "/export:EOS_Audio_StartOutputStream=" ORIGINAL_DLL ".EOS_Audio_StartOutputStream,@94")
#pragma comment(linker, "/export:EOS_Audio_StopInputStream=" ORIGINAL_DLL ".EOS_Audio_StopInputStream,@95")
#pragma comment(linker, "/export:EOS_Audio_StopOutputStream=" ORIGINAL_DLL ".EOS_Audio_StopOutputStream,@96")
#pragma comment(linker, "/export:EOS_Audio_UnregisterUser=" ORIGINAL_DLL ".EOS_Audio_UnregisterUser,@97")
#pragma comment(linker, "/export:EOS_Auth_AddNotifyLoginStatusChanged=" ORIGINAL_DLL ".EOS_Auth_AddNotifyLoginStatusChanged,@98")
#pragma comment(linker, "/export:EOS_Auth_CopyIdToken=" ORIGINAL_DLL ".EOS_Auth_CopyIdToken,@99")
#pragma comment(linker, "/export:EOS_Auth_CopyUserAuthToken=" ORIGINAL_DLL ".EOS_Auth_CopyUserAuthToken,@100")
#pragma comment(linker, "/export:EOS_Auth_DeletePersistentAuth=" ORIGINAL_DLL ".EOS_Auth_DeletePersistentAuth,@101")
#pragma comment(linker, "/export:EOS_Auth_GetLoggedInAccountByIndex=" ORIGINAL_DLL ".EOS_Auth_GetLoggedInAccountByIndex,@102")
#pragma comment(linker, "/export:EOS_Auth_GetLoggedInAccountsCount=" ORIGINAL_DLL ".EOS_Auth_GetLoggedInAccountsCount,@103")
#pragma comment(linker, "/export:EOS_Auth_GetLoginStatus=" ORIGINAL_DLL ".EOS_Auth_GetLoginStatus,@104")
#pragma comment(linker, "/export:EOS_Auth_GetMergedAccountByIndex=" ORIGINAL_DLL ".EOS_Auth_GetMergedAccountByIndex,@105")
#pragma comment(linker, "/export:EOS_Auth_GetMergedAccountsCount=" ORIGINAL_DLL ".EOS_Auth_GetMergedAccountsCount,@106")
#pragma comment(linker, "/export:EOS_Auth_GetSelectedAccountId=" ORIGINAL_DLL ".EOS_Auth_GetSelectedAccountId,@107")
#pragma comment(linker, "/export:EOS_Auth_IdToken_Release=" ORIGINAL_DLL ".EOS_Auth_IdToken_Release,@108")
#pragma comment(linker, "/export:EOS_Auth_LinkAccount=" ORIGINAL_DLL ".EOS_Auth_LinkAccount,@109")
#pragma comment(linker, "/export:EOS_Auth_Logout=" ORIGINAL_DLL ".EOS_Auth_Logout,@111")
#pragma comment(linker, "/export:EOS_Auth_QueryIdToken=" ORIGINAL_DLL ".EOS_Auth_QueryIdToken,@112")
#pragma comment(linker, "/export:EOS_Auth_RemoveNotifyLoginStatusChanged=" ORIGINAL_DLL ".EOS_Auth_RemoveNotifyLoginStatusChanged,@113")
#pragma comment(linker, "/export:EOS_Auth_Token_Release=" ORIGINAL_DLL ".EOS_Auth_Token_Release,@114")
#pragma comment(linker, "/export:EOS_Auth_VerifyIdToken=" ORIGINAL_DLL ".EOS_Auth_VerifyIdToken,@115")
#pragma comment(linker, "/export:EOS_Auth_VerifyUserAuth=" ORIGINAL_DLL ".EOS_Auth_VerifyUserAuth,@116")
#pragma comment(linker, "/export:EOS_BeginScopeEvent=" ORIGINAL_DLL ".EOS_BeginScopeEvent,@117")
#pragma comment(linker, "/export:EOS_BroadcastAudio_CreateNewInputStream=" ORIGINAL_DLL ".EOS_BroadcastAudio_CreateNewInputStream,@118")
#pragma comment(linker, "/export:EOS_BroadcastAudio_CreateNewOutputStream=" ORIGINAL_DLL ".EOS_BroadcastAudio_CreateNewOutputStream,@119")
#pragma comment(linker, "/export:EOS_BroadcastAudio_DestroyInputStream=" ORIGINAL_DLL ".EOS_BroadcastAudio_DestroyInputStream,@120")
#pragma comment(linker, "/export:EOS_BroadcastAudio_DestroyOutputStream=" ORIGINAL_DLL ".EOS_BroadcastAudio_DestroyOutputStream,@121")
#pragma comment(linker, "/export:EOS_BroadcastAudio_GetCurrentGainLevel=" ORIGINAL_DLL ".EOS_BroadcastAudio_GetCurrentGainLevel,@122")
#pragma comment(linker, "/export:EOS_BroadcastAudio_GetCurrentMicAmplitude=" ORIGINAL_DLL ".EOS_BroadcastAudio_GetCurrentMicAmplitude,@123")
#pragma comment(linker, "/export:EOS_BroadcastAudio_GetInputStreamInfo=" ORIGINAL_DLL ".EOS_BroadcastAudio_GetInputStreamInfo,@124")
#pragma comment(linker, "/export:EOS_BroadcastAudio_GetOutputStreamInfo=" ORIGINAL_DLL ".EOS_BroadcastAudio_GetOutputStreamInfo,@125")
#pragma comment(linker, "/export:EOS_BroadcastAudio_PushPacketToOutputStream=" ORIGINAL_DLL ".EOS_BroadcastAudio_PushPacketToOutputStream,@126")
#pragma comment(linker, "/export:EOS_BroadcastAudio_SetEncoderSettings=" ORIGINAL_DLL ".EOS_BroadcastAudio_SetEncoderSettings,@127")
#pragma comment(linker, "/export:EOS_BroadcastAudio_SetMicProcessingSettings=" ORIGINAL_DLL ".EOS_BroadcastAudio_SetMicProcessingSettings,@128")
#pragma comment(linker, "/export:EOS_BroadcastAudio_StartInputStream=" ORIGINAL_DLL ".EOS_BroadcastAudio_StartInputStream,@129")
#pragma comment(linker, "/export:EOS_BroadcastAudio_StartOutputStream=" ORIGINAL_DLL ".EOS_BroadcastAudio_StartOutputStream,@130")
#pragma comment(linker, "/export:EOS_BroadcastAudio_StopInputStream=" ORIGINAL_DLL ".EOS_BroadcastAudio_StopInputStream,@131")
#pragma comment(linker, "/export:EOS_BroadcastAudio_StopOutputStream=" ORIGINAL_DLL ".EOS_BroadcastAudio_StopOutputStream,@132")
#pragma comment(linker, "/export:EOS_ByteArray_ToString=" ORIGINAL_DLL ".EOS_ByteArray_ToString,@133")
#pragma comment(linker, "/export:EOS_Connect_AddNotifyAuthExpiration=" ORIGINAL_DLL ".EOS_Connect_AddNotifyAuthExpiration,@134")
#pragma comment(linker, "/export:EOS_Connect_AddNotifyLoginStatusChanged=" ORIGINAL_DLL ".EOS_Connect_AddNotifyLoginStatusChanged,@135")
#pragma comment(linker, "/export:EOS_Connect_CopyIdToken=" ORIGINAL_DLL ".EOS_Connect_CopyIdToken,@136")
#pragma comment(linker, "/export:EOS_Connect_CopyProductUserExternalAccountByAccountId=" ORIGINAL_DLL ".EOS_Connect_CopyProductUserExternalAccountByAccountId,@137")
#pragma comment(linker, "/export:EOS_Connect_CopyProductUserExternalAccountByAccountType=" ORIGINAL_DLL ".EOS_Connect_CopyProductUserExternalAccountByAccountType,@138")
#pragma comment(linker, "/export:EOS_Connect_CopyProductUserExternalAccountByIndex=" ORIGINAL_DLL ".EOS_Connect_CopyProductUserExternalAccountByIndex,@139")
#pragma comment(linker, "/export:EOS_Connect_CopyProductUserInfo=" ORIGINAL_DLL ".EOS_Connect_CopyProductUserInfo,@140")
#pragma comment(linker, "/export:EOS_Connect_CreateUser=" ORIGINAL_DLL ".EOS_Connect_CreateUser,@142")
#pragma comment(linker, "/export:EOS_Connect_DeleteDeviceId=" ORIGINAL_DLL ".EOS_Connect_DeleteDeviceId,@143")
#pragma comment(linker, "/export:EOS_Connect_ExternalAccountInfo_Release=" ORIGINAL_DLL ".EOS_Connect_ExternalAccountInfo_Release,@144")
#pragma comment(linker, "/export:EOS_Connect_GetExternalAccountMapping=" ORIGINAL_DLL ".EOS_Connect_GetExternalAccountMapping,@145")
#pragma comment(linker, "/export:EOS_Connect_GetLoggedInUserByIndex=" ORIGINAL_DLL ".EOS_Connect_GetLoggedInUserByIndex,@146")
#pragma comment(linker, "/export:EOS_Connect_GetLoggedInUsersCount=" ORIGINAL_DLL ".EOS_Connect_GetLoggedInUsersCount,@147")
#pragma comment(linker, "/export:EOS_Connect_GetLoginStatus=" ORIGINAL_DLL ".EOS_Connect_GetLoginStatus,@148")
#pragma comment(linker, "/export:EOS_Connect_GetProductUserExternalAccountCount=" ORIGINAL_DLL ".EOS_Connect_GetProductUserExternalAccountCount,@149")
#pragma comment(linker, "/export:EOS_Connect_GetProductUserIdMapping=" ORIGINAL_DLL ".EOS_Connect_GetProductUserIdMapping,@150")
#pragma comment(linker, "/export:EOS_Connect_IdToken_Release=" ORIGINAL_DLL ".EOS_Connect_IdToken_Release,@151")
#pragma comment(linker, "/export:EOS_Connect_LinkAccount=" ORIGINAL_DLL ".EOS_Connect_LinkAccount,@152")
#pragma comment(linker, "/export:EOS_Connect_QueryExternalAccountMappings=" ORIGINAL_DLL ".EOS_Connect_QueryExternalAccountMappings,@154")
#pragma comment(linker, "/export:EOS_Connect_QueryProductUserIdMappings=" ORIGINAL_DLL ".EOS_Connect_QueryProductUserIdMappings,@155")
#pragma comment(linker, "/export:EOS_Connect_RemoveNotifyAuthExpiration=" ORIGINAL_DLL ".EOS_Connect_RemoveNotifyAuthExpiration,@156")
#pragma comment(linker, "/export:EOS_Connect_RemoveNotifyLoginStatusChanged=" ORIGINAL_DLL ".EOS_Connect_RemoveNotifyLoginStatusChanged,@157")
#pragma comment(linker, "/export:EOS_Connect_TransferDeviceIdAccount=" ORIGINAL_DLL ".EOS_Connect_TransferDeviceIdAccount,@158")
#pragma comment(linker, "/export:EOS_Connect_UnlinkAccount=" ORIGINAL_DLL ".EOS_Connect_UnlinkAccount,@159")
#pragma comment(linker, "/export:EOS_Connect_VerifyIdToken=" ORIGINAL_DLL ".EOS_Connect_VerifyIdToken,@160")
#pragma comment(linker, "/export:EOS_ContinuanceToken_ToString=" ORIGINAL_DLL ".EOS_ContinuanceToken_ToString,@161")
#pragma comment(linker, "/export:EOS_CustomInvites_AcceptRequestToJoin=" ORIGINAL_DLL ".EOS_CustomInvites_AcceptRequestToJoin,@162")
#pragma comment(linker, "/export:EOS_CustomInvites_AddNotifyCustomInviteAccepted=" ORIGINAL_DLL ".EOS_CustomInvites_AddNotifyCustomInviteAccepted,@163")
#pragma comment(linker, "/export:EOS_CustomInvites_AddNotifyCustomInviteReceived=" ORIGINAL_DLL ".EOS_CustomInvites_AddNotifyCustomInviteReceived,@164")
#pragma comment(linker, "/export:EOS_CustomInvites_AddNotifyCustomInviteRejected=" ORIGINAL_DLL ".EOS_CustomInvites_AddNotifyCustomInviteRejected,@165")
#pragma comment(linker, "/export:EOS_CustomInvites_AddNotifyRequestToJoinAccepted=" ORIGINAL_DLL ".EOS_CustomInvites_AddNotifyRequestToJoinAccepted,@166")
#pragma comment(linker, "/export:EOS_CustomInvites_AddNotifyRequestToJoinReceived=" ORIGINAL_DLL ".EOS_CustomInvites_AddNotifyRequestToJoinReceived,@167")
#pragma comment(linker, "/export:EOS_CustomInvites_AddNotifyRequestToJoinRejected=" ORIGINAL_DLL ".EOS_CustomInvites_AddNotifyRequestToJoinRejected,@168")
#pragma comment(linker, "/export:EOS_CustomInvites_AddNotifyRequestToJoinResponseReceived=" ORIGINAL_DLL ".EOS_CustomInvites_AddNotifyRequestToJoinResponseReceived,@169")
#pragma comment(linker, "/export:EOS_CustomInvites_AddNotifySendCustomNativeInviteRequested=" ORIGINAL_DLL ".EOS_CustomInvites_AddNotifySendCustomNativeInviteRequested,@170")
#pragma comment(linker, "/export:EOS_CustomInvites_FinalizeInvite=" ORIGINAL_DLL ".EOS_CustomInvites_FinalizeInvite,@171")
#pragma comment(linker, "/export:EOS_CustomInvites_RejectRequestToJoin=" ORIGINAL_DLL ".EOS_CustomInvites_RejectRequestToJoin,@172")
#pragma comment(linker, "/export:EOS_CustomInvites_RemoveNotifyCustomInviteAccepted=" ORIGINAL_DLL ".EOS_CustomInvites_RemoveNotifyCustomInviteAccepted,@173")
#pragma comment(linker, "/export:EOS_CustomInvites_RemoveNotifyCustomInviteReceived=" ORIGINAL_DLL ".EOS_CustomInvites_RemoveNotifyCustomInviteReceived,@174")
#pragma comment(linker, "/export:EOS_CustomInvites_RemoveNotifyCustomInviteRejected=" ORIGINAL_DLL ".EOS_CustomInvites_RemoveNotifyCustomInviteRejected,@175")
#pragma comment(linker, "/export:EOS_CustomInvites_RemoveNotifyRequestToJoinAccepted=" ORIGINAL_DLL ".EOS_CustomInvites_RemoveNotifyRequestToJoinAccepted,@176")
#pragma comment(linker, "/export:EOS_CustomInvites_RemoveNotifyRequestToJoinReceived=" ORIGINAL_DLL ".EOS_CustomInvites_RemoveNotifyRequestToJoinReceived,@177")
#pragma comment(linker, "/export:EOS_CustomInvites_RemoveNotifyRequestToJoinRejected=" ORIGINAL_DLL ".EOS_CustomInvites_RemoveNotifyRequestToJoinRejected,@178")
#pragma comment(linker, "/export:EOS_CustomInvites_RemoveNotifyRequestToJoinResponseReceived=" ORIGINAL_DLL ".EOS_CustomInvites_RemoveNotifyRequestToJoinResponseReceived,@179")
#pragma comment(linker, "/export:EOS_CustomInvites_RemoveNotifySendCustomNativeInviteRequested=" ORIGINAL_DLL ".EOS_CustomInvites_RemoveNotifySendCustomNativeInviteRequested,@180")
#pragma comment(linker, "/export:EOS_CustomInvites_SendCustomInvite=" ORIGINAL_DLL ".EOS_CustomInvites_SendCustomInvite,@181")
#pragma comment(linker, "/export:EOS_CustomInvites_SendRequestToJoin=" ORIGINAL_DLL ".EOS_CustomInvites_SendRequestToJoin,@182")
#pragma comment(linker, "/export:EOS_CustomInvites_SetCustomInvite=" ORIGINAL_DLL ".EOS_CustomInvites_SetCustomInvite,@183")
#pragma comment(linker, "/export:EOS_EApplicationStatus_ToString=" ORIGINAL_DLL ".EOS_EApplicationStatus_ToString,@184")
#pragma comment(linker, "/export:EOS_ENetworkStatus_ToString=" ORIGINAL_DLL ".EOS_ENetworkStatus_ToString,@185")
#pragma comment(linker, "/export:EOS_EResult_IsOperationComplete=" ORIGINAL_DLL ".EOS_EResult_IsOperationComplete,@186")
#pragma comment(linker, "/export:EOS_EResult_ToString=" ORIGINAL_DLL ".EOS_EResult_ToString,@187")
#pragma comment(linker, "/export:EOS_Ecom_CatalogItem_Release=" ORIGINAL_DLL ".EOS_Ecom_CatalogItem_Release,@188")
#pragma comment(linker, "/export:EOS_Ecom_CatalogOffer_Release=" ORIGINAL_DLL ".EOS_Ecom_CatalogOffer_Release,@189")
#pragma comment(linker, "/export:EOS_Ecom_CatalogRelease_Release=" ORIGINAL_DLL ".EOS_Ecom_CatalogRelease_Release,@190")
#pragma comment(linker, "/export:EOS_Ecom_Checkout=" ORIGINAL_DLL ".EOS_Ecom_Checkout,@191")
#pragma comment(linker, "/export:EOS_Ecom_CopyEntitlementById=" ORIGINAL_DLL ".EOS_Ecom_CopyEntitlementById,@192")
#pragma comment(linker, "/export:EOS_Ecom_CopyEntitlementByIndex=" ORIGINAL_DLL ".EOS_Ecom_CopyEntitlementByIndex,@193")
#pragma comment(linker, "/export:EOS_Ecom_CopyEntitlementByNameAndIndex=" ORIGINAL_DLL ".EOS_Ecom_CopyEntitlementByNameAndIndex,@194")
#pragma comment(linker, "/export:EOS_Ecom_CopyItemById=" ORIGINAL_DLL ".EOS_Ecom_CopyItemById,@195")
#pragma comment(linker, "/export:EOS_Ecom_CopyItemImageInfoByIndex=" ORIGINAL_DLL ".EOS_Ecom_CopyItemImageInfoByIndex,@196")
#pragma comment(linker, "/export:EOS_Ecom_CopyItemReleaseByIndex=" ORIGINAL_DLL ".EOS_Ecom_CopyItemReleaseByIndex,@197")
#pragma comment(linker, "/export:EOS_Ecom_CopyLastRedeemedEntitlementByIndex=" ORIGINAL_DLL ".EOS_Ecom_CopyLastRedeemedEntitlementByIndex,@198")
#pragma comment(linker, "/export:EOS_Ecom_CopyOfferById=" ORIGINAL_DLL ".EOS_Ecom_CopyOfferById,@199")
#pragma comment(linker, "/export:EOS_Ecom_CopyOfferByIndex=" ORIGINAL_DLL ".EOS_Ecom_CopyOfferByIndex,@200")
#pragma comment(linker, "/export:EOS_Ecom_CopyOfferImageInfoByIndex=" ORIGINAL_DLL ".EOS_Ecom_CopyOfferImageInfoByIndex,@201")
#pragma comment(linker, "/export:EOS_Ecom_CopyOfferItemByIndex=" ORIGINAL_DLL ".EOS_Ecom_CopyOfferItemByIndex,@202")
#pragma comment(linker, "/export:EOS_Ecom_CopyTransactionById=" ORIGINAL_DLL ".EOS_Ecom_CopyTransactionById,@203")
#pragma comment(linker, "/export:EOS_Ecom_CopyTransactionByIndex=" ORIGINAL_DLL ".EOS_Ecom_CopyTransactionByIndex,@204")
#pragma comment(linker, "/export:EOS_Ecom_Entitlement_Release=" ORIGINAL_DLL ".EOS_Ecom_Entitlement_Release,@205")
#pragma comment(linker, "/export:EOS_Ecom_GetEntitlementsByNameCount=" ORIGINAL_DLL ".EOS_Ecom_GetEntitlementsByNameCount,@206")
#pragma comment(linker, "/export:EOS_Ecom_GetEntitlementsCount=" ORIGINAL_DLL ".EOS_Ecom_GetEntitlementsCount,@207")
#pragma comment(linker, "/export:EOS_Ecom_GetItemImageInfoCount=" ORIGINAL_DLL ".EOS_Ecom_GetItemImageInfoCount,@208")
#pragma comment(linker, "/export:EOS_Ecom_GetItemReleaseCount=" ORIGINAL_DLL ".EOS_Ecom_GetItemReleaseCount,@209")
#pragma comment(linker, "/export:EOS_Ecom_GetLastRedeemedEntitlementsCount=" ORIGINAL_DLL ".EOS_Ecom_GetLastRedeemedEntitlementsCount,@210")
#pragma comment(linker, "/export:EOS_Ecom_GetOfferCount=" ORIGINAL_DLL ".EOS_Ecom_GetOfferCount,@211")
#pragma comment(linker, "/export:EOS_Ecom_GetOfferImageInfoCount=" ORIGINAL_DLL ".EOS_Ecom_GetOfferImageInfoCount,@212")
#pragma comment(linker, "/export:EOS_Ecom_GetOfferItemCount=" ORIGINAL_DLL ".EOS_Ecom_GetOfferItemCount,@213")
#pragma comment(linker, "/export:EOS_Ecom_GetTransactionCount=" ORIGINAL_DLL ".EOS_Ecom_GetTransactionCount,@214")
#pragma comment(linker, "/export:EOS_Ecom_KeyImageInfo_Release=" ORIGINAL_DLL ".EOS_Ecom_KeyImageInfo_Release,@215")
#pragma comment(linker, "/export:EOS_Ecom_QueryEntitlementToken=" ORIGINAL_DLL ".EOS_Ecom_QueryEntitlementToken,@216")
#pragma comment(linker, "/export:EOS_Ecom_QueryEntitlements=" ORIGINAL_DLL ".EOS_Ecom_QueryEntitlements,@217")
#pragma comment(linker, "/export:EOS_Ecom_QueryOffers=" ORIGINAL_DLL ".EOS_Ecom_QueryOffers,@218")
// #pragma comment(linker, "/export:EOS_Ecom_QueryOwnership=" ORIGINAL_DLL ".EOS_Ecom_QueryOwnership,@219")
#pragma comment(linker, "/export:EOS_Ecom_QueryOwnershipBySandboxIds=" ORIGINAL_DLL ".EOS_Ecom_QueryOwnershipBySandboxIds,@220")
#pragma comment(linker, "/export:EOS_Ecom_QueryOwnershipToken=" ORIGINAL_DLL ".EOS_Ecom_QueryOwnershipToken,@221")
#pragma comment(linker, "/export:EOS_Ecom_RedeemEntitlements=" ORIGINAL_DLL ".EOS_Ecom_RedeemEntitlements,@222")
#pragma comment(linker, "/export:EOS_Ecom_Transaction_CopyEntitlementByIndex=" ORIGINAL_DLL ".EOS_Ecom_Transaction_CopyEntitlementByIndex,@223")
#pragma comment(linker, "/export:EOS_Ecom_Transaction_GetEntitlementsCount=" ORIGINAL_DLL ".EOS_Ecom_Transaction_GetEntitlementsCount,@224")
#pragma comment(linker, "/export:EOS_Ecom_Transaction_GetTransactionId=" ORIGINAL_DLL ".EOS_Ecom_Transaction_GetTransactionId,@225")
#pragma comment(linker, "/export:EOS_Ecom_Transaction_Release=" ORIGINAL_DLL ".EOS_Ecom_Transaction_Release,@226")
#pragma comment(linker, "/export:EOS_EndScopeEvent=" ORIGINAL_DLL ".EOS_EndScopeEvent,@227")
#pragma comment(linker, "/export:EOS_EpicAccountId_FromString=" ORIGINAL_DLL ".EOS_EpicAccountId_FromString,@228")
#pragma comment(linker, "/export:EOS_EpicAccountId_IsValid=" ORIGINAL_DLL ".EOS_EpicAccountId_IsValid,@229")
#pragma comment(linker, "/export:EOS_EpicAccountId_ToString=" ORIGINAL_DLL ".EOS_EpicAccountId_ToString,@230")
#pragma comment(linker, "/export:EOS_Friends_AcceptInvite=" ORIGINAL_DLL ".EOS_Friends_AcceptInvite,@231")
#pragma comment(linker, "/export:EOS_Friends_AddNotifyBlockedUsersUpdate=" ORIGINAL_DLL ".EOS_Friends_AddNotifyBlockedUsersUpdate,@232")
#pragma comment(linker, "/export:EOS_Friends_AddNotifyFriendsUpdate=" ORIGINAL_DLL ".EOS_Friends_AddNotifyFriendsUpdate,@233")
#pragma comment(linker, "/export:EOS_Friends_GetBlockedUserAtIndex=" ORIGINAL_DLL ".EOS_Friends_GetBlockedUserAtIndex,@234")
#pragma comment(linker, "/export:EOS_Friends_GetBlockedUsersCount=" ORIGINAL_DLL ".EOS_Friends_GetBlockedUsersCount,@235")
#pragma comment(linker, "/export:EOS_Friends_GetFriendAtIndex=" ORIGINAL_DLL ".EOS_Friends_GetFriendAtIndex,@236")
#pragma comment(linker, "/export:EOS_Friends_GetFriendsCount=" ORIGINAL_DLL ".EOS_Friends_GetFriendsCount,@237")
#pragma comment(linker, "/export:EOS_Friends_GetStatus=" ORIGINAL_DLL ".EOS_Friends_GetStatus,@238")
#pragma comment(linker, "/export:EOS_Friends_QueryFriends=" ORIGINAL_DLL ".EOS_Friends_QueryFriends,@239")
#pragma comment(linker, "/export:EOS_Friends_RejectInvite=" ORIGINAL_DLL ".EOS_Friends_RejectInvite,@240")
#pragma comment(linker, "/export:EOS_Friends_RemoveNotifyBlockedUsersUpdate=" ORIGINAL_DLL ".EOS_Friends_RemoveNotifyBlockedUsersUpdate,@241")
#pragma comment(linker, "/export:EOS_Friends_RemoveNotifyFriendsUpdate=" ORIGINAL_DLL ".EOS_Friends_RemoveNotifyFriendsUpdate,@242")
#pragma comment(linker, "/export:EOS_Friends_SendInvite=" ORIGINAL_DLL ".EOS_Friends_SendInvite,@243")
#pragma comment(linker, "/export:EOS_GetVersion=" ORIGINAL_DLL ".EOS_GetVersion,@244")
#pragma comment(linker, "/export:EOS_IntegratedPlatformOptionsContainer_Add=" ORIGINAL_DLL ".EOS_IntegratedPlatformOptionsContainer_Add,@246")
#pragma comment(linker, "/export:EOS_IntegratedPlatformOptionsContainer_Release=" ORIGINAL_DLL ".EOS_IntegratedPlatformOptionsContainer_Release,@247")
#pragma comment(linker, "/export:EOS_IntegratedPlatform_AddNotifyUserLoginStatusChanged=" ORIGINAL_DLL ".EOS_IntegratedPlatform_AddNotifyUserLoginStatusChanged,@248")
#pragma comment(linker, "/export:EOS_IntegratedPlatform_ClearUserPreLogoutCallback=" ORIGINAL_DLL ".EOS_IntegratedPlatform_ClearUserPreLogoutCallback,@249")
#pragma comment(linker, "/export:EOS_IntegratedPlatform_CreateIntegratedPlatformOptionsContainer=" ORIGINAL_DLL ".EOS_IntegratedPlatform_CreateIntegratedPlatformOptionsContainer,@250")
#pragma comment(linker, "/export:EOS_IntegratedPlatform_FinalizeDeferredUserLogout=" ORIGINAL_DLL ".EOS_IntegratedPlatform_FinalizeDeferredUserLogout,@251")
#pragma comment(linker, "/export:EOS_IntegratedPlatform_RemoveNotifyUserLoginStatusChanged=" ORIGINAL_DLL ".EOS_IntegratedPlatform_RemoveNotifyUserLoginStatusChanged,@252")
#pragma comment(linker, "/export:EOS_IntegratedPlatform_SetUserLoginStatus=" ORIGINAL_DLL ".EOS_IntegratedPlatform_SetUserLoginStatus,@253")
#pragma comment(linker, "/export:EOS_IntegratedPlatform_SetUserPreLogoutCallback=" ORIGINAL_DLL ".EOS_IntegratedPlatform_SetUserPreLogoutCallback,@254")
#pragma comment(linker, "/export:EOS_KWS_AddNotifyPermissionsUpdateReceived=" ORIGINAL_DLL ".EOS_KWS_AddNotifyPermissionsUpdateReceived,@255")
#pragma comment(linker, "/export:EOS_KWS_CopyPermissionByIndex=" ORIGINAL_DLL ".EOS_KWS_CopyPermissionByIndex,@256")
#pragma comment(linker, "/export:EOS_KWS_CreateUser=" ORIGINAL_DLL ".EOS_KWS_CreateUser,@257")
#pragma comment(linker, "/export:EOS_KWS_GetPermissionByKey=" ORIGINAL_DLL ".EOS_KWS_GetPermissionByKey,@258")
#pragma comment(linker, "/export:EOS_KWS_GetPermissionsCount=" ORIGINAL_DLL ".EOS_KWS_GetPermissionsCount,@259")
#pragma comment(linker, "/export:EOS_KWS_PermissionStatus_Release=" ORIGINAL_DLL ".EOS_KWS_PermissionStatus_Release,@260")
#pragma comment(linker, "/export:EOS_KWS_QueryAgeGate=" ORIGINAL_DLL ".EOS_KWS_QueryAgeGate,@261")
#pragma comment(linker, "/export:EOS_KWS_QueryPermissions=" ORIGINAL_DLL ".EOS_KWS_QueryPermissions,@262")
#pragma comment(linker, "/export:EOS_KWS_RemoveNotifyPermissionsUpdateReceived=" ORIGINAL_DLL ".EOS_KWS_RemoveNotifyPermissionsUpdateReceived,@263")
#pragma comment(linker, "/export:EOS_KWS_RequestPermissions=" ORIGINAL_DLL ".EOS_KWS_RequestPermissions,@264")
#pragma comment(linker, "/export:EOS_KWS_UpdateParentEmail=" ORIGINAL_DLL ".EOS_KWS_UpdateParentEmail,@265")
#pragma comment(linker, "/export:EOS_Leaderboards_CopyLeaderboardDefinitionByIndex=" ORIGINAL_DLL ".EOS_Leaderboards_CopyLeaderboardDefinitionByIndex,@266")
#pragma comment(linker, "/export:EOS_Leaderboards_CopyLeaderboardDefinitionByLeaderboardId=" ORIGINAL_DLL ".EOS_Leaderboards_CopyLeaderboardDefinitionByLeaderboardId,@267")
#pragma comment(linker, "/export:EOS_Leaderboards_CopyLeaderboardRecordByIndex=" ORIGINAL_DLL ".EOS_Leaderboards_CopyLeaderboardRecordByIndex,@268")
#pragma comment(linker, "/export:EOS_Leaderboards_CopyLeaderboardRecordByUserId=" ORIGINAL_DLL ".EOS_Leaderboards_CopyLeaderboardRecordByUserId,@269")
#pragma comment(linker, "/export:EOS_Leaderboards_CopyLeaderboardUserScoreByIndex=" ORIGINAL_DLL ".EOS_Leaderboards_CopyLeaderboardUserScoreByIndex,@270")
#pragma comment(linker, "/export:EOS_Leaderboards_CopyLeaderboardUserScoreByUserId=" ORIGINAL_DLL ".EOS_Leaderboards_CopyLeaderboardUserScoreByUserId,@271")
#pragma comment(linker, "/export:EOS_Leaderboards_Definition_Release=" ORIGINAL_DLL ".EOS_Leaderboards_Definition_Release,@272")
#pragma comment(linker, "/export:EOS_Leaderboards_GetLeaderboardDefinitionCount=" ORIGINAL_DLL ".EOS_Leaderboards_GetLeaderboardDefinitionCount,@273")
#pragma comment(linker, "/export:EOS_Leaderboards_GetLeaderboardRecordCount=" ORIGINAL_DLL ".EOS_Leaderboards_GetLeaderboardRecordCount,@274")
#pragma comment(linker, "/export:EOS_Leaderboards_GetLeaderboardUserScoreCount=" ORIGINAL_DLL ".EOS_Leaderboards_GetLeaderboardUserScoreCount,@275")
#pragma comment(linker, "/export:EOS_Leaderboards_LeaderboardDefinition_Release=" ORIGINAL_DLL ".EOS_Leaderboards_LeaderboardDefinition_Release,@276")
#pragma comment(linker, "/export:EOS_Leaderboards_LeaderboardRecord_Release=" ORIGINAL_DLL ".EOS_Leaderboards_LeaderboardRecord_Release,@277")
#pragma comment(linker, "/export:EOS_Leaderboards_LeaderboardUserScore_Release=" ORIGINAL_DLL ".EOS_Leaderboards_LeaderboardUserScore_Release,@278")
#pragma comment(linker, "/export:EOS_Leaderboards_QueryLeaderboardDefinitions=" ORIGINAL_DLL ".EOS_Leaderboards_QueryLeaderboardDefinitions,@279")
#pragma comment(linker, "/export:EOS_Leaderboards_QueryLeaderboardRanks=" ORIGINAL_DLL ".EOS_Leaderboards_QueryLeaderboardRanks,@280")
#pragma comment(linker, "/export:EOS_Leaderboards_QueryLeaderboardUserScores=" ORIGINAL_DLL ".EOS_Leaderboards_QueryLeaderboardUserScores,@281")
#pragma comment(linker, "/export:EOS_LobbyDetails_CopyAttributeByIndex=" ORIGINAL_DLL ".EOS_LobbyDetails_CopyAttributeByIndex,@282")
#pragma comment(linker, "/export:EOS_LobbyDetails_CopyAttributeByKey=" ORIGINAL_DLL ".EOS_LobbyDetails_CopyAttributeByKey,@283")
#pragma comment(linker, "/export:EOS_LobbyDetails_CopyInfo=" ORIGINAL_DLL ".EOS_LobbyDetails_CopyInfo,@284")
#pragma comment(linker, "/export:EOS_LobbyDetails_CopyMemberAttributeByIndex=" ORIGINAL_DLL ".EOS_LobbyDetails_CopyMemberAttributeByIndex,@285")
#pragma comment(linker, "/export:EOS_LobbyDetails_CopyMemberAttributeByKey=" ORIGINAL_DLL ".EOS_LobbyDetails_CopyMemberAttributeByKey,@286")
#pragma comment(linker, "/export:EOS_LobbyDetails_CopyMemberInfo=" ORIGINAL_DLL ".EOS_LobbyDetails_CopyMemberInfo,@287")
#pragma comment(linker, "/export:EOS_LobbyDetails_GetAttributeCount=" ORIGINAL_DLL ".EOS_LobbyDetails_GetAttributeCount,@288")
#pragma comment(linker, "/export:EOS_LobbyDetails_GetLobbyOwner=" ORIGINAL_DLL ".EOS_LobbyDetails_GetLobbyOwner,@289")
#pragma comment(linker, "/export:EOS_LobbyDetails_GetMemberAttributeCount=" ORIGINAL_DLL ".EOS_LobbyDetails_GetMemberAttributeCount,@290")
#pragma comment(linker, "/export:EOS_LobbyDetails_GetMemberByIndex=" ORIGINAL_DLL ".EOS_LobbyDetails_GetMemberByIndex,@291")
#pragma comment(linker, "/export:EOS_LobbyDetails_GetMemberCount=" ORIGINAL_DLL ".EOS_LobbyDetails_GetMemberCount,@292")
#pragma comment(linker, "/export:EOS_LobbyDetails_Info_Release=" ORIGINAL_DLL ".EOS_LobbyDetails_Info_Release,@293")
#pragma comment(linker, "/export:EOS_LobbyDetails_MemberInfo_Release=" ORIGINAL_DLL ".EOS_LobbyDetails_MemberInfo_Release,@294")
#pragma comment(linker, "/export:EOS_LobbyDetails_Release=" ORIGINAL_DLL ".EOS_LobbyDetails_Release,@295")
#pragma comment(linker, "/export:EOS_LobbyModification_AddAttribute=" ORIGINAL_DLL ".EOS_LobbyModification_AddAttribute,@296")
#pragma comment(linker, "/export:EOS_LobbyModification_AddMemberAttribute=" ORIGINAL_DLL ".EOS_LobbyModification_AddMemberAttribute,@297")
#pragma comment(linker, "/export:EOS_LobbyModification_Release=" ORIGINAL_DLL ".EOS_LobbyModification_Release,@298")
#pragma comment(linker, "/export:EOS_LobbyModification_RemoveAttribute=" ORIGINAL_DLL ".EOS_LobbyModification_RemoveAttribute,@299")
#pragma comment(linker, "/export:EOS_LobbyModification_RemoveMemberAttribute=" ORIGINAL_DLL ".EOS_LobbyModification_RemoveMemberAttribute,@300")
#pragma comment(linker, "/export:EOS_LobbyModification_SetAllowedPlatformIds=" ORIGINAL_DLL ".EOS_LobbyModification_SetAllowedPlatformIds,@301")
#pragma comment(linker, "/export:EOS_LobbyModification_SetBucketId=" ORIGINAL_DLL ".EOS_LobbyModification_SetBucketId,@302")
#pragma comment(linker, "/export:EOS_LobbyModification_SetInvitesAllowed=" ORIGINAL_DLL ".EOS_LobbyModification_SetInvitesAllowed,@303")
#pragma comment(linker, "/export:EOS_LobbyModification_SetMaxMembers=" ORIGINAL_DLL ".EOS_LobbyModification_SetMaxMembers,@304")
#pragma comment(linker, "/export:EOS_LobbyModification_SetPermissionLevel=" ORIGINAL_DLL ".EOS_LobbyModification_SetPermissionLevel,@305")
#pragma comment(linker, "/export:EOS_LobbySearch_CopySearchResultByIndex=" ORIGINAL_DLL ".EOS_LobbySearch_CopySearchResultByIndex,@306")
#pragma comment(linker, "/export:EOS_LobbySearch_Find=" ORIGINAL_DLL ".EOS_LobbySearch_Find,@307")
#pragma comment(linker, "/export:EOS_LobbySearch_GetSearchResultCount=" ORIGINAL_DLL ".EOS_LobbySearch_GetSearchResultCount,@308")
#pragma comment(linker, "/export:EOS_LobbySearch_Release=" ORIGINAL_DLL ".EOS_LobbySearch_Release,@309")
#pragma comment(linker, "/export:EOS_LobbySearch_RemoveParameter=" ORIGINAL_DLL ".EOS_LobbySearch_RemoveParameter,@310")
#pragma comment(linker, "/export:EOS_LobbySearch_SetLobbyId=" ORIGINAL_DLL ".EOS_LobbySearch_SetLobbyId,@311")
#pragma comment(linker, "/export:EOS_LobbySearch_SetMaxResults=" ORIGINAL_DLL ".EOS_LobbySearch_SetMaxResults,@312")
#pragma comment(linker, "/export:EOS_LobbySearch_SetParameter=" ORIGINAL_DLL ".EOS_LobbySearch_SetParameter,@313")
#pragma comment(linker, "/export:EOS_LobbySearch_SetTargetUserId=" ORIGINAL_DLL ".EOS_LobbySearch_SetTargetUserId,@314")
#pragma comment(linker, "/export:EOS_Lobby_AddNotifyJoinLobbyAccepted=" ORIGINAL_DLL ".EOS_Lobby_AddNotifyJoinLobbyAccepted,@315")
#pragma comment(linker, "/export:EOS_Lobby_AddNotifyLeaveLobbyRequested=" ORIGINAL_DLL ".EOS_Lobby_AddNotifyLeaveLobbyRequested,@316")
#pragma comment(linker, "/export:EOS_Lobby_AddNotifyLobbyInviteAccepted=" ORIGINAL_DLL ".EOS_Lobby_AddNotifyLobbyInviteAccepted,@317")
#pragma comment(linker, "/export:EOS_Lobby_AddNotifyLobbyInviteReceived=" ORIGINAL_DLL ".EOS_Lobby_AddNotifyLobbyInviteReceived,@318")
#pragma comment(linker, "/export:EOS_Lobby_AddNotifyLobbyInviteRejected=" ORIGINAL_DLL ".EOS_Lobby_AddNotifyLobbyInviteRejected,@319")
#pragma comment(linker, "/export:EOS_Lobby_AddNotifyLobbyMemberStatusReceived=" ORIGINAL_DLL ".EOS_Lobby_AddNotifyLobbyMemberStatusReceived,@320")
#pragma comment(linker, "/export:EOS_Lobby_AddNotifyLobbyMemberUpdateReceived=" ORIGINAL_DLL ".EOS_Lobby_AddNotifyLobbyMemberUpdateReceived,@321")
#pragma comment(linker, "/export:EOS_Lobby_AddNotifyLobbyUpdateReceived=" ORIGINAL_DLL ".EOS_Lobby_AddNotifyLobbyUpdateReceived,@322")
#pragma comment(linker, "/export:EOS_Lobby_AddNotifyRTCRoomConnectionChanged=" ORIGINAL_DLL ".EOS_Lobby_AddNotifyRTCRoomConnectionChanged,@323")
#pragma comment(linker, "/export:EOS_Lobby_AddNotifySendLobbyNativeInviteRequested=" ORIGINAL_DLL ".EOS_Lobby_AddNotifySendLobbyNativeInviteRequested,@324")
#pragma comment(linker, "/export:EOS_Lobby_Attribute_Release=" ORIGINAL_DLL ".EOS_Lobby_Attribute_Release,@325")
#pragma comment(linker, "/export:EOS_Lobby_CopyLobbyDetailsHandle=" ORIGINAL_DLL ".EOS_Lobby_CopyLobbyDetailsHandle,@326")
#pragma comment(linker, "/export:EOS_Lobby_CopyLobbyDetailsHandleByInviteId=" ORIGINAL_DLL ".EOS_Lobby_CopyLobbyDetailsHandleByInviteId,@327")
#pragma comment(linker, "/export:EOS_Lobby_CopyLobbyDetailsHandleByUiEventId=" ORIGINAL_DLL ".EOS_Lobby_CopyLobbyDetailsHandleByUiEventId,@328")
#pragma comment(linker, "/export:EOS_Lobby_CreateLobbySearch=" ORIGINAL_DLL ".EOS_Lobby_CreateLobbySearch,@330")
#pragma comment(linker, "/export:EOS_Lobby_DestroyLobby=" ORIGINAL_DLL ".EOS_Lobby_DestroyLobby,@331")
#pragma comment(linker, "/export:EOS_Lobby_GetConnectString=" ORIGINAL_DLL ".EOS_Lobby_GetConnectString,@332")
#pragma comment(linker, "/export:EOS_Lobby_GetInviteCount=" ORIGINAL_DLL ".EOS_Lobby_GetInviteCount,@333")
#pragma comment(linker, "/export:EOS_Lobby_GetInviteIdByIndex=" ORIGINAL_DLL ".EOS_Lobby_GetInviteIdByIndex,@334")
#pragma comment(linker, "/export:EOS_Lobby_GetRTCRoomName=" ORIGINAL_DLL ".EOS_Lobby_GetRTCRoomName,@335")
#pragma comment(linker, "/export:EOS_Lobby_HardMuteMember=" ORIGINAL_DLL ".EOS_Lobby_HardMuteMember,@336")
#pragma comment(linker, "/export:EOS_Lobby_IsRTCRoomConnected=" ORIGINAL_DLL ".EOS_Lobby_IsRTCRoomConnected,@337")
#pragma comment(linker, "/export:EOS_Lobby_JoinLobby=" ORIGINAL_DLL ".EOS_Lobby_JoinLobby,@338")
#pragma comment(linker, "/export:EOS_Lobby_JoinLobbyById=" ORIGINAL_DLL ".EOS_Lobby_JoinLobbyById,@339")
#pragma comment(linker, "/export:EOS_Lobby_KickMember=" ORIGINAL_DLL ".EOS_Lobby_KickMember,@340")
#pragma comment(linker, "/export:EOS_Lobby_LeaveLobby=" ORIGINAL_DLL ".EOS_Lobby_LeaveLobby,@341")
#pragma comment(linker, "/export:EOS_Lobby_ParseConnectString=" ORIGINAL_DLL ".EOS_Lobby_ParseConnectString,@342")
#pragma comment(linker, "/export:EOS_Lobby_PromoteMember=" ORIGINAL_DLL ".EOS_Lobby_PromoteMember,@343")
#pragma comment(linker, "/export:EOS_Lobby_QueryInvites=" ORIGINAL_DLL ".EOS_Lobby_QueryInvites,@344")
#pragma comment(linker, "/export:EOS_Lobby_RejectInvite=" ORIGINAL_DLL ".EOS_Lobby_RejectInvite,@345")
#pragma comment(linker, "/export:EOS_Lobby_RemoveNotifyJoinLobbyAccepted=" ORIGINAL_DLL ".EOS_Lobby_RemoveNotifyJoinLobbyAccepted,@346")
#pragma comment(linker, "/export:EOS_Lobby_RemoveNotifyLeaveLobbyRequested=" ORIGINAL_DLL ".EOS_Lobby_RemoveNotifyLeaveLobbyRequested,@347")
#pragma comment(linker, "/export:EOS_Lobby_RemoveNotifyLobbyInviteAccepted=" ORIGINAL_DLL ".EOS_Lobby_RemoveNotifyLobbyInviteAccepted,@348")
#pragma comment(linker, "/export:EOS_Lobby_RemoveNotifyLobbyInviteReceived=" ORIGINAL_DLL ".EOS_Lobby_RemoveNotifyLobbyInviteReceived,@349")
#pragma comment(linker, "/export:EOS_Lobby_RemoveNotifyLobbyInviteRejected=" ORIGINAL_DLL ".EOS_Lobby_RemoveNotifyLobbyInviteRejected,@350")
#pragma comment(linker, "/export:EOS_Lobby_RemoveNotifyLobbyMemberStatusReceived=" ORIGINAL_DLL ".EOS_Lobby_RemoveNotifyLobbyMemberStatusReceived,@351")
#pragma comment(linker, "/export:EOS_Lobby_RemoveNotifyLobbyMemberUpdateReceived=" ORIGINAL_DLL ".EOS_Lobby_RemoveNotifyLobbyMemberUpdateReceived,@352")
#pragma comment(linker, "/export:EOS_Lobby_RemoveNotifyLobbyUpdateReceived=" ORIGINAL_DLL ".EOS_Lobby_RemoveNotifyLobbyUpdateReceived,@353")
#pragma comment(linker, "/export:EOS_Lobby_RemoveNotifyRTCRoomConnectionChanged=" ORIGINAL_DLL ".EOS_Lobby_RemoveNotifyRTCRoomConnectionChanged,@354")
#pragma comment(linker, "/export:EOS_Lobby_RemoveNotifySendLobbyNativeInviteRequested=" ORIGINAL_DLL ".EOS_Lobby_RemoveNotifySendLobbyNativeInviteRequested,@355")
#pragma comment(linker, "/export:EOS_Lobby_SendInvite=" ORIGINAL_DLL ".EOS_Lobby_SendInvite,@356")
#pragma comment(linker, "/export:EOS_Lobby_UpdateLobby=" ORIGINAL_DLL ".EOS_Lobby_UpdateLobby,@357")
#pragma comment(linker, "/export:EOS_Lobby_UpdateLobbyModification=" ORIGINAL_DLL ".EOS_Lobby_UpdateLobbyModification,@358")
#pragma comment(linker, "/export:EOS_Logging_SetCallback=" ORIGINAL_DLL ".EOS_Logging_SetCallback,@359")
#pragma comment(linker, "/export:EOS_Logging_SetLogLevel=" ORIGINAL_DLL ".EOS_Logging_SetLogLevel,@360")
#pragma comment(linker, "/export:EOS_Mercury_Initialize=" ORIGINAL_DLL ".EOS_Mercury_Initialize,@361")
#pragma comment(linker, "/export:EOS_Mercury_Shutdown=" ORIGINAL_DLL ".EOS_Mercury_Shutdown,@362")
#pragma comment(linker, "/export:EOS_Mercury_Tick=" ORIGINAL_DLL ".EOS_Mercury_Tick,@363")
#pragma comment(linker, "/export:EOS_Metrics_BeginPlayerSession=" ORIGINAL_DLL ".EOS_Metrics_BeginPlayerSession,@364")
#pragma comment(linker, "/export:EOS_Metrics_EndPlayerSession=" ORIGINAL_DLL ".EOS_Metrics_EndPlayerSession,@365")
#pragma comment(linker, "/export:EOS_Mods_CopyModInfo=" ORIGINAL_DLL ".EOS_Mods_CopyModInfo,@366")
#pragma comment(linker, "/export:EOS_Mods_EnumerateMods=" ORIGINAL_DLL ".EOS_Mods_EnumerateMods,@367")
#pragma comment(linker, "/export:EOS_Mods_InstallMod=" ORIGINAL_DLL ".EOS_Mods_InstallMod,@368")
#pragma comment(linker, "/export:EOS_Mods_ModInfo_Release=" ORIGINAL_DLL ".EOS_Mods_ModInfo_Release,@369")
#pragma comment(linker, "/export:EOS_Mods_UninstallMod=" ORIGINAL_DLL ".EOS_Mods_UninstallMod,@370")
#pragma comment(linker, "/export:EOS_Mods_UpdateMod=" ORIGINAL_DLL ".EOS_Mods_UpdateMod,@371")
#pragma comment(linker, "/export:EOS_P2P_AcceptConnection=" ORIGINAL_DLL ".EOS_P2P_AcceptConnection,@372")
#pragma comment(linker, "/export:EOS_P2P_AddNotifyIncomingPacketQueueFull=" ORIGINAL_DLL ".EOS_P2P_AddNotifyIncomingPacketQueueFull,@373")
#pragma comment(linker, "/export:EOS_P2P_AddNotifyPeerConnectionClosed=" ORIGINAL_DLL ".EOS_P2P_AddNotifyPeerConnectionClosed,@374")
#pragma comment(linker, "/export:EOS_P2P_AddNotifyPeerConnectionEstablished=" ORIGINAL_DLL ".EOS_P2P_AddNotifyPeerConnectionEstablished,@375")
#pragma comment(linker, "/export:EOS_P2P_AddNotifyPeerConnectionInterrupted=" ORIGINAL_DLL ".EOS_P2P_AddNotifyPeerConnectionInterrupted,@376")
#pragma comment(linker, "/export:EOS_P2P_AddNotifyPeerConnectionRequest=" ORIGINAL_DLL ".EOS_P2P_AddNotifyPeerConnectionRequest,@377")
#pragma comment(linker, "/export:EOS_P2P_ClearPacketQueue=" ORIGINAL_DLL ".EOS_P2P_ClearPacketQueue,@378")
#pragma comment(linker, "/export:EOS_P2P_CloseConnection=" ORIGINAL_DLL ".EOS_P2P_CloseConnection,@379")
#pragma comment(linker, "/export:EOS_P2P_CloseConnections=" ORIGINAL_DLL ".EOS_P2P_CloseConnections,@380")
#pragma comment(linker, "/export:EOS_P2P_GetNATType=" ORIGINAL_DLL ".EOS_P2P_GetNATType,@381")
#pragma comment(linker, "/export:EOS_P2P_GetNextReceivedPacketSize=" ORIGINAL_DLL ".EOS_P2P_GetNextReceivedPacketSize,@382")
#pragma comment(linker, "/export:EOS_P2P_GetPacketQueueInfo=" ORIGINAL_DLL ".EOS_P2P_GetPacketQueueInfo,@383")
#pragma comment(linker, "/export:EOS_P2P_GetPortRange=" ORIGINAL_DLL ".EOS_P2P_GetPortRange,@384")
#pragma comment(linker, "/export:EOS_P2P_GetRelayControl=" ORIGINAL_DLL ".EOS_P2P_GetRelayControl,@385")
#pragma comment(linker, "/export:EOS_P2P_QueryNATType=" ORIGINAL_DLL ".EOS_P2P_QueryNATType,@386")
#pragma comment(linker, "/export:EOS_P2P_ReceivePacket=" ORIGINAL_DLL ".EOS_P2P_ReceivePacket,@387")
#pragma comment(linker, "/export:EOS_P2P_RemoveNotifyIncomingPacketQueueFull=" ORIGINAL_DLL ".EOS_P2P_RemoveNotifyIncomingPacketQueueFull,@388")
#pragma comment(linker, "/export:EOS_P2P_RemoveNotifyPeerConnectionClosed=" ORIGINAL_DLL ".EOS_P2P_RemoveNotifyPeerConnectionClosed,@389")
#pragma comment(linker, "/export:EOS_P2P_RemoveNotifyPeerConnectionEstablished=" ORIGINAL_DLL ".EOS_P2P_RemoveNotifyPeerConnectionEstablished,@390")
#pragma comment(linker, "/export:EOS_P2P_RemoveNotifyPeerConnectionInterrupted=" ORIGINAL_DLL ".EOS_P2P_RemoveNotifyPeerConnectionInterrupted,@391")
#pragma comment(linker, "/export:EOS_P2P_RemoveNotifyPeerConnectionRequest=" ORIGINAL_DLL ".EOS_P2P_RemoveNotifyPeerConnectionRequest,@392")
#pragma comment(linker, "/export:EOS_P2P_SendPacket=" ORIGINAL_DLL ".EOS_P2P_SendPacket,@393")
#pragma comment(linker, "/export:EOS_P2P_SetPacketQueueSize=" ORIGINAL_DLL ".EOS_P2P_SetPacketQueueSize,@394")
#pragma comment(linker, "/export:EOS_P2P_SetPortRange=" ORIGINAL_DLL ".EOS_P2P_SetPortRange,@395")
#pragma comment(linker, "/export:EOS_P2P_SetRelayControl=" ORIGINAL_DLL ".EOS_P2P_SetRelayControl,@396")
#pragma comment(linker, "/export:EOS_Platform_CheckForLauncherAndRestart=" ORIGINAL_DLL ".EOS_Platform_CheckForLauncherAndRestart,@397")
#pragma comment(linker, "/export:EOS_Platform_GetAchievementsInterface=" ORIGINAL_DLL ".EOS_Platform_GetAchievementsInterface,@399")
#pragma comment(linker, "/export:EOS_Platform_GetActiveCountryCode=" ORIGINAL_DLL ".EOS_Platform_GetActiveCountryCode,@400")
#pragma comment(linker, "/export:EOS_Platform_GetActiveLocaleCode=" ORIGINAL_DLL ".EOS_Platform_GetActiveLocaleCode,@401")
#pragma comment(linker, "/export:EOS_Platform_GetAntiCheatClientInterface=" ORIGINAL_DLL ".EOS_Platform_GetAntiCheatClientInterface,@402")
#pragma comment(linker, "/export:EOS_Platform_GetAntiCheatServerInterface=" ORIGINAL_DLL ".EOS_Platform_GetAntiCheatServerInterface,@403")
#pragma comment(linker, "/export:EOS_Platform_GetApplicationStatus=" ORIGINAL_DLL ".EOS_Platform_GetApplicationStatus,@404")
#pragma comment(linker, "/export:EOS_Platform_GetAuthInterface=" ORIGINAL_DLL ".EOS_Platform_GetAuthInterface,@405")
#pragma comment(linker, "/export:EOS_Platform_GetConnectInterface=" ORIGINAL_DLL ".EOS_Platform_GetConnectInterface,@406")
#pragma comment(linker, "/export:EOS_Platform_GetCustomInvitesInterface=" ORIGINAL_DLL ".EOS_Platform_GetCustomInvitesInterface,@407")
#pragma comment(linker, "/export:EOS_Platform_GetDesktopCrossplayStatus=" ORIGINAL_DLL ".EOS_Platform_GetDesktopCrossplayStatus,@408")
#pragma comment(linker, "/export:EOS_Platform_GetEcomInterface=" ORIGINAL_DLL ".EOS_Platform_GetEcomInterface,@409")
#pragma comment(linker, "/export:EOS_Platform_GetFriendsInterface=" ORIGINAL_DLL ".EOS_Platform_GetFriendsInterface,@410")
#pragma comment(linker, "/export:EOS_Platform_GetIntegratedPlatformInterface=" ORIGINAL_DLL ".EOS_Platform_GetIntegratedPlatformInterface,@411")
#pragma comment(linker, "/export:EOS_Platform_GetKWSInterface=" ORIGINAL_DLL ".EOS_Platform_GetKWSInterface,@412")
#pragma comment(linker, "/export:EOS_Platform_GetLeaderboardsInterface=" ORIGINAL_DLL ".EOS_Platform_GetLeaderboardsInterface,@413")
#pragma comment(linker, "/export:EOS_Platform_GetLobbyInterface=" ORIGINAL_DLL ".EOS_Platform_GetLobbyInterface,@414")
#pragma comment(linker, "/export:EOS_Platform_GetMetricsInterface=" ORIGINAL_DLL ".EOS_Platform_GetMetricsInterface,@415")
#pragma comment(linker, "/export:EOS_Platform_GetModsInterface=" ORIGINAL_DLL ".EOS_Platform_GetModsInterface,@416")
#pragma comment(linker, "/export:EOS_Platform_GetNetworkStatus=" ORIGINAL_DLL ".EOS_Platform_GetNetworkStatus,@417")
#pragma comment(linker, "/export:EOS_Platform_GetOverrideCountryCode=" ORIGINAL_DLL ".EOS_Platform_GetOverrideCountryCode,@418")
#pragma comment(linker, "/export:EOS_Platform_GetOverrideLocaleCode=" ORIGINAL_DLL ".EOS_Platform_GetOverrideLocaleCode,@419")
#pragma comment(linker, "/export:EOS_Platform_GetP2PInterface=" ORIGINAL_DLL ".EOS_Platform_GetP2PInterface,@420")
#pragma comment(linker, "/export:EOS_Platform_GetPlayerDataStorageInterface=" ORIGINAL_DLL ".EOS_Platform_GetPlayerDataStorageInterface,@421")
#pragma comment(linker, "/export:EOS_Platform_GetPresenceInterface=" ORIGINAL_DLL ".EOS_Platform_GetPresenceInterface,@422")
#pragma comment(linker, "/export:EOS_Platform_GetProgressionSnapshotInterface=" ORIGINAL_DLL ".EOS_Platform_GetProgressionSnapshotInterface,@423")
#pragma comment(linker, "/export:EOS_Platform_GetRTCAdminInterface=" ORIGINAL_DLL ".EOS_Platform_GetRTCAdminInterface,@424")
#pragma comment(linker, "/export:EOS_Platform_GetRTCInterface=" ORIGINAL_DLL ".EOS_Platform_GetRTCInterface,@425")
#pragma comment(linker, "/export:EOS_Platform_GetReportsInterface=" ORIGINAL_DLL ".EOS_Platform_GetReportsInterface,@426")
#pragma comment(linker, "/export:EOS_Platform_GetSanctionsInterface=" ORIGINAL_DLL ".EOS_Platform_GetSanctionsInterface,@427")
#pragma comment(linker, "/export:EOS_Platform_GetSessionsInterface=" ORIGINAL_DLL ".EOS_Platform_GetSessionsInterface,@428")
#pragma comment(linker, "/export:EOS_Platform_GetStatsInterface=" ORIGINAL_DLL ".EOS_Platform_GetStatsInterface,@429")
#pragma comment(linker, "/export:EOS_Platform_GetTitleStorageInterface=" ORIGINAL_DLL ".EOS_Platform_GetTitleStorageInterface,@430")
#pragma comment(linker, "/export:EOS_Platform_GetUIInterface=" ORIGINAL_DLL ".EOS_Platform_GetUIInterface,@431")
#pragma comment(linker, "/export:EOS_Platform_GetUserInfoInterface=" ORIGINAL_DLL ".EOS_Platform_GetUserInfoInterface,@432")
#pragma comment(linker, "/export:EOS_Platform_Release=" ORIGINAL_DLL ".EOS_Platform_Release,@433")
#pragma comment(linker, "/export:EOS_Platform_SetApplicationStatus=" ORIGINAL_DLL ".EOS_Platform_SetApplicationStatus,@434")
#pragma comment(linker, "/export:EOS_Platform_SetNetworkStatus=" ORIGINAL_DLL ".EOS_Platform_SetNetworkStatus,@435")
#pragma comment(linker, "/export:EOS_Platform_SetOverrideCountryCode=" ORIGINAL_DLL ".EOS_Platform_SetOverrideCountryCode,@436")
#pragma comment(linker, "/export:EOS_Platform_SetOverrideLocaleCode=" ORIGINAL_DLL ".EOS_Platform_SetOverrideLocaleCode,@437")
#pragma comment(linker, "/export:EOS_Platform_Tick=" ORIGINAL_DLL ".EOS_Platform_Tick,@438")
#pragma comment(linker, "/export:EOS_PlayerDataStorageFileTransferRequest_CancelRequest=" ORIGINAL_DLL ".EOS_PlayerDataStorageFileTransferRequest_CancelRequest,@439")
#pragma comment(linker, "/export:EOS_PlayerDataStorageFileTransferRequest_GetFileRequestState=" ORIGINAL_DLL ".EOS_PlayerDataStorageFileTransferRequest_GetFileRequestState,@440")
#pragma comment(linker, "/export:EOS_PlayerDataStorageFileTransferRequest_GetFilename=" ORIGINAL_DLL ".EOS_PlayerDataStorageFileTransferRequest_GetFilename,@441")
#pragma comment(linker, "/export:EOS_PlayerDataStorageFileTransferRequest_Release=" ORIGINAL_DLL ".EOS_PlayerDataStorageFileTransferRequest_Release,@442")
#pragma comment(linker, "/export:EOS_PlayerDataStorage_CopyFileMetadataAtIndex=" ORIGINAL_DLL ".EOS_PlayerDataStorage_CopyFileMetadataAtIndex,@443")
#pragma comment(linker, "/export:EOS_PlayerDataStorage_CopyFileMetadataByFilename=" ORIGINAL_DLL ".EOS_PlayerDataStorage_CopyFileMetadataByFilename,@444")
#pragma comment(linker, "/export:EOS_PlayerDataStorage_DeleteCache=" ORIGINAL_DLL ".EOS_PlayerDataStorage_DeleteCache,@445")
#pragma comment(linker, "/export:EOS_PlayerDataStorage_DeleteFile=" ORIGINAL_DLL ".EOS_PlayerDataStorage_DeleteFile,@446")
#pragma comment(linker, "/export:EOS_PlayerDataStorage_DuplicateFile=" ORIGINAL_DLL ".EOS_PlayerDataStorage_DuplicateFile,@447")
#pragma comment(linker, "/export:EOS_PlayerDataStorage_FileMetadata_Release=" ORIGINAL_DLL ".EOS_PlayerDataStorage_FileMetadata_Release,@448")
#pragma comment(linker, "/export:EOS_PlayerDataStorage_GetFileMetadataCount=" ORIGINAL_DLL ".EOS_PlayerDataStorage_GetFileMetadataCount,@449")
#pragma comment(linker, "/export:EOS_PlayerDataStorage_QueryFile=" ORIGINAL_DLL ".EOS_PlayerDataStorage_QueryFile,@450")
#pragma comment(linker, "/export:EOS_PlayerDataStorage_QueryFileList=" ORIGINAL_DLL ".EOS_PlayerDataStorage_QueryFileList,@451")
#pragma comment(linker, "/export:EOS_PlayerDataStorage_ReadFile=" ORIGINAL_DLL ".EOS_PlayerDataStorage_ReadFile,@452")
#pragma comment(linker, "/export:EOS_PlayerDataStorage_WriteFile=" ORIGINAL_DLL ".EOS_PlayerDataStorage_WriteFile,@453")
#pragma comment(linker, "/export:EOS_PresenceModification_DeleteData=" ORIGINAL_DLL ".EOS_PresenceModification_DeleteData,@454")
#pragma comment(linker, "/export:EOS_PresenceModification_Release=" ORIGINAL_DLL ".EOS_PresenceModification_Release,@455")
#pragma comment(linker, "/export:EOS_PresenceModification_SetData=" ORIGINAL_DLL ".EOS_PresenceModification_SetData,@456")
#pragma comment(linker, "/export:EOS_PresenceModification_SetJoinInfo=" ORIGINAL_DLL ".EOS_PresenceModification_SetJoinInfo,@457")
#pragma comment(linker, "/export:EOS_PresenceModification_SetRawRichText=" ORIGINAL_DLL ".EOS_PresenceModification_SetRawRichText,@458")
#pragma comment(linker, "/export:EOS_PresenceModification_SetStatus=" ORIGINAL_DLL ".EOS_PresenceModification_SetStatus,@459")
#pragma comment(linker, "/export:EOS_Presence_AddNotifyJoinGameAccepted=" ORIGINAL_DLL ".EOS_Presence_AddNotifyJoinGameAccepted,@460")
#pragma comment(linker, "/export:EOS_Presence_AddNotifyOnPresenceChanged=" ORIGINAL_DLL ".EOS_Presence_AddNotifyOnPresenceChanged,@461")
#pragma comment(linker, "/export:EOS_Presence_CopyPresence=" ORIGINAL_DLL ".EOS_Presence_CopyPresence,@462")
#pragma comment(linker, "/export:EOS_Presence_CreatePresenceModification=" ORIGINAL_DLL ".EOS_Presence_CreatePresenceModification,@463")
#pragma comment(linker, "/export:EOS_Presence_GetJoinInfo=" ORIGINAL_DLL ".EOS_Presence_GetJoinInfo,@464")
#pragma comment(linker, "/export:EOS_Presence_HasPresence=" ORIGINAL_DLL ".EOS_Presence_HasPresence,@465")
#pragma comment(linker, "/export:EOS_Presence_Info_Release=" ORIGINAL_DLL ".EOS_Presence_Info_Release,@466")
#pragma comment(linker, "/export:EOS_Presence_QueryPresence=" ORIGINAL_DLL ".EOS_Presence_QueryPresence,@467")
#pragma comment(linker, "/export:EOS_Presence_RemoveNotifyJoinGameAccepted=" ORIGINAL_DLL ".EOS_Presence_RemoveNotifyJoinGameAccepted,@468")
#pragma comment(linker, "/export:EOS_Presence_RemoveNotifyOnPresenceChanged=" ORIGINAL_DLL ".EOS_Presence_RemoveNotifyOnPresenceChanged,@469")
#pragma comment(linker, "/export:EOS_Presence_SetPresence=" ORIGINAL_DLL ".EOS_Presence_SetPresence,@470")
#pragma comment(linker, "/export:EOS_ProductUserId_FromString=" ORIGINAL_DLL ".EOS_ProductUserId_FromString,@471")
#pragma comment(linker, "/export:EOS_ProductUserId_IsValid=" ORIGINAL_DLL ".EOS_ProductUserId_IsValid,@472")
#pragma comment(linker, "/export:EOS_ProductUserId_ToString=" ORIGINAL_DLL ".EOS_ProductUserId_ToString,@473")
#pragma comment(linker, "/export:EOS_ProgressionSnapshot_AddProgression=" ORIGINAL_DLL ".EOS_ProgressionSnapshot_AddProgression,@474")
#pragma comment(linker, "/export:EOS_ProgressionSnapshot_BeginSnapshot=" ORIGINAL_DLL ".EOS_ProgressionSnapshot_BeginSnapshot,@475")
#pragma comment(linker, "/export:EOS_ProgressionSnapshot_DeleteSnapshot=" ORIGINAL_DLL ".EOS_ProgressionSnapshot_DeleteSnapshot,@476")
#pragma comment(linker, "/export:EOS_ProgressionSnapshot_EndSnapshot=" ORIGINAL_DLL ".EOS_ProgressionSnapshot_EndSnapshot,@477")
#pragma comment(linker, "/export:EOS_ProgressionSnapshot_SubmitSnapshot=" ORIGINAL_DLL ".EOS_ProgressionSnapshot_SubmitSnapshot,@478")
#pragma comment(linker, "/export:EOS_RTCAdmin_CopyUserTokenByIndex=" ORIGINAL_DLL ".EOS_RTCAdmin_CopyUserTokenByIndex,@479")
#pragma comment(linker, "/export:EOS_RTCAdmin_CopyUserTokenByUserId=" ORIGINAL_DLL ".EOS_RTCAdmin_CopyUserTokenByUserId,@480")
#pragma comment(linker, "/export:EOS_RTCAdmin_Kick=" ORIGINAL_DLL ".EOS_RTCAdmin_Kick,@481")
#pragma comment(linker, "/export:EOS_RTCAdmin_QueryJoinRoomToken=" ORIGINAL_DLL ".EOS_RTCAdmin_QueryJoinRoomToken,@482")
#pragma comment(linker, "/export:EOS_RTCAdmin_SetParticipantHardMute=" ORIGINAL_DLL ".EOS_RTCAdmin_SetParticipantHardMute,@483")
#pragma comment(linker, "/export:EOS_RTCAdmin_UserToken_Release=" ORIGINAL_DLL ".EOS_RTCAdmin_UserToken_Release,@484")
#pragma comment(linker, "/export:EOS_RTCAudio_AddNotifyAudioBeforeRender=" ORIGINAL_DLL ".EOS_RTCAudio_AddNotifyAudioBeforeRender,@485")
#pragma comment(linker, "/export:EOS_RTCAudio_AddNotifyAudioBeforeSend=" ORIGINAL_DLL ".EOS_RTCAudio_AddNotifyAudioBeforeSend,@486")
#pragma comment(linker, "/export:EOS_RTCAudio_AddNotifyAudioDevicesChanged=" ORIGINAL_DLL ".EOS_RTCAudio_AddNotifyAudioDevicesChanged,@487")
#pragma comment(linker, "/export:EOS_RTCAudio_AddNotifyAudioInputState=" ORIGINAL_DLL ".EOS_RTCAudio_AddNotifyAudioInputState,@488")
#pragma comment(linker, "/export:EOS_RTCAudio_AddNotifyAudioOutputState=" ORIGINAL_DLL ".EOS_RTCAudio_AddNotifyAudioOutputState,@489")
#pragma comment(linker, "/export:EOS_RTCAudio_AddNotifyParticipantUpdated=" ORIGINAL_DLL ".EOS_RTCAudio_AddNotifyParticipantUpdated,@490")
#pragma comment(linker, "/export:EOS_RTCAudio_CopyInputDeviceInformationByIndex=" ORIGINAL_DLL ".EOS_RTCAudio_CopyInputDeviceInformationByIndex,@491")
#pragma comment(linker, "/export:EOS_RTCAudio_CopyOutputDeviceInformationByIndex=" ORIGINAL_DLL ".EOS_RTCAudio_CopyOutputDeviceInformationByIndex,@492")
#pragma comment(linker, "/export:EOS_RTCAudio_GetAudioInputDeviceByIndex=" ORIGINAL_DLL ".EOS_RTCAudio_GetAudioInputDeviceByIndex,@493")
#pragma comment(linker, "/export:EOS_RTCAudio_GetAudioInputDevicesCount=" ORIGINAL_DLL ".EOS_RTCAudio_GetAudioInputDevicesCount,@494")
#pragma comment(linker, "/export:EOS_RTCAudio_GetAudioOutputDeviceByIndex=" ORIGINAL_DLL ".EOS_RTCAudio_GetAudioOutputDeviceByIndex,@495")
#pragma comment(linker, "/export:EOS_RTCAudio_GetAudioOutputDevicesCount=" ORIGINAL_DLL ".EOS_RTCAudio_GetAudioOutputDevicesCount,@496")
#pragma comment(linker, "/export:EOS_RTCAudio_GetInputDevicesCount=" ORIGINAL_DLL ".EOS_RTCAudio_GetInputDevicesCount,@497")
#pragma comment(linker, "/export:EOS_RTCAudio_GetOutputDevicesCount=" ORIGINAL_DLL ".EOS_RTCAudio_GetOutputDevicesCount,@498")
#pragma comment(linker, "/export:EOS_RTCAudio_InputDeviceInformation_Release=" ORIGINAL_DLL ".EOS_RTCAudio_InputDeviceInformation_Release,@499")
#pragma comment(linker, "/export:EOS_RTCAudio_OutputDeviceInformation_Release=" ORIGINAL_DLL ".EOS_RTCAudio_OutputDeviceInformation_Release,@500")
#pragma comment(linker, "/export:EOS_RTCAudio_QueryInputDevicesInformation=" ORIGINAL_DLL ".EOS_RTCAudio_QueryInputDevicesInformation,@501")
#pragma comment(linker, "/export:EOS_RTCAudio_QueryOutputDevicesInformation=" ORIGINAL_DLL ".EOS_RTCAudio_QueryOutputDevicesInformation,@502")
#pragma comment(linker, "/export:EOS_RTCAudio_RegisterPlatformAudioUser=" ORIGINAL_DLL ".EOS_RTCAudio_RegisterPlatformAudioUser,@503")
#pragma comment(linker, "/export:EOS_RTCAudio_RegisterPlatformUser=" ORIGINAL_DLL ".EOS_RTCAudio_RegisterPlatformUser,@504")
#pragma comment(linker, "/export:EOS_RTCAudio_RemoveNotifyAudioBeforeRender=" ORIGINAL_DLL ".EOS_RTCAudio_RemoveNotifyAudioBeforeRender,@505")
#pragma comment(linker, "/export:EOS_RTCAudio_RemoveNotifyAudioBeforeSend=" ORIGINAL_DLL ".EOS_RTCAudio_RemoveNotifyAudioBeforeSend,@506")
#pragma comment(linker, "/export:EOS_RTCAudio_RemoveNotifyAudioDevicesChanged=" ORIGINAL_DLL ".EOS_RTCAudio_RemoveNotifyAudioDevicesChanged,@507")
#pragma comment(linker, "/export:EOS_RTCAudio_RemoveNotifyAudioInputState=" ORIGINAL_DLL ".EOS_RTCAudio_RemoveNotifyAudioInputState,@508")
#pragma comment(linker, "/export:EOS_RTCAudio_RemoveNotifyAudioOutputState=" ORIGINAL_DLL ".EOS_RTCAudio_RemoveNotifyAudioOutputState,@509")
#pragma comment(linker, "/export:EOS_RTCAudio_RemoveNotifyParticipantUpdated=" ORIGINAL_DLL ".EOS_RTCAudio_RemoveNotifyParticipantUpdated,@510")
#pragma comment(linker, "/export:EOS_RTCAudio_SendAudio=" ORIGINAL_DLL ".EOS_RTCAudio_SendAudio,@511")
#pragma comment(linker, "/export:EOS_RTCAudio_SetAudioInputSettings=" ORIGINAL_DLL ".EOS_RTCAudio_SetAudioInputSettings,@512")
#pragma comment(linker, "/export:EOS_RTCAudio_SetAudioOutputSettings=" ORIGINAL_DLL ".EOS_RTCAudio_SetAudioOutputSettings,@513")
#pragma comment(linker, "/export:EOS_RTCAudio_SetInputDeviceSettings=" ORIGINAL_DLL ".EOS_RTCAudio_SetInputDeviceSettings,@514")
#pragma comment(linker, "/export:EOS_RTCAudio_SetOutputDeviceSettings=" ORIGINAL_DLL ".EOS_RTCAudio_SetOutputDeviceSettings,@515")
#pragma comment(linker, "/export:EOS_RTCAudio_SetPosition=" ORIGINAL_DLL ".EOS_RTCAudio_SetPosition,@516")
#pragma comment(linker, "/export:EOS_RTCAudio_UnregisterPlatformAudioUser=" ORIGINAL_DLL ".EOS_RTCAudio_UnregisterPlatformAudioUser,@517")
#pragma comment(linker, "/export:EOS_RTCAudio_UnregisterPlatformUser=" ORIGINAL_DLL ".EOS_RTCAudio_UnregisterPlatformUser,@518")
#pragma comment(linker, "/export:EOS_RTCAudio_UpdateParticipantVolume=" ORIGINAL_DLL ".EOS_RTCAudio_UpdateParticipantVolume,@519")
#pragma comment(linker, "/export:EOS_RTCAudio_UpdateReceiving=" ORIGINAL_DLL ".EOS_RTCAudio_UpdateReceiving,@520")
#pragma comment(linker, "/export:EOS_RTCAudio_UpdateReceivingVolume=" ORIGINAL_DLL ".EOS_RTCAudio_UpdateReceivingVolume,@521")
#pragma comment(linker, "/export:EOS_RTCAudio_UpdateSending=" ORIGINAL_DLL ".EOS_RTCAudio_UpdateSending,@522")
#pragma comment(linker, "/export:EOS_RTCAudio_UpdateSendingVolume=" ORIGINAL_DLL ".EOS_RTCAudio_UpdateSendingVolume,@523")
#pragma comment(linker, "/export:EOS_RTCData_AddNotifyDataReceived=" ORIGINAL_DLL ".EOS_RTCData_AddNotifyDataReceived,@524")
#pragma comment(linker, "/export:EOS_RTCData_RemoveNotifyDataReceived=" ORIGINAL_DLL ".EOS_RTCData_RemoveNotifyDataReceived,@525")
#pragma comment(linker, "/export:EOS_RTCData_SendData=" ORIGINAL_DLL ".EOS_RTCData_SendData,@526")
#pragma comment(linker, "/export:EOS_RTCData_UpdateReceiving=" ORIGINAL_DLL ".EOS_RTCData_UpdateReceiving,@527")
#pragma comment(linker, "/export:EOS_RTCData_UpdateSending=" ORIGINAL_DLL ".EOS_RTCData_UpdateSending,@528")
#pragma comment(linker, "/export:EOS_RTCVideo_AddNotifyParticipantUpdated=" ORIGINAL_DLL ".EOS_RTCVideo_AddNotifyParticipantUpdated,@529")
#pragma comment(linker, "/export:EOS_RTCVideo_AddNotifyVideoReceived=" ORIGINAL_DLL ".EOS_RTCVideo_AddNotifyVideoReceived,@530")
#pragma comment(linker, "/export:EOS_RTCVideo_CreateOutgoingVideoFrameFormat=" ORIGINAL_DLL ".EOS_RTCVideo_CreateOutgoingVideoFrameFormat,@531")
#pragma comment(linker, "/export:EOS_RTCVideo_RemoveNotifyParticipantUpdated=" ORIGINAL_DLL ".EOS_RTCVideo_RemoveNotifyParticipantUpdated,@532")
#pragma comment(linker, "/export:EOS_RTCVideo_RemoveNotifyVideoReceived=" ORIGINAL_DLL ".EOS_RTCVideo_RemoveNotifyVideoReceived,@533")
#pragma comment(linker, "/export:EOS_RTCVideo_SendVideo=" ORIGINAL_DLL ".EOS_RTCVideo_SendVideo,@534")
#pragma comment(linker, "/export:EOS_RTCVideo_SetAdaptVideoFrameCallback=" ORIGINAL_DLL ".EOS_RTCVideo_SetAdaptVideoFrameCallback,@535")
#pragma comment(linker, "/export:EOS_RTCVideo_SetVideoAllocationCallback=" ORIGINAL_DLL ".EOS_RTCVideo_SetVideoAllocationCallback,@536")
#pragma comment(linker, "/export:EOS_RTCVideo_SetVideoReleaseCallback=" ORIGINAL_DLL ".EOS_RTCVideo_SetVideoReleaseCallback,@537")
#pragma comment(linker, "/export:EOS_RTCVideo_UpdateReceiving=" ORIGINAL_DLL ".EOS_RTCVideo_UpdateReceiving,@538")
#pragma comment(linker, "/export:EOS_RTCVideo_UpdateSending=" ORIGINAL_DLL ".EOS_RTCVideo_UpdateSending,@539")
#pragma comment(linker, "/export:EOS_RTC_AddNotifyDisconnected=" ORIGINAL_DLL ".EOS_RTC_AddNotifyDisconnected,@540")
#pragma comment(linker, "/export:EOS_RTC_AddNotifyParticipantStatusChanged=" ORIGINAL_DLL ".EOS_RTC_AddNotifyParticipantStatusChanged,@541")
#pragma comment(linker, "/export:EOS_RTC_AddNotifyRoomStatisticsUpdated=" ORIGINAL_DLL ".EOS_RTC_AddNotifyRoomStatisticsUpdated,@542")
#pragma comment(linker, "/export:EOS_RTC_BlockParticipant=" ORIGINAL_DLL ".EOS_RTC_BlockParticipant,@543")
#pragma comment(linker, "/export:EOS_RTC_GetAudioInterface=" ORIGINAL_DLL ".EOS_RTC_GetAudioInterface,@544")
#pragma comment(linker, "/export:EOS_RTC_JoinRoom=" ORIGINAL_DLL ".EOS_RTC_JoinRoom,@545")
#pragma comment(linker, "/export:EOS_RTC_LeaveRoom=" ORIGINAL_DLL ".EOS_RTC_LeaveRoom,@546")
#pragma comment(linker, "/export:EOS_RTC_RemoveNotifyDisconnected=" ORIGINAL_DLL ".EOS_RTC_RemoveNotifyDisconnected,@547")
#pragma comment(linker, "/export:EOS_RTC_RemoveNotifyParticipantStatusChanged=" ORIGINAL_DLL ".EOS_RTC_RemoveNotifyParticipantStatusChanged,@548")
#pragma comment(linker, "/export:EOS_RTC_RemoveNotifyRoomStatisticsUpdated=" ORIGINAL_DLL ".EOS_RTC_RemoveNotifyRoomStatisticsUpdated,@549")
#pragma comment(linker, "/export:EOS_RTC_SetRoomSetting=" ORIGINAL_DLL ".EOS_RTC_SetRoomSetting,@550")
#pragma comment(linker, "/export:EOS_RTC_SetSetting=" ORIGINAL_DLL ".EOS_RTC_SetSetting,@551")
#pragma comment(linker, "/export:EOS_Reports_SendPlayerBehaviorReport=" ORIGINAL_DLL ".EOS_Reports_SendPlayerBehaviorReport,@552")
#pragma comment(linker, "/export:EOS_Sanctions_CopyPlayerSanctionByIndex=" ORIGINAL_DLL ".EOS_Sanctions_CopyPlayerSanctionByIndex,@553")
#pragma comment(linker, "/export:EOS_Sanctions_GetPlayerSanctionCount=" ORIGINAL_DLL ".EOS_Sanctions_GetPlayerSanctionCount,@554")
#pragma comment(linker, "/export:EOS_Sanctions_PlayerSanction_Release=" ORIGINAL_DLL ".EOS_Sanctions_PlayerSanction_Release,@555")
#pragma comment(linker, "/export:EOS_Sanctions_QueryActivePlayerSanctions=" ORIGINAL_DLL ".EOS_Sanctions_QueryActivePlayerSanctions,@556")
#pragma comment(linker, "/export:EOS_SessionDetails_Attribute_Release=" ORIGINAL_DLL ".EOS_SessionDetails_Attribute_Release,@557")
#pragma comment(linker, "/export:EOS_SessionDetails_CopyInfo=" ORIGINAL_DLL ".EOS_SessionDetails_CopyInfo,@558")
#pragma comment(linker, "/export:EOS_SessionDetails_CopySessionAttributeByIndex=" ORIGINAL_DLL ".EOS_SessionDetails_CopySessionAttributeByIndex,@559")
#pragma comment(linker, "/export:EOS_SessionDetails_CopySessionAttributeByKey=" ORIGINAL_DLL ".EOS_SessionDetails_CopySessionAttributeByKey,@560")
#pragma comment(linker, "/export:EOS_SessionDetails_GetSessionAttributeCount=" ORIGINAL_DLL ".EOS_SessionDetails_GetSessionAttributeCount,@561")
#pragma comment(linker, "/export:EOS_SessionDetails_Info_Release=" ORIGINAL_DLL ".EOS_SessionDetails_Info_Release,@562")
#pragma comment(linker, "/export:EOS_SessionDetails_Release=" ORIGINAL_DLL ".EOS_SessionDetails_Release,@563")
#pragma comment(linker, "/export:EOS_SessionModification_AddAttribute=" ORIGINAL_DLL ".EOS_SessionModification_AddAttribute,@564")
#pragma comment(linker, "/export:EOS_SessionModification_Release=" ORIGINAL_DLL ".EOS_SessionModification_Release,@565")
#pragma comment(linker, "/export:EOS_SessionModification_RemoveAttribute=" ORIGINAL_DLL ".EOS_SessionModification_RemoveAttribute,@566")
#pragma comment(linker, "/export:EOS_SessionModification_SetAllowedPlatformIds=" ORIGINAL_DLL ".EOS_SessionModification_SetAllowedPlatformIds,@567")
#pragma comment(linker, "/export:EOS_SessionModification_SetBucketId=" ORIGINAL_DLL ".EOS_SessionModification_SetBucketId,@568")
#pragma comment(linker, "/export:EOS_SessionModification_SetHostAddress=" ORIGINAL_DLL ".EOS_SessionModification_SetHostAddress,@569")
#pragma comment(linker, "/export:EOS_SessionModification_SetInvitesAllowed=" ORIGINAL_DLL ".EOS_SessionModification_SetInvitesAllowed,@570")
#pragma comment(linker, "/export:EOS_SessionModification_SetJoinInProgressAllowed=" ORIGINAL_DLL ".EOS_SessionModification_SetJoinInProgressAllowed,@571")
#pragma comment(linker, "/export:EOS_SessionModification_SetMaxPlayers=" ORIGINAL_DLL ".EOS_SessionModification_SetMaxPlayers,@572")
#pragma comment(linker, "/export:EOS_SessionModification_SetPermissionLevel=" ORIGINAL_DLL ".EOS_SessionModification_SetPermissionLevel,@573")
#pragma comment(linker, "/export:EOS_SessionSearch_CopySearchResultByIndex=" ORIGINAL_DLL ".EOS_SessionSearch_CopySearchResultByIndex,@574")
#pragma comment(linker, "/export:EOS_SessionSearch_Find=" ORIGINAL_DLL ".EOS_SessionSearch_Find,@575")
#pragma comment(linker, "/export:EOS_SessionSearch_GetSearchResultCount=" ORIGINAL_DLL ".EOS_SessionSearch_GetSearchResultCount,@576")
#pragma comment(linker, "/export:EOS_SessionSearch_Release=" ORIGINAL_DLL ".EOS_SessionSearch_Release,@577")
#pragma comment(linker, "/export:EOS_SessionSearch_RemoveParameter=" ORIGINAL_DLL ".EOS_SessionSearch_RemoveParameter,@578")
#pragma comment(linker, "/export:EOS_SessionSearch_SetMaxResults=" ORIGINAL_DLL ".EOS_SessionSearch_SetMaxResults,@579")
#pragma comment(linker, "/export:EOS_SessionSearch_SetParameter=" ORIGINAL_DLL ".EOS_SessionSearch_SetParameter,@580")
#pragma comment(linker, "/export:EOS_SessionSearch_SetSessionId=" ORIGINAL_DLL ".EOS_SessionSearch_SetSessionId,@581")
#pragma comment(linker, "/export:EOS_SessionSearch_SetTargetUserId=" ORIGINAL_DLL ".EOS_SessionSearch_SetTargetUserId,@582")
#pragma comment(linker, "/export:EOS_Sessions_AddNotifyJoinSessionAccepted=" ORIGINAL_DLL ".EOS_Sessions_AddNotifyJoinSessionAccepted,@583")
#pragma comment(linker, "/export:EOS_Sessions_AddNotifyLeaveSessionRequested=" ORIGINAL_DLL ".EOS_Sessions_AddNotifyLeaveSessionRequested,@584")
#pragma comment(linker, "/export:EOS_Sessions_AddNotifySendSessionNativeInviteRequested=" ORIGINAL_DLL ".EOS_Sessions_AddNotifySendSessionNativeInviteRequested,@585")
#pragma comment(linker, "/export:EOS_Sessions_AddNotifySessionInviteAccepted=" ORIGINAL_DLL ".EOS_Sessions_AddNotifySessionInviteAccepted,@586")
#pragma comment(linker, "/export:EOS_Sessions_AddNotifySessionInviteReceived=" ORIGINAL_DLL ".EOS_Sessions_AddNotifySessionInviteReceived,@587")
#pragma comment(linker, "/export:EOS_Sessions_AddNotifySessionInviteRejected=" ORIGINAL_DLL ".EOS_Sessions_AddNotifySessionInviteRejected,@588")
#pragma comment(linker, "/export:EOS_Sessions_CopyActiveSessionHandle=" ORIGINAL_DLL ".EOS_Sessions_CopyActiveSessionHandle,@589")
#pragma comment(linker, "/export:EOS_Sessions_CopySessionHandleByInviteId=" ORIGINAL_DLL ".EOS_Sessions_CopySessionHandleByInviteId,@590")
#pragma comment(linker, "/export:EOS_Sessions_CopySessionHandleByUiEventId=" ORIGINAL_DLL ".EOS_Sessions_CopySessionHandleByUiEventId,@591")
#pragma comment(linker, "/export:EOS_Sessions_CopySessionHandleForPresence=" ORIGINAL_DLL ".EOS_Sessions_CopySessionHandleForPresence,@592")
#pragma comment(linker, "/export:EOS_Sessions_CreateSessionModification=" ORIGINAL_DLL ".EOS_Sessions_CreateSessionModification,@593")
#pragma comment(linker, "/export:EOS_Sessions_CreateSessionSearch=" ORIGINAL_DLL ".EOS_Sessions_CreateSessionSearch,@594")
#pragma comment(linker, "/export:EOS_Sessions_DestroySession=" ORIGINAL_DLL ".EOS_Sessions_DestroySession,@595")
#pragma comment(linker, "/export:EOS_Sessions_DumpSessionState=" ORIGINAL_DLL ".EOS_Sessions_DumpSessionState,@596")
#pragma comment(linker, "/export:EOS_Sessions_EndSession=" ORIGINAL_DLL ".EOS_Sessions_EndSession,@597")
#pragma comment(linker, "/export:EOS_Sessions_GetInviteCount=" ORIGINAL_DLL ".EOS_Sessions_GetInviteCount,@598")
#pragma comment(linker, "/export:EOS_Sessions_GetInviteIdByIndex=" ORIGINAL_DLL ".EOS_Sessions_GetInviteIdByIndex,@599")
#pragma comment(linker, "/export:EOS_Sessions_IsUserInSession=" ORIGINAL_DLL ".EOS_Sessions_IsUserInSession,@600")
#pragma comment(linker, "/export:EOS_Sessions_JoinSession=" ORIGINAL_DLL ".EOS_Sessions_JoinSession,@601")
#pragma comment(linker, "/export:EOS_Sessions_QueryInvites=" ORIGINAL_DLL ".EOS_Sessions_QueryInvites,@602")
#pragma comment(linker, "/export:EOS_Sessions_RegisterPlayers=" ORIGINAL_DLL ".EOS_Sessions_RegisterPlayers,@603")
#pragma comment(linker, "/export:EOS_Sessions_RejectInvite=" ORIGINAL_DLL ".EOS_Sessions_RejectInvite,@604")
#pragma comment(linker, "/export:EOS_Sessions_RemoveNotifyJoinSessionAccepted=" ORIGINAL_DLL ".EOS_Sessions_RemoveNotifyJoinSessionAccepted,@605")
#pragma comment(linker, "/export:EOS_Sessions_RemoveNotifyLeaveSessionRequested=" ORIGINAL_DLL ".EOS_Sessions_RemoveNotifyLeaveSessionRequested,@606")
#pragma comment(linker, "/export:EOS_Sessions_RemoveNotifySendSessionNativeInviteRequested=" ORIGINAL_DLL ".EOS_Sessions_RemoveNotifySendSessionNativeInviteRequested,@607")
#pragma comment(linker, "/export:EOS_Sessions_RemoveNotifySessionInviteAccepted=" ORIGINAL_DLL ".EOS_Sessions_RemoveNotifySessionInviteAccepted,@608")
#pragma comment(linker, "/export:EOS_Sessions_RemoveNotifySessionInviteReceived=" ORIGINAL_DLL ".EOS_Sessions_RemoveNotifySessionInviteReceived,@609")
#pragma comment(linker, "/export:EOS_Sessions_RemoveNotifySessionInviteRejected=" ORIGINAL_DLL ".EOS_Sessions_RemoveNotifySessionInviteRejected,@610")
#pragma comment(linker, "/export:EOS_Sessions_SendInvite=" ORIGINAL_DLL ".EOS_Sessions_SendInvite,@611")
#pragma comment(linker, "/export:EOS_Sessions_StartSession=" ORIGINAL_DLL ".EOS_Sessions_StartSession,@612")
#pragma comment(linker, "/export:EOS_Sessions_UnregisterPlayers=" ORIGINAL_DLL ".EOS_Sessions_UnregisterPlayers,@613")
#pragma comment(linker, "/export:EOS_Sessions_UpdateSession=" ORIGINAL_DLL ".EOS_Sessions_UpdateSession,@614")
#pragma comment(linker, "/export:EOS_Sessions_UpdateSessionModification=" ORIGINAL_DLL ".EOS_Sessions_UpdateSessionModification,@615")
#pragma comment(linker, "/export:EOS_Shutdown=" ORIGINAL_DLL ".EOS_Shutdown,@616")
#pragma comment(linker, "/export:EOS_Stats_CopyStatByIndex=" ORIGINAL_DLL ".EOS_Stats_CopyStatByIndex,@617")
#pragma comment(linker, "/export:EOS_Stats_CopyStatByName=" ORIGINAL_DLL ".EOS_Stats_CopyStatByName,@618")
#pragma comment(linker, "/export:EOS_Stats_GetStatsCount=" ORIGINAL_DLL ".EOS_Stats_GetStatsCount,@619")
#pragma comment(linker, "/export:EOS_Stats_IngestStat=" ORIGINAL_DLL ".EOS_Stats_IngestStat,@620")
#pragma comment(linker, "/export:EOS_Stats_QueryStats=" ORIGINAL_DLL ".EOS_Stats_QueryStats,@621")
#pragma comment(linker, "/export:EOS_Stats_Stat_Release=" ORIGINAL_DLL ".EOS_Stats_Stat_Release,@622")
#pragma comment(linker, "/export:EOS_TitleStorageFileTransferRequest_CancelRequest=" ORIGINAL_DLL ".EOS_TitleStorageFileTransferRequest_CancelRequest,@623")
#pragma comment(linker, "/export:EOS_TitleStorageFileTransferRequest_GetFileRequestState=" ORIGINAL_DLL ".EOS_TitleStorageFileTransferRequest_GetFileRequestState,@624")
#pragma comment(linker, "/export:EOS_TitleStorageFileTransferRequest_GetFilename=" ORIGINAL_DLL ".EOS_TitleStorageFileTransferRequest_GetFilename,@625")
#pragma comment(linker, "/export:EOS_TitleStorageFileTransferRequest_Release=" ORIGINAL_DLL ".EOS_TitleStorageFileTransferRequest_Release,@626")
#pragma comment(linker, "/export:EOS_TitleStorage_CopyFileMetadataAtIndex=" ORIGINAL_DLL ".EOS_TitleStorage_CopyFileMetadataAtIndex,@627")
#pragma comment(linker, "/export:EOS_TitleStorage_CopyFileMetadataByFilename=" ORIGINAL_DLL ".EOS_TitleStorage_CopyFileMetadataByFilename,@628")
#pragma comment(linker, "/export:EOS_TitleStorage_DeleteCache=" ORIGINAL_DLL ".EOS_TitleStorage_DeleteCache,@629")
#pragma comment(linker, "/export:EOS_TitleStorage_FileMetadata_Release=" ORIGINAL_DLL ".EOS_TitleStorage_FileMetadata_Release,@630")
#pragma comment(linker, "/export:EOS_TitleStorage_GetFileMetadataCount=" ORIGINAL_DLL ".EOS_TitleStorage_GetFileMetadataCount,@631")
#pragma comment(linker, "/export:EOS_TitleStorage_QueryFile=" ORIGINAL_DLL ".EOS_TitleStorage_QueryFile,@632")
#pragma comment(linker, "/export:EOS_TitleStorage_QueryFileList=" ORIGINAL_DLL ".EOS_TitleStorage_QueryFileList,@633")
#pragma comment(linker, "/export:EOS_TitleStorage_ReadFile=" ORIGINAL_DLL ".EOS_TitleStorage_ReadFile,@634")
#pragma comment(linker, "/export:EOS_UI_AcknowledgeEventId=" ORIGINAL_DLL ".EOS_UI_AcknowledgeEventId,@635")
#pragma comment(linker, "/export:EOS_UI_AddNotifyDisplaySettingsUpdated=" ORIGINAL_DLL ".EOS_UI_AddNotifyDisplaySettingsUpdated,@636")
#pragma comment(linker, "/export:EOS_UI_AddNotifyMemoryMonitor=" ORIGINAL_DLL ".EOS_UI_AddNotifyMemoryMonitor,@637")
#pragma comment(linker, "/export:EOS_UI_GetFriendsExclusiveInput=" ORIGINAL_DLL ".EOS_UI_GetFriendsExclusiveInput,@638")
#pragma comment(linker, "/export:EOS_UI_GetFriendsVisible=" ORIGINAL_DLL ".EOS_UI_GetFriendsVisible,@639")
#pragma comment(linker, "/export:EOS_UI_GetNotificationLocationPreference=" ORIGINAL_DLL ".EOS_UI_GetNotificationLocationPreference,@640")
#pragma comment(linker, "/export:EOS_UI_GetToggleFriendsButton=" ORIGINAL_DLL ".EOS_UI_GetToggleFriendsButton,@641")
#pragma comment(linker, "/export:EOS_UI_GetToggleFriendsKey=" ORIGINAL_DLL ".EOS_UI_GetToggleFriendsKey,@642")
#pragma comment(linker, "/export:EOS_UI_HideFriends=" ORIGINAL_DLL ".EOS_UI_HideFriends,@643")
#pragma comment(linker, "/export:EOS_UI_IsSocialOverlayPaused=" ORIGINAL_DLL ".EOS_UI_IsSocialOverlayPaused,@644")
#pragma comment(linker, "/export:EOS_UI_IsValidButtonCombination=" ORIGINAL_DLL ".EOS_UI_IsValidButtonCombination,@645")
#pragma comment(linker, "/export:EOS_UI_IsValidKeyCombination=" ORIGINAL_DLL ".EOS_UI_IsValidKeyCombination,@646")
#pragma comment(linker, "/export:EOS_UI_PauseSocialOverlay=" ORIGINAL_DLL ".EOS_UI_PauseSocialOverlay,@647")
#pragma comment(linker, "/export:EOS_UI_PrePresent=" ORIGINAL_DLL ".EOS_UI_PrePresent,@648")
#pragma comment(linker, "/export:EOS_UI_RemoveNotifyDisplaySettingsUpdated=" ORIGINAL_DLL ".EOS_UI_RemoveNotifyDisplaySettingsUpdated,@649")
#pragma comment(linker, "/export:EOS_UI_RemoveNotifyMemoryMonitor=" ORIGINAL_DLL ".EOS_UI_RemoveNotifyMemoryMonitor,@650")
#pragma comment(linker, "/export:EOS_UI_ReportInputState=" ORIGINAL_DLL ".EOS_UI_ReportInputState,@651")
#pragma comment(linker, "/export:EOS_UI_SetDisplayPreference=" ORIGINAL_DLL ".EOS_UI_SetDisplayPreference,@652")
#pragma comment(linker, "/export:EOS_UI_SetToggleFriendsButton=" ORIGINAL_DLL ".EOS_UI_SetToggleFriendsButton,@653")
#pragma comment(linker, "/export:EOS_UI_SetToggleFriendsKey=" ORIGINAL_DLL ".EOS_UI_SetToggleFriendsKey,@654")
#pragma comment(linker, "/export:EOS_UI_ShowBlockPlayer=" ORIGINAL_DLL ".EOS_UI_ShowBlockPlayer,@655")
#pragma comment(linker, "/export:EOS_UI_ShowFriends=" ORIGINAL_DLL ".EOS_UI_ShowFriends,@656")
#pragma comment(linker, "/export:EOS_UI_ShowNativeProfile=" ORIGINAL_DLL ".EOS_UI_ShowNativeProfile,@657")
#pragma comment(linker, "/export:EOS_UI_ShowReportPlayer=" ORIGINAL_DLL ".EOS_UI_ShowReportPlayer,@658")
#pragma comment(linker, "/export:EOS_UserInfo_BestDisplayName_Release=" ORIGINAL_DLL ".EOS_UserInfo_BestDisplayName_Release,@659")
#pragma comment(linker, "/export:EOS_UserInfo_CopyBestDisplayName=" ORIGINAL_DLL ".EOS_UserInfo_CopyBestDisplayName,@660")
#pragma comment(linker, "/export:EOS_UserInfo_CopyBestDisplayNameWithPlatform=" ORIGINAL_DLL ".EOS_UserInfo_CopyBestDisplayNameWithPlatform,@661")
#pragma comment(linker, "/export:EOS_UserInfo_CopyExternalUserInfoByAccountId=" ORIGINAL_DLL ".EOS_UserInfo_CopyExternalUserInfoByAccountId,@662")
#pragma comment(linker, "/export:EOS_UserInfo_CopyExternalUserInfoByAccountType=" ORIGINAL_DLL ".EOS_UserInfo_CopyExternalUserInfoByAccountType,@663")
#pragma comment(linker, "/export:EOS_UserInfo_CopyExternalUserInfoByIndex=" ORIGINAL_DLL ".EOS_UserInfo_CopyExternalUserInfoByIndex,@664")
#pragma comment(linker, "/export:EOS_UserInfo_CopyUserInfo=" ORIGINAL_DLL ".EOS_UserInfo_CopyUserInfo,@665")
#pragma comment(linker, "/export:EOS_UserInfo_ExternalUserInfo_Release=" ORIGINAL_DLL ".EOS_UserInfo_ExternalUserInfo_Release,@666")
#pragma comment(linker, "/export:EOS_UserInfo_GetExternalUserInfoCount=" ORIGINAL_DLL ".EOS_UserInfo_GetExternalUserInfoCount,@667")
#pragma comment(linker, "/export:EOS_UserInfo_GetLocalPlatformType=" ORIGINAL_DLL ".EOS_UserInfo_GetLocalPlatformType,@668")
#pragma comment(linker, "/export:EOS_UserInfo_QueryUserInfo=" ORIGINAL_DLL ".EOS_UserInfo_QueryUserInfo,@669")
#pragma comment(linker, "/export:EOS_UserInfo_QueryUserInfoByDisplayName=" ORIGINAL_DLL ".EOS_UserInfo_QueryUserInfoByDisplayName,@670")
#pragma comment(linker, "/export:EOS_UserInfo_QueryUserInfoByExternalAccount=" ORIGINAL_DLL ".EOS_UserInfo_QueryUserInfoByExternalAccount,@671")
#pragma comment(linker, "/export:EOS_UserInfo_Release=" ORIGINAL_DLL ".EOS_UserInfo_Release,@672")

// #pragma comment(linker, "/export:EOS_Initialize=" ORIGINAL_DLL ".EOS_Initialize,@245")
#pragma comment(linker, "/export:EOS_Lobby_CreateLobby=" ORIGINAL_DLL ".EOS_Lobby_CreateLobby,@329")
#pragma comment(linker, "/export:EOS_Connect_CreateDeviceId=" ORIGINAL_DLL ".EOS_Connect_CreateDeviceId,@141")
// #pragma comment(linker, "/export:EOS_Platform_Create=" ORIGINAL_DLL ".EOS_Platform_Create,@398")
#pragma comment(linker, "/export:EOS_Platform_Release=" ORIGINAL_DLL ".EOS_Platform_Release,@433")
// #pragma comment(linker, "/export:EOS_Auth_Login=" ORIGINAL_DLL ".EOS_Auth_Login,@110")
// #pragma comment(linker, "/export:EOS_Connect_Login=" ORIGINAL_DLL ".EOS_Connect_Login,@153")

#include "steam.h"
#include "common.h"
#include <intrin.h>

static HMODULE g_hOrig = NULL;

// -------- EOS Structs ------------------------

typedef struct {
    int32_t ApiVersion;
    const char* ProductId;
    const char* SandboxId;
    const char* ClientId;
    const char* ClientSecret;
    const char* ProductName;
    const char* ProductVersion;
    void* Reserved;
    void* bReserved;
    uint32_t Flags;
    const char* ErrorCode;
    const char* ErrorMessage;
} EOS_Platform_Options;

typedef struct {
    int32_t ApiVersion;
    void* AllocateMemoryFunction;
    void* ReallocateMemoryFunction;
    void* ReleaseMemoryFunction;
    const char* ProductName;
    const char* ProductVersion;
    void* Reserved;
    void* SystemInitializeOptions;
    void* OverrideThreadAffinity;
} EOS_InitializeOptions;

// Configuration - set these via environment variable EOS_PROXY_APPID
#define DEFAULT_TARGET_PRODUCT_ID "335430" // Default dev/fork AppID - change as needed

static char TARGET_PRODUCT_ID[32]; 
static const char* TARGET_SANDBOX_ID = "";

// Initialize config on DLL load
static void InitConfig() {
    GetEnvironmentVariableA("EOS_PROXY_APPID", TARGET_PRODUCT_ID, sizeof(TARGET_PRODUCT_ID));
    if (TARGET_PRODUCT_ID[0] == '\0') {
        strcpy(TARGET_PRODUCT_ID, DEFAULT_TARGET_PRODUCT_ID);
    }
    LogText("[PROXY] Target AppID: %s", TARGET_PRODUCT_ID);
}

#define EOS_ERT_ZeroDivisionError 0

typedef int32_t EOS_EResult;

// Wrapper for EOS_Platform_Create that swaps ProductId
extern __declspec(dllexport) void* EOS_Platform_Create(EOS_Platform_Options* Options) {
    LogCall("EOS_Platform_Create", _ReturnAddress());
    
    if (Options && Options->ProductId) {
        LogText("Original ProductId: %s", Options->ProductId);
        
        // Swap ProductId pointers to use our target
        const char* originalProductId = Options->ProductId;
        Options->ProductId = TARGET_PRODUCT_ID ? TARGET_PRODUCT_ID : Options->ProductId;
        Options->SandboxId = TARGET_SANDBOX_ID;
        
        LogText("Swapped to ProductId: %s", Options->ProductId);
    }
    
    typedef void* (__cdecl* fn_t)(EOS_Platform_Options*);
    fn_t fp = (fn_t)GetProcAddress(g_hOrig, "EOS_Platform_Create");
    return fp(Options);
}

// Wrapper for EOS_Initialize that swaps ProductId
extern __declspec(dllexport) EOS_EResult EOS_Initialize(EOS_InitializeOptions* Options) {
    LogCall("EOS_Initialize", _ReturnAddress());
    
    if (Options && Options->ProductId) {
        LogText("Original Init ProductId: %s", Options->ProductId);
        const char* originalProductId = Options->ProductId;
        Options->ProductId = TARGET_PRODUCT_ID ? TARGET_PRODUCT_ID : Options->ProductId;
        Options->SandboxId = TARGET_SANDBOX_ID;
    }
    
    typedef EOS_EResult(__cdecl* fn_t)(EOS_InitializeOptions*);
    fn_t fp = (fn_t)GetProcAddress(g_hOrig, "EOS_Initialize");
    return fp(Options);
}

typedef struct {
    int32_t ApiVersion;
    const char* AccountId;
    const char* Token;
    uint32_t Type;
} EOS_Auth_Credentials;

typedef struct {
    int32_t ApiVersion;
    const EOS_Auth_Credentials* Credentials;
    uint32_t ScopeFlags;
} EOS_Auth_LoginOptions;

typedef struct {
    int32_t ApiVersion;
    int32_t ResultCode;
    void* ClientData;
    void* AccountId;
    void* LocalAccountId;
    void* PinGrantInfo;
    void* ContinuanceToken;
} EOS_Auth_OnLoginCallbackInfo;

// EOS_Auth_Login wrapper - provides fake Epic auth to allow Connect_Login to proceed
extern __declspec(dllexport) EOS_EResult EOS_Auth_Login(EOS_Auth_LoginOptions* Options, void* ClientData, void* CompletionDelegate) {
    LogCall("EOS_Auth_Login (proxy)", _ReturnAddress());
    
    // For games using Connect_Login, we just need to succeed
    // Call the callback directly with success
    if (CompletionDelegate) {
        EOS_Auth_OnLoginCallbackInfo callbackInfo = {0};
        callbackInfo.ApiVersion = 2;
        callbackInfo.ResultCode = 0; // EOS_Success
        callbackInfo.ClientData = ClientData;
        ((void(__cdecl*)(EOS_Auth_OnLoginCallbackInfo*))CompletionDelegate)(&callbackInfo);
    }
    return 0; // EOS_Success
}
typedef struct {
	int32_t ApiVersion;
	void* LocalUserId;
	uint32_t NumberOfProductIds;
	const char** ProductIdArray;
} EOS_Ecom_QueryOwnershipOptions;

typedef struct {
	int32_t ApiVersion;
	int32_t ResultCode;
	void* ClientData;
	void* LocalUserId;
} EOS_Ecom_QueryOwnershipCallbackInfo;

// Ownership query callback - fake ownership to pass entitlement checks
void EOS_Ecom_QueryOwnership_callback(EOS_Ecom_QueryOwnershipCallbackInfo *Info) {
    LogCall("CALLBACK --- EOS_Ecom_QueryOwnership (fake)", _ReturnAddress());
    Info->ResultCode = 0; // EOS_Success
}

// EOS_Ecom_QueryOwnership wrapper - always succeeds to allow multiplayer
extern __declspec(dllexport) EOS_EResult EOS_Ecom_QueryOwnership(void* Handle, EOS_Ecom_QueryOwnershipOptions* Options, void* ClientData, void* CompletionDelegate) {
    LogCall("EOS_Ecom_QueryOwnership (proxy)", _ReturnAddress());
    
    // Fake ownership success - call callback directly
    if (CompletionDelegate) {
        EOS_Ecom_QueryOwnershipCallbackInfo callbackInfo = {0};
        callbackInfo.ApiVersion = 2;
        callbackInfo.ResultCode = 0; // EOS_Success
        ((void(__cdecl*)(EOS_Ecom_QueryOwnershipCallbackInfo*))CompletionDelegate)(&callbackInfo);
    }
    return 0;
}

typedef struct {
	int32_t ApiVersion;
	const char* Token;
	uint32_t Type;
} EOS_Connect_Credentials;

typedef struct {
	int32_t ApiVersion;
	const char* DisplayName;
	const char* NsaIdToken;
} EOS_Connect_UserLoginInfo;

typedef struct {
    int32_t ApiVersion;
    const EOS_Connect_Credentials* Credentials;
    const EOS_Connect_UserLoginInfo* UserLoginInfo;
} EOS_Connect_LoginOptions;

typedef struct {
    int32_t ResultCode;
    void *ClientData;
    void *LocalUserId;
    void *ContinuanceToken;
} EOS_Connect_LoginCallbackInfo;

typedef struct {
		int32_t ApiVersion;
		const char* DeviceModel;
} EOS_Connect_CreateDeviceIdOptions;

typedef struct {
		int32_t ResultCode;
		void* ClientData;
} EOS_Connect_CreateDeviceIdCallbackInfo;

// -----------------------------------------

typedef struct {
    void *func;
    void *data;
} OriginalCallback;

typedef struct {
    OriginalCallback gameCallback;
    void *Handle;
    int32_t ConnectLoginApiVersion;
    int32_t CredentialsApiVersion;
} CallbackData;

void EOS_Connect_Login_callback(EOS_Connect_LoginCallbackInfo *Info) {
    LogCall("CALLBACK --- EOS_Connect_Login", _ReturnAddress());
    if (Info->ResultCode != 0) {
        LogText("ERROR when calling EOS_Connect_Login : %d", Info->ResultCode);
        MessageBox(NULL, "ERROR when calling EOS_Connect_Login. Check logs.", NULL, 0);
        exit(0);
    }
    LogText("ResultCode: %d", Info->ResultCode);

    CallbackData *data = Info->ClientData;

    typedef void(__cdecl* fn_t)(void *);
    Info->ClientData = data->gameCallback.data;
    fn_t actualCallback = (fn_t) data->gameCallback.func;
    free(data);
    actualCallback(Info);
}

void EOS_Connect_CreateDeviceId_callback(EOS_Connect_CreateDeviceIdCallbackInfo *Info) {
    LogCall("CALLBACK --- EOS_Connect_CreateDeviceId", _ReturnAddress());

    if (Info->ResultCode != 0 && Info->ResultCode != 24) {
        LogText("ERROR when calling EOS_Connect_CreateDeviceId : %d", Info->ResultCode);
        MessageBox(NULL, "ERROR when calling EOS_Connect_CreateDeviceId. Check logs.", NULL, 0);
        exit(0);
    }
    LogText("ResultCode: %d", Info->ResultCode);
    CallbackData *data = Info->ClientData;

    const char *personaName = Steam_GetPersonaName();
    LogText("Steam Persona Name Fetched: %s", personaName);

    EOS_Connect_Credentials credentials = {
        .ApiVersion = data->CredentialsApiVersion,
        .Token = NULL,
        .Type = 10 // EOS_ECT_DEVICEID_ACCESS_TOKEN
    };

    EOS_Connect_UserLoginInfo userLoginInfo = {
        .ApiVersion = 2,
        .NsaIdToken = NULL,
        .DisplayName = personaName
    };

    EOS_Connect_LoginOptions options = {
        .ApiVersion = data->ConnectLoginApiVersion,
        .Credentials = &credentials,
        .UserLoginInfo = &userLoginInfo
    };

    typedef void(__cdecl* fn_t)(void *, EOS_Connect_LoginOptions *, void *, void *);
    fn_t fp_EOSConnectLogin_original = (fn_t) GetProcAddress(g_hOrig, "EOS_Connect_Login");
    fp_EOSConnectLogin_original(data->Handle, &options, data, (void *) &EOS_Connect_Login_callback);
}

extern __declspec(dllexport) void EOS_Connect_Login(void* Handle, EOS_Connect_LoginOptions* Options, void* ClientData, void* CompletionDelegate) {
    LogCall("EOS_Connect_Login", _ReturnAddress());

    const EOS_Connect_Credentials* creds = Options->Credentials;
    LogText("EOS_Connect_Login | Type: %d", creds->Type);
    LogText("EOS_Connect_Login | Token: %.10s", creds->Token);
    if (Options->UserLoginInfo == NULL) {
        LogText("EOS_Connect_Login | UserLoginInfo is NULL");
    } else {
        LogText("EOS_Connect_Login | UserLoginInfo is not NULL...");
    }

    // Objective: Change the login from type steam to type device auth
    // First, call CreateDeviceId
    typedef void(__cdecl* fn_t)(void *, EOS_Connect_CreateDeviceIdOptions *, void *, void *);
    fn_t fp_EOS_CreateDeviceIdoriginal = (fn_t) GetProcAddress(g_hOrig, "EOS_Connect_CreateDeviceId");

    uint64_t steamID_int = Steam_GetSteamID();
    char steamID[32];
    sprintf_s(steamID, 32, "%llu", steamID_int);
    LogText("SteamID Fetched: %s", steamID);

    EOS_Connect_CreateDeviceIdOptions options = {
        .ApiVersion = 1,
        .DeviceModel = steamID
    };

    CallbackData *data = malloc(sizeof(CallbackData));
    data->gameCallback.data = ClientData;
    data->gameCallback.func = CompletionDelegate;
    data->Handle = Handle;
    data->ConnectLoginApiVersion = Options->ApiVersion;
    data->CredentialsApiVersion = Options->Credentials->ApiVersion;

    fp_EOS_CreateDeviceIdoriginal(Handle, &options, data, (void *) &EOS_Connect_CreateDeviceId_callback);    
}

// === AntiCheatClient intercepts ===
// These force AntiCheatClient to always report success, bypassing actual EAC initialization

typedef struct {
    int32_t ApiVersion;
    void* ClientData;
} EOS_AntiCheatClient_OnClientAuthStatusChangedCallbackInfo;

// AntiCheatClient_BeginSession fake - always succeeds
extern __declspec(dllexport) EOS_EResult EOS_AntiCheatClient_BeginSession(void* Options) {
    LogCall("EOS_AntiCheatClient_BeginSession (proxy - fake)", _ReturnAddress());
    return 0; // EOS_Success - pretend EAC is initialized
}

// AntiCheatClient_EndSession - forward to original
extern __declspec(dllexport) EOS_EResult EOS_AntiCheatClient_EndSession(void* Options) {
    LogCall("EOS_AntiCheatClient_EndSession (proxy)", _ReturnAddress());
    typedef EOS_EResult(__cdecl* fn_t)(void*);
    fn_t fp = (fn_t)GetProcAddress(g_hOrig, "EOS_AntiCheatClient_EndSession");
    return fp ? fp(Options) : 0;
}

// AntiCheatClient_PollStatus - return "running" status to pretend EAC is active
extern __declspec(dllexport) uint8_t EOS_AntiCheatClient_PollStatus(void* Options) {
    LogCall("EOS_AntiCheatClient_PollStatus (proxy - fake)", _ReturnAddress());
    return 1; // Return "authenticated" status
}

// AntiCheatClient_ProtectMessage - bypass message protection (not needed for EAC bypass)
extern __declspec(dllexport) EOS_EResult EOS_AntiCheatClient_ProtectMessage(void* Options) {
    LogCall("EOS_AntiCheatClient_ProtectMessage (proxy)", _ReturnAddress());
    typedef EOS_EResult(__cdecl* fn_t)(void*);
    fn_t fp = (fn_t)GetProcAddress(g_hOrig, "EOS_AntiCheatClient_ProtectMessage");
    return fp ? fp(Options) : 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
    switch (reason) {
    
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        InitLog("epic");
        InitConfig();

        // Add the folder with this DLL to DLL search path
        char path[1024];
        GetModuleFileNameA(hModule, path, sizeof(path));
        LogText("Location: %s", path);
        char dir[1024];
        _splitpath_s(path,
            NULL, 0,
            dir, sizeof(dir),
            NULL, 0,
            NULL, 0
        );
        SetDllDirectory(dir);

        g_hOrig = LoadLibraryA(ORIGINAL_DLL);
        if (!g_hOrig) {
            char err[256];
            _snprintf_s(err, sizeof(err), _TRUNCATE,
                "[PROXY] FATAL: could not load " ORIGINAL_DLL " (error %lu)\r\n",
                GetLastError());
            OutputDebugStringA(err);
            DWORD written;
            if (g_hLog != INVALID_HANDLE_VALUE)
                WriteFile(g_hLog, err, (DWORD)strlen(err), &written, NULL);
            return FALSE;
        } else {
            char ok[256];
            _snprintf_s(ok, sizeof(ok), _TRUNCATE,
                "[PROXY] Loaded " ORIGINAL_DLL " at 0x%p\r\n", g_hOrig);
            OutputDebugStringA(ok);
            DWORD written;
            if (g_hLog != INVALID_HANDLE_VALUE)
                WriteFile(g_hLog, ok, (DWORD)strlen(ok), &written, NULL);
        }
        break;

    case DLL_PROCESS_DETACH:
        if (g_hOrig) FreeLibrary(g_hOrig);
        if (g_hLog != INVALID_HANDLE_VALUE) CloseHandle(g_hLog);
        DeleteCriticalSection(&g_cs);
        break;
    }
    return TRUE;
}
