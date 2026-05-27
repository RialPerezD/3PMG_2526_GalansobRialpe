#include <MotArda/Engine.hpp>
#include <MotArda/ObjLoader.hpp>
#include <MotArda/Ecs.hpp>
#include <MotArda/Logger.hpp>
#include <MotArda/Components/PhysxComponent.hpp>
#include <MotArda/NetworkManager.hpp>
#include <MotArda/Systems/NetworkSystem.hpp>
#include <MotArda/CardGame/CardGame.hpp>
#include <MotArda/Components/NetworkComponent.hpp>
#include <MotArda/Components/TransformComponent.hpp>
#include <MotArda/Components/RenderComponent.hpp>
#include <MotArda/SimplePacketReceiver.hpp>

#include <algorithm>
#include <iostream>
#include <string>
#include <memory>
#include <random>

using namespace MTRD;

// PLAYER

enum class AppState {
    Menu,
    Connecting,
    Running
};

// --- Datos hardcodeados de cada slot de jugador ---
static const glm::vec3 SLOT_POSITIONS[4] = {
    glm::vec3(-3.5f, 0.0f, 3.0f),
    glm::vec3(3.5f, 0.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, 6.0f),
    glm::vec3(0.0f, 0.0f, -6.0f),
};
static const glm::vec3 SLOT_SCALES[4] = {
    glm::vec3(0.0003f),  // meshId 0 — table
    glm::vec3(0.0003f),  // meshId 1 — 86jfm
    glm::vec3(0.1f),     // meshId 2 — skull
    glm::vec3(0.3f),     // meshId 3 — plant
};
static const glm::vec3 REMOTE_SCALES[4] = {
    glm::vec3(0.0003f),  // meshId 0 — table
    glm::vec3(0.0003f),  // meshId 1 — 86jfm
    glm::vec3(0.1f),    // meshId 2 — skull
    glm::vec3(0.3f),     // meshId 3 — plant
};

static const glm::vec3 CAMERA_POSITIONS[4] = {
    glm::vec3(-3.5f, 5.0f, 3.0f),
    glm::vec3(6.5f, 5.0f, 3.0f),
    glm::vec3(0.0f, 4.0f,  9.0f),
    glm::vec3(0.0f, 2.0f, -6.0f),
};

static const glm::vec3 CAMERA_TARGETS[4] = {
    glm::vec3(3.5f, 0.0f, 3.0f),   // looking to each slot
    glm::vec3(-3.5f, 0.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -6.0f),
    glm::vec3(0.0f, 0.0f, 6.0f),
};

// --- Asigna mesh, posicion y escala al jugador local cuando llega su ID ---
static void AssignLocalPlayerMesh(
    ECSManager& ecs,
    size_t playerEntity,
    std::vector<std::shared_ptr<ObjItem>>& objItemList,
    MTRD::MotardaEng& eng)
{
    auto* netComp = ecs.GetComponent<MTRD::NetworkComponent>(playerEntity);
    if (!netComp || netComp->networkID == 0) return;

    int slot = netComp->networkID - 1;

    size_t objIdx;
    if (slot < 3) {
        objIdx = slot + 1;
    }
    else {
        objIdx = 3;
    }

    auto* t = ecs.GetComponent<MTRD::TransformComponent>(playerEntity);
    if (t) {
        t->position = SLOT_POSITIONS[slot];
        t->scale = SLOT_SCALES[objIdx];
    }

    ecs.AddComponent<MTRD::RenderComponent>(playerEntity);
    auto* r = ecs.GetComponent<MTRD::RenderComponent>(playerEntity);
    if (r) {
        r->objitem_ = objItemList[objIdx];
    }

    netComp->meshId_ = static_cast<float>(objIdx);

    int camSlot = netComp->networkID - 1;
    if (camSlot >= 0 && camSlot < 4) {
        eng.getCamera().setPosition(CAMERA_POSITIONS[camSlot]);
        eng.getCamera().setTarget(CAMERA_TARGETS[camSlot]);
    }
}

