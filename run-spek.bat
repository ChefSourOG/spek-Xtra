@echo off
setlocal

set "SPEK_EXE=%~dp0dist\win\Spek-Xtra\spek-xtra.exe"

set "SPEK_ZIP=%~dp0dist\win\Spek-Xtra\Spek-Xtra.zip"

if not exist "%SPEK_ZIP%" (
    echo ERROR: Could not find Spek-Xtra.zip. Please run dist/win/bundle.sh first.
    pause
    exit /b 1
)

echo Updating distribution archive ...
powershell -Command "Expand-Archive -Path '%SPEK_ZIP%' -DestinationPath '%~dp0dist\win\Spek-Xtra' -Force"

if not exist "%SPEK_EXE%" (
    echo ERROR: Could not extract spek-xtra.exe. Please check the build output.
    pause
    exit /b 1
)

set PATH=C:\msys64\mingw64\bin;%PATH%
start "" "%SPEK_EXE%" %*
