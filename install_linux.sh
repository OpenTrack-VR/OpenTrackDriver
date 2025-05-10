#!/bin/bash
set -e

# Script to install OpenTrackServer driver for SteamVR on Linux
# This driver now uses a high-performance UDP protocol (port 9000 by default) for all tracking data.
# No HTTP/JSON server is included or required.
echo "Installing OpenTrackServer driver for SteamVR (UDP-only version)..."

echo "Please run this script from the root dist directory (e.g., build/dist)."

# Check if driver directory exists
if [ ! -d "driver_OpenTrackServer" ]; then
    echo "Error: Driver directory not found. Please make sure you are running this script from the dist directory."
    exit 1
fi

# SteamVR drivers directory
STEAMVR_DRIVERS_DIR="$HOME/.steam/steam/steamapps/common/SteamVR/drivers"

# Check if SteamVR is installed
if [ ! -d "$STEAMVR_DRIVERS_DIR" ]; then
    echo "Error: SteamVR drivers directory not found at: $STEAMVR_DRIVERS_DIR"
    echo "Please make sure SteamVR is installed."
    exit 1
fi

# Create drivers directory if it doesn't exist
mkdir -p "$STEAMVR_DRIVERS_DIR"

# Copy the driver
echo "Copying driver to SteamVR drivers directory..."
cp -r "driver_OpenTrackServer" "$STEAMVR_DRIVERS_DIR/"

echo "Installation complete!"
echo "The OpenTrackServer driver now uses UDP on port 9000 for all tracking data."
echo "Please restart SteamVR to use the driver." 