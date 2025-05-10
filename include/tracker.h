#pragma once

#include <openvr_driver.h>
#include <string>
#include <array>

namespace vr {

class TrackerDevice : public vr::ITrackedDeviceServerDriver {
public:
    TrackerDevice(const std::string& serialNumber, const std::string& role);
    virtual ~TrackerDevice();

    // ITrackedDeviceServerDriver methods
    virtual vr::EVRInitError Activate(uint32_t unObjectId) override;
    virtual void Deactivate() override;
    virtual void EnterStandby() override;
    virtual void* GetComponent(const char* pchComponentNameAndVersion) override;
    virtual void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) override;
    virtual vr::DriverPose_t GetPose() override;

    // Custom methods
    void UpdatePose(const vr::DriverPose_t& pose);
    const std::string& GetSerialNumber() const { return m_serialNumber; }
    const std::string& GetRole() const { return m_role; }
    uint32_t GetObjectId() const { return m_objectId; }

private:
    std::string m_serialNumber;
    std::string m_role;
    uint32_t m_objectId;
    vr::DriverPose_t m_pose;
    bool m_isActive;
};

} // namespace vr 