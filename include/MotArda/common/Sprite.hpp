#pragma once
#include <string>
#include <vector>
#include "../deps/glm-master/glm/glm.hpp"
#include "glad/glad.h"
#include <MotArda/win64/Mesh.hpp>
#include <MotArda/win64/Material.hpp>

namespace MTRD {
    class Sprite {
        Sprite(const char* spriteRoute, size_t ecsEntityIdentifyer_);

        int layer_;

    public:
        static Sprite GenerateSprite(const char* spriteRoute, size_t ecsEntityIdentifyer_);

        size_t ecsEntityIdentifyer_;
    };
}