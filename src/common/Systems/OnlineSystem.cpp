#include "MotArda/common/Systems/OnlineSystem.hpp"
#include <cstdio>

MTRD::OnlineSystem::OnlineSystem()
    : isServer(true), serverIP("127.0.0.1"), port(1234), netHost_(nullptr), peer_(nullptr) {
}

MTRD::OnlineSystem::OnlineSystem(bool server, const std::string& ip, uint16_t p)
    : isServer(server), serverIP(ip), port(p), netHost_(nullptr), peer_(nullptr) {
}

MTRD::OnlineSystem::~OnlineSystem() {
    Shutdown();
}

bool MTRD::OnlineSystem::Init() {
    ENetAddress address;
    address.port = port;

    if (isServer) {
        address.host = ENET_HOST_ANY;
        netHost_ = enet_host_create(&address, 32, 2, 0, 0);
        if (netHost_) printf("Server started on port %u\n", port);
    } else {
        netHost_ = enet_host_create(NULL, 1, 2, 0, 0);
        if (!netHost_) return false;

        enet_address_set_host(&address, serverIP.c_str());

        peer_ = enet_host_connect(netHost_, &address, 2, 0);
        if (!peer_) return false;

        ENetEvent event;
        if (enet_host_service(netHost_, &event, 5000) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT) {
            printf("Connection established\n");
        } else {
            enet_peer_reset(peer_);
            peer_ = nullptr;
            printf("Connection failed: Server unreachable\n");
            return false;
        }
    }
    return netHost_ != nullptr;
}

void MTRD::OnlineSystem::PollEvents() {
    if (!netHost_) return;

    ENetEvent event;
    while (enet_host_service(netHost_, &event, 0) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
            if (isServer) {
                printf("Server: New client connected\n");
            }
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            enet_packet_destroy(event.packet);
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            printf("Disconnection detected\n");
            if (!isServer) peer_ = nullptr;
            break;
        }
    }
}

void MTRD::OnlineSystem::Shutdown() {
    if (netHost_) {
        if (!isServer && peer_) {
            enet_peer_disconnect(peer_, 0);
            ENetEvent event;
            while (enet_host_service(netHost_, &event, 3000) > 0) {
                if (event.type == ENET_EVENT_TYPE_DISCONNECT) break;
            }
        }
        enet_host_destroy(netHost_);
        netHost_ = nullptr;
        peer_ = nullptr;
    }
}
