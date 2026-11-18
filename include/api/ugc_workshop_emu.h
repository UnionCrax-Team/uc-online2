#pragma once
// ============================================================
// UCOnline2 -- Workshop/UGC emulation for ogAppId
//
// When ogAppId is set, the real Steam client runs under the spoofed
// AppId (480/Spacewar) and therefore has zero Workshop items for the
// real game. This module hooks the four ISteamUGC methods the game
// calls to discover installed Workshop content, and answers them by
// scanning the filesystem:
//
//   steamapps/workshop/content/<ogAppId>/<workshopId>/
//
// The appworkshop_<ogAppId>.acf manifest is parsed for per-item
// metadata (size, timestamp). Items not in the ACF but present on
// disk are still reported with best-effort values.
//
// ISteamUGC vtable indices (STEAMUGC_INTERFACE_VERSION021):
//   74: GetNumSubscribedItems
//   75: GetSubscribedItems
//   76: GetItemState
//   77: GetItemInstallInfo
// ============================================================

#include <Windows.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <string.h>
#include <vector>

struct UcoWorkshopItem
{
    uint64        id;
    uint64        sizeOnDisk;
    uint32        timeUpdated;
    char          folder[MAX_PATH];
};

class UcoWorkshopEmu
{
public:
    static bool          s_bScanned;
    static bool          s_bEnabled;
    static std::vector<UcoWorkshopItem> s_Items;
    static char          s_ContentPath[MAX_PATH];

    static void Init(uint32 ogAppId)
    {
        if (ogAppId == 0) return;

        // Build the workshop content path:
        // <steam_library>/steamapps/workshop/content/<ogAppId>/
        // We derive the steam library root from the game exe directory,
        // which is typically <library>/steamapps/common/<GameName>/
        char libraryRoot[MAX_PATH] = { 0 };
        DWORD len = GetModuleFileNameA(nullptr, libraryRoot, MAX_PATH);
        if (len == 0) return;
        PathRemoveFileSpecA(libraryRoot);

        // Walk up from game dir to steamapps
        // e.g. C:\...\steamapps\common\ProjectZomboid -> C:\...\steamapps
        char* pCommon = nullptr;
        // Case-insensitive search for "\common\" or "/common/"
        for (char* p = libraryRoot; *p; ++p)
        {
            if ((*p == '\\' || *p == '/') &&
                _strnicmp(p + 1, "common", 6) == 0 &&
                (p[7] == '\\' || p[7] == '/'))
            {
                pCommon = p;
                // Don't break -- take the LAST match in case the path
                // has "common" earlier (unlikely but safe)
            }
        }

        if (!pCommon)
        {
            UCOLOG("[UCOnline2] WorkshopEmu: cannot find 'steamapps' root from install path '%s'", libraryRoot);
            return;
        }

        // pCommon points to "\common\..." -- truncate there to get steamapps path
        // Then append \workshop\content\<ogAppId>
        *pCommon = '\0';
        sprintf_s(s_ContentPath, "%s\\workshop\\content\\%u", libraryRoot, ogAppId);

        DWORD attr = GetFileAttributesA(s_ContentPath);
        if (attr == INVALID_FILE_ATTRIBUTES || !(attr & FILE_ATTRIBUTE_DIRECTORY))
        {
            UCOLOG("[UCOnline2] WorkshopEmu: content dir not found: '%s'", s_ContentPath);
            return;
        }

        s_bEnabled = true;
        UCOLOG("[UCOnline2] WorkshopEmu: enabled for AppId %u, content dir: %s", ogAppId, s_ContentPath);

        // Parse the ACF manifest for metadata
        char acfPath[MAX_PATH];
        sprintf_s(acfPath, "%s\\workshop\\appworkshop_%u.acf", libraryRoot, ogAppId);
        ParseAcf(acfPath);

        // Scan the content directory for workshop items
        ScanContentDir();
    }

    static bool Enabled() { return s_bEnabled; }

    static uint32 GetNumSubscribedItems()
    {
        if (!s_bScanned) ScanContentDir();
        return (uint32)s_Items.size();
    }

