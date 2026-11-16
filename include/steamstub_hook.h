#pragma once
// SteamStub (Steam's DRM exe-wrapper) runtime bypass, shared by steam_api64.dll
// and the early-load proxy (version.dll).
//
// The stub runs an ownership check at the EXE ENTRY POINT and, on failure,
// exits with "Application load error" before any lazily-loaded DLL (a Unity
// game's steam_api64) is touched. So for those games the hook has to be armed
// from the early proxy's DllMain -- which runs before the entry point -- not
// from steam_api64, which loads too late. Only ONE module should arm it per
// process; coordinate via the UCO2_STEAMSTUB_HOOKED environment variable.
//
// Credits: DenuvoSanctuary's steam-stubbed (originally Rust). Signatures follow
// the current upstream (K0oRui fork): the check is `cmp al/bl, 30h ; jz owned`,
// and we rewrite the JZ to an unconditional JMP so the pass branch is always
// taken. x86-64 focused. Requires MinHook. All state is static (per-DLL).
#include <windows.h>
#include <intrin.h>
#include <atomic>
#include <cstdint>
#include "MinHook.h"

namespace UcoSteamStub {

typedef void (*LogFn)(const char*);
static LogFn s_log = nullptr;
static inline void Log(const char* m) { if (s_log) s_log(m); }

// Each pattern ends at the ownership check's JZ rel32 (0F 84); patchAt is the
// offset of the 0F within the match. We overwrite 0F 84 with 90 E9 (nop + jmp
// rel32), reusing the JZ's own operand so the jump lands on the same target
// unconditionally.
struct Sig { const uint8_t* bytes; size_t len; size_t patchAt; };

static constexpr uint8_t kSigAl[] = { 0x3C, 0x30, 0x0F, 0x84 };        // cmp al, 30h ; jz
static constexpr uint8_t kSigBl[] = { 0x80, 0xFB, 0x30, 0x0F, 0x84 };  // cmp bl, 30h ; jz
static constexpr Sig kSigs[] = {
    { kSigAl, sizeof(kSigAl), 2 },
    { kSigBl, sizeof(kSigBl), 3 },
};

typedef DWORD (WINAPI* GetTickCount_t)(void);
static GetTickCount_t     s_orig  = nullptr;
static std::atomic<bool>  s_armed{ false };

// Scan [base, base+range) under SEH so a window running past a mapped page
// can't fault the host. No objects with destructors here, so __try is legal.
static uint8_t* FindSig(uint8_t* base, size_t range, const Sig& sig)
{
    __try
    {
        if (range < sig.len)
            return nullptr;
        uint8_t* end = base + range - sig.len;
        for (uint8_t* p = base; p <= end; ++p)
        {
            bool match = true;
            for (size_t i = 0; i < sig.len; ++i)
                if (p[i] != sig.bytes[i]) { match = false; break; }
            if (match)
                return p;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return nullptr;
    }
    return nullptr;
}

static bool ApplyPatch(uint8_t* site, const Sig& sig)
{
    uint8_t* at = site + sig.patchAt;
    DWORD oldProtect = 0;
    if (!VirtualProtect(at, 2, PAGE_EXECUTE_READWRITE, &oldProtect))
        return false;
    at[0] = 0x90; // nop (was 0x0F)
    at[1] = 0xE9; // jmp rel32, keeps the JZ's operand (was 0x84)
    VirtualProtect(at, 2, oldProtect, &oldProtect);
    FlushInstructionCache(GetCurrentProcess(), at, 2);
    return true;
}

// The stub's check calls GetTickCount; the return address points just past the
// check. We never MH_DisableHook from inside the detour (unsafe on our own
// trampoline) -- once armed we stay installed as a pass-through, and the CAS
// makes only the first thread patch under a race.
static DWORD WINAPI Hook(void)
{
    DWORD tick = s_orig ? s_orig() : 0;

    if (!s_armed.load(std::memory_order_acquire))
    {
        uint8_t* ret = reinterpret_cast<uint8_t*>(_ReturnAddress());
        for (const auto& sig : kSigs)
        {
            uint8_t* site = FindSig(ret, 128, sig);
            if (!site)
                continue;
            bool expected = false;
            if (s_armed.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
                Log(ApplyPatch(site, sig) ? "SteamStub armed: jz->jmp patched"
                                          : "SteamStub VirtualProtect failed");
            break;
        }
    }

    return tick;
}

// Arm the GetTickCount hook. Safe from an early DllMain: at process init only
// the main thread exists, so MinHook's thread snapshot has nothing to suspend.
// Returns true when the hook is enabled.
static inline bool Install(LogFn log)
{
    s_log = log;
    MH_STATUS s = MH_Initialize();
    if (s != MH_OK && s != MH_ERROR_ALREADY_INITIALIZED)
        return false;
    void* target = reinterpret_cast<void*>(GetTickCount);
    if (MH_CreateHook(target, reinterpret_cast<void*>(Hook),
                      reinterpret_cast<void**>(&s_orig)) != MH_OK)
        return false;
    return MH_EnableHook(target) == MH_OK;
}

} // namespace UcoSteamStub
