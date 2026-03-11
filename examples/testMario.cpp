#include "MotArda/common/Engine.hpp"
#include "MotArda/common/ObjLoader.hpp"
#include <MotArda/common/Ecs.hpp>
#include <MotArda/common/Camera.hpp>
#include <MotArda/common/Components/LightComponent.hpp>

#include <memory>

#include <MotArda/win64/Systems/RenderLightsSystem.hpp>
#include <MotArda/win64/Systems/ShadowMapSystem.hpp>
#include <MotArda/win64/Texture.hpp>

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw error: %s\n", description);
}

struct Platform {
    float x, y;
    float width, height;
};

int MTRD::main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Mario Bros - MotArda Engine");
    if (!maybeEng.has_value()) return 1;

    auto& eng = maybeEng.value();

    MTRD::Camera& camera = eng.getCamera();
    camera.setPosition(glm::vec3(0, 0, 20));
    camera.setTarget(glm::vec3(0, 0, 0));

    eng.SetDebugMode(true);
    eng.SetRenderType(MotardaEng::RenderType::Bidimensional);
    eng.windowSetErrorCallback(error_callback);

    ECSManager& ecs = eng.getEcs();

    GLuint marioTexture = Texture::LoadTexture("../assets/textures/mario/MarioSheet.png");
    GLuint backgroundTexture = Texture::LoadTexture("../assets/textures/mario/background.png");

    Sprite backgroundSprite = eng.generateSprite(backgroundTexture, -16, 12);
    TransformComponent* backgroundTransform = ecs.GetComponent<TransformComponent>(backgroundSprite.getId());
    backgroundTransform->position = glm::vec3(0, 0, -2.0f);

    Sprite marioSprite = eng.generateSpriteSheet(marioTexture, -1, 64, 32, 4, 2, 5);
    TransformComponent* marioTransform = ecs.GetComponent<TransformComponent>(marioSprite.getId());
    marioTransform->position = glm::vec3(0.0f, 0.0f, 0.0f);

    std::vector<Platform> platforms;

    float groundY = -3.0f;
    for (float x = -12.0f; x <= 12.0f; x += 1.0f) {
        platforms.push_back({x, groundY, 1.0f, 1.0f});
    }

    float platformPositions[][3] = {
        {-2.0f, 1.0f}//, {-1.0f, -1.0f}, {0.0f, -1.0f}, {1.0f, -1.0f}, {2.0f, -1.0f}, {3.0f, -1.0f},
        //{3.0f, 0.5f}, {4.0f, 0.5f},
        //{-4.5f, 0.5f}, {-3.5f, 0.5f}, {-2.5f, 0.5f},
        //{-6.5f, 1.5f}, {-5.5f, 1.5f},
        //{6.5f, 1.5f}, {7.5f, 1.5f},
    };

    for (int i = 0; i < 15; i++) {
        platforms.push_back({platformPositions[i][0], platformPositions[i][1], 1.0f, 1.0f});
    }

    float movSpeed = 0.15f;
    float velocityY = 0.0f;
    float gravity = -0.015f;
    float jumpForce = 0.4f;
    bool canJump = false;
    bool moving = false;
    float animTimer = 0.0f;

    float marioWidth = 0.8f;
    float marioHeight = 0.8f;

    while (!eng.windowShouldClose()) {
        eng.windowInitFrame();

        if (eng.inputIsKeyPressed(Input::Keyboard::A)) {
            marioTransform->position.x -= movSpeed;
            moving = true;
        }
        if (eng.inputIsKeyPressed(Input::Keyboard::D)) {
            marioTransform->position.x += movSpeed;
            moving = true;
        }

        if (eng.inputIsKeyPressed(Input::Keyboard::W) && canJump) {
            velocityY = jumpForce;
            canJump = false;
        }

        if (!eng.inputIsKeyPressed(Input::Keyboard::A) && 
            !eng.inputIsKeyPressed(Input::Keyboard::D)) {
            moving = false;
        }

        velocityY += gravity;
        marioTransform->position.y += velocityY;

        bool onGround = false;
        for (const auto& plat : platforms) {
            float platLeft = plat.x - plat.width * 0.5f;
            float platRight = plat.x + plat.width * 0.5f;
            float platTop = plat.y + plat.height * 0.5f;
            float platBottom = plat.y - plat.height * 0.5f;

            float marioLeft = marioTransform->position.x - marioWidth * 0.5f;
            float marioRight = marioTransform->position.x + marioWidth * 0.5f;
            float marioBottom = marioTransform->position.y - marioHeight * 0.5f;
            float marioTop = marioTransform->position.y + marioHeight * 0.5f;

            if (marioRight > platLeft && marioLeft < platRight) {
                if (marioBottom <= platTop && marioBottom >= platTop - 0.3f && velocityY < 0) {
                    marioTransform->position.y = platTop + marioHeight * 0.5f;
                    velocityY = 0.0f;
                    onGround = true;
                }
            }
        }

        if (onGround) {
            canJump = true;
        }

        if (marioTransform->position.y < -10.0f) {
            marioTransform->position = glm::vec3(0.0f, 0.0f, 0.0f);
            velocityY = 0.0f;
        }

        //camera.setTarget(glm::vec3(marioTransform->position.x, marioTransform->position.y + 2.0f, 0.0f));

        animTimer += eng.windowGetLastFrameTime();
        if (moving && animTimer >= 0.15f) {
            marioSprite.nextFrame();
            animTimer = 0.0f;
        }

        eng.RenderScene();
        eng.windowEndFrame();
    }

    return 0;
}
