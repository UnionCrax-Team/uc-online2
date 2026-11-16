// ============================================================
// steam_overlay -- early-load system-DLL proxy: arms the SteamStub bypass and
// (optionally) forces the Steam overlay to attach before graphics startup.
//
// THIS IS NOT A UCONLINE2 PLUGIN. It is a renameable system-DLL proxy that the
// game loads at process start. patch.bat deploys it as version.dll for Unity or
// winmm.dll for Unreal. It sits next to the real game EXE, not in plugins\.
//
// ONE BINARY, TWO IDENTITIES
//   version.dll -- UnityPlayer.dll statically imports it. Passthrough is the
//     GetFileVersionInfo*/VerQueryValue* functions below (resolved lazily from
//     System32\version.dll on first call).
//   winmm.dll   -- Unreal shipping executables statically import it (timeGetTime
//     / timeBeginPeriod). Passthrough is 180 x64 jump thunks (winmm_thunks.asm)
//     tail-jumping through g_winmm_ptrs[], filled from System32\winmm.dll by
//     ResolveWinmm() in DllMain (winmm_resolve.cpp).
// Both identities are statically imported, so this shim's DllMain runs BEFORE the
// exe entry point -- early enough to arm the SteamStub bypass and preload plugin
// hooks. (winmm superseded an earlier XINPUT1_3.dll identity, which UE5 loads too
// late -- after the D3D12 renderer -- so the stub armed after its check had fired.)
//
// WHY THE OVERLAY NEEDS THIS
// The Steam overlay (GameOverlayRenderer64.dll) has to install its DXGI/D3D
// present hook BEFORE the engine creates its swapchain. UCOnline2 loads the
// overlay from steam_api64's DllMain -- but many IL2CPP Unity games don't
// import steam_api64 statically; they P/Invoke it lazily on their first Steam
// call, which is AFTER UnityPlayer has already created the swapchain. By then
// the overlay is too late: it only manages to hook XInput, never the present,
// so Shift+Tab does nothing (confirmed in Steam's own gameoverlay_renderer.txt:
// XInput hooks only, and gameoverlayui64 never spawns).
//
// Build (from plugins\steam_overlay): see steam_overlay.vcxproj ->
// overlay_proxy.dll. The patcher renames it to version.dll or winmm.dll.
// ============================================================
#include <Windows.h>
#include <stdio.h>
#include <stdarg.h>
#include "steamstub_hook.h"   // shared SteamStub bypass (armed early from DllMain)

// Runtime winmm passthrough (winmm_resolve.cpp) -- used only when this binary is
// deployed AS winmm.dll. Fills the g_winmm_ptrs[] slots the winmm export thunks
// (winmm_thunks.asm) jump through.
extern "C" void ResolveWinmm(HMODULE self);

static HMODULE g_Module        = nullptr;
static HMODULE g_SystemProxy   = nullptr;
static HMODULE g_Overlay       = nullptr;

static bool IsLogEnabled()
{
    char iniPath[MAX_PATH] = {};
    GetModuleFileNameA(nullptr, iniPath, MAX_PATH);
    char* slash = strrchr(iniPath, '\\');
    if (!slash) return false;
    strcpy_s(slash + 1, MAX_PATH - (size_t)(slash + 1 - iniPath), "union-crax.ini");

    char value[16] = {};
    GetPrivateProfileStringA("Settings", "LogOverlay", "no",
        value, sizeof(value), iniPath);
    return _stricmp(value, "yes") == 0 ||
           _stricmp(value, "true") == 0 ||
           _stricmp(value, "on") == 0 ||
           strcmp(value, "1") == 0;
}

