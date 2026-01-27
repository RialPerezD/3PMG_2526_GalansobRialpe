#include "MotArda/Engine.hpp"
#include "MotArda/ObjLoader.hpp"
#include <MotArda/Ecs.hpp>
#include <MotArda/Camera.hpp>
#include <MotArda/Systems.hpp>

#include <sol/sol.hpp>
#include <memory>
#include <atomic>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"

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


    // --- Create window ---
    eng.windowSetDebugMode(true);
    eng.windowSetErrorCallback(error_callback);
    eng.windowCreateContext();
    eng.windowSetSwapInterval(1);
    // --- *** ---

    sol::state lua;
    lua.open_libraries(sol::lib::base);

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

    lua.new_usertype<MTRD::Transform>(
        "Transform",
        "position", &MTRD::Transform::position,
        "rotation", &MTRD::Transform::rotation,
        "angleRotationRadians", &MTRD::Transform::angleRotationRadians,
        "scale", &MTRD::Transform::scale
    );

    // Movement binding
    lua.new_usertype<MTRD::Movement>(
        "Movement",
        "position", &MTRD::Movement::position,
        "rotation", &MTRD::Movement::rotation,
        "scale", &MTRD::Movement::scale,
        "angleRotationRadians", &MTRD::Movement::angleRotationRadians
    );

    lua.create_named_table("Input");

    lua["Input"]["isKeyPressed"] = [](int key) {
        if (!g_engine) return false;
        return g_engine->inputIsKeyPressed(
            static_cast<Input::Keyboard>(key)
        );
        };

    // Key constants
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

    // --- Load Objs ---
    std::vector <const char*> objsRoutes = {
        "indoor_plant_02.obj",
        "12140_Skull_v3_L2.obj"
    };

    std::atomic<bool> objsLoaded = false;
    std::vector<MTRD::Window::ObjItem> objItemList;
    // --- *** ---


    // async obj load
    eng.enqueueTask([&]() {
        objItemList = eng.loadObjs(objsRoutes);
        objsLoaded = true;
        }
    );
    // --- *** ---

    // --- Drawable transform additions ---
    float ratio = eng.windowGetSizeRatio();
    float movSpeed = 0.05f;
    // --- *** ---

    // --- Camera ---
    MTRD::Camera camera(
        glm::vec3(0.f, 0.f, 5.f),
        glm::vec3(0.f, 0.f, 0.f),
        glm::vec3(0.f, 1.f, 0.f),
        glm::radians(45.f),
        ratio,
        0.1f,
        100.f
    );

    camera.updateAll();
    // --- *** ---

    // --- Systems ---
    Systems systems;
    // --- *** ---

    // --- Ecs ---
    ECSManager ecs;
    // --- *** ---

    float frameTime = 0;
    bool firstTime = true;
    MTRD::Transform* t;
    MTRD::Render* r;
    MTRD::Movement* m;
    glm::mat4x4 vp, model;

    // --- Setup uniforms ---
    std::vector<Window::UniformAttrib> uniforms = {
        {"VP", -1, Window::UniformTypes::Mat4, glm::value_ptr(vp)},
        {"model", -1, Window::UniformTypes::Mat4, glm::value_ptr(model)},
    };

    std::vector<Window::VertexAttrib> attributes = {
        { "position", 3, offsetof(Vertex, position) },
        { "uv", 2, offsetof(Vertex, uv) },
        { "normal", 3, offsetof(Vertex, normal) }
    };
    // --- *** ---

    sol::protected_function lua_update_player = lua["update_player"];

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

            eng.windowLoadAllMaterials(objItemList);
            if (objItemList.size() == 0) return 1;
            // --- *** ---

            // --- Create drawable entitys ---
            ecs.AddComponentType<MTRD::Transform>();
            ecs.AddComponentType<MTRD::Render>();
            ecs.AddComponentType<MTRD::Movement>();

            for (int y = 0; y < 10; y++) {
                for (int x = 0; x < 10; x++) {
                    size_t entity = ecs.AddEntity();

                    float scl = 0.01f + rand() / (float)RAND_MAX * 0.06f;
                    t = ecs.AddComponent<MTRD::Transform>(entity);
                    t->position = glm::vec3(-3.0f + (y * 0.6), -2.0f + (x * 0.4f), 0.0f);
                    t->rotation = glm::vec3(0.0f);
                    t->scale = glm::vec3(scl);

                    r = ecs.AddComponent<MTRD::Render>(entity);
                    r->shapes = &objItemList[0].shapes;
                    r->materials = &objItemList[0].materials;

                    m = ecs.AddComponent<MTRD::Movement>(entity);
                    m->position = glm::vec3(std::rand() % 3 - 1, std::rand() % 3 - 1, 0);
                    m->rotation = glm::vec3(std::rand() % 3 - 1, std::rand() % 3 - 1, std::rand() % 3 - 1);
                    m->scale = glm::vec3(0.0f);
                }
            }

            size_t player = ecs.AddEntity();

            t = ecs.AddComponent<MTRD::Transform>(player);
            t->position = glm::vec3(0);
            t->rotation = glm::vec3(1, 0, 0);
            t->angleRotationRadians = -1;
            t->scale = glm::vec3(0.02f);

            r = ecs.AddComponent<MTRD::Render>(player);
            r->shapes = &objItemList[1].shapes;
            r->materials = &objItemList[1].materials;

            m = ecs.AddComponent<MTRD::Movement>(player);
            m->position = glm::vec3(0);
            m->rotation = glm::vec3(0, 0, 1);
            m->scale = glm::vec3(0.0f);
            m->shouldConstantMove = false;
            // --- *** ---


            // --- Load shaders ---
            const char* vertex_shader = eng.loadShaderFile("../assets/shaders/textured_obj_vertex.txt");
            const char* fragment_shader = eng.loadShaderFile("../assets/shaders/textured_obj_fragment.txt");
            // --- *** ---


            // --- Setup Window ---
            eng.windowOpenglSetup(
                ecs.GetComponentList<Render>(),
                vertex_shader,
                fragment_shader,
                uniforms,
                attributes
            );
            //la ventana no gestiona el shader, el render es el q deberia hacerlo
            // --- *** ---
        }

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


         // --- Player input (Lua) ---
        if (lua_update_player.valid()) {
            sol::protected_function_result r = lua_update_player(m);
            if (!r.valid()) {
                sol::error err = r;
                std::cout << "[Lua Error] " << err.what() << std::endl;
            }
        }


        // --- update vp ---
        vp = camera.getViewProj();
        // --- *** ---


        // --- Setup uniforms and draw ---
        systems.RunRenderSystemWithTraslations(ecs, eng, uniforms, model);
        // --- *** ---

        eng.windowEndFrame();

        frameTime = eng.windowGetLastFrameTime();
        //printf("Last frame time: %.4f secs\n", frameTime);
    }

    return 0;
}



