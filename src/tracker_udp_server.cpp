#include "tracker_udp_server.h"
#include "tracker_api.h"
#include <cstring>
#include <iostream>
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

namespace vr {

TrackerUDPServer& TrackerUDPServer::GetInstance() {
    static TrackerUDPServer instance;
    return instance;
}

TrackerUDPServer::TrackerUDPServer() {}

TrackerUDPServer::~TrackerUDPServer() {
    Stop();
}

bool TrackerUDPServer::Start(int port) {
    if (running_) return false;
    port_ = port;
    running_ = true;
    server_thread_ = std::make_unique<std::thread>(&TrackerUDPServer::RunServer, this);
    return true;
}

void TrackerUDPServer::Stop() {
    if (!running_) return;
    running_ = false;
    if (server_thread_ && server_thread_->joinable()) {
        server_thread_->join();
    }
}

void TrackerUDPServer::HandlePosePacket(const UdpPosePacket& packet) {
    // Null-terminate serial just in case
    char serial[17];
    strncpy(serial, packet.serial, 16);
    serial[16] = '\0';

    HmdVector3_t pos{packet.pos[0], packet.pos[1], packet.pos[2]};
    HmdQuaternion_t rot{packet.rot[0], packet.rot[1], packet.rot[2], packet.rot[3]};

    switch (packet.device_type) {
        case DeviceType::Tracker:
            TrackerAPI::GetInstance().UpdateTrackerPose(serial, pos, rot);
            break;
        case DeviceType::HMD:
            TrackerAPI::GetInstance().UpdateHMDPose(pos, rot);
            break;
        case DeviceType::LeftController:
            TrackerAPI::GetInstance().UpdateControllerPose(true, pos, rot);
            break;
        case DeviceType::RightController:
            TrackerAPI::GetInstance().UpdateControllerPose(false, pos, rot);
            break;
    }
}

void TrackerUDPServer::RunServer() {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "UDP socket creation failed" << std::endl;
        return;
    }
    sockaddr_in servaddr{};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port_);
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "UDP socket bind failed" << std::endl;
#ifdef _WIN32
        closesocket(sockfd);
        WSACleanup();
#else
        close(sockfd);
#endif
        return;
    }
    std::cout << "UDP server listening on port " << port_ << std::endl;
    while (running_) {
        char buffer[1024]; // Large enough for batch packet
        sockaddr_in cliaddr{};
        socklen_t len = sizeof(cliaddr);
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr, &len);
        
        if (n >= sizeof(UdpPosePacket)) {
            // Check if this is a batch packet
            if (n >= sizeof(UdpBatchPacket)) {
                const UdpBatchPacket* batch = reinterpret_cast<const UdpBatchPacket*>(buffer);
                if (batch->num_devices > 0 && batch->num_devices <= 8) {
                    for (uint8_t i = 0; i < batch->num_devices; ++i) {
                        HandlePosePacket(batch->devices[i]);
                    }
                }
            } else {
                // Single pose packet
                const UdpPosePacket* packet = reinterpret_cast<const UdpPosePacket*>(buffer);
                HandlePosePacket(*packet);
            }
        }
    }
#ifdef _WIN32
    closesocket(sockfd);
    WSACleanup();
#else
    close(sockfd);
#endif
}

} // namespace vr 