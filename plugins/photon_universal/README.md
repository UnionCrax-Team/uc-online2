# photon_universal plugin

**One DLL that handles every Photon multiplayer path UCOnline2 supports.**

This is the consolidated "mega" plugin. It replaces the old per-flavor plugins (`photon_realtime`, `photon_realtime_mono`, `photon_fusion`) with a single DLL that **auto-detects** the game's setup at launch and installs only the relevant hooks. You no longer pick a plugin per game — drop this one in and it figures out the rest.

It covers:

- **Photon Realtime / PUN** — IL2CPP *and* Mono scripting backends
- **Photon Fusion 2** — IL2CPP
- **Photon Voice** — paired with a Realtime/PUN connection

At init the plugin detects the Unity backend (Mono vs IL2CPP) by which runtime DLL is loaded and the Photon flavor (Realtime/PUN vs Fusion) by metadata/assembly scan. Modules that don't apply are simply never installed.

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
| **Outbound** | 2681030 | IL2CPP · Fusion 2 | First Fusion target; works asset-patch-free at runtime. |

## How multiplayer auth is bypassed

Photon-backed games normally authenticate with a Steam ticket validated against the *developer's* publisher key — which you don't have. The fix has two parts:

1. **Redirect the game to a Photon app you control** by rewriting the Photon AppId GUID on the wire at runtime (no asset patching needed — see below).
2. **Point that Photon app at a permissive Custom Authentication URL** — use the shared **`https://photon.lukewarmsodas.workers.dev`** (a hosted endpoint that always replies success; nothing to deploy), and force the wire-time auth type to `Custom`.

Both Photon's master/NameServer then accept the client without a real publisher key.

## Setup

Setup is asset-patch-free — the plugin reads everything from `union-crax.ini` and rewrites AppIds on the wire at runtime, so you never modify the game's `resources.assets`.

> **Shortcut:** **`UCO2.Patcher.exe`** (the release GUI) automates the DLL + ini steps — point it at a game folder and it detects whether the game uses Photon (Realtime/PUN or Fusion, Mono or IL2CPP, and whether it ships Voice), prompts for the real Steam AppId and your Photon GUID(s), writes `union-crax.ini` with the right section, and copies `photon_universal.dll` into `<game>\plugins\`. Non-Photon games are detected and skipped. (The repo-root `patch.bat` CLI does the same from source.) You still create the Photon app(s) and point their Custom Auth at `https://photon.lukewarmsodas.workers.dev` (steps 1–2 below), and drop in UCOnline2's `steam_api64.dll` yourself.

### 1. Create your Photon app(s)

At <https://dashboard.photonengine.com/> (free, no card):

- **Realtime/PUN game:** create one app of type **`Realtime`**. If the game has voice chat, also create one of type **`Voice`**.
- **Fusion 2 game:** create one app of type **`Fusion`**.

Copy each app's **AppId** GUID (e.g. `4ff936cd-afb9-486b-b8e3-6ab23d915af0`).

### 2. Point each app at the shared Custom Auth URL

On each Photon app you created (Realtime, Voice, Fusion — whichever apply):
**Manage → Authentication → Add Provider → Custom**, set the URL to

```
https://photon.lukewarmsodas.workers.dev
```

leave the mandatory key/value pairs empty, and **uncheck "Reject Clients on Authentication Failure"**. Save. Make sure it's the **only** Custom provider on the app — if an older URL is still listed, Photon may keep calling that one.

That's it — **you don't need to host anything.** `photon.lukewarmsodas.workers.dev` is a shared, permissive endpoint that approves every request and echoes the player's Steam name back as the `Nickname` (see "Player display name" below). It knows nothing about which Photon app points at it, so it works for anyone's app.

> **Set up before v1.20.17 with `https://photon.iforgor.cc`?** Change it to the URL above. That host sits behind Cloudflare Bot Fight Mode, which challenges Photon's server-to-server auth calls, so **every** login through it failed — in-game it looks like the connection hangs or times out, and the log shows `CustomAuthenticationFailed ... (403) Forbidden` (see step 5).

<details>
<summary>Prefer to self-host the auth endpoint?</summary>

Deploy this to <https://workers.cloudflare.com>, then use your own `https://….workers.dev` URL in place of `photon.lukewarmsodas.workers.dev`.

**Keep it on `workers.dev`** — don't route it through a custom domain on a zone with Cloudflare bot protection. Photon calls the URL from Azure datacenter IPs with an empty User-Agent, which is exactly what **Bot Fight Mode** challenges, and a challenge reaches Photon as a `403`. On the Free plan Bot Fight Mode can't be skipped per hostname, so a custom domain means turning it off for the whole zone.

