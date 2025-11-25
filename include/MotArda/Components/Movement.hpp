#pragma once
#include "../deps/glm-master/glm/glm.hpp"

namespace MTRD {

    struct Movement {
        glm::vec3 position;
        glm::vec3 scale;
        glm::vec3 rotation;
        float angleRotationRadians;

        bool shouldConstantMove;

        Movement();
        Movement(
            const glm::vec3& pos,
            const glm::vec3& scl,
            const glm::vec3& rot,
            float angleRotationRadians,
            bool should
        );
    };
}
