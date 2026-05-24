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
        camera.setPosition(glm::vec3(0.f, 1.f, 5.f));
        camera.setTarget(glm::vec3(0.f, 0.f, 0.f));
        // --- *** ---


        // --- Setup engine info ---
        eng.setDebugMode(true);
        eng.setRenderType(MTRD::MotardaEng::RenderType::LightsWithShadows);
        // --- *** ---


        std::vector <const char*> objsRoutes = { "12140_Skull_v3_L2.obj" };
        // Atomic bool which indicates if the Obj has been loaded to the multithread task
        std::atomic<bool> objsLoaded = false;

        std::vector<std::shared_ptr<MTRD::ObjItem>> ObjList;


        // --- Create drawable entitys ---
        ECSManager& ecs = eng.getEcs();
        ecs.AddComponentType<MTRD::TransformComponent>();
        ecs.AddComponentType<MTRD::RenderComponent>();
        ecs.AddComponentType<MTRD::LightComponent>();

        size_t entity = ecs.AddEntity();
        // --- *** ---

        MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(entity);
        t->position = glm::vec3(0.0f);
        t->rotation = glm::vec3(1.0f, 0.0f, 0.0f);
        t->angleRotationRadians = -1.5708f;
        t->scale = glm::vec3(0.05f);


        // async obj load no not work good in switch
        ObjList = eng.loadObjs(objsRoutes);
        eng.windowLoadAllMaterials(ObjList);

        MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(entity);
        r->objitem_ = ObjList[0];


        size_t light = ecs.AddEntity();
        ecs.AddComponent<MTRD::TransformComponent>(light);
        MTRD::LightComponent* lightComp = ecs.AddComponent<MTRD::LightComponent>(light);
        lightComp->directionalLights.push_back(
            MTRD::DirectionalLight(
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 1.0f),
                0.2f
            )
        );
        lightComp->pointLights.push_back(
            MTRD::PointLight(
                glm::vec3(-1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),
                1.0f,
                1.0f,
                0.09f,
                0.032f
            )
        );
        lightComp->pointLights.push_back(
            MTRD::PointLight(
                glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                1.0f,
                1.0f,
                0.09f,
                0.032f
            )
        );

        float radio = 10.f;
        float velocidad = 1.f;
        float timer = 0.0f;

        // The main loop will run until the user closes the window
        while (!eng.windowShouldClose()) {
            // Creates a new frame
            eng.windowInitFrame();

            timer += eng.windowGetLastFrameTime();
            float posX = radio * cos(timer * velocidad);
            float posY = radio * sin(timer * velocidad);

            lightComp->pointLights[0].position_ = glm::vec3(posX / 2, 0.0f, posY / 2);
            lightComp->pointLights[1].position_ = glm::vec3(-posX / 2, 0.0f, -posY / 2);

            eng.renderScene();

            // Ends the frame
            eng.windowEndFrame();
        }

        return 0;
    }

    romfsExit();
    return 1;
}