//
//#include "MotArda/Engine.hpp"
//#include <MotArda/Ecs.hpp>
//#include <string>
//#include <sol/sol.hpp>
//#include "../deps/glm-master/glm/glm.hpp"
//#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
//#include "../deps/glm-master/glm/gtc/type_ptr.hpp"
//
//std::string lua_program = R"(
//
//function hello_world_f()
//    print("HelloWorld")
//end
//
//function test_transform(t)
//    print("Lua modifying Transform")
//
//    t.position = vec3(0, 0, 0)
//    t.rotation = vec3(1, 0, 0)
//    t.angleRotationRadians = -1
//    t.scale = vec3(0.02, 0.02, 0.02)
//
//    print("Transform values:")
//    print(
//        "Position:",
//        t.position.x, t.position.y, t.position.z
//    )
//    print(
//        "Rotation:",
//        t.rotation.x, t.rotation.y, t.rotation.z
//    )
//    print(
//        "Angle (rad):",
//        t.angleRotationRadians
//    )
//    print(
//        "Scale:",
//        t.scale.x, t.scale.y, t.scale.z
//    )
//end
//
//)";
//
//int MTRD::main() {
//
//    sol::state state;
//    state.open_libraries(sol::lib::base, sol::lib::package);
//
//
//    // glm::vec3 no existe en Lua, así que hay que registrarlo como UserType 
//    state.new_usertype<glm::vec3>(
//        "vec3_t",
//        "x", &glm::vec3::x,
//        "y", &glm::vec3::y,
//        "z", &glm::vec3::z
//    );
//
//    // Constructor global necesario porq si no, Lua piensa que Vec3 es una tabla
//    state.set_function("vec3", [](float x, float y, float z) {
//        return glm::vec3(x, y, z);
//    });
//
//
//    // Hay que exponer Transform para poder acceder a él desde Scripting 
//    state.new_usertype<MTRD::Transform>(
//        "Transform",
//        "position", &MTRD::Transform::position,
//        "rotation", &MTRD::Transform::rotation,
//        "angleRotationRadians", &MTRD::Transform::angleRotationRadians,
//        "scale", &MTRD::Transform::scale
//    );
//
//    // Cacho de código para que en el caso de no funcionar, te diga cual es el error
//    // (StackOverflow goes brrrr)
//    sol::load_result chunk = state.load(lua_program);
//    if (!chunk.valid()) {
//        sol::error err = chunk;
//        std::cout << err.what() << std::endl;
//        return 1;
//    }
//    chunk();
//
//
//    ECSManager ecs;
//
//    ecs.AddComponentType<MTRD::Transform>();
//    ecs.AddComponentType<MTRD::Render>();
//    ecs.AddComponentType<MTRD::Movement>();
//
//
//    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda window");
//    if (!maybeEng.has_value())
//        return 1;
//
//    auto& eng = maybeEng.value();
//
//
//    size_t entity = ecs.AddEntity();
//
//    MTRD::Transform* t = ecs.AddComponent<MTRD::Transform>(entity);
//    ecs.AddComponent<MTRD::Render>(entity);
//    ecs.AddComponent<MTRD::Movement>(entity);
//
//    // Llama a la función que se llama "test_transform", porq tmb tenemos la función
//    // de Print
//    sol::protected_function test_transform = state["test_transform"];
//    if (test_transform.valid()) {
//        sol::protected_function_result result = test_transform(t);
//
//        if (!result.valid()) {
//            sol::error err = result;
//            std::cout << "[Lua Error] " << err.what() << std::endl;
//        }
//    }
//
//
//    while (!eng.windowShouldClose()) {
//        eng.windowEndFrame();
//
//    }
//
//    return 0;
//}
