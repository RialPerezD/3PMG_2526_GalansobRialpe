#include "MotArda/common/Components/PhysxComponent.hpp"

namespace MTRD {
    PhysxComponent::PhysxComponent()
        : actor(nullptr), shape(nullptr), shapeType(PhysxShapeType::Sphere), mass(1.0f), isDynamic(true), position(glm::vec3(0.0f)), radius(0.5f), halfExtents(0.5f, 0.5f, 0.5f) {}

    PhysxComponent::PhysxComponent(physx::PxRigidActor* actor, physx::PxShape* shape, PhysxShapeType type, float mass, bool isDynamic)
        : actor(actor), shape(shape), shapeType(type), mass(mass), isDynamic(isDynamic), position(glm::vec3(0.0f)), radius(0.5f), halfExtents(0.5f, 0.5f, 0.5f) {}
}

