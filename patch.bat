@echo off
rem Release packages include both interfaces. Ask on normal launches, while
rem /gui and /legacy keep shortcuts and automated use deterministic.
set "GUI_EXE=%~dp0UCO2.Patcher.exe"
if /i "%~1"=="/legacy" (
  shift
  goto legacy_start
)
if /i "%~1"=="/gui" (
  shift
  goto launch_gui
)
if exist "%GUI_EXE%" (
  echo.
  choice /c YN /n /m "Use the new UCOnline2 GUI (experimental)? [Y/N] "
  if errorlevel 2 goto legacy_start
  goto launch_gui
)
goto legacy_start

:launch_gui
if not exist "%GUI_EXE%" goto legacy_start
if "%~1"=="" (
  start "" "%GUI_EXE%"
) else (
  start "" "%GUI_EXE%" --game "%~1"
)
exit /b 0

:legacy_start
setlocal enableextensions enabledelayedexpansion
title UCOnline2 - patch.bat (auto-patcher)
color 0b

echo ============================================================
echo  UCOnline2 Auto-Patcher
echo ============================================================

rem ============================================================
rem  UCOnline2 patch.bat
rem
rem  Drop a game folder onto this file (or run: patch.bat "C:\path\to\game").
rem
rem  It works out what the game is and does the whole deploy:
rem    * finds the engine (Unity or Unreal) and the real executable
rem    * finds where steam_api64.dll actually lives and installs ours THERE,
rem      backing up the original first
rem    * installs the early overlay proxy as version.dll for Unity or
rem      winmm.dll beside the real Unreal shipping executable
rem    * detects Photon / EOS / PlayFab and copies the matching plugin
rem    * writes a union-crax.ini with the right sections
rem
rem  It does NOT modify game assets. Any third-party app IDs (Photon GUIDs,
rem  your own Epic app, a PlayFab TitleId) are YOURS, so it prompts for them
rem  and will happily write a stub section you can fill in later.
rem
rem  DETECTION NOTE: backends are detected by FILE PRESENCE, and for Unreal
rem  also by ANSI module names inside the shipping exe. Deliberately not by
rem  scanning for endpoint strings -- Unreal stores most literals as UTF-16,
rem  so a byte-level search for "playfabapi.com" finds nothing even in a game
rem  that clearly uses it. Module names like OnlineSubsystemEOS are ANSI and
rem  do match. Enum values are avoided as signals for the same reason a plain
rem  "PlayFab" search is useless: a platform-abstraction plugin lists every
rem  provider it could ever target, so the string proves nothing.
rem ============================================================

set "SCRIPTDIR=%~dp0"

rem ---- locate OUR steam_api64.dll (the emulator itself) ----
rem Three layouts, in the order they turn up:
rem   1. repo checkout -> relbuild\x64\
rem   2. release zip   -> x64\   (release.yml packages the DLLs into x86\ and
rem      x64\, NOT at the package root. Miss this and the released copy of this
rem      script can never find the emulator it is supposed to install.)
rem   3. dropped beside this script
set "EMU_DLL="
if exist "%SCRIPTDIR%relbuild\x64\steam_api64.dll" set "EMU_DLL=%SCRIPTDIR%relbuild\x64\steam_api64.dll"
if not defined EMU_DLL if exist "%SCRIPTDIR%x64\steam_api64.dll" set "EMU_DLL=%SCRIPTDIR%x64\steam_api64.dll"
if not defined EMU_DLL if exist "%SCRIPTDIR%steam_api64.dll" set "EMU_DLL=%SCRIPTDIR%steam_api64.dll"
if not defined EMU_DLL (
  echo [WARN] UCOnline2's steam_api64.dll not found next to this script.
  echo        Build it:  msbuild uc_online2.vcxproj -p:Configuration=Release -p:Platform=x64
  echo        The ini and plugins will still be written; copy the DLL yourself.
  echo.
)

rem ---- locate the early overlay proxy ----
rem One binary is renamed at deployment time. UnityPlayer imports version.dll;
rem UE shipping executables statically import winmm.dll (timeGetTime). The proxy
rem inspects its deployed filename and forwards to the matching real system DLL.
set "OVERLAY_PROXY="
if exist "%SCRIPTDIR%plugins\steam_overlay\relbuild\x64\overlay_proxy.dll" set "OVERLAY_PROXY=%SCRIPTDIR%plugins\steam_overlay\relbuild\x64\overlay_proxy.dll"
if not defined OVERLAY_PROXY if exist "%SCRIPTDIR%plugins\overlay_proxy.dll" set "OVERLAY_PROXY=%SCRIPTDIR%plugins\overlay_proxy.dll"
if not defined OVERLAY_PROXY if exist "%SCRIPTDIR%overlay_proxy.dll" set "OVERLAY_PROXY=%SCRIPTDIR%overlay_proxy.dll"

rem ---- key-only mode ----
rem
rem   patch.bat "C:\path\to\game" /keyonly
rem
rem Changes the coherence project a game points at and nothing else -- no DLL,
rem no plugins, no ini rewrite. Use it to switch between your own project and
rem the shared one without redoing, and possibly disturbing, a working install.
rem
rem For a FRESH install do a normal full run instead and answer SHARED at the
rem runtime-key prompt; key-only does not deploy the emulator or the plugin.
set "KEYONLY="
if /i "%~2"=="/keyonly" set "KEYONLY=1"
if /i "%~1"=="/keyonly" set "KEYONLY=1"
if /i "%~2"=="-keyonly" set "KEYONLY=1"

rem ---- resolve game folder (arg or prompt) ----
set "GAME=%~1"
if /i "%GAME%"=="/keyonly" set "GAME=%~2"
if not defined GAME set /p "GAME=Drag the game folder here (or paste its path): "
set "GAME=%GAME:"=%"
if not defined GAME echo [ERROR] No folder given.& goto :end
if not exist "%GAME%\" echo [ERROR] Not a folder: %GAME%& goto :end
rem Strip a trailing backslash so joined paths never end up with a double one.
if "%GAME:~-1%"=="\" set "GAME=%GAME:~0,-1%"
rem Keep disabled third-party loaders completely outside the game tree. Some
rem games inventory filenames recursively, so an inert OnlineFix64.dll renamed
rem to OnlineFix64.dll.uco-disabled can still trip their integrity checks.
set "QUARANTINE_DIR=%GAME%.uco2-quarantine"
call :stage "1/4  SCAN GAME"
echo   Folder:      %GAME%

rem ============================================================
rem  Identify the engine
rem
rem  Unity  -> a "<Game>_Data" folder beside the exe
rem  Unreal -> "<Game>-Win64-Shipping.exe" under <Game>\Binaries\Win64\
rem
rem  The launcher in the root (Solarpunk.exe) is usually a thin shim; the
rem  Shipping exe is the real binary and the one worth inspecting.
rem ============================================================
set "ENGINE="
set "DATA="
set "GAME_EXE="

rem Matching "*_Data" alone is not enough: Farming Simulator ships a "web_data"
rem folder, which matched and made this declare a Unity game that then failed
rem for having no Managed\ or il2cpp_data\. Require the actual Unity marker.
for /d %%D in ("%GAME%\*_Data") do (
  if exist "%%~fD\Managed\" set "DATA=%%~fD"
  if exist "%%~fD\il2cpp_data\" set "DATA=%%~fD"
)

rem Some repacks nest the game one level down and put a loader at the top --
rem gbe's ColdClientLoader does exactly this ("Vampire Survivors.exe" beside a
rem "Vampire Survivors\" folder holding the real game). Only checking the top
rem level makes patch.bat report "could not identify the engine" for a perfectly
rem ordinary Unity game, so fall back to a recursive search.
if not defined DATA (
  for /d /r "%GAME%" %%D in (*_Data) do if not defined DATA (
    if exist "%%~fD\Managed\" set "DATA=%%~fD"
    if exist "%%~fD\il2cpp_data\" set "DATA=%%~fD"
  )
)
if defined DATA set "ENGINE=Unity"

