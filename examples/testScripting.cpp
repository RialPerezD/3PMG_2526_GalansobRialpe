#include "MotArda/Engine.hpp"
#include <MotArda/Ecs.hpp>
#include <string>
#include <sol/sol.hpp>
#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"

std::string lua_program = R"(

function hello_world_f()
    print("HelloWorld")
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

int MTRD::main() {

    sol::state state;
    state.open_libraries(sol::lib::base, sol::lib::package);


    // glm::vec3 no existe en Lua, así que hay que registrarlo como UserType 
    state.new_usertype<glm::vec3>(
        "vec3_t",
        "x", &glm::vec3::x,
        "y", &glm::vec3::y,
        "z", &glm::vec3::z
    );

    // Constructor global necesario porq si no, Lua piensa que Vec3 es una tabla
    state.set_function("vec3", [](float x, float y, float z) {
        return glm::vec3(x, y, z);
    });


    // Hay que exponer Transform para poder acceder a él desde Scripting 
    state.new_usertype<MTRD::Transform>(
        "Transform",
        "position", &MTRD::Transform::position,
        "rotation", &MTRD::Transform::rotation,
        "angleRotationRadians", &MTRD::Transform::angleRotationRadians,
        "scale", &MTRD::Transform::scale
    );

    // Cacho de código para que en el caso de no funcionar, te diga cual es el error
    // (StackOverflow goes brrrr)
    sol::load_result chunk = state.load(lua_program);
    if (!chunk.valid()) {
        sol::error err = chunk;
        std::cout << err.what() << std::endl;
        return 1;
    }
    chunk();


    ECSManager ecs;

    ecs.AddComponentType<MTRD::Transform>();
    ecs.AddComponentType<MTRD::Render>();
    ecs.AddComponentType<MTRD::Movement>();


    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda window");
    if (!maybeEng.has_value())
        return 1;

    auto& eng = maybeEng.value();


    size_t entity = ecs.AddEntity();

    MTRD::Transform* t = ecs.AddComponent<MTRD::Transform>(entity);
    ecs.AddComponent<MTRD::Render>(entity);
    ecs.AddComponent<MTRD::Movement>(entity);

    // Llama a la función que se llama "test_transform", porq tmb tenemos la función
    // de Print
    sol::protected_function test_transform = state["test_transform"];
    if (test_transform.valid()) {
        sol::protected_function_result result = test_transform(t);

        if (!result.valid()) {
            sol::error err = result;
            std::cout << "[Lua Error] " << err.what() << std::endl;
        }
    }


    while (!eng.windowShouldClose()) {
        eng.windowEndFrame();

    }

    return 0;
}
