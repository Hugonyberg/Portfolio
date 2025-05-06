#pragma once
#include <DreamEngine\math\Vector3.h>
#include <DreamEngine/math/Transform.h>
#include <DreamEngine/graphics/GraphicsEngine.h>

class EnemySteeringBehaviour
{
public:
	EnemySteeringBehaviour();
	~EnemySteeringBehaviour();

	void Init(DreamEngine::Transform aTransform, const float aMaxSpeed, const float aSlowRadius);
	DE::Vector3f Update(float aDeltaTime, DE::Transform aTransform, DE::Vector3f aTarget);

	// Steering forces
	DE::Vector3f ArrivalForce(const DreamEngine::Vector3f aDirection);
	DE::Vector3f SeekForce();
	DE::Vector3f FleeForce();

	// Rotation methods
	DE::Vector3f RotateToThisOverTime(DreamEngine::Vector3f aPoint, float aDeltaTime, float aRotationSpeed, DreamEngine::Vector3f aCurrentRotation);
	DE::Vector3f RotateToThis(DreamEngine::Vector3f aPoint);
	DE::Vector3f RotateToVelocity(); 

private:
	void CalculateWeights();
	DE::Vector3f Truncate(const DreamEngine::Vector3f aDirection, float aSpeed);
private:

	DreamEngine::Transform myTransform;
	DE::Vector3f myTarget;
	DE::Vector3f myVelocity;
	DE::Vector3f mySeekForce;
	DE::Vector3f myFleeForce;
	DE::Vector3f myPredictForce;

	float myMaxSpeed;
	float mySlowingRadius;
	float mySeekWeight = 0.0f;
	float myFleeWeight = 0.0f;
	float myArivalWeight = 0.0f;
	float myPredictWeight = 0.0f;
	float myMinDistance = 0.0f;
};

