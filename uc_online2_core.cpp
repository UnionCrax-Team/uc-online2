/**
 *  Core DLL - Handles plugin loading, SteamStub, and extended features.
 *
 *  ~veeλnti<3 2026
 */

#include <Windows.h>
#include <Shlwapi.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <atomic>
#include <cstdint>
#include <string>
#include <vector>
#include <DbgHelp.h>

// ============================================================
// Core DLL globals (these are private to this DLL)
// ============================================================

static HMODULE g_hModule = nullptr;

class CDLLLoader;
static CDLLLoader* s_pPluginLoader = nullptr;

// Forward declarations for SteamStub
static bool g_bSteamStubEnabled = false;
static uint32_t s_ForcedAppId = 480;
static uint32_t s_OriginalAppId = 0;

static void SteamStub_Init();

#define UC_CORE_EXPORTS

#include "include/uc_loader.h"

// ============================================================
// Logging
// ============================================================

static FILE* g_logFile = nullptr;
static CRITICAL_SECTION g_logLock;

static void InitializeLogging()
{
    InitializeCriticalSection(&g_logLock);
    
    char logPath[MAX_PATH] = { 0 };
    DWORD len = GetTempPathA(MAX_PATH, logPath);
    if (len == 0 || len > (MAX_PATH - 25)) 
        return;

    if (!PathAppendA(logPath, "uc_online2.log")) 
        return;

    fopen_s(&g_logFile, logPath, "ab");
}

static void ShutdownLogging()
{
    if (g_logFile)
    {
        fclose(g_logFile);
        g_logFile = nullptr;
    }
    DeleteCriticalSection(&g_logLock);
}

static void UCOLogImpl(const char* fmt, va_list args)
{
    if (!g_logFile) 
        return;

    EnterCriticalSection(&g_logLock);

    char msg[2048] = { 0 };
    _vsnprintf_s(msg, sizeof(msg), _TRUNCATE, fmt, args);

    SYSTEMTIME st = { 0 };
    GetLocalTime(&st);

    fprintf(g_logFile, "[%04u-%02u-%02u %02u:%02u:%02u.%03u] %s",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, msg);

    size_t msgLen = strlen(msg);
    if (msgLen == 0 || msg[msgLen - 1] != '\n')
    {
        fputs("\r\n", g_logFile);
        fflush(g_logFile);
    }

    LeaveCriticalSection(&g_logLock);
}

void UCOLOG(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    UCOLogImpl(fmt, args);
    va_end(args);
}

// Minimal CGameID implementation for AppID file
struct CGameID
{
    uint64_t m_ulGameID;
    
    CGameID(uint32_t nAppID) : m_ulGameID(nAppID) {}
    uint64_t ToUint64() const { return m_ulGameID; }
};

// ============================================================
// SteamStub Implementation
// ============================================================

#include <intrin.h>
#include "include/MinHook.h"

static std::atomic<uint32_t> g_SteamStubCount{ 0 };
static constexpr uint32_t STEAM_STUB_MAX_COUNT = 1;
static constexpr uint8_t STEAM_STUB_SIGNATURE[] = { 0x44, 0x0F, 0xB6, 0xF8, 0x3C, 0x30, 0x0F, 0x84 };

typedef DWORD(WINAPI* GetTickCount_t)(void);
static GetTickCount_t g_OrigGetTickCount = nullptr;

static uint8_t* SteamStub_FindSignature(uint8_t* start, uint8_t* end, const uint8_t* sig, size_t sigLen)
{
    for (uint8_t* p = start; p < end - sigLen; ++p)
    {
        bool match = true;
        for (size_t i = 0; i < sigLen; ++i)
        {
            if (p[i] != sig[i])
            {
                match = false;
                break;
            }
        }
        if (match)
            return p;
    }
    return nullptr;
}

