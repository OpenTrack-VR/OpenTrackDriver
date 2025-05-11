#pragma once

#include <string>
#include <array>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <functional>
#include <mutex>
#include <queue>
#include <atomic>
#include <cmath>       // For std::sqrt
#include <map>         // For std::map
#include <vector>      // For std::vector
#include <algorithm>   // For std::min

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

namespace opentrack {

// Constants
constexpr int DEFAULT_PORT = 9000;
constexpr size_t MAX_SERIAL_LENGTH = 15;
constexpr size_t MAX_BATCH_SIZE = 8;
constexpr size_t PACKET_SIZE = 45;

// Device types
enum class DeviceType : uint8_t {
    Tracker = 0,
    HMD = 1,
    LeftController = 2,
    RightController = 3
};

// Vector3 for position
struct Vector3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vector3() = default;
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
};

// Quaternion for rotation
struct Quaternion {
    float w = 1.0f;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Quaternion() = default;
    Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

    // Normalize the quaternion
    void normalize() {
        float len = std::sqrt(w * w + x * x + y * y + z * z);
        if (len > 0.0001f) {
            w /= len;
            x /= len;
            y /= len;
            z /= len;
        }
    }
};

// Pose data
struct Pose {
    Vector3 position;
    Quaternion rotation;

    Pose() = default;
    Pose(const Vector3& pos, const Quaternion& rot) : position(pos), rotation(rot) {}
};

// Tracker class for individual devices
class Tracker {
public:
    Tracker(const std::string& serial, DeviceType type)
        : serial_(serial), type_(type) {
        if (serial.length() > MAX_SERIAL_LENGTH) {
            throw std::invalid_argument("Serial number too long");
        }
    }

    // Update pose
    void updatePose(const Pose& pose) {
        std::lock_guard<std::mutex> lock(mutex_);
        Pose normalized_pose = pose;
        normalized_pose.rotation.normalize();
        current_pose_ = normalized_pose;
        has_pose_ = true;
    }

    // Get current pose
    Pose getPose() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return current_pose_;
    }

    // Check if we have valid pose data
    bool hasPose() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return has_pose_;
    }

    // Get serial number
    const std::string& getSerial() const { return serial_; }

    // Get device type
    DeviceType getType() const { return type_; }

private:
    std::string serial_;
    DeviceType type_;
    Pose current_pose_;
    bool has_pose_ = false;
    mutable std::mutex mutex_;
};

// Main API class
class TrackerManager {
public:
    static TrackerManager& getInstance() {
        static TrackerManager instance;
        return instance;
    }

    void init(const std::string& host = "127.0.0.1", int port = DEFAULT_PORT) {
        if (initialized_) return;

#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("Failed to initialize Winsock");
        }
#endif

