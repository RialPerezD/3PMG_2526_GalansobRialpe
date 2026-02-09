#include "MotArda/common/Components/TransformComponent.hpp"

MTRD::TransformComponent::TransformComponent(){
    position = { 0,0,0 };
    scale = { 0,0,0 };
    rotation = { 0,0,0 };
    angleRotationRadians = 0;
}


MTRD::TransformComponent::TransformComponent(
    const glm::vec3& pos,
    const glm::vec3& scl,
    const glm::vec3& rot,
    float angle
)
    : position(pos), scale(scl), rotation(rot), angleRotationRadians(angle){
}
