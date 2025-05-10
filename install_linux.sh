#!/bin/bash
set -e

# install script
echo "Installing OpenTrackDriver for SteamVR (UDP-only version)..."

echo "Please run this script from the root dist directory (e.g., build/dist)."

# check driver
if [ ! -d "driver_OpenTrackDriver" ]; then
    echo "Error: Driver directory not found. Please make sure you are running this script from the dist directory."
    exit 1
fi

# steamvr path
STEAMVR_DRIVERS_DIR="$HOME/.steam/steam/steamapps/common/SteamVR/drivers"

# check steamvr
if [ ! -d "$STEAMVR_DRIVERS_DIR" ]; then
    echo "Error: SteamVR drivers directory not found at: $STEAMVR_DRIVERS_DIR"
    echo "Please make sure SteamVR is installed."
    exit 1
fi

# create dir
mkdir -p "$STEAMVR_DRIVERS_DIR"

# copy files
echo "Copying driver to SteamVR drivers directory..."
cp -r "driver_OpenTrackDriver" "$STEAMVR_DRIVERS_DIR/"

echo "Installation complete!"
echo "The OpenTrackDriver now uses UDP on port 9000 for all tracking data."
echo "Please restart SteamVR to use the driver." 