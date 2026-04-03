#include "MotArda/common/NetworkManager.hpp"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <functional>

namespace MTRD {

    NetworkManager::NetworkManager()
        : isServer_(false), host_(nullptr), peer_(nullptr), peers_() {
        static bool enetInitialized = false;
        if (!enetInitialized) {
            if (enet_initialize() != 0) {
                printf("[NetworkManager] Failed to initialize ENet\n");
            } else {
                enetInitialized = true;
                atexit(enet_deinitialize);
            }
        }
    }

    NetworkManager::~NetworkManager() {
        Shutdown();
    }

    bool NetworkManager::InitServer(uint16_t port, uint32_t maxClients) {
        if (maxClients > 10) maxClients = 10;

        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = port;

        host_ = enet_host_create(&address, maxClients, 2, 0, 0);
        if (!host_) {
            printf("[NetworkManager] Failed to create server on port %u\n", port);
            return false;
        }

        isServer_ = true;
        printf("[NetworkManager] Server started on port %u\n", port);
        return true;
    }

    bool NetworkManager::InitClient(const std::string& serverIP, uint16_t port) {
        ENetAddress address;
        address.port = port;

        host_ = enet_host_create(NULL, 1, 2, 0, 0);
        if (!host_) {
            printf("[NetworkManager] Failed to create client host\n");
            return false;
        }

        enet_address_set_host(&address, serverIP.c_str());

        peer_ = enet_host_connect(host_, &address, 2, 0);
        if (!peer_) return false;

        ENetEvent event;
        if (enet_host_service(host_, &event, 5000) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT) {
            isServer_ = false;
            printf("[NetworkManager] Connected to server\n");
            return true;
        } else {
            enet_peer_reset(peer_);
            peer_ = nullptr;
            printf("[NetworkManager] Connection failed: Server unreachable\n");
            return false;
        }
    }

    void NetworkManager::Shutdown() {
        if (host_) {
            if (!isServer_ && peer_) {
                enet_peer_disconnect(peer_, 0);
                ENetEvent event;
                while (enet_host_service(host_, &event, 3000) > 0) {
                    if (event.type == ENET_EVENT_TYPE_DISCONNECT) break;
                }
            }
            enet_host_destroy(host_);
            host_ = nullptr;
            peer_ = nullptr;
        }
        peers_.clear();
    }

    void NetworkManager::PollEvents() {
        PollEvents(nullptr);
    }

    void NetworkManager::PollEvents(std::function<void(uint32_t, const void*, size_t)> callback) {
        if (!host_) return;

        ENetEvent event;
        while (enet_host_service(host_, &event, 0) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                if (isServer_) {
                    if (peers_.size() >= 10) {
                        printf("[NetworkManager] Server full, rejecting client\n");
                        enet_peer_disconnect_now(event.peer, 0);
                    } else {
                        NetworkPeer np;
                        np.networkID = GenerateNetworkID();
                        np.peer = event.peer;
                        peers_.push_back(np);
                        printf("[NetworkManager] Client connected (ID: %u)\n", np.networkID);

                        ENetPacket* packet = enet_packet_create(&np.networkID, sizeof(np.networkID), ENET_PACKET_FLAG_RELIABLE);
                        enet_peer_send(event.peer, 0, packet);

                        for (const auto& p : peers_) {
                            if (p.peer != event.peer) {
                                ENetPacket* broadcastPacket = enet_packet_create(&np.networkID, sizeof(np.networkID), ENET_PACKET_FLAG_RELIABLE);
                                enet_peer_send(p.peer, 0, broadcastPacket);
                            }
                        }

                        if (callback) {
                            callback(np.networkID, nullptr, 0);
                        }
                    }
                }
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                if (callback && event.packet->dataLength >= sizeof(uint32_t)) {
                    uint32_t senderID = 0;
                    if (isServer_) {
                        for (const auto& p : peers_) {
                            if (p.peer == event.peer) {
                                senderID = p.networkID;
                                break;
                            }
                        }
                        if (senderID != 0) {
                            ENetPacket* broadcastPacket = enet_packet_create(event.packet->data, event.packet->dataLength, ENET_PACKET_FLAG_UNSEQUENCED);
                            for (const auto& p : peers_) {
                                if (p.peer != event.peer) {
                                    enet_peer_send(p.peer, 0, broadcastPacket);
                                }
                            }
                        }
                    }
                    callback(senderID, event.packet->data, event.packet->dataLength);
                }
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                if (isServer_) {
                    uint32_t disconnectedID = 0;
                    for (auto it = peers_.begin(); it != peers_.end(); ++it) {
                        if (it->peer == event.peer) {
                            disconnectedID = it->networkID;
                            peers_.erase(it);
                            break;
                        }
                    }
                    printf("[NetworkManager] Client disconnected (ID: %u)\n", disconnectedID);

                    uint32_t disconnectMsg = disconnectedID | 0x80000000;
                    ENetPacket* broadcastPacket = enet_packet_create(&disconnectMsg, sizeof(disconnectMsg), ENET_PACKET_FLAG_RELIABLE);
                    for (const auto& p : peers_) {
                        enet_peer_send(p.peer, 0, broadcastPacket);
                    }

                    if (callback) {
                        callback(disconnectedID | 0x80000000, nullptr, 0);
                    }
                }
                break;
            }
        }
    }

    bool NetworkManager::SendPacket(uint32_t targetNetworkID, const void* data, size_t size, bool reliable) {
        ENetPeer* targetPeer = nullptr;

        if (isServer_) {
            for (const auto& p : peers_) {
                if (p.networkID == targetNetworkID) {
                    targetPeer = p.peer;
                    break;
                }
            }
        } else {
            targetPeer = peer_;
        }

        if (!targetPeer) return false;

        ENetPacket* packet = enet_packet_create(data, size,
            reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED);
        enet_peer_send(targetPeer, 0, packet);
        return true;
    }

    bool NetworkManager::BroadcastPacket(const void* data, size_t size, bool reliable) {
        if (!host_) return false;

        ENetPacket* packet = enet_packet_create(data, size,
            reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED);
        enet_host_broadcast(host_, 0, packet);
        return true;
    }

    bool NetworkManager::IsConnected() const {
        if (isServer_) {
            return host_ != nullptr;
        }
        return host_ != nullptr && peer_ != nullptr;
    }

    void NetworkManager::SendNetworkID(uint32_t networkID) {
        if (isServer_) {
            for (auto& p : peers_) {
                if (p.networkID == networkID && p.peer) {
                    ENetPacket* packet = enet_packet_create(&networkID, sizeof(networkID), ENET_PACKET_FLAG_RELIABLE);
                    enet_peer_send(p.peer, 0, packet);
                    break;
                }
            }
        } else if (peer_) {
            ENetPacket* packet = enet_packet_create(&networkID, sizeof(networkID), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer_, 0, packet);
        }
    }

    uint32_t NetworkManager::GenerateNetworkID() {
        static uint32_t nextID = 1;
        return nextID++;
    }
}
