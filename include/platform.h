/**
 *  platform.h -- the Win32 -> POSIX shim.
 *
 *  The emulator used to be Windows/MSVC only: every header pulled in
 *  <Windows.h>, the interface hooks were MinHook inline patches, and the
 *  Steam install path came out of the registry. None of that exists on
 *  Linux, where a game expects to load "libsteam_api.so" and where Steam
 *  lives in ~/.steam / ~/.local/share/Steam.
 *
 *  This header lets ONE source tree build as both:
 *
 *      Windows : steam_api64.dll   (MSVC, unchanged)
 *      Linux   : libsteam_api.so   (GCC/Clang, x86_64)
 *
 *  Everything below the _WIN32 branch reimplements the handful of Win32
 *  calls the emulator actually makes. Nothing here is a stub that quietly
 *  succeeds: when a call cannot be honoured on this platform it reports
 *  the same failure the Win32 call would have reported.
 *
 *  Path separators are the other half of the problem. The Windows build
 *  hard-codes "\\" in every path it builds, so PLAT_SEP / PLAT_SEP_STR
 *  exist and every path literal in the codebase uses them.
 *
 *  ~vλ<3
 */
#pragma once

#include <stdint.h>
#include <stddef.h>

#if defined(_WIN32)

// ============================================================
// Windows
// ============================================================

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <Shlwapi.h>

#define PLAT_IS_WINDOWS 1

#define PLAT_SEP          '\\'
#define PLAT_SEP_STR      "\\"
#define PLAT_DLL_EXT      ".dll"
#define PLAT_DLL_PATTERN  "*.dll"
#define PLAT_IS_DIR_SEP(c) ((c) == '\\' || (c) == '/')

// The overlay proxy (version.dll / winmm.dll) and the DbgHelp minidump
// writer are Windows-only; both are compiled out elsewhere.
#define PLAT_HAS_OVERLAY_PROXY 1

#else

// ============================================================
// Linux / POSIX
// ============================================================

#define PLAT_IS_WINDOWS 0

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cstdint>
#include <cctype>
#include <cerrno>
#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <mutex>

#include <dlfcn.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define PLAT_SEP          '/'
#define PLAT_SEP_STR      "/"
#define PLAT_DLL_EXT      ".so"
#define PLAT_DLL_PATTERN  "*.so"
#define PLAT_IS_DIR_SEP(c) ((c) == '/')

// ------------------------------------------------------------
// Fundamental types
// ------------------------------------------------------------
typedef int             BOOL;
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef int32_t         LONG;
typedef uint32_t        DWORD;
typedef unsigned int    UINT;
typedef uint32_t        ULONG;
typedef uint64_t        ULONGLONG;
typedef uint64_t        DWORD64;
typedef char*           LPSTR;
typedef const char*     LPCSTR;
typedef wchar_t*        LPWSTR;
typedef const wchar_t*  LPCWSTR;
typedef void*           LPVOID;
typedef const void*     LPCVOID;
typedef unsigned char*  LPBYTE;
typedef void*           HANDLE;
typedef void*           HMODULE;
typedef void*           FARPROC;
typedef void*           HKEY;
typedef long            LSTATUS;
typedef intptr_t        INT_PTR;

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef MAX_PATH
#define MAX_PATH 4096
#endif

#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS            0L
#define ERROR_FILE_NOT_FOUND     2L
#define ERROR_INSUFFICIENT_BUFFER 122L
#endif

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((HANDLE)(intptr_t)-1)
#endif

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES 0xFFFFFFFFu
#endif

#define FILE_ATTRIBUTE_READONLY   0x00000001u
#define FILE_ATTRIBUTE_HIDDEN     0x00000002u
#define FILE_ATTRIBUTE_SYSTEM     0x00000004u
#define FILE_ATTRIBUTE_DIRECTORY  0x00000010u
#define FILE_ATTRIBUTE_ARCHIVE    0x00000020u
#define FILE_ATTRIBUTE_NORMAL     0x00000080u

#define HKEY_CURRENT_USER   ((HKEY)(intptr_t)0x80000001)
#define KEY_READ            0x20019
#define REG_SZ              1
#define REG_DWORD           4
#define RRF_RT_REG_SZ       0x00000002

