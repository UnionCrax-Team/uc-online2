@echo off
SETLOCAL EnableDelayedExpansion

:: --- Configuration ---
SET "UC_OUT=steam_api64.dll"
SET "EOS_OUT=EOSSDK-Win64-Shipping.dll"
SET "EOS_SPOOF_DLL=EOSSDK-Win64-Shipping.yes"

echo [!] Starting Jiro's Unified Build Process (v2 - Stability Focused)...

:: 1. Setup MSVC Environment
where cl >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] cl.exe not found. Please run this from the Visual Studio Developer Command Prompt.
    pause
    exit /b 1
)

:: ---------------------------------------------------------
:: TARGET 1: UC-Online2 (The Steam Side)
:: ---------------------------------------------------------
echo.
echo [+] Building UC-Online2 (via MSBuild for stability)...
:: Use the project file since it has all the correct flags and dependencies
set "PROJECT=%~dp0uc_online2.vcxproj"
set "MSBUILD=c:\program files\microsoft visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
if not exist "%MSBUILD%" set "MSBUILD="
    cl.exe /nologo /D"_CRT_SECURE_NO_WARNINGS" /EHsc /LD /I"./include" /Fe:"%UC_OUT%" dllmain.cpp /link /LIBPATH:./lib libMinHook.x64.lib
) else (
    "%MSBUILD%" "%PROJECT%" -p:Configuration=Release -p:Platform=x64 -m
)

if %errorlevel% neq 0 (
    echo [ERROR] UC-Online2 build failed!
    pause
    exit /b 1
)
echo [SUCCESS] Generated %UC_OUT%

:: ---------------------------------------------------------
:: TARGET 2: EOS Proxy (The Epic Side)
:: ---------------------------------------------------------
echo.
echo [+] Building EOS Proxy
:: Added /LD for DLL and /O2 for optimization
cl.exe /nologo /D"_CRT_SECURE_NO_WARNINGS" /LD /O2 /Fe:"%EOS_OUT%" eos_proxy.c
if %errorlevel% neq 0 (
    echo [ERROR] EOS Proxy build failed!
    pause
    exit /b 1
)
echo [SUCCESS] Generated %EOS_OUT%

:end
echo.
echo [!] All builds completed.
echo ---------------------------------------------------------
echo [TIPS] 
echo 1. Copy %UC_OUT% to game folder.
echo 2. Copy %EOS_OUT% to game folder.
echo 3. Rename original EOS DLL to %EOS_SPOOF_DLL%
echo ---------------------------------------------------------
pause
