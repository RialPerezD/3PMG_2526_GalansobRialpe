#include "MotArda/Engine.hpp"
#include <string>
#include <sol/sol.hpp>

std::string lua_program = R"(

function hello_world_f()
    print("HelloWorld")
end

)";


int MTRD::main() {

    sol::state state;
    state.open_libraries(sol::lib::base, sol::lib::package);

    sol::load_result chunk = state.load(lua_program);

    if (chunk.valid()) {
        chunk();

        sol::protected_function print_Lua = state["hello_world_f"];
        if (print_Lua.valid()) {
            print_Lua();
        }
    }

    return 1;
}