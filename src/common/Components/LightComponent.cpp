#include <MotArda/common/Components/LightComponent.hpp>
#include "../../../deps/glm-master/glm/ext/matrix_clip_space.hpp"
#include "../../../deps/glm-master/glm/ext/matrix_transform.hpp"

namespace MTRD {
    AmbientLight::AmbientLight()
        : color_(glm::vec3(1.0f))
        , intensity_(0.1f)
    {
    }

    AmbientLight::AmbientLight(const glm::vec3& color, float intensity)
        : color_(color)
        , intensity_(intensity)
    {
    }

    DirectionalLight::DirectionalLight()
        : direction_(glm::vec3(0.0f, -1.0f, 0.0f))
        , color_(glm::vec3(1.0f))
        , intensity_(1.0f)
    {
    }

    DirectionalLight::DirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity)
        : direction_(direction)
        , color_(color)
        , intensity_(intensity)
    {
    }

    glm::mat4 DirectionalLight::getLightSpaceMatrix(
        float orthoSize,
        float nearPlane,
        float farPlane
    ) const {
        glm::vec3 lightPos = -glm::normalize(direction_) * 10.0f;
        glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        return lightProjection * lightView;
    }

    SpotLight::SpotLight()
        : position_(glm::vec3(0.0f))
        , direction_(glm::vec3(0.0f, -1.0f, 0.0f))
        , color_(glm::vec3(1.0f))
        , intensity_(1.0f)
        , cutOff_(glm::cos(glm::radians(12.5f)))
        , outerCutOff_(glm::cos(glm::radians(17.5f)))
        , constant_(1.0f)
        , linear_(0.09f)
        , quadratic_(0.032f)
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
        : position_(position)
        , direction_(direction)
        , color_(color)
        , intensity_(intensity)
        , cutOff_(cutOff)
        , outerCutOff_(outerCutOff)
        , constant_(constant)
        , linear_(linear)
        , quadratic_(quadratic)
    {
    }

    glm::mat4 SpotLight::getLightSpaceMatrix() {
        float near_plane = 0.1f;
        float far_plane = 100.0f;
        float aspect = 1.0f;

        float fov = glm::acos(outerCutOff_);
        glm::mat4 lightProjection = glm::perspective(fov, aspect, near_plane, far_plane);

        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 normDir = glm::normalize(direction_);
        if (glm::abs(glm::dot(normDir, up)) > 0.99f) {
            up = glm::vec3(1.0f, 0.0f, 0.0f);
        }

        glm::vec3 temPos = position_ - (glm::vec3(-2.5f, 0, -2.5f) * fov);
        glm::mat4 lightView = glm::lookAt(
            temPos,
            temPos + direction_,
            up
        );

        return lightProjection * lightView;
    }

    LightComponent::LightComponent()
        : hasAmbient_(false)
        , ambient_()
    {
    }
}
