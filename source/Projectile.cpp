#include "stdafx.h"
#include "Projectile.h"
#include "MainSingleton.h"
#include "DreamEngine/graphics/TextureManager.h"
#include "../../Dependencies/Wwise/Wwise_Project/GeneratedSoundBanks/Wwise_IDs.h"
#include "../../Dependencies/Wwise/Wwise_SDK/include/AK/SpatialAudio/Common/AkSpatialAudio.h"
#include "../../Dependencies/Wwise/Wwise_SDK/include/AK/SoundEngine/Common/AkTypes.h"

#include <DreamEngine/graphics/AnimatedModelInstance.h>
#include <DreamEngine/graphics/ModelInstance.h>
#include <DreamEngine/graphics/ModelDrawer.h>
#include <DreamEngine/graphics/ModelFactory.h>
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/graphics/GraphicsStateStack.h>
#include <DreamEngine/utilities/StringCast.h>
#include <DreamEngine\utilities\UtilityFunctions.h>
#include <PhysX\PxPhysicsAPI.h>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Projectile::Projectile()
{
	myTag = eTag::EnemyProjectile;

	DreamEngine::ModelFactory& modelFactory = DreamEngine::ModelFactory::GetInstance();

	myModelInstance = std::make_shared<DE::ModelInstance>(modelFactory.GetModelInstance(L"3D/SM_Ch_Dog.fbx"));
	myModelInstance->SetScale({ myScale });
	myTransform = myModelInstance->GetTransform();
}

Projectile::~Projectile()
{
}

void Projectile::SetModelInstance(std::shared_ptr<DreamEngine::AnimatedModelInstance> aModel)
{
	myAnimatedModelInstance = aModel;
}

void Projectile::SetDamage(const int aDamageAmount)
{
	myDamage = aDamageAmount;
}

void Projectile::SetEnemysProjectile(const bool isEnemy)
{
	myEnemyProjectile = isEnemy;
}

void Projectile::Update(float aDeltaTime)
{
	if (myIsActive)
	{
		if(myEnemyProjectile)
			HandlePlayerCollision();
		else
			HandleEnemyCollision();
		
		if (myTimer < myTimeLimit)
		{
			myTimer += aDeltaTime;

			DreamEngine::Vector3f position = myTransform.GetPosition();
			position += myVelocity.GetNormalized() * mySpeed * aDeltaTime;

			myTransform.SetPosition(position);
			myModelInstance->SetTransform(myTransform);
		}
		else
		{
			myTimer = 0.0f;
			Deactivate();
		}
	}
	myWasActiveLastFrame = myIsActive;
}

float Projectile::GetCurvatureTowardPos(DE::Vector3f aPos)
{
	DE::Rotator dirToPos = aPos - myTransform.GetPosition();
	float calculatedAngle = UtilityFunctions::CalculateYRotationBetweenDirections(myTransform.GetMatrix().GetForward().x, myTransform.GetMatrix().GetForward().z, dirToPos.GetNormalized().x, dirToPos.GetNormalized().z);
	if (calculatedAngle > 0.0f) 
	{
		return UtilityFunctions::Min(90.0f, calculatedAngle);
	}
	else 
	{
		return UtilityFunctions::Max(-90.0f, calculatedAngle);
	}
}

void Projectile::TurnTowards(DE::Vector3f aPos)
{
	DE::Vector3f diff = myTransform.GetPosition() - aPos; // I think bullet model rotation is off
	float horizontalDistance = sqrt(diff.x * diff.x + diff.z * diff.z);
	float yaw = atan2(diff.x, diff.z);         // Rotation around Y-axis
	float pitch = -atan2(diff.y, horizontalDistance);  // Rotation around X-axis

	constexpr float PI = 3.14159265358979323846f;
	//yaw = RadToDeg(yaw);
	yaw = yaw * (180.0f / PI);
	pitch = pitch * (180.0f / PI);

	myTransform.SetRotation({ pitch, yaw, 0.0f });
	myModelInstance->SetRotation(myTransform.GetRotation());
}