if not defined ENGINE (
  for /r "%GAME%" %%F in (*-Win64-Shipping.exe) do (
    set "CAND=%%~fF"
    set "SKIP="
    rem CrashReportClient ships with every UE title and is never the game.
    echo !CAND! | findstr /i /c:"CrashReport" >nul && set "SKIP=1"
    if not defined SKIP if not defined GAME_EXE set "GAME_EXE=!CAND!"
  )
  if defined GAME_EXE set "ENGINE=Unreal"
)

rem Plenty of games are neither Unity nor Unreal -- Farming Simulator has its
rem own engine. There is still exactly one thing that matters: where
rem steam_api64.dll lives and which exe loads it, so fall back to that rather
rem than refusing to help.
if not defined ENGINE (
  for /r "%GAME%" %%F in (steam_api64.dll) do if exist "%%~fF" set "ENGINE=Generic"
)

if not defined ENGINE (
  echo [ERROR] Could not identify the game.
  echo   Looked for a "<Game>_Data" folder ^(Unity^), a "*-Win64-Shipping.exe"
  echo   ^(Unreal^) and any steam_api64.dll, and found none of them.
  goto :end
)

rem A competing emulator's loader will fight ours: gbe's ColdClientLoader
rem injects its own steamclient64.dll, so the game talks to that instead of the
rem real Steam client UCOnline2 needs.
if exist "%GAME%\steamclient64.ini" (
  echo.
  echo [WARN] This folder has a gbe/ColdClientLoader setup ^(steamclient64.ini^).
  echo        UCOnline2 is a passthrough and needs the REAL Steam client, but
  echo        that loader injects its own steamclient64.dll instead. Launch the
  echo        game's own exe directly rather than the loader in this folder.
  echo.
)

rem ============================================================
rem  Find where steam_api64.dll belongs
rem
rem  The authoritative answer is wherever the game ALREADY keeps one -- that
rem  path is engine-, version- and packager-specific and not worth guessing.
rem  Unreal buries it under Engine\Binaries\ThirdParty\Steamworks\Steamv1XX\
rem  Win64\, Unity uses <Game>_Data\Plugins\x86_64\, and repacks move it.
rem  Only fall back to a convention when the game ships no copy at all.
rem ============================================================
rem The "if exist" is load-bearing, not defensive. `for /r` with a plain
rem filename (no wildcard) does NOT test for the file -- it yields that name
rem joined to EVERY directory it walks, so the first result is always the
rem search root. Without the guard this picked the game folder itself and
rem installed the emulator where nothing loads it.
rem Some games ship steam_api64.dll more than once -- Farming Simulator has an
rem identical copy at the root and in x64\, and only the one beside the real
rem binary is loaded. Taking the first hit picked the root and patched a file
rem nothing reads, so choose the copy sitting next to the BIGGEST executable:
rem the launcher and the dedicated server are a fraction of the game's size.
set "STEAM_DIR="
set "STEAM_BEST=0"
for /r "%GAME%" %%F in (steam_api64.dll) do (
  if exist "%%~fF" (
    set "CAND_DIR=%%~dpF"
    set "CAND_MAX=0"
    for %%E in ("%%~dpF*.exe") do (
      if %%~zE GTR !CAND_MAX! set "CAND_MAX=%%~zE"
    )
    if !CAND_MAX! GTR !STEAM_BEST! (
      set "STEAM_BEST=!CAND_MAX!"
      set "STEAM_DIR=!CAND_DIR!"
    )
    if not defined STEAM_DIR set "STEAM_DIR=!CAND_DIR!"
  )
)

rem A 32-bit game ships steam_api.dll and no steam_api64.dll. Installing our
rem x64 build there would be silently wrong -- the game would fail to load it
rem and the cause would not be obvious. Detect that case and refuse, rather
rem than "helpfully" writing the wrong architecture.
set "IS_32BIT="
if not defined STEAM_DIR (
  for /r "%GAME%" %%F in (steam_api.dll) do if exist "%%~fF" set "IS_32BIT=1"
)
if defined IS_32BIT (
  echo [WARN] This game is 32-bit ^(ships steam_api.dll, not steam_api64.dll^).
  echo        UCOnline2 is built x64 here, so the emulator will NOT be installed.
  echo        Build the Win32 configuration and copy steam_api.dll yourself.
  set "EMU_DLL="
)

if not defined STEAM_DIR if not defined IS_32BIT (
  if "%ENGINE%"=="Unity"  set "STEAM_DIR=%DATA%\Plugins\x86_64\"
  if "%ENGINE%"=="Unreal" for %%F in ("%GAME_EXE%") do set "STEAM_DIR=%%~dpF"
  echo [WARN] Game ships no steam_api64.dll; falling back to the usual place.
)
rem SteamStub detection has to inspect the executable that actually starts,
rem not a launcher elsewhere in the tree. Unreal already supplied GAME_EXE.
rem For Unity, prefer the exe matching <name>_Data; for generic engines (and
rem unusual Unity layouts), use the biggest exe beside the selected Steam DLL.
if not defined GAME_EXE if "%ENGINE%"=="Unity" (
  for %%D in ("%DATA%") do (
    set "DATA_STEM=%%~nD"
    set "DATA_STEM=!DATA_STEM:~0,-5!"
    if exist "%%~dpD!DATA_STEM!.exe" set "GAME_EXE=%%~dpD!DATA_STEM!.exe"
  )
)
if not defined GAME_EXE if defined STEAM_DIR (
  set "GAME_EXE_SIZE=0"
  for %%E in ("%STEAM_DIR%*.exe") do if exist "%%~fE" (
    if %%~zE GTR !GAME_EXE_SIZE! (
      set "GAME_EXE_SIZE=%%~zE"
      set "GAME_EXE=%%~fE"
    )
  )
)

set "GET_STUBBED=false"
set "STUB_RESULT=3"
if defined GAME_EXE (
  call :detect_steamstub
) else (
  echo [WARN] Could not identify the running executable; SteamStub was not checked.
)

rem ============================================================
rem  Neutralize a competing Steam emulator that ships its own loader
rem
rem  Repacks (SKIDROW, etc.) often bundle SteamFix or OnlineFix: a tiny
rem  winmm.dll (or version.dll / dxgi.dll) proxy the exe loads on startup,
rem  which in turn loads *Fix64.dll and installs its OWN Steam emulation.
rem  Drop UCOnline2's steam_api64.dll in beside that and two emulators fight
rem  over the interfaces -- the game behaves as if the backend is broken
rem  (looks exactly like a "missing export"). Move the competing loader into
rem  a reversible quarantine beside the game folder so only UCOnline2 is live.
rem ============================================================
set "FIX_DIR=%GAME%\"
if "%ENGINE%"=="Unity"  for %%F in ("%DATA%") do set "FIX_DIR=%%~dpF"
if "%ENGINE%"=="Generic" if defined STEAM_DIR set "FIX_DIR=%STEAM_DIR%"
if "%ENGINE%"=="Unreal" for %%F in ("%GAME_EXE%") do set "FIX_DIR=%%~dpF"
call :neutralize_competing "%FIX_DIR%"
if defined STEAM_DIR if /i not "%STEAM_DIR%"=="%FIX_DIR%" call :neutralize_competing "%STEAM_DIR%"

