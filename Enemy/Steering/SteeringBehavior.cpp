#include "stdafx.h"
#include "SteeringBehavior.h"

#include "FlyingEnemy.h"
#include "GroundEnemy.h"

#include "Navmesh.h"

#include "MainSingleton.h"

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

SteeringBehavior::SteeringBehavior()
    : myArrivalRadius(200.0f)
    , mySeparationRadius(550.0f)
    , myEnemySeparationWeight(75.0f)
    , myObjectSeparationWeight(25.0f)
    , mySeekWeight(1.5f)
    , myBody(nullptr)
{
}

void SteeringBehavior::SetActiveGroundEnemies(std::vector<std::shared_ptr<GroundEnemy>> someEnemies)
{
    myActiveGroundEnemies = someEnemies;
}

void SteeringBehavior::SetActiveFlyingEnemies(std::vector< std::shared_ptr<FlyingEnemy>> someEnemies)
{
    myActiveFlyingEnemies = someEnemies;
}

DE::Vector3f SteeringBehavior::UpdateSteering()
{
    DE::Vector3f steering;

    if (mySeparationFlag)
    {
        WeightedForce separationForce = GetSeparationSteering();
        DE::Vector3f separationSteering = separationForce.direction;
        float separationWeight = separationForce.weight;

        steering += separationSteering * separationWeight * myEnemySeparationWeight;
    }

    if (mySeekFlag)
    {
        WeightedForce seekForce = GetSeekSteering();
        DE::Vector3f seekSteering = seekForce.direction;
        float seekWeight = seekForce.weight;

        steering += seekSteering * seekWeight * mySeekWeight;
    }

    if (mySpreadFlag)
    {
        WeightedForce spreadForce = GetSpreadSteering(false);
        DE::Vector3f spreadSteering = spreadForce.direction;
        float spreadWeight = spreadForce.weight;

        steering += spreadSteering * spreadWeight * myEnemySeparationWeight;
    }

    if (myObjectSeparationFlag)
    {
        WeightedForce objectSeparationForce = GetObjectSeparationSteering();
        DE::Vector3f objectSeparationSteering = objectSeparationForce.direction;
        float objectSeparationWeight = objectSeparationForce.weight;

        steering += objectSeparationSteering * objectSeparationWeight * myObjectSeparationWeight;
    }

    if (myNavmeshSteeringFlag)
    {
        WeightedForce navmeshForce = GetNavmeshSteering();
        DE::Vector3f navmeshSteering = navmeshForce.direction;
        float navmeshWeight = navmeshForce.weight;

        steering += navmeshSteering * navmeshWeight * myNavmeshWeight;
    }

    return steering;
}

void SteeringBehavior::SetUpdateParameters(float aDeltaTime, const DE::Vector3f& aPosition, const DE::Vector3f& aVelocity, const DE::Vector3f& aTarget, const float aSpeed)
{
    myPosition = aPosition;
    myVelocity = aVelocity;
    myTarget = aTarget;
    mySpeed = aSpeed;

    // Compute the base orientation from the current velocity.
    float baseYaw, basePitch;
    DE::Vector3f baseOrientationVelocity = myVelocity;
    baseOrientationVelocity.y = 0.f; // Ignores y rotation
    ComputeBaseOrientation(baseOrientationVelocity, baseYaw, basePitch);

    // Update each separation ray's direction
    for (auto& ray : mySeparationRays)
    {
        float rayYaw = baseYaw + ray.yawOffset;
        float rayPitch = basePitch + ray.pitchOffset;
        ray.direction = ComputeDirectionFromYawPitch(rayYaw, rayPitch);
    }
}

WeightedForce SteeringBehavior::GetSeparationSteering()
{
    float totalWeight = 0.f;
    DE::Vector3f steeringForce(0.f, 0.f);

    if (myActiveFlyingEnemies.empty())
    {
        const float separationRadius = mySeparationRadius * myScale * 2.f;

        for (const auto& enemy : myActiveGroundEnemies)
        {
            if (!enemy->IsAlive() && enemy->GetShouldPlayDeathAnimation())
                continue;

            const DE::Vector3f basePosition = enemy->GetTransform()->GetPosition();
            const DE::Vector3f backPosition = basePosition + enemy->GetVelocity() * -50.f; // Since enemies are rectangular, quick fix for separating from rear

            for (const DE::Vector3f& sample : { basePosition, backPosition })
            {
                DE::Vector3f offset = myPosition - sample;
                offset.y = 0.f;
                float distance = offset.Length();

                if (distance > 0.01f && distance < separationRadius && enemy->IsAlive())
                {
                    float weight = (2.f / distance) * 1.5f;
                    steeringForce += offset.GetNormalized() * weight;
                    totalWeight += weight;
                }
            }
        }
    }
    else
    {
        for (const auto& enemy : myActiveFlyingEnemies)
        {
            if (!enemy->IsAlive() && enemy->GetShouldPlayDeathAnimation())
                continue;

            const DE::Vector3f position = enemy->GetTransform()->GetPosition();
            DE::Vector3f offset = myPosition - position;
            float distance = offset.Length();

            float expandedRadius = mySeparationRadius + enemy->GetSteering().GetSeparationRadius();

            if (distance > 0.01f && distance < expandedRadius)
            {
                float weight = (2.f / distance) * 1.5f;
                steeringForce += offset.GetNormalized() * weight;
                totalWeight += weight;
            }
        }
    }

    if (steeringForce.Length() > 0.01f)
        steeringForce = steeringForce.GetNormalized() * mySpeed;

    return { steeringForce, totalWeight };
}