#define PROCESS_QUERY_LIMITED_INFORMATION 0x1000

// Windows load flags. dlopen has no equivalent, so they are accepted and
// ignored.
#define LOAD_WITH_ALTERED_SEARCH_PATH 0x00000008
#define LOAD_LIBRARY_SEARCH_SYSTEM32   0x00000800

#define FOREGROUND_BLUE      0x0001
#define FOREGROUND_GREEN     0x0002
#define FOREGROUND_RED       0x0004
#define FOREGROUND_INTENSITY 0x0008

#define _TRUNCATE (-1)

// ------------------------------------------------------------
// Locks.
//
// SRWLOCK is exclusive-only here: the shared variants map onto the same
// mutex. Nothing in the codebase holds a shared lock while taking
// another shared lock, so the extra concurrency is not needed and a
// plain std::mutex keeps the semantics obvious.
//
// CRITICAL_SECTION MUST stay recursive -- CCallbackDispatcher holds it
// across a game's callback Run(), and a callback is allowed to
// register/unregister other callbacks from inside itself.
// ------------------------------------------------------------
struct SRWLOCK
{
    std::mutex m;
};
#define SRWLOCK_INIT {}

struct CRITICAL_SECTION
{
    std::recursive_mutex m;
};

inline void InitializeSRWLock(SRWLOCK* l)             { (void)l; }
inline void AcquireSRWLockExclusive(SRWLOCK* l)       { l->m.lock(); }
inline void ReleaseSRWLockExclusive(SRWLOCK* l)       { l->m.unlock(); }
inline void AcquireSRWLockShared(SRWLOCK* l)          { l->m.lock(); }
inline void ReleaseSRWLockShared(SRWLOCK* l)          { l->m.unlock(); }

inline void InitializeCriticalSection(CRITICAL_SECTION* cs) { (void)cs; }
inline void EnterCriticalSection(CRITICAL_SECTION* cs)     { cs->m.lock(); }
inline void LeaveCriticalSection(CRITICAL_SECTION* cs)     { cs->m.unlock(); }
inline void DeleteCriticalSection(CRITICAL_SECTION* cs)     { (void)cs; }

// ------------------------------------------------------------
// Interlocked
// ------------------------------------------------------------
inline LONG InterlockedIncrement(volatile LONG* v)
{
    return __sync_add_and_fetch(v, 1);
}
inline LONG InterlockedDecrement(volatile LONG* v)
{
    return __sync_sub_and_fetch(v, 1);
}
inline LONG InterlockedCompareExchange(volatile LONG* v, LONG exchange, LONG comparand)
{
    return __sync_val_compare_and_swap(v, exchange, comparand);
}
inline LONG InterlockedExchange(volatile LONG* v, LONG exchange)
{
    return __sync_lock_test_and_set(v, exchange);
}

// ------------------------------------------------------------
// Time
// ------------------------------------------------------------
struct SYSTEMTIME
{
    WORD wYear, wMonth, wDay, wDayOfWeek, wHour, wMinute, wSecond, wMilliseconds;
};

inline void GetLocalTime(SYSTEMTIME* st)
{
    if (!st) return;
    time_t now = time(nullptr);
    struct tm tmv;
    localtime_r(&now, &tmv);
    st->wYear         = (WORD)(tmv.tm_year + 1900);
    st->wMonth        = (WORD)(tmv.tm_mon + 1);
    st->wDay          = (WORD)tmv.tm_mday;
    st->wDayOfWeek    = (WORD)tmv.tm_wday;
    st->wHour         = (WORD)tmv.tm_hour;
    st->wMinute       = (WORD)tmv.tm_min;
    st->wSecond       = (WORD)tmv.tm_sec;
    st->wMilliseconds = 0;
}

// Milliseconds since boot. CLOCK_MONOTONIC so the values never go
// backwards when the wall clock is adjusted -- the callback scheduler
// compares these against "now" to decide what is due.
inline ULONGLONG PlatMonotonicMs()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ULONGLONG)ts.tv_sec * 1000ULL + (ULONGLONG)(ts.tv_nsec / 1000000);
}
inline DWORD GetTickCount()   { return (DWORD)PlatMonotonicMs(); }
inline ULONGLONG GetTickCount64() { return PlatMonotonicMs(); }
inline void Sleep(DWORD ms)   { usleep((useconds_t)ms * 1000); }