rem ============================================================
rem  Where union-crax.ini and plugins\ must live
rem
rem  NOT the game folder -- UCOnline2's ReadConfig builds the ini path from
rem  GetModuleFileName(NULL), i.e. the directory of the RUNNING EXE. For Unity
rem  that is the game root, so the two coincide. For Unreal the running process
rem  is the Shipping exe several folders down, and an ini in the root is simply
rem  never read: the emulator falls back to AppId=480 with no ogAppId and logs
rem  "No usable ogAppId". PluginsFolder is relative to the same place, so
rem  plugins\ belongs here too.
rem ============================================================
rem For Unity the exe sits beside <Game>_Data, which is NOT necessarily the
rem folder that was dropped on this script -- see the nested-layout note above.
set "INI_DIR=%GAME%"
if "%ENGINE%"=="Unity"  for %%F in ("%DATA%") do set "INI_DIR=%%~dpF"
rem Generic: the emulator reads its ini from the running exe's directory, and
rem that is the folder we just picked for steam_api64.dll.
if "%ENGINE%"=="Generic" if defined STEAM_DIR set "INI_DIR=%STEAM_DIR%"
if "%ENGINE%"=="Unreal" for %%F in ("%GAME_EXE%") do set "INI_DIR=%%~dpF"
if "%INI_DIR:~-1%"=="\" set "INI_DIR=%INI_DIR:~0,-1%"

rem ============================================================
rem  Detect backends
rem ============================================================
set "FLAVOR="
set "HAS_VOICE="
set "HAS_EOS="
set "HAS_PLAYFAB="
set "BACKEND="

if "%ENGINE%"=="Unity" (
  if exist "%DATA%\Managed" (
    set "BACKEND=Mono"
    if exist "%DATA%\Managed\PhotonUnityNetworking.dll" set "FLAVOR=Realtime"
    if not defined FLAVOR if exist "%DATA%\Managed\PhotonRealtime.dll" set "FLAVOR=Realtime"
    if exist "%DATA%\Managed\Fusion.Realtime.dll" set "FLAVOR=Fusion"
    if exist "%DATA%\Managed\PhotonVoice.dll" set "HAS_VOICE=1"
    if exist "%DATA%\Managed\PhotonVoice.PUN.dll" set "HAS_VOICE=1"
    if exist "%DATA%\Managed\PlayFabAllSDK.dll" set "HAS_PLAYFAB=1"
    for %%F in ("%DATA%\Managed\PlayFab*.dll") do set "HAS_PLAYFAB=1"
  ) else if exist "%DATA%\il2cpp_data\Metadata\global-metadata.dat" (
    set "BACKEND=IL2CPP"
    set "META=%DATA%\il2cpp_data\Metadata\global-metadata.dat"
    findstr /m /c:"NetworkRunner" "!META!" >nul 2>&1 && set "FLAVOR=Fusion"
    if not defined FLAVOR findstr /m /c:"LoadBalancingClient" "!META!" >nul 2>&1 && set "FLAVOR=Realtime"
    if not defined FLAVOR findstr /m /c:"PhotonNetwork" "!META!" >nul 2>&1 && set "FLAVOR=Realtime"
    findstr /m /c:"PhotonVoice" "!META!" >nul 2>&1 && set "HAS_VOICE=1"
    findstr /m /c:"PlayFabSettings" "!META!" >nul 2>&1 && set "HAS_PLAYFAB=1"
  ) else (
    echo [ERROR] Found neither Managed\ nor il2cpp_data\ under the data folder.
    goto :end
  )
) else (
  set "BACKEND=%ENGINE%"
  rem ANSI module names -- see the DETECTION NOTE at the top.
  findstr /m /c:"OnlineSubsystemEOS" "%GAME_EXE%" >nul 2>&1 && set "HAS_EOS=1"
  findstr /m /c:"OnlineSubsystemPlayFab" "%GAME_EXE%" >nul 2>&1 && set "HAS_PLAYFAB=1"
  findstr /m /c:"PhotonUnityNetworking" "%GAME_EXE%" >nul 2>&1 && set "FLAVOR=Realtime"
)

rem Native SDKs shipped as standalone DLLs beside the exe (No Man's Sky and other
rem games on their own engine): these are neither a Unity Managed assembly nor an
rem Unreal OnlineSubsystem symbol, so every check above misses them. Probe the
rem binaries dir directly. PartyWin / PlayFabMultiplayer are the PlayFab-Party
rem multiplayer tell; Core/Services alone can be just analytics, but flag it either
rem way (the plugin stays idle until you give it a TitleId).
if defined STEAM_DIR (
  if not defined HAS_PLAYFAB if exist "%STEAM_DIR%\PartyWin.dll" set "HAS_PLAYFAB=1"
  if not defined HAS_PLAYFAB for %%F in ("%STEAM_DIR%\PlayFab*.dll") do set "HAS_PLAYFAB=1"
  if not defined HAS_EOS for %%F in ("%STEAM_DIR%\EOSSDK*.dll") do set "HAS_EOS=1"
)

rem coherence: the SDK bakes a schema next to the game. That file is present
rem in every coherence build regardless of engine version, which makes it a
rem better marker than any symbol name.
set "HAS_COHERENCE="
for /r "%GAME%" %%F in (combined.schema) do (
  if exist "%%~fF" set "HAS_COHERENCE=1"
)

rem combined.schema is NOT always on disk. Lost Skies is a coherence game with
rem no such file -- the schema is embedded in the build instead (coherence can
rem carry it in RuntimeSettings.CombinedSchemaText). Detecting only by that file
rem reported "no secondary backend" for a game that plainly uses coherence, so
rem fall back to the SDK's own types: a Mono build ships Coherence.Toolkit.dll,
rem and an IL2CPP build leaves CoherenceBridge in global-metadata.dat.
if not defined HAS_COHERENCE if defined DATA (
  if exist "%DATA%\Managed\Coherence.Toolkit.dll" set "HAS_COHERENCE=1"
  if not defined HAS_COHERENCE if exist "%DATA%\il2cpp_data\Metadata\global-metadata.dat" (
    findstr /m /c:"CoherenceBridge" "%DATA%\il2cpp_data\Metadata\global-metadata.dat" >nul 2>&1 && set "HAS_COHERENCE=1"
  )
)

rem File presence beats any string scan -- if the SDK ships, it is in use.
rem Same `for /r` trap as above: without "if exist" these fire in every
rem directory walked and every game on earth "uses EOS".
for /r "%GAME%" %%F in (EOSSDK-Win64-Shipping.dll) do if exist "%%~fF" set "HAS_EOS=1"
for /r "%GAME%" %%F in (EOSSDK.dll) do if exist "%%~fF" set "HAS_EOS=1"

echo.
echo   Engine:      %ENGINE% / %BACKEND%
if defined DATA echo   Data:        %DATA%
if defined GAME_EXE echo   Executable:  %GAME_EXE%
if defined STEAM_DIR echo   Steam API:   %STEAM_DIR%
echo   Config:      %INI_DIR%
if /i "%GET_STUBBED%"=="true" (
  echo   SteamStub:   detected ^(runtime hook enabled^)
  echo                if the game still shows "Application load error", unpack the exe with Steamless: https://github.com/atom0s/Steamless
) else if "%STUB_RESULT%"=="2" (
  echo   SteamStub:   uncertain ^(unrecognized .bind layout^)
  echo                GetStubbedLol cannot handle this variant - unpack the exe with Steamless: https://github.com/atom0s/Steamless
) else if "%STUB_RESULT%"=="3" (
  echo   SteamStub:   not checked ^(invalid or unreadable PE^)
) else (
  echo   SteamStub:   not detected
)
if defined FLAVOR (
  echo   Photon:      %FLAVOR%
  if defined HAS_VOICE echo   Voice:      detected
) else (
  echo   Photon:      none
)
if defined HAS_EOS       echo   EOS:        detected
if defined HAS_PLAYFAB   echo   PlayFab:    detected
if defined HAS_COHERENCE echo   coherence:  detected
if not defined FLAVOR if not defined HAS_EOS if not defined HAS_PLAYFAB if not defined HAS_COHERENCE (
  echo   Services:    Steam only ^(no plugin needed^)
)
echo.

call :stage "2/4  CONFIGURE"

