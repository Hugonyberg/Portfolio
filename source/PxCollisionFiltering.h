#pragma once
#include <PxPhysicsAPI.h>
#include <iostream> 

class CollisionFiltering
{
public:
    enum eFilterGroup   //MAX 32 layers
    {
        Player = (1 << 0),  // Bit 0: Player
        Enemy = (1 << 1),  // Bit 1: Enemy
        EnemySpawner = (1 << 2),  // Bit 2: Companion 
        Environment = (1 << 3),  // Bit 3: Environment
        HarpoonableObjects = (1 << 4),  // Bit 4: HarpoonableObjects
        Harpoon = (1 << 5),  // Bit 5: Harpoon
    };

	CollisionFiltering();
	~CollisionFiltering();

    void setupFiltering(physx::PxShape* shape, physx::PxU32 filterGroup, physx::PxU32 filterMask);

private:

};

physx::PxFilterFlags FilterShader(
    physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
    physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
    physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);


class SimulationEventCallback: public physx::PxSimulationEventCallback
{
public:
    virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;

    // Other overrides (not using)
    virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override {}
    virtual void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override {}
    virtual void onWake(physx::PxActor** actors, physx::PxU32 count) override {}
    virtual void onSleep(physx::PxActor** actors, physx::PxU32 count) override {}
    virtual void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override {}
};