    static uint32 GetSubscribedItems(uint64* pvecPublishedFileID, uint32 cMaxEntries)
    {
        if (!s_bScanned) ScanContentDir();
        uint32 count = (uint32)s_Items.size();
        if (count > cMaxEntries) count = cMaxEntries;
        for (uint32 i = 0; i < count; ++i)
            pvecPublishedFileID[i] = s_Items[i].id;
        return count;
    }

    static uint32 GetItemState(uint64 nPublishedFileID)
    {
        if (!s_bScanned) ScanContentDir();
        for (const auto& item : s_Items)
        {
            if (item.id == nPublishedFileID)
                return 5; // k_EItemStateSubscribed (1) | k_EItemStateInstalled (4)
        }
        return 0; // k_EItemStateNone
    }

    static bool GetItemInstallInfo(uint64 nPublishedFileID, uint64* punSizeOnDisk,
                                   char* pchFolder, uint32 cchFolderSize, uint32* punTimeStamp)
    {
        if (!s_bScanned) ScanContentDir();
        for (const auto& item : s_Items)
        {
            if (item.id == nPublishedFileID)
            {
                if (punSizeOnDisk) *punSizeOnDisk = item.sizeOnDisk;
                if (pchFolder) strncpy_s(pchFolder, cchFolderSize, item.folder, _TRUNCATE);
                if (punTimeStamp) *punTimeStamp = item.timeUpdated;
                return true;
            }
        }
        return false;
    }

private:
    // Lightweight ACF parser -- only extracts WorkshopItemsInstalled entries
    struct AcfItemInfo
    {
        uint64 id;
        uint64 size;
        uint32 timeupdated;
    };
    static std::vector<AcfItemInfo> s_AcfItems;

    static void ParseAcf(const char* path)
    {
        FILE* f = nullptr;
        fopen_s(&f, path, "r");
        if (!f) return;

        char line[1024];
        bool inInstalled = false;
        bool inItem = false;
        uint64 currentId = 0;
        uint64 currentSize = 0;
        uint32 currentTime = 0;
        int braceDepth = 0;

        while (fgets(line, sizeof(line), f))
        {
            // Trim leading whitespace
            char* p = line;
            while (*p == ' ' || *p == '\t') ++p;

            if (!inInstalled)
            {
                if (strstr(p, "\"WorkshopItemsInstalled\""))
                    inInstalled = true;
                continue;
            }

            if (*p == '{')
            {
                ++braceDepth;
                continue;
            }
            if (*p == '}')
            {
                --braceDepth;
                if (braceDepth <= 0)
                {
                    // End of WorkshopItemsInstalled block
                    if (inItem && currentId != 0)
                    {
                        AcfItemInfo info = { currentId, currentSize, currentTime };
                        s_AcfItems.push_back(info);
                    }
                    break;
                }
                if (inItem && currentId != 0)
                {
                    AcfItemInfo info = { currentId, currentSize, currentTime };
                    s_AcfItems.push_back(info);
                    inItem = false;
                    currentId = 0;
                    currentSize = 0;
                    currentTime = 0;
                }
                continue;
            }

            // Parse key-value: "key"  "value"
            if (*p == '"')
            {
                char key[256] = { 0 };
                char val[256] = { 0 };
                if (sscanf_s(p, "\"%255[^\"]\"", key, (unsigned)sizeof(key)) == 1)
                {
                    // Find the value part
                    char* vp = strchr(p + 1, '"');
                    if (vp) vp = strchr(vp + 1, '"');
                    if (vp) vp = strchr(vp + 1, '"');
                    if (vp)
                    {
                        sscanf_s(vp, "\"%255[^\"]\"", val, (unsigned)sizeof(val));
                    }

                    if (braceDepth == 1)
                    {
                        // Top-level key inside WorkshopItemsInstalled = item ID
                        if (inItem && currentId != 0)
                        {
                            AcfItemInfo info = { currentId, currentSize, currentTime };
                            s_AcfItems.push_back(info);
                        }
                        inItem = true;
                        currentId = _strtoui64(key, nullptr, 10);
                        currentSize = 0;
                        currentTime = 0;
                    }
                    else if (braceDepth == 2 && inItem)
                    {
                        if (_stricmp(key, "size") == 0)
                            currentSize = _strtoui64(val, nullptr, 10);
                        else if (_stricmp(key, "timeupdated") == 0)
                            currentTime = (uint32)strtoul(val, nullptr, 10);
                    }
                }
            }
        }

        fclose(f);
        UCOLOG("[UCOnline2] WorkshopEmu: parsed %d items from ACF manifest", (int)s_AcfItems.size());
    }

