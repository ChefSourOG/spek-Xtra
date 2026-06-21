@echo off
setlocal

set "MSYS2=C:\msys64"
set "PROJECT_MSYS=/c/Users/jpisa/Desktop/spek-x_fork/spek-Xtra"

if not exist "%MSYS2%\usr\bin\env.exe" (
    echo ERROR: MSYS2 not found at %MSYS2%
    pause
    exit /b 1
)

echo.
echo ============================================
echo  Spek-Xtra MinGW64 Build
echo ============================================
echo.
echo Project: %PROJECT_MSYS%
echo MSYS2:   %MSYS2%
echo.

"%MSYS2%\usr\bin\env.exe" MSYSTEM=MINGW64 /usr/bin/bash -lc "cd '%PROJECT_MSYS%' && make distclean || true && set -e && ./autogen.sh && ./configure --prefix=/mingw64 && make -j$(nproc)"

if %ERRORLEVEL% neq 0 (
    echo.
    echo ============================================
    echo  BUILD FAILED
    echo ============================================
    echo  See error messages above.
    echo.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ============================================
echo  BUILD COMPLETED SUCCESSFULLY
echo ============================================
echo  Binary location:
echo  C:\Users\jpisa\Desktop\spek-x_fork\spek-Xtra\src\spek-xtra.exe
echo.
pause
