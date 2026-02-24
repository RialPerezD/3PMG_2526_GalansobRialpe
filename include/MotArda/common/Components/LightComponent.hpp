#pragma once
#include "../deps/glm-master/glm/glm.hpp"
#include <vector>

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
    
        glm::mat4 getLightSpaceMatrix(
            float orthoSize = 15.0f,
            float nearPlane = 3.0f,
            float farPlane = 100.0f
        ) const;
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

        glm::mat4 getLightSpaceMatrix(const SpotLight& light);
    };

    struct LightComponent {
        bool hasAmbient;
        AmbientLight ambient;

        std::vector<DirectionalLight> directionalLights;
        std::vector<SpotLight> spotLights;

        LightComponent();
    };
}
