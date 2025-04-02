#pragma once
#ifndef ACTIONPLANNER_H
#define ACTIONPLANNER_H

#include "WorldModel.h"
#include "Goal.h"
#include "TranspositionTable.h"
#include <vector>

#include <queue>
#include <vector>
#include <limits>

#include "WorldModelState.h"

struct AStarNode 
{
    WorldModel model;
    std::vector<Action> actions;  // actions taken to reach this node
    float cost;                   // cost so far (g)
    float heuristic;              // estimated cost to goal (h)
    int depth;                    // current search depth

    // f = g + h: total estimated cost
    float f() const { return cost + heuristic; }
};

struct CompareAStarNode 
{
    bool operator()(const AStarNode& a, const AStarNode& b) const 
    {
        return a.f() > b.f();
    }
};

class Heuristic 
{
public:
    static float Estimate(float currentValue, float targetValue) 
    {
        float difference = targetValue - currentValue;
        return difference;
    }

    static float EvaluateGoalHeuristic(const WorldState& current, const WorldState& target)
    {
        // RESOURCES
        float diffFood = static_cast<float>(std::abs(target.food - current.food));
        float diffWood = static_cast<float>(std::abs(target.wood - current.wood));
        float diffStone = static_cast<float>(std::abs(target.stone - current.stone));
        float diffGold = static_cast<float>(std::abs(target.gold - current.gold));

        // POPULATION
        float diffSoldiers = static_cast<float>(std::abs(target.numOfSoldiers - current.numOfSoldiers));
        float diffMaxSoldiers = static_cast<float>(std::abs(target.maxNumOfSoldiers - current.maxNumOfSoldiers));
        float diffFarmers = static_cast<float>(std::abs(target.numOfFarmers - current.numOfFarmers));
        float diffMaxFarmers = static_cast<float>(std::abs(target.maxNumOfFarmers - current.maxNumOfFarmers));
        float diffPop = static_cast<float>(std::abs(target.population - current.population));

        float diffHouses = static_cast<float>(std::abs(target.houses - current.houses));

        return (diffFood) +
            (diffWood) +
            (diffStone) +
            (diffGold) +
            (diffSoldiers) +
            (diffMaxSoldiers) +
            (diffFarmers) +
            (diffMaxFarmers) +
            (diffPop) +
            (diffHouses);
    }
};

class ActionPlanner 
{
public:
    ActionPlanner(int maxDepth);

    Action PlanAction(WorldModel& worldModel, const Goal& goal);

private:
    const int myMaxDepth;

    std::pair<float, Action> DoDepthFirst(WorldModel& worldModel, const Goal& goal, Heuristic& heuristic, TranspositionTable& transTable, int maxDepth, float cutoff);
  
};

#endif // ACTIONPLANNER_H