# Linux (x64) build

The Windows `.dll` and the Linux `.so` are built from the same source tree.
The Linux build produces **`libsteam_api.so`** — the name the Valve Linux SDK
uses for the 64-bit library (note there is **no** `64` suffix, unlike the
Windows `steam_api64.dll`).

## Build

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Result: `build/x64/libsteam_api.so` (`build/x86/…` for a 32-bit build).

Requirements: CMake ≥ 3.16, a C++17 compiler (GCC or Clang), Linux headers.
No external dependencies — `dlopen`/`dlsym` come from libc, the lock shims from
pthreads.

## Install

1. Copy `build/x64/libsteam_api.so` next to the game's executable (or symlink
   it there). The game resolves `libsteam_api.so` through its `DT_NEEDED`
   entry, so the file has to be findable by the dynamic loader — next to the
   executable is the usual place.
2. Put `union-crax.ini` next to the **running executable** as well. An ini in
   the wrong directory is silently ignored. Its contents are the same as on
   Windows; see the [configuration section of the README](README.md#configuration).

   ```ini
   [Settings]
   AppId=480
   ogAppId=1234

   [DLC]
   UnlockAll=1
   2280850=Deluxe Edition
   ```

3. Steam must be able to talk to the client. The emulator talks to the real
   `steamclient.so`, which it looks for in this order:

   - `$SteamPath/steamclient.so`, where `SteamPath` comes from the environment
   - `~/.steam/sdk64/steamclient.so`
   - `~/.local/share/Steam/linux64/steamclient.so`
   - `~/.steam/steam/ubuntu12_64/steamclient.so`
   - `~/.local/share/Steam/ubuntu12_64/steamclient.so`
   - `~/.steam/debian-installation/ubuntu12_64/steamclient.so`

   The same list is used to resolve the Steam install path, which ends up in
   `SteamPath` and is what the game reads for `SteamAPI_GetSteamInstallPath`.

A log is written to `/tmp/uc_online2.log`.

## What works

- The full Steam API surface — the exported symbol set matches the Valve
  Linux SDK exactly (nothing the real SDK exports is missing).
- The AppId spoof, the DLC store, the ticket emulation, the local cloud
  (`LocalSaves`), the local inventory and the settings/ini handling.
- The vtable hooks that implement the spoof. On Linux there is no MinHook:
  the hook shim patches the interface **vtable slots** in place, which is
  what the Steamworks C++ interfaces dispatch through anyway.

## What is not available on Linux

These parts of the Windows build have no Linux equivalent and are skipped:

- **The overlay proxy.** `version.dll` / `winmm.dll` and the renderer hooks
  (`GameOverlayRenderer`) are a Windows-only mechanism. `[Settings]
  LoadOverlay` therefore has no effect here.
- **The SteamStub bypass.** `steamstub_hook.h` is Windows-only.
- **steamclient auth tracing.** The Windows build hooks
  `Steam_GetGSHandle` and friends inside `steamclient64.dll` to trace auth
  tickets; the Linux `steamclient.so` does not export them.
- **Plugins.** The bundled plugins are separate MSVC projects and have not
  been ported; the loader itself looks for `*.so` next to the library and
  `dlopen`s what it finds.
- **The registry.** There is no registry on Linux, so the Windows registry
  lookups (the Steam install path) are replaced by the path probing listed
  above.
