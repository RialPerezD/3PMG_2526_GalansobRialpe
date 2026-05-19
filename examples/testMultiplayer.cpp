#include <MotArda/Engine.hpp>
#include <MotArda/Logger.hpp>
#include <MotArda/ObjLoader.hpp>
#include <MotArda/Ecs.hpp>
#include <MotArda/NetworkManager.hpp>
#include <MotArda/Systems/NetworkSystem.hpp>
#include <MotArda/Components/NetworkComponent.hpp>
#include <MotArda/Components/TransformComponent.hpp>
#include <MotArda/Components/RenderComponent.hpp>
#include <MotArda/SimplePacketReceiver.hpp>

#include <cstdlib>
#include <ctime>
#include <map>
#include <functional>
#include <deque>
#include <string>

int MTRD::main() {
    constexpr bool IS_SERVER = true;
    constexpr uint16_t PORT = 1234;
    constexpr const char* SERVER_IP = "127.0.0.1";
    constexpr size_t meshIdSelector = 0;

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    auto maybeEng = MotardaEng::createEngine(800, 600, "MotArda Multiplayer");
    if (!maybeEng.has_value()) return 1;
    auto& eng = maybeEng.value();


    eng.setDebugMode(true);
    eng.setRenderType(MotardaEng::RenderType::Base);

    Camera& camera = eng.getCamera();
    camera.setPosition(glm::vec3(0, 5, 10));
    camera.setTarget(glm::vec3(0, 0, 0));

    std::vector<std::shared_ptr<ObjItem>> objItemList;
    objItemList.push_back(eng.generateCube(1));
    objItemList.push_back(eng.generateSphere(0.5f, 20, 20));
    objItemList.push_back(eng.generateSphere(0.5f, 20, 20, 1));
    eng.windowLoadAllMaterials(objItemList);

    ECSManager& ecs = eng.getEcs();
    ecs.AddComponentType<MTRD::NetworkComponent>();
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();

    size_t player = SIZE_MAX;

    std::deque<std::string> chatMessages;
    chatMessages.push_back("Welcome to the chat!");

    if (!IS_SERVER) {
        player = ecs.AddEntity();

        float randomX = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 6.0f)) - 3.0f;
        float randomZ = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 6.0f)) - 3.0f;

        auto* netComp = ecs.AddComponent<MTRD::NetworkComponent>(player);
        netComp->networkID = 0;
        netComp->meshId_ = meshIdSelector;
        netComp->isLocal = true;

        auto* transform = ecs.AddComponent<MTRD::TransformComponent>(player);
        transform->position = glm::vec3(randomX, 0, randomZ);
        transform->rotation = glm::vec3(0, 0, 0);
        transform->angleRotationRadians = 0;
        transform->scale = glm::vec3(1.f);

        auto* render = ecs.AddComponent<MTRD::RenderComponent>(player);
        assert(meshIdSelector < objItemList.size());
        render->objitem_ = objItemList[meshIdSelector];

        MTRD::Logger::info("Player created at ({:.2f}, {:.2f}), waiting for networkID...",
            randomX, randomZ);
    }

    SimplePacketReceiver simplPacRec(&objItemList, &ecs, player);

    NetworkManager netMgr;
    if (IS_SERVER) {
        if (!netMgr.InitServer(PORT, 32)) return 1;
    } else {
        if (!netMgr.InitClient(SERVER_IP, PORT)) return 1;
    }

    NetworkSystem netSys(ecs, netMgr, std::bind(&MTRD::SimplePacketReceiver::OnReceivePacket, &simplPacRec, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    netSys.SetChatCallback([&chatMessages](uint32_t senderId, const MTRD::ChatPayload& payload) {
        std::string fullMsg = "Player " + std::to_string(senderId) + ": " + payload.text;

        chatMessages.push_back(fullMsg);
        if (chatMessages.size() > 20) chatMessages.pop_front();
        });

    static char tempBuffer[256] = "";
    while (!eng.windowShouldClose()) {
        eng.windowInitFrame();

        if constexpr (!IS_SERVER && player != SIZE_MAX) {
            auto* transform = ecs.GetComponent<MTRD::TransformComponent>(player);
            if (transform) {
                if (eng.inputIsKeyPressed(Input::Keyboard::W)) transform->position.z -= 0.1f;
                if (eng.inputIsKeyPressed(Input::Keyboard::S)) transform->position.z += 0.1f;
                if (eng.inputIsKeyPressed(Input::Keyboard::A)) transform->position.x -= 0.1f;
                if (eng.inputIsKeyPressed(Input::Keyboard::D)) transform->position.x += 0.1f;
            }
        }

        ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_NoCollapse);
        ImGui::BeginChild("ChatMessages", ImVec2(0, -30), true);
        for (const auto& msg : chatMessages) ImGui::TextWrapped("%s", msg.c_str());
        if (!chatMessages.empty()) ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();

        ImGui::PushItemWidth(-60);
        if (ImGui::InputText("Message", tempBuffer, IM_ARRAYSIZE(tempBuffer), ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::Button("Send")) {
            if (tempBuffer[0] != '\0') {
                struct FullChatPacket {
                    NetMessage header;
                    ChatPayload payload;
                } packet;

                uint32_t myID = 0;
                if (!IS_SERVER) {
                    auto* localNetComp = ecs.GetComponent<MTRD::NetworkComponent>(player);
                    if (localNetComp) myID = localNetComp->networkID;
                }

                packet.header.type = MessageType::Chat;
                packet.header.senderId = myID;
                strncpy_s(packet.payload.text, tempBuffer, 255);
                packet.payload.text[255] = '\0';

                if (IS_SERVER) {
                    chatMessages.push_back("Server: " + std::string(tempBuffer));
                    netMgr.BroadcastPacket(&packet, sizeof(packet), true);
                } else {
                    netMgr.SendPacket(0, &packet, sizeof(packet), true);
                    chatMessages.push_back("Me: " + std::string(tempBuffer));
                }

                if (chatMessages.size() > 20) chatMessages.pop_front();
                memset(tempBuffer, 0, sizeof(tempBuffer));
            }
        }
        ImGui::PopItemWidth();
        ImGui::End();

        netSys.Process();
        eng.renderScene();
        eng.windowEndFrame();
    }

    eng.endDebugger();
    netMgr.Shutdown();
    return 0;
}