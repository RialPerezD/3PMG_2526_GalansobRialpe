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
    float x, y, width, height;
};

bool checkGroundCollision(const glm::vec3& pos, const Platform& plat, float w, float h) {
    float pL = plat.x - plat.width * 0.5f, pR = plat.x + plat.width * 0.5f;
    float pT = plat.y + plat.height * 0.5f;
    float mL = pos.x - w * 0.5f, mR = pos.x + w * 0.5f, mB = pos.y - h * 0.5f;

    return (mR > pL && mL < pR && mB <= pT && mB >= pT - 0.3f);
}

int MTRD::main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Mario Bros - MotArda Engine");
    if (!maybeEng.has_value()) return 1;

    auto& eng = maybeEng.value();
    eng.getCamera().setPosition({ 0, 0, 20 });
    eng.getCamera().setTarget({ 0, 0, 0 });
    eng.SetDebugMode(true);
    eng.SetRenderType(MotardaEng::RenderType::Bidimensional);
    eng.windowSetErrorCallback(error_callback);

    ECSManager& ecs = eng.getEcs();
    GLuint marioTex = Texture::LoadTexture("../assets/textures/mario/MarioSheet.png");
    GLuint bgTex = Texture::LoadTexture("../assets/textures/mario/background.png");

    Sprite bg = eng.generateSprite(bgTex, -16, 12);
    ecs.GetComponent<TransformComponent>(bg.getId())->position = { 0, 0, -2.0f };

    Sprite mario = eng.generateSpriteSheet(marioTex, -1, 64, 32, 4, 2, 5);
    auto* mTrans = ecs.GetComponent<TransformComponent>(mario.getId());
    mTrans->position = { 0.0f, 0.0f, 0.0f };

    std::vector<Platform> platforms;
    for (float x = -12.0f; x <= 12.0f; x += 1.0f) platforms.push_back({ x, -4.2f, 1.0f, 1.0f });

    std::vector<glm::vec2> extraPos = { {-4.0f, 2.5f}, {-2.0f, 2.5f}, {-1.0f, 2.5f} };
    for (auto& p : extraPos) platforms.push_back({ p.x, p.y, 1.0f, 1.0f });

    float velY = 0, gravity = -0.015f, jump = 0.5f, speed = 0.15f, animTimer = 0;
    float mW = 0.8f, mH = 0.8f;
    bool canJump = false, moving = false;

    while (!eng.windowShouldClose()) {
        eng.windowInitFrame();

        moving = false;
        if (eng.inputIsKeyPressed(Input::Keyboard::A)) {
            float temPos = mTrans->position.x - speed;
            if (temPos > -7) {
                mTrans->position.x = temPos;
                moving = true;
            }
        }else if (eng.inputIsKeyPressed(Input::Keyboard::D)) {
            float temPos = mTrans->position.x + speed;
            if (temPos < 7) {
                mTrans->position.x = temPos;
                moving = true;
            }
        }

        if (eng.inputIsKeyPressed(Input::Keyboard::W) && canJump) { velY = jump; canJump = false; }

        velY += gravity;
        mTrans->position.y += velY;

        bool onGround = false;
        for (const auto& plat : platforms) {
            if (velY < 0 && checkGroundCollision(mTrans->position, plat, mW, mH)) {
                mTrans->position.y = (plat.y + plat.height * 0.5f) + mH * 0.5f;
                velY = 0;
                onGround = true;
                break;
            }
        }
        canJump = onGround;

        if (mTrans->position.y < -10.0f) { mTrans->position = { 0, 0, 0 }; velY = 0; }

        animTimer += eng.windowGetLastFrameTime();
        if (moving && animTimer >= 0.15f) { mario.nextFrame(); animTimer = 0; }

        eng.RenderScene();
        eng.windowEndFrame();
    }

    return 0;
}