#pragma once
#ifndef GOAL_H
#define GOAL_H

#include <string>
#include "WorldModelState.h"

class Goal 
{
public:
    Goal() = default;
    Goal(const std::string& name, float targetValue);
    Goal(const std::string& name, WorldState targetState);

    bool IsFulfilled(float currentValue) const;
    bool IsFulfilled(WorldState currentState) const;

    float GetDiscontentment(float currentValue) const;

    std::string GetName() const;
    float GetTargetValue() const;
    WorldState GetTargetState() const { return myTargetState; }

private:
    std::string myName;
    float myTargetValue;
    WorldState myTargetState;
};

#endif // GOAL_H