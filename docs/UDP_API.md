# OpenTrackDriver UDP API Documentation

## Available Trackers

The driver supports the following trackers by default:

| Serial Number | Purpose | Description |
|--------------|---------|-------------|
| OpenTrackDriver_Waist | Waist Tracker | Tracks hip/waist position |
| OpenTrackDriver_LeftFoot | Left Foot | Tracks left foot position |
| OpenTrackDriver_RightFoot | Right Foot | Tracks right foot position |
| OpenTrackDriver_LeftKnee | Left Knee | Tracks left knee position |
| OpenTrackDriver_RightKnee | Right Knee | Tracks right knee position |
| OpenTrackDriver_LeftElbow | Left Elbow | Tracks left elbow position |
| OpenTrackDriver_RightElbow | Right Elbow | Tracks right elbow position |
| OpenTrackDriver_Chest | Chest | Tracks chest position |

You can also track:
- HMD (Head position)
- Left Controller
- Right Controller

## Overview
The OpenTrackDriver uses UDP to receive tracking data for SteamVR. The driver listens on port 9000 by default and accepts pose data for various VR devices including trackers, HMD, and controllers.

## Protocol Specification

### Packet Types

#### Single Device Packet (UdpPosePacket)
```cpp
struct UdpPosePacket {
    DeviceType device_type;  // device type
    char serial[16];        // device serial
    float pos[3];          // position xyz
    float rot[4];          // rotation wxyz
};
```

#### Batch Packet (UdpBatchPacket)
```cpp
struct UdpBatchPacket {
    uint8_t num_devices;    // device count
    UdpPosePacket devices[8]; // device batch
};
```

### Device Types
```cpp
enum class DeviceType : uint8_t {
    Tracker = 0,      // Generic tracker device
    HMD = 1,          // Head-mounted display
    LeftController = 2, // Left motion controller
    RightController = 3  // Right motion controller
};
```

## Data Format

### Position (pos)
- Array of 3 float values representing X, Y, Z coordinates in meters
- Right-handed coordinate system
- Origin at floor level
- X: positive right, negative left
- Y: positive up, negative down
- Z: positive forward, negative backward
- Values should be in meters (e.g., 1.7 for average human height)
- Stored in `HmdVector3_t` structure in the driver

### Rotation (rot)
- Array of 4 float values representing quaternion rotation (W, X, Y, Z)
- Right-handed coordinate system
- Must be a unit quaternion (W² + X² + Y² + Z² = 1)
- W: scalar component
- X: rotation around X axis
- Y: rotation around Y axis
- Z: rotation around Z axis
- Rotation order: ZYX (yaw, pitch, roll)
- Stored in `HmdQuaternion_t` structure in the driver

### Serial Number
- 16-byte character array
- Must be null-terminated
- Used to identify individual trackers
- For HMD and controllers, this field is ignored
- Maximum length: 15 characters plus null terminator
- Valid characters: alphanumeric and underscore only
- Used as key in the driver's tracker registry

## Device Properties

### Trackers
- Device class: GenericTracker
- Wireless: true
- Will drift in yaw: false
- Battery percentage: 100%
- Charging: false
- Model number: "OpenTrackDriver"
- Input components:
  - /input/trigger/click (boolean)
  - /input/trigger/value (scalar, normalized one-sided)

### HMD and Controllers
- No additional properties set
- Only pose data is updated

## Usage Examples

### Sending Single Device Data
```cpp
UdpPosePacket packet;
packet.device_type = DeviceType::Tracker;
strncpy(packet.serial, "TRACKER_001", 16);
packet.pos[0] = 1.0f;  // X: 1 meter right
packet.pos[1] = 2.0f;  // Y: 2 meters up
packet.pos[2] = 0.5f;  // Z: 0.5 meters forward
packet.rot[0] = 1.0f;  // W: no rotation
packet.rot[1] = 0.0f;  // X: no rotation around X
packet.rot[2] = 0.0f;  // Y: no rotation around Y
packet.rot[3] = 0.0f;  // Z: no rotation around Z
```

### Sending Batch Data
```cpp
UdpBatchPacket batch;
batch.num_devices = 2;  // Must be between 1 and 8

// First device
batch.devices[0].device_type = DeviceType::Tracker;
strncpy(batch.devices[0].serial, "TRACKER_001", 16);
batch.devices[0].pos[0] = 1.0f;  // X: 1 meter right
batch.devices[0].pos[1] = 2.0f;  // Y: 2 meters up
batch.devices[0].pos[2] = 0.5f;  // Z: 0.5 meters forward
batch.devices[0].rot[0] = 1.0f;  // W: no rotation
batch.devices[0].rot[1] = 0.0f;  // X: no rotation around X
batch.devices[0].rot[2] = 0.0f;  // Y: no rotation around Y
batch.devices[0].rot[3] = 0.0f;  // Z: no rotation around Z

// Second device
batch.devices[1].device_type = DeviceType::HMD;
batch.devices[1].pos[0] = 0.0f;   // X: at origin
batch.devices[1].pos[1] = 1.7f;   // Y: 1.7 meters up (average height)
batch.devices[1].pos[2] = 0.0f;   // Z: at origin
batch.devices[1].rot[0] = 1.0f;   // W: no rotation
batch.devices[1].rot[1] = 0.0f;   // X: no rotation around X
batch.devices[1].rot[2] = 0.0f;   // Y: no rotation around Y
batch.devices[1].rot[3] = 0.0f;   // Z: no rotation around Z
```

## Best Practices

1. **Update Frequency**
   - Send updates at least 60Hz for smooth tracking
   - Recommended update rate: 90-144Hz
   - Batch multiple device updates when possible

2. **Data Validation**
   - Ensure quaternions are normalized (W² + X² + Y² + Z² = 1)
   - Validate position values are within reasonable ranges (-10m to +10m)
   - Check serial numbers are properly null-terminated
   - Verify device types are valid (0-3)
   - Ensure batch size is between 1 and 8

3. **Network Considerations**
   - Use a reliable network connection
   - Consider using QoS settings for UDP packets
   - Monitor packet loss and latency

4. **Error Handling**
   - Implement timeout detection
   - Handle network disconnections gracefully
   - Log tracking data issues

## Implementation Notes

- The driver runs on port 9000 by default
- Packets are sent using UDP protocol
- Maximum batch size is 8 devices
- All floating-point values are 32-bit IEEE 754 format
- Packets are packed with 1-byte alignment for optimal network transmission
- Total packet sizes:
  - Single device: 25 bytes (1 + 16 + 3*4 + 4*4)
  - Batch: 1 + (25 * num_devices) bytes
- Thread-safe implementation using mutex locks
- Pose updates are atomic per device

## Troubleshooting

1. **No Tracking Data**
   - Verify UDP port 9000 is open
   - Check network connectivity
   - Ensure correct packet format
   - Verify device types are correct
   - Check packet size matches expected size
   - Verify tracker is registered with correct serial number

2. **Jittery Tracking**
   - Check network latency
   - Verify update frequency
   - Ensure consistent timing
   - Monitor system resources

3. **Incorrect Orientation**
   - Verify quaternion values are normalized
   - Check coordinate system alignment
   - Validate rotation order (ZYX)
   - Ensure proper device type assignment 