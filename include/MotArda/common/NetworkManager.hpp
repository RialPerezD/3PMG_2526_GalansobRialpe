#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <enet/enet.h>

#include <string>
#include <vector>
#include <cstdint>

namespace MTRD {

    struct NetworkPeer {
        uint32_t networkID;
        ENetPeer* peer;
    };

    class NetworkManager {
    public:
        NetworkManager();
        ~NetworkManager();

        bool InitServer(uint16_t port, uint32_t maxClients = 32);
        bool InitClient(const std::string& serverIP, uint16_t port);
        void Shutdown();

        void PollEvents();

        bool SendPacket(uint32_t targetNetworkID, const void* data, size_t size, bool reliable = true);
        bool BroadcastPacket(const void* data, size_t size, bool reliable = true);

        bool IsServer() const { return isServer_; }
        bool IsConnected() const;

        using PacketCallback = void(*)(uint32_t senderID, const void* data, size_t size);
        void SetReceiveCallback(PacketCallback callback) { receiveCallback_ = callback; }

    private:
        bool isServer_;
        ENetHost* host_;
        ENetPeer* peer_;
        std::vector<NetworkPeer> peers_;
        PacketCallback receiveCallback_;

        uint32_t GenerateNetworkID();
    };
}
