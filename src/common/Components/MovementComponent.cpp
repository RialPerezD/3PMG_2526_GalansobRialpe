#include "MotArda/common/Components/MovementComponent.hpp"

MTRD::MovementComponent::MovementComponent(){
    position = { 0,0,0 };
    scale = { 0,0,0 };
    rotation = { 0,0,0 };
    angleRotationRadians = 0;
    shouldConstantMove = true;
}


MTRD::MovementComponent::MovementComponent(
    const glm::vec3& pos,
    const glm::vec3& scl,
    const glm::vec3& rot,
    float angle,
    bool should)
    : position(pos), scale(scl), rotation(rot), angleRotationRadians(angle), shouldConstantMove(should) {
}
