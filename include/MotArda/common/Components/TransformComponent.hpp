#pragma once
#include "../deps/glm-master/glm/glm.hpp"

namespace MTRD {

    struct TransformComponent {
        glm::vec3 position;
        glm::vec3 scale;
        glm::vec3 rotation;
        float angleRotationRadians;

        TransformComponent();
        TransformComponent(
            const glm::vec3& pos,
            const glm::vec3& scl,
            const glm::vec3& rot,
            float angleRotationRadians);
    };
}
