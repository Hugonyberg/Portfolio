#include "Enemy.h"
#include "HealthComponent.h"
#include "Player.h"
#include "MainSingleton.h"
#include "Navmesh.h"

#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/graphics/ModelDrawer.h>
#include <DreamEngine/graphics/ModelInstance.h>
#include <DreamEngine/graphics/AnimatedModelInstance.h>
#include <DreamEngine/utilities/UtilityFunctions.h>
#include <DreamEngine/graphics/TextureManager.h>
#include <DreamEngine/graphics/SpriteDrawer.h>
#include <DreamEngine/math/Matrix.h>

#include <imgui\imgui.h>


Enemy::Enemy(const EnemyType& anEnemyType) : myEnemyType(anEnemyType)
{
	myInvincibleFrames = CU::CountdownTimer(0.33f, 0.33f);
	myInvincibleFrames.Zeroize();
}

void Enemy::BaseInit()
{
	myTag = eTag::Enemy;

	AddComponent<HealthComponent>();
	auto* healthCmp = GetComponent<HealthComponent>();
	healthCmp->SetMaxHealth(myEnemyType.maxHealth);
	healthCmp->SetCurrentHealth(myEnemyType.maxHealth);

	myAnimatedModelInstance->SetLocation(myTransform.GetPosition());
	mySpawnPos = myTransform.GetPosition();
}

void Enemy::SetPlayer(std::shared_ptr<Player> aPlayer)
{
	myPlayer = aPlayer;
}

void Enemy::Update(float aDeltaTime)
{
	GameObject::Update(aDeltaTime);
}

DE::Vector3f Enemy::TruncateSteering(const DE::Vector3f aVector, float aMaxSpeed)
{
	if (aVector.Length() > aMaxSpeed)
	{
		DE::Vector3f truncated = aVector;
		truncated.Normalize();
		truncated *= aMaxSpeed;
		return truncated;
	}
	return aVector;
}


const int Enemy::GetCurrentHealth() const
{
	return GetComponent<HealthComponent>()->GetCurrentHealth();
}

std::shared_ptr<DreamEngine::AnimatedModelInstance> Enemy::GetAnimatedModelInstance()
{
	return myAnimatedModelInstance;
}

void Enemy::SetCurrentHealth(const int aHealthAmount)
{
	GetComponent<HealthComponent>()->SetCurrentHealth(aHealthAmount);
}

const int Enemy::GetMaxHealth() const
{
	return GetComponent<HealthComponent>()->GetMaxHealth();
}

const bool Enemy::IsAlive() const
{
	return GetComponent<HealthComponent>()->IsAlive();
}

void Enemy::TakeDamage(const int& aDamageAmount)
{
	if (IsAlive() && myInvincibleFrames.IsDone())
	{
		GetComponent<HealthComponent>()->SetCurrentHealth(UtilityFunctions::Max(GetCurrentHealth() - aDamageAmount, 0));
		myInvincibleFrames.Reset();
	}
}

void Enemy::Respawn()
{
	myTransform.SetPosition(mySpawnPos);
	myAnimatedModelInstance->SetLocation(myTransform.GetPosition());
	myAnimations[myCurrentAnimationIndex].Stop();
	myAnimations[myCurrentAnimationIndex].Update(UtilityFunctions::GetRandomFloat(0.0f, 0.5f));
}

void Enemy::Spawn(DE::Vector3f aSpawnPoint)
{
	myTransform.SetPosition(aSpawnPoint);
	myIsActive = true;
}

void Enemy::Render(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
	if (IsAlive() || myShouldAnimateDeath)
	{
		if (myModelInstance && !myShouldAnimateDeath)
		{
			aGraphicsEngine.GetModelDrawer().DrawGBCalc(*myModelInstance.get());
		}
		else
		{
			aGraphicsEngine.GetModelDrawer().DrawGBCalcAnimated(*myAnimatedModelInstance.get());
		}
	}
}

void Enemy::RenderToGBufferWithoutShadow(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
	aGraphicsEngine;
}

const eEnemyType Enemy::GetEnemyType() const
{
	return myEnemyType.enemyType;
}

void Enemy::SetAnimationIndex(int anAnimationIndex)
{
	myCurrentAnimationIndex = anAnimationIndex;
	myAnimations[myCurrentAnimationIndex].SetFrame(0);
	myAnimations[myCurrentAnimationIndex].Play();
	myAnimations[myCurrentAnimationIndex].Stop();
}