void Projectile::OnCollision(GameObject* anObject, eCollisionLayer aCollisionLayer)
{
	aCollisionLayer;

	if (anObject->GetTag() == eTag::Player)
	{
		Deactivate();
	}
}

void Projectile::Activate(DreamEngine::Vector3f aPosition, DreamEngine::Vector3f aDirection, DE::Transform* aTargetTransform)
{
	myIsActive = true;
	myTransform.SetPosition(aPosition);
	TurnTowards(aTargetTransform->GetPosition());
	myTargetTransform = aTargetTransform;
	myTargetPosition = myTargetTransform->GetPosition();
	myVelocity = myTargetPosition - myTransform.GetPosition();
}

void Projectile::Activate(DreamEngine::Vector3f aPosition, DreamEngine::Vector3f aDirection, DreamEngine::Vector3f aTargetPosition)
{
	myIsActive = true;
	myTransform.SetPosition(aPosition);
	TurnTowards(aTargetPosition);
	myTargetPosition = aTargetPosition;
	myVelocity = myTargetPosition - myTransform.GetPosition();
}

void Projectile::Deactivate()
{
	myIsActive = false;
}

const int Projectile::GetDamage() const
{
	return myDamage;
}

void Projectile::HandlePlayerCollision()
{
	// Define a projectile sphere at the query position
	physx::PxSphereGeometry sphereGeometry(myScale);
	
	DE::Vector3f position = myTransform.GetPosition();

	// Creates a transform at the query position
	physx::PxTransform pose(physx::PxVec3(position.x, position.y, position.z));

	physx::PxQueryFilterData queryFilterData;  
	queryFilterData.data.word0 = MainSingleton::GetInstance()->GetCollisionFiltering().Player;
	queryFilterData.flags = physx::PxQueryFlag::eANY_HIT | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC;

	// Run the overlap query
	physx::PxOverlapBuffer overlapBuffer;
	bool status = MainSingleton::GetInstance()->GetPhysXScene()->overlap(sphereGeometry, pose, overlapBuffer, queryFilterData);


	if (status && overlapBuffer.hasAnyHits())
	{
		for (physx::PxU32 i = 0; i < overlapBuffer.getNbAnyHits(); i++)
		{
			const physx::PxOverlapHit& hit = overlapBuffer.getAnyHit(i);

		}
	}
}

void Projectile::HandleEnemyCollision()
{
	physx::PxSphereGeometry sphereGeometry(myScale * 0.5f);

	DE::Vector3f position = myTransform.GetPosition();
	physx::PxTransform pose(physx::PxVec3(position.x, position.y, position.z));

	physx::PxQueryFilterData queryFilterData; 
	queryFilterData.data.word0 = MainSingleton::GetInstance()->GetCollisionFiltering().Enemy; 
	queryFilterData.flags = physx::PxQueryFlag::eANY_HIT | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC; 

	// Run the overlap query
	physx::PxOverlapBuffer overlapBuffer;
	bool status = MainSingleton::GetInstance()->GetPhysXScene()->overlap(sphereGeometry, pose, overlapBuffer, queryFilterData);

	if(status && overlapBuffer.hasAnyHits())
	{
		for(physx::PxU32 i = 0; i < overlapBuffer.getNbAnyHits(); i++)
		{
			const physx::PxOverlapHit& hit = overlapBuffer.getAnyHit(i);

			// Check if the hit actor is an enemy
			if(hit.actor && hit.actor->getName() && strcmp(hit.actor->getName(), "Enemy") == 0)
			{
				MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &std::pair<physx::PxRigidActor*, int>(hit.actor, myDamage), eMessageType::EnemyTakeDamage });
				Deactivate();
				return;
			}
		}
	}
}

void Projectile::Render(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
	if (myIsActive)
	{
		aGraphicsEngine.GetModelDrawer().DrawGBCalc(*myModelInstance);
	}
}