#include "stdafx.h"
#include "ActionPlanner.h"
#include <limits>

ActionPlanner::ActionPlanner(int maxDepth) : myMaxDepth(maxDepth) {}

Action ActionPlanner::PlanAction(WorldModel& worldModel, const Goal& goal)
{

    Heuristic heuristic;
    float cutoff = heuristic.EvaluateGoalHeuristic(worldModel.GetWorldState(), goal.GetTargetState());
    int depth = myMaxDepth;

    while (cutoff > 0.0f)
    {
        TranspositionTable transTable;
        std::pair<float, Action> result = DoFastDepthFirst(worldModel, goal, heuristic, transTable, depth, cutoff);
        float newCutoff = result.first;
        Action action = result.second;

        if (newCutoff == 0.0f)
        {
            return action;
        }
        cutoff = newCutoff;
    }

    return Action("Idle", eActionType::Count, 9999.0f);
}


std::pair<float, Action> ActionPlanner::DoDepthFirst( 
    WorldModel& worldModel,
    const Goal& goal,
    Heuristic& heuristic,
    TranspositionTable& transTable,
    int maxDepth,
    float cutoff
)
{
    // Store states, actions, and costs at each depth
    std::vector<WorldModel> models(maxDepth + 1);
    std::vector<Action> actions(maxDepth);
    std::vector<float> costs(maxDepth + 1, 0.0f);

    models[0] = worldModel;
    models[0].SetAvailableActions(worldModel.GetAvailableActions(goal));

    // Track the best alternative cutoff/cost
    int currentDepth = 0;
    float smallestCutoff = std::numeric_limits<float>::infinity();

    while (currentDepth >= 0)
    {
        // Stop searching if we reached max depth
        if (currentDepth >= maxDepth)
        {
            return { 0.0f, actions[0] };
        }

        // Compute estimated cost with heuristic
        float cost = heuristic.EvaluateGoalHeuristic(models[currentDepth].GetWorldState(), goal.GetTargetState()) + costs[currentDepth];

        // Prune paths that exceed cutoff
        if (cost > cutoff)
        {
            if (cost < smallestCutoff)
            {
                smallestCutoff = cost; 
            }

            currentDepth -= 1;
            continue;
        }

        // Get the next available action
        Action nextAction = models[currentDepth].NextAction();
        if (!nextAction.IsValid())
        {
            currentDepth -= 1;
            continue;
        }

        // Apply the action to create a new state
        models[currentDepth + 1] = models[currentDepth]; // Copy state
        actions[currentDepth] = nextAction; // Store action at depth
        models[currentDepth + 1].ApplyAction(nextAction);
        costs[currentDepth + 1] = costs[currentDepth] + nextAction.CalculateCost(models[currentDepth].GetWorldState(), goal.GetTargetState());
        models[currentDepth + 1].SetAvailableActions(models[currentDepth + 1].GetAvailableActions(goal));

        // Check if this state has been visited before
        if (!transTable.Has(models[currentDepth + 1]))
        {
            transTable.Add(models[currentDepth + 1], currentDepth, costs[currentDepth + 1]);
            currentDepth += 1; 
        }
        else
        {
            currentDepth -= 1;
        }
    }

    // If no valid action found, return best alternative cutoff/cost
    return { smallestCutoff, actions[0] };
}