WeightedForce SteeringBehavior::GetObjectSeparationSteering()
{
    if (mySeparationRays.empty())
        return WeightedForce({ 0.0f, 0.0f, 0.0f }, 1.0f);

    DE::Vector3f steeringForce(0.f, 0.f, 0.f);
    float totalWeight = 0.f;

    physx::PxScene* scene = MainSingleton::GetInstance()->GetPhysXScene();

    for (auto& ray : mySeparationRays)
    {
        DE::Vector3f rayOriginPos = myPosition + ray.direction;
        rayOriginPos.y += myObjectSeparationOffset;

        physx::PxVec3 origin(rayOriginPos.x, rayOriginPos.y, rayOriginPos.z);
        physx::PxVec3 direction(ray.direction.x, ray.direction.y, ray.direction.z);

        physx::PxRaycastBufferN<64> hitInfo;
        physx::PxQueryFilterData queryFilterData;
        queryFilterData.data.word0 = MainSingleton::GetInstance()->GetCollisionFiltering().Environment;

        bool status = scene->raycast(origin, direction, ray.separationRayLength,
            hitInfo, physx::PxHitFlag::eDEFAULT, queryFilterData);

        if (status)
        {
            for (physx::PxU32 i = 0; i < hitInfo.nbTouches; ++i)
            {
                const auto& hit = hitInfo.touches[i];
                if (hit.actor == myBody)
                    continue;

                const float distance = hit.distance;
                if (distance > 0.001f)
                {
                    ray.isColliding = true;
                    ray.collidingDistance = distance;

                    float t = 1.0f - (distance / ray.separationRayLength);

                    // Choose one of these strength calculations:
                    // float strengthExp = powf(t, 2.0f); // Exponential
                    // float strength = strengthExp;

                    // float strengthLinear = (ray.separationRayLength - distance) / ray.separationRayLength; // Linear
                    // float strength = strengthLinear;

                    float smoothingFactor = 4.0f; // Smoothing factor from 2.0 to 5.0
                    float strengthSmooth = t * t * (smoothingFactor - (smoothingFactor - 1) * t);
                    float strength = strengthSmooth; // Smoothstep

                    steeringForce += (-ray.direction) * strength * ray.weight;
                    totalWeight += ray.weight;
                }
            }
        }
        else
        {
            ray.isColliding = false;
            ray.collidingDistance = 0.0f;
        }
    }

    if (totalWeight > 0.f)
        steeringForce /= totalWeight;

    return WeightedForce(steeringForce, 1.0f);
}

WeightedForce SteeringBehavior::GetSeekSteering()
{
    DE::Vector3f desiredVelocity = myTarget - myPosition;
    float distance = desiredVelocity.Length();
    desiredVelocity.Normalize();

    if (distance < myArrivalRadius)
    {
        desiredVelocity *= mySpeed * (distance / myArrivalRadius);  // Scale by proximity
    }
    else
    {
        // Normal seek behavior when outside arrival radius
        desiredVelocity *= mySpeed;
    }

    DE::Vector3f steering = desiredVelocity - myVelocity;

    WeightedForce returnedInformation;
    returnedInformation.direction = steering;
    returnedInformation.weight = 1.f;
    return returnedInformation;
}

WeightedForce SteeringBehavior::GetNavmeshSteering()
{
    if (!myNavmesh)
        return WeightedForce();

    float distanceToEdge = 0.f;
    DE::Vector3f edgeNormal(0.f, 0.f, 0.f);

    bool nearEdge = DE::Pathfinding::GetNearestEdge(myNavmesh, myPosition, distanceToEdge, edgeNormal);

    if (nearEdge && distanceToEdge < mySeparationRadius)
    {
        float t = 1.0f - (distanceToEdge / mySeparationRadius);

        float smoothingFactor = 4.0f; // Adjust between 2.0 and 5.0 for different curves
        float strengthSmooth = t * t * (smoothingFactor - (smoothingFactor - 1) * t);
        float strength = strengthSmooth;

        DE::Vector3f steeringForce = edgeNormal * strength;
        steeringForce.y = 0.0f;

        return WeightedForce(steeringForce, * strength);
    }

    return WeightedForce();
}