// ---- tiny log, written next to this DLL (i.e. the game folder) ----
static void Log(const char* fmt, ...)
{
    if (!IsLogEnabled()) return;

    char path[MAX_PATH] = {};
    if (g_Module)
    {
        GetModuleFileNameA(g_Module, path, MAX_PATH);
        char* slash = strrchr(path, '\\');
        if (slash) strcpy_s(slash + 1, MAX_PATH - (size_t)(slash + 1 - path), "steam_overlay.log");
    }
    if (!path[0]) return;

    FILE* f = nullptr;
    if (fopen_s(&f, path, "a") == 0 && f)
    {
        va_list ap; va_start(ap, fmt); vfprintf(f, fmt, ap); va_end(ap);
        fputc('\n', f);
        fclose(f);
    }
}

// Read the spoofed AppId from steam_appid.txt next to the exe (default 480).
static void ReadAppId(char* out, DWORD cch)
{
    strcpy_s(out, cch, "480");
    char exe[MAX_PATH] = {};
    GetModuleFileNameA(nullptr, exe, MAX_PATH);
    char* slash = strrchr(exe, '\\');
    if (!slash) return;
    strcpy_s(slash + 1, MAX_PATH - (size_t)(slash + 1 - exe), "steam_appid.txt");

    FILE* f = nullptr;
    if (fopen_s(&f, exe, "rb") == 0 && f)
    {
        char buf[32] = {};
        size_t n = fread(buf, 1, sizeof(buf) - 1, f);
        fclose(f);
        // Keep only the leading digits.
        DWORD w = 0;
        for (size_t i = 0; i < n && w + 1 < cch; ++i)
        {
            if (buf[i] < '0' || buf[i] > '9') break;
            out[w++] = buf[i];
        }
        out[w] = '\0';
        if (!out[0]) strcpy_s(out, cch, "480");
    }
}

// The overlay reads SteamGameId to know which app it's overlaying, and won't
// arm without it. Set the identity BEFORE loading the overlay, but never clobber
// a value the launcher already provided.
static void EnsureEnv(const char* name, const char* value)
{
    char cur[64] = {};
    if (GetEnvironmentVariableA(name, cur, sizeof(cur)) == 0)
        SetEnvironmentVariableA(name, value);
}

// Steam install path from the registry (HKCU first, HKLM fallback).
static bool GetSteamPath(char* out, DWORD cch)
{
    HKEY k;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Valve\\Steam", 0, KEY_READ, &k) == ERROR_SUCCESS)
    {
        DWORD type = 0, sz = cch;
        LONG r = RegQueryValueExA(k, "SteamPath", nullptr, &type, (BYTE*)out, &sz);
        RegCloseKey(k);
        if (r == ERROR_SUCCESS && type == REG_SZ)
        {
            for (char* p = out; *p; ++p) if (*p == '/') *p = '\\';   // Steam stores forward slashes
            return true;
        }
    }
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\Valve\\Steam", 0, KEY_READ, &k) == ERROR_SUCCESS)
    {
        DWORD type = 0, sz = cch;
        LONG r = RegQueryValueExA(k, "InstallPath", nullptr, &type, (BYTE*)out, &sz);
        RegCloseKey(k);
        if (r == ERROR_SUCCESS && type == REG_SZ)
            return true;
    }
    return false;
}

// Read a boolean flag from union-crax.ini next to the exe.
static bool ReadIniBool(const char* section, const char* key, bool defVal)
{
    char iniPath[MAX_PATH] = {};
    GetModuleFileNameA(nullptr, iniPath, MAX_PATH);
    char* slash = strrchr(iniPath, '\\');
    if (!slash) return defVal;
    strcpy_s(slash + 1, MAX_PATH - (size_t)(slash + 1 - iniPath), "union-crax.ini");

    char value[16] = {};
    GetPrivateProfileStringA(section, key, defVal ? "yes" : "no",
        value, sizeof(value), iniPath);
    return _stricmp(value, "yes") == 0 || _stricmp(value, "true") == 0 ||
           _stricmp(value, "on") == 0 || strcmp(value, "1") == 0;
}