```js
export default {
  async fetch(request) {
    // The plugin sends the player's Steam name as ?name=... (see "Player
    // display name" below). Echo it back as the Nickname; fall back to a
    // constant if it's absent.
    const name = new URL(request.url).searchParams.get("name") || "Player";
    return new Response(JSON.stringify({
      ResultCode: 1,
      UserId: "anon-" + crypto.randomUUID(),
      Nickname: name
    }), { headers: { "Content-Type": "application/json" } });
  }
};
```
</details>

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
- `VerboseLog=1` — optional. Log **every** Photon operation response and connection-state change, not just the failures. Failed connects/logins and dropped connections are logged either way (see step 5). The patcher's **Verbose Photon logging** toggle sets it.

The `[Realtime]` section is also accepted under the legacy name `[PUN]`, so inis written by older tooling keep working.

### Player display name (Steam username)

Games that read their nickname from the Custom Auth response (e.g. Phasmophobia) otherwise show the Worker's constant `Nickname` for *every* player. The plugin fixes this by sending the player's name to the Worker as a custom-auth parameter:

- On `OpAuthenticate` it sets `params[216]` (`ClientAuthenticationParams`) to `name=<url-encoded name>`. Photon forwards that to the Custom Auth URL as `?name=...`, and the Worker echoes it back as the `Nickname` (see the Worker code above).
- The name defaults to the **real Steam persona** — UCOnline2 proxies `ISteamFriends` straight through to real Steam, so each player gets their own Steam username automatically. No config needed.
- Override with `[Realtime] Nickname=<text>` if you want a fixed name instead.
- Requires the name-echoing Worker above. With the old constant-`Nickname` Worker the injected param is simply ignored (safe no-op).

Log line proving it fired: `[Universal] Photon Nickname from Steam persona '…' -> params[216]=name=…`.

### 5. Launch and verify

Run the game, trigger multiplayer, and tail the log:

```powershell
Get-Content "$env:TEMP\uc_online2.log" -Wait -Tail 40
```

The lines that prove each module fired:

- `[Realtime] OpAuthenticate hook @ …` / `[Realtime/Mono] module active` — Realtime/PUN hooks in place.
- `[Realtime] SendOp op=220 (Realtime peer): params[224] AppId -> …` — wire-time AppId rewrite fired (critical: PUN's `OpGetRegions` op 220 sends an *empty* ApplicationId that Photon would otherwise reject).
- `[Fusion] OpAuthenticate: authType X -> 0` — Fusion auth-type override fired.
- `[Realtime/Mono] response logging: OnOperationResponse=hooked OnStatusChanged=hooked` — the plugin can see what Photon sends **back** (below).

Photon dashboard CCU going 0 → 1 confirms the connection reached your app.

#### When it doesn't connect

The plugin logs Photon's own answer whenever a connect, login or join fails, or the connection drops — no flag needed. The line to look for:

```
response (Realtime peer, NameServer): op=230 Authenticate FAILED -> ReturnCode 32755 CustomAuthenticationFailed: "The remote server returned an error: (403) Forbidden."
```

| What the log says | What it means |
|---|---|
| `CustomAuthenticationFailed` … `(403) Forbidden` | Photon called your app's Custom Auth URL and was **refused**. The app points at an old/dead worker, or the URL sits behind Cloudflare bot protection (see step 2). Re-point it at `https://photon.lukewarmsodas.workers.dev`. |
| `CustomAuthenticationFailed` with another HTTP error | Same idea — the Custom Auth URL answered with an error. Open it in a browser; it should show `{"ResultCode":1,...}`. |
| `InvalidAuthentication` | The AppId was rejected — usually a Realtime GUID in the Voice slot or vice versa (see "Why two Photon apps" below). |
| `MaxCcuReached` | Your Photon app is at its free-tier CCU limit. |
| `status … TimeoutDisconnect` / `DisconnectByServer…` | The connection itself dropped — network or firewall, not configuration. |

With `[Realtime] VerboseLog=1` you also get every successful step (`GetRegions -> Ok`, `Authenticate -> Ok`, …), which shows how far a connection got.

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
## Limitations

- Players must share the same Photon AppId(s).
- The Cloudflare Worker accepts every auth request — don't reuse that Photon app for anything real.
- Photon free tier caps CCU (~20) — fine for friend groups, not public servers.
- Anti-cheat blocks this — don't try it on EAC/BattlEye titles.
