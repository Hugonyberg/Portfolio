#include "EnemySteeringBehaviour.h"

namespace
{
	// Constants
	constexpr float minDistance = 5.0f;
}

EnemySteeringBehaviour::EnemySteeringBehaviour()
{
	myVelocity = 0.0f;
	mySlowingRadius = 30.0f;
	mySeekWeight = 0.0f;
	myFleeWeight = 0.0f;
	myArivalWeight = 0.0f;
}

EnemySteeringBehaviour::~EnemySteeringBehaviour()
{
}

void EnemySteeringBehaviour::Init(DreamEngine::Transform aTransform, const float aMaxSpeed, const float aSlowRadius)
{
	myTransform = aTransform;
	myMaxSpeed = aMaxSpeed;
	myMinDistance = aSlowRadius;
}

DE::Vector3f EnemySteeringBehaviour::Update(float aDeltaTime, DE::Transform aTransform, DE::Vector3f aTarget)
{
	auto lenght = (aTarget - aTransform.GetPosition()).Length();
	if (lenght <= myMinDistance)
		return 0.0f;

	myTransform = aTransform;
	myTarget = aTarget;

	CalculateWeights();

	DreamEngine::Vector3f seekForce = SeekForce() * mySeekWeight;
	//DreamEngine::Vector3f fleeForce = FleeForce() * myFleeWeight;
	DreamEngine::Vector3f arrivalForce = ArrivalForce(myTarget) * myArivalWeight;

	myVelocity += (seekForce /*+ fleeForce*/ + arrivalForce) * aDeltaTime;
	myVelocity = Truncate(myVelocity, myMaxSpeed);

	return myVelocity;
}

DE::Vector3f EnemySteeringBehaviour::ArrivalForce(const DreamEngine::Vector3f aDirection)
{
	auto desiredVelocity = aDirection - myTransform.GetPosition();
	float distance = (desiredVelocity).Length();

	if (distance < mySlowingRadius)
	{
		float scale = (distance / mySlowingRadius);
		desiredVelocity = desiredVelocity.GetNormalized() * (myMaxSpeed * scale);
	}
	else
		desiredVelocity = desiredVelocity.GetNormalized() * myMaxSpeed;

	return desiredVelocity - myVelocity;
}

DE::Vector3f EnemySteeringBehaviour::SeekForce()
{
	DreamEngine::Vector3f toTarget = myTarget - myTransform.GetPosition();
	DreamEngine::Vector3f desiredVelocity = toTarget.GetNormalized() * myMaxSpeed;
	mySeekForce = desiredVelocity - myVelocity;

	return mySeekForce;
}

DE::Vector3f EnemySteeringBehaviour::FleeForce()
{
    return DE::Vector3f();
}

DE::Vector3f EnemySteeringBehaviour::RotateToThisOverTime(DreamEngine::Vector3f aPoint, float aDeltaTime, float aRotationSpeed, DreamEngine::Vector3f aCurrentRotation)
{
	auto myCurrentDir = aPoint.GetNormalized();

	DreamEngine::Vector3f up = DreamEngine::Vector3f(0, 1, 0) + myCurrentDir * 0.011f;
	up = up.GetNormalized();

	DreamEngine::Vector3f forward = { 0.f, 1.f, 0.f };
	forward = (forward - forward.Dot(up) * up).GetNormalized();
	forward *= -1.f;

	DreamEngine::Vector3f right = up.Cross(forward).GetNormalized();

	DE::Quatf q = DE::Quatf::CreateFromOrthonormalBasisVectors(right, up, forward);
	DE::Vector3f rotation = q.GetEulerAnglesDegrees();

	DE::Vector3f delta = rotation - aCurrentRotation;

	while (delta.x > 180) delta.x -= 360;
	while (delta.x < -180) delta.x += 360;
	while (delta.y > 180) delta.y -= 360;
	while (delta.y < -180) delta.y += 360;
	while (delta.z > 180) delta.z -= 360;
	while (delta.z < -180) delta.z += 360;

	DE::Vector3f newRotation = aCurrentRotation + delta * aRotationSpeed * aDeltaTime;

	return newRotation;
}

DE::Vector3f EnemySteeringBehaviour::RotateToThis(DreamEngine::Vector3f aPoint)
{
	auto myCurrentDir = aPoint.GetNormalized();

	DreamEngine::Vector3f up = DreamEngine::Vector3f(0, 1, 0) + myCurrentDir * 0.011f;
	up = up.GetNormalized();

	DreamEngine::Vector3f forward = { 0.f, 1.f, 0.f };
	forward = (forward - forward.Dot(up) * up).GetNormalized();
	forward *= -1.f;

	DreamEngine::Vector3f right = up.Cross(forward).GetNormalized();

	DE::Quatf q = DE::Quatf::CreateFromOrthonormalBasisVectors(right, up, forward);
	DE::Vector3f rotation = q.GetEulerAnglesDegrees();

	return rotation;
}

DE::Vector3f EnemySteeringBehaviour::RotateToVelocity()
{
	auto myCurrentDir = myVelocity.GetNormalized();

	DreamEngine::Vector3f up = DreamEngine::Vector3f(0, 1, 0) + myCurrentDir * 0.011f;
	up = up.GetNormalized();

	DreamEngine::Vector3f forward = { 0.f, 1.f, 0.f };
	forward = (forward - forward.Dot(up) * up).GetNormalized();
	forward *= -1.f;

	DreamEngine::Vector3f right = up.Cross(forward).GetNormalized();

	DE::Quatf q = DE::Quatf::CreateFromOrthonormalBasisVectors(right, up, forward);
	DE::Vector3f rotation = q.GetEulerAnglesDegrees();

	return rotation;
}

void EnemySteeringBehaviour::CalculateWeights()
{
	float distanceToTarget = (myTarget - myTransform.GetPosition()).Length();
	myArivalWeight = max(0.0f, 1.0f - (distanceToTarget - minDistance) / mySlowingRadius);

	mySeekWeight = max(0.0f, 1.0f - myFleeWeight - myArivalWeight - myPredictWeight);
}

DE::Vector3f EnemySteeringBehaviour::Truncate(const DreamEngine::Vector3f aDirection, float aSpeed)
{
	float length = aDirection.Length();
	if (length > aSpeed)
	{
		return aDirection.GetNormalized() * aDirection;
	}
	return aDirection;
}