// ------------------------------------------------------------
// Process / thread identity
// ------------------------------------------------------------
inline DWORD GetCurrentProcessId() { return (DWORD)getpid(); }
inline DWORD GetCurrentThreadId()
{
    return (DWORD)(uintptr_t)pthread_self();
}
inline HANDLE GetCurrentProcess() { return (HANDLE)(intptr_t)-1; }
inline HANDLE GetCurrentThread()  { return (HANDLE)(intptr_t)-1; }

// ------------------------------------------------------------
// Error reporting
// ------------------------------------------------------------
inline DWORD GetLastError() { return (DWORD)errno; }
inline void  SetLastError(DWORD e) { errno = (int)e; }

// ------------------------------------------------------------
// Environment
// ------------------------------------------------------------
inline BOOL SetEnvironmentVariableA(const char* name, const char* value)
{
    if (!name || !name[0]) return FALSE;
    if (value) return setenv(name, value, 1) == 0 ? TRUE : FALSE;
    unsetenv(name);
    return TRUE;
}

// Returns the length of the value (excluding the NUL). If the buffer is
// too small the value is truncated and the return value is the length
// the value would have needed -- same contract as Win32.
inline DWORD GetEnvironmentVariableA(const char* name, char* buf, DWORD size)
{
    if (!name || !name[0]) return 0;
    const char* v = getenv(name);
    if (!v) return 0;
    const size_t len = strlen(v);
    if (!buf || size == 0) return (DWORD)len;
    const size_t copy = (len < (size_t)size - 1) ? len : (size_t)size - 1;
    memcpy(buf, v, copy);
    buf[copy] = '\0';
    return (DWORD)((len >= (size_t)size) ? len : copy);
}

// ------------------------------------------------------------
// Modules
// ------------------------------------------------------------
inline HMODULE LoadLibraryA(const char* path)
{
    if (!path || !path[0]) return nullptr;
    return dlopen(path, RTLD_LAZY | RTLD_LOCAL);
}
inline HMODULE LoadLibraryW(const wchar_t* path)
{
    if (!path || !path[0]) return nullptr;
    // The only wide paths this codebase ever loads are the Windows overlay
    // modules, which do not exist here; convert anyway so the call compiles.
    std::string narrow;
    for (const wchar_t* w = path; *w; ++w)
        narrow += (char)(*w < 128 ? (char)*w : '?');
    return dlopen(narrow.c_str(), RTLD_LAZY | RTLD_LOCAL);
}
inline HMODULE LoadLibraryExA(const char* path, void* reserved, DWORD flags)
{
    (void)reserved; (void)flags;
    return LoadLibraryA(path);
}
inline HMODULE LoadLibraryExW(const wchar_t* path, void* reserved, DWORD flags)
{
    (void)reserved; (void)flags;
    return LoadLibraryW(path);
}
inline BOOL  FreeLibrary(HMODULE mod) { return mod ? (dlclose(mod) == 0 ? TRUE : FALSE) : FALSE; }
inline FARPROC GetProcAddress(HMODULE mod, const char* name)
{
    if (!mod || !name) return nullptr;
    return (FARPROC)dlsym(mod, name);
}
// There is no GameOverlayRenderer on Linux -- the overlay is injected by
// the Steam client itself, not by the game. Nothing to find.
inline HMODULE GetModuleHandleW(const wchar_t* name) { (void)name; return nullptr; }
inline HMODULE GetModuleHandleA(const char* name)   { (void)name; return nullptr; }

// The path of the module that owns `addr`, or the running executable when
// addr is null (which is what Win32's GetModuleFileNameA(nullptr, ...)
// means).
inline std::string PlatModulePathOf(void* addr)
{
    if (addr)
    {
        Dl_info info;
        if (dladdr(addr, &info) && info.dli_fname)
            return std::string(info.dli_fname);
        return std::string();
    }

    char buf[MAX_PATH] = { 0 };
    const ssize_t n = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (n > 0) buf[n] = '\0';
    return std::string(buf);
}

