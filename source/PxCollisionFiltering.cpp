#include "PxCollisionFiltering.h"
#include <foundation/Px.h>       // For basic types like PxU32
#include "ColliderCommon.h" // For PxFilterFlags, PxFilterData, etc.
#include "MainSingleton.h"

CollisionFiltering::CollisionFiltering()
{}

CollisionFiltering::~CollisionFiltering()
{}

void CollisionFiltering::setupFiltering(physx::PxShape * shape, physx::PxU32 filterGroup, physx::PxU32 filterMask)
{
    physx::PxFilterData filterData; 
    filterData.word0 = filterGroup; // word0 = own ID
    filterData.word1 = filterMask;  // word1 = ID mask to filter pairs that trigger a contact callback
                                    // To simplify we use this when its OK to collision with an object
    shape->setSimulationFilterData(filterData); 
    shape->setQueryFilterData(filterData); 
}

physx::PxFilterFlags FilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, 
                                  physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, 
                                  physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
    // Let triggers through
    if(physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
    {
        pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
        return physx::PxFilterFlag::eDEFAULT;
    }

    // Generate contacts for all pairs that were not filtered above
    pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

    // Enable contact notifications for specific pairs
    if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
    {
        pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND; // Enable contact notifications
        return physx::PxFilterFlag::eDEFAULT; // Allow collision
    }

    return physx::PxFilterFlag::eKILL; // Block collision
}

void SimulationEventCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
    //std::cout << "onContact called!" << std::endl; // Debugging

    if (!pairHeader.actors[0] || !pairHeader.actors[1])
    {
        std::cerr << "Invalid actors in contact pair!" << std::endl;
        return;
    }

    for (physx::PxU32 i = 0; i < nbPairs; i++)
    {
        const physx::PxContactPair& pair = pairs[i];

        // Check if the contact pair has started touching
        if (pair.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
        {
            physx::PxActor* actor1 = pairHeader.actors[0];
            physx::PxActor* actor2 = pairHeader.actors[1];

        }
    }
}