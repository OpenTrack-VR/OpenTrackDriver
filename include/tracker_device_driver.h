#pragma once

#include <array>
#include <string>
#include <atomic>
#include <thread>
#include <mutex>
#include "openvr_driver.h"

enum TrackerComponent {
    TrackerComponent_trigger_value,
    TrackerComponent_trigger_click,
    TrackerComponent_MAX
};

class TrackerDeviceDriver : public vr::ITrackedDeviceServerDriver {
public:
    TrackerDeviceDriver(const std::string& serial_number, const std::string& model_number, vr::ETrackedDeviceClass device_class);

    vr::EVRInitError Activate(uint32_t unObjectId) override;
    void Deactivate() override;
    void EnterStandby() override;
    void* GetComponent(const char* pchComponentNameAndVersion) override;
    void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) override;
    vr::DriverPose_t GetPose() override;

    void UpdatePose(const vr::HmdVector3_t& position, const vr::HmdQuaternion_t& rotation);
    void RunFrame();

private:
    std::string serial_number_;
    std::string model_number_;
    vr::ETrackedDeviceClass device_class_;
    std::atomic<vr::TrackedDeviceIndex_t> device_index_;
    std::atomic<bool> is_active_;
    std::atomic<bool> is_connected_;
    
    std::array<vr::VRInputComponentHandle_t, TrackerComponent_MAX> input_handles_;
    
    vr::DriverPose_t current_pose_;
    std::mutex pose_mutex_;
}; 