static DWORD WINAPI SteamStub_HookGetTickCount(void)
{
    uint8_t* returnAddr = reinterpret_cast<uint8_t*>(_ReturnAddress());

    uint8_t* start = returnAddr;
    uint8_t* end = start + 128;

    DWORD oldProtect = 0;
    if (!VirtualProtect(start, static_cast<SIZE_T>(end - start), PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        return g_OrigGetTickCount();
    }

    uint8_t* found = SteamStub_FindSignature(start, end, STEAM_STUB_SIGNATURE, sizeof(STEAM_STUB_SIGNATURE));
    if (found)
    {
        found[6] = 0x90;
        found[7] = 0xE9;

        uint32_t count = g_SteamStubCount.fetch_add(1, std::memory_order_seq_cst) + 1;
        if (count >= STEAM_STUB_MAX_COUNT)
        {
            MH_DisableHook(reinterpret_cast<LPVOID*>(GetTickCount));
        }
    }

    VirtualProtect(start, static_cast<SIZE_T>(end - start), oldProtect, &oldProtect);

    return g_OrigGetTickCount();
}

static void SteamStub_Init()
{
    if (MH_Initialize() != MH_OK)
        return;

    void* pTarget = reinterpret_cast<void*>(GetTickCount);

    if (MH_CreateHook(pTarget, SteamStub_HookGetTickCount, reinterpret_cast<LPVOID*>(&g_OrigGetTickCount)) != MH_OK)
        return;

    if (MH_EnableHook(pTarget) != MH_OK)
        return;

    UCOLOG("[UCOnline2] SteamStub hook initialized");
}

// ============================================================
// CDLLLoader (duplicated here for core DLL)
// ============================================================

class CDLLLoader
{
private:
    std::vector<HMODULE> m_Modules;
    char m_IniPath[MAX_PATH];

public:
    CDLLLoader() { m_IniPath[0] = '\0'; }
    ~CDLLLoader() { UnloadAll(); }

    void ReadConfig()
    {
        char exeDir[MAX_PATH] = { 0 };
        DWORD len = GetModuleFileNameA(nullptr, exeDir, MAX_PATH);
        if (len == 0) return;
        if (!PathRemoveFileSpecA(exeDir)) return;

        _snprintf_s(m_IniPath, MAX_PATH, _TRUNCATE, "%s\\union-crax.ini", exeDir);

        DWORD attribs = GetFileAttributesA(m_IniPath);
        if (attribs == INVALID_FILE_ATTRIBUTES)
            m_IniPath[0] = '\0';
    }

    uint32_t GetAppId()
    {
        if (m_IniPath[0] == '\0')
            return 480;

        char buf[16] = { 0 };
        GetPrivateProfileStringA("Settings", "AppId", "480", buf, sizeof(buf), m_IniPath);

        if (buf[0] == '\0')
            return 480;

        uint32_t id = (uint32_t)strtoul(buf, nullptr, 10);
        return (id == 0) ? 480 : id;
    }

    uint32_t GetOgAppId()
    {
        if (m_IniPath[0] == '\0')
            return 0;

        char buf[16] = { 0 };
        GetPrivateProfileStringA("Settings", "ogAppId", "", buf, sizeof(buf), m_IniPath);

        if (buf[0] == '\0')
            return 0;

        uint32_t id = (uint32_t)strtoul(buf, nullptr, 10);
        return id;
    }

    bool GetSteamStubEnabled()
    {
        if (m_IniPath[0] == '\0')
            return false;

        char buf[8] = { 0 };
        GetPrivateProfileStringA("Settings", "GetStubbedLol", "false", buf, sizeof(buf), m_IniPath);

        return (_stricmp(buf, "true") == 0 || _stricmp(buf, "1") == 0 || _stricmp(buf, "yes") == 0);
    }

    void LoadPlugins()
    {
        if (m_IniPath[0] == '\0')
            return;

        char exeDir[MAX_PATH] = { 0 };
        GetModuleFileNameA(nullptr, exeDir, MAX_PATH);
        PathRemoveFileSpecA(exeDir);

        char folderName[MAX_PATH] = { 0 };
        GetPrivateProfileStringA("Settings", "PluginsFolder", "", folderName, MAX_PATH, m_IniPath);

        if (folderName[0] == '\0')
            return;

        char dllPath[MAX_PATH] = { 0 };
        if (_snprintf_s(dllPath, MAX_PATH, _TRUNCATE, "%s\\%s", exeDir, folderName) == _TRUNCATE)
            return;

        DWORD folderAttribs = GetFileAttributesA(dllPath);
        if (folderAttribs == INVALID_FILE_ATTRIBUTES || !(folderAttribs & FILE_ATTRIBUTE_DIRECTORY))
            return;

        char findPattern[MAX_PATH] = { 0 };
        if (_snprintf_s(findPattern, MAX_PATH, _TRUNCATE, "%s\\*.dll", dllPath) == _TRUNCATE)
            return;

        std::vector<std::string> names;
        std::vector<std::string> paths;

        WIN32_FIND_DATAA fd = { 0 };
        HANDLE hFind = FindFirstFileA(findPattern, &fd);

        if (hFind == INVALID_HANDLE_VALUE)
            return;

        do
        {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                continue;

            char fullPath[MAX_PATH] = { 0 };
            if (_snprintf_s(fullPath, MAX_PATH, _TRUNCATE, "%s\\%s", dllPath, fd.cFileName) == _TRUNCATE)
                continue;

            names.push_back(fd.cFileName);
            paths.push_back(fullPath);
        } while (FindNextFileA(hFind, &fd));

        FindClose(hFind);

        for (size_t i = 0; i < names.size(); i++)
        {
            size_t minIdx = i;
            for (size_t j = i + 1; j < names.size(); j++)
            {
                if (_stricmp(names[j].c_str(), names[minIdx].c_str()) < 0)
                    minIdx = j;
            }

            if (minIdx != i)
            {
                std::swap(names[i], names[minIdx]);
                std::swap(paths[i], paths[minIdx]);
            }
        }

        for (size_t i = 0; i < names.size(); i++)
        {
            HMODULE hMod = LoadLibraryExA(paths[i].c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
            if (hMod)
            {
                m_Modules.push_back(hMod);
                UCOLOG("[UCOnline2] Loaded plugin: %s", names[i].c_str());
            }
            else
            {
                UCOLOG("[UCOnline2] Failed to load plugin: %s (error %lu)", names[i].c_str(), GetLastError());
            }
        }
    }

    void UnloadAll()
    {
        for (size_t i = 0; i < m_Modules.size(); i++)
        {
            if (m_Modules[i])
                FreeLibrary(m_Modules[i]);
        }
        m_Modules.clear();
    }

    size_t LoadedCount() const { return m_Modules.size(); }
};

// ============================================================
// Core DLL Implementation
// ============================================================

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        g_hModule = hModule;
        InitializeLogging();
        s_pPluginLoader = new CDLLLoader();
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        if (s_pPluginLoader)
        {
            s_pPluginLoader->UnloadAll();
            delete s_pPluginLoader;
            s_pPluginLoader = nullptr;
        }
        ShutdownLogging();
    }
    return TRUE;
}

