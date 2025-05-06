#include "EnemyShooting.h"
#include "HealthComponent.h"
#include "RigidBodyComponent.h"
#include "MainSingleton.h"
#include "EnemySteeringBehaviour.h"
#include "ProjectilePool.h" 

#include <PhysX\PxPhysicsAPI.h> 
#include <DreamEngine\utilities\UtilityFunctions.h>

EnemyShooting::~EnemyShooting()
{
	if(myProjectilePool != nullptr) delete myProjectilePool;
	if(mySteeringBehavior != nullptr) delete mySteeringBehavior;
}


void EnemyShooting::Init()
{
	AddComponent<HealthComponent>();
	auto* healthCmp = GetComponent<HealthComponent>();
	healthCmp->SetMaxHealth(myEnemyType.maxHealth);
	healthCmp->SetCurrentHealth(myEnemyType.maxHealth);

	myModelInstance->SetLocation(myTransform.GetPosition()); 
	mySpawningPosition = myTransform.GetPosition(); 

	myAttackCooldown.SetCurrentValue(3.0f);

	myMaxSpeed = 500.f;
	mySteeringBehavior = new EnemySteeringBehaviour;
	mySteeringBehavior->Init(myTransform, myMaxSpeed, 20.0f);

	 myRandomizeUp = UtilityFunctions::GetRandomFloat(-1500.0f, -1200.0f);
	 myRandomizeRight = UtilityFunctions::GetRandomFloat(-1000.0f, 1000.0f);

	 myProjectilePool = new ProjectilePool(5, false); 

	// fixing collision
	AddComponent<RigidBodyComponent>();
	auto* physXScene = MainSingleton::GetInstance()->GetPhysXScene();
	physx::PxShape* shape = DE::Engine::GetPhysXPhysics()->createShape(physx::PxSphereGeometry(25.0f), *MainSingleton::GetInstance()->GetPhysXMaterials()[0]);

	auto filter = MainSingleton::GetInstance()->GetCollisionFiltering();
	filter.setupFiltering(shape, filter.Enemy, filter.Environment);

	physx::PxRigidDynamic* body = DE::Engine::GetPhysXPhysics()->createRigidDynamic(
		physx::PxTransform(myTransform.GetPosition().x, myTransform.GetPosition().y, myTransform.GetPosition().z));

	body->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*body, 50.0f);

	body->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, false);

	physx::PxTransform currentPose = body->getGlobalPose();
	body->setName("EnemyShooting");
	physXScene->addActor(*body);
	GetComponent<RigidBodyComponent>()->SetBody(body);

	physx::PxTransform updatedPose(physx::PxVec3(myTransform.GetPosition().x, myTransform.GetPosition().y, myTransform.GetPosition().z), currentPose.q);
	body->setGlobalPose(updatedPose);
}

void EnemyShooting::Update(float aDeltaTime)
{
	UpdateStateMachine(aDeltaTime);
	CountDownCooldowns(aDeltaTime);

	if (myProjectilePool)
		myProjectilePool->Update(aDeltaTime);

	myModelInstance->SetTransform(myTransform);
}

void EnemyShooting::SetAnimatedModel()
{
}


void EnemyShooting::Respawn()
{
	Spawn(mySpawningPosition);
	myState = eState::eArriving; 
}

void EnemyShooting::Spawn(DE::Vector3f aSpawnPoint)
{
	myTransform.SetPosition(aSpawnPoint);
}

void EnemyShooting::SetAttackWayPoints(DE::Vector3f aPosition)
{
	myAttackWayPoint.push_back(aPosition);
}

void EnemyShooting::RenderProjectiles(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
	if (myProjectilePool)
		myProjectilePool->Render(aGraphicsEngine);
}

void EnemyShooting::SetAnimationState(float aDeltaTime)
{
	aDeltaTime;
}

void EnemyShooting::CountDownCooldowns(float aDeltaTime)
{
	myAttackCooldown.Update(aDeltaTime);
}

void EnemyShooting::UpdateStateMachine(float aDeltaTime)
{
	switch (myState)
	{
	case EnemyShooting::eState::eAttacking:
	{
		// attack boat
		DE::Vector3f dirToBoat = DE::Vector3f(myBoatTransform.GetPosition() - myTransform.GetPosition());
		myProjectilePool->GetProjectile(myTransform.GetPosition(), dirToBoat.GetNormalized(), &myBoatTransform);

		myAttackCooldown.Reset();
		myState = eState::eIdle;

		break;
	}
	case EnemyShooting::eState::eIdle:
	{
		// wait for attacking the boat
		if (myAttackCooldown.IsDone())
			myState = eState::eAttacking;

		UpdatePhysics(aDeltaTime);
		break;
	}
	case EnemyShooting::eState::eArriving:
	{
		// Swim to surfaces 
		UpdatePhysics(aDeltaTime);
		myState = eState::eIdle;
		break;
	}
	case EnemyShooting::eState::count:
		break;
	}
}

void EnemyShooting::UpdatePhysics(float aDeltaTime)
{
	SetOffsetPosition(myBoatTransform.GetPosition());
	DreamEngine::Vector3f steeringForce = mySteeringBehavior->Update(aDeltaTime, myTransform, myOffsetPosition);

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
}

void EnemyShooting::SetOffsetPosition(DE::Vector3f aBoatPos)
{
	DE::Vector3f offset; 
	auto camMatrix = MainSingleton::GetInstance()->GetActiveCamera()->GetTransform().GetMatrix();

	offset = camMatrix.GetUp() * myRandomizeUp;
	offset += camMatrix.GetRight() * myRandomizeRight;

	offset.y = aBoatPos.y;

	myOffsetPosition = aBoatPos + offset;
}
