#include "Leviathan.h"
#include "HealthComponent.h"
#include "EnemySteeringBehaviour.h"
#include "RigidBodyComponent.h"
#include "MainSingleton.h"

#include <DreamEngine\utilities\UtilityFunctions.h>
#include <PhysX\PxPhysicsAPI.h> 

Leviathan::~Leviathan()
{
   /* mySteeringBehaviour->~EnemySteeringBehaviour();
    delete mySteeringBehaviour; */
}

void Leviathan::Init(const float anAttackCoolDown, const float anAttackingSpeed, const float anAttackRange, const float aSeekingSpeed, const float aSeekRange)
{
    myAttackCooldown.SetCurrentValue(anAttackCoolDown);
    myAttackSpeed = anAttackingSpeed;
    myAttackRange = anAttackRange;
    mySeekingSpeed = aSeekingSpeed;
    mySeekingRange = aSeekRange;
    mySpawnPosition = myTransform.GetPosition(); 

    AddComponent<HealthComponent>();
    auto* healthCmp = GetComponent<HealthComponent>();
    healthCmp->SetMaxHealth(myEnemyType.maxHealth);
    healthCmp->SetCurrentHealth(myEnemyType.maxHealth);

    myState = eState::ePathing;

    mySteeringBehavior = new EnemySteeringBehaviour;
    mySteeringBehavior->Init(myTransform, mySeekingSpeed, 10.0f); 

    // fixing collision
    AddComponent<RigidBodyComponent>();
    auto* physXScene = MainSingleton::GetInstance()->GetPhysXScene();
    physx::PxShape* shape = DE::Engine::GetPhysXPhysics()->createShape(physx::PxSphereGeometry(anAttackRange), *MainSingleton::GetInstance()->GetPhysXMaterials()[0]);

    auto filter = MainSingleton::GetInstance()->GetCollisionFiltering();
    filter.setupFiltering(shape, filter.Enemy, filter.Environment);

    physx::PxRigidDynamic* body = DE::Engine::GetPhysXPhysics()->createRigidDynamic(
        physx::PxTransform(myTransform.GetPosition().x, myTransform.GetPosition().y, myTransform.GetPosition().z));

    body->attachShape(*shape);
    physx::PxRigidBodyExt::updateMassAndInertia(*body, 50.0f);

    body->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, false);

    physx::PxTransform currentPose = body->getGlobalPose();
    body->setName("Leviathan");
    physXScene->addActor(*body);
    GetComponent<RigidBodyComponent>()->SetBody(body);

    physx::PxTransform updatedPose(physx::PxVec3(myTransform.GetPosition().x, myTransform.GetPosition().y, myTransform.GetPosition().z), currentPose.q);
    body->setGlobalPose(updatedPose);
}

void Leviathan::Update(float aDeltaTime)
{
    UpdateStateMachine(aDeltaTime);

    myModelInstance->SetTransform(myTransform);
    aDeltaTime;
}

void Leviathan::SetAnimatedModel()
{
}

void Leviathan::Respawn()
{
    myTransform.SetPosition(mySpawnPosition);
    myWayPointIndex = 0;
    myState = eState::ePathing;
}

void Leviathan::UpdateStateMachine(float aDeltaTime)
{
    switch (myState)
    {
    case Leviathan::eState::eSearching:
    {
        // ping boat & move twoards boat
        break;
    }
    case Leviathan::eState::ePathing:
    {
        
        DreamEngine::Vector3f steeringForce = mySteeringBehavior->Update(aDeltaTime, myTransform, myWayPointPositions[myWayPointIndex]);

        physx::PxRigidDynamic* body = static_cast<physx::PxRigidDynamic*>(GetComponent<RigidBodyComponent>()->GetBody());

        // Set velocity
        physx::PxVec3 velocity(steeringForce.x, steeringForce.y, steeringForce.z);
        body->setLinearVelocity(velocity);

        // Update position and handle gravity
        physx::PxTransform currentPose = body->getGlobalPose();

        if (steeringForce.Length() == 0.0f)
        {
            body->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
        }
        else
            body->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);

        myTransform.SetPosition(DreamEngine::Vector3f(currentPose.p.x, myTransform.GetPosition().y, currentPose.p.z));
        myTransform.SetRotation(mySteeringBehavior->RotateToVelocity());

        if (UtilityFunctions::Near(myTransform.GetPosition(), myWayPointPositions[myWayPointIndex], mySeekingRange))
        {
            myWayPointIndex == myWayPointPositions.size() - 1 ? myWayPointIndex = 0 : myWayPointIndex++;
        }
        break;
    }
    case Leviathan::eState::eChasing:
    {
        // chase scene 
        break;
    }
    case Leviathan::eState::count:
        break;
    }
}

void Leviathan::SetWayPoints(DE::Vector3f aWaypoint)
{
    aWaypoint.y = myTransform.GetPosition().y;
    myWayPointPositions.push_back(aWaypoint);
}

DE::Vector3f Leviathan::GetBoatPosition()
{
    return DE::Vector3f();
}

void Leviathan::SetAnimationState(float aDeltaTime)
{
    aDeltaTime;
}

void Leviathan::CountDownCooldowns(float aDeltaTime)
{
    aDeltaTime;
}
