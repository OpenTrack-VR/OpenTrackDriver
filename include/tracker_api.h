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

    // register tracker
    void RegisterTracker(const std::string& serial_number, std::shared_ptr<class TrackerDeviceDriver> tracker);
    
    // unregister tracker
    void UnregisterTracker(const std::string& serial_number);
    
    // update pose
    bool UpdateTrackerPose(const std::string& serial_number, 
                          const HmdVector3_t& position, 
                          const HmdQuaternion_t& rotation);

    // get hmd
    DevicePose GetHMDPose() const;
    
    // get controller
    DevicePose GetControllerPose(bool is_left) const;
    
    // update hmd
    void UpdateHMDPose(const HmdVector3_t& position, const HmdQuaternion_t& rotation);
    
    // update controller
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