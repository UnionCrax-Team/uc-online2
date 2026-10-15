# EOS_custom plugin

**Gets multiplayer working in Epic Online Services (EOS) games running under UCOnline2, by pointing the game at an Epic app it can actually authenticate against and logging in anonymously.**

Release builds ship with a **default Epic app baked into the DLL**, so most games need **no `[EOS]` configuration at all** — drop the plugin in and go. You can still bring your own Epic app, or keep the game on its own, when you want to: see [Which Epic app?](#which-epic-app).

Co-op then runs over **Epic's own relay** — no LAN, no VPN, and no EOS emulator.

## The problem this solves

EOS games on Steam typically log into EOS Connect using a **Steam session ticket**. Epic validates that ticket *server-side* against Steam, which always fails when the ticket comes from an emulator. The game's own log shows it plainly:

```
LogOnlineIdentity: STEAM: Obtained steam authticket
LogEOS: Error: External credential 'STEAM_SESSION_TICKET' failed to authenticate
        with EOS Connect: EOS_Connect_ExternalTokenValidationFailed
```

With no EOS identity there is no `ProductUserId`, so the session layer refuses to host:

```
LogEOS: Error: HostingPlayerNum provided to CreateSession does not have online identity.
```

No session is created, so nothing is advertised to Steam. The symptoms look unrelated to auth, which makes this easy to misdiagnose:

- Steam's **"Join Game"** button never appears on your friends list
- the joiner sees no lobby / can't join, or fails during map transition
- the multiplayer menu may still open and even list servers, because that part is local

## The fix

Two hooks on the **genuine Epic** `EOSSDK-Win64-Shipping.dll`:

1. **`EOS_Platform_Create`** — rewrites `ProductId`, `SandboxId`, `DeploymentId`, `ClientId` and `ClientSecret` to **whichever Epic app you're using** (the built-in default, or your own), so every player lands in the same session pool. With `KeepGameApp=1` this rewrite is skipped and the game stays on its own Epic app.
2. **`EOS_Connect_Login`** — replaces the doomed `STEAM_SESSION_TICKET` credential (type 18) with **`DEVICEID_ACCESS_TOKEN`** (type 10). Device ID login is anonymous: there's no external platform for Epic to validate, so it succeeds and yields a real `ProductUserId`.

Device ID login requires a device id to already exist, so the plugin calls `EOS_Connect_CreateDeviceId` once on the first attempt and lets the game's own login retry loop (these games re-attempt every few seconds) succeed on a later pass.

## Known working

| Game | Steam AppId | Notes |
|---|---|---|
| **Forever Skies** | 1641960 | Redpoint EOS Online Framework (`RedpointSteam` OSS). Uses EOS **Sessions**. Remote co-op over Epic's relay. |
| **Palworld** | 1623730 | Dual-stack Steam + EOS with its own integration, built on EOS **Lobbies**. Verified on the **1.0 release** and on the older `0.7.3.90464` build. |
| **StarRupture** | 1631270 | UE5, Steam + EOS. Needs **`NoPresence=1`**: its lobbies demand a real linked-platform identity and reject the anonymous user without it. Joining rides Steam. |
| **Subnautica 2** | 1962700 | **Community-reported**, not verified first-hand. No details on the EOS mechanism, tested build or SDK version — treat as a strong lead rather than a guarantee. |

Forever Skies and Palworld use EOS very differently on top of the same login — Redpoint Sessions vs. raw Lobbies — and both are fixed by the same two hooks, because the hooks are on the **EOS SDK itself**, not on any game code. Expect this to work for most EOS titles that log in with a platform (Steam) credential.

They also span different SDK builds (Forever Skies and old Palworld ship a byte-identical 1.15.5; Palworld 1.0 reports `EOS_Platform_Options` `ApiVersion=12`), and the same offsets resolved the game's real IDs in each.

The evidence for each end of that range is different, and worth stating precisely. **1.19.1.2 is header-verified**: `EOS_Platform_Options` was read straight out of the official SDK headers and every field the plugin touches is exactly where it expects — `ProductId` +16, `SandboxId` +24, `ClientCredentials` +32, `DeploymentId` +80. Everything added since (`Flags`, `CacheDirectory`, `TickBudgetInMilliseconds`, `RTCOptions`, `IntegratedPlatformOptionsContainerHandle`, `SystemSpecificOptions`, `TaskNetworkTimeoutSeconds`) was appended *after* `DeploymentId`. EOS appends, it does not reorder.

**1.15.5 is verified empirically, not from headers** — Epic no longer distributes SDKs older than 1.17. The proof there is that Forever Skies and Palworld ship it and both work: the plugin reads their real IDs out at those offsets and co-op connects. Versions in between are untested but bracketed by the two ends.

The quick runtime sanity check still applies to anything newer: if the `[EOSAuth]` log prints the game's original IDs as valid 32-hex strings, the layout matched.

## Which Epic app?

The plugin has to log in against *some* Epic app. There are three choices, and they resolve in this order — **first match wins**:

| Priority | Option | When to use it |
|---|---|---|
| 1 | **`KeepGameApp=1`** | Stay on the **game's own** Epic app — no redirect at all, just swap the login for Device ID. Best when the game's own backend runs the matchmaking you need. Note some locked deployments reject anonymous logins (403); fall back to a redirect if so. |
| 2 | **Your own Epic app** (all five ids in `[EOS]`) | You want your own session pool, quota and control. All five ids must be present or the block is ignored. |
| 3 | **The built-in default** (nothing configured) | The zero-config path. Release builds have a working Epic app baked in; just deploy the plugin. |

### `[EOS]` reference

| Key | Default | Meaning |
|---|---|---|
| `ProductId`, `SandboxId`, `DeploymentId`, `ClientId`, `ClientSecret` | *built-in default app* | Your own Epic app. **All five are required together** — a partial block is ignored in favour of the default. |
| `DisplayName` | `Player` | Name attached to the anonymous Device ID identity. Give each player a different one. |
| `KeepGameApp` | `0` | Don't redirect; anonymously log into the **game's own** Epic app. Wins over the ids above. |
| `NoPresence` | `0` | For games whose lobbies demand a real linked-platform identity. Forces `bPresenceEnabled=false` on every lobby create/join **and** disables the EOS integrated platform. Symptom it fixes: `Cannot create Lobby, user permissions do not allow it` / `ValidateUserPlatformLoginStatus ... is invalid`. |
| `VerboseLog` | `0` | Route the EOS SDK's own verbose log into `uc_online2.log`. Invaluable when bringing up a new game. |

## Setup

### 1. (Optional) Create your own Epic app

**Skip this** unless you want your own app — release builds already have one baked in.

At <https://dev.epicgames.com/portal/> → create a Product. From **Product Settings** collect:

- **ProductId**, **SandboxId**, **DeploymentId**
- **ClientId** and **ClientSecret** (Clients → add a client; the secret is shown once)

Make sure the client's **Client Policy** permits what the game uses — Connect, plus P2P / Lobbies / Sessions.

> **Do NOT add an Identity Provider.** Every provider in that list (Steam, Epic, …) exists to validate a *real* platform account and would need that platform's Web API key. Device ID login deliberately bypasses all of it.

### 2. Restore the genuine Epic EOS SDK

The plugin hooks the **real** Epic SDK. If you previously dropped in an EOS *emulator* (e.g. Nemirtingas), put the game's original `EOSSDK-Win64-Shipping.dll` back — hooking an emulator does nothing, because it never contacts Epic.

Note the SDK may live in a subfolder, e.g. Forever Skies:
`ProjectZeppelin\Binaries\Win64\RedpointEOS\EOSSDK-Win64-Shipping.dll`

### 3. Deploy

- Build (see below) and drop `EOS_custom.dll` into `<game>\plugins\`
- Drop UCOnline2's `steam_api64.dll` into the game's Steam-loading location (back up the original)

### 4. Configure `union-crax.ini`

Next to the game's shipping exe. **The normal case needs no Epic ids at all:**

```ini
[Settings]
AppId=480
ogAppId=<the game's real Steam AppId>
PluginsFolder=plugins
GetStubbedLol=false

[EOS]
DisplayName=YourName
```

Only if you're bringing **your own** Epic app, add all five ids:

```ini
[EOS]
ProductId=<your ProductId>
SandboxId=<your SandboxId>
DeploymentId=<your DeploymentId>
ClientId=<your ClientId>
ClientSecret=<your ClientSecret>
DisplayName=YourName
```

**Every player must be on the same Epic app, with a different `DisplayName`.** Same app = same session pool; the display name seeds a distinct identity. (With the built-in default that's automatic — everyone shares it.)

If co-op fails with a lobby-permission error, add `NoPresence=1`. To see what the SDK is actually doing, add `VerboseLog=1`.

### 5. Launch

- The **real Steam client must be running and signed in** (UCOnline2 is a passthrough emulator).
- Some games need to be **run as administrator** — Forever Skies does.

## Verify

```powershell
Get-Content "$env:TEMP\uc_online2.log" -Wait -Tail 30 | Select-String '\[EOSAuth\]'
```

- `[EOSAuth] EOS_Platform_Create ... -> REDIRECTED to Product=… Sandbox=… Deployment=…` — platform pointed at your app
- `[EOSAuth] EOS_Connect_Login: ... Credentials.Type=18 (STEAM_SESSION_TICKET) ... -> REWROTE to DEVICEID_ACCESS_TOKEN` — **this is the line that proves the fix**
- `[EOSAuth] Requesting EOS_Connect_CreateDeviceId ...` — first-run device id creation

The real confirmation is in the **game's own log**, which is far more informative than `uc_online2.log` for anything session-related:

```
%LOCALAPPDATA%\<GameName>\Saved\Logs\<GameName>.log
```

`EOS_Connect_ExternalTokenValidationFailed` and `does not have online identity` should both be **gone**.

## Build

```powershell
msbuild plugins\EOS_custom\EOS_custom_plugin.vcxproj -p:Configuration=Release -p:Platform=x64 -m
```

Output: `plugins\EOS_custom\relbuild\x64\EOS_custom.dll`. Single-source; MinHook statically linked.

## Notes and limitations

- **Whichever Epic app you use is doing the hosting.** You can only play with people on the *same* app, and you won't see legitimate players — that's deliberate, it keeps you off the publisher's backend. With the built-in default, everyone on a stock release shares one app and its free-tier limits; supply your own ids if you'd rather have your own pool and quota.
- **The baked-in credentials are recoverable from the shipped DLL.** It's a public binary — that app is shared and disposable, never treat it as a secret.
- Anonymous Device ID identities are per-machine. Deleting the local device id means a new `ProductUserId`.
- Struct offsets are header-verified at **1.19.1.2** and empirically confirmed at **1.15.5** (via two shipping games; Epic no longer distributes SDKs below 1.17) (`ProductId` +16, `SandboxId` +24, `ClientCredentials` +32, `DeploymentId` +80). Structs the plugin *builds* are declared at the newest layout it knows and clamp their advertised `ApiVersion` to match, so the SDK is never told to read a field that isn't there.
- If a game's own code *also* validates the platform identity (not just EOS), it may need extra handling.
- The hooks install from `DllMain` because the EOS SDK usually loads before `UCO_PluginInit`; a fallback logger writes to `%TEMP%\uc_online2.log` until the host's logger is available.
