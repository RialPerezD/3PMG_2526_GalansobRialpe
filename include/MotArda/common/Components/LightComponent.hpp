#pragma once
#include "../deps/glm-master/glm/glm.hpp"
#include <vector>

namespace MTRD {

    struct AmbientLight {
        glm::vec3 color_;
        float intensity_;

        AmbientLight();
        AmbientLight(const glm::vec3& color, float intensity);
    };

    struct DirectionalLight {
        glm::vec3 direction_;
        glm::vec3 color_;
        float intensity_;

        DirectionalLight();
        DirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity);
    
        glm::mat4 getLightSpaceMatrix(
            float orthoSize = 15.0f,
            float nearPlane = 3.0f,
            float farPlane = 100.0f
        ) const;
    };

    struct SpotLight {
        glm::vec3 position_;
        glm::vec3 direction_;
        glm::vec3 color_;
        float intensity_;
        float cutOff_;
        float outerCutOff_;
        float constant_;
        float linear_;
        float quadratic_;

        float wRatio_;

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
            float quadratic,
			float wRatio
        );

        glm::mat4 getLightSpaceMatrix(float orthoSize = 25.0f, float nearPlane = 0.1f, float farPlane = 100.0f);
    };

    struct LightComponent {
        bool hasAmbient_;
        AmbientLight ambient_;

        std::vector<DirectionalLight> directionalLights;
        std::vector<SpotLight> spotLights;

        LightComponent();
    };
}
