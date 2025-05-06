#include "stdafx.h"
#include "SteeringBehavior.h"
#include "Navmesh.h"
#include "MainSingleton.h"

/*************************************HELPER FUNCTIONS*************************************************/
DE::Vector3f ComputeDirectionFromYawPitch(float yawDegrees, float pitchDegrees)
{
    const float DEG2RAD = 3.14159265f / 180.f;
    float yawRad = yawDegrees * DEG2RAD;
    float pitchRad = pitchDegrees * DEG2RAD;
    DE::Vector3f direction;
    direction.x = sin(yawRad) * cos(pitchRad);
    direction.y = sin(pitchRad);
    direction.z = cos(yawRad) * cos(pitchRad);
    return direction.GetNormalized();
}

void ComputeBaseOrientation(const DE::Vector3f& velocity, float& outBaseYaw, float& outBasePitch)
{
    // Compute yaw (rotation about Y) from the X and Z components.
    outBaseYaw = atan2(velocity.x, velocity.z) * (180.f / 3.14159265f);

    // Compute pitch from the Y component and the horizontal length.
    float horizLength = sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
    outBasePitch = atan2(velocity.y, horizLength) * (180.f / 3.14159265f);
}
/*************************************HELPER FUNCTIONS*************************************************/
SteeringBehavior::SteeringBehavior()
    : myArrivalRadius(200.0f)
    , mySeparationRadius(550.0f)
    , myEnemySeparationWeight(75.0f)
    , myObjectSeparationWeight(25.0f)
    , mySeekWeight(1.5f)
    , myBody(nullptr)
{

}

void SteeringBehavior::Init() 
{
    
}

DE::Vector3f SteeringBehavior::UpdateSteering()
{
    DE::Vector3f steering;

    return steering;
}

DE::Vector3f SteeringBehavior::UpdateSteering(bool aUseRaySteering)
{
    DE::Vector3f steering;

    return steering;
}

void SteeringBehavior::SetUpdateParameters(float aDeltaTime, const DE::Vector3f& aPosition, const DE::Vector3f& aVelocity, const DE::Vector3f& aTarget, const float aSpeed)
{
    myPosition = aPosition;
    myVelocity = aVelocity;
    myTarget = aTarget;
    mySpeed = aSpeed;

    // Compute the base orientation from the enemy's current velocity.
    float baseYaw, basePitch;
    DE::Vector3f baseOrientationVelocity = myVelocity;
    baseOrientationVelocity.y = 0.f; // Ignore y rotation
    ComputeBaseOrientation(baseOrientationVelocity, baseYaw, basePitch);

    // Update each separation ray's direction based on the base orientation plus its offsets.
  
}
