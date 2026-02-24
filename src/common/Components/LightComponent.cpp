#include <MotArda/common/Components/LightComponent.hpp>
#include "../../../deps/glm-master/glm/ext/matrix_clip_space.hpp"
#include "../../../deps/glm-master/glm/ext/matrix_transform.hpp"

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

    glm::mat4 DirectionalLight::getLightSpaceMatrix(
        float orthoSize,
        float nearPlane,
        float farPlane
    ) const {
        glm::vec3 lightPos = -glm::normalize(direction) * 10.0f;
        glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        return lightProjection * lightView;
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

    glm::mat4 SpotLight::getLightSpaceMatrix(const SpotLight& light) {
        float near_plane = 10.0f;
        float far_plane = 50.0f;
        float aspect = 1.0f;

        glm::mat4 lightProjection = glm::perspective(glm::radians(45.0f), aspect, near_plane, far_plane);

        glm::mat4 lightView = glm::lookAt(
            light.position,
            light.position + glm::vec3(0,-1,0),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        return lightProjection * lightView;
    }

    LightComponent::LightComponent()
        : hasAmbient(false)
        , ambient()
    {
    }
}
