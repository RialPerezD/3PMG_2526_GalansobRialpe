#include <MotArda/Engine.hpp>
#include <MotArda/Logger.hpp>
#include <MotArda/Ecs.hpp>
#include <MotArda/Camera.hpp>
#include <MotArda/Systems/RenderSystem.hpp>

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


static void error_callback([[maybe_unused]] int error, const char* description) {
    MTRD::Logger::error("Glfw error: {}\n", description);

}

int MTRD::main() {

    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "MotArda triangle");
    if (!maybeEng.has_value()) return 1;
    auto& eng = maybeEng.value();

    // --- Camera ---
    MTRD::Camera& camera = eng.getCamera();
    camera.setPosition(glm::vec3(0.f, 0.f, 0.5f));
    camera.setTarget(glm::vec3(0.f, 0.f, 0.f));
    float movSpeed = 0.01f;

    // --- Setup Engigne ---

    eng.setDebugMode(true);
    eng.setRenderType(MotardaEng::RenderType::Base);
    eng.windowSetErrorCallback(error_callback);

    // --- Create drawable entitys ---
    ECSManager& ecs = eng.getEcs();
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();

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

    std::vector<std::shared_ptr<ObjItem>> ObjList;
    ObjList.push_back(std::make_shared<ObjItem>());

    std::unique_ptr<Mesh> TriangleMesh = eng.createMesh(vertexList, "triangle");

    Material mat;
    mat.diffuse = glm::vec3(1.0f);
    mat.specular = glm::vec3(1.0f);
    mat.shininess = 32.0f;
    mat.loadeable = true;
    mat.diffuseTexPath = "../assets/textures/blank/blank.jpg";

    ObjList[0]->materials.push_back(mat);
    ObjList[0]->meshes.push_back(std::move(TriangleMesh));
    ObjList[0]->meshes[0]->materialId_ = 0;

    eng.windowLoadAllMaterials(ObjList);

    r->objitem_ = ObjList[0];
    // --- *** ---

    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();
        eng.renderScene();

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

    eng.endDebugger();

    return 0;
}
