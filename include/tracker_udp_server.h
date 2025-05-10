#pragma once

#include <thread>
#include <atomic>
#include <string>

namespace vr {

enum class DeviceType : uint8_t {
    Tracker = 0,
    HMD = 1,
    LeftController = 2,
    RightController = 3
};

#pragma pack(push, 1)
struct UdpPosePacket {
    DeviceType device_type;  // Type of device (1 byte)
    char serial[16];        // Device serial (16 bytes)
    float pos[3];          // Position x,y,z (12 bytes)
    float rot[4];          // Rotation w,x,y,z (16 bytes)
};

struct UdpBatchPacket {
    uint8_t num_devices;    // Number of devices in batch (1 byte)
    UdpPosePacket devices[8]; // Up to 8 devices per batch
};
#pragma pack(pop)

class TrackerUDPServer {
public:
    static TrackerUDPServer& GetInstance();
    bool Start(int port = 9000);
    void Stop();
    ~TrackerUDPServer();
private:
    TrackerUDPServer();
    void RunServer();
    void HandlePosePacket(const UdpPosePacket& packet);
    std::unique_ptr<std::thread> server_thread_;
    std::atomic<bool> running_{false};
    int port_ = 9000;
};

} // namespace vr 