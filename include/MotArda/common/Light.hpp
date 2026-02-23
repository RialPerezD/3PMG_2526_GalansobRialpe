#pragma once
#include "../deps/glm-master/glm/glm.hpp"

namespace MTRD {

    struct AmbientLight {
        glm::vec3 color;
        float intensity;

        AmbientLight();
        AmbientLight(const glm::vec3& color, float intensity);
    };

    struct DirectionalLight {
        glm::vec3 direction;
        glm::vec3 color;
        float intensity;

        DirectionalLight();
        DirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity);
    };

    struct SpotLight {
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 color;
        float intensity;
        float cutOff;
        float outerCutOff;
        float constant;
        float linear;
        float quadratic;

        SpotLight();
        SpotLight(
            const glm::vec3& position,
            const glm::vec3& direction,
            const glm::vec3& color,
            float intensity,
            float cutOff,
            float outerCutOff,
            float constant,
            float linear,
            float quadratic
        );
    };

    struct LightComponent {
        bool hasAmbient;
        bool hasDirectional;
        bool hasSpot;

        AmbientLight ambient;
        DirectionalLight directional;
        SpotLight spot;

        LightComponent();
    };
}
