#pragma once
#include "stdafx.h"

struct WeightedForce
{
	Tga::Vector2f direction;
	float weight;

	WeightedForce() : direction(0.f, 0.f), weight(0.f) {}
	WeightedForce(const Tga::Vector2f& dir, float w) : direction(dir), weight(w) {}
};

struct SteeringParams
{
    Tga::Vector2f currentVelocity;
    Tga::Vector2f currentPosition;
    Tga::Vector2f currentTarget;

    float separationRadius;
    float currentSeparationWeight;

    float arrivalRadius;
    float currentSeekWeight;

    float maxSpeed;
};

class SteeringBehavior
{
public:
    SteeringBehavior() = default;
    ~SteeringBehavior() = default;

    void SetSteeringParameters(const SteeringParams& someSteeringParams);
    Tga::Vector2f UpdateSteering();

    WeightedForce GetSeparationSteering();
    WeightedForce GetSeekSteering();

private:
    SteeringParams mySteeringParams;
};