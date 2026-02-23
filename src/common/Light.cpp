#include <MotArda/common/Light.hpp>

namespace MTRD {
    AmbientLight::AmbientLight()
        : color(glm::vec3(1.0f))
        , intensity(0.1f)
    {
    }

    AmbientLight::AmbientLight(const glm::vec3& color, float intensity)
        : color(color)
        , intensity(intensity)
    {
    }

    DirectionalLight::DirectionalLight()
        : direction(glm::vec3(0.0f, -1.0f, 0.0f))
        , color(glm::vec3(1.0f))
        , intensity(1.0f)
    {
    }

    DirectionalLight::DirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity)
        : direction(direction)
        , color(color)
        , intensity(intensity)
    {
    }

    SpotLight::SpotLight()
        : position(glm::vec3(0.0f))
        , direction(glm::vec3(0.0f, -1.0f, 0.0f))
        , color(glm::vec3(1.0f))
        , intensity(1.0f)
        , cutOff(glm::cos(glm::radians(12.5f)))
        , outerCutOff(glm::cos(glm::radians(17.5f)))
        , constant(1.0f)
        , linear(0.09f)
        , quadratic(0.032f)
    {
    }

    SpotLight::SpotLight(
        const glm::vec3& position,
        const glm::vec3& direction,
        const glm::vec3& color,
        float intensity,
        float cutOff,
        float outerCutOff,
        float constant,
        float linear,
        float quadratic
    )
        : position(position)
        , direction(direction)
        , color(color)
        , intensity(intensity)
        , cutOff(cutOff)
        , outerCutOff(outerCutOff)
        , constant(constant)
        , linear(linear)
        , quadratic(quadratic)
    {
    }

    LightComponent::LightComponent()
        : hasAmbient(false)
        , hasDirectional(false)
        , hasSpot(false)
        , ambient()
        , directional()
        , spot()
    {
    }
}
