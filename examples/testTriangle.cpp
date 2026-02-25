#include "MotArda/common/Engine.hpp"

#include <memory>
#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"
#include <MotArda/common/Ecs.hpp>
#include <MotArda/common/Camera.hpp>
#include <MotArda/win64/Systems/RenderSystem.hpp>


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw error: %s\n", description);
}

int MTRD::main() {

    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda triangle");
    if (!maybeEng.has_value()) return 1;
    auto& eng = maybeEng.value();

    eng.windowSetErrorCallback(error_callback);


    // --- Create drawable entitys ---
    ECSManager ecs;
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

    bool FirstTime = false;
    std::unique_ptr<Mesh> TriangleMesh = std::make_unique<Mesh>(
        vertexList,
        eng.window_,
        "triangle",
        FirstTime,
        -1,
        true
    );

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

    // --- Vectores de uniforms y atributos ---
    glm::mat4x4 vp, model;
    // --- *** --- 

    // --- Drawable transforms additions ---
    float ratio = eng.windowGetSizeRatio();
    float movSpeed = 0.05f;
    // --- *** ---

    // --- Camera ---
    MTRD::Camera camera(
        glm::vec3(0.f, 0.f, 0.5f),
        glm::vec3(0.f, 0.f, 0.f),
        glm::vec3(0.f, 1.f, 0.f),
        glm::radians(45.f),
        ratio,
        0.1f,
        100.f
    );

    RenderSystem renderSystem = RenderSystem(vp, model);

    camera.updateAll();
    // --- *** ---

    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();

        vp = camera.getViewProj();
        renderSystem.Render(
            ecs,
            ecs.GetEntitiesWithComponents<RenderComponent, TransformComponent>(),
            model,
            true
            );

        //eng.windowOpenglSetUniformsValues(uniforms);
        //eng.windowOpenglProgramUniformDrawRender(*r);

        eng.windowEndFrame();
    }

    return 0;
}
