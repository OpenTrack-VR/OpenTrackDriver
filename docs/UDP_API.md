# OpenTrackDriver UDP API Documentation

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
    Tracker = 0,
    HMD = 1,
    LeftController = 2,
    RightController = 3
};
```

## Data Format

### Position (pos)
- Array of 3 float values representing X, Y, Z coordinates in meters
- Right-handed coordinate system
- Origin at floor level

### Rotation (rot)
- Array of 4 float values representing quaternion rotation (W, X, Y, Z)
- Right-handed coordinate system
- Unit quaternion (should be normalized)

### Serial Number
- 16-byte character array
- Null-terminated string
- Used to identify individual trackers
- For HMD and controllers, this field is ignored

## Usage Examples

### Sending Single Device Data
```cpp
UdpPosePacket packet;
packet.device_type = DeviceType::Tracker;
strncpy(packet.serial, "TRACKER_001", 16);
packet.pos[0] = 1.0f;  // X
packet.pos[1] = 2.0f;  // Y
packet.pos[2] = 0.5f;  // Z
packet.rot[0] = 1.0f;  // W
packet.rot[1] = 0.0f;  // X
packet.rot[2] = 0.0f;  // Y
packet.rot[3] = 0.0f;  // Z
```

### Sending Batch Data
```cpp
UdpBatchPacket batch;
batch.num_devices = 2;

// First device
batch.devices[0].device_type = DeviceType::Tracker;
strncpy(batch.devices[0].serial, "TRACKER_001", 16);
batch.devices[0].pos[0] = 1.0f;
batch.devices[0].pos[1] = 2.0f;
batch.devices[0].pos[2] = 0.5f;
batch.devices[0].rot[0] = 1.0f;
batch.devices[0].rot[1] = 0.0f;
batch.devices[0].rot[2] = 0.0f;
batch.devices[0].rot[3] = 0.0f;

// Second device
batch.devices[1].device_type = DeviceType::HMD;
batch.devices[1].pos[0] = 0.0f;
batch.devices[1].pos[1] = 1.7f;
batch.devices[1].pos[2] = 0.0f;
batch.devices[1].rot[0] = 1.0f;
batch.devices[1].rot[1] = 0.0f;
batch.devices[1].rot[2] = 0.0f;
batch.devices[1].rot[3] = 0.0f;
```

## Best Practices

1. **Update Frequency**
   - Send updates at least 60Hz for smooth tracking
   - Recommended update rate: 90-144Hz
   - Batch multiple device updates when possible

2. **Data Validation**
   - Ensure quaternions are normalized
   - Validate position values are within reasonable ranges
   - Check serial numbers are properly null-terminated

3. **Network Considerations**
   - Use a reliable network connection
   - Consider using QoS settings for UDP packets
   - Monitor packet loss and latency

4. **Error Handling**
   - Implement timeout detection
   - Handle network disconnections gracefully
   - Log tracking data issues

## Implementation Notes

- The server runs on port 9000 by default
- Packets are sent using UDP protocol
- Maximum batch size is 8 devices
- All floating-point values are 32-bit IEEE 754 format
- Packets are packed with 1-byte alignment for optimal network transmission

## Troubleshooting

1. **No Tracking Data**
   - Verify UDP port 9000 is open
   - Check network connectivity
   - Ensure correct packet format
   - Verify device types are correct

2. **Jittery Tracking**
   - Check network latency
   - Verify update frequency
   - Ensure consistent timing
   - Monitor system resources

3. **Incorrect Orientation**
   - Verify quaternion values are normalized
   - Check coordinate system alignment
   - Validate rotation order
   - Ensure proper device type assignment 