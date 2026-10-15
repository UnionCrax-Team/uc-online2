# steam_overlay

This is not a normal UCOnline2 plugin. It is a renameable system-DLL proxy
that a game loads at process startup, before its graphics engine creates the
swapchain. The build output is `overlay_proxy.dll`.

The patcher (`UCO2.Patcher.exe`, or the `patch.bat` CLI from source) detects the
engine and deploys it automatically:

- Unity: renamed to `version.dll` beside the game executable.
- Unreal Engine: renamed to `winmm.dll` beside the real
  `*-Win64-Shipping.exe`.

Do not put it in the game's `plugins` folder.

### Why winmm.dll for Unreal (not XINPUT1_3.dll)

Unreal shipping executables statically import both, but `XINPUT1_3.dll` is
resolved *late* on UE5 — after the D3D12 renderer is up — which is too late to
arm the SteamStub bypass (the stub's ownership check has already fired) and too
late for the overlay's present hook. `winmm.dll` (timeGetTime / timeBeginPeriod)
is bound before the entry point, so this shim's `DllMain` runs pre-OEP. The
patcher retires any `XINPUT1_3.dll` a previous UCOnline2 left behind.

## What It Fixes

Some games load `steam_api64.dll` only after Unity or Unreal has initialized
graphics. Loading `GameOverlayRenderer64.dll` from UCOnline2 at that point is
too late: the renderer can enter the process but miss the original graphics
device or swapchain creation, leaving Shift+Tab and invite dialogs unavailable.

The early proxy loads Valve's overlay renderer during process startup. It also
sets `SteamAppId`, `SteamGameId`, and `SteamClientLaunch` before the renderer is
loaded. The AppId comes from `steam_appid.txt` beside the executable and falls
back to `480`.

The proxy forwards calls to the real Windows DLL in `System32`, chosen by its
deployed filename:

- `version.dll` — the `GetFileVersionInfo*` / `VerQueryValue*` functions are
  resolved by name from `System32\version.dll` lazily, on first call.
- `winmm.dll` — all 180 winmm exports (at their real ordinals 2–182) are x64
  jump thunks that tail-jump through a pointer table filled from
  `System32\winmm.dll` in `DllMain`. Loading by full path returns the real
  winmm as a distinct module even though this proxy shares its base name.

## Automatic Deployment

Point `UCO2.Patcher.exe` at the game's top-level folder (or, from source, the
`patch.bat` CLI):

```powershell
patch.bat "C:\path\to\game"
```

The patcher already distinguishes Unity from Unreal and locates Unreal's real
shipping executable. It backs up a different existing proxy to
`<name>.uco2.bak` before installing. If that backup already exists, it refuses
to overwrite the current DLL again.

Some titles are deliberately skipped. **Phasmophobia** inventories the files
beside its executable during startup and aborts on an unrecognised
`version.dll`, so the patcher reports `[SKIP]` and leaves the proxy out. The
game still runs through UCOnline2 normally; it simply keeps whatever overlay
behaviour it had, because the proxy is an optional extra rather than part of
the emulator.

For manual deployment:

1. Copy `overlay_proxy.dll` beside the executable that runs the game.
2. Rename it to `version.dll` for Unity or `winmm.dll` for Unreal.
3. Set `LogOverlay=yes` under `[Settings]` when diagnostics are needed.
4. Launch and check `steam_overlay.log`.

Overlay logging is disabled by default. The proxy does not create or append to
`steam_overlay.log` unless `union-crax.ini` contains:

```ini
[Settings]
LogOverlay=yes
```

If the target DLL name is already occupied by a mod or custom controller
wrapper, preserve it. The automatic patcher creates one backup and reports the
collision.

## Build

```powershell
msbuild plugins\steam_overlay\steam_overlay.vcxproj -p:Configuration=Release -p:Platform=x64
```

Output:

```text
plugins\steam_overlay\relbuild\x64\overlay_proxy.dll
```

The binary is x64 only. Its combined export table carries the `version.dll` API
names (parked at ordinals 200+) and every `winmm.dll` export at its real ordinal
(2–182, including the ordinal-2 NONAME alias of `PlaySound`).

### Regenerating the winmm exports

`winmm_thunks.asm`, `winmm_names.h`, and the winmm block of `overlay_proxy.def`
are generated from the host's `System32\winmm.dll` export table (ordinal + name,
in ordinal order). Regenerate them only if a future Windows adds or reorders
winmm exports:

```bash
dumpbin -exports C:\Windows\System32\winmm.dll
```

Each named export becomes a `PROC` that does `jmp qword ptr [g_winmm_ptrs + 8*i]`
(index `i` = ordinal − 3), the same name at the same ordinal in the `.def`, and
the same name at index `i` in `kWinmmNames[]`. Keep all three in lockstep.
