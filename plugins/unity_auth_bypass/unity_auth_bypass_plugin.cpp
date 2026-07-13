// ============================================================
// UCOnline2 plugin -- Unity Gaming Services auth bypass
//
// Many modern Unity games gate multiplayer behind Unity
// Gaming Services (UGS) authentication. The typical flow is:
//
//   AuthenticationService.Instance.SignInWithSteamAsync(ticket)
//
// internally:
//   1. Fetch a Steam auth ticket via ISteamUser
//   2. POST { ticket, appId } to Unity's UGS backend
//   3. UGS asks Steam Web API "is this ticket entitled to
//      AppId X?"
//   4. If yes -> AuthenticationService.IsSignedIn = true,
//      PlayerInfo populated, game proceeds.
//   5. If no  -> SignInFailed event, game shows an error like
//      "Failed to get Steam account information" or "Not
//      signed into Unity Services" and refuses to start
//      multiplayer.
//
// With UCOnline2 spoofing AppId = 480 (Spacewar), step 3
// always fails: the ticket is for app 480 but UGS asks Steam
// about the real AppId.
//
// This plugin sidesteps the problem by hooking
// SignInWithSteamAsync and redirecting every call to
// SignInAnonymouslyAsync. UGS still creates a real, valid
// player session -- it just isn't tied to a Steam account.
// The game's "IsSignedIn" / "PlayerInfo" checks pass and the
// multiplayer flow proceeds normally.
//
// Confirmed needed for:
//   - Phasmophobia (AppId 739630, in conjunction with the
//                   photon_universal plugin -- which also bundles
//                   an equivalent Phasmo gate NOP)
//
// MinHook is statically linked into this DLL.
// ============================================================
#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../../include/MinHook.h"
#include "../../include/uco_plugin.h"

#include "il2cpp_runtime.h"

// ============================================================
// Plugin-local state
// ============================================================
static UCO_LogFn g_Log              = nullptr;
static volatile LONG g_bShutdown    = 0;
static HANDLE    g_hWatcherThread   = nullptr;

#define LOG(...) do { if (g_Log) g_Log(__VA_ARGS__); } while (0)

extern "C" void IL2CPP_Log(const char* fmt, ...)
{
    if (!g_Log) return;
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    _vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, ap);
    va_end(ap);
    g_Log("%s", buf);
}

// ============================================================
// Byte-pattern scanner for GameAssembly.dll
//
// Phasmophobia uses Beebyte-style obfuscation that renames every
// class/method to garbage Unicode at runtime, so name-based IL2CPP
// lookup can't find SteamAuth. But the compiled native code is
// untouched -- we sig-scan for the method's distinctive prologue.
// Same technique as OFM's fix (which patches RVA 0x42CA4A0 of
// GameAssembly.dll in our exact Phasmo build).
//
// Signature is the first ~33 bytes of SteamAuth's entry-point:
// 4-arg layout (rcx->rsi, rdx->r14, r8->rbp, r9->rdi), 0x40 stack
// frame, class-init flag check, 0x67 short jump skipping the
// init chain. The class-init flag RVA varies per build, hence
// the wildcards.
// ============================================================
struct SigByte { uint8_t value; bool wildcard; };

static const SigByte kSteamAuthSig[] = {
    {0x48,false},{0x89,false},{0x6C,false},{0x24,false},{0x18,false},  // mov [rsp+0x18], rbp
    {0x56,false},                                                       // push rsi
    {0x57,false},                                                       // push rdi
    {0x41,false},{0x56,false},                                          // push r14
    {0x48,false},{0x83,false},{0xEC,false},{0x40,false},                // sub rsp, 0x40
    {0x80,false},{0x3D,false},                                          // cmp byte [rip+...], 0
    {0x00,true}, {0x00,true}, {0x00,true}, {0x00,true},                 // (RVA wildcards)
    {0x00,false},                                                       //   ", 0
    {0x49,false},{0x8B,false},{0xF9,false},                             // mov rdi, r9
    {0x49,false},{0x8B,false},{0xE8,false},                             // mov rbp, r8
    {0x4C,false},{0x8B,false},{0xF2,false},                             // mov r14, rdx
    {0x48,false},{0x8B,false},{0xF1,false},                             // mov rsi, rcx
    {0x75,false},{0x67,false},                                          // jne +0x67
};