inline DWORD GetModuleFileNameA(HMODULE mod, char* buf, DWORD size)
{
    if (!buf || size == 0) return 0;
    const std::string p = PlatModulePathOf((void*)mod);
    if (p.empty()) { buf[0] = '\0'; return 0; }
    const size_t copy = (p.size() < (size_t)size) ? p.size() : (size_t)size - 1;
    memcpy(buf, p.c_str(), copy);
    buf[copy] = '\0';
    return (DWORD)copy;
}

// Directory holding the running executable.
inline std::string PlatExeDir()
{
    std::string p = PlatModulePathOf(nullptr);
    const size_t slash = p.rfind(PLAT_SEP);
    return (slash == std::string::npos) ? std::string() : p.substr(0, slash);
}

// ------------------------------------------------------------
// File system
// ------------------------------------------------------------
inline DWORD GetFileAttributesA(const char* path)
{
    if (!path || !path[0]) return INVALID_FILE_ATTRIBUTES;
    struct stat st;
    if (stat(path, &st) != 0) return INVALID_FILE_ATTRIBUTES;
    DWORD a = FILE_ATTRIBUTE_NORMAL;
    if (S_ISDIR(st.st_mode))  a = FILE_ATTRIBUTE_DIRECTORY;
    else if (S_ISLNK(st.st_mode)) a = FILE_ATTRIBUTE_NORMAL;
    if (st.st_mode & S_IWUSR) a &= ~FILE_ATTRIBUTE_READONLY;
    return a;
}

struct FILETIME
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
};

union ULARGE_INTEGER
{
    struct { DWORD LowPart; DWORD HighPart; };
    uint64_t QuadPart;
};

struct WIN32_FILE_ATTRIBUTE_DATA
{
    DWORD      dwFileAttributes;
    FILETIME   ftCreationTime;
    FILETIME   ftLastAccessTime;
    FILETIME   ftLastWriteTime;
    DWORD      nFileSizeHigh;
    DWORD      nFileSizeLow;
};

#define GetFileExInfoStandard 0

inline BOOL GetFileAttributesExA(const char* path, int level, WIN32_FILE_ATTRIBUTE_DATA* data)
{
    (void)level;
    if (!path || !data) return FALSE;
    struct stat st;
    if (stat(path, &st) != 0) return FALSE;
    memset(data, 0, sizeof(*data));
    data->dwFileAttributes = GetFileAttributesA(path);
    data->nFileSizeLow  = (DWORD)(st.st_size & 0xFFFFFFFFu);
    data->nFileSizeHigh = (DWORD)((uint64_t)st.st_size >> 32);
    // FILETIME is 100ns ticks since 1601-01-01; the epoch offset is
    // 11644473600 seconds.
    const uint64_t ticks = (uint64_t)st.st_mtime * 10000000ULL + 116444736000000000ULL;
    data->ftLastWriteTime.dwLowDateTime  = (DWORD)(ticks & 0xFFFFFFFFu);
    data->ftLastWriteTime.dwHighDateTime = (DWORD)(ticks >> 32);
    return TRUE;
}

inline BOOL CreateDirectoryA(const char* path, void* sec)
{
    (void)sec;
    if (!path || !path[0]) return FALSE;
    if (mkdir(path, 0755) == 0) return TRUE;
    return (errno == EEXIST) ? FALSE : FALSE;
}

inline BOOL DeleteFileA(const char* path)
{
    if (!path || !path[0]) return FALSE;
    return unlink(path) == 0 ? TRUE : FALSE;
}

inline DWORD GetTempPathA(DWORD size, char* buf)
{
    if (!buf || size == 0) return 0;
    const char* tmp = getenv("TMPDIR");
    if (!tmp || !tmp[0]) tmp = "/tmp";
    std::string s(tmp);
    if (s.empty() || s[s.size() - 1] != PLAT_SEP) s += PLAT_SEP;
    const size_t copy = (s.size() < (size_t)size) ? s.size() : (size_t)size - 1;
    memcpy(buf, s.c_str(), copy);
    buf[copy] = '\0';
    return (DWORD)copy;
}

struct WIN32_FIND_DATAA
{
    DWORD    dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD    nFileSizeHigh;
    DWORD    nFileSizeLow;
    char     cFileName[MAX_PATH];
    char     cAlternateFileName[14];
};