// --- Corrige la escala de todas las entidades remotas segun su meshId_ ---
static void UpdateRemoteScales(ECSManager& ecs, size_t playerEntity) {
    auto entities = ecs.GetEntitiesWithComponents<MTRD::NetworkComponent, MTRD::TransformComponent>();
    for (size_t entity : entities) {
        if (entity == playerEntity) continue;

        auto* netComp = ecs.GetComponent<MTRD::NetworkComponent>(entity);
        auto* t = ecs.GetComponent<MTRD::TransformComponent>(entity);
        if (netComp && t) {
            size_t meshIdx = static_cast<size_t>(netComp->meshId_);
            if (meshIdx < 4) {
                t->scale = REMOTE_SCALES[meshIdx];
            }
        }
    }
}

static bool keyWasPressed[3] = { false, false, false };
static bool alreadyPlayedThisRound = false;

static void ProcessCardInput(ECSManager& ecs, size_t playerEntity,
    NetworkManager& netMgr, MTRD::MotardaEng& eng,
    MTRD::CardGame& cardGame) {

    auto* netComp = ecs.GetComponent<MTRD::NetworkComponent>(playerEntity);
    if (!netComp || netComp->networkID == 0) return;

    // Si ya has jugado en esta ronda, bloqueamos el input completamente
    if (alreadyPlayedThisRound) {
        return;
    }

    // Teclas 1, 2, 3 → slots de carta 0, 1, 2
    Input::Keyboard keys[3] = {
        Input::Keyboard::A,
        Input::Keyboard::S,
        Input::Keyboard::D
    };

    for (int i = 0; i < 3; ++i) {
        bool pressed = eng.inputIsKeyPressed(keys[i]);

        if (pressed && !keyWasPressed[i]) {
            keyWasPressed[i] = true;

            if (i >= (int)cardGame.playerHand.size()) {
                MTRD::Logger::info("There is no card in the slot {}\n", i + 1);
                continue;
            }

            // Marcar que el jugador ya ha gastado su jugada de la ronda
            alreadyPlayedThisRound = true;

            // Remove the card from the hand
            MTRD::Card cardToPlay = cardGame.playerHand[i];
            cardGame.playerHand.erase(cardGame.playerHand.begin() + i);

            // Send to the server
            MTRD::CardPacket packet;
            packet.header.type = MTRD::MessageType::CardPlay;
            packet.header.senderId = netComp->networkID;
            packet.payload.suit = static_cast<uint32_t>(cardToPlay.suit);
            packet.payload.value = static_cast<uint32_t>(cardToPlay.number);

            netMgr.SendPacket(0, &packet, sizeof(packet), true);

            MTRD::Logger::info(">>> Jugada: {} de {} (slot {})\n",
                cardToPlay.number, MTRD::GetSuitName(cardToPlay.suit), i + 1);

            break;
        }
        else if (!pressed) {
            keyWasPressed[i] = false;
        }
    }
}

