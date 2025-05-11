#include "opentrack_api.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        // Initialize the API
        auto& manager = opentrack::TrackerManager::getInstance();
        manager.init();

        // Create trackers using the new TrackerRole enum
        auto waist = manager.createTracker(opentrack::TrackerRole::Waist);
        auto left_foot = manager.createTracker(opentrack::TrackerRole::LeftFoot);
        auto right_foot = manager.createTracker(opentrack::TrackerRole::RightFoot);

        // Update poses
        opentrack::Pose waist_pose(
            opentrack::Vector3(0.0f, 1.0f, 0.0f),  // Position
            opentrack::Quaternion(1.0f, 0.0f, 0.0f, 0.0f)  // Rotation
        );
        waist->updatePose(waist_pose);

        opentrack::Pose left_foot_pose(
            opentrack::Vector3(-0.2f, 0.0f, 0.0f),
            opentrack::Quaternion(1.0f, 0.0f, 0.0f, 0.0f)
        );
        left_foot->updatePose(left_foot_pose);

        opentrack::Pose right_foot_pose(
            opentrack::Vector3(0.2f, 0.0f, 0.0f),
            opentrack::Quaternion(1.0f, 0.0f, 0.0f, 0.0f)
        );
        right_foot->updatePose(right_foot_pose);

        // Update HMD pose
        opentrack::Pose hmd_pose(
            opentrack::Vector3(0.0f, 1.7f, 0.0f),
            opentrack::Quaternion(1.0f, 0.0f, 0.0f, 0.0f)
        );
        manager.updateHMDPose(hmd_pose);

        // Send batch update
        manager.sendBatchUpdate();

        // Wait a bit to ensure data is sent
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