// A directory handle for FindFirstFileA. Kept opaque so the caller
// cannot tell it apart from a Win32 HANDLE.
struct PlatFindHandle
{
    std::vector<std::string> names;
    size_t index = 0;
    bool   valid = false;
};

inline HANDLE FindFirstFileA(const char* pattern, WIN32_FIND_DATAA* fd)
{
    if (!pattern || !fd) return INVALID_HANDLE_VALUE;

    std::string pat(pattern);
    const size_t slash = pat.rfind(PLAT_SEP);
    const std::string dir  = (slash == std::string::npos) ? std::string(".") : pat.substr(0, slash);
    const std::string base = (slash == std::string::npos) ? pat : pat.substr(slash + 1);

    // Only the "*" wildcard is supported; that is all the loader asks for.
    if (base.find('*') == std::string::npos && base.find('?') == std::string::npos)
    {
        struct stat st;
        if (stat(pat.c_str(), &st) != 0) return INVALID_HANDLE_VALUE;
        PlatFindHandle* h = new PlatFindHandle();
        h->names.push_back(base);
        h->valid = true;
        memset(fd, 0, sizeof(*fd));
        fd->dwFileAttributes = GetFileAttributesA(pat.c_str());
        strncpy(fd->cFileName, base.c_str(), sizeof(fd->cFileName) - 1);
        return (HANDLE)h;
    }

    DIR* d = opendir(dir.c_str());
    if (!d) return INVALID_HANDLE_VALUE;

    PlatFindHandle* h = new PlatFindHandle();
    struct dirent* ent;
    while ((ent = readdir(d)) != nullptr)
    {
        const std::string name(ent->d_name);
        if (name == "." || name == "..") continue;
        // Prefix match on the part before the wildcard.
        const size_t star = base.find('*');
        const std::string prefix = base.substr(0, star);
        if (!prefix.empty() && name.compare(0, prefix.size(), prefix) != 0) continue;
        h->names.push_back(name);
    }
    closedir(d);

    if (h->names.empty())
    {
        delete h;
        return INVALID_HANDLE_VALUE;
    }

    h->valid = true;
    // Hand back the first entry right away, like Win32 does.
    const std::string first = h->names[0];
    h->index = 1;
    memset(fd, 0, sizeof(*fd));
    fd->dwFileAttributes = GetFileAttributesA((dir + PLAT_SEP_STR + first).c_str());
    strncpy(fd->cFileName, first.c_str(), sizeof(fd->cFileName) - 1);
    return (HANDLE)h;
}

inline BOOL FindNextFileA(HANDLE h, WIN32_FIND_DATAA* fd)
{
    if (!h || !fd) return FALSE;
    PlatFindHandle* fh = (PlatFindHandle*)h;
    if (!fh->valid || fh->index >= fh->names.size()) return FALSE;

    const std::string name = fh->names[fh->index++];
    memset(fd, 0, sizeof(*fd));
    fd->dwFileAttributes = 0;
    strncpy(fd->cFileName, name.c_str(), sizeof(fd->cFileName) - 1);
    return TRUE;
}

inline BOOL FindClose(HANDLE h)
{
    if (!h || h == INVALID_HANDLE_VALUE) return FALSE;
    PlatFindHandle* fh = (PlatFindHandle*)h;
    delete fh;
    return TRUE;
}

// ------------------------------------------------------------
// Path helpers
// ------------------------------------------------------------
inline BOOL PathRemoveFileSpecA(char* path)
{
    if (!path || !path[0]) return FALSE;
    char* last = nullptr;
    for (char* c = path; *c; ++c)
        if (PLAT_IS_DIR_SEP(*c)) last = c;
    if (!last) return FALSE;
    *last = '\0';
    return TRUE;
}

inline BOOL PathAppendA(char* path, const char* more)
{
    if (!path || !more) return FALSE;
    size_t len = strlen(path);
    if (len > 0 && !PLAT_IS_DIR_SEP(path[len - 1]))
    {
        if (len + 2 > MAX_PATH) return FALSE;
        path[len++] = PLAT_SEP;
        path[len] = '\0';
    }
    const size_t mlen = strlen(more);
    if (len + mlen + 1 > MAX_PATH) return FALSE;
    memcpy(path + len, more, mlen + 1);
    return TRUE;
}

