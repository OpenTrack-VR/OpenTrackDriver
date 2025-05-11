# OpenTrackDriver UDP API Documentation

## Available Trackers

The OpenTrackDriver API supports a variety of trackers for different body parts. Each tracker has a unique serial number, which can be used to identify it within the system. Below is a list of supported trackers by default:

| Serial Number               | Purpose       | Description                 |
| --------------------------- | ------------- | --------------------------- |
| OpenTrackDriver\_Waist      | Waist Tracker | Tracks hip/waist position   |
| OpenTrackDriver\_LeftFoot   | Left Foot     | Tracks left foot position   |
| OpenTrackDriver\_RightFoot  | Right Foot    | Tracks right foot position  |
| OpenTrackDriver\_LeftKnee   | Left Knee     | Tracks left knee position   |
| OpenTrackDriver\_RightKnee  | Right Knee    | Tracks right knee position  |
| OpenTrackDriver\_LeftElbow  | Left Elbow    | Tracks left elbow position  |
| OpenTrackDriver\_RightElbow | Right Elbow   | Tracks right elbow position |
| OpenTrackDriver\_Chest      | Chest         | Tracks chest position       |

Additionally, the following devices can also be tracked:

* **HMD** (Head position)
* **Left Controller**
* **Right Controller**

## Raw Byte Format

You can send tracking data to the driver directly using the raw byte format. Below are the formats for sending a single device packet and a batch of device data.

### Single Device Packet (25 bytes)

Each tracker or device sends a packet that is **25 bytes** long, structured as follows:

```
[0]      - Device Type (1 byte)
          0 = Tracker
          1 = HMD
          2 = Left Controller
          3 = Right Controller

[1-16]   - Serial Number (16 bytes)
          Null-terminated string
          For HMD/Controllers: ignored

[17-28]  - Position (12 bytes)
          [17-20]   X (float)
          [21-24]   Y (float)
          [25-28]   Z (float)

[29-44]  - Rotation (16 bytes)
          [29-32]   W (float)
          [33-36]   X (float)
          [37-40]   Y (float)
          [41-44]   Z (float)
```

### Batch Packet

To send data for multiple devices in one packet, you can use a **Batch Packet**. The first byte represents the number of devices, and the rest of the packet contains each device’s data (25 bytes per device).

```
[0]      - Number of devices (1 byte)
          Must be between 1 and 8

[1-225]  - Device data (25 bytes per device)
          Same format as single device packet
          Total size = 1 + (25 * num_devices)
```

## API Usage

To interact with the OpenTrackDriver API, you can use the provided **TrackerManager** class. This class provides an interface to create and manage trackers, update their poses, and send data to the driver via UDP. Here’s a brief guide on how to use the API.

### Initializing the API

Before using the API, you need to initialize the **TrackerManager**. This is done via the `init()` function, which will open a UDP socket and set up the connection.

```cpp
opentrack::TrackerManager& manager = opentrack::TrackerManager::getInstance();
manager.init("127.0.0.1", 9000); // Default IP and port
```

### Creating a New Tracker

You can create new trackers by providing the **serial number** and **device type**. The serial number should be a unique string of up to 15 characters.

```cpp
std::shared_ptr<opentrack::Tracker> tracker = manager.createTracker("OpenTrackDriver_Waist", opentrack::DeviceType::Tracker);
```

### Updating Tracker Pose

Once a tracker is created, you can update its pose using the `updatePose()` function. This function accepts a `Pose` object that contains position and rotation data.

```cpp
opentrack::Pose pose(opentrack::Vector3(1.0f, 1.5f, 0.5f), opentrack::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
tracker->updatePose(pose);
```

The `Pose` class holds a `Vector3` for the position (X, Y, Z) and a `Quaternion` for the rotation (W, X, Y, Z).

### Getting the Tracker's Pose

You can retrieve the current pose of a tracker by calling the `getPose()` function. This will return the pose that was most recently updated.

```cpp
opentrack::Pose currentPose = tracker->getPose();
```

### Updating HMD or Controller Pose

To update the pose of an HMD or controller, you can use the `updateHMDPose()` or `updateControllerPose()` functions. If the tracker does not exist yet, these functions will create it for you.

```cpp
opentrack::Pose hmdPose(opentrack::Vector3(0.0f, 1.7f, 0.0f), opentrack::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
manager.updateHMDPose(hmdPose);

opentrack::Pose leftControllerPose(opentrack::Vector3(0.5f, 1.5f, 0.2f), opentrack::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
manager.updateControllerPose(true, leftControllerPose); // true for left controller
```

### Sending Batch Updates

If you want to send pose data for multiple devices in one go, you can use the `sendBatchUpdate()` function. This will send all trackers with valid poses to the driver.

```cpp
manager.sendBatchUpdate();
```

## Data Format

### Position (pos)

* Array of 3 float values representing the X, Y, Z coordinates in meters.
* Right-handed coordinate system with the origin at the floor level:

  * X: Positive to the right, negative to the left
  * Y: Positive upward, negative downward
  * Z: Positive forward, negative backward
* Values should be in meters (e.g., 1.7 meters for average human height).

### Rotation (rot)

* Array of 4 float values representing quaternion rotation (W, X, Y, Z).
* The quaternion must be unit length: $W^2 + X^2 + Y^2 + Z^2 = 1$.
* The rotation is represented in ZYX order (yaw, pitch, roll):

  * W: Scalar component
  * X, Y, Z: Rotation components around the respective axes.

### Serial Number

* The serial number is a 16-byte null-terminated string, used to uniquely identify devices.
* For HMD and controllers, the serial number field is ignored.
* The serial number should be alphanumeric and may include underscores, with a maximum length of 15 characters.

## Device Properties

### Trackers

* **Device class:** GenericTracker
* **Wireless:** true
* **Model number:** "OpenTrackDriver"
* **Input components:**

  * `/input/trigger/click` (boolean)
  * `/input/trigger/value` (scalar, normalized one-sided)

### HMD and Controllers

* No additional properties for HMDs and controllers beyond pose data updates.