static uint8_t* SigScan(uint8_t* base, size_t size, const SigByte* sig, size_t sigLen)
{
    if (size < sigLen) return nullptr;
    size_t end = size - sigLen;
    for (size_t i = 0; i <= end; ++i)
    {
        bool ok = true;
        for (size_t j = 0; j < sigLen; ++j)
        {
            if (!sig[j].wildcard && base[i + j] != sig[j].value) { ok = false; break; }
        }
        if (ok) return base + i;
    }
    return nullptr;
}

// Looks up a section by name (max 8 chars). Returns false if not
// found. IL2CPP games keep compiled C# code in a section called
// "il2cpp" separate from the runtime's own ".text" -- callers
// must check both for IL2CPP-method byte signatures.
static bool GetSection(HMODULE mod, const char* name,
                       uint8_t** outBase, size_t* outSize)
{
    if (!mod) return false;
    uint8_t* image = (uint8_t*)mod;
    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)image;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return false;
    IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(image + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return false;
    IMAGE_SECTION_HEADER* sec = IMAGE_FIRST_SECTION(nt);
    char padded[9] = {};
    strncpy_s(padded, sizeof(padded), name, 8);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i)
    {
        if (memcmp(sec[i].Name, padded, 8) == 0)
        {
            *outBase = image + sec[i].VirtualAddress;
            *outSize = sec[i].Misc.VirtualSize;
            return true;
        }
    }
    return false;
}

// ============================================================
// Hook: Phasmo's obfuscated SteamAuth entry-point
//
// 4-arg method (this + 3 ptrs). The original calls into
// Steamworks.NET, fetches an auth ticket, POSTs it to Kinetic
// Games' backend, fails because we spoofed AppID 480, then
// throws "Failed to get Steam account information". Returning
// null short-circuits before the HTTP POST. Caller is async
// (looking at the prologue's caller-saved layout) and tolerates
// the null return.
// ============================================================
typedef void* (__fastcall *Fn_SteamAuth4Arg)(void* pThis, void* a1, void* a2, void* a3);
static Fn_SteamAuth4Arg g_pfnOrigPhasmoSteamAuth = nullptr;

static void* __fastcall Hooked_PhasmoSteamAuth(void* pThis, void* a1, void* a2, void* a3)
{
    LOG("[Auth] Phasmo SteamAuth intercepted (this=%p a1=%p a2=%p a3=%p) -> returning null",
        pThis, a1, a2, a3);
    return nullptr;
}

static bool TryPatchPhasmoSteamAuth()
{
    HMODULE ga = GetModuleHandleA("GameAssembly.dll");
    if (!ga) return false;

    // IL2CPP-compiled methods live in the "il2cpp" section, NOT
    // ".text" (.text contains the IL2CPP runtime itself). The
    // SteamAuth method we're after is at RVA 0x42CA4A0 in our
    // Phasmo build, which falls inside "il2cpp".
    uint8_t* secBase = nullptr;
    size_t   secSize = 0;
    if (!GetSection(ga, "il2cpp", &secBase, &secSize))
    {
        LOG("[Auth] could not locate GameAssembly.dll 'il2cpp' section "
            "-- not an IL2CPP build?");
        return false;
    }
    LOG("[Auth] scanning GameAssembly.dll il2cpp section (base=%p size=0x%zx) "
        "for Phasmo SteamAuth signature...", secBase, secSize);

    // Diag: dump the bytes at OFM's known target RVA (0x42CA4A0)
    // so we can compare runtime bytes vs the on-disk bytes our
    // signature is derived from. If these differ, the file is
    // being modified at load time (Beebyte runtime patches,
    // section decompression, etc).
    uint8_t* probe = (uint8_t*)ga + 0x42CA4A0;
    char hexbuf[256] = {};
    int off = 0;
    for (int i = 0; i < 48 && off < (int)sizeof(hexbuf) - 4; ++i)
    {
        off += _snprintf_s(hexbuf + off, sizeof(hexbuf) - off, _TRUNCATE,
                           "%02X ", probe[i]);
    }
    LOG("[Auth] runtime bytes at GameAssembly+0x42CA4A0: %s", hexbuf);

    size_t sigLen = sizeof(kSteamAuthSig) / sizeof(kSteamAuthSig[0]);
    uint8_t* hit = SigScan(secBase, secSize, kSteamAuthSig, sigLen);
    if (!hit)
    {
        LOG("[Auth] Phasmo SteamAuth signature NOT FOUND in GameAssembly.dll .text "
            "-- not Phasmophobia or signature has drifted across builds");
        return false;
    }

    uintptr_t rva = (uintptr_t)(hit - (uint8_t*)ga);
    LOG("[Auth] Phasmo SteamAuth signature matched at %p (RVA 0x%llx)",
        hit, (unsigned long long)rva);

    if (MH_CreateHook(hit, (void*)&Hooked_PhasmoSteamAuth,
                      (void**)&g_pfnOrigPhasmoSteamAuth) != MH_OK ||
        MH_EnableHook(hit) != MH_OK)
    {
        LOG("[Auth] failed to install Phasmo SteamAuth hook at %p", hit);
        return false;
    }
    LOG("[Auth] Phasmo SteamAuth hook installed at %p", hit);
    return true;
}

