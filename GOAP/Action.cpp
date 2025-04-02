#include "stdafx.h"
#include "Action.h"

#include "WorldModel.h"

Action::Action()
{
    myName = "";
    myType = eActionType::Count;
    myCost = 0.0f;
}

Action::Action(const std::string& name, const eActionType aType, float cost) : myName(name), myType(aType), myCost(cost) {}

Action::Action(const std::string& name, const eActionType aType)
{
    myName = name;
    myType = aType;
}

void Action::AddGoalEffect(WorldState stateChange)
{
    myCost = static_cast<float>(stateChange.GetStateCost());
    myGoalEffect = stateChange;
}

float Action::GetGoalChange(const Goal& goal) const 
{
    auto it = myGoalEffects.find(goal.GetName());
    return it != myGoalEffects.end() ? it->second : 0.0f;
}

float Action::GetCost() const 
{ 
    return myCost; 
}

float Action::CalculateCost(const WorldState& before, const WorldState& target) const
{
    float foodThreshold = 2.0f;
    bool lowFood = (before.food < foodThreshold);

    // **Boolean Priority Flags**
    bool ShouldSpawnSoldier = (before.numOfSoldiers < before.maxNumOfSoldiers && before.food >= 1 && before.gold >= 3);
    bool ShouldBuildHouse = (before.food >= 2 && before.wood >= 5 && before.stone >= 5 && (target.houses > before.houses));
    bool ShouldSpawnFarmer = (before.numOfFarmers < before.maxNumOfFarmers && before.food >= 2 && !ShouldSpawnSoldier);
    bool ShouldPrioritiseFood = (!ShouldSpawnFarmer && lowFood && !ShouldBuildHouse && !ShouldSpawnSoldier);

    // Find the most abundant resource for prioritization
    int minResource = std::min({ before.gold, before.stone, before.wood });

    bool ShouldPrioritiseWood = false;
    bool ShouldPrioritiseStone = false;
    bool ShouldPrioritiseGold = false;

    if (!ShouldSpawnSoldier && !ShouldBuildHouse && !ShouldSpawnFarmer && !ShouldPrioritiseFood)
    {
        ShouldPrioritiseWood = (before.wood == minResource);
        ShouldPrioritiseStone = (before.stone == minResource);
        ShouldPrioritiseGold = (before.gold == minResource);
        if (before.numOfSoldiers < before.maxNumOfSoldiers)
        {
            ShouldPrioritiseWood = false;
            ShouldPrioritiseStone = false;
            ShouldPrioritiseGold = true;
        }
        else if ((target.houses <= before.houses))
        {
            int newMin = std::min({ before.food, before.stone, before.wood });
            ShouldPrioritiseWood = (before.wood == newMin);
            ShouldPrioritiseStone = (before.stone == newMin);
            ShouldPrioritiseFood = (before.food == newMin);
        }
    }

    switch (myType)
    {
    case eActionType::GatherFood:
    {
        if (ShouldPrioritiseFood)
        {
            return myCost;
        }
        else return myCost + 1.0f;
        break;
    }
    case eActionType::GatherWood:
    {
        if (ShouldPrioritiseWood)
        {
            return myCost;
        }
        else return myCost + 1.0f;
        break;
    }
    case eActionType::GatherStone:
    {
        if (ShouldPrioritiseStone)
        {
            return myCost;
        }
        else return myCost + 1.0f;
        break;
    }
    case eActionType::GatherGold:
    {
        if (ShouldPrioritiseGold)
        {
            return myCost;
        }
        else return myCost + 1.0f;
        break;
    }
    case eActionType::BuildHouse:
    {
        return myCost; // Direct return for the highest priority action
    }
    case eActionType::SpawnFarmer:
    {
        return myCost; // Direct return for second priority
    }
    case eActionType::SpawnSoldier:
    {
        return myCost; // Direct return for second priority
    }
    }

    return myCost;
}

float Action::GetPriority(const Goal& goal, const WorldModel* worldModel) const
{
    float remaining = goal.GetTargetValue() - worldModel->EvaluateGoal(goal);

    float effect = 0.f;
    auto it = myGoalEffects.find(goal.GetName());
    if (it != myGoalEffects.end())
    {
        effect = it->second;
    }

    float netGap = remaining - effect;

    // Incorporate the cost of the action.
    float cost = GetCost();
    if (cost <= 0.0001f)
    {
        cost = 0.0001f;
    }

    float progressPerCost = effect / cost;

    float invertedNetGap = 1.0f / (1.0f + netGap); // Lower netGap => higher invertedNetGap.

    float weightNetGap = 0.5f;
    float weightProgress = 0.5f;
    float priority = weightNetGap * invertedNetGap + weightProgress * progressPerCost;

    return priority;
}

std::string Action::GetName() const { return myName; }

const bool Action::IsValid()
{
    if (myName == "" && myCost == 0.0f)
    {
        return false;
    }
    else return true;
}
