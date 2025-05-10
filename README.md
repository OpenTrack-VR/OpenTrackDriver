# OpenTrackServer

OpenTrackServer is a SteamVR driver that uses a high-performance UDP protocol for tracking data. It supports HMD, controllers, and multiple trackers.

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
   git clone https://github.com/yourusername/OpenTrackServer.git
   cd OpenTrackServer
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

## Usage

The driver uses UDP on port 9000 for all tracking data. No HTTP/JSON server is included or required.

## License

[Your License Here]

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. 