    static void ScanContentDir()
    {
        s_bScanned = true;
        s_Items.clear();

        if (!s_bEnabled) return;

        char searchPath[MAX_PATH];
        sprintf_s(searchPath, "%s\\*", s_ContentPath);

        WIN32_FIND_DATAA fd;
        HANDLE hFind = FindFirstFileA(searchPath, &fd);
        if (hFind == INVALID_HANDLE_VALUE) return;

        do
        {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
            if (fd.cFileName[0] == '.') continue;

            // Directory name should be a numeric workshop ID
            char* end = nullptr;
            uint64 id = _strtoui64(fd.cFileName, &end, 10);
            if (!end || *end != '\0' || id == 0) continue;

            UcoWorkshopItem item = {};
            item.id = id;
            sprintf_s(item.folder, "%s\\%s", s_ContentPath, fd.cFileName);

            // Look up metadata from ACF
            for (const auto& acf : s_AcfItems)
            {
                if (acf.id == id)
                {
                    item.sizeOnDisk = acf.size;
                    item.timeUpdated = acf.timeupdated;
                    break;
                }
            }

            // Fallback: use directory modify time if no ACF data
            if (item.timeUpdated == 0)
            {
                FILETIME ft = fd.ftLastWriteTime;
                ULARGE_INTEGER uli;
                uli.LowPart = ft.dwLowDateTime;
                uli.HighPart = ft.dwHighDateTime;
                item.timeUpdated = (uint32)((uli.QuadPart - 116444736000000000ULL) / 10000000ULL);
            }

            s_Items.push_back(item);
        }
        while (FindNextFileA(hFind, &fd));

        FindClose(hFind);
        UCOLOG("[UCOnline2] WorkshopEmu: found %d workshop items on disk", (int)s_Items.size());
    }
};

// Static member definitions
bool                          UcoWorkshopEmu::s_bScanned = false;
bool                          UcoWorkshopEmu::s_bEnabled = false;
std::vector<UcoWorkshopItem>  UcoWorkshopEmu::s_Items;
char                          UcoWorkshopEmu::s_ContentPath[MAX_PATH] = { 0 };
std::vector<UcoWorkshopEmu::AcfItemInfo> UcoWorkshopEmu::s_AcfItems;

// ============================================================
// ISteamUGC vtable hook functions
// ============================================================

typedef uint32 (S_CALLTYPE *Fn_GetNumSubscribedItems)(void* pThis, bool bIncludeLocallyDisabled);
typedef uint32 (S_CALLTYPE *Fn_GetSubscribedItems)(void* pThis, uint64* pvecPublishedFileID, uint32 cMaxEntries, bool bIncludeLocallyDisabled);
typedef uint32 (S_CALLTYPE *Fn_GetItemState)(void* pThis, uint64 nPublishedFileID);
typedef bool   (S_CALLTYPE *Fn_GetItemInstallInfo)(void* pThis, uint64 nPublishedFileID, uint64* punSizeOnDisk, char* pchFolder, uint32 cchFolderSize, uint32* punTimeStamp);

static Fn_GetNumSubscribedItems  g_pfnOriginalGetNumSubscribedItems  = nullptr;
static Fn_GetSubscribedItems     g_pfnOriginalGetSubscribedItems     = nullptr;
static Fn_GetItemState           g_pfnOriginalGetItemState           = nullptr;
static Fn_GetItemInstallInfo     g_pfnOriginalGetItemInstallInfo     = nullptr;
static bool                      g_bUgcLoggedFirst                   = false;

