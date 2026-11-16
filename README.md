# uc-online2

Custom modified Steam API .dll for Steam games to spoof your game as Spacewar. Drop-in replacement for `steam_api.dll` / `steam_api64.dll`.

> **Does your game work?** Check the **[compatibility list →](https://uco2list.iforgor.cc)**
> — what's confirmed, on which backend (Steam / EOS / Photon / coherence / …), and
> how each build is fixed. It's community-maintained and grows as games are tested.

**Contents**

- [Quick start](#quick-start) · [Manual install](#manual-install)
- [Configuration](#configuration) — [settings reference](#settings-reference), [DLC](#dlc), [ticket emulation](#ticket-emulation), [old-SDK games](#old-sdk-games-client), [SDR](#steam-datagram-relay-sdr), [overlay](#overlay), [SteamStub](#steamstub)
- [Plugins](#plugins) — [loader](#plugin-loader), [ABI](#plugin-abi-v1), [coherence shortcut](#coherence-games-the-easy-route)
- [Troubleshooting](#troubleshooting) — [won't launch](#the-game-wont-launch-or-acts-like-steam-isnt-running), [multiplayer won't connect](#multiplayer-wont-connect), [what this cannot fix](#what-this-cannot-fix)
- [Building](#building) · [Forking](#forking--modifications) · [Known issues](#known-issues)

---

## Quick start

**Run `UCO2.Patcher.exe`** — the release package's folder-first GUI
**(experimental)**. Point it at a game folder (or drag the folder onto it): it
searches the folder name on Steam for the game AppId, then offers preflight
detection, large option toggles, a final review, verified backup snapshots,
one-click restore, installed-fix updates and path-preserving fix ZIP packaging.

It will:

- find the engine (Unity or Unreal) and the game's real executable
- find where `steam_api64.dll` actually lives and install ours **there**, backing
  up the original to `.bak` first
- deploy the early Steam overlay proxy as `version.dll` for Unity or
  `winmm.dll` beside the real Unreal shipping executable
- write `union-crax.ini` next to the **running exe** — for Unreal that is not the
  game folder, and an ini in the wrong place is silently ignored
- detect **Photon**, **EOS**, **PlayFab** and **coherence**, copy the matching
  plugin, and prompt for whatever app IDs that backend needs

Everything third-party stays yours: it never invents a Photon GUID, an Epic app
or a coherence project.

> **Working from a clone?** The repo still carries `patch.bat`, the original
> interactive CLI (`patch.bat "C:\Games\SomeGame"`, or append `/keyonly` for just a
> coherence runtime key). It is **no longer shipped in release packages** — the GUI
> is the supported path — but remains for people building from source
> (`patch.bat /legacy` forces the interactive flow, `/gui` launches the GUI).

**What it will not do:**

- Install into a 32-bit game. It refuses rather than writing an x64 DLL where it
  cannot load.
- Redirect EOS to *your own* Epic app unless you fill the `[EOS]` fields — by
  default `EOS_custom` uses the Epic app built into the DLL, so EOS co-op works
  with no setup (see [plugins/EOS_custom](plugins/EOS_custom/README.md)).
- Upload a coherence schema. That needs the Unity editor; see
  [`tools/coherence_schema`](tools/coherence_schema/README.md).

If it reports **no secondary backend**, try the game with no plugin at all —
titles whose multiplayer is purely Steam lobbies and P2P work through
passthrough unmodified.

Steam has to be running, at the same elevation as the game — see
[the game won't launch](#the-game-wont-launch-or-acts-like-steam-isnt-running).

## Manual install

**From a [release](https://github.com/LukeWarmSodas/uc-online2/releases):**

1. Extract the archive from the **latest** release.
2. Back up the game's original DLL by renaming it (`steam_api64.dll.bak`, or
   `steam_api_o.dll` as Goldberg suggests — the name doesn't matter).
3. Copy ours in its place: `x86\steam_api.dll` for 32-bit games,
   `x64\steam_api64.dll` for 64-bit.
4. Make sure Steam is running, then launch the game from its .exe as usual.

If the game has **SteamStub**, either unpack it with Steamless or set
[`GetStubbedLol`](#steamstub).

**From your own build:** see [Building](#building), then copy
`relbuild\x86\steam_api.dll` or `relbuild\x64\steam_api64.dll` the same way.

---

## Configuration

Create `union-crax.ini` **next to the game executable**. Without it, AppId
defaults to `480` and no plugins load. `PluginsFolder` is relative to that same
place. Check the `steam_appid.txt` written on launch to confirm your AppId was
accepted.

```ini
[Settings]
AppId=480
ogAppId=220              # Half-Life 2 — the game's REAL AppId
PluginsFolder=plugins
GetStubbedLol=false
EmulateTicket=true
```

Some games have `480` hard-coded in their own code. If yours misbehaves on
Spacewar, try another free multiplayer AppId such as `440` (Team Fortress 2) —
Shapes of Dreams did not work on `480` but was fine on `440`.
*(Thanks to deityofsukana for pinning that down.)*

### What `ogAppId` is for

This lets the overlay use the right game assets even though you are ostensibly
running Spacewar. The AppId you set here is converted to the 64-bit Game ID
string Steam expects and used for the `SteamOverlayGameId` environment variable.
You could pass that as a launch argument yourself, but you would need to know the
long numeric form — this just makes it easier.

`SteamGameId` is deliberately **not** touched by `ogAppId`, because changing it
causes problems; that one follows `AppId` (also converted to the 64-bit form).

`ogAppId` is also what DLC, ticket emulation and the stats fix answer *as*, so
set it for any game where you care about those.

### Settings reference

All keys live under `[Settings]` unless noted.

| Key | Default | What it does |
|---|---|---|
| `AppId` | `480` | The AppId Steam sees. Spacewar unless the game objects. |
| `ogAppId` | *(none)* | The game's real AppId. Used for the overlay, DLC, tickets and stats. |
| `PluginsFolder` | *(none)* | Folder of `.dll` plugins to load, relative to the exe. |
| `EmulateTicket` | `false` | [Ticket emulation](#ticket-emulation) for peer-validated multiplayer. |
| `PassthroughTicket` | `false` | Explicitly use genuine Steam tickets. Overrides `EmulateTicket` when both are enabled. |
| `Client` | *(none)* | Pin the `SteamClient()` version for [old-SDK games](#old-sdk-games-client), e.g. `017`. |
| `SDR` | `false` | [Steam Datagram Relay](#steam-datagram-relay-sdr) split context. Requires `ogAppId`. |
| `GetStubbedLol` | `false` | Patch [SteamStub](#steamstub) at runtime instead of unpacking. |
| `LoadOverlay` | `true` | Set `no` to keep Steam's overlay renderer [out of the process](#overlay). |
| `LogOverlay` | `no` | Write `steam_overlay.log` from the early overlay proxy. |
| `WarnOverlayDisabled` | `false` | Log a startup hint when the overlay looks like it won't work. |
| `UnlockDLC` | *(none)* | Legacy comma-separated DLC list; prefer the [`[DLC]`](#dlc) section. |
| `VerboseLog` | `false` | Per-frame callback traces. Very noisy — for debugging only. |
| `LocalSaves` | `true` | Keep saves out of the spoofed AppId's shared cloud folder. See [Saves and Steam Cloud](#saves-and-steam-cloud). |
| `RealAppIdEnv` | `false` | Show the game its real AppId in the `SteamAppId` env for a startup check. Requires `ogAppId`. |
| `InventoryAutoGrant` | `false` | Serve `ISteamInventory` purchases/grants from a local store. |
| `ForceOwnership` | `true` | Answer ownership checks for the real AppId. |

### Saves and Steam Cloud

Real Steam binds `ISteamRemoteStorage` to the app the process is **running as**, which
under UCOnline2 is the spoofed AppId. Left alone, that means every UCO2 game on the
machine shares one cloud folder:

```
Steam\userdata\<account>\480\remote\
    OutbreakSettings.data          <- game A
    SaveData                       <- game B   (generic name: collision bait)
    Saved\SaveGames\SaveData.dat   <- game C
```

Two games that pick the same save name overwrite each other, and because that folder
really does sync, a second machine can pull one game's `SaveData` down and hand it to
a different game.

`[Settings] LocalSaves` (**on by default**) fixes it in two steps:

1. **Steam Cloud reports as disabled.** Most games keep their own local save path and
   simply use it — `%LOCALAPPDATA%`, `AppData\LocalLow`, `Documents\My Games` — which
   is where that game's own backups, wikis and save editors expect to find it.
2. **The Steam file API is served from `<game folder>\uco_cloud\`** for the games that
   have no other path. Plenty of titles use `ISteamRemoteStorage` *as* their
   filesystem, and for those, "cloud off" changes nothing on real Steam — that flag
   only stops the upload; writes still go to the shared folder. So they get somewhere
   else to land.

Anything already in the spoofed app's cloud folder is **copied** into `uco_cloud\` the
first time the game runs with this on, so an existing save is not left behind. Nothing
is deleted — the originals stay where they were, and `%TEMP%\uc_online2.log` lists what
was imported.

Set `LocalSaves=false` to go back to writing straight into the spoofed AppId's cloud
folder. Workshop/UGC is unaffected either way; it still talks to real Steam.

Two limits worth knowing:

- **32-bit games** only get the flat-export half of this. The C++ interface methods are
  `__thiscall` on x86 and the detours are `__cdecl`, so hooking them would scramble the
  arguments — and a scrambled save write is worse than no fix. Most 32-bit titles
  (Steamworks.NET, older Unity) go through the flat exports anyway.
- **Steam Auto-Cloud** games — where the file patterns live in Valve's app config rather
  than in the game's code — were never affected. Under the spoofed AppId the Steam
  client has no auto-cloud config to act on, so those saves were always local.

### DLC

Games ask about DLC in two different ways, and both have to be answered:

- **"Do I own AppId 211?"** — `BIsSubscribedApp` / `BIsDlcInstalled`
- **"List my DLC"** — `GetDLCCount` + `BGetDLCDataByIndex`

The second kind is why unlocking used to look unreliable: a game that checks ids it
already knows worked, while a game that builds its DLC menu by *enumerating* saw
nothing, because real Steam answers for the spoofed AppId and reports no DLC.

```ini
[DLC]
UnlockAll=1              ; answer "owned" for any DLC id the game asks about

; Named entries are what enumeration can report. Needed by games that build a
; DLC list from the API rather than checking ids they already know.
211=Half-Life 2: Deathmatch
212=Half-Life 2: Lost Coast
```

`UnlockAll=1` handles every ownership *question* without you knowing any ids, but it
cannot invent a *list* — only named entries appear in `GetDLCCount` /
`BGetDLCDataByIndex`. So if DLC still doesn't show up in-game, add the entries
explicitly; the ids are on SteamDB under the game's DLC tab.

The old comma-separated form still works and is merged in (those entries enumerate
as `DLC <id>`, since they carry no name):

```ini
[Settings]
UnlockDLC=211,212,213,218
```

To check what was picked up, look in `%TEMP%/uc_online2.log` for:

```
[UCOnline2] DLC store: UnlockAll=1, 2 entries
[UCOnline2] ISteamApps DLC hooks: 5/5 installed (UnlockAll=1, 2 configured DLC)
```

> DLC hooks are skipped when [`Client`](#old-sdk-games-client) is set — an old-SDK
> game uses an older `ISteamApps` whose vtable doesn't match the one we hook, and
> forcing it there crashes the game.

### Ticket emulation

Set `PassthroughTicket=yes` when the game needs Steam to return a genuine ticket
for `AppId`, but a later authentication step is handled separately. The normal
Steam calls and callbacks remain untouched. `true`, `1`, `yes`, and `on` are
accepted. If both ticket settings are enabled, passthrough wins.

```ini
[Settings]
AppId=480
PassthroughTicket=yes
```

`EmulateTicket` covers **both** kinds of Steam ticket a game may ask for:

- **Encrypted app ticket** — `GetEncryptedAppTicket` / `RequestEncryptedAppTicket`, plus `UserHasLicenseForApp` ownership checks, answered for `ogAppId` (or `AppId` if unset).
- **Auth session ticket** — `GetAuthSessionTicket`, plus the legacy `InitiateGameConnection` client/server handshake used by older Steam integrations. Passed straight through, real Steam mints the ticket under the *spoofed* AppId, so the host rejects it. The tell in the log is the game registering callback `163` (`GetAuthSessionTicketResponse_t`) repeatedly: asking for a ticket, never getting a usable one, retrying.

`BeginAuthSession` / `EndAuthSession` / `CancelAuthTicket` are emulated to match, and the response callbacks (`GetAuthSessionTicketResponse_t`, `ValidateAuthTicketResponse_t`) are delivered — a game that *waits* on those would otherwise hang even with a valid-looking ticket.

This works because the check is **peer-side**: both players run the emulator, so one mints the ticket and the other accepts it. It does **not** help where a publisher's own server asks Steam to validate the ticket — see [what this cannot fix](#what-this-cannot-fix).

```ini
[Settings]
AppId=480
ogAppId=440              # tickets are emulated for this
EmulateTicket=true
```

Look for this in `%TEMP%/uc_online2.log`:

```
[UCOnline2] auth ticket emulation: 6/6 hooks installed
[UCOnline2] GetAuthSessionTicket emulated -> handle=1 appid=2300320 size=64
[UCOnline2] BeginAuthSession emulated -> OK for 7656119... (64 byte ticket)
```

> I really didn't know how to actually go about this, sorry — I used AI to try and
> finish what I had gotten through with it. I don't know how OFME utilizes it, but I
> can assume it actually is a ticket emulation system, which is what I tried to make.
> I can't say for certain whether it works in every case, so I'll rely on the
> community to find out from testing. Sorry to throw that on y'all. ^^;

### Old-SDK games (`Client`)

```ini
[Settings]
Client=017
```

Some older games don't ask for interfaces by name at all: they call the exported
`SteamClient()` accessor and then walk the returned vtable using the offsets from
the SDK **they** were compiled against. UCOnline2 normally hands back a modern
`SteamClient023`, so every getter lands on the wrong slot — the game stores
whatever comes back and crashes later when it uses it.

`Client=017` pins what that accessor returns, so the vtable matches what the game
expects. UCOnline2 keeps using the modern interface internally. Both `017` and
`SteamClient017` are accepted. Leave it unset unless you need it.

**Rivals of Aether** needs `Client=017`. Without it the game dies a few seconds
into startup with an access violation reading address 0, inside its own code,
with nothing obviously failing in the log.

Old-SDK games tend to need more than the pin, and UCOnline2 handles the rest
automatically once `Client` is set: pre-018 `SteamClient` requests pass through to
real Steam (so a game's own extension DLL can link), stats callbacks get their
game id rewritten to `ogAppId`, and the DLC hooks are skipped.

### Steam Datagram Relay (SDR)

Games that use SDR may need UCOnline2 to create the real Steam networking context
before switching back to the spoofed AppId:

```ini
[Settings]
AppId=480
ogAppId=<the game's real Steam AppId>
SDR=yes
```

`SDR=yes` requires `ogAppId`. It uses the older `SteamClient017` connection path
for compatibility, then exposes the current Steam interfaces to the game. Leave it
disabled unless the game uses Steam Networking Sockets / SDR — it stamps the real
AppId into the networking context, which fails for accounts that don't own the game.

### Overlay

UCOnline2 pulls Steam's `GameOverlayRenderer` into the process so a
directly-launched game still gets the overlay. The patcher additionally deploys an
early proxy (`version.dll` / `winmm.dll`) for engines that load
`steam_api64.dll` too late for the overlay to hook the swapchain — see
[`plugins/steam_overlay`](plugins/steam_overlay/README.md).

```ini
[Settings]
LoadOverlay=no           # skip loading the overlay renderer entirely
LogOverlay=yes           # write steam_overlay.log next to the exe
WarnOverlayDisabled=true # log a hint at startup if the overlay looks unavailable
```

The renderer hooks D3D/DXGI early, and a game that objects to that has no other
way to opt out — `LoadOverlay=no` is that escape hatch. You lose the overlay,
nothing else.

Treat `WarnOverlayDisabled` as a hint, not a verdict: the overlay does work for
directly-launched games, but a few titles never show it regardless.

### Early loading (`LoadDLLsEarly`)

Unity games P/Invoke `steam_api64.dll` lazily — it isn't loaded until the game
makes its first Steam call. For a game that *also* uses EOS or PlayFab, that can
happen **after** the game has already initialised its backend, so a plugin
(`EOS_custom`, `playfab_universal`) installs its hooks too late to redirect it
and online play silently fails to connect.

The early proxy (`version.dll` / `winmm.dll`) loads before graphics init, so
it can pull `steam_api64` in ahead of time and arm those plugin hooks in advance:

```ini
[VersionProxy]
LoadDLLsEarly=true
```

The patcher sets this automatically whenever a plugin is
deployed, so you rarely write it by hand. It's a no-op when `steam_api64` is
already loaded (e.g. games that import it statically), so it's safe to leave on.
The proxy's overlay renderer is separate and still honours
[`LoadOverlay`](#overlay), so a D3D12 title can use the proxy purely as an early
loader with `LoadOverlay=no`.

`[VersionProxy] SdrSafe` covers a second early-load hazard. With
[SDR](#steam-datagram-relay-sdr), relay authorisation is keyed off the
**process's** Steam AppId — but the overlay proxy otherwise preloads
`steamclient` and the overlay as **spacewar (480)**, binding the process to
`480` before UCO2's SDR path can claim `ogAppId`, so the relay refuses to route.

```ini
[VersionProxy]
SdrSafe=true
```

`SdrSafe=true` makes the proxy stamp the **real** AppId (`ogAppId`) onto the
process context while pointing the overlay at the spoofed id via
`SteamOverlayGameId` — the same split UCO2's own `SetAppIDEnv` uses. The GUI
patcher sets it automatically when SDR is enabled. (`LoadOverlay=no` sidesteps
the early overlay load entirely, which is the usual choice for the D3D12 titles
SDR tends to appear in.)

`[VersionProxy] RequireSteam` (**on by default**) makes the proxy check that the
Steam client is running in its DllMain — before the game does anything — and, if
it isn't, show a "Steam isn't running" warning and abort the launch. UCOnline2
proxies the real Steam client, so a directly-launched game (common for SteamStub
titles) with Steam closed otherwise fails in a confusing way. The check is
conservative — only a definitive "Steam not running" aborts — and is a no-op for
any game launched through Steam. Set `RequireSteam=false` to opt out.

### SteamStub

If `GetStubbedLol` is enabled, UCOnline2 patches SteamStub on the fly. This is
meant for games Steamless cannot unpack, such as Dave the Diver. It can also be
used to avoid modifying the game files at all. If it's disabled, the function is
ignored entirely and everything continues as though it were never implemented.

The stub's ownership check runs at the **exe entry point**, before a Unity game
lazily P/Invokes `steam_api64` — and a wrapped exe that fails the check exits
before `steam_api64` ever loads. So the bypass is armed from the early proxy
([`version.dll`](#early-loading-loaddllsearly)) in its DllMain, before the entry
point, whenever `GetStubbedLol=true` and the proxy is deployed. `steam_api64`
still arms it as a fallback for games that load it early, coordinating through an
environment flag so GetTickCount is never hooked twice. The `cmp al, 30h` and
`cmp bl, 30h` check signatures track the current upstream.

I'm not responsible for the original code — it came from DenuvoSanctuary's Rust
implementation, [found here](https://github.com/denuvosanctuary/steamstubbed). I
rewrote it in C++ so I could integrate it into this project rather than injecting
it. I did not ask for permission, so if there's an issue with that, contact me and
I'll remove it or work something out.

---

## Plugins

### Plugin loader

If `PluginsFolder` is set, all `.dll` files in that folder load at startup in
alphabetical order. Use prefixes to control load order:

```
plugins/
  01_first_plugin.dll
  02_second_plugin.dll
  03_another_one_(dj_khaled!!).dll
```

### Plugin ABI (v1)

Plugins that want to integrate with UCOnline2 (rather than just running DllMain side effects) can export two C functions defined in [`include/uco_plugin.h`](include/uco_plugin.h):

```c
__declspec(dllexport) int  UCO_PluginInit(const UCO_PluginContext* ctx);
__declspec(dllexport) void UCO_PluginShutdown(void);
```

- `UCO_PluginInit` is called once after `SteamAPI_Init` succeeds. The context exposes the resolved `ISteam*` interfaces, the configured AppId / ogAppId, a logger, and a callback-patcher registration function. Return 0 for success.
- `UCO_PluginShutdown` is called on detach (in reverse load order). Use it to disable MinHook hooks and clean up.

**What lives in core vs in a plugin.** Core (`steam_api64.dll`) only ships generic
Steam-side spoofing — `ISteamUtils::GetAppID` and `ISteamApps::BIsSubscribedApp`
are vtable-hooked to report `ogAppId`, so games that ask "what AppId am I, and do
I own it?" get a consistent answer.

Anything game-specific — ticket synthesis, `BeginAuthSession` bypass, EOSSDK
hooks, custom networking, IL2CPP patches — belongs in a plugin.

### coherence games: the easy route

A coherence game has to point at a coherence project that has its schema
uploaded. Setting that up yourself means an account, the Unity editor and a
schema upload — so there's a shortcut: a shared community project.

**Use the shared project's runtime key** `fce1ea692a854b50b9f945ef6aa17758` — paste
it into the coherence runtime-key field in the GUI (or answer `SHARED` at the
`patch.bat` CLI's runtime-key prompt, which fills in that same key). It points the
game at a community project which already has the schema uploaded and every region
enabled: no account, no Unity, nothing else to do. (A runtime key is a client-side
identifier that ships in every coherence game, like a Photon AppId — safe to share.)

It is a free tier, unmonitored, and shared with everyone else using it, so
**availability is not guaranteed** — it may be rate-limited or rotated without
notice, and everyone on it sees everyone else's lobbies. If co-op stops working,
suspect that first and set up your own project with
[`tools/coherence_schema`](tools/coherence_schema/README.md).

`/keyonly` changes which project a game points at and nothing else — handy for
switching between your own project and the shared one without disturbing a
working install. It does **not** deploy the emulator or the plugin, so for a
fresh install do a normal full run and answer `SHARED` there instead. Running it
twice is safe; the second run reports the key already matches.

---

## Troubleshooting

### The game won't launch, or acts like Steam isn't running

**Check elevation first.** UCOnline2 is a **passthrough** — the real Steam client
has to be running, and the game talks to it over Steam's IPC. That IPC is
sensitive to Windows integrity levels, so **run Steam and the game at the same
elevation.**

| Steam | Game | Result |
|---|---|---|
| normal | normal | ✅ works — the usual setup |
| **admin** | **admin** | ✅ works — use this when the game needs admin |
| normal | admin | ✅ usually works |
| admin | normal | ❌ won't work — the game can't reach Steam |

A mismatch typically looks like the game refusing to start at all, an init/auth
failure, or the game behaving as though Steam isn't running.

**Some games genuinely require admin.** Those aren't broken — bring Steam up to
match:

1. Fully exit Steam (tray icon → Exit — not just closing the window).
2. Right-click `steam.exe` → **Run as administrator**, let it finish signing in.
3. Launch the game as admin as usual.

If a game *doesn't* need admin, leave both normal.

Two things that catch people out:

- Windows silently elevates a child process when its parent is elevated — launching the game from an elevated launcher, script, or terminal elevates the game too, even with every compatibility checkbox clear.
- Closing Steam's window only hides it to the tray. If you're switching Steam's elevation you have to actually **Exit** it first, or you'll just reattach to the still-running non-elevated instance.

If the game throws an auth error, restart Steam and try again.

### Multiplayer won't connect

If the game launches and everything looks healthy but you can't join anyone,
work through these in order.

**1. Are you both on UCOnline2?** You share AppId `480`'s lobby pool, so someone
on a legitimate copy is in a different pool entirely and will never see your
lobbies. Both sides need the same setup, and the same game version.

**2. Do your backend IDs match exactly?** For Photon, PlayFab, EOS or coherence,
everyone must use the **same** app/title/project id. Two valid-but-different ids
fail silently: everything initialises, nobody finds anybody.

**3. Check the log for the real error.** `%TEMP%/uc_online2.log` will show whether
the backend actually authenticated. If it did, and you still can't connect, the
problem is the network path — read on.

**4. Your connection may be blocking peer-to-peer traffic.** This one is worth
knowing about because it looks *exactly* like a broken fix:

- the game launches fine, no errors anywhere
- the backend logs in (you can see yourself in the PlayFab/Photon dashboard)
- you can **see** the host, click join, get "connecting…", then lose the connection
- **someone else on the identical setup connects without trouble**

Multiplayer backends carry the actual session over **UDP relays** (Valve's SDR,
PlayFab Party over Azure, Photon's relays). Logging in is plain HTTPS and works
almost anywhere; the session transport is what gets blocked. Causes, cheapest
first:

- **Windows Firewall** — allow the game on **both** Private and Public profiles
- **Antivirus with network/web protection** — several filter UDP; test with it off
- **Mobile data or a phone hotspot** — carrier-grade NAT, very often fatal to P2P
- **ISP-level filtering** — some countries interfere with P2P, VoIP and relay
  traffic wholesale. This is real and it is not something the emulator can fix.

**Confirming it isn't us:** try **Steam Remote Play Together** with the same
person. It rides the same relay network and involves none of this project's code.
If that fails too, the problem is your network path.

**The fix is to tunnel out.** Start with **[Cloudflare WARP](https://one.one.one.one/)**
— it's free, takes five minutes, and has resolved this in practice (a user who
could not join anyone was able to join immediately over WARP). If WARP doesn't
help, a **full-tunnel VPN** is the next step; note that a split tunnel or a
browser-only proxy will *not* carry the UDP that's actually broken. In countries
where commercial VPNs are themselves blocked, a self-hosted WireGuard server on a
cheap VPS tends to survive, since it isn't a published provider IP range.

None of this is a UCOnline2 problem, and the compatibility list should not record
a game as broken because of it.

### What this cannot fix

Some games are out of reach no matter what the emulator does, and it is worth
knowing the shape of that before spending an evening on one.

**Servers that verify a Steam app-ownership ticket.** The game asks Steam for an
auth session ticket and sends it to the publisher, whose server checks Valve's
**128-byte signature** on it. That signature cannot be produced without Valve's
private key, so no amount of client-side work passes the check. `EmulateTicket`
produces a structurally correct ticket, which is enough for a peer that runs this
same emulator, and never enough for a server that validates properly.

Confirmed on **Farming Simulator 25** by hooking `GetAuthSessionTicket` inside a
competing fix and reading the bytes it produces: its ticket is a **genuine,
Valve-signed ticket belonging to a different Steam account** — one that actually
owns the game — generated on a server weeks earlier and replayed by every user of
that fix. It works because the credential is real, not because the check is weak.

How to recognise it early:

- the game registers callback `163` (`GetAuthSessionTicketResponse_t`) and the
  connection fails a second or two later — a round trip, not a local error
- a structurally valid emulated ticket still gets rejected

**Related walls**, all server-side and all unforgeable: a publisher backend that
allow-lists its own application ID, an account service that mints its own session
token from a platform ticket, and kernel-level anti-cheat. If ownership is proven
to a machine you do not control, the answer is no.

**What still works:** everything validated *peer-side*. Games where multiplayer is
Steam lobbies plus P2P, or where the host itself validates the joining player, are
the normal case and are what this project is for.

---

## Building

**Quick way (true Chad way — quick, simple, and easy):**

1. Run `build.bat`.
2. ???
3. Profit.

**With Visual Studio (the bum way — requires too much effort):**

1. Open `uc_online2.vcxproj`.
2. Select Release | Win32 or Release | x64.
3. Build.

Requires Visual Studio 2022 with the C/C++ environment (v143 or higher toolset).
If MSBuild isn't found, `build.bat` will tell you where to get it. That script has
a preset path matching my personal setup — if it doesn't match yours you'll need
to edit it, or you'll error out even with everything installed.

## Forking / Modifications

- Please feel free to fork this and modify it however you see fit, and if you feel it would benefit the original repo, make a PR and I'll look into it.
- You are allowed to modify this and distribute it to your liking, all I ask is that I'm made aware of this. Read the license for more info.
   - I only want this so I can be certain that it's not being distributed as a method to spread malware. (I should make the license have something for this, or make it better...)
- Have any ideas but you're not sure how to implement it? Or don't know how to code? Contact me with your ideas. I'll work on it for you and communicate with you throughout the process.

## Known issues

- **Denuvo games will not work.** If you think it can be made to, modify it so it works like an activated game — but even then I can't guarantee anything. It will likely reject you and your token will be permanently messed up. **I say don't even bother**; it'll waste your time and the activators' time too.
- **DLC you don't own** may still not work in every game. See [DLC](#dlc) for what is and isn't answerable.
- **Games with the AppId hard-coded** (Godot games, for instance) need the game itself modified to use the AppId you want — though if you do that, you probably don't need this at all.
- **VAC-protected servers, or servers hosted on the real AppId**, cannot be joined in Garry's Mod or other Source games and anything with similar protections. (GoldSrc seems fine — CS 1.6 let me join anything.) Please don't message me asking why Garry's Mod won't let you join. Ask me how to play with friends who own legitimate copies instead. :)
- For anything else unexpected, contact me. I haven't tested every game and I rely on the community for that.