// Read a string value from union-crax.ini next to the exe. Returns false (and
// leaves out empty) when the key is missing.
static bool ReadIniString(const char* section, const char* key, char* out, DWORD cch)
{
    out[0] = '\0';
    char iniPath[MAX_PATH] = {};
    GetModuleFileNameA(nullptr, iniPath, MAX_PATH);
    char* slash = strrchr(iniPath, '\\');
    if (!slash) return false;
    strcpy_s(slash + 1, MAX_PATH - (size_t)(slash + 1 - iniPath), "union-crax.ini");
    return GetPrivateProfileStringA(section, key, "", out, cch, iniPath) > 0 && out[0];
}

// Locate the UCOnline2 steam_api64.dll to preload. Checks next to the exe
// (Unreal / generic) first, then the Unity layout <root>\*_Data\Plugins\x86_64.
static bool FindSteamApiDll(char* out, DWORD cch)
{
    char root[MAX_PATH] = {};
    GetModuleFileNameA(nullptr, root, MAX_PATH);
    char* slash = strrchr(root, '\\');
    if (!slash) return false;
    *slash = '\0';   // root = game directory

    _snprintf_s(out, cch, _TRUNCATE, "%s\\steam_api64.dll", root);
    if (GetFileAttributesA(out) != INVALID_FILE_ATTRIBUTES) return true;

    char pattern[MAX_PATH] = {};
    _snprintf_s(pattern, MAX_PATH, _TRUNCATE, "%s\\*_Data", root);
    WIN32_FIND_DATAA fd = {};
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h != INVALID_HANDLE_VALUE)
    {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
            _snprintf_s(out, cch, _TRUNCATE,
                "%s\\%s\\Plugins\\x86_64\\steam_api64.dll", root, fd.cFileName);
            if (GetFileAttributesA(out) != INVALID_FILE_ATTRIBUTES) { FindClose(h); return true; }
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }
    return false;
}