// ============================================================
// Patch: SteamAuth "Failed to get Steam account information"
// error gate.
//
// Phasmo's obfuscated SteamAuth MoveNext (the async state
// machine for the sign-in async method) calls an inner check
// that returns true if the Steam ticket validates. When the
// check returns false the state machine falls into a branch
// that loads the "Failed to get Steam account information"
// string literal and feeds it to the error display path. UI
// shows the popup and multiplayer never reaches Photon.
//
// The check itself is unbypassable without real Steam (it's
// downstream of GetAuthSessionTicketForWebApi). But the
// branch that consumes its result is a single je rel32:
//
//   xor    ecx, ecx
//   call   <inner check>
//   test   al, al
//   je     <error block>          <-- THIS
//
// Replacing the je with 6 NOPs makes the failure branch
// unreachable. Execution flows into the success path
// regardless of what the check returned.
//
// Located in Phasmo build 23249745 at GameAssembly.dll RVA
// 0xEAF7CE. Byte-verified before patching.
// ============================================================
static bool TryPatchPhasmoSteamAccountGate()
{
    HMODULE ga = GetModuleHandleA("GameAssembly.dll");
    if (!ga) return false;

    // Build-specific RVA. If this drifts, the byte-verify below
    // catches it -- we don't blindly write.
    const uintptr_t kPatchRva = 0xEAF7CE;
    uint8_t* site = (uint8_t*)ga + kPatchRva;

    const uint8_t kExpected[6] = { 0x0F, 0x84, 0xC3, 0x06, 0x00, 0x00 };
    if (memcmp(site, kExpected, 6) != 0)
    {
        char hex[64] = {};
        int off = 0;
        for (int i = 0; i < 6 && off < (int)sizeof(hex) - 4; ++i)
            off += _snprintf_s(hex + off, sizeof(hex) - off, _TRUNCATE,
                               "%02X ", site[i]);
        LOG("[Auth] SteamAccountGate: byte mismatch at GameAssembly+0x%llx "
            "(found: %s) -- build drifted, skipping patch",
            (unsigned long long)kPatchRva, hex);
        return false;
    }

    DWORD oldProt = 0;
    if (!VirtualProtect(site, 6, PAGE_EXECUTE_READWRITE, &oldProt))
    {
        LOG("[Auth] SteamAccountGate: VirtualProtect failed, GLE=%lu",
            GetLastError());
        return false;
    }
    for (int i = 0; i < 6; ++i) site[i] = 0x90;
    DWORD tmp = 0;
    VirtualProtect(site, 6, oldProt, &tmp);
    FlushInstructionCache(GetCurrentProcess(), site, 6);

    LOG("[Auth] SteamAccountGate: NOPed je at GameAssembly+0x%llx "
        "-- 'Failed to get Steam account information' branch now unreachable",
        (unsigned long long)kPatchRva);
    return true;
}

// ============================================================
// Hook: AuthenticationServiceInternal.SignInWithSteamAsync
//
// Generic signature covering every overload we've seen in the
// wild (1 / 2 / 3 explicit args after `this`):
//
//   Task SignInWithSteamAsync(string ticket)
//   Task SignInWithSteamAsync(string ticket, SignInOptions options)
//   Task SignInWithSteamAsync(string ticket, string identity, SignInOptions options)
//
// We declare 3 explicit args after `this`. Extras passed by
// the game land in R8/R9/stack and are simply ignored, which
// is safe under x64 __fastcall. We then forward to
// SignInAnonymouslyAsync, returning its Task object so the
// caller's `await` resolves normally with a real signed-in
// state.
// ============================================================
typedef void* (__fastcall *Fn_SignInTask)(void* pThis, void* a1, void* a2, void* a3);

