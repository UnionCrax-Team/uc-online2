// ============================================================
// UCOnline2 -- local ISteamInventory emulation (auto-grant + persist)
//
// UCO2 normally proxies ISteamInventory to real Steam, which under the spoofed
// AppId owns none of the game's items -- so purchases fail and nothing persists.
// With [Settings] InventoryAutoGrant=1, the flat ISteamInventory calls for
// PURCHASES, GRANTS and OWNED-ITEM reads are served from a per-game local file;
// everything else (RequestPrices / GetItemsWithPrices / item definitions) still
// passes through to real Steam, so a game's shop stays populated.
//
// Included into dllmain.cpp AFTER globals.h + callback_dispatcher.h + the SDK
// headers (relies on that include order, like api_flat.h). No SDK includes here.
// ============================================================
#pragma once
#include <cstdio>
#include <cstdint>
#include <ctime>
#include <map>
#include <vector>
#include <string>
#include <mutex>

namespace UcoInvEmu {

    static std::recursive_mutex s_lock;
    static bool s_enabled = false;
    static std::string s_file;
    static std::map<SteamItemDef_t, uint32_t> s_items;   // defid -> total quantity
    static std::map<SteamInventoryResult_t, std::vector<SteamItemDetails_t>> s_results;
    static SteamInventoryResult_t s_nextResult = 1;
    // hCall -> { iCallback, serialized result }. High range so it never collides
    // with a real Steam SteamAPICall_t.
    static std::map<SteamAPICall_t, std::pair<int, std::vector<uint8_t>>> s_calls;
    static SteamAPICall_t s_nextCall = 0x5000000000000000ULL;

    inline bool Enabled() { return s_enabled; }

