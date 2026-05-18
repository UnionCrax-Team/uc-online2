@echo off
setlocal

set "PROJECT=%~dp0uc_online2.vcxproj"

REM Find MSBuild
set "MSBUILD="
for /f "delims=" %%i in ('dir /b /s "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" 2^>nul') do (
    if not defined MSBUILD set "MSBUILD=%%i"
)

if not defined MSBUILD (
    echo [ERROR] MSBuild not found.
    echo.
    echo You need Visual Studio Build Tools 2022 or later.
    echo Install it here: https://visualstudio.microsoft.com/visual-cpp-build-tools/
    echo.
    echo Select "Desktop development with C++" during installation.
    echo.
    pause
    exit /b 1
)

echo Found MSBuild: %MSBUILD%
echo.

echo ========================================
echo  Building x86 (debug) (steam_api.dll)
echo ========================================
"%MSBUILD%" "%PROJECT%" -p:Configuration=Debug -p:Platform=Win32 -m
if %errorlevel% neq 0 (
    echo.
    echo [ERROR] x86 build failed.
    pause
    exit /b 1
)
echo x86 debug build succeeded: debbuild\x86\steam_api.dll
echo.

echo ========================================
echo  Building x64 (debug) (steam_api64.dll)
echo ========================================
"%MSBUILD%" "%PROJECT%" -p:Configuration=Debug -p:Platform=x64 -m
if %errorlevel% neq 0 (
    echo.
    echo [ERROR] x64 build failed.
    pause
    exit /b 1
)
echo x64 debug build succeeded: debbuild\x64\steam_api64.dll
echo.

echo ========================================
echo  Both debug builds completed successfully
echo ========================================
echo  x86: debbuild\x86\steam_api.dll
echo  x64: debbuild\x64\steam_api64.dll
echo ========================================

pause
