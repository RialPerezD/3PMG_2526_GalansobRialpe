#include "MotArda/common/Engine.hpp"

#include <memory>
#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"
#include <MotArda/common/Ecs.hpp>
#include <MotArda/common/Camera.hpp>
#include <MotArda/win64/Systems/RenderSystem.hpp>


static void error_callback(int error, const char* description){
    fprintf(stderr, "Glfw error: %s\n", description);
}

int MTRD::main() {

    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda triangle");
    if (!maybeEng.has_value()) return 1;
    auto& eng = maybeEng.value();

    // --- Camera ---
    MTRD::Camera& camera = eng.getCamera();
    camera.setPosition(glm::vec3(0.f, 0.f, 0.5f));
    camera.setTarget(glm::vec3(0.f, 0.f, 0.f));
    float movSpeed = 0.01f;

    // --- Setup Engigne ---

    eng.SetDebugMode(true);
    eng.SetRenderType(MotardaEng::RenderType::Base);
    eng.windowSetErrorCallback(error_callback);

    // --- Create drawable entitys ---
    ECSManager& ecs = eng.getEcs();
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();
    ecs.AddComponentType<MTRD::MovementComponent>();

    size_t entity = ecs.AddEntity();

    MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(entity);
    t->position = glm::vec3(0.0f);
    t->rotation = glm::vec3(1.0f, 0.0f, 0.0f);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(0.05f);

    MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(entity);

    std::vector<Vertex> vertexList = {
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

    std::vector<ObjItem> ObjList;
    ObjList.push_back(ObjItem());

    std::unique_ptr<Mesh> TriangleMesh = eng.createMesh(vertexList, "triangle");

    Material mat;
    mat.diffuse = glm::vec3(1.0f);
    mat.specular = glm::vec3(1.0f);
    mat.shininess = 32.0f;
    mat.loadeable = true;
    mat.diffuseTexPath = "../assets/textures/blank/blank.jpg";

    ObjList[0].materials.push_back(mat);
    ObjList[0].meshes.push_back(std::move(TriangleMesh));

    eng.windowLoadAllMaterials(ObjList);

    r->meshes_ = &ObjList[0].meshes;
    r->materials_ = &ObjList[0].materials;
    // --- *** ---

    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();
        eng.RenderScene();

        // --- Input to move camera ---
        if (eng.inputIsKeyPressed(Input::Keyboard::W)) camera.moveForward(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::S)) camera.moveBackward(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::A)) camera.moveLeft(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::D)) camera.moveRight(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::E)) camera.moveUp(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::Q)) camera.moveDown(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::R)) camera.rotate(10.0f, 0.0f);
        if (eng.inputIsKeyPressed(Input::Keyboard::T)) camera.rotate(-10.0f, 0.0f);
        // --- *** ---

        eng.windowEndFrame();
    }

    return 0;
}
