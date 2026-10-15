# playfab_universal plugin

**Points a Unity/Mono game's PlayFab backend at a PlayFab title *you* control, and logs in without Steam ownership validation.**

This is the game-agnostic version of what [`raft_mp`](../raft_mp/) does for Raft — the same four modules, with every game-specific detail moved into `union-crax.ini`.

## The problem

Games that gate multiplayer behind PlayFab call:

```csharp
PlayFabClientAPI.LoginWithSteam(...)
```

PlayFab then asks Steam *"does this ticket own AppId X?"*. Under UCOnline2 the ticket is minted for the spoofed **AppId 480 (Spacewar)**, so the answer is no and login fails. You'll typically see the game sit in an offline/limited state with multiplayer greyed out.

You can't fix this on the publisher's title — you don't own it. But **PlayFab titles are free**, so you point the game at your own and swap the login for an anonymous `CustomID` that any title accepts.

This is the same escape hatch as [`EOS_custom`](../EOS_custom/) (your own Epic app + Device ID) and `photon_universal` (your own Photon AppId). The rule across all three:

> Auth you can redirect to **your own app** → solvable.
> Auth validated by the **publisher's own service** → not solvable client-side.

## What it does

| # | Module | How |
|---|---|---|
| 1 | **Title redirect** | Sets `PlayFab.PlayFabSettings.TitleId` to yours. Done at the source, so it's transport-agnostic. |
| 2 | **Login switch** | Hooks `PlayFabUnityHttp.MakeApiCall` and rewrites the request in flight: URL `/Client/LoginWithSteam` → `/Client/LoginWithCustomID`, body → `{"TitleId":"…","CustomId":"…","CreateAccount":true}`. |
| 3 | **Native endpoint redirect** | Hooks `WinHttpConnect` and rewrites `*.playfabapi.com` → `<yours>.playfabapi.com`, for PlayFab Party. |
| 4 | **Offline-gate unlock** *(optional)* | Forces a named static `bool` true, so multiplayer UI is usable before login resolves. |

### Why module 3 exists

PlayFab Party (`PartyWin32.dll` / `PartyWin.dll`) is **native** and never sees the C# `TitleId`. It POSTs to the *original* title using *our* entity token, and PlayFab rejects it with `InvalidAPIEndpoint`. Rewriting the WinHTTP host fixes it — both hosts share the `*.playfabapi.com` wildcard cert, so TLS still validates.

### Why the managed hook, not WinHTTP, for module 1

WinHTTP hooking does **not** reach Unity's own traffic. `UnityPlayer.dll` only imports `WinHttpGetIEProxyConfigForCurrentUser`; `UnityWebRequest` has its own HTTP stack. That's why the TitleId is set at the C# source instead. Module 2 additionally rewrites the host on any request still aimed at the old title, as a safety net if `set_TitleId` lands after the game's first call.

## Setup

### No Man's Sky (AppId 275850) — quickstart

Full Steam↔Steam co-op works. NMS is a **native** (non-Mono) PlayFab game, so modules 3 + 5 carry it: the WinHTTP host-rewrite plus the libHttpClient `LoginWithSteam → LoginWithCustomID` swap (the login body is set via a streaming read-function, which the plugin hooks). No `Gate*` keys needed.

> **Both players must be on the SAME PlayFab title.** NMS brokers the co-op session through PlayFab (Party), so two people on *different* titles can't find each other. Pick one:
>
> - **Play with anyone — use the shared community title `1D861F`.** It has `LoginWithCustomID` enabled and is open for exactly this. Everyone who sets `TitleId=1D861F` lands in the same pool and can connect with each other — no PlayFab account needed.
> - **Private group:** make your own title (step 1 below) and have everyone in your group use *that* ID.

```ini
[Settings]
AppId=480
ogAppId=275850
PluginsFolder=plugins
GetStubbedLol=false
; leave SDR unset/no — the SDR split-context needs a real 275850 license and
; will fail SteamAPI_Init for accounts that don't own the game.

[PlayFab]
TitleId=1D861F
```

Also needed:
- **UCOnline2 ≥ v1.19.3** — it includes the `SteamNetworkingSockets` **AcceptConnection** fix that the co-op P2P transport relies on.
- Launch the **unpacked** exe (the packed one trips SteamStub / wants admin), with **real Steam running** (UCOnline2 is a passthrough).
- The failing `merged-nms-auth.nomanssky.com` calls in the background are fine — that's HG cloud features (saves / discoveries / bases), **not** co-op.

### 1. Create your PlayFab title

