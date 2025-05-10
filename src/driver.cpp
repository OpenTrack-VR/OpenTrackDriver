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
    MyDeviceProvider() : m_pTrackerServer(nullptr) {}

    EVRInitError Init(IVRDriverContext* pDriverContext) override {
        VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
        
        // init server
        m_pTrackerServer = std::make_unique<TrackerUDPServer>();
        if (!m_pTrackerServer->Initialize()) {
            return VRInitError_Driver_Failed;
        }

        // add devices
        AddDevices();
        
        return VRInitError_None;
    }

    void Cleanup() override {
        // cleanup server
        if (m_pTrackerServer) {
            m_pTrackerServer->Shutdown();
            m_pTrackerServer.reset();
        }
    }

    const char* const* GetInterfaceVersions() override {
        return k_InterfaceVersions;
    }

    void RunFrame() override {
        // update devices
        if (m_pTrackerServer) {
            m_pTrackerServer->Update();
        }
    }

    bool ShouldBlockStandbyMode() override { return false; }
    void EnterStandby() override {}
    void LeaveStandby() override {}

private:
    void AddDevices() {
        // add hmd
        auto hmd = std::make_shared<TrackerDeviceDriver>(k_unTrackedDeviceIndex_Hmd);
        VRServerDriverHost()->TrackedDeviceAdded(hmd->GetSerialNumber().c_str(), TrackedDeviceClass_HMD, hmd.get());
        m_vecTrackedDevices.push_back(hmd);

        // add controllers
        auto leftController = std::make_shared<TrackerDeviceDriver>(k_unTrackedDeviceIndex_Controller_Left);
        VRServerDriverHost()->TrackedDeviceAdded(leftController->GetSerialNumber().c_str(), TrackedDeviceClass_Controller, leftController.get());
        m_vecTrackedDevices.push_back(leftController);

        auto rightController = std::make_shared<TrackerDeviceDriver>(k_unTrackedDeviceIndex_Controller_Right);
        VRServerDriverHost()->TrackedDeviceAdded(rightController->GetSerialNumber().c_str(), TrackedDeviceClass_Controller, rightController.get());
        m_vecTrackedDevices.push_back(rightController);

        // add trackers
        for (int i = 0; i < 5; i++) {
            auto tracker = std::make_shared<TrackerDeviceDriver>(k_unTrackedDeviceIndex_Other_Start + i);
            VRServerDriverHost()->TrackedDeviceAdded(tracker->GetSerialNumber().c_str(), TrackedDeviceClass_GenericTracker, tracker.get());
            m_vecTrackedDevices.push_back(tracker);
        }
    }

    std::unique_ptr<TrackerUDPServer> m_pTrackerServer;
    std::vector<std::shared_ptr<TrackerDeviceDriver>> m_vecTrackedDevices;
};

// driver entry
HMD_DLL_EXPORT void* HmdDriverFactory(const char* pInterfaceName, int* pReturnCode) {
    if (0 == strcmp(IServerTrackedDeviceProvider_Version, pInterfaceName)) {
        return new MyDeviceProvider();
    }
    if (pReturnCode) {
        *pReturnCode = VRInitError_Init_InterfaceNotFound;
    }
    return nullptr;
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