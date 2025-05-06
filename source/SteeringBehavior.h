#pragma once
#include <DreamEngine\math\Vector3.h>
#include <PhysX\PxPhysicsAPI.h>

class GraphicsEngine;
namespace DreamEngine
{
	struct Navmesh;
}

class SteeringBehavior
{
public:
	SteeringBehavior();

	void Init();

	DE::Vector3f UpdateSteering();
	DE::Vector3f UpdateSteering(bool aUseRaySteering);
	void SetUpdateParameters(float aDeltaTime, const DE::Vector3f& aPosition, const DE::Vector3f& aVelocity, const DE::Vector3f& aTarget, const float aSpeed);


	void SetNavmesh(std::shared_ptr<DE::Navmesh> aNavmesh) { myNavmesh = aNavmesh; }

	const DE::Vector3f& GetPosition() const { return myPosition; }
	void SetPosition(const DE::Vector3f& position) { myPosition = position; }

	const DE::Vector3f& GetVelocity() const { return myVelocity; }
	void SetVelocity(const DE::Vector3f& velocity) { myVelocity = velocity; }

	const DE::Vector3f& GetSeekDirection() const { return mySeekDirection; }
	void SetSeekDirection(const DE::Vector3f& direction) { mySeekDirection = direction; }

	const DE::Vector3f& GetTarget() const { return myTarget; }
	void SetTarget(const DE::Vector3f& target) { myTarget = target; }

	float GetSpeed() const { return mySpeed; }
	void SetSpeed(float speed) { mySpeed = speed; }

	float GetScale() const { return myScale; }
	void SetScale(float aScale) { myScale = aScale; }

	float GetArrivalRadius() const { return myArrivalRadius; }
	void SetArrivalRadius(float radius) { myArrivalRadius = radius; }

	float GetSeparationRadius() const { return mySeparationRadius; }
	void SetSeparationRadius(float radius) { mySeparationRadius = radius; }

	float GetObjectSeparationWeight() const { return myObjectSeparationWeight; }
	void SetObjectSeparationWeight(float aWeight) { myObjectSeparationWeight = aWeight; }

	float GetEnemySeparationWeight() const { return myEnemySeparationWeight; }
	void SetEnemySeparationWeight(float aWeight) { myEnemySeparationWeight = aWeight; }

	float GetSmoothingFactor() const { return mySteeringSmoothingFactor; }
	void SetSmoothingFactor(float aFactor) { mySteeringSmoothingFactor = aFactor; }

	float GetMaxTurnRate() const { return myMaxTurnRate; }
	void SetMaxTurnRate(float aTurnRate) { myMaxTurnRate = aTurnRate; }

	float GetSeekWeight() const { return mySeekWeight; }
	void SetSeekWeight(float aWeight) { mySeekWeight = aWeight; }

	bool GetSeekFlag() const { return mySeekFlag; }
	void SetSeekFlag(bool aFlag) { mySeekFlag = aFlag; }

	bool GetSeparationFlag() const { return mySeparationFlag; }
	void SetSeparationFlag(bool aFlag) { mySeparationFlag = aFlag; }

	bool GetObjectSeparationFlag() const { return myObjectSeparationFlag; }
	void SetObjectSeparationFlag(bool aFlag) { myObjectSeparationFlag = aFlag; }

	bool GetSpreadFlag() const { return mySpreadFlag; }
	void SetSpreadFlag(bool aFlag) { mySpreadFlag = aFlag; }

	bool GetNavmeshSteeringFlag() const { return myNavmeshSteeringFlag; }
	void SetNavmeshSteeringFlag(bool aFlag) { myNavmeshSteeringFlag = aFlag; }

	physx::PxRigidDynamic* GetBody() const { return myBody; }
	void SetBody(physx::PxRigidDynamic* body) { myBody = body; }

	void DebugRender(DreamEngine::GraphicsEngine& aGraphicsEngine);

	void SetObjectSeparationOffset(float anOffset) { myObjectSeparationOffset = anOffset; }

private:
	DE::Vector3f myPosition;
	DE::Vector3f myVelocity;
	DE::Vector3f myTarget;
	float mySpeed;
	float myScale;

	DE::Vector3f mySeekDirection;

	float myArrivalRadius;
	float mySeparationRadius;

	float myEnemySeparationWeight;
	float myObjectSeparationWeight;
	float mySeekWeight;

	float mySeekBoost = 1.0f;

	physx::PxRigidDynamic* myBody;

	std::shared_ptr<DE::Navmesh> myNavmesh;

	float mySteeringSmoothingFactor = 0.1f;
	// Maximum allowed angular change (in radians) per second.
	float myMaxTurnRate = 5.0f;
	float myObjectSeparationOffset = 0.0f;

	/* STEERING FLAGS*/
	bool mySeekFlag = true;
	bool mySeparationFlag = true;
	bool myObjectSeparationFlag = true;
	bool mySpreadFlag = false;
	bool myNavmeshSteeringFlag = false;
};