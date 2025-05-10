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
    DeviceType device_type;  // device type
    char serial[16];        // device serial
    float pos[3];          // position xyz
    float rot[4];          // rotation wxyz
};

struct UdpBatchPacket {
    uint8_t num_devices;    // device count
    UdpPosePacket devices[8]; // device batch
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