@echo off
setlocal enabledelayedexpansion

echo Installing OpenTrackDriver for SteamVR (UDP-only version)...

echo This driver now uses a high-performance UDP protocol (port 9000 by default) for all tracking data.
echo No HTTP/JSON server is included or required.

echo Please run this script from the root dist directory (e.g., build\dist).

:: check driver
if not exist "driver_OpenTrackDriver" (
    echo Error: Driver directory not found. Please make sure you are running this script from the dist directory.
    exit /b 1
)

:: find steam
set "STEAM_PATH="
for /f "tokens=2*" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Valve\Steam" /v "InstallPath" 2^>nul') do set "STEAM_PATH=%%b"
if not defined STEAM_PATH (
    for /f "tokens=2*" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Valve\Steam" /v "InstallPath" 2^>nul') do set "STEAM_PATH=%%b"
)

if not defined STEAM_PATH (
    echo Error: Could not find Steam installation directory.
    echo Please make sure Steam is installed.
    exit /b 1
)

:: set path
set "STEAMVR_DRIVERS_DIR=%STEAM_PATH%\steamapps\common\SteamVR\drivers"

:: check steamvr
if not exist "%STEAMVR_DRIVERS_DIR%" (
    echo Error: SteamVR drivers directory not found at: %STEAMVR_DRIVERS_DIR%
    echo Please make sure SteamVR is installed.
    exit /b 1
)

:: create dir
if not exist "%STEAMVR_DRIVERS_DIR%" mkdir "%STEAMVR_DRIVERS_DIR%"

:: copy files
echo Copying driver to SteamVR drivers directory...
xcopy /E /I /Y "driver_OpenTrackDriver" "%STEAMVR_DRIVERS_DIR%\driver_OpenTrackDriver"

echo Installation complete!
echo The OpenTrackDriver now uses UDP on port 9000 for all tracking data.
echo Please restart SteamVR to use the driver.

pause 