if defined KEYONLY (
  if not defined HAS_COHERENCE (
    echo [ERROR] /keyonly only applies to coherence games, and this is not one.
    goto :end
  )
  echo   Key-only mode: only the coherence runtime key will change.
  goto :ask_coherence
)

rem ============================================================
rem  Gather config
rem
rem  set /p misbehaves inside a ( ) block, so every prompt is at top level
rem  and reached by goto. Do not "tidy" these into an if-block.
rem ============================================================
set "OGAPPID="
set /p "OGAPPID=Real Steam AppId of the game (the ogAppId): "
if not defined OGAPPID echo [ERROR] AppId is required.& goto :end

set "RTGUID="
set "VOICEGUID="
set "FUSIONGUID="
set "EOS_PRODUCT="
set "EOS_SANDBOX="
set "EOS_DEPLOY="
set "EOS_CLIENT="
set "EOS_SECRET="
set "PF_TITLE="

if not defined FLAVOR goto :ask_eos
if /i "%FLAVOR%"=="Fusion" goto :ask_fusion

:ask_realtime
set /p "RTGUID=Your Photon REALTIME app GUID: "
if not defined RTGUID echo [ERROR] Realtime GUID is required.& goto :end
if defined HAS_VOICE goto :ask_voice_required
set /p "VOICEGUID=Photon VOICE app GUID (optional, press Enter to skip): "
goto :ask_eos

:ask_voice_required
set /p "VOICEGUID=Your Photon VOICE app GUID (required for this game): "
goto :ask_eos

:ask_fusion
set /p "FUSIONGUID=Your Photon FUSION app GUID: "
if not defined FUSIONGUID echo [ERROR] Fusion GUID is required.& goto :end

:ask_eos
if not defined HAS_EOS goto :ask_playfab
echo.
echo   EOS needs credentials from an Epic app you control.
echo   Press Enter to leave these blank and skip the EOS plugin.
set /p "EOS_PRODUCT=  EOS ProductId: "
set /p "EOS_SANDBOX=  EOS SandboxId: "
set /p "EOS_DEPLOY=  EOS DeploymentId: "
set /p "EOS_CLIENT=  EOS ClientId: "
set /p "EOS_SECRET=  EOS ClientSecret: "

:ask_playfab
if not defined HAS_PLAYFAB goto :ask_coherence
echo.
rem When coherence is also present, PlayFab is probably NOT the backend and
rem skipping is the right answer -- keep that prompt short.
if defined HAS_COHERENCE goto :playfab_secondary

rem Otherwise PlayFab IS how this game matchmakes, and the plugin sits idle
rem without a TitleId. Skipping produces an install that launches perfectly and
rem can never find anyone, which reads as "the fix is broken" rather than "a
rem field was left blank" -- so spell out what blank costs, and offer the shared
rem title the same way the coherence prompt below does.
echo   This game brokers multiplayer through PlayFab, so it needs a TitleId.
echo   Leaving it blank still gives you a working game, but NO multiplayer.
echo.
echo   EVERYONE YOU PLAY WITH MUST USE THE SAME TitleId, or you will not see
echo   each other no matter what else is set up correctly.
echo.
if "%OGAPPID%"=="275850" (
  echo   No Man's Sky: type SHARED to use the community title 1D861F, which is
  echo   published for exactly this. Details: plugins\playfab_universal\README.md
) else (
  echo   Type SHARED for the community title 1D861F, enter your own title, or
  echo   leave blank. Details: plugins\playfab_universal\README.md
)
echo.
set /p "PF_TITLE=  PlayFab TitleId (SHARED / your own / blank): "
if /i "!PF_TITLE!"=="SHARED" set "PF_TITLE=1D861F"
if defined PF_TITLE (
  echo   Using PlayFab TitleId !PF_TITLE! -- make sure everyone matches it.
) else (
  echo   [WARN] No TitleId set. PlayFab multiplayer will NOT work until you fill
  echo          in TitleId under [PlayFab] in union-crax.ini.
)
goto :ask_coherence

:playfab_secondary
echo   coherence is also present and is probably the multiplayer backend.
echo   Leave PlayFab empty unless testing proves it is required.
echo.
set /p "PF_TITLE=  PlayFab TitleId (press Enter to skip): "
if /i "!PF_TITLE!"=="SHARED" set "PF_TITLE=1D861F"

:ask_coherence
if not defined HAS_COHERENCE goto :write_ini
echo.
echo   coherence needs a matching project and schema.
echo   Enter your runtime key, type SHARED for the community project, or leave
echo   it blank to configure later. Guide: plugins\coherence_universal\README.md
echo.

rem Offer the upload BEFORE asking for a key: you cannot give a runtime key for
rem a project you have not set up yet, and a key without a matching schema gets
rem you a SchemaNotFound that looks like the key being wrong.
call :find_schema_tool
if defined SCHEMA_TOOL (
  echo   The schema upload tool is available for your own project.
  set /p "COH_UPLOAD=  Run the schema upload tool first? (y/N): "
  if /i "!COH_UPLOAD!"=="y" call :run_schema_tool
  echo.
)
set /p "COH_KEY=  coherence runtime key, or SHARED (press Enter to skip): "

rem The shared project's RUNTIME key. Safe to publish: a runtime key is a
rem client-side identifier that ships inside every coherence game by design,
rem like a Photon AppId. Portal/service tokens are a different thing entirely
rem and must never appear here.
set "COH_SHARED="
if /i "%COH_KEY%"=="shared" (
  set "COH_KEY=fce1ea692a854b50b9f945ef6aa17758"
  set "COH_SHARED=1"
  echo.
  echo   [OK] Using the shared coherence project ^(availability is not guaranteed^).
)

rem Only relevant to someone bringing their OWN project. Printing it after a
rem SHARED answer tells people they need Unity immediately after they chose the
rem route that does not, which reads as a warning that something is missing.
if not defined COH_SHARED if defined COH_KEY (
  echo.
  echo   [INFO] Your project needs the matching schema. The helper is in
  echo          tools\coherence_schema; without it the game reports SchemaNotFound.
)

rem ============================================================
rem  Write union-crax.ini  (sequential appends -- robust)
rem ============================================================
:write_ini
if defined KEYONLY goto :deploy_plugins
call :stage "3/4  INSTALL"
set "INI=%INI_DIR%\union-crax.ini"
> "%INI%" echo [Settings]
>> "%INI%" echo AppId=480
>> "%INI%" echo ogAppId=%OGAPPID%
>> "%INI%" echo PluginsFolder=plugins
>> "%INI%" echo GetStubbedLol=%GET_STUBBED%
>> "%INI%" echo LogOverlay=no

