#include "MotArda/common/Engine.hpp"
#include "MotArda/common/ObjLoader.hpp"
#include <MotArda/common/Ecs.hpp>
#include <MotArda/common/Camera.hpp>
#include <MotArda/common/Components/LightComponent.hpp>

#include <memory>

#include <MotArda/common/Systems/TraslationSystem.hpp>
#include <MotArda/win64/Systems/RenderLightsSystem.hpp>
#include <MotArda/win64/Systems/ShadowMapSystem.hpp>

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
    // --- *** ---


    // --- Setup engine info ---
    eng.SetDebugMode(true);
    eng.SetRenderType(MotardaEng::RenderType::Bidimensional);
    eng.windowSetErrorCallback(error_callback);
    // --- *** ---


    // --- Create drawable entitys ---
    ECSManager& ecs = eng.getEcs();
    // --- *** ---

    Sprite sprite = eng.generateSprite("", 1);
    

    // --- Main window bucle ---
    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();

        // Generate shadow map
        eng.RenderScene();
        // --- *** ---

        eng.windowEndFrame();
    }

    return 0;
}