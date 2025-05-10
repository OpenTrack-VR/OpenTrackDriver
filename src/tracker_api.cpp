#include "tracker_api.h"
#include "tracker_device_driver.h"

namespace vr {

TrackerAPI& TrackerAPI::GetInstance() {
    static TrackerAPI instance;
    return instance;
}

void TrackerAPI::RegisterTracker(const std::string& serial_number, std::shared_ptr<TrackerDeviceDriver> tracker) {
    std::lock_guard<std::mutex> lock(trackers_mutex_);
    trackers_[serial_number] = tracker;
}

void TrackerAPI::UnregisterTracker(const std::string& serial_number) {
    std::lock_guard<std::mutex> lock(trackers_mutex_);
    trackers_.erase(serial_number);
}

bool TrackerAPI::UpdateTrackerPose(const std::string& serial_number, 
                                 const HmdVector3_t& position, 
                                 const HmdQuaternion_t& rotation) {
    std::lock_guard<std::mutex> lock(trackers_mutex_);
    auto it = trackers_.find(serial_number);
    if (it != trackers_.end()) {
        it->second->UpdatePose(position, rotation);
        return true;
    }
    return false;
}

DevicePose TrackerAPI::GetHMDPose() const {
    std::lock_guard<std::mutex> lock(device_poses_mutex_);
    return hmd_pose_;
}

DevicePose TrackerAPI::GetControllerPose(bool is_left) const {
    std::lock_guard<std::mutex> lock(device_poses_mutex_);
    return is_left ? left_controller_pose_ : right_controller_pose_;
}

void TrackerAPI::UpdateHMDPose(const HmdVector3_t& position, const HmdQuaternion_t& rotation) {
    std::lock_guard<std::mutex> lock(device_poses_mutex_);
    hmd_pose_.position = position;
    hmd_pose_.rotation = rotation;
    hmd_pose_.is_valid = true;
}

void TrackerAPI::UpdateControllerPose(bool is_left, const HmdVector3_t& position, const HmdQuaternion_t& rotation) {
    std::lock_guard<std::mutex> lock(device_poses_mutex_);
    if (is_left) {
        left_controller_pose_.position = position;
        left_controller_pose_.rotation = rotation;
        left_controller_pose_.is_valid = true;
    } else {
        right_controller_pose_.position = position;
        right_controller_pose_.rotation = rotation;
        right_controller_pose_.is_valid = true;
    }
}

} // namespace vr 