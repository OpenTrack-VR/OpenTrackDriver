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

// device provider
class MyDeviceProvider : public IServerTrackedDeviceProvider {
public:
    MyDeviceProvider() {
        // start server
        if (!TrackerUDPServer::GetInstance().Start()) {
            std::cerr << "Failed to start UDP tracker server" << std::endl;
        }
    }

    ~MyDeviceProvider() {
        // stop server
        TrackerUDPServer::GetInstance().Stop();
    }

    EVRInitError Init(IVRDriverContext* pDriverContext) override {
        VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
        
        // create trackers
        auto waist_tracker = std::make_shared<TrackerDeviceDriver>("OpenTrackServer_Waist", "OpenTrackServer", TrackedDeviceClass_GenericTracker);
        auto left_foot_tracker = std::make_shared<TrackerDeviceDriver>("OpenTrackServer_LeftFoot", "OpenTrackServer", TrackedDeviceClass_GenericTracker);
        auto right_foot_tracker = std::make_shared<TrackerDeviceDriver>("OpenTrackServer_RightFoot", "OpenTrackServer", TrackedDeviceClass_GenericTracker);
        auto left_knee_tracker = std::make_shared<TrackerDeviceDriver>("OpenTrackServer_LeftKnee", "OpenTrackServer", TrackedDeviceClass_GenericTracker);
        auto right_knee_tracker = std::make_shared<TrackerDeviceDriver>("OpenTrackServer_RightKnee", "OpenTrackServer", TrackedDeviceClass_GenericTracker);
        auto left_elbow_tracker = std::make_shared<TrackerDeviceDriver>("OpenTrackServer_LeftElbow", "OpenTrackServer", TrackedDeviceClass_GenericTracker);
        auto right_elbow_tracker = std::make_shared<TrackerDeviceDriver>("OpenTrackServer_RightElbow", "OpenTrackServer", TrackedDeviceClass_GenericTracker);
        auto chest_tracker = std::make_shared<TrackerDeviceDriver>("OpenTrackServer_Chest", "OpenTrackServer", TrackedDeviceClass_GenericTracker);

        // register trackers
        TrackerAPI::GetInstance().RegisterTracker("OpenTrackServer_Waist", waist_tracker);
        TrackerAPI::GetInstance().RegisterTracker("OpenTrackServer_LeftFoot", left_foot_tracker);
        TrackerAPI::GetInstance().RegisterTracker("OpenTrackServer_RightFoot", right_foot_tracker);
        TrackerAPI::GetInstance().RegisterTracker("OpenTrackServer_LeftKnee", left_knee_tracker);
        TrackerAPI::GetInstance().RegisterTracker("OpenTrackServer_RightKnee", right_knee_tracker);
        TrackerAPI::GetInstance().RegisterTracker("OpenTrackServer_LeftElbow", left_elbow_tracker);
        TrackerAPI::GetInstance().RegisterTracker("OpenTrackServer_RightElbow", right_elbow_tracker);
        TrackerAPI::GetInstance().RegisterTracker("OpenTrackServer_Chest", chest_tracker);

        // add trackers
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

    void Cleanup() override {
        // unregister trackers
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

    void RunFrame() override {
        // update trackers
        for (auto& tracker : trackers_) {
            tracker->RunFrame();
        }
    }

    const char* const* GetInterfaceVersions() override {
        return k_InterfaceVersions;
    }

    bool ShouldBlockStandbyMode() override { return false; }
    void EnterStandby() override {}
    void LeaveStandby() override {}

private:
    std::vector<std::shared_ptr<TrackerDeviceDriver>> trackers_;
};

} // namespace vr

// driver entry
extern "C" DRIVER_EXPORT void* HmdDriverFactory(const char* pInterfaceName, int* pReturnCode) {
    if (0 == strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName)) {
        return new vr::MyDeviceProvider();
    }

    if (pReturnCode) {
        *pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
    }
    return nullptr;
} 