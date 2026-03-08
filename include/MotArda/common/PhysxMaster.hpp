#pragma once

#include "../../deps/physx/include/PxPhysicsAPI.h"
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
