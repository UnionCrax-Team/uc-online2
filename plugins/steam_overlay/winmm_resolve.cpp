// ============================================================
// winmm_resolve.cpp -- runtime passthrough for the winmm.dll identity.
//
// When overlay_proxy.dll is deployed AS winmm.dll (the Unreal identity), the
// game imports winmm functions from us. The export thunks in winmm_thunks.asm
// tail-jump through g_winmm_ptrs[], which we fill here from the REAL winmm in
// System32.
//
// WHY RUNTIME (not .def forwarders): a .def forwarder to "winmm.func" would be
// circular (we ARE winmm.dll), and forwarding to an absolute System32 path
// depends on the loader parsing a device-path forwarder string -- fragile
// across Windows builds. Loading System32\winmm.dll by full path is robust: the
// modern loader keys its already-loaded check on the full path, so it maps the
// real winmm as a SEPARATE module from our same-named proxy (verified against
// OnlineFix's working winmm proxy, where the real winmm and the proxy coexist).
// GetProcAddress on that module therefore returns the real functions, not our
// own thunks.
// ============================================================
#include <Windows.h>
#include "winmm_names.h"   // kWinmmNames[], kWinmmCount

// Slots the asm thunks jump through. One per named winmm export, ordinal order
// (index i == ordinal 3+i). Zero-initialised: an unresolved slot is null, and a
// thunk that hits a null slot jumps to address 0 -- but ResolveWinmm() runs in
// DllMain, before the game's first winmm call, so by then every slot is filled.
extern "C" { void* g_winmm_ptrs[kWinmmCount] = { 0 }; }

// Fill g_winmm_ptrs from the real System32\winmm.dll. `self` is our own module
// handle: if the loader ever handed us back OURSELVES (a same-base-name collision
// on some ancient loader), binding to it would make every thunk recurse into
// itself -- so refuse that and leave the slots null (winmm calls become no-ops
// returning 0) rather than hang. On every supported Windows this loads the real
// winmm as a distinct module and fills all slots.
extern "C" void ResolveWinmm(HMODULE self)
{
    char sys[MAX_PATH] = {};
    UINT n = GetSystemDirectoryA(sys, MAX_PATH);
    if (n == 0 || n >= MAX_PATH - 12) return;
    strcat_s(sys, MAX_PATH, "\\winmm.dll");

    HMODULE h = LoadLibraryA(sys);
    if (!h || h == self) return;

    for (int i = 0; i < kWinmmCount; ++i)
        g_winmm_ptrs[i] = reinterpret_cast<void*>(GetProcAddress(h, kWinmmNames[i]));
}
