#include <MotArda/Engine.hpp>
#include <MotArda/ObjLoader.hpp>
#include <MotArda/Ecs.hpp>
#include <MotArda/NetworkManager.hpp>
#include <MotArda/Systems/NetworkSystem.hpp>
#include <MotArda/Components/NetworkComponent.hpp>
#include <MotArda/Components/TransformComponent.hpp>
#include <MotArda/Components/RenderComponent.hpp>
#include <MotArda/SimplePacketReciver.hpp>

#include <iostream>
#include <string>
#include <memory>

// CLIENTE

enum class AppState {
    Menu,
    Connecting,
    Running
};

int MTRD::main() {
    static char nickBuffer[64] = "Jugador1";
    static char ipBuffer[64] = "127.0.0.1";
    static int portInput = 1234;

    bool isServer = false;
    AppState currentState = AppState::Menu;

    // --- Create engine ---
    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda Multiplayer");
    if (!maybeEng.has_value()) return 1;
    auto& eng = maybeEng.value();

    // --- Configuracion basica ---
    eng.setDebugMode(true);
    eng.setRenderType(MotardaEng::RenderType::Base);
    eng.getCamera().setPosition(glm::vec3(0, 5, 10));
    eng.getCamera().setTarget(glm::vec3(0, 0, 0));

    // --- Carga de Geometrï¿½a ---
    std::vector<const char*> objsRoutes = { "tableRound.obj",
                                            "86jfmjiufzv2.obj",
                                            "12140_Skull_v3_L2.obj",
                                            "indoor_plant_02.obj" };
    std::atomic<bool> objsLoaded = false;

    std::vector<std::shared_ptr<ObjItem>> objItemList;
    objItemList.push_back(std::make_shared<ObjItem>());

    // --- ECS Setup ---
    ECSManager& ecs = eng.getEcs();
    ecs.AddComponentType<MTRD::NetworkComponent>();
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();

    bool firstTime = true;
    bool meshAssigned = false;

    // TABLE
    size_t table = ecs.AddEntity();
    ecs.AddComponent<MTRD::TransformComponent>(table);
    ecs.AddComponent<MTRD::RenderComponent>(table);

    auto* ttable = ecs.GetComponent<MTRD::TransformComponent>(table);
    ttable->position = glm::vec3(0.0f, 0.0f, 3.0f);
    ttable->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    ttable->angleRotationRadians = -1;
    ttable->scale = glm::vec3(1.0f);

    size_t playerEntity = SIZE_MAX;
    NetworkManager netMgr;
    std::unique_ptr<NetworkSystem> netSys;
    std::unique_ptr<SimplePacketReciver> simplPacRec;

    // async obj load
    eng.enqueueTask([&]() {
        objItemList = eng.loadObjs(objsRoutes);
        objsLoaded.store(true, std::memory_order_release);
        });

    // --- Main window bucle ---
    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();

        if (!objsLoaded.load(std::memory_order_acquire)) {
            eng.windowEndFrame();
            continue;
        }
        else if (firstTime) {
            firstTime = false;
            printf(">>> firstTime ejecutado, meshes: %zu\n", objItemList[0]->meshes.size());
            eng.windowLoadAllMaterials(objItemList);
            auto* rtable = ecs.GetComponent<MTRD::RenderComponent>(table);
            rtable->objitem_ = objItemList[0];
        }

        if (currentState == AppState::Menu) {
            ImGui::SetNextWindowSize(ImVec2(350, 250), ImGuiCond_FirstUseEver);
            ImGui::Begin("Conexion MotArda");

            ImGui::Text("Configuracion de Red");
            ImGui::Separator();
            ImGui::InputText("Nick", nickBuffer, IM_ARRAYSIZE(nickBuffer));
            ImGui::InputText("IP Servidor", ipBuffer, IM_ARRAYSIZE(ipBuffer));
            ImGui::InputInt("Puerto", &portInput);

            ImGui::Spacing();
            if (ImGui::Button("MODO SERVIDOR", ImVec2(150, 40))) {
                if (netMgr.InitServer(static_cast<uint16_t>(portInput), 32)) {
                    isServer = true;
                    currentState = AppState::Running;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("CONECTAR CLIENTE", ImVec2(150, 40))) {
                if (netMgr.InitClient(ipBuffer, static_cast<uint16_t>(portInput))) {
                    isServer = false;
                    currentState = AppState::Connecting;
                }
            }
            ImGui::End();
        }
        else if (currentState == AppState::Running) {
            ImGui::Begin("Info Jugador");
            ImGui::Text("Nick: %s", nickBuffer);
            ImGui::Text("Modo: %s", isServer ? "Servidor" : "Cliente");

            auto* netComp = ecs.GetComponent<MTRD::NetworkComponent>(playerEntity);
            if (netComp) {
                if (netComp->networkID != 0)
                    ImGui::Text("Tu ID de red: %u", netComp->networkID);
                else
                    ImGui::TextDisabled("Esperando ID del servidor...");
            }

            ImGui::End();
        }

        if (currentState == AppState::Connecting) {
            playerEntity = ecs.AddEntity();
            auto* netComp = ecs.AddComponent<MTRD::NetworkComponent>(playerEntity);
            netComp->isLocal = true;

            auto* tplayer = ecs.AddComponent<MTRD::TransformComponent>(playerEntity);
            tplayer->position = glm::vec3(0, 0, 0);
            tplayer->scale = glm::vec3(1.0f);

            // Crear simplPacRec aqui para empezar a recibir la ID del servidor
            simplPacRec = std::make_unique<SimplePacketReciver>(&objItemList, &ecs, playerEntity);

            currentState = AppState::Running;
        }

        if (currentState == AppState::Running) {

            // Wait for the player ID to be received in order to assign the mesh
            if (!meshAssigned && playerEntity != SIZE_MAX) {
                auto* netComp = ecs.GetComponent<MTRD::NetworkComponent>(playerEntity);
                if (netComp && netComp->networkID != 0) {
                    int slot = netComp->networkID - 1;

                    // Hard-coded player positions
                    glm::vec3 slotPositions[4] = {
                        glm::vec3(-5.0f, 0.0f, 3.0f),
                        glm::vec3(5.0f, 0.0f, 3.0f),
                        glm::vec3(-5.0f, 0.0f, 0.0f),
                        glm::vec3(5.0f, 0.0f, 0.0f),
                    };
                    glm::vec3 slotScales[4] = {
                        glm::vec3(0.0003f),
                        glm::vec3(0.07f),
                        glm::vec3(0.1f),
                        glm::vec3(0.1f),
                    };

                    size_t objIdx;
                    if (slot < 3) {
                        objIdx = slot + 1;
                    }
                    else {
                        objIdx = 3;
                    }

                    auto* t = ecs.GetComponent<MTRD::TransformComponent>(playerEntity);
                    if (t) {
                        t->position = slotPositions[slot];
                        t->scale = slotScales[slot];
                    }

                    ecs.AddComponent<MTRD::RenderComponent>(playerEntity);
                    auto* r = ecs.GetComponent<MTRD::RenderComponent>(playerEntity);
                    if (r) {
                        r->objitem_ = objItemList[objIdx];

                    }

                    auto* netComp2 = ecs.GetComponent<MTRD::NetworkComponent>(playerEntity);
                    if (netComp2) {
                        netComp2->meshId_ = static_cast<float>(objIdx);
                    }

                    meshAssigned = true;
                }
            }

            // Polling manual hasta que el netSys esta creado
            if (!netSys) {
                netMgr.PollEvents([&](uint32_t senderID, const void* data, size_t size) {
                    if (simplPacRec) {
                        simplPacRec->OnReceivePacket(senderID, data, size);
                    }
                    });

                // Crear netSys solo cuando ya tenemos el mesh asignado
                if (meshAssigned) {
                    netSys = std::make_unique<NetworkSystem>(ecs, netMgr, std::bind(
                        &MTRD::SimplePacketReciver::OnReceivePacket, simplPacRec.get(),
                        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
                    ));
                }
            }
            else {
                if (!isServer && playerEntity != SIZE_MAX) {
                    auto* transform = ecs.GetComponent<MTRD::TransformComponent>(playerEntity);
                    if (transform) {
                        if (eng.inputIsKeyPressed(Input::Keyboard::W)) transform->position.z -= 0.1f;
                        if (eng.inputIsKeyPressed(Input::Keyboard::S)) transform->position.z += 0.1f;
                    }

                    if (eng.inputIsKeyPressed(Input::Keyboard::C)) {
                        auto* netComp = ecs.GetComponent<MTRD::NetworkComponent>(playerEntity);

                        if (netComp && netComp->networkID != 0) {
                            MTRD::CardPacket packet;
                            packet.header.type = MTRD::MessageType::CardPlay;
                            packet.header.senderId = netComp->networkID;

                            packet.payload.suit = 0;
                            packet.payload.value = 5;

                            netMgr.SendPacket(0, &packet, sizeof(packet), true);

                            printf(">>> Carta enviada: %d de %d\n", packet.payload.value, packet.payload.suit);
                        }
                    }
                }

                glm::vec3 slotScales[4] = {
                    glm::vec3(0.0003f),
                    glm::vec3(0.0003f),
                    glm::vec3(0.07f),
                    glm::vec3(0.1f),
                };

                auto entities = ecs.GetEntitiesWithComponents<MTRD::NetworkComponent, MTRD::TransformComponent>();
                for (size_t entity : entities) {
                    // This IF will skip the current player
                    if (entity == playerEntity) continue;

                    auto* netComp = ecs.GetComponent<MTRD::NetworkComponent>(entity);
                    auto* t = ecs.GetComponent<MTRD::TransformComponent>(entity);
                    if (netComp && t) {
                        size_t meshIdx = static_cast<size_t>(netComp->meshId_);
                        if (meshIdx < 4) {
                            t->scale = slotScales[meshIdx];
                        }
                    }
                }
                netSys->Process();
            }
        }

        eng.renderScene();
        eng.windowEndFrame();
    }

    netMgr.Shutdown();
    return 0;
}