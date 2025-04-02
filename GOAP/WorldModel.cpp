#include "WorldModel.h"
#include <functional>
#include <tuple>
#include <algorithm>

#include "WorldModelState.h"

WorldModel::WorldModel()
{
    myActionIndex = 0;
    myFood = 0;
    myWood = 0;
    myStone = 0;
    myGold = 0;
}

void WorldModel::ApplyAction(const Action& action) 
{
    myFood += action.GetWorldStateEffect().food;
    myStone += action.GetWorldStateEffect().stone;
    myGold += action.GetWorldStateEffect().gold;
    myWood += action.GetWorldStateEffect().wood;
    myPopulation += (action.GetWorldStateEffect().numOfFarmers + action.GetWorldStateEffect().numOfSoldiers);
    myNumberOfSoldiers += action.GetWorldStateEffect().numOfSoldiers;
    myMaxNumberOfSoldiers += action.GetWorldStateEffect().maxNumOfSoldiers;
    myNumberOfFarmers += action.GetWorldStateEffect().numOfFarmers;
    myMaxNumberOfFarmers += action.GetWorldStateEffect().maxNumOfFarmers;
    myHouses += action.GetWorldStateEffect().houses;
}

void WorldModel::SetAvailableActions(const std::vector<Action>& actions)
{
    myAvailableActions = actions;
    myActionIndex = 0;  // Reset index when setting new actions
}

std::vector<Action> WorldModel::GetAvailableActions() const
{
    std::vector<Action> actions;

    // Add "Gather Wood" action
    if (myWood < 10) 
    {
        Action gatherWood("Gather Wood", eActionType::GatherWood);

        WorldState goalEffect;
        goalEffect.wood = 1;
        gatherWood.AddGoalEffect(goalEffect); // Increases myWood by 1

        actions.push_back(gatherWood);
    }
    // Add "Gather Stone" action
    if (myStone < 10)
    {
        Action gatherStone("Gather Stone", eActionType::GatherStone);

        WorldState goalEffect;
        goalEffect.stone = 1;
        gatherStone.AddGoalEffect(goalEffect); // Increases myStone by 1

        actions.push_back(gatherStone);
    }
    // Add "Spawn Farmer" action
    if (myFood >= 2 && myNumberOfFarmers < myMaxNumberOfFarmers)
    {
        Action spawnFarmer("Spawn Farmer", eActionType::SpawnFarmer);

        WorldState goalEffect;
        goalEffect.food = -2;
        goalEffect.numOfFarmers = 1;
        goalEffect.population = 1;
        spawnFarmer.AddGoalEffect(goalEffect);

        actions.push_back(spawnFarmer);
    }

    // Add "Spawn Soldier" action
    if (myFood >= 1 && myGold >= 3 && myNumberOfSoldiers < myMaxNumberOfSoldiers)
    {
        Action spawnSoldier("Spawn Soldier", eActionType::SpawnSoldier);

        WorldState goalEffect;
        goalEffect.food = -1;
        goalEffect.gold = -3;
        goalEffect.numOfSoldiers = 1;
        goalEffect.population = 1;
        spawnSoldier.AddGoalEffect(goalEffect);

        actions.push_back(spawnSoldier);
    }

    // Add "Gather Food" action
    if (myFood < 10) 
    {
        Action gatherFood("Gather Food", eActionType::GatherFood);

        WorldState goalEffect;
        goalEffect.food = 1;
        gatherFood.AddGoalEffect(goalEffect); // Increases myFood by 1

        actions.push_back(gatherFood);
    }
    // Add "Gather Gold" action
    if (myGold < 10)
    {
        Action gatherGold("Gather Gold", eActionType::GatherGold);

        WorldState goalEffect;
        goalEffect.gold = 1;
        gatherGold.AddGoalEffect(goalEffect); // Increases myGold by 1

        actions.push_back(gatherGold);
    }
    // Add "Build House" action

    if (myFood >= 2 && myWood >= 5 && myStone >= 5) 
    {
        Action buildHouse("Build House", eActionType::BuildHouse);
    
        WorldState goalEffect;
        goalEffect.food = -2;
        goalEffect.wood = -5;
        goalEffect.stone = -5;
        goalEffect.maxNumOfFarmers = 5;
        goalEffect.maxNumOfSoldiers = 3;
        goalEffect.houses = 1;
        buildHouse.AddGoalEffect(goalEffect);
    
        actions.push_back(buildHouse);
    }
    
    return actions;
}

Action WorldModel::NextAction()
{
    if (myActionIndex < myAvailableActions.size())
    {
        return myAvailableActions[myActionIndex++]; // Return & advance index
    }
    
    return Action("", eActionType::Count, 0);
}

bool WorldModel::HasMoreActions()
{
    return myActionIndex < myAvailableActions.size();
}

WorldState WorldModel::GetWorldState()
{
    WorldState state;
    state.food = myFood;
    state.stone = myStone;
    state.gold = myGold;
    state.wood = myWood;

    state.population = myPopulation;
    state.numOfSoldiers = myNumberOfSoldiers;
    state.maxNumOfSoldiers = myMaxNumberOfSoldiers;
    state.numOfFarmers = myNumberOfFarmers;
    state.maxNumOfFarmers = myMaxNumberOfFarmers;

    state.houses = myHouses;

    return state;
}

void WorldModel::SetWorldState(WorldState& aWorldState)
{
    myFood = aWorldState.food;
    myWood = aWorldState.wood;
    myStone = aWorldState.stone;
    myGold = aWorldState.gold;
    
    myNumberOfSoldiers = aWorldState.numOfSoldiers;
    myMaxNumberOfSoldiers = aWorldState.maxNumOfSoldiers;
    myNumberOfFarmers = aWorldState.numOfFarmers;
    myMaxNumberOfFarmers = aWorldState.maxNumOfFarmers;
    
    myHouses = aWorldState.houses;
    myPopulation = aWorldState.population;
}

size_t WorldModel::Hash() const 
{
    size_t seed = 0;

    // Hash combine function
    auto hashCombine = [](size_t& seed, size_t value) 
        {
            seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        };

    hashCombine(seed, std::hash<int>{}(myMaxNumberOfFarmers));
    hashCombine(seed, std::hash<int>{}(myNumberOfFarmers));
    hashCombine(seed, std::hash<int>{}(myMaxNumberOfSoldiers));
    hashCombine(seed, std::hash<int>{}(myNumberOfSoldiers));
    hashCombine(seed, std::hash<int>{}(myPopulation));
    hashCombine(seed, std::hash<int>{}(myFood));
    hashCombine(seed, std::hash<int>{}(myWood));
    hashCombine(seed, std::hash<int>{}(myStone));
    hashCombine(seed, std::hash<int>{}(myGold));

    return seed;
}