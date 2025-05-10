#include <openvr_driver.h>
#include "driver.h"
#include "tracker_device_driver.h"
#include "tracker_api.h"
#include "tracker_udp_server.h"
#include <memory>
#include <iostream>
#include <cstring>

#ifndef DRIVER_EXPORT
#define DRIVER_EXPORT __attribute__((visibility("default")))
#endif

namespace vr {

MyDeviceProvider::MyDeviceProvider() {
    // Start the UDP server
    if (!TrackerUDPServer::GetInstance().Start()) {
        std::cerr << "Failed to start UDP tracker server" << std::endl;
    }
}

MyDeviceProvider::~MyDeviceProvider() {
    // Stop the UDP server
    TrackerUDPServer::GetInstance().Stop();
}

EVRInitError MyDeviceProvider::Init(IVRDriverContext* pDriverContext) {
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

    // Create and register trackers
    auto waist_tracker = std::make_shared<TrackerDeviceDriver>("OpenTrackServer_Waist", "OpenTrackServer", TrackedDeviceClass_GenericTracker);
    auto left_foot_tracker = std::make_shared<TrackerDeviceDriver>("OpenTrackServer_LeftFoot", "OpenTrackServer", TrackedDeviceClass_GenericTracker);
    auto right_foot_tracker = std::make_shared<TrackerDeviceDriver>("OpenTrackServer_RightFoot", "OpenTrackServer", TrackedDeviceClass_GenericTracker);
    auto left_knee_tracker = std::make_shared<TrackerDeviceDriver>("OpenTrackServer_LeftKnee", "OpenTrackServer", TrackedDeviceClass_GenericTracker);
    auto right_knee_tracker = std::make_shared<TrackerDeviceDriver>("OpenTrackServer_RightKnee", "OpenTrackServer", TrackedDeviceClass_GenericTracker);
    auto left_elbow_tracker = std::make_shared<TrackerDeviceDriver>("OpenTrackServer_LeftElbow", "OpenTrackServer", TrackedDeviceClass_GenericTracker);
    auto right_elbow_tracker = std::make_shared<TrackerDeviceDriver>("OpenTrackServer_RightElbow", "OpenTrackServer", TrackedDeviceClass_GenericTracker);
    auto chest_tracker = std::make_shared<TrackerDeviceDriver>("OpenTrackServer_Chest", "OpenTrackServer", TrackedDeviceClass_GenericTracker);

    // Register trackers with the API
    TrackerAPI::GetInstance().RegisterTracker("OpenTrackServer_Waist", waist_tracker);
    TrackerAPI::GetInstance().RegisterTracker("OpenTrackServer_LeftFoot", left_foot_tracker);
    TrackerAPI::GetInstance().RegisterTracker("OpenTrackServer_RightFoot", right_foot_tracker);
    TrackerAPI::GetInstance().RegisterTracker("OpenTrackServer_LeftKnee", left_knee_tracker);
    TrackerAPI::GetInstance().RegisterTracker("OpenTrackServer_RightKnee", right_knee_tracker);
    TrackerAPI::GetInstance().RegisterTracker("OpenTrackServer_LeftElbow", left_elbow_tracker);
    TrackerAPI::GetInstance().RegisterTracker("OpenTrackServer_RightElbow", right_elbow_tracker);
    TrackerAPI::GetInstance().RegisterTracker("OpenTrackServer_Chest", chest_tracker);

    // Add trackers to the device list
    trackers_.push_back(waist_tracker);
    trackers_.push_back(left_foot_tracker);
    trackers_.push_back(right_foot_tracker);
    trackers_.push_back(left_knee_tracker);
    trackers_.push_back(right_knee_tracker);
    trackers_.push_back(left_elbow_tracker);
    trackers_.push_back(right_elbow_tracker);
    trackers_.push_back(chest_tracker);

    return VRInitError_None;
}

void MyDeviceProvider::Cleanup() {
    // Unregister all trackers
    TrackerAPI::GetInstance().UnregisterTracker("OpenTrackServer_Waist");
    TrackerAPI::GetInstance().UnregisterTracker("OpenTrackServer_LeftFoot");
    TrackerAPI::GetInstance().UnregisterTracker("OpenTrackServer_RightFoot");
    TrackerAPI::GetInstance().UnregisterTracker("OpenTrackServer_LeftKnee");
    TrackerAPI::GetInstance().UnregisterTracker("OpenTrackServer_RightKnee");
    TrackerAPI::GetInstance().UnregisterTracker("OpenTrackServer_LeftElbow");
    TrackerAPI::GetInstance().UnregisterTracker("OpenTrackServer_RightElbow");
    TrackerAPI::GetInstance().UnregisterTracker("OpenTrackServer_Chest");

    trackers_.clear();
}

void MyDeviceProvider::RunFrame() {
    for (auto& tracker : trackers_) {
        tracker->RunFrame();
    }
}

const char* const* MyDeviceProvider::GetInterfaceVersions() {
    return k_InterfaceVersions;
}

bool MyDeviceProvider::ShouldBlockStandbyMode() {
    return false;
}

void MyDeviceProvider::EnterStandby() {
}

void MyDeviceProvider::LeaveStandby() {
}

} // namespace vr

// Entry point for the driver
extern "C" DRIVER_EXPORT void* HmdDriverFactory(const char* pInterfaceName, int* pReturnCode)
{
    if (0 == strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName)) {
        return new vr::MyDeviceProvider();
    }

    if (pReturnCode) {
        *pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
    }
    return nullptr;
} 