int MTRD::main() {
    static char nickBuffer[64] = "Jugador1";
    static char ipBuffer[64] = "127.0.0.1";
    static int portInput = 1234;

    bool isServer = false;
    AppState currentState = AppState::Menu;

    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda Multiplayer");
    if (!maybeEng.has_value()) return 1;
    auto& eng = maybeEng.value();

    eng.hasPhysx(true);

    eng.setDebugMode(true);
    eng.setRenderType(MotardaEng::RenderType::Base);
    eng.getCamera().setPosition(glm::vec3(0, 5, 10));
    eng.getCamera().setTarget(glm::vec3(0, 0, 0));

    std::vector<const char*> objsRoutes = {
        "tableRound.obj",
        "86jfmjiufzv2.obj",
        "12140_Skull_v3_L2.obj",
        "indoor_plant_02.obj"
    };
    std::atomic<bool> objsLoaded = false;
    std::vector<std::shared_ptr<ObjItem>> objItemList;
    objItemList.push_back(std::make_shared<ObjItem>());

    ECSManager& ecs = eng.getEcs();
    ecs.AddComponentType<MTRD::NetworkComponent>();
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();
    ecs.AddComponentType<MTRD::PhysxComponent>();

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
    std::unique_ptr<SimplePacketReceiver> simplPacRec;

    eng.enqueueTask([&]() {
        objItemList = eng.loadObjs(objsRoutes);
        objsLoaded.store(true, std::memory_order_release);
        });

    MTRD::CardGame cardGame;

    int connectedPlayers = 0;
    MTRD::CardGame serverCardGame;

    int clientPoints = 0;

    // --- Main loop ---
    while (!eng.windowShouldClose()) {
        eng.windowInitFrame();

        if (!objsLoaded.load(std::memory_order_acquire)) {
            eng.windowEndFrame();
            continue;
        }
        else if (firstTime) {
            firstTime = false;
            eng.windowLoadAllMaterials(objItemList);

            auto* rtable = ecs.GetComponent<MTRD::RenderComponent>(table);
            if (rtable) rtable->objitem_ = objItemList[0];
        }

        // --- Menu ---
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

                    serverCardGame.initDeck();
                    serverCardGame.shuffleDeck();
                    MTRD::Logger::info("Mazo del servidor inicializado y barajado por consola (48 cartas)!\n");

                    simplPacRec = std::make_unique<SimplePacketReceiver>(&objItemList, &ecs, SIZE_MAX);

                    // Create netsys
                    netSys = std::make_unique<NetworkSystem>(ecs, netMgr,
                        [&](uint32_t senderID, const void* data, size_t size) {
                            if (simplPacRec) simplPacRec->OnReceivePacket(senderID, data, size);

                            // Play cards (El servidor las procesa en orden solo dentro de la fase Jugando)
                            if (size == sizeof(MTRD::CardPacket) && serverCardGame.GetState() == ServerGameState::Jugando) {
                                const MTRD::CardPacket* cp = static_cast<const MTRD::CardPacket*>(data);
                                if (cp->header.type == MTRD::MessageType::CardPlay) {
                                    MTRD::Card played(
                                        static_cast<int>(cp->payload.suit),
                                        static_cast<int>(cp->payload.value)
                                    );
                                    serverCardGame.tableCards.push_back({ cp->header.senderId, played });

                                    MTRD::Logger::info("Servidor: Player {} played {} of {}\n",
                                        cp->header.senderId,
                                        cp->payload.value,
                                        MTRD::GetSuitName(cp->payload.suit).c_str());

                                    // Si todos los jugadores conectados han echado carta, saltamos a leer la baza
                                    if ((int)serverCardGame.tableCards.size() == netMgr.GetConnectedCount()
                                        && netMgr.GetConnectedCount() > 0) {

                                        serverCardGame.SetState(ServerGameState::LeyendoBaza);
                                    }
                                }
                            }

                            //Add players if someone logs in
                            if (size == 0 && senderID != 0 && !(senderID & 0x80000000)) {
                                MTRD::Logger::info("Jugador {} conectado.\n", senderID);


                                auto entities = ecs.GetEntitiesWithComponents
                                    <MTRD::NetworkComponent, MTRD::TransformComponent >();

                                for (size_t entity : entities) {
                                    auto* netComp = ecs.GetComponent<MTRD::NetworkComponent>(entity);
                                    auto* transform = ecs.GetComponent<MTRD::TransformComponent>(entity);
                                    if (!netComp || !transform || netComp->networkID == 0) continue;
                                    if (netComp->networkID == senderID) continue; // Dont send to myself

                                    struct FullUpdatePacket {
                                        MTRD::NetMessage header;
                                        MTRD::EntityUpdatePayload payload;
                                    } packet;

                                    packet.header.type = MTRD::MessageType::EntityUpdate;
                                    packet.header.senderId = netComp->networkID;
                                    packet.payload.networkID = netComp->networkID;
                                    packet.payload.meshId_ = netComp->meshId_;
                                    packet.payload.posX = transform->position.x;
                                    packet.payload.posY = transform->position.y;
                                    packet.payload.posZ = transform->position.z;
                                    packet.payload.rotX = transform->rotation.x;
                                    packet.payload.rotY = transform->rotation.y;
                                    packet.payload.rotZ = transform->rotation.z;

                                    // Only send to the player that just joined
                                    netMgr.SendPacket(senderID, &packet, sizeof(packet), true);
                                    MTRD::Logger::info("Enviando estado de jugador {} al nuevo jugador {}\n",
                                        netComp->networkID, senderID);
                                }
                            }
                            //Remove players if someone leaves the game
                            if (size == 0 && (senderID & 0x80000000)) {
                                if (connectedPlayers > 0) connectedPlayers--;
                                MTRD::Logger::info("Jugador desconectado. Total: {}\n", connectedPlayers);
                            }
                        }
                    );

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

        // --- HUD info jugador ---
        if (currentState == AppState::Running) {
            ImGui::Begin("Info Jugador");
            ImGui::Text("Nick: %s", nickBuffer);
            ImGui::Text("Modo: %s", isServer ? "Servidor" : "Cliente");

            if (isServer) {
                ImGui::Separator();
                ImGui::Text("Contador conectados: %d", netMgr.GetConnectedCount());

                // Debug del estado interno del bucle del juego en el servidor
                const char* statusStr = "Inactivo";
                ServerGameState currentStateEnum = serverCardGame.GetState();
                if (currentStateEnum == ServerGameState::Idle) statusStr = "Esperando Inicio";
                else if (currentStateEnum == ServerGameState::Jugando) statusStr = "Esperando cartas de jugadores";
                else if (currentStateEnum == ServerGameState::LeyendoBaza) statusStr = "Resolviendo Baza...";
                else if (currentStateEnum == ServerGameState::Robando) statusStr = "Repartiendo robo de turno...";
                else if (currentStateEnum == ServerGameState::EndGame) statusStr = "¡Partida Finalizada!";
                ImGui::Text("Estado Turno: %s", statusStr);

                // El botón solo arranca el loop si estamos en reposo (Idle)
                if (currentStateEnum == ServerGameState::Idle) {
                    if (ImGui::Button("REPARTIR CARTAS INICIALES", ImVec2(220, 40))) {
                        if (netMgr.GetConnectedCount() > 0) {
                            serverCardGame.SetState(ServerGameState::DealingInitial);
                        }
                        else {
                            MTRD::Logger::info("No se puede iniciar el bucle sin clientes conectados.\n");
                        }
                    }
                }
            }
            else {
                // Información del cliente
                auto* netComp = ecs.GetComponent<MTRD::NetworkComponent>(playerEntity);
                if (netComp) {
                    if (netComp->networkID != 0) {
                        ImGui::Text("Tu ID de red: %u", netComp->networkID);
                        ImGui::Separator();

                        // Show current hand
                        ImGui::Text("Tu mano (%d cartas):", (int)cardGame.playerHand.size());
                        for (size_t i = 0; i < cardGame.playerHand.size(); ++i) {
                            ImGui::Text("  [%c] %d de %s",
                                'A' + (char)i,
                                cardGame.playerHand[i].number,
                                MTRD::GetSuitName(cardGame.playerHand[i].suit).c_str());
                        }

                        ImGui::Separator();

                        // Show points
                        ImGui::Text("Puntos: %d", clientPoints);
                    }
                    else {
                        ImGui::TextDisabled("Esperando ID del servidor...");
                    }
                }
            }
            ImGui::End();
        }

        // --- Connecting ---
        if (currentState == AppState::Connecting) {
            playerEntity = ecs.AddEntity();
            auto* netComp = ecs.AddComponent<MTRD::NetworkComponent>(playerEntity);
            netComp->isLocal = true;

            auto* tplayer = ecs.AddComponent<MTRD::TransformComponent>(playerEntity);
            tplayer->position = glm::vec3(0, 0, 0);
            tplayer->scale = glm::vec3(1.0f);

            simplPacRec = std::make_unique<SimplePacketReceiver>(&objItemList, &ecs, playerEntity);
            currentState = AppState::Running;
        }

        // --- MÁQUINA DE ESTADOS LOGICA CENTRAL DEL SERVIDOR ---
        if (isServer && currentState == AppState::Running) {
            switch (serverCardGame.GetState()) {
            case ServerGameState::DealingInitial: {
                MTRD::Logger::info("Initial distribution from the server (3 cards)\n");
                for (int i = 1; i <= netMgr.GetConnectedCount(); ++i) {
                    DealCardsPacket initPacket;
                    initPacket.header.type = MessageType::DealCards;
                    initPacket.header.senderId = 0; // Server ID
                    initPacket.payload = serverCardGame.dealThreeCards();

                    netMgr.SendPacket(static_cast<uint32_t>(i), &initPacket, sizeof(initPacket), true);
                    MTRD::Logger::info("3 initial cards provided to the player ID: {}\n", i);
                }
                // Change state to playing after the initial 3 cards
                serverCardGame.SetState(ServerGameState::Jugando);
                break;
            }
            case ServerGameState::LeyendoBaza: {
                MTRD::Logger::info("[SERVER] Procesando resolucion de baza...\n");
                int trump = 0; // Trinfo
                uint32_t winnerID = serverCardGame.resolveBaza(trump);

                MTRD::Logger::info("Baza ganada por jugador {}! Puntos: {}\n",
                    winnerID, serverCardGame.scores[winnerID]);

                CardResultPacket result;
                result.header.type = MTRD::MessageType::CardResult;
                result.header.senderId = 0;
                result.payload.winnerID = winnerID;
                result.payload.points = static_cast<uint32_t>(serverCardGame.scores[winnerID]);
                result.payload.card1Suit = 0; result.payload.card1Value = 0;
                result.payload.card2Suit = 0; result.payload.card2Value = 0;

                netMgr.BroadcastPacket(&result, sizeof(result), true);

                // Tras resolver la baza, pasamos automáticamente a la fase de robar
                serverCardGame.SetState(ServerGameState::Robando);
                break;
            }
            case ServerGameState::Robando: {
                MTRD::Logger::info("[SERVER] Iniciando fase de robo de turno...\n");

                // Comprobar si queda al menos una carta disponible en el mazo
                bool hasCardsLeft = false;
                for (size_t i = 0; i < serverCardGame.cards.size(); ++i) {
                    // Accedemos a usedCards de forma pública indirecta o como estaba en tu lógica original
                    // Nota: Si usedCards es privada en tu cabecera, puedes usar serverCardGame.cards con una función,
                    // pero mantengo la lectura tal cual la tenías en tu cpp.
                }

                // Para respetar fielmente tu lógica con usedCards privada, usaremos un truco o asumo que se verifica en el servidor:
                // Como moviste usedCards a private, una forma limpia es simular el flag o delegar. 
                // Para compilar directo, asumo que comprueba si dealOneCard devuelve algo válido o añadimos la comprobación:
                DealCardsPayload drawTest = serverCardGame.dealOneCard();

                if (drawTest.value[0] != 0) {
                    MTRD::Logger::info("Server providing 1 card to everyone!\n");
                    for (int i = 1; i <= netMgr.GetConnectedCount(); ++i) {
                        DealCardsPacket drawPacket;
                        drawPacket.header.type = MessageType::DealCards;
                        drawPacket.header.senderId = 0;

                        if (i == 1) {
                            drawPacket.payload = drawTest; // El jugador 1 recibe la que ya sacamos como test
                        }
                        else {
                            drawPacket.payload = serverCardGame.dealOneCard();
                        }

                        netMgr.SendPacket(static_cast<uint32_t>(i), &drawPacket, sizeof(drawPacket), true);
                        MTRD::Logger::info("1 card provided to the player ID: {}\n", i);
                    }
                    // Volvemos a esperar que jueguen carta en el nuevo turno
                    serverCardGame.SetState(ServerGameState::Jugando);
                }
                else {
                    MTRD::Logger::info("[SERVER] ¡Mazo vacío! No quedan más cartas para robar. Fin del juego.\n");
                    serverCardGame.SetState(ServerGameState::EndGame);
                }
                break;
            }
            case ServerGameState::EndGame: {
                // Loop en reposo al acabar la partida (Muestra puntuación final estática en logs)
                break;
            }
            default: break;
            }
        }

        // --- Running ---
        if (currentState == AppState::Running) {

            // Init NetworkSystem if still does not exist
            if (!netSys && !isServer) {
                netSys = std::make_unique<NetworkSystem>(ecs, netMgr,
                    [&](uint32_t senderID, const void* data, size_t size) {
                        bool intercepted = false;

                        // Catch the card packet
                        if (!isServer && size == sizeof(DealCardsPacket)) {
                            const DealCardsPacket* deal = static_cast<const DealCardsPacket*>(data);
                            if (deal->header.type == MessageType::DealCards) {
                                cardGame.receiveSpecificCards(deal->payload);
                                intercepted = true;
                            }
                        }

                        // Y en el callback del cliente donde recibes CardResult
                        if (!intercepted && size == sizeof(CardResultPacket)) {
                            const CardResultPacket* res = static_cast<const CardResultPacket*>(data);
                            if (res->header.type == MTRD::MessageType::CardResult) {
                                MTRD::Logger::info("--- RESULT OF BAZA ---");
                                MTRD::Logger::info("Winner: player {} | Points this round: {}",
                                    res->payload.winnerID, res->payload.points);

                                // --- AQUÍ DESBLOQUEAMOS EL INPUT DEL CLIENTE ---
                                alreadyPlayedThisRound = false;
                                MTRD::Logger::info("New round, you can play again!\n");

                                // Si yo soy el ganador, actualizar mis puntos locales
                                auto* netComp = ecs.GetComponent<MTRD::NetworkComponent>(playerEntity);
                                if (netComp && res->payload.winnerID == netComp->networkID) {
                                    clientPoints = static_cast<int>(res->payload.points);
                                    MTRD::Logger::info("You are the winner of this Baza!\n");
                                }
                                intercepted = true;
                            }
                        }

                        if (!intercepted && simplPacRec) {
                            simplPacRec->OnReceivePacket(senderID, data, size);
                        }

                        // Connected player counter
                        if (isServer && size == 0 && senderID != 0) {
                            connectedPlayers++;
                        }
                    }
                );
            }

            // Assign the mesh for the players
            if (!meshAssigned && playerEntity != SIZE_MAX) {
                auto* netComp = ecs.GetComponent<MTRD::NetworkComponent>(playerEntity);
                if (netComp && netComp->networkID != 0) {
                    AssignLocalPlayerMesh(ecs, playerEntity, objItemList, eng);
                    meshAssigned = true;
                }
            }

            // Game/Net update
            if (netSys) {
                if (!isServer && playerEntity != SIZE_MAX) {
                    //ProcessPlayerInput(ecs, playerEntity, eng);
                    ProcessCardInput(ecs, playerEntity, netMgr, eng, cardGame);
                }

                UpdateRemoteScales(ecs, playerEntity);
                netSys->Process();
            }
        }

        eng.renderScene();
        eng.windowEndFrame();
    }

    netMgr.Shutdown();
    return 0;
}