// Runs on its own thread so it isn't holding the loader lock. It still lands
// well before graphics init: version.dll is loaded as a UnityPlayer import,
// long before the exe's entry point, so this fires while the engine is still
// starting up.
static DWORD WINAPI LoaderThread(void*)
{
    // [VersionProxy] LoadDLLsEarly -- preload UCOnline2's steam_api64.dll here,
    // BEFORE the game runs. Unity P/Invokes steam_api64 lazily on its first
    // Steam call, which for an EOS/PlayFab game can land AFTER the game has
    // already created its backend platform -- too late for a plugin (e.g.
    // EOS_custom) to redirect it. version.dll loads before graphics init, so
    // preloading steam_api64 here arms those plugin hooks in time.
    if (ReadIniBool("VersionProxy", "LoadDLLsEarly", false))
    {
        if (GetModuleHandleA("steam_api64.dll"))
        {
            Log("[steam_overlay] LoadDLLsEarly: steam_api64.dll already loaded");
        }
        else
        {
            char dll[MAX_PATH] = {};
            if (FindSteamApiDll(dll, sizeof(dll)))
            {
                HMODULE h = LoadLibraryExA(dll, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
                Log("[steam_overlay] LoadDLLsEarly: %s (%s)",
                    h ? "loaded steam_api64.dll EARLY" : "FAILED to load steam_api64.dll", dll);
            }
            else
            {
                Log("[steam_overlay] LoadDLLsEarly: steam_api64.dll not found");
            }
        }
    }

    // The overlay renderer is separate from the preload above. Skip it when the
    // overlay is disabled -- e.g. D3D12 titles where hooking the swapchain can
    // destabilise renderer bring-up.
    if (!ReadIniBool("Settings", "LoadOverlay", true))
    {
        Log("[steam_overlay] LoadOverlay disabled -- overlay not loaded");
        return 0;
    }

    if (g_Overlay || GetModuleHandleA("GameOverlayRenderer64.dll"))
    {
        Log("[steam_overlay] overlay already present -- nothing to do");
        return 0;
    }

    char appId[32] = {};
    ReadAppId(appId, sizeof(appId));   // spoofed id (480) -- the overlay target

    // [VersionProxy] SdrSafe -- for a game using Steam Datagram Relay, the
    // process's Steam context must be the REAL AppId: relay authorisation is
    // keyed off it, and preloading steamclient/overlay as spacewar here binds
    // the process to 480 before UCO2's SDR path runs, so the relay refuses to
    // route. Mirror UCO2's own split (SetAppIDEnv): put ogAppId on the context
    // vars and steer the overlay at the spoofed id via SteamOverlayGameId.
    char ogAppId[32] = {};
    if (ReadIniBool("VersionProxy", "SdrSafe", false) &&
        ReadIniString("Settings", "ogAppId", ogAppId, sizeof(ogAppId)))
    {
        SetEnvironmentVariableA("SteamAppId", ogAppId);
        SetEnvironmentVariableA("SteamGameId", ogAppId);
        EnsureEnv("SteamOverlayGameId", appId);
        Log("[steam_overlay] SdrSafe: Steam context AppId=%s, overlay id=%s", ogAppId, appId);
    }
    else
    {
        EnsureEnv("SteamAppId", appId);
        EnsureEnv("SteamGameId", appId);
    }
    EnsureEnv("SteamClientLaunch", "1");

    char steam[MAX_PATH] = {};
    if (!GetSteamPath(steam, sizeof(steam)))
    {
        Log("[steam_overlay] could not resolve Steam path from the registry");
        return 0;
    }

    char path[MAX_PATH] = {};
    // Preload steamclient64 so the overlay's dependencies resolve.
    if (!GetModuleHandleA("steamclient64.dll"))
    {
        _snprintf_s(path, MAX_PATH, _TRUNCATE, "%s\\steamclient64.dll", steam);
        LoadLibraryExA(path, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
    }

    _snprintf_s(path, MAX_PATH, _TRUNCATE, "%s\\GameOverlayRenderer64.dll", steam);
    g_Overlay = LoadLibraryExA(path, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
    Log("[steam_overlay] appid=%s  %s  (%s)",
        appId, g_Overlay ? "loaded GameOverlayRenderer64 EARLY" : "FAILED to load overlay", path);
    return 0;
}

// ---- version.dll passthrough ----
//
// The binary's export table is the union of the version.dll and winmm.dll entry
// points, so it satisfies whichever identity it was renamed to. version.dll's
// functions forward lazily by name through here; winmm.dll's forward through the
// asm thunks instead (see winmm_resolve.cpp), so they never reach InitProxy.
static void InitProxy()
{
    if (g_SystemProxy) return;

    char modulePath[MAX_PATH] = {};
    GetModuleFileNameA(g_Module, modulePath, MAX_PATH);
    const char* filename = strrchr(modulePath, '\\');
    filename = filename ? filename + 1 : modulePath;

    // Only the version.dll identity forwards through this lazy, by-name path.
    // The winmm.dll identity forwards through the asm thunks + ResolveWinmm()
    // instead, so its exports never reach Resolve<T>() -- nothing to do here.
    const char* systemName = nullptr;
    if (_stricmp(filename, "version.dll") == 0)
        systemName = "version.dll";
    else
    {
        Log("[steam_overlay] Resolve<T> called under unexpected proxy filename: %s", filename);
        return;
    }

    char systemPath[MAX_PATH] = {};
    GetSystemDirectoryA(systemPath, MAX_PATH);
    strcat_s(systemPath, MAX_PATH, "\\");
    strcat_s(systemPath, MAX_PATH, systemName);
    g_SystemProxy = LoadLibraryA(systemPath);
    if (!g_SystemProxy)
        Log("[steam_overlay] failed to load system proxy target: %s (error %lu)",
            systemPath, GetLastError());
}

template <typename T>
static T Resolve(const char* name)
{
    InitProxy();
    return g_SystemProxy ? reinterpret_cast<T>(GetProcAddress(g_SystemProxy, name)) : nullptr;
}

extern "C" DWORD WINAPI Proxy_GetFileVersionInfoSizeA(LPCSTR file, LPDWORD handle)
{ using Fn = DWORD (WINAPI*)(LPCSTR, LPDWORD); auto f=Resolve<Fn>("GetFileVersionInfoSizeA"); return f ? f(file,handle) : 0; }
extern "C" DWORD WINAPI Proxy_GetFileVersionInfoSizeW(LPCWSTR file, LPDWORD handle)
{ using Fn = DWORD (WINAPI*)(LPCWSTR, LPDWORD); auto f=Resolve<Fn>("GetFileVersionInfoSizeW"); return f ? f(file,handle) : 0; }
extern "C" DWORD WINAPI Proxy_GetFileVersionInfoSizeExA(DWORD flags, LPCSTR file, LPDWORD handle)
{ using Fn = DWORD (WINAPI*)(DWORD,LPCSTR,LPDWORD); auto f=Resolve<Fn>("GetFileVersionInfoSizeExA"); return f ? f(flags,file,handle) : 0; }
extern "C" DWORD WINAPI Proxy_GetFileVersionInfoSizeExW(DWORD flags, LPCWSTR file, LPDWORD handle)
{ using Fn = DWORD (WINAPI*)(DWORD,LPCWSTR,LPDWORD); auto f=Resolve<Fn>("GetFileVersionInfoSizeExW"); return f ? f(flags,file,handle) : 0; }
extern "C" BOOL WINAPI Proxy_GetFileVersionInfoA(LPCSTR file, DWORD handle, DWORD len, LPVOID data)
{ using Fn = BOOL (WINAPI*)(LPCSTR,DWORD,DWORD,LPVOID); auto f=Resolve<Fn>("GetFileVersionInfoA"); return f ? f(file,handle,len,data) : FALSE; }
extern "C" BOOL WINAPI Proxy_GetFileVersionInfoW(LPCWSTR file, DWORD handle, DWORD len, LPVOID data)
{ using Fn = BOOL (WINAPI*)(LPCWSTR,DWORD,DWORD,LPVOID); auto f=Resolve<Fn>("GetFileVersionInfoW"); return f ? f(file,handle,len,data) : FALSE; }
extern "C" BOOL WINAPI Proxy_GetFileVersionInfoExA(DWORD flags, LPCSTR file, DWORD handle, DWORD len, LPVOID data)
{ using Fn = BOOL (WINAPI*)(DWORD,LPCSTR,DWORD,DWORD,LPVOID); auto f=Resolve<Fn>("GetFileVersionInfoExA"); return f ? f(flags,file,handle,len,data) : FALSE; }
extern "C" BOOL WINAPI Proxy_GetFileVersionInfoExW(DWORD flags, LPCWSTR file, DWORD handle, DWORD len, LPVOID data)
{ using Fn = BOOL (WINAPI*)(DWORD,LPCWSTR,DWORD,DWORD,LPVOID); auto f=Resolve<Fn>("GetFileVersionInfoExW"); return f ? f(flags,file,handle,len,data) : FALSE; }
extern "C" BOOL WINAPI Proxy_GetFileVersionInfoByHandle(HANDLE file, LPVOID data, DWORD len)
{ using Fn = BOOL (WINAPI*)(HANDLE,LPVOID,DWORD); auto f=Resolve<Fn>("GetFileVersionInfoByHandle"); return f ? f(file,data,len) : FALSE; }
extern "C" BOOL WINAPI Proxy_VerQueryValueA(const LPVOID block, LPCSTR sub, LPVOID* value, PUINT len)
{ using Fn = BOOL (WINAPI*)(const LPVOID,LPCSTR,LPVOID*,PUINT); auto f=Resolve<Fn>("VerQueryValueA"); return f ? f(block,sub,value,len) : FALSE; }
extern "C" BOOL WINAPI Proxy_VerQueryValueW(const LPVOID block, LPCWSTR sub, LPVOID* value, PUINT len)
{ using Fn = BOOL (WINAPI*)(const LPVOID,LPCWSTR,LPVOID*,PUINT); auto f=Resolve<Fn>("VerQueryValueW"); return f ? f(block,sub,value,len) : FALSE; }

static void SteamStub_LogCb(const char* m) { Log("[steam_overlay] %s", m); }

// True when this binary was deployed under `name` (case-insensitive base name).
static bool SelfNamed(const char* name)
{
    char path[MAX_PATH] = {};
    GetModuleFileNameA(g_Module, path, MAX_PATH);
    const char* base = strrchr(path, '\\');
    base = base ? base + 1 : path;
    return _stricmp(base, name) == 0;
}

// Is the Steam client running? ActiveProcess\pid holds the live Steam PID and
// is cleared to 0 on a clean exit. Conservative: only a definitive pid==0 counts
// as "not running" -- any ambiguity (registry missing/unreadable) returns true
// so we never wrongly abort a game whose Steam is actually up.
static bool SteamRunning()
{
    HKEY k;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Valve\\Steam\\ActiveProcess",
                      0, KEY_READ, &k) != ERROR_SUCCESS)
        return true;
    DWORD pid = 0, sz = sizeof(pid), type = 0;
    bool read = RegQueryValueExA(k, "pid", nullptr, &type, (BYTE*)&pid, &sz) == ERROR_SUCCESS;
    RegCloseKey(k);
    return !read || pid != 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        g_Module = module;
        DisableThreadLibraryCalls(module);

        // UCOnline2 proxies the REAL Steam client, so a game launched with Steam
        // closed (common for directly-run SteamStub titles) has nothing to
        // connect to and fails confusingly. Warn and abort the launch instead.
        // We only get here before the exe entry point, so terminating now stops
        // the game cleanly. RequireSteam=false opts out.
        if (ReadIniBool("VersionProxy", "RequireSteam", true) && !SteamRunning())
        {
            MessageBoxA(nullptr,
                "Steam isn't running.\n\n"
                "Start Steam and sign in, then launch the game again.",
                "UCOnline2", MB_OK | MB_ICONWARNING | MB_TOPMOST | MB_SETFOREGROUND);
            TerminateProcess(GetCurrentProcess(), 0);
            return FALSE;
        }

        // winmm.dll identity: fill the passthrough slots the export thunks
        // (winmm_thunks.asm) tail-jump through, from the real System32\winmm.dll.
        // Done here in DllMain -- which runs before the exe entry point -- so the
        // slots are ready before the game makes its first winmm call. No-op when
        // deployed as version.dll (Unity forwards through Resolve<T> instead).
        if (SelfNamed("winmm.dll"))
            ResolveWinmm(g_Module);

        // Arm the SteamStub bypass HERE -- synchronously, in DllMain, which runs
        // before the exe entry point where the stub's ownership check fires. On
        // a Unity game steam_api64 loads too late (lazy P/Invoke), and a wrapped
        // exe that fails the check exits before steam_api64 ever loads. Signal
        // steam_api64 (which also honours GetStubbedLol) so it doesn't hook
        // GetTickCount a second time. Safe in DllMain: only the main thread
        // exists this early, so MinHook's thread snapshot suspends nothing.
        if (ReadIniBool("Settings", "GetStubbedLol", false) &&
            UcoSteamStub::Install(&SteamStub_LogCb))
            SetEnvironmentVariableA("UCO2_STEAMSTUB_HOOKED", "1");

        HANDLE t = CreateThread(nullptr, 0, LoaderThread, nullptr, 0, nullptr);
        if (t) CloseHandle(t);
    }
    return TRUE;
}
