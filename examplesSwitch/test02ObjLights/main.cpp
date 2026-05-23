#include <MotArda/Engine.hpp>
#include <MotArda/Ecs.hpp>
#include <MotArda/Camera.hpp>
#include <MotArda/Systems/RenderSystem.hpp>

#include <memory>

#include <switch.h>
#include <unistd.h>

#include <fstream>

static int s_nxlinkSock = -1;

extern "C" void userAppInit() {
    if (R_SUCCEEDED(socketInitializeDefault())) {
        s_nxlinkSock = nxlinkStdio();
        if (s_nxlinkSock < 0) {
            socketExit();
        }
    }else{
        printf("fallo");
    }
}

extern "C" void userAppExit() {
    if (s_nxlinkSock >= 0) {
        close(s_nxlinkSock);
        socketExit();
        s_nxlinkSock = -1;
    }
}


// Light component identifier within the ECS
size_t lightEntity;
glm::vec3 spotLigthCenter = glm::vec3(10, 0, 0);
glm::vec3 pointLigthCenter = glm::vec3(-10, 0, 0);

// Create the structure that will be illuminated by the corresponding light.
void GenerateSpotLightEntitys(ECSManager& ecs, std::vector<std::shared_ptr<MTRD::ObjItem>>& objItemList) {
    size_t player = ecs.AddEntity();
    size_t floor = ecs.AddEntity();
    size_t cubes[4] = { ecs.AddEntity(), ecs.AddEntity(), ecs.AddEntity(), ecs.AddEntity() };
    lightEntity = ecs.AddEntity();

    MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(player);
    t->position = glm::vec3(0, -2.f, 0) + spotLigthCenter;
    t->rotation = glm::vec3(0, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(1.f);

    MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(player);
    r->objitem_ = objItemList[0];


    t = ecs.AddComponent<MTRD::TransformComponent>(floor);
    t->position = glm::vec3(0, -3, 0) + spotLigthCenter;
    t->rotation = glm::vec3(0, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(1.f);

    r = ecs.AddComponent<MTRD::RenderComponent>(floor);
    r->objitem_ = objItemList[1];


    for (int i = 0; i < 4; i++) {
        t = ecs.AddComponent<MTRD::TransformComponent>(cubes[i]);
        t->position = glm::vec3(-5 * ((i % 2) * 2 - 1), -2.0f, -5 * ((i / 2) * 2 - 1)) + spotLigthCenter;
        t->rotation = glm::vec3(0, 0, 0);
        t->angleRotationRadians = -1;
        t->scale = glm::vec3(1.f);

        r = ecs.AddComponent<MTRD::RenderComponent>(cubes[i]);
        r->objitem_ = objItemList[2];
    }
}


void GenerateSpotLights(MTRD::LightComponent* lightComp, MTRD::MotardaEng& eng) {
    lightComp->spotLights.push_back(
        MTRD::SpotLight(
            glm::vec3(0.0f, 0.0f, 0.0f) + spotLigthCenter,
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            5.0f,
            1.f,
            0.75f,
            1.0f,
            0.09f,
            0.032f,
            eng.windowGetSizeRatio()
        )
    );

    lightComp->spotLights.push_back(
        MTRD::SpotLight(
            glm::vec3(0.0f, 0.0f, 0.0f) + spotLigthCenter,
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            5.0f,
            1.0f,
            0.75f,
            1.0f,
            0.09f,
            0.032f,
            eng.windowGetSizeRatio()
        )
    );

    lightComp->spotLights.push_back(
        MTRD::SpotLight(
            glm::vec3(5.0f, 5.0f, 5.0f) + spotLigthCenter,
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            7.0f,
            1.0f,
            0.85f,
            1.0f,
            0.09f,
            0.032f,
            eng.windowGetSizeRatio()
        )
    );
}

// Create the structure that will be illuminated by the corresponding light.
void GeneratePointLightEntitys(ECSManager& ecs, std::vector<std::shared_ptr<MTRD::ObjItem>>& objItemList) {
    size_t player = ecs.AddEntity();
    size_t floor = ecs.AddEntity();
    size_t cubes[4] = { ecs.AddEntity(), ecs.AddEntity(), ecs.AddEntity(), ecs.AddEntity() };
    lightEntity = ecs.AddEntity();

    MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(player);
    t->position = glm::vec3(0, -2.f, 0) + pointLigthCenter;
    t->rotation = glm::vec3(0, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(1.f);

    MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(player);
    r->objitem_ = objItemList[0];


    t = ecs.AddComponent<MTRD::TransformComponent>(floor);
    t->position = glm::vec3(0, -3, 0) + pointLigthCenter;
    t->rotation = glm::vec3(0, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(1.f);

    r = ecs.AddComponent<MTRD::RenderComponent>(floor);
    r->objitem_ = objItemList[1];


    for (int i = 0; i < 4; i++) {
        t = ecs.AddComponent<MTRD::TransformComponent>(cubes[i]);
        t->position = glm::vec3(-5 * ((i % 2) * 2 - 1), -2.0f, -5 * ((i / 2) * 2 - 1)) + pointLigthCenter;
        t->rotation = glm::vec3(0, 0, 0);
        t->angleRotationRadians = -1;
        t->scale = glm::vec3(1.f);

        r = ecs.AddComponent<MTRD::RenderComponent>(cubes[i]);
        r->objitem_ = objItemList[2];
    }
}


void GeneratePointLights(MTRD::LightComponent* lightComp) {
    lightComp->pointLights.push_back(
        MTRD::PointLight(
            glm::vec3(0.0f, 1.0f, 0.0f) + pointLigthCenter,
            glm::vec3(1.0f, 1.0f, 0.0f),
            1.0f,
            1.0f,
            0.09f,
            0.032f
        )
    );
}


void GenerateDirectionalLights(MTRD::LightComponent* lightComp) {
    lightComp->directionalLights.push_back(
        MTRD::DirectionalLight(
            glm::vec3(-1.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 1.0f),
            0.05f
        )
    );
}

int main(int argc, char* argv[])
{
    romfsInit();
    auto maybeEng = MTRD::MotardaEng::createEngine(1280, 720, "Motarda triangle Switch");

    if (maybeEng.has_value()) {
        auto& eng = maybeEng.value();
        eng.setDebugMode(true);

        { std::ofstream file("testeo.txt"); file << "inicio main\n"; }

        // --- Camera ---
        MTRD::Camera& camera = eng.getCamera();
        camera.setPosition(glm::vec3(0, 1, 20));
        camera.setTarget(glm::vec3(0.0f, -5.0f, 0.0f));
        // --- *** ---


        // --- Setup engine info ---
        eng.setDebugMode(true);
        eng.setRenderType(MTRD::MotardaEng::RenderType::LightsWithShadows);
        // --- *** ---


        // --- Create Geometry to use in elements ---
        std::vector<std::shared_ptr<MTRD::ObjItem>> objItemList;
        objItemList.push_back(eng.generateSphere(0.5f, 20, 20, 1));
        objItemList.push_back(eng.generatePlane(20, 20, 1));
        objItemList.push_back(eng.generateCube(1));
        eng.windowLoadAllMaterials(objItemList);
        // --- *** ---


        // --- Create drawable entitys ---
        ECSManager& ecs = eng.getEcs();
        ecs.AddComponentType<MTRD::TransformComponent>();
        ecs.AddComponentType<MTRD::RenderComponent>();
        ecs.AddComponentType<MTRD::LightComponent>();

        GenerateSpotLightEntitys(ecs, objItemList);
        GeneratePointLightEntitys(ecs, objItemList);
        // --- *** ---


        // --- Lights ---
        MTRD::LightComponent* lightComp = ecs.AddComponent<MTRD::LightComponent>(lightEntity);
        ecs.AddComponent<MTRD::TransformComponent>(lightEntity);
        GenerateSpotLights(lightComp, eng);
        GenerateDirectionalLights(lightComp);
        GeneratePointLights(lightComp);
        // --- *** ---

        float radio = 10.f;
        float velocidad = 1.f;
        float timer = 0.0f;

        // The main loop will run until the user closes the window
        while (!eng.windowShouldClose()) {
            // Creates a new frame
            eng.windowInitFrame();

            timer += eng.windowGetLastFrameTime();

            float coseno = radio * cos(timer * velocidad);
            float seno = radio * sin(timer * velocidad);


            lightComp->spotLights[0].position_ = glm::vec3(coseno / 5, 0.0f, seno / 5) + spotLigthCenter;
            lightComp->spotLights[0].direction_ = glm::normalize(glm::vec3(0, -2, 0) - lightComp->spotLights[0].position_ + spotLigthCenter);

            lightComp->spotLights[1].position_ = glm::vec3(seno / 5, 0.0f, coseno / 5) + spotLigthCenter;
            lightComp->spotLights[1].direction_ = glm::normalize(glm::vec3(0, -2, 0) - lightComp->spotLights[1].position_ + spotLigthCenter);

            lightComp->spotLights[2].position_ = glm::vec3(-coseno * 0.68f, 0.0f, seno * 0.68f) + spotLigthCenter;


            eng.renderScene();

            // Ends the frame
            eng.windowEndFrame();
        }

        return 0;
    }

    romfsExit();
    return 1;
}