static Fn_SignInTask g_pfnSignInAnonymous       = nullptr;
static Fn_SignInTask g_pfnOrigSignInWithSteam   = nullptr;

static void* __fastcall Hooked_SignInWithSteam(void* pThis, void* ticket, void* a2, void* a3)
{
    LOG("[Auth] SignInWithSteamAsync intercepted -> SignInAnonymouslyAsync (this=%p)", pThis);
    if (!g_pfnSignInAnonymous)
    {
        LOG("[Auth] WARNING: SignInAnonymouslyAsync not resolved, falling through");
        return g_pfnOrigSignInWithSteam(pThis, ticket, a2, a3);
    }
    // SignInAnonymouslyAsync is typically:
    //   Task SignInAnonymouslyAsync()
    //   Task SignInAnonymouslyAsync(SignInOptions options)
    // Passing nullptr for the options arg works for both.
    return g_pfnSignInAnonymous(pThis, nullptr, nullptr, nullptr);
}

// ============================================================
// Hook: AuthenticationServiceInternal.UpdatePlayerNameAsync
//
// After UGS sign-in succeeds, Phasmo calls UpdatePlayerNameAsync
// to register the player's display name. With UCOnline2 spoofing
// every cracked-Phasmo user to the same SteamID + display name,
// the UGS PlayerNames API (which requires unique names) returns
// HTTP 409 Conflict -- many users have already registered the
// default name. The resulting RequestFailedException propagates
// up to Phasmo's UI, which shows "Failed to get Steam account
// information".
//
// We short-circuit the call: return Task.CompletedTask without
// contacting UGS at all. The caller's await resolves successfully
// and Phasmo's flow continues to the multiplayer screen.
//
// Signature: Task UpdatePlayerNameAsync(string playerName)
// ============================================================
typedef void* (__fastcall *Fn_get_CompletedTask)();
typedef void* (__fastcall *Fn_UpdatePlayerName)(void* pThis, void* playerName);

static Fn_get_CompletedTask g_pfnGetCompletedTask     = nullptr;
static Fn_UpdatePlayerName  g_pfnOrigUpdatePlayerName = nullptr;

static void* __fastcall Hooked_UpdatePlayerName(void* pThis, void* playerName)
{
    LOG("[Auth] UpdatePlayerNameAsync intercepted -> Task.CompletedTask (skipping UGS round-trip)");
    if (g_pfnGetCompletedTask)
    {
        void* task = g_pfnGetCompletedTask();
        return task;
    }
    // Fallback: pass through to original. The 409 will resurface
    // but at least we won't NullRef the caller.
    LOG("[Auth] WARNING: Task.get_CompletedTask not resolved, falling through");
    return g_pfnOrigUpdatePlayerName(pThis, playerName);
}

// ============================================================
// Hook: AuthenticationServiceInternal.LinkWithSteamAsync
//
// After our SignInWithSteamAsync redirect creates an anonymous
// UGS session, Phasmo tries to LINK the Steam identity to it
// via LinkWithSteamAsync. UGS validates the Steam ticket
// server-side (more strictly than sign-in) and rejects our
// spoofed ticket with 401 PERMISSION_DENIED ("invalid token").
//
// We can't mint a valid ticket -- only Steam can. So we
// short-circuit the link call: return Task.CompletedTask
// (or equivalent) without contacting UGS. The anonymous
// session remains active but Phasmo thinks the Steam link
// succeeded.
// ============================================================
typedef void* (__fastcall *Fn_LinkTask)(void* pThis, void* a1, void* a2, void* a3);
static Fn_LinkTask g_pfnOrigLinkWithSteam = nullptr;

static void* __fastcall Hooked_LinkWithSteam(void* pThis, void* ticket, void* a2, void* a3)
{
    LOG("[Auth] LinkWithSteamAsync intercepted -> Task.CompletedTask (skipping UGS link)");
    if (g_pfnGetCompletedTask)
    {
        return g_pfnGetCompletedTask();
    }
    LOG("[Auth] WARNING: Task.get_CompletedTask not resolved, falling through to original");
    return g_pfnOrigLinkWithSteam(pThis, ticket, a2, a3);
}

