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

        float layer_;
        size_t ecsEntityIdentifyer_;

    public:
        Sprite(size_t ecsEntityIdentifyer, float layer);

        size_t getId() { return ecsEntityIdentifyer_; }
    };
}