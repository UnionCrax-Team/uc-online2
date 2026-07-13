# photon_universal plugin

**One DLL that handles every Photon multiplayer path UCOnline2 supports.**

This is the consolidated "mega" plugin. It replaces the old per-flavor plugins (`photon_realtime`, `photon_realtime_mono`, `photon_fusion`) with a single DLL that **auto-detects** the game's setup at launch and installs only the relevant hooks. You no longer pick a plugin per game — drop this one in and it figures out the rest.

It covers:

- **Photon Realtime / PUN** — IL2CPP *and* Mono scripting backends
- **Photon Fusion 2** — IL2CPP
- **Photon Voice** — paired with a Realtime/PUN connection
- **Unity Services auth bypass** — `SignInWithSteam` → anonymous
- **Phasmophobia SteamAuth gate NOP** — game-specific byte-signature patch

At init the plugin detects the Unity backend (Mono vs IL2CPP) by which runtime DLL is loaded, the Photon flavor (Realtime/PUN vs Fusion) by metadata/assembly scan, and the Phasmo gate by byte signature in `GameAssembly.dll`. Modules that don't apply are simply never installed.

## Quick terminology

- **Photon Realtime SDK** is Photon's low-level networking library. On the [Photon dashboard](https://dashboard.photonengine.com/) you create apps of type **`Realtime`**.
- **PUN (Photon Unity Networking)** is a higher-level Unity wrapper *built on top of* Realtime. A PUN game ships `PhotonUnityNetworking.dll` + `PhotonRealtime.dll`; its dashboard app is still a **Realtime-type** app.
- **Photon Fusion 2** is a separate, newer middleware. A Fusion game ships `Fusion.Realtime.dll` etc., and its dashboard app is type **`Fusion`**.
- **Photon Voice** (`PhotonVoice.PUN.dll`) is voice chat. It needs a **second, separate** Photon app of type **`Voice`** with its own AppId GUID.

## Confirmed working

| Game | Steam AppId | Backend / flavor | Notes |
|---|---|---|---|
| **R.E.P.O.** | 3241660 | Mono · Realtime/PUN + Voice | Needs **two** Photon apps (one Realtime + one Voice). Voice slot is mandatory — Photon's NameServer rejects the connection if the Voice AppId still holds the dev's GUID. |
| **PEAK** | 3527290 | Mono · Realtime/PUN + Voice | Same shape as R.E.P.O. — PUN + proximity-chat Voice, so both a Realtime and a Voice app are required. Steam-friends-invite co-op. |
| **Phasmophobia** | 739630 | IL2CPP · Realtime/PUN | The built-in Phasmo SteamAuth gate NOP fires automatically (byte signature in `GameAssembly.dll`), NOPing the Beebyte-obfuscated ticket-verify gate that would otherwise block before Photon is reached. |
| **Outbound** | 2681030 | IL2CPP · Fusion 2 | First Fusion target; works asset-patch-free at runtime. |

## How multiplayer auth is bypassed

Photon-backed games normally authenticate with a Steam ticket validated against the *developer's* publisher key — which you don't have. The fix has two parts:

1. **Redirect the game to a Photon app you control** by rewriting the Photon AppId GUID on the wire at runtime (no asset patching needed — see below).
2. **Point that Photon app at a permissive Custom Authentication URL** (a Cloudflare Worker that always replies success), and force the wire-time auth type to `Custom`.

Both Photon's master/NameServer then accept the client without a real publisher key.

## Setup

Setup is asset-patch-free — the plugin reads everything from `union-crax.ini` and rewrites AppIds on the wire at runtime, so you never modify the game's `resources.assets`.

