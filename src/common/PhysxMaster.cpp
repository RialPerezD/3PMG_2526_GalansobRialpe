#include "MotArda/common/PhysxMaster.hpp"

namespace MTRD {

    static physx::PxFilterFlags defaultFilterShader(
        physx::PxFilterObjectAttributes attributes0,
        physx::PxFilterData filterData0,
        physx::PxFilterObjectAttributes attributes1,
        physx::PxFilterData filterData1,
        physx::PxPairFlags& pairFlags,
        const void* constantBlock,
        physx::PxU32 constantBlockSize)
    {
        pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
        return physx::PxFilterFlag::eDEFAULT;
    }

    PhysxMaster::PhysxMaster()
        : gFoundation(nullptr), gPhysics(nullptr), gScene(nullptr), gMaterial(nullptr), initialized(false) {}

    PhysxMaster::~PhysxMaster() {
        if (gScene) gScene->release();
        if (gPhysics) gPhysics->release();
        if (gFoundation) gFoundation->release();
    }

    void PhysxMaster::init() {
        if (initialized) return;

        gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
        if (!gFoundation) return;
        
        gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale());
        if (!gPhysics) return;
        
        gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.1f);
        if (!gMaterial) return;
        
        physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
        sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
        sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
        if (!sceneDesc.cpuDispatcher) return;
        sceneDesc.filterShader = defaultFilterShader;
        
        gScene = gPhysics->createScene(sceneDesc);
        initialized = true;
    }

    void PhysxMaster::update(float deltaTime) {
        if (!gScene || !initialized) return;
        
        const float fixedDt = 1.0f / 60.0f;
        gScene->simulate(fixedDt);
        gScene->fetchResults(true);
    }

    void PhysxMaster::createActor(PhysxComponent* physxComp, TransformComponent* transform) {
        if (!gPhysics || !gScene || !initialized) return;
        
        physx::PxShape* shape = nullptr;
        physx::PxTransform actorTransform(transform->position.x, transform->position.y, transform->position.z);
        
        switch (physxComp->shapeType) {
            case PhysxShapeType::Sphere:
                shape = gPhysics->createShape(physx::PxSphereGeometry(physxComp->radius), *gMaterial);
                break;
            case PhysxShapeType::Box:
                shape = gPhysics->createShape(
                    physx::PxBoxGeometry(physxComp->halfExtents.x, physxComp->halfExtents.y, physxComp->halfExtents.z),
                    *gMaterial
                );
                break;
            case PhysxShapeType::Plane: {
                shape = gPhysics->createShape(physx::PxPlaneGeometry(), *gMaterial);
                actorTransform.q = physx::PxQuat(-physx::PxPiDivTwo, physx::PxVec3(1.0f, 0.0f, 0.0f));
                physx::PxRigidStatic* actor = gPhysics->createRigidStatic(actorTransform);
                actor->attachShape(*shape);
                gScene->addActor(*actor);
                physxComp->actor = actor;
                return;
            }
        }
        
        if (!shape) return;
        
        physxComp->shape = shape;
        
        if (physxComp->isDynamic) {
            physx::PxRigidDynamic* actor = gPhysics->createRigidDynamic(actorTransform);
            actor->attachShape(*shape);
            actor->setMass(physxComp->mass);
            gScene->addActor(*actor);
            physxComp->actor = actor;
        } else {
            physx::PxRigidStatic* actor = gPhysics->createRigidStatic(actorTransform);
            actor->attachShape(*shape);
            gScene->addActor(*actor);
            physxComp->actor = actor;
        }
    }

    void PhysxMaster::syncTransform(PhysxComponent* physxComp, TransformComponent* transform) {
        if (!physxComp->actor || !physxComp->isDynamic) return;
        
        physx::PxTransform pxTransform = physxComp->actor->getGlobalPose();
        transform->position.x = pxTransform.p.x;
        transform->position.y = pxTransform.p.y;
        transform->position.z = pxTransform.p.z;
    }

}
