#include <MotArda/Components/TransformComponent.hpp>

MTRD::TransformComponent::TransformComponent()
    : position(0.0f, 0.0f, 0.0f),
    scale(0.0f, 0.0f, 0.0f),
    rotation(0.0f, 0.0f, 0.0f),
    angleRotationRadians(0.0f) {
}


MTRD::TransformComponent::TransformComponent(
    const glm::vec3& pos,
    const glm::vec3& scl,
    const glm::vec3& rot,
    float angle
)
    : position(pos), scale(scl), rotation(rot), angleRotationRadians(angle){
}
