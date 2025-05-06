#pragma once
#include <DreamEngine\math\Vector3.h>
#include <DreamEngine\utilities\CountTimer.h>
#include <PhysX\PxPhysicsAPI.h>

class GameObject;

enum class eObjectType
{
	Ammo,
	Repair,
	None
};

class PlayerStats 
{
public:
	// Health management
	void SetCurrentHealth(GameObject* aPlayerPtr, const int aHealthAmount);
	const int GetCurrentHealth(GameObject* aPlayerPtr) const;
	const int GetMaxHealth(GameObject* aPlayerPtr) const;
	void TakeDamage(GameObject* aPlayerPtr, const int& aDamageAmount);
	void Heal(GameObject* aPlayerPtr, const int& aHealingAmount);
	void HealToMax(GameObject* aPlayerPtr);
	const bool IsAlive(GameObject* aPlayerPtr) const;
	CommonUtilities::CountdownTimer iFramesTimer;
	CommonUtilities::CountdownTimer healingTimer;
	CommonUtilities::CountdownTimer healingCooldown;

	// Camera
	float nearPlane = 10.0f;
	float farPlane = 10000.0f;
	const float physicsBodyVerticalOffset = 65.0f;
	const float cameraVerticalOffset = 130.0f;

	// Interaction
	const float maximumInteractRange = 250.0f;

	// Grounded movement
	CU::CountdownTimer respawnTimer;
	physx::PxVec3 spawnPoint;
	const float moveSpeed = 250.0f;

	physx::PxVec3 groundedNormal;
	float groundedDotProduct;
	const float groundedTraceLength = 70.0f;

	// Rotation
	bool shouldRotate = false;
	DE::Vector2i rotationDelta;
	const float rotationSpeed = 12.0f;
	const float maxPitch = 90.0f;
	const float minPitch = -90.0f;

	// Movement input
	DE::Vector2f moveInputDirection;
	bool shouldMove = false;

	// Interacting
	bool shouldInteract = false;
	bool isLocked = false;
	bool isCarryingObject = false;
	eObjectType inventory = eObjectType::None;

	// Jumping
	bool shouldJump = false;
	const float coyoteTime = 0.25f;

	// Grappling hook input
	bool shouldUseGrapplingHook = false;
};