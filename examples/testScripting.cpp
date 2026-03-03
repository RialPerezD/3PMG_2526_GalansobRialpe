#include "MotArda/common/Engine.hpp"
#include "MotArda/common/ObjLoader.hpp"
#include <MotArda/common/Ecs.hpp>
#include <MotArda/common/Camera.hpp>

#include <sol/sol.hpp>
#include <memory>
#include <atomic>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"
#include <MotArda/win64/Systems/RenderSystem.hpp>


static const char* lua_program = R"(

function update_player(movement)
    if Input.isKeyPressed(KEY_I) then
        movement.position.y = movement.position.y + 0.1
    end
    if Input.isKeyPressed(KEY_K) then
        movement.position.y = movement.position.y - 0.1
    end
    if Input.isKeyPressed(KEY_J) then
        movement.position.x = movement.position.x - 0.1
    end
    if Input.isKeyPressed(KEY_L) then
        movement.position.x = movement.position.x + 0.1
    end

    if Input.isKeyPressed(KEY_U) then
        movement.angleRotationRadians = movement.angleRotationRadians + 0.05
    end
    if Input.isKeyPressed(KEY_O) then
        movement.angleRotationRadians = movement.angleRotationRadians - 0.05
    end

    local scaleSpeed = 0.005

    if Input.isKeyPressed(KEY_N) then
        movement.scale.x = movement.scale.x + scaleSpeed
        movement.scale.y = movement.scale.y + scaleSpeed
        movement.scale.z = movement.scale.z + scaleSpeed
    end

    if Input.isKeyPressed(KEY_M) then
        movement.scale.x = movement.scale.x - scaleSpeed
        movement.scale.y = movement.scale.y - scaleSpeed
        movement.scale.z = movement.scale.z - scaleSpeed
    end

    end

function update_camera(camera, speed)
    if Input.isKeyPressed(KEY_W) then
        camera:moveForward(speed)
    end
    if Input.isKeyPressed(KEY_S) then
        camera:moveBackward(speed)
    end
    if Input.isKeyPressed(KEY_A) then
        camera:moveLeft(speed)
    end
    if Input.isKeyPressed(KEY_D) then
        camera:moveRight(speed)
    end
    if Input.isKeyPressed(KEY_E) then
        camera:moveUp(speed)
    end
    if Input.isKeyPressed(KEY_Q) then
        camera:moveDown(speed)
    end
    if Input.isKeyPressed(KEY_R) then
        camera:rotate(10.0, 0.0)
    end
    if Input.isKeyPressed(KEY_T) then
        camera:rotate(-10.0, 0.0)
    end
end

function test_transform(t)
    print("Lua modifying Transform")

    t.position = vec3(0, 0, 0)
    t.rotation = vec3(1, 0, 0)
    t.angleRotationRadians = -1
    t.scale = vec3(0.02, 0.02, 0.02)

    print("Transform values:")
    print(
        "Position:",
        t.position.x, t.position.y, t.position.z
    )
    print(
        "Rotation:",
        t.rotation.x, t.rotation.y, t.rotation.z
    )
    print(
        "Angle (rad):",
        t.angleRotationRadians
    )
    print(
        "Scale:",
        t.scale.x, t.scale.y, t.scale.z
    )
end

)";

static MTRD::MotardaEng* g_engine = nullptr;

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw error: %s\n", description);
}

