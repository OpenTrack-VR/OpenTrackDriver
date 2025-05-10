#pragma once

#include <vector>
#include <memory>
#include <openvr_driver.h>
#include "tracker_device_driver.h"

namespace vr {

class MyDeviceProvider : public IServerTrackedDeviceProvider {
public:
    MyDeviceProvider();
    ~MyDeviceProvider();

    EVRInitError Init(IVRDriverContext* pDriverContext) override;
    void Cleanup() override;
    const char* const* GetInterfaceVersions() override;
    void RunFrame() override;
    bool ShouldBlockStandbyMode() override;
    void EnterStandby() override;
    void LeaveStandby() override;

private:
    std::vector<std::shared_ptr<TrackerDeviceDriver>> trackers_;
};

} // namespace vr 