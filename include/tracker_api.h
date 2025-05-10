#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <openvr_driver.h>
#include "tracker_device_driver.h"

namespace vr {

struct DevicePose {
    HmdVector3_t position;
    HmdQuaternion_t rotation;
    bool is_valid;
};

class TrackerAPI {
public:
    static TrackerAPI& GetInstance();

    // Register a tracker with the API
    void RegisterTracker(const std::string& serial_number, std::shared_ptr<class TrackerDeviceDriver> tracker);
    
    // Unregister a tracker
    void UnregisterTracker(const std::string& serial_number);
    
    // Update tracker pose
    bool UpdateTrackerPose(const std::string& serial_number, 
                          const HmdVector3_t& position, 
                          const HmdQuaternion_t& rotation);

    // Get HMD pose
    DevicePose GetHMDPose() const;
    
    // Get controller pose
    DevicePose GetControllerPose(bool is_left) const;
    
    // Update HMD pose
    void UpdateHMDPose(const HmdVector3_t& position, const HmdQuaternion_t& rotation);
    
    // Update controller pose
    void UpdateControllerPose(bool is_left, const HmdVector3_t& position, const HmdQuaternion_t& rotation);

private:
    TrackerAPI() = default;
    ~TrackerAPI() = default;
    TrackerAPI(const TrackerAPI&) = delete;
    TrackerAPI& operator=(const TrackerAPI&) = delete;

    std::unordered_map<std::string, std::shared_ptr<class TrackerDeviceDriver>> trackers_;
    std::mutex trackers_mutex_;
    
    DevicePose hmd_pose_{};
    DevicePose left_controller_pose_{};
    DevicePose right_controller_pose_{};
    mutable std::mutex device_poses_mutex_;
};

} // namespace vr 