        sock_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_ < 0) {
            throw std::runtime_error("Failed to create socket");
        }

        server_addr_.sin_family = AF_INET;
        server_addr_.sin_port = htons(port);
        inet_pton(AF_INET, host.c_str(), &server_addr_.sin_addr);

        initialized_ = true;
    }

    ~TrackerManager() {
        if (initialized_) {
#ifdef _WIN32
            closesocket(sock_);
            WSACleanup();
#else
            close(sock_);
#endif
        }
    }

    std::shared_ptr<Tracker> createTracker(const std::string& serial, DeviceType type) {
        auto tracker = std::make_shared<Tracker>(serial, type);
        trackers_[serial] = tracker;
        return tracker;
    }

    std::shared_ptr<Tracker> getTracker(const std::string& serial) {
        auto it = trackers_.find(serial);
        return it != trackers_.end() ? it->second : nullptr;
    }

    void updateTrackerPose(const std::string& serial, const Pose& pose) {
        auto tracker = getTracker(serial);
        if (tracker) {
            tracker->updatePose(pose);
            sendPose(tracker);
        }
    }

    void updateHMDPose(const Pose& pose) {
        auto hmd = getTracker("HMD");
        if (!hmd) {
            hmd = createTracker("HMD", DeviceType::HMD);
        }
        hmd->updatePose(pose);
        sendPose(hmd);
    }

    void updateControllerPose(bool isLeft, const Pose& pose) {
        const std::string serial = isLeft ? "LeftController" : "RightController";
        auto controller = getTracker(serial);
        if (!controller) {
            controller = createTracker(serial, isLeft ? DeviceType::LeftController : DeviceType::RightController);
        }
        controller->updatePose(pose);
        sendPose(controller);
    }

    void sendBatchUpdate() {
        if (trackers_.empty()) return;

        std::vector<std::shared_ptr<Tracker>> active_trackers;
        for (const auto& pair : trackers_) {
            if (pair.second->hasPose()) {
                active_trackers.push_back(pair.second);
            }
        }

        if (active_trackers.empty()) return;

        std::vector<uint8_t> packet(1 + (PACKET_SIZE * active_trackers.size()));
        packet[0] = static_cast<uint8_t>(active_trackers.size());

        for (size_t i = 0; i < active_trackers.size(); ++i) {
            const auto& tracker = active_trackers[i];
            const auto& pose = tracker->getPose();
            size_t offset = 1 + (i * PACKET_SIZE);

            // Device type
            packet[offset] = static_cast<uint8_t>(tracker->getType());

            // Serial number
            std::memset(&packet[offset + 1], 0, MAX_SERIAL_LENGTH);
            const std::string& serial = tracker->getSerial();
            std::memcpy(&packet[offset + 1], serial.c_str(), std::min(serial.length(), MAX_SERIAL_LENGTH));

            // Position
            std::memcpy(&packet[offset + 17], &pose.position.x, sizeof(float));
            std::memcpy(&packet[offset + 21], &pose.position.y, sizeof(float));
            std::memcpy(&packet[offset + 25], &pose.position.z, sizeof(float));

            // Rotation
            std::memcpy(&packet[offset + 29], &pose.rotation.w, sizeof(float));
            std::memcpy(&packet[offset + 33], &pose.rotation.x, sizeof(float));
            std::memcpy(&packet[offset + 37], &pose.rotation.y, sizeof(float));
            std::memcpy(&packet[offset + 41], &pose.rotation.z, sizeof(float));
        }

        sendto(sock_, reinterpret_cast<char*>(packet.data()), packet.size(), 0,
               reinterpret_cast<sockaddr*>(&server_addr_), sizeof(server_addr_));
    }

private:
    TrackerManager() = default;
    TrackerManager(const TrackerManager&) = delete;
    TrackerManager& operator=(const TrackerManager&) = delete;

    void sendPose(const std::shared_ptr<Tracker>& tracker) {
        if (!tracker || !tracker->hasPose()) return;

        const auto& pose = tracker->getPose();
        std::array<uint8_t, PACKET_SIZE> packet;

        packet[0] = static_cast<uint8_t>(tracker->getType());

        std::memset(&packet[1], 0, MAX_SERIAL_LENGTH);
        const std::string& serial = tracker->getSerial();
        std::memcpy(&packet[1], serial.c_str(), std::min(serial.length(), MAX_SERIAL_LENGTH));

        std::memcpy(&packet[17], &pose.position.x, sizeof(float));
        std::memcpy(&packet[21], &pose.position.y, sizeof(float));
        std::memcpy(&packet[25], &pose.position.z, sizeof(float));

        std::memcpy(&packet[29], &pose.rotation.w, sizeof(float));
        std::memcpy(&packet[33], &pose.rotation.x, sizeof(float));
        std::memcpy(&packet[37], &pose.rotation.y, sizeof(float));
        std::memcpy(&packet[41], &pose.rotation.z, sizeof(float));

        sendto(sock_, reinterpret_cast<char*>(packet.data()), packet.size(), 0,
               reinterpret_cast<sockaddr*>(&server_addr_), sizeof(server_addr_));
    }

    std::map<std::string, std::shared_ptr<Tracker>> trackers_;
    int sock_ = -1;
    sockaddr_in server_addr_{};
    bool initialized_ = false;
};

} // namespace opentrack
