/**
 *  hook_shim.h -- one way to patch an interface method on both platforms.
 *
 *  On Windows this is MinHook: an inline patch over the function body, so
 *  every caller of that body lands in the detour no matter which vtable
 *  points at it. That is why the Windows build can hook "the" ISteamApps
 *  implementation and still catch a game that asked for an older version.
 *
 *  On Linux there is no MinHook, and there does not need to be one: the
 *  things the emulator spoofs are all VTABLE SLOTS. A slot lives in the
 *  interface's vtable, which the linker puts in .data.rel.ro and the
 *  loader then maps read-only. mprotect() can add write access back to
 *  that page (the mapping is private, so the write is copy-on-write and
 *  nothing else in the system sees it), and from then on the slot points
 *  at our detour.
 *
 *  The difference in scope is worth stating plainly: a MinHook inline
 *  patch catches EVERY caller of the function body, while a vtable patch
 *  catches every caller that goes through THAT vtable. The emulator
 *  already handles the per-version case (it hooks each interface version
 *  as it is handed out, see UcoInstallUserAuthHooks and
 *  UcoInstallRemoteStorageHooks), so the coverage is the same in practice.
 *
 *  Both platforms expose the same call:
 *
 *      PlatHookVTableMethod(vt, index, detour, &original)
 *
 *  where `vt` is the vtable pointer of the interface object and `index`
 *  the slot. `original` receives the previous implementation so a detour
 *  that needs to call through can.
 *
 *  ~vλ<3
 */
#pragma once

// Logging comes from globals.h; declared here so this header can be
// included on its own.
void UCOLOG(const char* fmt, ...);

#if defined(_WIN32)

#include "MinHook.h"

enum PlatHookStatus
{
    PLAT_HOOK_OK = 0,
    PLAT_HOOK_ALREADY,
    PLAT_HOOK_FAILED,
    PLAT_HOOK_UNSUPPORTED,
};

// Hook the method at `index` of the vtable `vt`.
//
// The Windows build keeps MinHook's inline patch so the existing
// behaviour (one body, many vtables, one hook) is preserved exactly.
inline PlatHookStatus PlatHookVTableMethod(void** vt, int index, void* detour, void** original)
{
    if (!vt || index < 0 || !detour) return PLAT_HOOK_FAILED;

    void* target = vt[index];
    if (!target) return PLAT_HOOK_FAILED;

    MH_STATUS st = MH_CreateHook(target, detour, original);
    if (st == MH_ERROR_ALREADY_CREATED)
    {
        // The same implementation is shared by another version's vtable;
        // it is already hooked, so there is nothing left to do.
        return PLAT_HOOK_ALREADY;
    }
    if (st != MH_OK)
    {
        UCOLOG("[UCOnline2] MH_CreateHook failed (vtable[%d]): %d", index, st);
        return PLAT_HOOK_FAILED;
    }
    if (MH_EnableHook(target) != MH_OK)
    {
        UCOLOG("[UCOnline2] MH_EnableHook failed (vtable[%d])", index);
        return PLAT_HOOK_FAILED;
    }
    return PLAT_HOOK_OK;
}

// Hook a plain exported function by address. Used for the steamclient
// exports the emulator traces (Steam_GetGSHandle and friends).
inline PlatHookStatus PlatHookFunction(void* target, void* detour, void** original)
{
    if (!target || !detour) return PLAT_HOOK_FAILED;

    MH_STATUS st = MH_CreateHook(target, detour, original);
    if (st == MH_ERROR_ALREADY_CREATED) return PLAT_HOOK_ALREADY;
    if (st != MH_OK) return PLAT_HOOK_FAILED;
    if (MH_EnableHook(target) != MH_OK) return PLAT_HOOK_FAILED;
    return PLAT_HOOK_OK;
}

inline void PlatHookShutdown()
{
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}

#else // Linux

#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>

enum PlatHookStatus
{
    PLAT_HOOK_OK = 0,
    PLAT_HOOK_ALREADY,
    PLAT_HOOK_FAILED,
    PLAT_HOOK_UNSUPPORTED,
};

// Make the page holding `addr` writable.
//
// The vtables live in .data.rel.ro, which the loader maps read-only
// (GNU_RELRO). mprotect() can add PROT_WRITE back: the mapping is
// MAP_PRIVATE, so the write is copy-on-write and the change is invisible
// to every other process. Verified against a real steamclient.so.
inline bool PlatMakePageWritable(void* addr)
{
    if (!addr) return false;
    const uintptr_t page = (uintptr_t)addr & ~(uintptr_t)4095;
    return mprotect((void*)page, (size_t)4096, PROT_READ | PROT_WRITE) == 0;
}

// Replace the method at `index` of the vtable `vt` with `detour`.
//
// `original` receives the implementation that was in the slot, so a
// detour that wants to call through still can.
inline PlatHookStatus PlatHookVTableMethod(void** vt, int index, void* detour, void** original)
{
    if (!vt || index < 0 || !detour) return PLAT_HOOK_FAILED;

    void* previous = vt[index];
    if (!previous) return PLAT_HOOK_FAILED;

    // Already pointing at our detour -- nothing to do.
    if (previous == detour) return PLAT_HOOK_ALREADY;

    if (!PlatMakePageWritable(&vt[index]))
    {
        UCOLOG("[UCOnline2] mprotect failed for vtable[%d] (%p)", index, (void*)vt);
        return PLAT_HOOK_FAILED;
    }

    if (original && !*original)
        *original = previous;

    vt[index] = detour;
    return PLAT_HOOK_OK;
}

// Hooking a bare exported function would mean rewriting machine code,
// which is not something this build does. The only place that wanted one
// was the steamclient auth tracing, and the real Linux SDK does not
// export those symbols either -- so nothing is lost.
inline PlatHookStatus PlatHookFunction(void* target, void* detour, void** original)
{
    (void)target; (void)detour; (void)original;
    return PLAT_HOOK_UNSUPPORTED;
}

inline void PlatHookShutdown() {}

#endif // !_WIN32