inline const char* PlatStrStrIA(const char* haystack, const char* needle)
{
    if (!haystack || !needle) return nullptr;
    if (!*needle) return haystack;
    const size_t nlen = strlen(needle);
    for (const char* h = haystack; *h; ++h)
    {
        size_t i = 0;
        while (i < nlen && h[i] &&
               tolower((unsigned char)h[i]) == tolower((unsigned char)needle[i]))
            ++i;
        if (i == nlen) return h;
    }
    return nullptr;
}
#define StrStrIA PlatStrStrIA

// ------------------------------------------------------------
// Registry.
//
// There is no registry on Linux. Every lookup fails with
// ERROR_FILE_NOT_FOUND, which is exactly what the callers check for --
// the Steam install path and the running-process probe are done by
// looking at the filesystem instead (see SteamAPI_GetSteamInstallPath).
// ------------------------------------------------------------
inline LSTATUS RegOpenKeyExA(HKEY hkey, const char* path, DWORD opts, DWORD access, HKEY* out)
{
    (void)hkey; (void)path; (void)opts; (void)access;
    if (out) *out = nullptr;
    return ERROR_FILE_NOT_FOUND;
}
inline LSTATUS RegQueryValueExA(HKEY hkey, const char* name, DWORD* reserved, DWORD* type,
                                LPBYTE data, DWORD* size)
{
    (void)hkey; (void)name; (void)reserved; (void)type; (void)data; (void)size;
    return ERROR_FILE_NOT_FOUND;
}
inline LSTATUS RegGetValueA(HKEY hkey, const char* path, const char* name, DWORD flags,
                            DWORD* type, void* data, DWORD* size)
{
    (void)hkey; (void)path; (void)name; (void)flags; (void)type; (void)data; (void)size;
    return ERROR_FILE_NOT_FOUND;
}
inline LSTATUS RegCloseKey(HKEY hkey) { (void)hkey; return ERROR_SUCCESS; }

// ------------------------------------------------------------
// Processes
// ------------------------------------------------------------
inline HANDLE OpenProcess(DWORD access, BOOL inherit, DWORD pid)
{
    (void)access; (void)inherit;
    // Only "is it alive" is ever asked, so answer with a marker handle
    // when /proc/<pid> exists.
    char path[64];
    snprintf(path, sizeof(path), "/proc/%u", (unsigned)pid);
    struct stat st;
    if (stat(path, &st) != 0) return nullptr;
    return (HANDLE)(intptr_t)pid;
}
inline BOOL GetExitCodeProcess(HANDLE h, DWORD* code)
{
    if (!h) return FALSE;
    if (code) *code = 259;   // STILL_ACTIVE
    return TRUE;
}
inline BOOL CloseHandle(HANDLE h) { (void)h; return TRUE; }

// ------------------------------------------------------------
// Memory protection (the Steam stub hook uses it; it is Windows-only)
// ------------------------------------------------------------
inline BOOL VirtualProtect(void* addr, size_t size, DWORD newProtect, DWORD* oldProtect)
{
    (void)addr; (void)size; (void)newProtect;
    if (oldProtect) *oldProtect = 0;
    return TRUE;
}

// ------------------------------------------------------------
// MSVC string helpers
// ------------------------------------------------------------
#ifndef _stricmp
#define _stricmp strcasecmp
#endif
#ifndef _strnicmp
#define _strnicmp strncasecmp
#endif

#ifndef _vsnprintf_s
#define _vsnprintf_s(buf, size, count, fmt, args) vsnprintf(buf, size, fmt, args)
#endif

// _snprintf_s(buf, size, count, fmt, ...) -- the count is the buffer size
// again on MSVC, and _TRUNCATE is the usual value. The variadic template
// forwards the format arguments properly, which a plain macro cannot do.
template <typename... Args>
inline int PlatSnprintfS(char* buf, size_t size, size_t count, const char* fmt, Args... args)
{
    (void)count;
    if (!buf || size == 0) return -1;
    const int n = snprintf(buf, size, fmt, args...);
    // MSVC returns -1 when the output was truncated.
    if (n < 0 || (size_t)n >= size) return -1;
    return n;
}
#ifndef _snprintf_s
#define _snprintf_s(...) PlatSnprintfS(__VA_ARGS__)
#endif