WeightedForce SteeringBehavior::GetSpreadSteering()
{
    const float clusteringDistance = 10000.f;
    const float verticalThreshold = 50.f;
    const float horizontalThreshold = 50.f;
    const float baseUpForce = 10.f;
    const float baseLateralForce = 3.f;

    DE::Vector3f forward = myVelocity;
    forward.y = 0.f;

    if (forward.LengthSqr() < 0.0001f)
        return WeightedForce(); // No movement = no spread

    forward.Normalize();

    DE::Vector3f worldUp(0.f, 1.f, 0.f);
    DE::Vector3f lateral = worldUp.Cross(forward);

    if (lateral.LengthSqr() < 0.0001f)
        lateral = DE::Vector3f(1.f, 0.f, 0.f);
    else
        lateral.Normalize();

    int verticalCount = 0;
    int horizontalCount = 0;
    float lateralBiasSum = 0.f;

    for (const auto& enemy : myActiveFlyingEnemies)
    {
        const DE::Vector3f friendPos = enemy->GetTransform()->GetPosition();
        const DE::Vector3f toFriend = friendPos - myPosition;
        const float distance = toFriend.Length();

        if (distance < 0.01f || distance > clusteringDistance)
            continue;

        if (forward.Dot(toFriend) <= 0.f)
            continue;

        if (fabs(friendPos.y - myPosition.y) < verticalThreshold)
            ++verticalCount;

        const float lateralOffset = lateral.Dot(toFriend);
        if (fabs(lateralOffset) < horizontalThreshold)
        {
            ++horizontalCount;
            lateralBiasSum += (lateralOffset >= 0.f) ? 1.f : -1.f;
        }
    }

    DE::Vector3f upwardForce(0.f, 0.f, 0.f);
    if (verticalCount > 0)
    {
        float verticalScale = static_cast<float>(verticalCount) / 5.f;
        upwardForce.y = baseUpForce * verticalScale;
    }

    DE::Vector3f sidewaysForce(0.f, 0.f, 0.f);
    if (horizontalCount > 0)
    {
        float avgSide = lateralBiasSum / static_cast<float>(horizontalCount);
        sidewaysForce = lateral * (-baseLateralForce * avgSide);
    }

    DE::Vector3f spreadForce = upwardForce + sidewaysForce;

    return WeightedForce{ spreadForce, spreadForce.Length() };
}

void SteeringBehavior::AddSeparationRay(const SeparationRaycastInfo& aRay)
{
    mySeparationRays.push_back(aRay);
}

void SteeringBehavior::SetRays(std::vector<SeparationRaycastInfo>& someRays)
{
    mySeparationRays = someRays;
}

void SteeringBehavior::DebugRender(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
    DreamEngine::LineDrawer& lineDrawer = aGraphicsEngine.GetLineDrawer();
    DreamEngine::LinePrimitive lineToDraw;

    for (size_t i = 0; i < std::size(mySeparationRays); i++)
    {
        const auto& ray = mySeparationRays[i];
        lineToDraw.fromPosition = myPosition;
        lineToDraw.fromPosition.y += myObjectSeparationOffset;

        if (ray.isColliding)
        {
            lineToDraw.color = { 0.f, 1.f, 0.0f, 1.0f };
            lineToDraw.toPosition = myPosition + ray.direction * ray.collidingDistance;
            lineToDraw.toPosition.y += myObjectSeparationOffset;
            lineDrawer.Draw(lineToDraw);

            lineToDraw.color = { 1.f, 0.f, 0.0f, 1.0f };
            lineToDraw.fromPosition = lineToDraw.toPosition;
            lineToDraw.toPosition = myPosition + ray.direction * ray.separationRayLength;
            lineToDraw.toPosition.y += myObjectSeparationOffset;
            lineDrawer.Draw(lineToDraw);
        }
        else
        {
            // If no collision, draw the entire line in green
            lineToDraw.color = { 0.f, 1.f, 0.0f, 1.0f };
            lineToDraw.toPosition = myPosition + ray.direction * ray.separationRayLength;
            lineToDraw.toPosition.y += myObjectSeparationOffset;
            lineDrawer.Draw(lineToDraw);
        }
    }
}