// ============================================================
// Hook: AuthenticationServiceInternal.RefreshAccessTokenAsync
//
// After anonymous sign-in + fake Steam link, downstream UGS
// consumers (Vivox voice chat, Cloud Code, etc.) try to use
// the access token. They periodically refresh it via
// RefreshAccessTokenAsync. UGS validates that the user's claims
// still match their session -- our fake Steam link doesn't
// actually exist server-side, so the refresh fails with 401
// PERMISSION_DENIED "invalid token", which Phasmo surfaces as
// "Failed to get Steam account information" in the UI.
//
// Short-circuit the refresh to return Task.CompletedTask. The
// access token stays "valid" client-side and downstream
// services proceed.
//
// Signature: Task RefreshAccessTokenAsync()  (no args after this)
// ============================================================
typedef void* (__fastcall *Fn_Refresh)(void* pThis);
static Fn_Refresh g_pfnOrigRefreshAccessToken = nullptr;

static void* __fastcall Hooked_RefreshAccessToken(void* pThis)
{
    LOG("[Auth] RefreshAccessTokenAsync intercepted -> Task.CompletedTask "
        "(skipping UGS token refresh)");
    if (g_pfnGetCompletedTask)
    {
        return g_pfnGetCompletedTask();
    }
    LOG("[Auth] WARNING: Task.get_CompletedTask not resolved, falling through");
    return g_pfnOrigRefreshAccessToken(pThis);
}

// ============================================================
// Hook: AuthenticationServiceInternal.HandleSignInRefreshRequestAsync
//
// The INTERNAL refresh handler. Different from the public
// RefreshAccessTokenAsync -- this one is called by downstream
// UGS services (Vivox, Cloud Code, etc.) when they detect a
// stale or about-to-expire token and want to refresh it via
// the existing session credentials. With our fake Steam link,
// UGS server rejects the refresh as "invalid token".
//
// Signature varies: usually Task<SignInResponse> with the
// session token as arg. We treat as opaque and return
// CompletedTask. The caller awaits and proceeds with the
// existing cached access token, which Vivox already showed
// it can use.
// ============================================================
typedef void* (__fastcall *Fn_RefreshHandler)(void* pThis, void* a1, void* a2);
static Fn_RefreshHandler g_pfnOrigRefreshHandler = nullptr;

static void* __fastcall Hooked_HandleSignInRefreshRequest(void* pThis, void* a1, void* a2)
{
    LOG("[Auth] HandleSignInRefreshRequestAsync intercepted -> Task.CompletedTask "
        "(blocking internal token refresh)");
    if (g_pfnGetCompletedTask)
    {
        return g_pfnGetCompletedTask();
    }
    LOG("[Auth] WARNING: Task.get_CompletedTask not resolved, falling through");
    return g_pfnOrigRefreshHandler(pThis, a1, a2);
}