rem [VersionProxy] -- when we deploy any plugin, tell the early-load proxy
rem (version.dll for Unity / winmm.dll for Unreal) to preload steam_api64
rem BEFORE the game runs, so the plugin's hooks install before the game inits
rem its backend. Unity P/Invokes steam_api64 lazily on its first Steam call,
rem which can otherwise land AFTER EOS/PlayFab is already up (too late to hook).
set "HAS_PLUGIN="
if defined FLAVOR set "HAS_PLUGIN=1"
if defined HAS_EOS set "HAS_PLUGIN=1"
if defined HAS_PLAYFAB set "HAS_PLUGIN=1"
if defined HAS_COHERENCE set "HAS_PLUGIN=1"
if defined HAS_PLUGIN (
  >> "%INI%" echo(
  >> "%INI%" echo [VersionProxy]
  >> "%INI%" echo LoadDLLsEarly=true
)

rem ============================================================
rem  [DLC]
rem
rem  Games ask about DLC two different ways and both have to be answered:
rem
rem    "do I own 211?"  -> BIsSubscribedApp / BIsDlcInstalled
rem    "list my DLC"    -> GetDLCCount + BGetDLCDataByIndex
rem
rem  UnlockAll=true answers the first for ANY id without knowing them, so it is
rem  always written -- nobody should have to go hunting for a DLC list just to
rem  get their content working.
rem
rem  It does NOT answer the second: GetDLCCount falls back to Steam's count
rem  (zero, for a spoofed app) when we have no entries, so a game that builds
rem  its DLC menu by enumerating still shows nothing. That needs real ids, so
rem  harvest them where the game folder already tells us what they are.
rem ============================================================
>> "%INI%" echo(
>> "%INI%" echo [DLC]
>> "%INI%" echo ; UnlockAll answers any "do I own this DLC?" check, for any id,
>> "%INI%" echo ; so DLC works without knowing what the ids are.
>> "%INI%" echo ;
>> "%INI%" echo ; The "appid=name" lines below are a separate thing: they are what
>> "%INI%" echo ; a game reads when it ENUMERATES its DLC to build a menu.
>> "%INI%" echo ; Both work together -- the list is checked first and UnlockAll is
>> "%INI%" echo ; only the fallback, so you do NOT have to turn it off to use a list.
>> "%INI%" echo ;
>> "%INI%" echo ; Set UnlockAll=false when you want ONLY the ids listed here to
>> "%INI%" echo ; count -- for instance if a game misbehaves once it is told it
>> "%INI%" echo ; owns everything.
>> "%INI%" echo UnlockAll=true

rem Source 1: a gbe/Goldberg configs.app.ini already lists "<appid>=<name>"
rem under [app::dlcs] -- exactly the format we want, names included.
set "DLC_SRC="
for /r "%GAME%" %%F in (configs.app.ini) do (
  if exist "%%~fF" if not defined DLC_SRC set "DLC_SRC=%%~fF"
)
set "DLC_FOUND="
if defined DLC_SRC (
  for /f "usebackq tokens=1,* delims==" %%A in (`findstr /r /c:"^[0-9][0-9]*=" "!DLC_SRC!"`) do (
    >> "%INI%" echo %%A=%%B
    set "DLC_FOUND=1"
  )
  if defined DLC_FOUND echo [OK] DLC ids taken from !DLC_SRC!
)

rem Source 2: many repacks drop one folder per DLC named for its AppId.
if not defined DLC_FOUND (
  for /d %%D in ("%INI_DIR%\*") do (
    echo %%~nxD| findstr /r /c:"^[0-9][0-9]*$" >nul && (
      >> "%INI%" echo %%~nxD=DLC %%~nxD
      set "DLC_FOUND=1"
    )
  )
  if defined DLC_FOUND echo [OK] DLC ids taken from the AppId-named folders in the game directory.
)

if not defined DLC_FOUND (
  echo [INFO] No DLC list found; UnlockAll still handles ownership checks.
)
echo [OK] DLC ownership enabled ^(add appid=name only if a DLC menu is empty^).

if not defined FLAVOR goto :ini_eos
>> "%INI%" echo(
if /i "%FLAVOR%"=="Fusion" (
  >> "%INI%" echo [Fusion]
  >> "%INI%" echo PhotonAppIdFusion=%FUSIONGUID%
  >> "%INI%" echo ForcedAuthType=0
) else (
  >> "%INI%" echo [Realtime]
  >> "%INI%" echo PhotonAppIdRealtime=%RTGUID%
  if defined VOICEGUID >> "%INI%" echo PhotonAppIdVoice=%VOICEGUID%
  >> "%INI%" echo ForcedAuthType=0
)

:ini_eos
if not defined HAS_EOS goto :ini_coherence
>> "%INI%" echo(
>> "%INI%" echo [EOS]
if defined EOS_PRODUCT (>> "%INI%" echo ProductId=%EOS_PRODUCT%) else (>> "%INI%" echo ProductId=)
if defined EOS_SANDBOX (>> "%INI%" echo SandboxId=%EOS_SANDBOX%) else (>> "%INI%" echo SandboxId=)
if defined EOS_DEPLOY  (>> "%INI%" echo DeploymentId=%EOS_DEPLOY%) else (>> "%INI%" echo DeploymentId=)
if defined EOS_CLIENT  (>> "%INI%" echo ClientId=%EOS_CLIENT%) else (>> "%INI%" echo ClientId=)
if defined EOS_SECRET  (>> "%INI%" echo ClientSecret=%EOS_SECRET%) else (>> "%INI%" echo ClientSecret=)
>> "%INI%" echo DisplayName=Player

:ini_coherence
if not defined HAS_COHERENCE goto :ini_playfab
>> "%INI%" echo(
>> "%INI%" echo [Coherence]
>> "%INI%" echo ForceGuestLogin=true
if defined COH_KEY (>> "%INI%" echo RuntimeKey=%COH_KEY%) else (>> "%INI%" echo RuntimeKey=)
>> "%INI%" echo LocalMode=false

:ini_playfab
if not defined HAS_PLAYFAB goto :wrote_ini
>> "%INI%" echo(
>> "%INI%" echo [PlayFab]
if defined PF_TITLE (>> "%INI%" echo TitleId=%PF_TITLE%) else (>> "%INI%" echo TitleId=)

:wrote_ini
echo.
echo [OK] Wrote %INI%

rem ============================================================
rem  Install the emulator, backing up whatever is there
rem ============================================================
if defined KEYONLY goto :deploy_plugins
if not defined EMU_DLL goto :deploy_plugins
if not exist "%STEAM_DIR%" mkdir "%STEAM_DIR%" 2>nul

rem Replace EVERY copy of the Steam API DLL in the game folder. A Unity game can
rem ship one beside the exe AND one under Data\Plugins\x86_64; the engine loads
rem the Data\Plugins copy, so patching only one leaves the game on the real DLL.
rem Never overwrite an existing .bak -- on a second run that would replace the
rem pristine original with our own DLL and lose it for good.
for %%F in ("%EMU_DLL%") do set "API_NAME=%%~nxF"
set "API_FOUND="
for /r "%GAME%" %%D in (%API_NAME%) do (
  set "API_FOUND=1"
  if exist "%%~fD.bak" (
    echo [SKIP] Backup exists for "%%~fD" -- replacing DLL only.
  ) else (
    copy /y "%%~fD" "%%~fD.bak" >nul
    echo [OK] Backed up "%%~fD"
  )
  copy /y "%EMU_DLL%" "%%~fD" >nul
  if errorlevel 1 (
    echo [ERROR] Failed to install "%%~fD" -- is the game running?
  ) else (
    echo [OK] Installed UCOnline2 -^> "%%~fD"
  )
)

if not defined API_FOUND (
  rem Nothing to replace -- fresh install into the detected primary location.
  copy /y "%EMU_DLL%" "%STEAM_DIR%%API_NAME%" >nul
  if errorlevel 1 (
    echo [ERROR] Failed to install %API_NAME% to %STEAM_DIR%
  ) else (
    echo [OK] Installed UCOnline2 %API_NAME% to %STEAM_DIR%
  )
)

call :deploy_overlay_proxy

rem ============================================================
rem  Deploy plugins
rem ============================================================
:deploy_plugins
set "NEEDDIR="
if defined KEYONLY goto :skip_other_deploys
if defined FLAVOR set "NEEDDIR=1"
if defined HAS_COHERENCE set "NEEDDIR=1"
if defined HAS_EOS set "NEEDDIR=1"
if defined HAS_PLAYFAB set "NEEDDIR=1"
if defined NEEDDIR if not exist "%INI_DIR%\plugins\" mkdir "%INI_DIR%\plugins"

if defined FLAVOR (
  call :deploy photon_universal
)
rem Only deploy EOS_custom once it has credentials to use. Without your own
rem Epic app it cannot redirect anything, and dropping an inert plugin in
rem front of a game whose co-op might be plain Steam P2P just adds a variable
rem to the first test. Detected-but-not-configured is reported instead.
if defined HAS_EOS (
  if defined EOS_PRODUCT (
    call :deploy EOS_custom
  ) else (
    echo [SKIP] EOS detected but no Epic app given -- EOS_custom NOT deployed.
    echo        Test the game bare first; if co-op fails on EOS, create a free
    echo        app at dev.epicgames.com, fill in [EOS], and re-run this script.
  )
)
:skip_other_deploys
if defined KEYONLY goto :coherence_deploy
if defined HAS_PLAYFAB (
  call :deploy playfab_universal
)

:coherence_deploy
if defined HAS_COHERENCE (
  if not defined KEYONLY call :deploy coherence_universal
  if defined COH_KEY call :patch_runtime_key
)

rem ============================================================
rem  Summary
rem ============================================================
call :stage "4/4  RESULT"
if defined KEYONLY (
  echo   Done: only the coherence runtime key was processed.
  goto :summary_end
)
if not defined FLAVOR if not defined HAS_EOS if not defined HAS_PLAYFAB (
  echo   Plugins:    none needed; test the game with Steam passthrough.
)
if defined FLAVOR (
  echo   Photon:     %FLAVOR% plugin installed.
  if defined HAS_VOICE echo   Voice:      a Photon Voice app is required.
)
if defined HAS_EOS (
  if defined EOS_PRODUCT (
    echo   EOS:        plugin installed.
  ) else (
    echo   EOS:        skipped ^(no Epic credentials^).
  )
)
if defined HAS_PLAYFAB (
  if defined HAS_COHERENCE (
    echo   PlayFab:    installed; idle while TitleId is empty.
  ) else (
    echo   PlayFab:    installed ^(set TitleId if still blank^).
  )
)
if defined HAS_COHERENCE (
  if defined COH_KEY (
    echo   coherence:  configured; use one enabled region.
  ) else (
    echo   coherence:  runtime key still required.
  )
)
if /i "%GET_STUBBED%"=="true" (
  echo   SteamStub:   runtime hook enabled.
)
echo.
:summary_end
echo   Config: %INI_DIR%\union-crax.ini
echo   Log:    %%TEMP%%\uc_online2.log
echo ============================================================
goto :end

rem ------------------------------------------------------------
rem  :find_schema_tool  -> SCHEMA_TOOL
rem
rem  Ships beside patch.bat in a release zip and lives at the same relative
rem  path in a repo checkout, so one lookup covers both.
rem ------------------------------------------------------------
:find_schema_tool
set "SCHEMA_TOOL="
if exist "%SCRIPTDIR%tools\coherence_schema\Invoke-CoherenceSchemaPipeline.ps1" (
  set "SCHEMA_TOOL=%SCRIPTDIR%tools\coherence_schema\Invoke-CoherenceSchemaPipeline.ps1"
)
goto :eof

rem ------------------------------------------------------------
rem  :run_schema_tool
rem
rem  The game's combined.schema is already known from the detection above, so
rem  pass it through rather than making the user find it. Everything else the
rem  tool prompts for itself.
rem ------------------------------------------------------------
:run_schema_tool
set "COMBINED="
for /r "%GAME%" %%F in (combined.schema) do (
  if exist "%%~fF" if not defined COMBINED set "COMBINED=%%~fF"
)
echo.
echo   Launching the schema upload tool. It will ask for your Unity project and
echo   your coherence project id and token; the game's schema is filled in.
echo.
if defined COMBINED (
  powershell -NoProfile -ExecutionPolicy Bypass -File "%SCHEMA_TOOL%" -CombinedSchemaPath "%COMBINED%"
) else (
  powershell -NoProfile -ExecutionPolicy Bypass -File "%SCHEMA_TOOL%"
)
if errorlevel 1 (
  echo.
  echo   [WARN] The schema upload did not complete. You can re-run it later:
  echo          tools\coherence_schema\Run-CoherenceSchemaPipeline.bat
  echo          Without a matching schema the game fails with SchemaNotFound.
)
goto :eof

rem ------------------------------------------------------------
rem  :patch_runtime_key
rem
rem  Points the game at YOUR coherence project by replacing the publisher's
rem  runtime key in the game data.
rem
rem  This CANNOT be done at runtime. coherence copies the key during its own
rem  init, about a second before UCOnline2 loads plugins (plugins load at
rem  SteamAPI_Init, after the engine has booted). Patching the settings field,
rem  and even patching the string's characters in place, both lose that race.
rem
rem  Runtime keys are 32 hex characters, so this is a same-length replace and
rem  no offset in the asset file moves. Batch cannot edit binary files, so the
rem  work goes to a small PowerShell helper written to %TEMP%.
rem ------------------------------------------------------------
:patch_runtime_key
set "COH_ASSETS="
if defined DATA if exist "%DATA%\globalgamemanagers.assets" set "COH_ASSETS=%DATA%\globalgamemanagers.assets"
if not defined COH_ASSETS (
  echo [SKIP] globalgamemanagers.assets not found -- patch the runtime key by hand.
  goto :eof
)

set "PS1=%TEMP%\uco2_cohkey.ps1"
> "%PS1%" echo param([string]$File,[string]$NewKey)
>>"%PS1%" echo $bytes = [IO.File]::ReadAllBytes($File)
>>"%PS1%" echo $text  = [Text.Encoding]::GetEncoding(28591).GetString($bytes)
>>"%PS1%" echo # Locating the key is the whole risk here: a bare 32-hex search matches
>>"%PS1%" echo # thousands of false positives, because binary data reads as hex-ish
>>"%PS1%" echo # ASCII. Anchoring on the first 40-char hex is not enough either -- an
>>"%PS1%" echo # unrelated one appears earlier in the file, and following it patched 32
>>"%PS1%" echo # bytes of unrelated data during testing.
>>"%PS1%" echo #
>>"%PS1%" echo # The serialized coherence RuntimeSettings lays out schemaID (40 hex),
>>"%PS1%" echo # then runtimeKey (32 hex), then localHost. So require ALL THREE in one
>>"%PS1%" echo # window, and reject a candidate with almost no character variety --
>>"%PS1%" echo # real keys are not runs of 0s and 1s.
>>"%PS1%" echo $found = $false
>>"%PS1%" echo foreach ($a in [regex]::Matches($text, '\b[0-9a-f]{40}\b')) {
>>"%PS1%" echo   $win = $text.Substring($a.Index, [Math]::Min(400, $text.Length - $a.Index))
>>"%PS1%" echo   if ($win -notmatch 'localhost') { continue }
>>"%PS1%" echo   foreach ($c in [regex]::Matches($win, '\b[0-9a-f]{32}\b')) {
>>"%PS1%" echo     if ((($c.Value.ToCharArray() ^| Sort-Object -Unique).Count) -lt 8) { continue }
>>"%PS1%" echo     $old = $c.Value; $idx = $a.Index + $c.Index; $found = $true; break
>>"%PS1%" echo   }
>>"%PS1%" echo   if ($found) { break }
>>"%PS1%" echo }
>>"%PS1%" echo if (-not $found) { Write-Host '  [SKIP] could not locate the runtime key; patch by hand - see plugins\coherence_universal\README.md'; exit 1 }
>>"%PS1%" echo if ($old -eq $NewKey) { Write-Host '  [OK] already points at your project'; exit 0 }
>>"%PS1%" echo if ($old.Length -ne $NewKey.Length) { Write-Host '  [ERROR] key length differs, refusing'; exit 1 }
>>"%PS1%" echo $bak = $File + '.uco2.bak'
>>"%PS1%" echo if (-not (Test-Path $bak)) { Copy-Item $File $bak; Write-Host ('  [OK] backed up -^> ' + $bak) }
>>"%PS1%" echo $newB = [Text.Encoding]::ASCII.GetBytes($NewKey)
>>"%PS1%" echo [Array]::Copy($newB, 0, $bytes, $idx, $newB.Length)
>>"%PS1%" echo [IO.File]::WriteAllBytes($File, $bytes)
>>"%PS1%" echo Write-Host ('  [OK] runtime key ' + $old + ' -^> ' + $NewKey)
powershell -NoProfile -ExecutionPolicy Bypass -File "%PS1%" "%COH_ASSETS%" "%COH_KEY%"
del "%PS1%" >nul 2>&1
goto :eof

rem ------------------------------------------------------------
rem  :deploy <plugin base name>
rem  Finds the DLL in any layout this script ships in and copies it.
rem ------------------------------------------------------------
:deploy
set "P=%~1"
set "SRC="
if exist "%SCRIPTDIR%plugins\%P%\relbuild\x64\%P%.dll" set "SRC=%SCRIPTDIR%plugins\%P%\relbuild\x64\%P%.dll"
if not defined SRC if exist "%SCRIPTDIR%plugins\%P%.dll" set "SRC=%SCRIPTDIR%plugins\%P%.dll"
if not defined SRC if exist "%SCRIPTDIR%%P%.dll" set "SRC=%SCRIPTDIR%%P%.dll"
if not defined SRC (
  echo [WARN] %P%.dll not found -- build it or drop it beside patch.bat.
  goto :eof
)
copy /y "%SRC%" "%INI_DIR%\plugins\%P%.dll" >nul
if errorlevel 1 (
  echo [ERROR] Failed to copy %P%.dll
) else (
  echo [OK] Copied %P%.dll to %INI_DIR%\plugins\
)
goto :eof

rem ------------------------------------------------------------
rem  :deploy_overlay_proxy
rem
rem  Deploys the same renameable early-loader under the DLL name imported by
rem  the detected engine. This gets GameOverlayRenderer64 into the process
rem  before Unity or Unreal creates its graphics swapchain.
rem ------------------------------------------------------------
:deploy_overlay_proxy
if not defined OVERLAY_PROXY (
  echo [SKIP] Early overlay proxy not included in this build.
  goto :eof
)

rem Phasmophobia inventories unexpected files beside its executable during
rem startup and aborts after reporting an unknown version.dll. UCOnline2 still
rem loads normally through the game's steam_api64.dll, so only the optional
rem early overlay proxy must be skipped for this title.
if /i "%ENGINE%"=="Unity" if defined GAME_EXE for %%F in ("%GAME_EXE%") do (
  if /i "%%~nxF"=="Phasmophobia.exe" (
    echo [SKIP] Phasmophobia rejects an extra version.dll; overlay proxy skipped.
    goto :eof
  )
)

set "OVERLAY_TARGET="
set "OVERLAY_NAME="
if /i "%ENGINE%"=="Unity" (
  set "OVERLAY_NAME=version.dll"
  set "OVERLAY_TARGET=%INI_DIR%\version.dll"
)
if /i "%ENGINE%"=="Unreal" (
  set "OVERLAY_NAME=winmm.dll"
  for %%F in ("%GAME_EXE%") do set "OVERLAY_TARGET=%%~dpFwinmm.dll"
)
rem Older UCOnline2 deployed the Unreal early-loader as XINPUT1_3.dll, which UE5
rem loads AFTER its D3D12 renderer -- too late to arm the SteamStub bypass. winmm
rem is statically imported and loads before OEP. Retire any XINPUT1_3.dll our old
rem installs left behind so we never run two proxies at once.
if /i "%ENGINE%"=="Unreal" call :retire_stale_xinput
if not defined OVERLAY_TARGET (
  echo [SKIP] No early overlay proxy rule for %ENGINE% games.
  goto :eof
)

if exist "%OVERLAY_TARGET%" (
  fc /b "%OVERLAY_PROXY%" "%OVERLAY_TARGET%" >nul 2>&1
  if not errorlevel 1 (
    echo [OK] Overlay proxy already installed as %OVERLAY_NAME%.
    goto :eof
  )
  if exist "%OVERLAY_TARGET%.uco2.bak" (
    echo [SKIP] Existing %OVERLAY_NAME% differs and its backup already exists.
    echo        Refusing to replace it again: %OVERLAY_TARGET%
    goto :eof
  )
  copy /y "%OVERLAY_TARGET%" "%OVERLAY_TARGET%.uco2.bak" >nul
  if errorlevel 1 (
    echo [ERROR] Could not back up existing %OVERLAY_NAME%; overlay proxy skipped.
    goto :eof
  )
  echo [OK] Backed up existing %OVERLAY_NAME% to %OVERLAY_NAME%.uco2.bak
)

copy /y "%OVERLAY_PROXY%" "%OVERLAY_TARGET%" >nul
if errorlevel 1 (
  echo [ERROR] Failed to install overlay proxy as %OVERLAY_NAME% -- is the game running?
) else (
  echo [OK] Installed early overlay proxy as %OVERLAY_NAME%
  echo      %OVERLAY_TARGET%
)
goto :eof

rem ------------------------------------------------------------
rem  :retire_stale_xinput
rem
rem  A previous UCOnline2 deployed the Unreal early-loader as XINPUT1_3.dll. Now
rem  that Unreal uses winmm.dll, remove that stale copy IF it is ours (byte-equal
rem  to the current overlay proxy). If we had backed up the game's own
rem  XINPUT1_3.dll, restore it; otherwise just delete ours. Never touch an
rem  XINPUT1_3.dll that isn't ours.
rem ------------------------------------------------------------
:retire_stale_xinput
if not defined GAME_EXE goto :eof
if not defined OVERLAY_PROXY goto :eof
for %%F in ("%GAME_EXE%") do set "STALE_XI=%%~dpFXINPUT1_3.dll"
if not exist "%STALE_XI%" goto :eof
fc /b "%OVERLAY_PROXY%" "%STALE_XI%" >nul 2>&1
if errorlevel 1 goto :eof
if exist "%STALE_XI%.uco2.bak" (
  copy /y "%STALE_XI%.uco2.bak" "%STALE_XI%" >nul
  if not errorlevel 1 (
    del /q "%STALE_XI%.uco2.bak" >nul
    echo [OK] Restored original XINPUT1_3.dll ^(superseded by winmm.dll^).
  )
) else (
  del /q "%STALE_XI%" >nul
  echo [OK] Removed stale UCOnline2 XINPUT1_3.dll ^(superseded by winmm.dll^).
)
goto :eof

rem ------------------------------------------------------------
rem  :neutralize_competing <directory>
rem
rem  OnlineFix/SteamFix loaders often live beside the running executable while
rem  steam_api64.dll is nested under a Unity data folder. Check both locations.
rem ------------------------------------------------------------
:neutralize_competing
set "N_DIR=%~1"
if not defined N_DIR goto :eof
if not "%N_DIR:~-1%"=="\" set "N_DIR=%N_DIR%\"
set "COMPET="
if exist "%N_DIR%SteamFix64.dll"  set "COMPET=SteamFix"
if exist "%N_DIR%OnlineFix64.dll" set "COMPET=OnlineFix"
rem Migrate leftovers made by patch.bat versions that renamed files in place.
if exist "%N_DIR%SteamFix64.dll.uco-disabled"  set "COMPET=SteamFix"
if exist "%N_DIR%OnlineFix64.dll.uco-disabled" set "COMPET=OnlineFix"
if not defined COMPET goto :eof

echo.
echo [WARN] Competing emulator found: %COMPET%
echo        Folder: %N_DIR%
echo        Moving it outside the game tree:
echo        %QUARANTINE_DIR%

rem A named proxy is unambiguous and means generic proxies in the same folder
rem can be left alone. This protects UCOnline2's own version.dll overlay shim.
set "NAMED_FIX_PROXY="
if exist "%N_DIR%winmm.dll"              set "NAMED_FIX_PROXY=1"
if exist "%N_DIR%winmm.dll.uco-disabled" set "NAMED_FIX_PROXY=1"
for %%P in (winmm.dll winmm.txt winmm.ini SteamFix64.dll SteamFix.ini OnlineFix64.dll OnlineFix.ini dlllist.txt) do (
  call :quarantine_file "%N_DIR%" "%%P"
  call :quarantine_file "%N_DIR%" "%%P.uco-disabled"
)

rem OFME's Launcher.exe can load OnlineFix64.dll directly without winmm. Its
rem adjacent OnlineFix.json is the identifying marker, so a publisher launcher
rem with the same generic filename is never disabled by name alone.
set "OFME_MARKER="
if exist "%N_DIR%OnlineFix.json"              set "OFME_MARKER=1"
if exist "%N_DIR%OnlineFix.json.uco-disabled" set "OFME_MARKER=1"
if /i "%COMPET%"=="OnlineFix" if defined OFME_MARKER (
  for %%P in (Launcher.exe OnlineFix.json OnlineFix.url PhotonBridge.dll) do (
    call :quarantine_file "%N_DIR%" "%%P"
    call :quarantine_file "%N_DIR%" "%%P.uco-disabled"
  )
)

rem Defender commonly blocks moving the payload itself. Once every loader
rem above is disabled the remaining DLL is inert, but say so explicitly.
if exist "%N_DIR%OnlineFix64.dll" (
  echo [INFO] OnlineFix64.dll remains ^(Windows may block touching it^), but its
  echo        winmm/dlllist/Launcher load paths have been quarantined.
)

rem Some fixes use only a generic proxy name. Restrict this fallback to small
rem DLLs and only use it when no named winmm loader identified the chain.
if not defined NAMED_FIX_PROXY for %%P in (version.dll dxgi.dll dsound.dll winhttp.dll) do (
  if exist "%N_DIR%%%P" (
    for %%Z in ("%N_DIR%%%P") do if %%~zZ LSS 307200 (
      call :quarantine_file "%N_DIR%" "%%P"
    )
  )
  call :quarantine_file "%N_DIR%" "%%P.uco-disabled"
)
goto :eof

rem ------------------------------------------------------------
rem  :quarantine_file <source directory> <filename>
rem
rem  Moves one known competing-loader file outside the game tree. The source
rem  directory is encoded below the quarantine root so files from different
rem  nested plugin folders do not collide. Existing backups are never replaced.
rem ------------------------------------------------------------
:quarantine_file
set "Q_SOURCE_DIR=%~1"
set "Q_NAME=%~2"
if not defined Q_SOURCE_DIR goto :eof
if not defined Q_NAME goto :eof
if not "%Q_SOURCE_DIR:~-1%"=="\" set "Q_SOURCE_DIR=%Q_SOURCE_DIR%\"
if not exist "%Q_SOURCE_DIR%%Q_NAME%" goto :eof

set "Q_REL=!Q_SOURCE_DIR:%GAME%\=!"
if /i "!Q_REL!"=="!Q_SOURCE_DIR!" (
  for %%D in ("!Q_SOURCE_DIR:~0,-1!") do set "Q_REL=external__%%~nxD"
)
if not defined Q_REL set "Q_REL=root"
if "!Q_REL:~-1!"=="\" set "Q_REL=!Q_REL:~0,-1!"
set "Q_REL=!Q_REL:\=__!"
set "Q_TARGET_DIR=%QUARANTINE_DIR%\!Q_REL!"
if not exist "!Q_TARGET_DIR!\" mkdir "!Q_TARGET_DIR!" >nul 2>&1
if not exist "!Q_TARGET_DIR!\" (
  echo [ERROR] Could not create quarantine folder for !Q_NAME!
  goto :eof
)

set "Q_TARGET=!Q_TARGET_DIR!\!Q_NAME!"
set /a Q_SUFFIX=1
:quarantine_find_name
if not exist "!Q_TARGET!" goto :quarantine_move
set "Q_TARGET=!Q_TARGET_DIR!\!Q_NAME!.!Q_SUFFIX!"
set /a Q_SUFFIX+=1
goto :quarantine_find_name

:quarantine_move
move /y "%Q_SOURCE_DIR%%Q_NAME%" "!Q_TARGET!" >nul 2>&1
if errorlevel 1 (
  echo [ERROR] Could not quarantine !Q_NAME! -- is it in use?
) else (
  echo          - !Q_NAME!
)
goto :eof

rem ------------------------------------------------------------
rem  :stage <name>
rem ------------------------------------------------------------
:stage
echo.
echo ------------------------------------------------------------
echo  %~1
echo ------------------------------------------------------------
goto :eof

rem ------------------------------------------------------------
rem  :detect_steamstub
rem
rem  Parses the selected executable's PE section table and checks the .bind
rem  payload for the loader signatures used by SteamStub 1.x through 3.x.
rem  PowerShell is built into supported Windows versions and lets us inspect
rem  binary data without shipping another executable beside patch.bat.
rem ------------------------------------------------------------
:detect_steamstub
set "UCO_STUB_EXE=%GAME_EXE%"
powershell.exe -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -Command "$p=$env:UCO_STUB_EXE; try { $b=[IO.File]::ReadAllBytes($p); if($b.Length -lt 256 -or $b[0] -ne 0x4d -or $b[1] -ne 0x5a){exit 3}; $pe=[BitConverter]::ToInt32($b,0x3c); if($pe -lt 0 -or $pe+24 -ge $b.Length -or $b[$pe] -ne 0x50 -or $b[$pe+1] -ne 0x45){exit 3}; $count=[BitConverter]::ToUInt16($b,$pe+6); $opt=[BitConverter]::ToUInt16($b,$pe+20); $table=$pe+24+$opt; $bind=$null; for($i=0;$i -lt $count;$i++){ $o=$table+40*$i; if($o+40 -gt $b.Length){break}; $name=[Text.Encoding]::ASCII.GetString($b,$o,8).Trim([char]0); if($name -eq '.bind'){ $size=[BitConverter]::ToUInt32($b,$o+16); $raw=[BitConverter]::ToUInt32($b,$o+20); if($raw -lt $b.Length){$take=[Math]::Min([int64]$size,[int64]$b.Length-$raw); $bind=New-Object byte[] $take; [Array]::Copy($b,$raw,$bind,0,$take)}; break } }; if($null -eq $bind){exit 1}; function HasSeq([byte[]]$h,[byte[]]$n){ if($h.Length -lt $n.Length){return $false}; for($x=0;$x -le $h.Length-$n.Length;$x++){ $ok=$true; for($y=0;$y -lt $n.Length;$y++){if($h[$x+$y] -ne $n[$y]){$ok=$false;break}}; if($ok){return $true} }; return $false }; $s64=[byte[]](0xe8,0,0,0,0,0x50,0x53,0x51,0x52,0x56,0x57,0x55,0x41,0x50); $s3=[byte[]](0xe8,0,0,0,0,0x50,0x53,0x51,0x52,0x56,0x57,0x55,0x8b,0x44,0x24,0x1c,0x2d,5,0,0,0,0x8b,0xcc,0x83,0xe4,0xf0,0x51,0x51,0x51,0x50); $s2=[byte[]](0x53,0x51,0x52,0x56,0x57,0x55,0x8b,0xec,0x81,0xec,0,0x10,0,0); $s1=[byte[]](0x60,0x81,0xec,0,0x10,0,0,0xbe); if((HasSeq $bind $s64) -or (HasSeq $bind $s3) -or (HasSeq $bind $s2) -or (HasSeq $bind $s1)){exit 0}; exit 2 } catch { exit 3 }" >nul 2>&1
set "STUB_RESULT=%errorlevel%"
if "%STUB_RESULT%"=="0" (
  set "GET_STUBBED=true"
) else if "%STUB_RESULT%"=="2" (
  echo [WARN] The executable has a .bind section, but its loader signature is
  echo        unfamiliar. SteamStub was not enabled automatically.
) else if "%STUB_RESULT%"=="3" (
  echo [WARN] Could not parse the executable for SteamStub detection.
)
set "UCO_STUB_EXE="
goto :eof

:end
echo.
pause
endlocal
