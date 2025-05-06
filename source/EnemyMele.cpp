#include "EnemyMele.h"
#include "HealthComponent.h"
#include "EnemySteeringBehaviour.h"
#include "RigidBodyComponent.h"
#include "MainSingleton.h"

constexpr float RAD_TO_DEG = 57.2957795131f;

EnemyMele::~EnemyMele()
{
}

void EnemyMele::Init()
{
	AddComponent<HealthComponent>();
	auto* healthCmp = GetComponent<HealthComponent>();
	healthCmp->SetMaxHealth(myEnemyType.maxHealth);
	healthCmp->SetCurrentHealth(myEnemyType.maxHealth);

	myTransform.SetPosition(myTransform.GetPosition());
	myModelInstance->SetLocation(myTransform.GetPosition());

	myMaxSpeed = 50.0f;

	mySteeringBehavior = new EnemySteeringBehaviour;
	mySteeringBehavior->Init(myTransform, myMaxSpeed, 30.0f);

	CaptureLocalTransform(myBoatTransform);

	// fixing collision
	AddComponent<RigidBodyComponent>();
	auto* physXScene = MainSingleton::GetInstance()->GetPhysXScene();
	physx::PxShape* shape = DE::Engine::GetPhysXPhysics()->createShape(physx::PxSphereGeometry(20.0f), *MainSingleton::GetInstance()->GetPhysXMaterials()[0]);

	auto filter = MainSingleton::GetInstance()->GetCollisionFiltering();
	filter.setupFiltering(shape, filter.Enemy, filter.Environment);

	physx::PxRigidDynamic* body = DE::Engine::GetPhysXPhysics()->createRigidDynamic(
		physx::PxTransform(myTransform.GetPosition().x, myTransform.GetPosition().y, myTransform.GetPosition().z));

	body->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*body, 50.0f);

	body->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, false);

	physx::PxTransform currentPose = body->getGlobalPose();
	body->setName("EnemyMele");
	physXScene->addActor(*body);
	GetComponent<RigidBodyComponent>()->SetBody(body);

	physx::PxTransform updatedPose(physx::PxVec3(myTransform.GetPosition().x, myTransform.GetPosition().y, myTransform.GetPosition().z), currentPose.q);
	body->setGlobalPose(updatedPose);
}

void EnemyMele::Update(float aDeltaTime)
{
	UpdatePosition(aDeltaTime);

	switch (myState)
	{
	case EnemyMele::eState::eAttacking:
	{
		UpdatePosition(aDeltaTime); 
		break;
	}
	case EnemyMele::eState::eIdle:
	{
		UpdatePosition(aDeltaTime); 
		break;
	}
	case EnemyMele::eState::eWalking:
	{
		UpdatePosition(aDeltaTime); 
		break;
	}
	case EnemyMele::eState::eArriving:
	{
		UpdatePosition(aDeltaTime); 
		break;
	}
	case EnemyMele::eState::count:
		break;
	}
}

void EnemyMele::SetAnimatedModel()
{
}


void EnemyMele::Respawn()
{
}

void EnemyMele::Spawn(DE::Vector3f aSpawnPoint)
{
	aSpawnPoint;
}

void EnemyMele::SetAttackWayPoints(DE::Vector3f aWaypoint, int anIndex)
{
	auto pos = aWaypoint;
	pos.y = myTransform.GetPosition().y;
	myAttackWayPoint.at(anIndex) = pos; 
}

void EnemyMele::SetAnimationState(float aDeltaTime)
{
	aDeltaTime;
}

void EnemyMele::CountDownCooldowns(float aDeltaTime)
{
	aDeltaTime;
}

void EnemyMele::UpdatePosition(float aDeltaTime)
{
	DE::Vector3f localPosition = myLocalTransform.GetPosition();
	DE::Vector3f worldPosition = myBoatTransform.TransformPosition(localPosition);

	DE::Quaternionf boatQuat = myBoatTransform.GetQuaternion();
	DE::Quaternionf localQuat = myLocalTransform.GetQuaternion();
	DE::Quaternionf worldQuat = boatQuat * localQuat;

	DE::Vector3f worldScale = myLocalTransform.GetScale();

	DreamEngine::Transform newXform{ worldPosition, worldQuat, worldScale };
	myModelInstance->SetTransform(newXform);
	myTransform = myModelInstance->GetTransform();

	DreamEngine::Vector3f moveDirection = mySteeringBehavior->Update(aDeltaTime, myTransform, myAttackWayPoint.at(1));

	if (moveDirection.LengthSqr() > 0.0f)
	{
		moveDirection.Normalize();

		// Convert local -> world, apply movement, then convert back to local
		const DE::Matrix4x4f& boatMatrix = myBoatTransform.GetMatrix();
		DE::Vector3f worldPosition = boatMatrix * DE::Vector4f(localPosition, 1.0f);
		worldPosition += moveDirection * myMaxSpeed * aDeltaTime;
		DE::Vector3f newLocalPosition = boatMatrix.GetInverse() * DE::Vector4f(worldPosition, 1.0f);

		// Check boundaries
		if (newLocalPosition.x < 200.0f && newLocalPosition.x > -200.0f &&
			newLocalPosition.z < 700.0f && newLocalPosition.z > -700.0f)
		{
			myLocalTransform.SetPosition(newLocalPosition);
		}

		// Update rotation to face movement direction
		const float yawRadians = std::atan2(moveDirection.x, moveDirection.z);
		const DreamEngine::Quaternionf worldRotation({ 0.0f, yawRadians, 0.0f });
		const DreamEngine::Quaternionf localRotation = myBoatTransform.GetQuaternion().GetConjugate() * worldRotation;
		myLocalTransform.SetRotation(localRotation.GetEulerAnglesDegrees());
	}

	DE::Vector3f worldPos = myBoatTransform.TransformPosition(localPosition);
	worldPos.y = myTransform.GetPosition().y; // Lock Y-axis
	physx::PxRigidDynamic* body = static_cast<physx::PxRigidDynamic*>(GetComponent<RigidBodyComponent>()->GetBody());
	/*physx::PxTransform currentPose = ()
	body->setGlobalPose(currentPose); */
}

DE::Vector3f EnemyMele::UpdatePhysix(float aDeltaTime)
{
	return DE::Vector3f();
}

void EnemyMele::CaptureLocalTransform(const DreamEngine::Transform& boatXform)
{
	DreamEngine::Transform worldXform = myModelInstance->GetTransform();

	DE::Vector3f worldPosition = worldXform.GetPosition();
	DE::Vector3f localPosition = boatXform.InverseTransformPosition(worldPosition);

	DE::Quaternionf boatQuat = boatXform.GetQuaternion();
	DE::Quaternionf inverseBoatQuat = boatQuat.GetConjugate();
	DE::Quaternionf worldQuat = worldXform.GetQuaternion();
	DE::Quaternionf localQuat = inverseBoatQuat * worldQuat;

	DE::Vector3f localScale = worldXform.GetScale();

	myLocalTransform = DreamEngine::Transform
	{
		localPosition,
		localQuat,
		localScale
	};
}
