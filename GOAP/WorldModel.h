#pragma once
#ifndef WORLDMODEL_H
#define WORLDMODEL_H

#include "Action.h"
#include "WorldModelState.h"
#include <vector>

class WorldModel 
{
public:
    WorldModel();

    int EvaluateGoal(const Goal& goal) const;

    void SetAvailableActions(const std::vector<Action>& actions);
    std::vector<Action> GetAvailableActions() const;
    Action NextAction();

    int GetNumberOfSoldiers() const { return myNumberOfSoldiers; }
    int GetMaxNumberOfSoldiers() const { return myMaxNumberOfSoldiers; }
    int GetNumberOfFarmers() const { return myNumberOfFarmers; }
    int GetMaxNumberOfFarmers() const { return myMaxNumberOfFarmers; }

    bool HasMoreActions();
    WorldState GetWorldState();
    void SetWorldState(WorldState& aWorldState);

    size_t Hash() const; // For TranspositionTable

    bool operator==(const WorldModel& other) const 
    {
        return (myFood == other.myFood) 
            && (myWood == other.myWood) 
            && (myStone == other.myStone) 
            && (myGold == other.myGold)
            && (myHouses == other.myHouses)
            && (myPopulation == other.myPopulation)
            && (myNumberOfSoldiers == other.myNumberOfSoldiers)
            && (myMaxNumberOfSoldiers == other.myMaxNumberOfSoldiers)
            && (myNumberOfFarmers == other.myNumberOfFarmers)
            && (myMaxNumberOfFarmers == other.myMaxNumberOfFarmers);
    }

    bool operator<(const WorldModel& other) const 
    {
        return std::tie(myFood, myWood, myStone, myGold, myHouses, myNumberOfSoldiers, myMaxNumberOfSoldiers, myNumberOfFarmers, myMaxNumberOfFarmers) <
            std::tie(other.myFood, other.myWood, other.myStone, other.myGold, other.myHouses, other.myNumberOfSoldiers, other.myMaxNumberOfSoldiers, other.myNumberOfFarmers, other.myMaxNumberOfFarmers);
    }

private:

    int myFood, myWood, myStone, myGold;

    int myPopulation = 1;
    int myNumberOfSoldiers = 0;
    int myMaxNumberOfSoldiers = 0;
    int myNumberOfFarmers = 1;
    int myMaxNumberOfFarmers = 5;

    int myHouses = 0;

    int myActionIndex = 0;
    std::vector<Action> myAvailableActions;
};

#endif // WORLDMODEL_H