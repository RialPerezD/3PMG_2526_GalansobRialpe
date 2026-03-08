#pragma once

#include "../deps/glm-master/glm/glm.hpp"
#include "../../deps/physx/include/PxPhysicsAPI.h"

namespace MTRD {

    enum class PhysxShapeType {
        Sphere,
        Box,
        Plane
    };

    struct PhysxComponent {
        physx::PxRigidActor* actor;
        physx::PxShape* shape;
        PhysxShapeType shapeType;
        float mass;
        bool isDynamic;
        glm::vec3 position;
        float radius;
        glm::vec3 halfExtents;

        PhysxComponent();
        PhysxComponent(physx::PxRigidActor* actor, physx::PxShape* shape, PhysxShapeType type, float mass, bool isDynamic);
    };
}