1. Sign up at [playfab.com](https://playfab.com) (free tier is plenty) and create a **title** — you get a short ID like `A1B2C`.
2. **Settings → API Features → Allow client to post player statistics / Enable API access** as the game needs.
3. Make sure **`LoginWithCustomID` account creation** is permitted (default).

You do **not** need the Steam add-on — that's the thing being bypassed.

### 2. Deploy

Drop `playfab_universal.dll` into `<game>\plugins\`, and UCOnline2's `steam_api64.dll` into wherever the game loads it from (back up the original).

### 3. Configure `union-crax.ini` (game root)

```ini
[Settings]
AppId=480
ogAppId=<the game's real Steam AppId>
PluginsFolder=plugins
GetStubbedLol=false

[PlayFab]
TitleId=A1B2C
```

That's the minimum. Everything below is optional:

| Key | Default | Meaning |
|---|---|---|
| `TitleId` | *(none — plugin idles)* | **Required.** Your PlayFab title ID. |
| `CustomId` | *your Steam ID* | Literal identity to log in as. Leave empty so each player gets a stable, distinct entity. |
| `LoginEndpoints` | `LoginWithSteam` | Comma-separated logins to rewrite. Add e.g. `LoginWithXbox` for a game that uses a different platform login. |
| `RedirectNativeHttp` | `1` | Module 3. Set `0` if the game has no PlayFab Party. |
| `GateAssembly` | *(none)* | Module 4: assembly holding the gate class, e.g. `Assembly-CSharp`. |
| `GateNamespace` | *(empty)* | Namespace of the gate class. |
| `GateClass` | *(none)* | Class holding the static bool, e.g. `Raft_Network`. |
| `GateField` | *(none)* | The static bool to force true, e.g. `SignedIntoPlayfab`. |

> `[Playfab]` also works — INI lookup is case-insensitive. An existing `[Raft] PlayFabTitleId=` is read as a last fallback so old `raft_mp` installs keep working.

### Raft

[`raft_mp`](../raft_mp/) still exists and is still the simplest way to play Raft — it needs no `Gate*` keys and is unchanged. If you have both DLLs deployed, `raft_mp` detects this plugin and drops into **companion mode**, keeping only its Raft-specific `Raft_Network.Update` gate and leaving title/login/Party to this plugin. Nothing contends.

If you'd rather run this plugin *without* `raft_mp`, the gate is reproducible from ini:

```ini
[PlayFab]
TitleId=A1B2C
GateAssembly=Assembly-CSharp
GateClass=Raft_Network
GateField=SignedIntoPlayfab
```

`raft_mp` drives that gate from Raft's own `Update`, which is tighter than this plugin's 200 ms poll — so for Raft specifically, running both is the better setup.

> **Do not** also force `Raft_Network.localSteamID`. The current build identifies players over the Party network by their **PlayFab entity ID**, which the game's own `OnPlayFabSignedIn` assigns to that field. Overwriting it with the raw Steam ID makes the joiner fail to find itself in the received world (NRE in `GameManager.OnWorldRecieved` → `SelfDisconnect`).

## Verify

```powershell
Get-Content "$env:TEMP\uc_online2.log" -Wait -Tail 40 | Select-String '\[PlayFab\]'
```

| Line | Means |
|---|---|
| `init: … TitleId=A1B2C …` | Config read. |
| `PlayFabSettings.TitleId -> A1B2C` | Module 1 landed. |
| `MakeApiCall hook @ … (FullUrl off …)` | Module 2 installed. |
| `login switched -> LoginWithCustomID (CustomId=…)` | **The line that proves the fix.** |
| `WinHttpConnect redirect … -> A1B2C.playfabapi.com` | Module 3 caught Party. |
| `gate forced: Raft_Network.SignedIntoPlayfab = true` | Module 4 applied. |
| `late host rewrite -> …` | Module 1 lost the race; the safety net covered it. Harmless. |
| `no [PlayFab]TitleId in union-crax.ini` | Not configured — plugin does nothing. |

Confirm server-side in the **PlayFab dashboard**: players should appear under your title after a successful login.

## Build

```powershell
msbuild plugins\playfab_universal\playfab_universal_plugin.vcxproj -p:Configuration=Release -p:Platform=x64 -m
```

Output: `plugins\playfab_universal\relbuild\x64\playfab_universal.dll`. Mono only; MinHook statically linked.

## Limitations

- **Mono only.** It resolves methods through Mono's embed API. An IL2CPP game needs the IL2CPP resolver instead (see `photon_universal`'s `il2cpp_runtime`).
- **Unity C# PlayFab SDK only.** Games using the native C++ SDK (`PFMultiplayer` / `XAsync` / `libHttpClient`) have no `PlayFabUnityHttp` to hook. Those *are* reachable — `libHttpClient.Win32.dll` imports WINHTTP and exports `HCHttpCallRequestSetUrl` / `SetRequestBodyBytes` — but that's a different hook set than this plugin implements.
- **Your title is empty.** Anything the publisher implemented server-side — CloudScript functions, title data, catalogs, matchmaking queues — does not exist on your title. If the game's join flow calls a CloudScript function, you have to reimplement it yourself. This is the wall that stops Raft's *crossplay* (as opposed to its Steam P2P path) and it's usually the real blocker.
- **Anonymous identity.** `LoginWithCustomID` means no Steam-linked PlayFab account, so anything keyed to a Steam-backed entity (cross-device progression, friend lookup by Steam ID) won't behave normally.
- Doesn't help when ownership is validated **server-side by the publisher** — see the Halo `spartan-token` case. Different class of wall entirely.
