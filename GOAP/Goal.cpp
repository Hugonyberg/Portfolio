#include "Goal.h"
#include <cmath>

Goal::Goal(const std::string& name, float targetValue)
    : myName(name), myTargetValue(targetValue) 
{
    myTargetState = WorldState();
}

Goal::Goal(const std::string& name, WorldState targetState)
{
    myName = name;
    myTargetState = targetState;
    myTargetValue = 0.0f;
}

bool Goal::IsFulfilled(float currentValue) const 
{
    return currentValue >= myTargetValue;
}

bool Goal::IsFulfilled(WorldState currentState) const
{
    return currentState >= myTargetState;
}

float Goal::GetDiscontentment(float currentValue) const 
{
    float difference = myTargetValue - currentValue;
    return difference;
}

std::string Goal::GetName() const { return myName; }
float Goal::GetTargetValue() const { return myTargetValue; }