int MTRD::main() {

    // --- Rand seed ---
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    // --- *** ---

    // --- Create engine ---
    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda OBJ Viewer");
    if (!maybeEng.has_value()) return 1;
    auto& eng = maybeEng.value();
    g_engine = &eng;
    // --- *** ---
 
    // --- Camera ---
    MTRD::Camera camera = MTRD::Camera::CreateCamera(eng.windowGetSizeRatio());
    camera.setPosition(glm::vec3(0.f, 1.f, 5.f));
    camera.setTarget(glm::vec3(0.f, 0.f, 0.f));
    // --- *** ---

    // --- Setup Engine ---
    eng.SetDebugMode(true);
    eng.SetRenderType(MotardaEng::RenderType::Base, camera);
    eng.windowSetErrorCallback(error_callback);
    // --- *** ---

    // --- Load Objs ---
    std::vector <const char*> objsRoutes = {
        "indoor_plant_02.obj",
        "12140_Skull_v3_L2.obj"
    };
    std::atomic<bool> objsLoaded = false;

    std::vector<ObjItem> ObjList;
    ObjList.push_back(ObjItem());
    // --- *** ---

    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.new_usertype<glm::vec2>(
        "vec2_t",
        "x", &glm::vec2::x,
        "y", &glm::vec2::y
    );
    lua.new_usertype<glm::vec3>(
        "vec3_t",
        "x", &glm::vec3::x,
        "y", &glm::vec3::y,
        "z", &glm::vec3::z
    );

    // Vec3 binding
    lua.set_function("vec3", [](float x, float y, float z) {
        return glm::vec3(x, y, z);
    });

    // Transform binding
    lua.new_usertype<MTRD::TransformComponent>(
        "Transform",
        "position", &MTRD::TransformComponent::position,
        "rotation", &MTRD::TransformComponent::rotation,
        "angleRotationRadians", &MTRD::TransformComponent::angleRotationRadians,
        "scale", &MTRD::TransformComponent::scale
    );

    // Movement binding
    lua.new_usertype<MTRD::MovementComponent>(
        "Movement",
        "position", &MTRD::MovementComponent::position,
        "rotation", &MTRD::MovementComponent::rotation,
        "scale", &MTRD::MovementComponent::scale,
        "angleRotationRadians", &MTRD::MovementComponent::angleRotationRadians
    );


    // Render(vertex) binding
    lua.new_usertype<MTRD::Vertex>(
        "Vertex",
        "position", &MTRD::Vertex::position,
        "uv", &MTRD::Vertex::uv,
        "normal", &MTRD::Vertex::normal
    );
    // Render(Render) binding
    lua.new_usertype<MTRD::RenderComponent>(
        "Render",
        "position", &MTRD::RenderComponent::meshes_,
        "uv", &MTRD::RenderComponent::materials_
    );

    // Camera binding
    lua.new_usertype<MTRD::Camera>(
        "Camera",
        "moveForward", &MTRD::Camera::moveForward,
        "moveBackward", &MTRD::Camera::moveBackward,
        "moveLeft", &MTRD::Camera::moveLeft,
        "moveRight", &MTRD::Camera::moveRight,
        "moveUp", &MTRD::Camera::moveUp,
        "moveDown", &MTRD::Camera::moveDown,
        "rotate", &MTRD::Camera::rotate
    );

    lua.create_named_table("Input");

    lua["Input"]["isKeyPressed"] = [](int key) {
        if (!g_engine) return false;
        return g_engine->inputIsKeyPressed(
            static_cast<Input::Keyboard>(key)
        );
    };

    // Key constants

    lua["KEY_W"] = (int)Input::Keyboard::W;
    lua["KEY_S"] = (int)Input::Keyboard::S;
    lua["KEY_A"] = (int)Input::Keyboard::A;
    lua["KEY_D"] = (int)Input::Keyboard::D;
    lua["KEY_Q"] = (int)Input::Keyboard::Q;
    lua["KEY_E"] = (int)Input::Keyboard::E;
    lua["KEY_R"] = (int)Input::Keyboard::R;
    lua["KEY_T"] = (int)Input::Keyboard::T;

    lua["KEY_I"] = (int)Input::Keyboard::I;
    lua["KEY_K"] = (int)Input::Keyboard::K;
    lua["KEY_J"] = (int)Input::Keyboard::J;
    lua["KEY_L"] = (int)Input::Keyboard::L;

    lua["KEY_U"] = (int)Input::Keyboard::U;
    lua["KEY_O"] = (int)Input::Keyboard::O;

    lua["KEY_N"] = (int)Input::Keyboard::N;
    lua["KEY_M"] = (int)Input::Keyboard::M;



    sol::load_result chunk = lua.load(lua_program);
    if (!chunk.valid()) {
        sol::error err = chunk;
        std::cout << err.what() << std::endl;
        return 1;
    }
    chunk();


    // async obj load
    eng.enqueueTask([&]() {
        ObjList = eng.loadObjs(objsRoutes);
        objsLoaded = true;
        }
    );
    // --- *** ---

    // --- Drawable transform additions ---
    float ratio = eng.windowGetSizeRatio();
    float movSpeed = 0.05f;
    // --- *** ---


    // --- Ecs ---
    ECSManager ecs;

    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();
    ecs.AddComponentType<MTRD::MovementComponent>();
    // --- *** ---

    float frameTime = 0;
    bool firstTime = true;

    glm::mat4x4 vp, model;


    sol::protected_function lua_update_player = lua["update_player"];
    sol::protected_function lua_update_camera = lua["update_camera"];
   
    // Camera and movSpeed exposed

    lua["camera"] = &camera;
    lua["cameraSpeed"] = movSpeed;

    // --
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            size_t entity = ecs.AddEntity();

            float scl = 0.01f + rand() / (float)RAND_MAX * 0.06f;
            MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(entity);
            t->position = glm::vec3(-3.0f + (y * 0.6), -2.0f + (x * 0.4f), 0.0f);
            t->rotation = glm::vec3(0.0f);
            t->scale = glm::vec3(scl);

            MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(entity);
            r->meshes_ = &ObjList[0].meshes;
            r->materials_ = &ObjList[0].materials;

            MTRD::MovementComponent* m = ecs.AddComponent<MTRD::MovementComponent>(entity);
            m->position = glm::vec3(std::rand() % 3 - 1, std::rand() % 3 - 1, 0);
            m->rotation = glm::vec3(std::rand() % 3 - 1, std::rand() % 3 - 1, std::rand() % 3 - 1);
            m->scale = glm::vec3(0.0f);
        }
    }

    size_t player = ecs.AddEntity();

    MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(player);
    t->position = glm::vec3(0);
    t->rotation = glm::vec3(1, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(0.02f);

    MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(player);
    r->meshes_ = &ObjList[1].meshes;
    r->materials_ = &ObjList[1].materials;

    MTRD::MovementComponent* m = ecs.AddComponent<MTRD::MovementComponent>(player);
    m->position = glm::vec3(0);
    m->rotation = glm::vec3(0, 0, 1);
    m->scale = glm::vec3(0.0f);
    m->shouldConstantMove = false;
    // --- *** ---

    // --- Main window bucle ---
    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();

        if (!objsLoaded) {
            printf("Cargando maya...\n");
            eng.windowEndFrame();
            continue;

        }
        else if (firstTime) {
            firstTime = false;
            eng.windowLoadAllMaterials(ObjList);

        }

        // --- Input to move camera ---
        if (lua_update_camera.valid()) {
            sol::protected_function_result r =
                lua_update_camera(camera, movSpeed);

            if (!r.valid()) {
                sol::error err = r;
                std::cout << "[Lua Camera Error] " << err.what() << std::endl;
            }
        }
        // --- *** ---


        // --- Input to move player ---
        if (lua_update_player.valid()) {
            sol::protected_function_result r = lua_update_player(m);
            if (!r.valid()) {
                sol::error err = r;
                std::cout << "[Lua Error] " << err.what() << std::endl;
            }
        }
        //eng.RenderScene(ecs, camera); He comentado esto porq crashea
        eng.windowEndFrame();

        frameTime = eng.windowGetLastFrameTime();
        //printf("Last frame time: %.4f secs\n", frameTime);
    }

    return 0;
}