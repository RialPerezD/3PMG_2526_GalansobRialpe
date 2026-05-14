#include <switch.h>

#include <MotArda/Engine.hpp>
#include <MotArda/Ecs.hpp>
#include <MotArda/Camera.hpp>
#include <MotArda/Systems/RenderSystem.hpp>

#include <memory>

//-----------------------------------------------------------------------------
// nxlink support
//-----------------------------------------------------------------------------

#ifndef ENABLE_NXLINK
#define TRACE(fmt,...) ((void)0)
#else
#include <unistd.h>
#define TRACE(fmt,...) printf("%s: " fmt "\n", __PRETTY_FUNCTION__, ## __VA_ARGS__)

static int s_nxlinkSock = -1;

static void initNxLink()
{
    if (R_FAILED(socketInitializeDefault()))
        return;

    s_nxlinkSock = nxlinkStdio();
    if (s_nxlinkSock >= 0)
        TRACE("printf output now goes to nxlink server");
    else
        socketExit();
}

static void deinitNxLink()
{
    if (s_nxlinkSock >= 0)
    {
        close(s_nxlinkSock);
        socketExit();
        s_nxlinkSock = -1;
    }
}

extern "C" void userAppInit()
{
    initNxLink();
}

extern "C" void userAppExit()
{
    deinitNxLink();
}

#endif

int main(int argc, char* argv[])
{
    auto maybeEng = MTRD::MotardaEng::createEngine(1280, 720, "Motarda triangle Switch");
    if (!maybeEng.has_value()) return EXIT_FAILURE;
    auto& eng = maybeEng.value();

    // --- Camera ---
    MTRD::Camera& camera = eng.getCamera();
    camera.setPosition(glm::vec3(0.f, 0.f, 0.5f));
    camera.setTarget(glm::vec3(0.f, 0.f, 0.f));
    float movSpeed = 0.1f;

    // --- Setup Engine ---
    eng.SetDebugMode(true);
    eng.SetRenderType(MTRD::MotardaEng::RenderType::Base);


    // --- Create drawable entities ---
    ECSManager& ecs = eng.getEcs();
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();
    ecs.AddComponentType<MTRD::MovementComponent>();

    while (true) {
        printf("Hola que tal\n");
    }

    size_t entity = ecs.AddEntity();

    MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(entity);
    t->position = glm::vec3(0.0f);
    t->rotation = glm::vec3(1.0f, 0.0f, 0.0f);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(0.05f);

    std::vector<MTRD::Vertex> vertexList = {
       {
            { 1.0f,  0.0f, 0.f },
            { 0.0f,  0.0f },
            { 1.0f,  0.0f, 0.f }
        },
       {
            { 0.0f,  1.5f, 0.f},
            { 0.0f,  0.0f},
            { 0.0f,  1.0f, 0.f}
        },

        {
            {-1.0f,  0.0f, 0.f},
            { 0.0f,  0.0f},
            { 0.0f,  0.0f, 1.f}
        }
    };

    std::vector<std::shared_ptr<MTRD::ObjItem>> ObjList;
    ObjList.push_back(std::make_shared<MTRD::ObjItem>());

    std::unique_ptr<MTRD::Mesh> TriangleMesh = eng.createMesh(vertexList, "triangle");

    MTRD::Material mat;
    mat.diffuse = glm::vec3(1.0f);
    mat.specular = glm::vec3(1.0f);
    mat.shininess = 32.0f;
    mat.loadeable = true;
    mat.diffuseTexPath = "../assets/textures/blank/blank.jpg";

    ObjList[0]->materials.push_back(mat);
    ObjList[0]->meshes.push_back(std::move(TriangleMesh));
    ObjList[0]->meshes[0]->materialId_ = 0;

    eng.windowLoadAllMaterials(ObjList);

    MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(entity);
    r->objitem_ = ObjList[0];

    while (!eng.windowShouldClose()) {
        eng.windowInitFrame();
        eng.RenderScene();
        eng.windowEndFrame();
    }

    return EXIT_SUCCESS;
}