// ============================================================
// IL2CPP hook installer -- runs once GameAssembly.dll loads
// ============================================================
static bool InstallIl2CppHooks()
{
    if (!IL2CPP_IsReady()) return false;

    // Resolve SignInAnonymouslyAsync first; if it's missing
    // there's no point hooking SignInWithSteamAsync.
    g_pfnSignInAnonymous = (Fn_SignInTask)IL2CPP_FindMethodPtr(
        nullptr, "Unity.Services.Authentication",
        "AuthenticationServiceInternal", "SignInAnonymouslyAsync", -1);
    if (!g_pfnSignInAnonymous)
    {
        LOG("[Auth] could not find SignInAnonymouslyAsync -- bypass disabled");
        return false;
    }
    LOG("[Auth] SignInAnonymouslyAsync at %p", g_pfnSignInAnonymous);

    void* steamFn = IL2CPP_FindMethodPtr(
        nullptr, "Unity.Services.Authentication",
        "AuthenticationServiceInternal", "SignInWithSteamAsync", -1);
    if (!steamFn)
    {
        LOG("[Auth] could not find SignInWithSteamAsync -- game probably "
            "doesn't use UGS Steam sign-in; nothing to bypass");
        return false;
    }

    // Resolve System.Threading.Tasks.Task.get_CompletedTask so our
    // UpdatePlayerNameAsync hook can return a real completed Task.
    g_pfnGetCompletedTask = (Fn_get_CompletedTask)IL2CPP_FindMethodPtr(
        "mscorlib", "System.Threading.Tasks", "Task", "get_CompletedTask", 0);
    if (g_pfnGetCompletedTask)
        LOG("[Auth] Task.get_CompletedTask at %p", g_pfnGetCompletedTask);
    else
        LOG("[Auth] Task.get_CompletedTask not found -- UpdatePlayerName fallback only");

    // Install UpdatePlayerNameAsync hook (no-op return CompletedTask)
    void* updateNameFn = IL2CPP_FindMethodPtr(
        nullptr, "Unity.Services.Authentication",
        "AuthenticationServiceInternal", "UpdatePlayerNameAsync", 1);
    if (updateNameFn)
    {
        if (MH_CreateHook(updateNameFn, (void*)&Hooked_UpdatePlayerName,
                          (void**)&g_pfnOrigUpdatePlayerName) == MH_OK &&
            MH_EnableHook(updateNameFn) == MH_OK)
        {
            LOG("[Auth] UpdatePlayerNameAsync hook installed at %p", updateNameFn);
        }
        else
        {
            LOG("[Auth] failed to install UpdatePlayerNameAsync hook at %p", updateNameFn);
        }
    }
    else
    {
        LOG("[Auth] UpdatePlayerNameAsync not found (may not be needed for this game)");
    }

    // Install LinkWithSteamAsync hook (no-op return CompletedTask)
    void* linkFn = IL2CPP_FindMethodPtr(
        nullptr, "Unity.Services.Authentication",
        "AuthenticationServiceInternal", "LinkWithSteamAsync", -1);
    if (linkFn)
    {
        if (MH_CreateHook(linkFn, (void*)&Hooked_LinkWithSteam,
                          (void**)&g_pfnOrigLinkWithSteam) == MH_OK &&
            MH_EnableHook(linkFn) == MH_OK)
        {
            LOG("[Auth] LinkWithSteamAsync hook installed at %p", linkFn);
        }
        else
        {
            LOG("[Auth] failed to install LinkWithSteamAsync hook at %p", linkFn);
        }
    }
    else
    {
        LOG("[Auth] LinkWithSteamAsync not found (may not be needed for this game)");
    }

    // Install RefreshAccessTokenAsync hook (no-op return CompletedTask)
    void* refreshFn = IL2CPP_FindMethodPtr(
        nullptr, "Unity.Services.Authentication",
        "AuthenticationServiceInternal", "RefreshAccessTokenAsync", -1);
    if (refreshFn)
    {
        if (MH_CreateHook(refreshFn, (void*)&Hooked_RefreshAccessToken,
                          (void**)&g_pfnOrigRefreshAccessToken) == MH_OK &&
            MH_EnableHook(refreshFn) == MH_OK)
        {
            LOG("[Auth] RefreshAccessTokenAsync hook installed at %p", refreshFn);
        }
        else
        {
            LOG("[Auth] failed to install RefreshAccessTokenAsync hook at %p", refreshFn);
        }
    }
    else
    {
        LOG("[Auth] RefreshAccessTokenAsync not found (may not be needed for this game)");
    }

    // Install HandleSignInRefreshRequestAsync hook (internal refresh handler)
    void* refreshHandlerFn = IL2CPP_FindMethodPtr(
        nullptr, "Unity.Services.Authentication",
        "AuthenticationServiceInternal", "HandleSignInRefreshRequestAsync", -1);
    if (refreshHandlerFn)
    {
        if (MH_CreateHook(refreshHandlerFn, (void*)&Hooked_HandleSignInRefreshRequest,
                          (void**)&g_pfnOrigRefreshHandler) == MH_OK &&
            MH_EnableHook(refreshHandlerFn) == MH_OK)
        {
            LOG("[Auth] HandleSignInRefreshRequestAsync hook installed at %p", refreshHandlerFn);
        }
        else
        {
            LOG("[Auth] failed to install HandleSignInRefreshRequestAsync hook at %p",
                refreshHandlerFn);
        }
    }
    else
    {
        LOG("[Auth] HandleSignInRefreshRequestAsync not found (may not be needed for this game)");
    }

    if (MH_CreateHook(steamFn, (void*)&Hooked_SignInWithSteam,
                      (void**)&g_pfnOrigSignInWithSteam) != MH_OK ||
        MH_EnableHook(steamFn) != MH_OK)
    {
        LOG("[Auth] failed to install SignInWithSteamAsync hook at %p", steamFn);
        return false;
    }
    LOG("[Auth] SignInWithSteamAsync hook installed at %p", steamFn);

    // Diagnostic dump: enumerate methods on game-specific auth
    // wrappers (e.g. Phasmophobia's SteamAuth class under
    // \Assets\Scripts\SDK Managers\Steam\SteamAuth.cs). Limited
    // to Assembly-CSharp{,-firstpass} only -- iterating every
    // loaded image and passing an empty namespace string causes
    // il2cpp_class_from_name to fault on certain Unity-internal
    // images (observed crash on Phasmo at 11:07:42).
    // SteamAuth/DefaultStoreAuth/IStoreAuth are renamed by
    // Phasmo's Beebyte obfuscator (Malayalam Unicode names at
    // runtime). The classes referenced by the Unity inspector
    // or by reflection (MonoBehaviours, scriptable objects)
    // keep their original names -- those are our targets.
    static const char* kDiagClasses[] = {
        "StoreSDKManager",        // entry-point orchestrator
        "SignInSplashScreen",     // owns the error UI
        "SplashScreen",
        "ConnectionManager",
        "ErrorManager",
        "LobbyController",
        "ConnectToServer",
        nullptr
    };
    static const char* kDiagImages[] = {
        "Assembly-CSharp", "Assembly-CSharp-firstpass", nullptr
    };
    for (int i = 0; kDiagClasses[i]; ++i)
    {
        Il2CppClass* k = nullptr;
        for (int j = 0; kDiagImages[j] && !k; ++j)
        {
            k = IL2CPP_FindClass(kDiagImages[j], "", kDiagClasses[i]);
        }
        if (k)
        {
            LOG("[Auth] dumping methods of %s ...", kDiagClasses[i]);
            IL2CPP_DumpClassMethods(k, kDiagClasses[i]);
        }
        else
        {
            LOG("[Auth] diag class '%s' not found in Assembly-CSharp{,-firstpass}",
                kDiagClasses[i]);
        }
    }
    return true;
}

