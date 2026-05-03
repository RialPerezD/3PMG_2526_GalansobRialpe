#include "MotArda/common/Engine.hpp"
#include "MotArda/common/ObjLoader.hpp"
#include "MotArda/common/Ecs.hpp"
#include "MotArda/common/NetworkManager.hpp"
#include "MotArda/common/Systems/NetworkSystem.hpp"
#include "MotArda/common/Components/NetworkComponent.hpp"
#include "MotArda/common/Components/TransformComponent.hpp"
#include "MotArda/common/Components/RenderComponent.hpp"
#include "MotArda/common/SimplePacketReciver.hpp"

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

    // --- Configuración básica ---
    eng.SetDebugMode(true);
    eng.SetRenderType(MotardaEng::RenderType::Base);
    eng.getCamera().setPosition(glm::vec3(0, 5, 10));
    eng.getCamera().setTarget(glm::vec3(0, 0, 0));

    // --- Carga de Geometría ---
    std::vector <const char*> objsRoutes = { "table.obj" };
    std::atomic<bool> objsLoaded = false;

    std::vector<ObjItem> engineGeometries;
    engineGeometries.push_back(std::move(eng.generateCube(1)));
    eng.windowLoadAllMaterials(engineGeometries);

    std::vector<ObjItem> objItemList;
    objItemList.push_back(ObjItem());


    // --- ECS Setup ---
    ECSManager& ecs = eng.getEcs();
    ecs.AddComponentType<MTRD::NetworkComponent>();
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();

    size_t table = ecs.AddEntity();

    bool firstTime = true;

    MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(table);
    t->position = glm::vec3(0.0f);
    t->rotation = glm::vec3(1.0f, 0.0f, 0.0f);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(0.05f);

    /*MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(table);*/
    MTRD::RenderComponent* r = nullptr;

    size_t cube = ecs.AddEntity();

    MTRD::TransformComponent* ct = ecs.AddComponent<MTRD::TransformComponent>(cube);
    ct->position = glm::vec3(0.0f, 0.0f, 0.0f);
    ct->rotation = glm::vec3(1.0f, 0.0f, 0.0f);
    ct->angleRotationRadians = -1;
    ct->scale = glm::vec3(1.0f);

    MTRD::RenderComponent* cr = ecs.AddComponent<MTRD::RenderComponent>(cube);

    cr->meshes_ = &engineGeometries[0].meshes;
    cr->materials_ = &engineGeometries[0].materials;

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

            printf("ObjList size: %zu\n", objItemList.size());
            if (!objItemList.empty()) {
                printf("Meshes en [0]: %zu\n", objItemList[0].meshes.size());
            }

            eng.windowLoadAllMaterials(objItemList);

            r = ecs.AddComponent<MTRD::RenderComponent>(table);

            r->meshes_ = &objItemList[0].meshes;
            r->materials_ = &objItemList[0].materials;
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
            ImGui::End();
        }

        if (currentState == AppState::Connecting) {
            playerEntity = ecs.AddEntity();
            auto* netComp = ecs.AddComponent<MTRD::NetworkComponent>(playerEntity);
            netComp->isLocal = true;

            auto* trans = ecs.AddComponent<MTRD::TransformComponent>(playerEntity);
            trans->position = glm::vec3(0, 0, 0);
            trans->scale = glm::vec3(1.0f);

            auto* rend = ecs.AddComponent<MTRD::RenderComponent>(playerEntity);
            rend->meshes_ = &engineGeometries[0].meshes;
            rend->materials_ = &engineGeometries[0].materials;

            currentState = AppState::Running;
        }

        if (currentState == AppState::Running) {
            if (!netSys) {
                simplPacRec = std::make_unique<SimplePacketReciver>(&engineGeometries, &ecs, playerEntity);
                netSys = std::make_unique<NetworkSystem>(ecs, netMgr, std::bind(
                    &MTRD::SimplePacketReciver::OnReceivePacket, simplPacRec.get(),
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
                ));
            }

            if (!isServer && playerEntity != SIZE_MAX) {
                auto* transform = ecs.GetComponent<MTRD::TransformComponent>(playerEntity);
                if (transform) {
                    if (eng.inputIsKeyPressed(Input::Keyboard::W)) transform->position.z -= 0.1f;
                    if (eng.inputIsKeyPressed(Input::Keyboard::S)) transform->position.z += 0.1f;
                }
            }
            netSys->Process();
        }

        eng.RenderScene();
        eng.windowEndFrame();
    }

    netMgr.Shutdown();
    return 0;
}