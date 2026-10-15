# unity_auth_bypass plugin

**Gets past Unity Gaming Services (UGS) sign-in in Unity IL2CPP games running under UCOnline2.**

Many modern Unity games gate multiplayer behind UGS authentication. The normal flow is:

```csharp
AuthenticationService.Instance.SignInWithSteamAsync(ticket)
```

which internally:

1. fetches a Steam auth ticket via `ISteamUser`,
2. POSTs `{ ticket, appId }` to Unity's UGS backend,
3. UGS asks the Steam Web API *"is this ticket entitled to AppId X?"*,
4. on success → `IsSignedIn = true`, `PlayerInfo` populated, game proceeds,
5. on failure → `SignInFailed`, and the game shows something like *"Failed to get Steam account information"* or *"Not signed into Unity Services"* and refuses to start multiplayer.

Because UCOnline2 spoofs **AppId 480 (Spacewar)**, step 3 always fails: the ticket is for 480, but UGS asks Steam about the game's real AppId.

## The fix

Hook `AuthenticationServiceInternal.SignInWithSteamAsync` and redirect every call to **`SignInAnonymouslyAsync`**. UGS still creates a real, valid player session — it just isn't tied to a Steam account. The game's `IsSignedIn` / `PlayerInfo` checks pass and multiplayer proceeds.

This is the same idea as the EOS Device ID swap in [`EOS_custom`](../EOS_custom/): replace a platform credential the backend will reject with an anonymous one it accepts.

The plugin covers all three overloads the SDK exposes:

```csharp
Task SignInWithSteamAsync(string ticket)
Task SignInWithSteamAsync(string ticket, SignInOptions options)
Task SignInWithSteamAsync(string ticket, string identity, SignInOptions options)
```

## Confirmed working

| Game | Steam AppId | Notes |
|---|---|---|
| _None in the release tree yet_ | | Keep game-specific experiments outside regular release plugins until they are supportable. |

The UGS hook itself is game-agnostic — it targets `Unity.Services.Authentication`, not any game's code — so it should apply to other Unity games that sign into UGS with a Steam ticket.

> Use this standalone plugin for non-Photon UGS games. Game-specific experiments should stay outside release packaging until they are ready.

## Setup

1. Build (see below) and drop `unity_auth_bypass.dll` into `<game>\plugins\`.
2. Drop UCOnline2's `steam_api64.dll` into `<game>\<Game>_Data\Plugins\x86_64\` (back up the original).
3. `union-crax.ini` at the game root — no plugin-specific section needed:
   ```ini
   [Settings]
   AppId=480
   ogAppId=<the game's real Steam AppId>
   PluginsFolder=plugins
   GetStubbedLol=false
   ```

## Verify

```powershell
Get-Content "$env:TEMP\uc_online2.log" -Wait -Tail 40 | Select-String '\[Auth\]'
```

- `[Auth] SignInWithSteamAsync intercepted -> SignInAnonymouslyAsync` — **the line that proves the fix**.

If you never see the intercept line, the game either isn't using UGS Steam sign-in or hadn't reached it yet.

## Build

```powershell
msbuild plugins\unity_auth_bypass\unity_auth_bypass_plugin.vcxproj -p:Configuration=Release -p:Platform=x64 -m
```

Output: `plugins\unity_auth_bypass\relbuild\x64\unity_auth_bypass.dll`. IL2CPP only; MinHook statically linked.

## Limitations

- **IL2CPP only.** It resolves methods through the IL2CPP runtime (`GameAssembly.dll`); a Mono game needs a different approach.
- Anonymous UGS sign-in means **no Steam-linked identity** — anything the game ties to a Steam-backed UGS account (cross-device progression, friend lookups by Steam ID) won't behave normally.
- Doesn't help when the backend validates ownership **server-side** in a way anonymous sign-in can't satisfy — that's a different class of wall entirely.
