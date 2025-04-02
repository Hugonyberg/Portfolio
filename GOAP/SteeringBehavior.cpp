#include "stdafx.h"
#include "SteeringBehavior.h"

#include "MainSingleton.h"


void SteeringBehavior::SetSteeringParameters(const SteeringParams& someSteeringParams)
{
    mySteeringParams = someSteeringParams;
}


Tga::Vector2f SteeringBehavior::UpdateSteering()
{
    WeightedForce seekSteering = GetSeekSteering();
    WeightedForce separationSteering = GetSeparationSteering();
    
    Tga::Vector2f returnVelocity;
    returnVelocity = seekSteering.direction * seekSteering.weight + separationSteering.direction * separationSteering.weight;
    return returnVelocity;
}

WeightedForce SteeringBehavior::GetSeparationSteering()
{
    WeightedForce separation;

    std::vector<std::shared_ptr<Entity>> nearbyActors = MainSingleton::GetInstance().GetActiveActors();
    const auto& grid = MainSingleton::GetInstance().GetGrid();
    GridLocation gridLocation = grid->GetTileFromPosition(mySteeringParams.currentPosition);
    std::vector<GridLocation> gridNeighbors = grid->UnwalkableNeighbors(gridLocation);

    float totalWeight = 0.0f;
    Tga::Vector2f desiredVelocity(0.0f, 0.0f);

    for (const auto& actor : nearbyActors)
    {
        std::shared_ptr<Farmer> farmer = std::dynamic_pointer_cast<Farmer>(actor);
        std::shared_ptr<Soldier> soldier = std::dynamic_pointer_cast<Soldier>(actor);
        std::shared_ptr<Goblin> goblin = std::dynamic_pointer_cast<Goblin>(actor);
        if (farmer)
        {
            if (actor->GetGridPosition() == gridLocation || farmer->IsIdle() || farmer->GetHealth() <= 0) continue;
        }
        else if (soldier)
        {
            if (actor->GetGridPosition() == gridLocation || soldier->IsIdle()) continue;
        }
        else if (goblin)
        {
            if (actor->GetGridPosition() == gridLocation || goblin->GetHealth() <= 0) continue;
        }

        float actorSize = actor->GetSpriteData().mySize.x;

        Tga::Vector2f actorPosition = actor->GetSpriteData().myPosition;
        Tga::Vector2f offset = mySteeringParams.currentPosition - actorPosition;
        float distance = offset.Length();

        // Calculate combined separation radius (half the size of both actors)
        float separationThreshold = actorSize;

        if (distance > 0.01f && distance < separationThreshold) // Avoid zero division
        {
            float weight = (separationThreshold / distance) * 1.5f; // More weight when closer
            desiredVelocity += offset.Normalize() * weight;
            totalWeight += weight;
        }
    }


    if (desiredVelocity.Length() > 0.01f)
    {
        desiredVelocity = desiredVelocity.Normalize() * mySteeringParams.maxSpeed;
    }

    Tga::Vector2f steering = desiredVelocity;

    return { steering, totalWeight };
}

WeightedForce SteeringBehavior::GetSeekSteering()
{
    WeightedForce seek;
    Tga::Vector2f desiredVelocity = mySteeringParams.currentTarget - mySteeringParams.currentPosition;
    float distance = desiredVelocity.Length(); 

    desiredVelocity.Normalize();

    if (distance < mySteeringParams.arrivalRadius)
    {
        desiredVelocity *= mySteeringParams.maxSpeed * (distance / mySteeringParams.arrivalRadius); 
    }
    else
    {
        desiredVelocity *= mySteeringParams.maxSpeed;
    }

    Tga::Vector2f steering = desiredVelocity - mySteeringParams.currentVelocity;

    WeightedForce returnedInformation;
    returnedInformation.direction = steering;
    returnedInformation.weight = mySteeringParams.currentSeekWeight;
    return returnedInformation;
}
