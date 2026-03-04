#include "MotArda/common/Engine.hpp"
#include "MotArda/common/ObjLoader.hpp"
#include <MotArda/common/Ecs.hpp>
#include <MotArda/common/Camera.hpp>
#include <MotArda/common/Components/LightComponent.hpp>

#include <memory>

#include <MotArda/common/Systems/TraslationSystem.hpp>
#include <MotArda/win64/Systems/RenderLightsSystem.hpp>
#include <MotArda/win64/Systems/ShadowMapSystem.hpp>
#include <MotArda/win64/Texture.hpp>

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


    // --- Camera ---
    MTRD::Camera& camera = eng.getCamera();
    camera.setPosition(glm::vec3(0, 1, 20));
    camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    float movSpeed = 0.1f;
    // --- *** ---


    // --- Setup engine info ---
    eng.SetDebugMode(true);
    eng.SetRenderType(MotardaEng::RenderType::Bidimensional);
    eng.windowSetErrorCallback(error_callback);
    // --- *** ---


    // --- Create drawable entitys ---
    ECSManager& ecs = eng.getEcs();
    // --- *** ---

    GLuint skullTexture = Texture::LoadTexture("../assets/textures/12140_Skull_v3_L2/Skull.jpg");

    Sprite sprite = eng.generateSprite(skullTexture, 1, 1);
    Sprite sprite2 = eng.generateSprite(skullTexture, 20, 0);
    Sprite sprite3 = eng.generateSprite(skullTexture, 10, 2);

    TransformComponent* spritTransform = ecs.GetComponent<TransformComponent>(sprite.getId());
    

    // --- Main window bucle ---
    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();

        // --- Input to move camera ---
        if (eng.inputIsKeyPressed(Input::Keyboard::W)) spritTransform->position.y += movSpeed;
        if (eng.inputIsKeyPressed(Input::Keyboard::S)) spritTransform->position.y -= movSpeed;
        if (eng.inputIsKeyPressed(Input::Keyboard::A)) spritTransform->position.x -= movSpeed;
        if (eng.inputIsKeyPressed(Input::Keyboard::D)) spritTransform->position.x += movSpeed;
        // --- *** ---

        // Generate shadow map
        eng.RenderScene();
        // --- *** ---

        eng.windowEndFrame();
    }

    return 0;
}