extern "C" {

// These errors are to be expected in the IDE.
UC_CORE_API void UC_Core_Init()
{
    UCOLOG("[UCOnline2] UC_Core_Init called");
    
    s_pPluginLoader->ReadConfig();
    s_ForcedAppId = s_pPluginLoader->GetAppId();
    s_OriginalAppId = s_pPluginLoader->GetOgAppId();

    UCOLOG("[UCOnline2] AppID: %u, Original AppID: %u", s_ForcedAppId, s_OriginalAppId);

    UC_Core_SetAppIDEnv();
    UC_Core_WriteAppIDFile();

    s_pPluginLoader->LoadPlugins();
    UCOLOG("[UCOnline2] %zu plugin(s) loaded", s_pPluginLoader->LoadedCount());

    g_bSteamStubEnabled = s_pPluginLoader->GetSteamStubEnabled();
    UCOLOG("[UCOnline2] SteamStub enabled: %s", g_bSteamStubEnabled ? "true" : "false");
    
    if (g_bSteamStubEnabled)
    {
        UCOLOG("[UCOnline2] Initializing SteamStub");
        SteamStub_Init();
    }
    else
    {
        UCOLOG("[UCOnline2] SteamStub disabled, skipping initialization");
    }
    
    UCOLOG("[UCOnline2] UC_Core_Init completed");
}

UC_CORE_API void UC_Core_Shutdown()
{
    if (s_pPluginLoader)
    {
        s_pPluginLoader->UnloadAll();
    }
}

UC_CORE_API uint32_t UC_Core_GetAppId()
{
    return s_ForcedAppId;
}

UC_CORE_API uint32_t UC_Core_GetOgAppId()
{
    return s_OriginalAppId;
}

UC_CORE_API bool UC_Core_GetSteamStubEnabled()
{
    return g_bSteamStubEnabled;
}

UC_CORE_API void UC_Core_SetAppIDEnv()
{
    char szApp[16] = { 0 };
    char szGame[32] = { 0 };
    char szOverlayGame[32] = { 0 };

    _snprintf_s(szApp, sizeof(szApp), _TRUNCATE, "%u", s_ForcedAppId);
    _snprintf_s(szGame, sizeof(szGame), _TRUNCATE, "%llu", CGameID(s_ForcedAppId).ToUint64());

    uint32_t overlayAppId = (s_OriginalAppId != 0) ? s_OriginalAppId : s_ForcedAppId;
    _snprintf_s(szOverlayGame, sizeof(szOverlayGame), _TRUNCATE, "%llu", CGameID(overlayAppId).ToUint64());

    SetEnvironmentVariableA("SteamAppId", szApp);
    SetEnvironmentVariableA("SteamGameId", szGame);
    SetEnvironmentVariableA("SteamOverlayGameId", szOverlayGame);
}

UC_CORE_API void UC_Core_WriteAppIDFile()
{
    char buf[16] = { 0 };
    _snprintf_s(buf, sizeof(buf), _TRUNCATE, "%u\n", s_ForcedAppId);

    FILE* f = nullptr;
    if (fopen_s(&f, "steam_appid.txt", "wb") == 0 && f)
    {
        fwrite(buf, 1, strlen(buf), f);
        fclose(f);
    }

    char exePath[MAX_PATH] = { 0 };
    if (GetModuleFileNameA(nullptr, exePath, MAX_PATH) != 0)
    {
        if (PathRemoveFileSpecA(exePath))
        {
            char fullPath[MAX_PATH] = { 0 };
            _snprintf_s(fullPath, sizeof(fullPath), _TRUNCATE, "%s\\steam_appid.txt", exePath);

            f = nullptr;
            if (fopen_s(&f, fullPath, "wb") == 0 && f)
            {
                fwrite(buf, 1, strlen(buf), f);
                fclose(f);
            }
        }
    }
}

} // extern "C"