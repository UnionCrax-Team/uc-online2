# raft_mp plugin

**Restores multiplayer on the current (post-crossplay) build of Raft (1.1.01) under UCOnline2, by routing its PlayFab backend through a PlayFab title you control.**

Raft **1.09** (pre-crossplay) is pure Steam P2P and works through UCOnline2 with **no plugin**. The **current build (1.1.01)** rebuilt multiplayer on **PlayFab**: it logs into PlayFab with a Steam ticket, gates all multiplayer UI behind that login, and brokers every session — even Steam friend joins — through a **PlayFab Party** relay network. Under UCOnline2 the Steam ticket is for AppID 480, so the real PlayFab login fails and nothing works.

This plugin makes it work by pointing Raft at **your own PlayFab title** and fixing up the login + the native Party endpoint so a Party network actually gets created and joined.

> If you don't need the latest version, **just use Raft 1.09 — it needs no plugin at all.** This plugin is only for people who want current-build multiplayer.

## Confirmed working

| Game | Steam AppId | Notes |
|---|---|---|
| Raft | 648800 | Current build **1.1.01**. Two Steam players, each running this plugin, both pointed at the same PlayFab title. Real PlayFab Party co-op (crossplay toggle state doesn't matter). |

## What you need first: your own PlayFab title

1. Create a free title at <https://developer.playfab.com/>. Note its **Title ID** (e.g. `131640`).
2. **Settings → API Features** → allow new player accounts to be created via anonymous login APIs (un-block account creation; new titles disable this by default as of 2025-06-30). Needed for `LoginWithCustomID`.
3. **Multiplayer → Party** → ensure Party is enabled for the title.

## Setup

1. Build (see below) and drop `raft_mp.dll` into `<Raft>\plugins\` **on every player's machine**.
2. Drop UCOnline2's `steam_api64.dll` into `<Raft>\Raft_Data\Plugins\x86_64\` (back up the original).
3. `union-crax.ini` at the Raft root — add a `[Playfab]` section with **your** Title ID:
   ```ini
   [Settings]
   AppId=480
   ogAppId=648800
   PluginsFolder=plugins
   GetStubbedLol=false

   [Playfab]
   TitleId=131640
   ```
4. Launch Raft on both machines, host, and join via Steam friends list → **Join Game**.

All players must run the same Raft build, the same plugin, and the **same `TitleId`** (they share one PlayFab title).

## What it does (four modules, all from a hook on `Raft_Network.Update()` + WinHTTP)

1. **Offline-gate unlock** — forces `Raft_Network.SignedIntoPlayfab = true` so the multiplayer UI is usable. (Once login succeeds the game sets this itself; the force just covers the startup window. We deliberately do **not** touch `localSteamID` — the game sets it to the PlayFab entity ID on login, and overriding it breaks the joiner's local-player lookup.)
2. **PlayFab title redirect** — calls `PlayFab.PlayFabSettings.set_TitleId(<your TitleId>)` so all C# PlayFab SDK traffic (login, profiles, CloudScript) targets your title.
3. **Login switch** — hooks `PlayFabUnityHttp.MakeApiCall` and rewrites `/Client/LoginWithSteam` → `/Client/LoginWithCustomID` (CustomId = SteamID64, CreateAccount=true). `LoginWithSteam` can't succeed on your title (no Steam publisher key); `LoginWithCustomID` returns a real EntityToken. Response type (`LoginResult`) is identical, so the game's sign-in path runs unchanged.
4. **Native Party endpoint redirect** — `PartyWin32.dll` (the native PlayFab Party library) builds its own endpoint from Raft's real title and isn't affected by module 2. It POSTs `/Party/RequestParty` to `<RaftTitle>.playfabapi.com` with your entity token → PlayFab rejects (`InvalidAPIEndpoint`). PartyWin32 uses WinHTTP, so we hook `WinHttpConnect` and rewrite the host `*.playfabapi.com` → `<your TitleId>.playfabapi.com`. Both share the `*.playfabapi.com` wildcard cert, so TLS still validates.

`GetProfile` / `get_platform_id` CloudScript will still log errors (they need the publisher's profile/CloudScript setup) but they're **cosmetic** — they don't block the join.

## Verify

```powershell
Get-Content "$env:TEMP\uc_online2.log" -Wait -Tail 40 | Select-String '\[Raft'
```

Expected, in order:
- `[RaftUnlock] applied: SignedIntoPlayfab=true …`
- `[RaftRedirect] PlayFabSettings.TitleId -> <yourTitle>`
- `[RaftLogin] LoginWithSteam -> LoginWithCustomID (CustomId=…)`
- `[RaftParty] WinHttpConnect redirect <RaftTitle>.playfabapi.com -> <yourTitle>.playfabapi.com`

And in Raft's `Player.log`: `Congratulations, you made your first successful API call!`, then `OnNetworkJoined`, `OnRemotePlayerJoinedNetwork`, world transfer, and the friend stays in.

## Build

```powershell
msbuild plugins\raft_mp\raft_mp_plugin.vcxproj -p:Configuration=Release -p:Platform=x64 -m
```

Output: `plugins\raft_mp\relbuild\x64\raft_mp.dll`. Mono + WinHTTP; MinHook statically linked.

## Limitations

- Needs a **PlayFab title you control** (with account-creation + Party enabled). All players share it.
- PlayFab free/Development tier Party quotas apply (fine for a friend group).
- Keyed to the current build's names/offsets (`Raft_Network.SignedIntoPlayfab`/`Update`, `PlayFabSettings.set_TitleId`, `PlayFabUnityHttp.MakeApiCall`, `CallRequestContainer.FullUrl/Payload`). A Raft update may require re-decompiling and adjusting.
- `GetProfile`/`get_platform_id` (player platform metadata) don't work — they need the publisher's server-side setup. Cosmetic only.
- 1.09 remains the zero-effort path if you don't need the current version.
