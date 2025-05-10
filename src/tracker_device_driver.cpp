#include "tracker_device_driver.h"
#include <cstring>

TrackerDeviceDriver::TrackerDeviceDriver(const std::string& serial_number, const std::string& model_number, vr::ETrackedDeviceClass device_class)
    : serial_number_(serial_number)
    , model_number_(model_number)
    , device_class_(device_class)
    , device_index_(vr::k_unTrackedDeviceIndexInvalid)
    , is_active_(false)
    , is_connected_(true)
{
    current_pose_.poseIsValid = true;
    current_pose_.result = vr::TrackingResult_Running_OK;
    current_pose_.deviceIsConnected = true;
    current_pose_.qWorldFromDriverRotation = {1, 0, 0, 0};
    current_pose_.qDriverFromHeadRotation = {1, 0, 0, 0};
    current_pose_.vecWorldFromDriverTranslation[0] = 0.0;
    current_pose_.vecWorldFromDriverTranslation[1] = 0.0;
    current_pose_.vecWorldFromDriverTranslation[2] = 0.0;
    current_pose_.vecDriverFromHeadTranslation[0] = 0.0;
    current_pose_.vecDriverFromHeadTranslation[1] = 0.0;
    current_pose_.vecDriverFromHeadTranslation[2] = 0.0;
}

vr::EVRInitError TrackerDeviceDriver::Activate(uint32_t unObjectId) {
    device_index_ = unObjectId;
    is_active_ = true;

    // Register input components
    vr::VRDriverInput()->CreateBooleanComponent(device_index_, "/input/trigger/click", &input_handles_[TrackerComponent_trigger_click]);
    vr::VRDriverInput()->CreateScalarComponent(device_index_, "/input/trigger/value", &input_handles_[TrackerComponent_trigger_value], vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);

    // Set device properties
    vr::VRProperties()->SetStringProperty(device_index_, vr::Prop_ModelNumber_String, model_number_.c_str());
    vr::VRProperties()->SetStringProperty(device_index_, vr::Prop_SerialNumber_String, serial_number_.c_str());
    vr::VRProperties()->SetInt32Property(device_index_, vr::Prop_DeviceClass_Int32, device_class_);
    vr::VRProperties()->SetBoolProperty(device_index_, vr::Prop_WillDriftInYaw_Bool, false);
    vr::VRProperties()->SetBoolProperty(device_index_, vr::Prop_DeviceIsWireless_Bool, true);
    vr::VRProperties()->SetBoolProperty(device_index_, vr::Prop_DeviceIsCharging_Bool, false);
    vr::VRProperties()->SetFloatProperty(device_index_, vr::Prop_DeviceBatteryPercentage_Float, 1.0f);

    return vr::VRInitError_None;
}

void TrackerDeviceDriver::Deactivate() {
    device_index_ = vr::k_unTrackedDeviceIndexInvalid;
    is_active_ = false;
}

void TrackerDeviceDriver::EnterStandby() {
    // Optional: Implement standby mode
}

void* TrackerDeviceDriver::GetComponent(const char* pchComponentNameAndVersion) {
    return nullptr;
}

void TrackerDeviceDriver::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) {
    if (unResponseBufferSize >= 1)
        pchResponseBuffer[0] = 0;
}

vr::DriverPose_t TrackerDeviceDriver::GetPose() {
    std::lock_guard<std::mutex> lock(pose_mutex_);
    return current_pose_;
}

void TrackerDeviceDriver::UpdatePose(const vr::HmdVector3_t& position, const vr::HmdQuaternion_t& rotation) {
    std::lock_guard<std::mutex> lock(pose_mutex_);
    current_pose_.vecPosition[0] = position.v[0];
    current_pose_.vecPosition[1] = position.v[1];
    current_pose_.vecPosition[2] = position.v[2];
    current_pose_.qRotation = rotation;
}

void TrackerDeviceDriver::RunFrame() {
    if (!is_active_ || !is_connected_)
        return;

    // Update device properties
    vr::VRProperties()->SetBoolProperty(device_index_, vr::Prop_DeviceIsWireless_Bool, is_connected_);
    
    // Update input components if needed
    // vr::VRDriverInput()->UpdateBooleanComponent(input_handles_[TrackerComponent_trigger_click], false, 0.0);
    // vr::VRDriverInput()->UpdateScalarComponent(input_handles_[TrackerComponent_trigger_value], 0.0f, 0.0);
} 