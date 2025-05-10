# OpenTrackDriver

OpenTrackDriver is a SteamVR driver component of the OpenTrack full body app. It's Job is to send the Calculated Tracking Data from the OpenTrackServer to SteamVR

## Features

- UDP-based tracking data transmission (port 9000 by default)
- Support for HMD, left/right controllers, and multiple trackers
- Batch updates for efficient multi-device tracking

## Building

### Prerequisites

- CMake (version 3.10 or higher)
- C++17 compatible compiler
- OpenVR SDK

### Build Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/OpenTrack-VR/OpenTrackDriver.git
   cd OpenTrackDriver
   ```

2. Create a build directory and build the project:
   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build .
   cmake --build . --target dist
   ```

3. The distribution package will be created in `build/dist`.

## Installation

### Linux

1. Navigate to the `build/dist` directory.
2. Run the install script:
   ```bash
   ./install_linux.sh
   ```

### Windows

1. Navigate to the `build\dist` directory.
2. Run the install script:
   ```batch
   install_windows.bat
   ```

## License

This project is licensed under the GNU Affero General Public License v3.0 (AGPL-3.0) - see the [LICENSE](LICENSE) file for details.

### What this means for you:

- You are free to use, modify, and distribute this software
- You must include the original copyright notice and license
- If you modify the code, you must share your changes under the same license
- If you use this code in a network service, you must share your modifications
- You cannot use this code in closed-source commercial products without permission

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. 