static uint32 S_CALLTYPE Hooked_GetNumSubscribedItems(void* pThis, bool bIncludeLocallyDisabled)
{
    uint32 emuCount = UcoWorkshopEmu::GetNumSubscribedItems();

    if (emuCount > 0)
    {
        if (!g_bUgcLoggedFirst)
        {
            UCOLOG("[UCOnline2] GetNumSubscribedItems hook: returning %u items from disk (ogAppId workshop)", emuCount);
            g_bUgcLoggedFirst = true;
        }
        return emuCount;
    }

    // Fall through to real Steam if we have no local items
    return g_pfnOriginalGetNumSubscribedItems(pThis, bIncludeLocallyDisabled);
}

static uint32 S_CALLTYPE Hooked_GetSubscribedItems(void* pThis, uint64* pvecPublishedFileID, uint32 cMaxEntries, bool bIncludeLocallyDisabled)
{
    uint32 emuCount = UcoWorkshopEmu::GetNumSubscribedItems();

    if (emuCount > 0)
        return UcoWorkshopEmu::GetSubscribedItems(pvecPublishedFileID, cMaxEntries);

    return g_pfnOriginalGetSubscribedItems(pThis, pvecPublishedFileID, cMaxEntries, bIncludeLocallyDisabled);
}

static uint32 S_CALLTYPE Hooked_GetItemState(void* pThis, uint64 nPublishedFileID)
{
    uint32 state = UcoWorkshopEmu::GetItemState(nPublishedFileID);
    if (state != 0)
        return state;

    return g_pfnOriginalGetItemState(pThis, nPublishedFileID);
}

static bool S_CALLTYPE Hooked_GetItemInstallInfo(void* pThis, uint64 nPublishedFileID, uint64* punSizeOnDisk, char* pchFolder, uint32 cchFolderSize, uint32* punTimeStamp)
{
    if (UcoWorkshopEmu::GetItemInstallInfo(nPublishedFileID, punSizeOnDisk, pchFolder, cchFolderSize, punTimeStamp))
        return true;

    return g_pfnOriginalGetItemInstallInfo(pThis, nPublishedFileID, punSizeOnDisk, pchFolder, cchFolderSize, punTimeStamp);
}

// Call this from InstallSteamSpoofHooks() to install the UGC hooks
static void UcoInstallWorkshopHooks(ISteamUGC* pUGC)
{
    if (!pUGC) return;

    void** ugcVT = *reinterpret_cast<void***>(pUGC);

    // ISteamUGC vtable layout (STEAMUGC_INTERFACE_VERSION021):
    // 74: GetNumSubscribedItems
    // 75: GetSubscribedItems
    // 76: GetItemState
    // 77: GetItemInstallInfo
    struct UgcHook { int index; void* detour; void** original; const char* name; };
    const UgcHook hooks[] = {
        { 74, (void*)&Hooked_GetNumSubscribedItems,  (void**)&g_pfnOriginalGetNumSubscribedItems,  "GetNumSubscribedItems" },
        { 75, (void*)&Hooked_GetSubscribedItems,     (void**)&g_pfnOriginalGetSubscribedItems,     "GetSubscribedItems" },
        { 76, (void*)&Hooked_GetItemState,           (void**)&g_pfnOriginalGetItemState,           "GetItemState" },
        { 77, (void*)&Hooked_GetItemInstallInfo,     (void**)&g_pfnOriginalGetItemInstallInfo,     "GetItemInstallInfo" },
    };

    int installed = 0;
    for (const UgcHook& h : hooks)
    {
        void* target = ugcVT[h.index];
        MH_STATUS s = MH_CreateHook(target, h.detour, h.original);
        if (s == MH_OK && MH_EnableHook(target) == MH_OK)
            ++installed;
        else
            UCOLOG("[UCOnline2] failed to hook ISteamUGC::%s (vtable[%d]): %d",
                h.name, h.index, s);
    }

    UCOLOG("[UCOnline2] ISteamUGC Workshop hooks: %d/%d installed (%d items on disk)",
        installed, (int)(sizeof(hooks) / sizeof(hooks[0])),
        UcoWorkshopEmu::GetNumSubscribedItems());
}