    inline bool IsOurResult(SteamInventoryResult_t h) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        return s_results.find(h) != s_results.end();
    }
    inline bool IsOurCall(SteamAPICall_t h) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        return s_calls.find(h) != s_calls.end();
    }

    inline void Save() {
        FILE* f = nullptr; fopen_s(&f, s_file.c_str(), "wb");
        if (!f) return;
        for (auto& kv : s_items)
            if (kv.second) fprintf(f, "%d %u\n", (int)kv.first, kv.second);
        fclose(f);
    }
    inline void Load() {
        s_items.clear();
        FILE* f = nullptr; fopen_s(&f, s_file.c_str(), "rb");
        if (!f) return;
        int def; unsigned qty;
        while (fscanf_s(f, "%d %u", &def, &qty) == 2)
            if (qty) s_items[(SteamItemDef_t)def] = qty;
        fclose(f);
    }

    // iniPath is <game>\union-crax.ini; store the inventory beside it, keyed by
    // AppId so different games don't share an inventory.
    inline void Init(const char* iniPath, uint32_t appId) {
        if (!iniPath || !iniPath[0]) return;
        std::string dir(iniPath);
        size_t slash = dir.find_last_of("\\/");
        dir = (slash == std::string::npos) ? std::string(".") : dir.substr(0, slash);
        char name[64]; _snprintf_s(name, sizeof(name), _TRUNCATE, "\\uco_inventory_%u.txt", appId);
        s_file = dir + name;
        std::lock_guard<std::recursive_mutex> g(s_lock);
        Load();
        s_enabled = true;
        UCOLOG("[UCOnline2] Inventory emu active: %zu item stack(s) from %s",
            s_items.size(), s_file.c_str());
    }

    // Snapshot the local store into a fresh result handle.
    inline SteamInventoryResult_t MakeResult() {
        std::vector<SteamItemDetails_t> items;
        for (auto& kv : s_items) {
            SteamItemDetails_t d{};
            d.m_itemId = 0x100000000ULL | (uint64)(uint32)kv.first; // synthetic instance id
            d.m_iDefinition = kv.first;
            d.m_unQuantity = (uint16)(kv.second > 0xFFFFu ? 0xFFFFu : kv.second);
            d.m_unFlags = k_ESteamItemNoTrade;
            items.push_back(d);
        }
        SteamInventoryResult_t h = s_nextResult++;
        s_results[h] = std::move(items);
        return h;
    }

    inline void PostReady(SteamInventoryResult_t h) {
        SteamInventoryResultReady_t r{}; r.m_handle = h; r.m_result = k_EResultOK;
        GetDispatcher()->PostCallback(SteamInventoryResultReady_t::k_iCallback, &r, sizeof(r), g_ClientUser, false, 10);
        SteamInventoryFullUpdate_t u{}; u.m_handle = h;
        GetDispatcher()->PostCallback(SteamInventoryFullUpdate_t::k_iCallback, &u, sizeof(u), g_ClientUser, false, 12);
    }

    inline void GrantInternal(const SteamItemDef_t* defs, const uint32* qtys, uint32 n) {
        for (uint32 i = 0; i < n; ++i) {
            uint32 q = qtys ? qtys[i] : 1; if (!q) q = 1;
            s_items[defs[i]] += q;
        }
        Save();
    }

    // --- intercepted ISteamInventory operations -------------------------------

    inline bool GetAllItems(SteamInventoryResult_t* pResult) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        SteamInventoryResult_t h = MakeResult();
        if (pResult) *pResult = h;
        PostReady(h);
        return true;
    }

    inline bool GrantItems(const SteamItemDef_t* defs, const uint32* qtys, uint32 n, SteamInventoryResult_t* pResult) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        GrantInternal(defs, qtys, n);
        SteamInventoryResult_t h = MakeResult();
        if (pResult) *pResult = h;
        UCOLOG("[UCOnline2] Inventory: granted %u def(s), now %zu stack(s)", n, s_items.size());
        PostReady(h);
        return true;
    }

    inline bool AddPromoItem(SteamItemDef_t def, SteamInventoryResult_t* pResult) {
        uint32 one = 1; return GrantItems(&def, &one, 1, pResult);
    }

    inline bool ConsumeItem(SteamItemInstanceID_t itemId, uint32 qty, SteamInventoryResult_t* pResult) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        SteamItemDef_t def = (SteamItemDef_t)(uint32)(itemId & 0xFFFFFFFFULL);
        auto it = s_items.find(def);
        if (it != s_items.end()) {
            if (!qty || qty >= it->second) s_items.erase(it);
            else it->second -= qty;
            Save();
        }
        SteamInventoryResult_t h = MakeResult();
        if (pResult) *pResult = h;
        PostReady(h);
        return true;
    }

    // StartPurchase: auto-succeed, grant locally, and deliver the async
    // SteamInventoryStartPurchaseResult_t call result (see GetAPICallResult).
    inline SteamAPICall_t StartPurchase(const SteamItemDef_t* defs, const uint32* qtys, uint32 n) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        GrantInternal(defs, qtys, n);
        SteamAPICall_t hCall = s_nextCall++;
        SteamInventoryStartPurchaseResult_t res{};
        res.m_result = k_EResultOK;
        res.m_ulOrderID = (uint64)hCall;
        res.m_ulTransID = (uint64)hCall;
        std::vector<uint8_t> data(sizeof(res));
        memcpy(data.data(), &res, sizeof(res));
        s_calls[hCall] = { (int)SteamInventoryStartPurchaseResult_t::k_iCallback, std::move(data) };

        SteamAPICallCompleted_t done{};
        done.m_hAsyncCall = hCall;
        done.m_iCallback = SteamInventoryStartPurchaseResult_t::k_iCallback;
        done.m_cubParam = sizeof(SteamInventoryStartPurchaseResult_t);
        GetDispatcher()->PostCallback(SteamAPICallCompleted_t::k_iCallback, &done, sizeof(done), g_ClientUser, false, 10);
        UCOLOG("[UCOnline2] Inventory: auto-approved StartPurchase of %u def(s) -> call %llu", n, (unsigned long long)hCall);
        return hCall;
    }

    // --- result accessors (only ever called for our own handles) --------------

    inline EResult GetResultStatus(SteamInventoryResult_t) { return k_EResultOK; }

    inline bool GetResultItems(SteamInventoryResult_t h, SteamItemDetails_t* pOut, uint32* pCount) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        auto it = s_results.find(h);
        if (it == s_results.end() || !pCount) return false;
        uint32 avail = (uint32)it->second.size();
        if (!pOut) { *pCount = avail; return true; }
        uint32 copy = *pCount < avail ? *pCount : avail;
        for (uint32 i = 0; i < copy; ++i) pOut[i] = it->second[i];
        *pCount = copy;
        return true;
    }

    inline bool GetResultItemProperty(SteamInventoryResult_t, uint32, const char*, char* pchValue, uint32* punValueSize) {
        if (punValueSize) { if (pchValue && *punValueSize) pchValue[0] = '\0'; *punValueSize = 1; }
        return false; // no per-item properties in this first version
    }

    inline uint32 GetResultTimestamp(SteamInventoryResult_t) { return (uint32)time(nullptr); }
    inline bool CheckResultSteamID(SteamInventoryResult_t, class CSteamID) { return true; }

    inline void DestroyResult(SteamInventoryResult_t h) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        s_results.erase(h);
    }

    // --- async call-result delivery (only for our synthetic hCalls) -----------

    inline bool IsAPICallCompleted(SteamAPICall_t, bool* pbFailed) {
        if (pbFailed) *pbFailed = false;
        return true; // ours complete immediately
    }

    inline bool GetAPICallResult(SteamAPICall_t hCall, void* pBuf, int cubBuf, int /*iExpected*/, bool* pbFailed) {
        std::lock_guard<std::recursive_mutex> g(s_lock);
        auto it = s_calls.find(hCall);
        if (it == s_calls.end()) return false;
        if (pbFailed) *pbFailed = false;
        int n = (int)it->second.second.size();
        if (pBuf && cubBuf >= n) memcpy(pBuf, it->second.second.data(), n);
        return true;
    }
}
