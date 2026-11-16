@echo off
REM Builds and runs the standalone check for the LocalSaves store (UcoCloudEmu).
REM Not part of the normal build -- run it by hand after touching remote_storage_emu.h.
setlocal
set VS=C:\Program Files\Microsoft Visual Studio\18\Community
call "%VS%\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
pushd "%~dp0"
cl /nologo /EHsc /std:c++17 /W3 /Fe:cloudemu_test.exe cloudemu_test.cpp /I"%~dp0..\include" || goto :fail
.\cloudemu_test.exe
popd
exit /b %ERRORLEVEL%
:fail
popd
echo BUILD FAILED
exit /b 1
