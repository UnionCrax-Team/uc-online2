/**
 *  The heavy lifter of this and how multiplayer games work on Steam in general.
 *  This is the class that handles all of the callbacks, and also the call results.
 *  Matchmaking, lobby creation & handling, server list retrieval, all of that is
 *  done through here. Without the dispatcher, none of these things would work.
 *
 *  ~veeλnti<3 2026
 */

#pragma once

#include <map>
#include <vector>

// A callback we generate ourselves rather than receive from Steam.
//
// The dispatcher normally only relays what real Steam hands it through
// BGetCallback. That is a problem for anything we emulate: a game that calls
// GetAuthSessionTicket registers GetAuthSessionTicketResponse_t and WAITS for
// it, and real Steam will never send one for a ticket we invented. Same for
// DlcInstalled_t after an emulated InstallDLC. Without this, emulating the call
// is useless -- the game just sits there.
//
// Queued rather than dispatched inline because games do not expect a callback
// to fire underneath the API call that requested it; the small delay mimics
// Steam's own asynchrony and keeps re-entrancy out of the caller's stack frame.
struct UcoPendingCallback
{
	int                  iCallback = 0;
	std::vector<uint8_t> data;
	HSteamUser           user = 0;
	bool                 bServer = false;
	ULONGLONG            dueTick = 0;   // GetTickCount64() before which it must not fire
};

typedef bool (S_CALLTYPE* Fn_BGetCallback)(HSteamPipe hPipe, CallbackMsg_t* pMsg);
typedef void (S_CALLTYPE* Fn_FreeLastCallback)(HSteamPipe hPipe);
typedef bool (S_CALLTYPE* Fn_GetAPICallResult)(HSteamPipe hPipe, SteamAPICall_t hCall, void* pBuf, int cubBuf, int iExpected, bool* pbFailed);

// Scoped lock for CCallbackDispatcher::m_MapLock.
class CDispatcherLock
{
public:
	explicit CDispatcherLock(CRITICAL_SECTION* pcs) : m_pcs(pcs) { if (m_pcs) EnterCriticalSection(m_pcs); }
	~CDispatcherLock() { if (m_pcs) LeaveCriticalSection(m_pcs); }
private:
	CRITICAL_SECTION* m_pcs;
	CDispatcherLock(const CDispatcherLock&);
	CDispatcherLock& operator=(const CDispatcherLock&);
};

class CCallbackDispatcher
{
public:
	Fn_BGetCallback m_pfnBGetCallback;
	Fn_FreeLastCallback m_pfnFreeLastCallback;
	Fn_GetAPICallResult m_pfnGetAPICallResult;
	HSteamUser m_CurrentUser;
	int m_ManualCbId;
	int m_ManualCbSize;
	std::vector<uint8_t> m_ManualSyntheticData;
	bool m_bManualSyntheticActive;
	bool m_bProcessing;
	std::multimap<int, CCallbackBase*> m_CallbackMap;
	std::map<SteamAPICall_t, CCallbackBase*> m_CallResultMap;
	std::map<SteamAPICall_t, BYTE*> m_BufferMap;
	std::vector<UcoPendingCallback> m_Synthetic;   // guarded by m_MapLock

	// Guards the three maps above.
	//
	// SteamAPI_RunCallbacks serializes DISPATCH against itself (via
	// g_CallbackLock), but SteamAPI_RegisterCallback / UnregisterCallback /
	// RegisterCallResult / UnregisterCallResult take NO lock. So a game thread
	// can mutate these maps while another thread is walking them mid-dispatch --
	// UE does exactly this, driving SteamAPI_RunCallbacks from
	// FOnlineAsyncTaskManagerSteam::OnlineTick() while the game thread registers
	// callbacks. The stale iterator then yields a dangling CCallbackBase*, and
	// the virtual call through it faults (seen as EXCEPTION_ACCESS_VIOLATION at
	// 0x8 / 0xffffffffffffffff in _guard_dispatch_icall under Forever Skies).
	//
	// Re-entrant by design: callbacks legitimately register/unregister others
	// from inside Run(), so this must be a recursive CRITICAL_SECTION.
	CRITICAL_SECTION m_MapLock;

	CCallbackDispatcher();
	~CCallbackDispatcher();

	void Shutdown();
	void DispatchFrame(HSteamPipe hPipe, bool bServer);
	void DispatchFrameSafe(HSteamPipe hPipe, bool bServer);
	void ExecuteCallResult(HSteamPipe hPipe, SteamAPICall_t hCall, CCallbackBase* pCb);
	void Add(CCallbackBase* pCb, int iCallback);

	// Queue a callback we synthesised. Safe to call from any thread; it is
	// delivered from whichever thread next drives SteamAPI_RunCallbacks.
	void PostCallback(int iCallback, const void* pvData, size_t cubData,
	                  HSteamUser user, bool bServer = false, unsigned delayMs = 10);
	bool TryGetSyntheticForManual(bool bServer, CallbackMsg_t* pMsg);
	bool FreeManualSynthetic();
	// Deliver any queued callbacks that are due. Called at the top of dispatch.
	void DrainSynthetic(bool bServer);
	// Shared by real and synthetic delivery so both obey the same matching rules.
	bool DispatchToTarget(int iCallback, void* pvData, uint32 cubData,
	                      HSteamUser user, bool bServer);
	void AddCallResult(CCallbackBase* pCb, SteamAPICall_t hCall);
	void Remove(CCallbackBase* pCb);
	void RemoveCallResult(CCallbackBase* pCb, SteamAPICall_t hCall);
};

CCallbackDispatcher* GetDispatcher();