static DWORD WINAPI WatcherProc(LPVOID)
{
    for (int i = 0; i < 600 && InterlockedCompareExchange(&g_bShutdown, 0, 0) == 0; ++i)
    {
        if (IL2CPP_TryInit())
        {
            InstallIl2CppHooks();
            // Game-specific byte-pattern patches. Run AFTER
            // InstallIl2CppHooks so MinHook is initialized and
            // GameAssembly.dll is fully loaded.
            TryPatchPhasmoSteamAuth();
            TryPatchPhasmoSteamAccountGate();
            return 0;
        }
        Sleep(200);
    }
    if (!InterlockedCompareExchange(&g_bShutdown, 0, 0))
        LOG("[Auth] GameAssembly.dll never resolved -- giving up on IL2CPP hooks");
    return 0;
}

// ============================================================
// Plugin ABI entry points
// ============================================================
extern "C" __declspec(dllexport) int __cdecl UCO_PluginInit(const UCO_PluginContext* ctx)
{
    if (!ctx) return 1;
    if (ctx->ApiVersion != UCO_PLUGIN_API_VERSION) return 2;

    g_Log = ctx->Log;
    LOG("[Auth] plugin v1 init: AppId=%u ogAppId=%u",
        ctx->ForcedAppId, ctx->OriginalAppId);

    if (MH_Initialize() != MH_OK)
    {
        // Already initialized by another plugin -- that's fine.
        LOG("[Auth] MH_Initialize returned non-OK (already inited?)");
    }

    g_hWatcherThread = CreateThread(nullptr, 0, WatcherProc, nullptr, 0, nullptr);
    return 0;
}

extern "C" __declspec(dllexport) void __cdecl UCO_PluginShutdown(void)
{
    InterlockedExchange(&g_bShutdown, 1);
    if (g_hWatcherThread)
    {
        WaitForSingleObject(g_hWatcherThread, 1000);
        CloseHandle(g_hWatcherThread);
        g_hWatcherThread = nullptr;
    }
    // Don't MH_DisableHook(ALL) here -- another plugin may share
    // the MinHook instance. Disable only what we own.
    LOG("[Auth] plugin shutdown");
}

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) { return TRUE; }
