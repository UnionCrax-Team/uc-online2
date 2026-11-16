@echo off
setlocal
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Invoke-CoherenceSchemaPipeline.ps1"
set "RESULT=%ERRORLEVEL%"
echo.
if not "%RESULT%"=="0" echo Coherence schema setup failed. Read the error above.
pause
exit /b %RESULT%