> **Shortcut:** the repo-root **`patch.bat`** automates the DLL + ini steps. Drop a game folder onto it (or run `patch.bat "C:\path\to\game"`); it detects whether the game uses Photon (Realtime/PUN or Fusion, Mono or IL2CPP, and whether it ships Voice), prompts for the real Steam AppId and your Photon GUID(s), writes `union-crax.ini` with the right section, and copies `photon_universal.dll` into `<game>\plugins\`. Non-Photon games are detected and skipped with no changes. You still create the Photon app(s) + Cloudflare Worker (steps 1–2 below) and drop in UCOnline2's `steam_api64.dll` yourself.

### 1. Create your Photon app(s)

At <https://dashboard.photonengine.com/> (free, no card):

- **Realtime/PUN game:** create one app of type **`Realtime`**. If the game has voice chat, also create one of type **`Voice`**.
- **Fusion 2 game:** create one app of type **`Fusion`**.

Copy each app's **AppId** GUID (e.g. `4ff936cd-afb9-486b-b8e3-6ab23d915af0`).

### 2. Add a permissive auth backend to each app

On each Photon app: **Manage → Authentication → Add Provider → Custom**. Paste a Cloudflare Worker URL that always returns success, leave the mandatory key/value pairs empty, and **uncheck "Reject Clients on Authentication Failure"**. Save.

Worker code:

```js
export default {
  async fetch() {
    return new Response(JSON.stringify({
      ResultCode: 1,
      UserId: "anon-" + crypto.randomUUID(),
      Nickname: "Player"
    }), { headers: { "Content-Type": "application/json" } });
  }
};
```

Deploy at <https://workers.cloudflare.com>, copy the resulting `https://….workers.dev` URL, and paste it into **every** Photon app you created (Realtime, Voice, Fusion — whichever apply).

### 3. Deploy the DLL