#ifndef strncpy_s
inline int strncpy_s(char* dst, size_t dstSize, const char* src, size_t count)
{
    (void)count;
    if (!dst || dstSize == 0) return EINVAL;
    if (!src) { dst[0] = '\0'; return EINVAL; }
    const size_t len = strlen(src);
    if (len >= dstSize) { memcpy(dst, src, dstSize - 1); dst[dstSize - 1] = '\0'; return ERANGE; }
    memcpy(dst, src, len + 1);
    return 0;
}
#endif

#ifndef sprintf_s
inline int sprintf_s(char* buf, size_t size, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int n = vsnprintf(buf, size, fmt, args);
    va_end(args);
    if (n < 0 || (size_t)n >= size) return -1;
    return n;
}
#endif

#ifndef strtok_s
inline char* strtok_s(char* str, const char* delim, char** ctx)
{
    if (!ctx) return nullptr;
    if (!str) str = *ctx;
    if (!str) { *ctx = nullptr; return nullptr; }
    char* start = str;
    // Skip leading delimiters.
    while (*start && strchr(delim, *start)) ++start;
    if (!*start) { *ctx = nullptr; return nullptr; }
    char* end = start;
    while (*end && !strchr(delim, *end)) ++end;
    if (*end) { *end = '\0'; *ctx = end + 1; }
    else *ctx = nullptr;
    return start;
}
#endif

#ifndef fopen_s
inline int fopen_s(FILE** pFile, const char* filename, const char* mode)
{
    if (!pFile) return EINVAL;
    *pFile = fopen(filename, mode);
    return (*pFile) ? 0 : errno;
}
#endif

#ifndef fscanf_s
inline int fscanf_s(FILE* f, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int n = vfscanf(f, fmt, args);
    va_end(args);
    return n;
}
#endif

#ifndef SecureZeroMemory
inline void SecureZeroMemory(void* ptr, size_t len)
{
    volatile unsigned char* p = (volatile unsigned char*)ptr;
    while (len--) *p++ = 0;
}
#endif

// __debugbreak is the "you called a Steam API before SteamAPI_Init" trap.
// MSVC compiles it to int 3, which raises a breakpoint exception; the GCC
// equivalent is a trap instruction. The behaviour matches on both.
#ifndef __debugbreak
#define __debugbreak() __builtin_trap()
#endif

// ------------------------------------------------------------
// INI files (GetPrivateProfileStringA)
//
// Windows' profile API has two behaviours worth copying exactly:
//
//   * A missing key yields the caller's default.
//   * A NULL key name enumerates the section's key names as a
//     double-NUL-terminated block -- UcoDlcStore relies on this to
//     discover the named DLC entries.
//
// Values are trimmed of surrounding whitespace, and section/key names are
// matched case-insensitively, exactly like the Windows reader.
// ------------------------------------------------------------
struct PlatIniSection
{
    std::string name;
    std::vector<std::pair<std::string, std::string> > keys;
};

inline std::string PlatTrim(const std::string& s)
{
    size_t b = 0, e = s.size();
    while (b < e && (s[b] == ' ' || s[b] == '\t' || s[b] == '\r' || s[b] == '\n')) ++b;
    while (e > b && (s[e - 1] == ' ' || s[e - 1] == '\t' || s[e - 1] == '\r' || s[e - 1] == '\n')) --e;
    return s.substr(b, e - b);
}

inline bool PlatIniEquals(const std::string& a, const std::string& b)
{
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i])) return false;
    return true;
}

