#pragma once

#include "../../deps/physx/include/PxPhysicsAPI.h"
#include "../../deps/glm-master/glm/glm.hpp"
#include "Components/PhysxComponent.hpp"
#include "Components/TransformComponent.hpp"

namespace MTRD {

    class PhysxMaster {
    public:
        PhysxMaster();
        ~PhysxMaster();

        void init();
        void update(float deltaTime);
        
        void createActor(PhysxComponent* physxComp, TransformComponent* transform);
        void syncTransform(PhysxComponent* physxComp, TransformComponent* transform);

        bool raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, glm::vec3& hitPosition, void*& hitActor);

        bool initialized;
    private:
        physx::PxDefaultAllocator gAllocator;
        physx::PxDefaultErrorCallback gErrorCallback;
        physx::PxFoundation* gFoundation;
        physx::PxPhysics* gPhysics;
        physx::PxScene* gScene;
        physx::PxMaterial* gMaterial;
    };
}