Build (see [Build](#build)) and copy `photon_universal.dll` into the game's plugins folder:

```powershell
Copy-Item plugins\photon_universal\relbuild\x64\photon_universal.dll `
  C:\path\to\TheGame\plugins\ -Force
```

Create that `plugins\` folder if it doesn't exist. Also drop UCOnline2's `steam_api64.dll` into the game's Steam-loading location (back up the original first).

### 4. Configure `union-crax.ini`

Place `union-crax.ini` next to the game exe. Fill in **only** the section(s) matching the game's Photon flavor — the plugin reads both and ignores the inactive one.

```ini
[Settings]
AppId=480
ogAppId=<the game's real Steam AppId>
PluginsFolder=plugins
GetStubbedLol=false

; --- Fill this for Realtime / PUN games ---
[Realtime]
PhotonAppIdRealtime=<your Realtime-type app's GUID>
PhotonAppIdVoice=<your Voice-type app's GUID>   ; optional, voice-chat games only
ForcedAuthType=0

; --- Fill this for Fusion 2 games ---
[Fusion]
PhotonAppIdFusion=<your Fusion-type app's GUID>
ForcedAuthType=0
```

- `PhotonAppIdRealtime` — your Realtime app's GUID. Applied to the Realtime peer in `OpAuthenticate` and the `SendOperation` params dict.
- `PhotonAppIdVoice` — optional. The plugin classifies peer instances at runtime (first peer = Realtime, second distinct peer = Voice) and routes the Voice peer's `params[224]` to this GUID. Required for any PUN game that ships `PhotonVoice` (like R.E.P.O.).
- `PhotonAppIdFusion` — your Fusion app's GUID.
- `ForcedAuthType=0` — `Custom` (matches the Custom Auth provider). Use `255` for `None` if you want pure anonymous instead.

The `[Realtime]` section is also accepted under the legacy name `[PUN]`, so inis written by older tooling keep working.

### 5. Launch and verify

Run the game, trigger multiplayer, and tail the log:

```powershell
Get-Content "$env:TEMP\uc_online2.log" -Wait -Tail 40
```

The lines that prove each module fired:

- `[Realtime] OpAuthenticate hook @ …` / `[Realtime/Mono] module active` — Realtime/PUN hooks in place.
- `[Realtime] SendOp op=220 (Realtime peer): params[224] AppId -> …` — wire-time AppId rewrite fired (critical: PUN's `OpGetRegions` op 220 sends an *empty* ApplicationId that Photon would otherwise reject).
- `[Fusion] OpAuthenticate: authType X -> 0` — Fusion auth-type override fired.
- `[Auth] SignInWithSteam intercepted -> SignInAnonymouslyAsync` — Unity Services bypass fired.
- `[Phasmo] SteamAccountGate NOPed at GameAssembly+0x…` — Phasmo gate patched (only on that specific Phasmo build; silently skipped everywhere else).

Photon dashboard CCU going 0 → 1 confirms the connection reached your app.

## Why two Photon apps for PUN+Voice games?

Games like R.E.P.O. open **two** Photon connections — one Realtime, one Voice — each with its own AppId, each validated server-side by product type. Put a Realtime AppId where Voice is expected → `InvalidAuthentication`. Leave Voice pointing at the dev's GUID → same rejection. You must create both apps and set `PhotonAppIdVoice`; the plugin routes the second peer to it automatically.

## Trying it on a new game

The plugin auto-detects, so the practical question is just *which Photon middleware* the game uses:

- **`Fusion.Realtime.dll` present anywhere under `<Game>_Data\`** → Fusion 2. Fill `[Fusion]`.
- **`PhotonUnityNetworking.dll` + `PhotonRealtime.dll` in `<Game>_Data\Managed\`** → Mono PUN. Fill `[Realtime]`. (If `PhotonVoice.PUN.dll` is also there, create a Voice app.)
- **No `Managed\` folder, C# in `GameAssembly.dll`** → IL2CPP. Scan `<Game>_Data\il2cpp_data\Metadata\global-metadata.dat`: `PhotonNetwork` + `LoadBalancingClient` present (and no `NetworkRunner`) = PUN → fill `[Realtime]`; `NetworkRunner` present = Fusion → fill `[Fusion]`.

If the game is pure Steam P2P with no Photon at all, you don't need this plugin — test bare first.

## Build

```powershell
msbuild plugins\photon_universal\photon_universal_plugin.vcxproj `
  -p:Configuration=Release -p:Platform=x64 -m
```

Output: `plugins\photon_universal\relbuild\x64\photon_universal.dll`. MinHook is statically linked.

## What the plugin does at runtime (per module)

- **Realtime/PUN (IL2CPP & Mono):** hooks `LoadBalancingPeer.OpAuthenticate` / `OpAuthenticateOnce` (rewrite `appId`, force `authType=0`), `PhotonPeer.SendOperation` (rewrite `params[224]` ApplicationId + `params[217]` auth type for ops 220/226/230/231), and `AuthenticationValues.set_AuthType`. A per-peer classifier routes the first peer to the Realtime AppId and the second distinct peer to the Voice AppId.
- **Fusion 2:** hooks `PhotonAppSettings.get_Global` (rewrite `AppIdFusion`), `AuthenticationValues.set_AuthType`, and `LoadBalancingPeer.OpAuthenticate`/`OpAuthenticateOnce` (force `authType` at offset 0x10 on the wire).
- **Unity Services auth bypass:** hooks `Unity.Services.Authentication.AuthenticationServiceInternal.SignInWithSteamAsync` and redirects to anonymous sign-in.
- **Phasmo gate:** verifies the byte signature before patching (so non-Phasmo / new-build games are skipped), then NOPs the `SteamAccountGate` in `GameAssembly.dll`.

## Limitations

- Players must share the same Photon AppId(s).
- The Cloudflare Worker accepts every auth request — don't reuse that Photon app for anything real.
- Photon free tier caps CCU (~20) — fine for friend groups, not public servers.
- Anti-cheat blocks this — don't try it on EAC/BattlEye titles.
- The Phasmo gate NOP is keyed to a specific build's byte signature; after a Phasmo update it silently no-ops until the signature is re-located. For other deeply-obfuscated games whose own UI gates multiplayer on auth state, you may still need the standalone [`unity_auth_bypass`](../unity_auth_bypass/) plugin.