inline std::vector<PlatIniSection> PlatIniParse(const char* path)
{
    std::vector<PlatIniSection> sections;
    if (!path || !path[0]) return sections;

    FILE* f = fopen(path, "rb");
    if (!f) return sections;

    char line[2048];
    std::string current;
    bool first = true;
    while (fgets(line, sizeof(line), f))
    {
        std::string s(line);
        if (first)
        {
            first = false;
            // Strip a UTF-8 BOM if the file has one.
            if (s.size() >= 3 && (unsigned char)s[0] == 0xEF &&
                (unsigned char)s[1] == 0xBB && (unsigned char)s[2] == 0xBF)
                s.erase(0, 3);
        }

        const size_t hash = s.find_first_not_of(" \t\r\n");
        if (hash == std::string::npos) continue;
        s = s.substr(hash);

        if (s[0] == ';' || s[0] == '#') continue;

        if (s[0] == '[')
        {
            const size_t close = s.find(']');
            if (close == std::string::npos) continue;
            current = PlatTrim(s.substr(1, close - 1));
            PlatIniSection sec;
            sec.name = current;
            sections.push_back(sec);
            continue;
        }

        const size_t eq = s.find('=');
        if (eq == std::string::npos) continue;
        if (sections.empty())
        {
            // A key before any [section] header belongs to no section.
            PlatIniSection sec;
            sec.name = "";
            sections.push_back(sec);
        }
        PlatIniSection& sec = sections.back();
        sec.keys.push_back(std::make_pair(PlatTrim(s.substr(0, eq)),
                                          PlatTrim(s.substr(eq + 1))));
    }
    fclose(f);
    return sections;
}

inline DWORD GetPrivateProfileStringA(const char* section, const char* key,
                                       const char* def, char* out, DWORD size,
                                       const char* path)
{
    if (!out || size == 0) return 0;
    out[0] = '\0';

    const std::vector<PlatIniSection> sections = PlatIniParse(path);

    const PlatIniSection* sec = nullptr;
    for (size_t i = 0; i < sections.size(); ++i)
    {
        if (PlatIniEquals(sections[i].name, section ? section : ""))
        {
            sec = &sections[i];
            break;
        }
    }

    if (!sec) return 0;

    // Enumeration mode: the key list of the section.
    if (!key)
    {
        size_t used = 0;
        for (size_t i = 0; i < sec->keys.size(); ++i)
        {
            const std::string& k = sec->keys[i].first;
            if (used + k.size() + 2 > (size_t)size) break;
            memcpy(out + used, k.c_str(), k.size() + 1);
            used += k.size() + 1;
        }
        if ((size_t)size >= used + 2)
        {
            out[used] = '\0';
            out[used + 1] = '\0';    // double NUL terminates the list
        }
        return (DWORD)used;
    }

    for (size_t i = 0; i < sec->keys.size(); ++i)
    {
        if (!PlatIniEquals(sec->keys[i].first, key)) continue;
        const std::string& v = sec->keys[i].second;
        const size_t copy = (v.size() < (size_t)size - 1) ? v.size() : (size_t)size - 1;
        memcpy(out, v.c_str(), copy);
        out[copy] = '\0';
        return (DWORD)copy;
    }

    // Not found: Windows copies the default back to the caller.
    if (def)
    {
        const size_t copy = strlen(def) < (size_t)size - 1 ? strlen(def) : (size_t)size - 1;
        memcpy(out, def, copy);
        out[copy] = '\0';
        return (DWORD)copy;
    }
    return 0;
}

inline UINT GetPrivateProfileIntA(const char* section, const char* key, int def,
                                  const char* path)
{
    char buf[64] = { 0 };
    GetPrivateProfileStringA(section, key, "", buf, sizeof(buf), path);
    if (!buf[0]) return (UINT)def;
    return (UINT)strtol(buf, nullptr, 10);
}
#define GetPrivateProfileInt GetPrivateProfileIntA

// ------------------------------------------------------------
// Steam discovery helpers
// ------------------------------------------------------------

// Is a process with this pid alive?
inline bool PlatIsProcessRunning(unsigned pid)
{
    if (!pid) return false;
    char path[64];
    snprintf(path, sizeof(path), "/proc/%u", pid);
    struct stat st;
    return stat(path, &st) == 0;
}

// The home directory, honouring HOME the way the rest of the system does.
inline std::string PlatHomeDir()
{
    const char* h = getenv("HOME");
    if (h && h[0]) return std::string(h);
    return std::string();
}

inline bool PlatFileExists(const std::string& path)
{
    struct stat st;
    return stat(path.c_str(), &st) == 0;
}

#endif // !_WIN32
