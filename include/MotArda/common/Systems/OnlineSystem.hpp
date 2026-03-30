#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <enet/enet.h>
#include <string>

namespace MTRD {

    class OnlineSystem {
    public:
        OnlineSystem();
        OnlineSystem(bool isServer, const std::string& ip, uint16_t port);
        ~OnlineSystem();

        bool Init();
        void PollEvents();
        void Shutdown();

        bool isServer;
        std::string serverIP;
        uint16_t port;

    private:
        ENetHost* netHost_;
        ENetPeer* peer_;
    };
}
