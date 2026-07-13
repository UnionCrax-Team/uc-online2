@echo off
setlocal enableextensions enabledelayedexpansion
title UCOnline2 - patch.bat (Photon auto-patcher)
color 0b

rem ============================================================
rem  UCOnline2 patch.bat
rem
rem  Drop a game folder onto this file (or run: patch.bat "C:\path\to\game").
rem  It detects whether the game uses Photon, and if so:
rem    * copies photon_universal.dll into <game>\plugins\
rem    * writes a union-crax.ini with the right [Realtime] or [Fusion] section
rem
rem  It does NOT modify game assets. The GUIDs are YOUR Photon app IDs
rem  (create them at https://dashboard.photonengine.com/), so it prompts
rem  for them. For non-Photon games it says so and exits without changes.
rem ============================================================

set "SCRIPTDIR=%~dp0"

rem ---- locate the plugin DLL (built output, else release-style fallback) ----
set "PLUGIN_DLL="
if exist "%SCRIPTDIR%plugins\photon_universal\relbuild\x64\photon_universal.dll" set "PLUGIN_DLL=%SCRIPTDIR%plugins\photon_universal\relbuild\x64\photon_universal.dll"
if not defined PLUGIN_DLL if exist "%SCRIPTDIR%photon_universal.dll" set "PLUGIN_DLL=%SCRIPTDIR%photon_universal.dll"
if not defined PLUGIN_DLL (
  echo [ERROR] photon_universal.dll not found.
  echo   Build it:  msbuild plugins\photon_universal\photon_universal_plugin.vcxproj -p:Configuration=Release -p:Platform=x64
  echo   or drop photon_universal.dll next to this patch.bat.
  goto :end
)

rem ---- resolve game folder (arg or prompt) ----
set "GAME=%~1"
if not defined GAME set /p "GAME=Drag the game folder here (or paste its path): "
set "GAME=%GAME:"=%"
if not defined GAME echo [ERROR] No folder given.& goto :end
if not exist "%GAME%\" echo [ERROR] Not a folder: %GAME%& goto :end
echo.
echo Game folder: %GAME%

rem ---- find the *_Data folder ----
set "DATA="
for /d %%D in ("%GAME%\*_Data") do set "DATA=%%~fD"
if not defined DATA (
  echo [ERROR] No "<Game>_Data" folder found. Is this a Unity game folder?
  goto :end
)
echo Data folder: %DATA%

rem ============================================================
rem  Detect backend + Photon flavor
rem ============================================================
set "BACKEND="
set "FLAVOR="
set "HAS_VOICE="

if exist "%DATA%\Managed" (
  set "BACKEND=Mono"
  if exist "%DATA%\Managed\PhotonUnityNetworking.dll" set "FLAVOR=Realtime"
  if not defined FLAVOR if exist "%DATA%\Managed\PhotonRealtime.dll" set "FLAVOR=Realtime"
  if exist "%DATA%\Managed\Fusion.Realtime.dll" set "FLAVOR=Fusion"
  if exist "%DATA%\Managed\PhotonVoice.dll" set "HAS_VOICE=1"
  if exist "%DATA%\Managed\PhotonVoice.PUN.dll" set "HAS_VOICE=1"
) else if exist "%DATA%\il2cpp_data\Metadata\global-metadata.dat" (
  set "BACKEND=IL2CPP"
  set "META=%DATA%\il2cpp_data\Metadata\global-metadata.dat"
  findstr /m /c:"NetworkRunner" "%DATA%\il2cpp_data\Metadata\global-metadata.dat" >nul 2>&1 && set "FLAVOR=Fusion"
  if not defined FLAVOR findstr /m /c:"LoadBalancingClient" "%DATA%\il2cpp_data\Metadata\global-metadata.dat" >nul 2>&1 && set "FLAVOR=Realtime"
  if not defined FLAVOR findstr /m /c:"PhotonNetwork" "%DATA%\il2cpp_data\Metadata\global-metadata.dat" >nul 2>&1 && set "FLAVOR=Realtime"
  findstr /m /c:"PhotonVoice" "%DATA%\il2cpp_data\Metadata\global-metadata.dat" >nul 2>&1 && set "HAS_VOICE=1"
) else (
  echo [ERROR] Could not find Managed\ or il2cpp_data\ under the data folder.
  goto :end
)

echo.
if not defined FLAVOR (
  echo [DETECTED] Backend=%BACKEND%  Photon: none
  echo   No Photon found. Writing a base union-crax.ini only ^(no plugin^).
  echo   If multiplayer is pure Steam P2P this is all you need; try it bare.
) else (
  echo [DETECTED] Backend=%BACKEND%  Photon flavor=%FLAVOR%
  if defined HAS_VOICE echo            Photon Voice present ^(a separate Voice app is required^).
)
echo.

rem ============================================================
rem  Gather config -- AppId is always needed
rem ============================================================
set "OGAPPID="
set /p "OGAPPID=Real Steam AppId of the game (the ogAppId): "
if not defined OGAPPID echo [ERROR] AppId is required.& goto :end

rem NOTE: `set /p` must NOT be inside a ( ) block -- it misbehaves there. Each
rem prompt is at top level, dispatched by goto, so reads are reliable.
set "RTGUID="
set "VOICEGUID="
set "FUSIONGUID="

if not defined FLAVOR goto :write_ini
if /i "%FLAVOR%"=="Fusion" goto :ask_fusion

:ask_realtime
set /p "RTGUID=Your Photon REALTIME app GUID: "
if not defined RTGUID echo [ERROR] Realtime GUID is required.& goto :end
if defined HAS_VOICE goto :ask_voice_required
set /p "VOICEGUID=Photon VOICE app GUID (optional, press Enter to skip): "
goto :write_ini

:ask_voice_required
set /p "VOICEGUID=Your Photon VOICE app GUID (required for this game): "
goto :write_ini

:ask_fusion
set /p "FUSIONGUID=Your Photon FUSION app GUID: "
if not defined FUSIONGUID echo [ERROR] Fusion GUID is required.& goto :end
goto :write_ini

rem ============================================================
rem  Write union-crax.ini  (sequential appends -- robust)
rem ============================================================
:write_ini
set "INI=%GAME%\union-crax.ini"
> "%INI%" echo [Settings]
>> "%INI%" echo AppId=480
>> "%INI%" echo ogAppId=%OGAPPID%
>> "%INI%" echo PluginsFolder=plugins
>> "%INI%" echo GetStubbedLol=false
if not defined FLAVOR goto :wrote_ini
>> "%INI%" echo(
if /i "%FLAVOR%"=="Fusion" goto :write_fusion
>> "%INI%" echo [Realtime]
>> "%INI%" echo PhotonAppIdRealtime=%RTGUID%
if defined VOICEGUID >> "%INI%" echo PhotonAppIdVoice=%VOICEGUID%
>> "%INI%" echo ForcedAuthType=0
goto :wrote_ini

:write_fusion
>> "%INI%" echo [Fusion]
>> "%INI%" echo PhotonAppIdFusion=%FUSIONGUID%
>> "%INI%" echo ForcedAuthType=0

:wrote_ini
echo [OK] Wrote %INI%

rem ============================================================
rem  Deploy the plugin DLL (Photon games only)
rem ============================================================
if not defined FLAVOR goto :done_nophoton
if not exist "%GAME%\plugins\" mkdir "%GAME%\plugins"
copy /y "%PLUGIN_DLL%" "%GAME%\plugins\photon_universal.dll" >nul
if errorlevel 1 echo [ERROR] Failed to copy plugin DLL.& goto :end
echo [OK] Copied photon_universal.dll to %GAME%\plugins\

echo.
echo ============================================================
echo  DONE ^(Photon: %FLAVOR%^).
echo  Still to do yourself:
echo   1. Put UCOnline2's steam_api64.dll in %DATA%\Plugins\x86_64\ (back up the original).
echo   2. On each Photon app: Manage -^> Authentication -^> Add Provider -^> Custom,
echo      paste your permissive Cloudflare Worker URL, UNCHECK "Reject Clients
echo      on Authentication Failure", Save.
if defined HAS_VOICE echo   3. This game uses Photon Voice - you MUST create a Voice-type app too.
echo  Then launch. Tail %%TEMP%%\uc_online2.log for [Realtime]/[Fusion] lines.
echo ============================================================
goto :end

:done_nophoton
echo.
echo ============================================================
echo  DONE ^(base ini only, no Photon plugin^).
echo  Still to do yourself:
echo   1. Put UCOnline2's steam_api64.dll in %DATA%\Plugins\x86_64\ (back up the original).
echo   2. Launch the game. If multiplayer is pure Steam P2P it should work bare.
echo      If it needs EOS/Photon/etc, this game needs a different plugin.
echo ============================================================

:end
echo.
pause
endlocal
