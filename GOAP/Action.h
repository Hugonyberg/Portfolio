#pragma once
#ifndef ACTION_H
#define ACTION_H

#include "Goal.h"
#include <string>
#include <unordered_map>

#include "WorldModelState.h"

class WorldModel;

enum class eActionType
{
    GatherFood,
    GatherStone,
    GatherWood,
    GatherGold,
    BuildHouse,
    SpawnFarmer,
    SpawnSoldier,
    DeliverToBase,
    Count,
};

class Action 
{
public:
    Action();
    Action(const std::string& name, const eActionType aType, float cost);
    Action(const std::string& name, const eActionType aType);

    void AddGoalEffect(const Goal& goal, float change);
    void AddGoalEffect(WorldState stateChange);
    float GetGoalChange(const Goal& goal) const;
    float GetCost() const;
    float CalculateCost(const WorldState& before, const WorldState& after) const;
    float GetPriority(const Goal& goal, const WorldModel* worldModel) const;
    eActionType GetType() { return myType; }
    WorldState GetWorldStateEffect() const { return myGoalEffect; }
    std::string GetName() const;

    const bool IsValid();

private:
    std::string myName;
    eActionType myType;
    float myCost;
    std::unordered_map<std::string, float> myGoalEffects;
    WorldState myGoalEffect;
};

#endif // ACTION_H