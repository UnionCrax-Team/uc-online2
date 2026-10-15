# UCOnline2 Patcher

The GUI patcher provides a preflighted, transactional Windows application while
the original interactive patcher remains available. A normal `patch.bat` launch
asks which interface to use; `/gui` and `/legacy` select one directly.

## Features

- Accepts any game folder through the folder picker or `--game <path>`.
- Searches the selected folder name on Steam and fills the likely game AppId.
- Detects Unity, Unreal, generic engines, architecture, SteamStub and backends.
- Lets the user choose overlay deployment, plugins and individual INI flags.
- Shows the complete settings, warnings and file list before applying a patch.
- Remembers successfully applied Photon, EOS and PlayFab fields in a Windows user-encrypted profile and offers them for later compatible games.
- Backs up every existing target to Local AppData with a JSON/hash manifest.
- Rolls back a failed transaction and restores any selected snapshot.
- Detects stale installed fix files after a patcher update.
- Packages the currently deployed fix into the game root with relative paths.
- Checks GitHub releases, verifies the archive digest and self-updates.

## Development

```powershell
dotnet build tools/UCO2.Patcher/UCO2.Patcher.sln
dotnet run --project tools/UCO2.Patcher/UCO2.Patcher.Tests
dotnet run --project tools/UCO2.Patcher/UCO2.Patcher.Gui
```

The release workflow publishes the GUI as a self-contained `win-x64`
single-file executable named `UCO2.Patcher.exe`.
