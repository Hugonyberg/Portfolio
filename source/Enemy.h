#pragma once
#include "EnemyType.h"
#include "GameObject.h"
#include "Observer.h"
#include "ParticleSystem3D.h"

#include <DreamEngine\math\Vector3.h>
#include <DreamEngine\utilities\CountTimer.h>
#include <DreamEngine\graphics\AnimationPlayer.h>
#include <DreamEngine/math/Collider.h>
#include <DreamEngine/graphics/sprite.h>

#include "../../Dependencies/Wwise/Wwise_Project/GeneratedSoundBanks/Wwise_IDs.h"
#include "../../Dependencies/Wwise/Wwise_SDK/include/AK/SpatialAudio/Common/AkSpatialAudio.h"
#include "../../Dependencies/Wwise/Wwise_SDK/include/AK/SoundEngine/Common/AkTypes.h"

namespace DreamEngine
{
	class GraphicsEngine;
	class ModelInstance;
	class AnimatedModelInstance;
	struct Navmesh;
}

class Player;
class Enemy : public GameObject
{
public:
	Enemy() = delete;
	Enemy(const EnemyType& anEnemyType);

	void BaseInit();
	void Update(float aDeltaTime) override;

	void Render(DreamEngine::GraphicsEngine& aGraphicsEngine) override;
	void RenderToGBufferWithoutShadow(DreamEngine::GraphicsEngine& aGraphicsEngine);

	virtual void SetAnimatedModel() {};
	void SetModelInstance(std::shared_ptr<DreamEngine::ModelInstance>& aModelInstance) { myModelInstance = aModelInstance; }
	void SetPlayer(std::shared_ptr<Player> aPlayer);
	void SetCurrentHealth(const int aHealthAmount);

	const int GetCurrentHealth() const;
	std::shared_ptr<DreamEngine::AnimatedModelInstance> GetAnimatedModelInstance();
	const int GetMaxHealth() const;
	const eEnemyType GetEnemyType() const;

	const bool IsAlive() const;
	void TakeDamage(const int& aDamageAmount);
	virtual void Respawn();
	virtual void Spawn(DE::Vector3f aSpawnPoint);

protected:
	void SetAnimationIndex(int anAnimationIndex);
	
	virtual void SetAnimationState(float aDeltaTime) {};
	virtual void CountDownCooldowns(float aDeltaTime) = 0;

	DE::Vector3f TruncateSteering(const DE::Vector3f aVector, float aMaxSpeed);

protected:
	const EnemyType& myEnemyType;
	std::shared_ptr<Player> myPlayer;

	CU::CountdownTimer myAttackTimer;
	CU::CountdownTimer myInvincibleFrames;
	CU::CountdownTimer myIsLostTimer;
	CU::CountdownTimer myTimeTryingToAttackPlayerTimer;
	CU::CountdownTimer myLifeTime;
	CU::CountdownTimer mySlowDurationTimer;

	DE::Vector3f mySpawnPos;
	DE::Vector3f myVelocity;
	DE::Vector3f myPreviousVelocity;

	std::shared_ptr<DE::AnimatedModelInstance> myAnimatedModelInstance;
	std::shared_ptr<DE::ModelInstance> myModelInstance;
	std::vector<DE::AnimationPlayer> myAnimations;

	std::shared_ptr<DreamEngine::Navmesh> myNavmesh;
	std::shared_ptr<DreamEngine::Navmesh> myDetailedNavmesh;

	float mySteeringSmoothingFactor = 0.1f;
	float myMaxTurnRate = 5.0f;
	float mySeparationDistance = 50.0f;

	int myCurrentAnimationIndex = 0;

	bool myShouldAnimateDeath = false;
	bool myShouldDeactivateSelf = false;
	bool myShouldAttackPlayer = false;
};