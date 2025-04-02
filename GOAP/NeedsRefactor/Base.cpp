#include "stdafx.h"
#include "Base.h"

#include "MainSingleton.h"

void Base::InitBase()
{
    myWorldState.food = 2;
    SpawnFarmer();
    myWorldState.maxNumOfFarmers = 5;
    myWorldState.food = 1;
    myWorldState.gold = 3;
    myWorldState.maxNumOfSoldiers = 3;
    SpawnSoldier();

    WorldModel world;
    WorldState goalState;
    //goalState.food = 5;
    //goalState.wood = 5;
    //goalState.stone = 5;
    //goalState.population = 10;
    //goalState.numOfFarmers = 10;
    //goalState.maxNumOfFarmers = 10;
    //goalState.houses = 1;
    goalState.food = 5;
    goalState.wood = 5;
    goalState.gold = 5;
    goalState.stone = 5;
    goalState.population = 88;
    goalState.numOfFarmers = 55;
    goalState.maxNumOfFarmers = 55;
    goalState.numOfSoldiers = 33;
    goalState.maxNumOfSoldiers = 33;
    goalState.houses = 10;
    std::vector<Goal> goals =
    {
        Goal("Increase Population", goalState)
        /*Goal("Increase Population", 10),
        Goal("Gather Wood", 10),
        Goal("Gather Stone", 10),
        Goal("Gather Food", 10),*/
    };

    myGoal = goals[0];

    //IDAPlanner planner(10); // Max depth

    //std::cout << "Starting IDA* GOAP Test with Multiple Goals...\n";
    //bool allGoalsFulfilled = false;
    //int i = 0;
    ////for (int i = 0; i < 40; i++) 
    //while (!allGoalsFulfilled)
    //{

    //    std::cout << "\nTurn " << (i + 1) << ":\n";
    //    i++;

    //    // Determine the highest priority goal
    //    Goal* topGoal = &goals[0];
    //    for (Goal& g : goals) 
    //    {
    //        if (world.EvaluateGoal(g) < topGoal->GetTargetValue()) 
    //        {
    //            topGoal = &g;
    //        }
    //    }

    //    Action bestAction = planner.PlanAction(world, *topGoal);

    //    std::cout << "Best action chosen: " << bestAction.GetName() << "\n";

    //    myPlannedActions.push_back(bestAction);
    //    world.ApplyAction(bestAction);

    //    for (const Goal& g : goals) 
    //    {
    //        std::cout << g.GetName() << " Level: "
    //            << world.EvaluateGoal(g) << " / "
    //            << g.GetTargetValue() << "\n";
    //    }

    //    allGoalsFulfilled = true;
    //    for (const Goal& g : goals) 
    //    {
    //        //if (!g.IsFulfilled(static_cast<float>(world.EvaluateGoal(g))))
    //        if (!g.IsFulfilled(world.GetWorldState()))
    //        {
    //            allGoalsFulfilled = false;
    //            break;
    //        }
    //    }

    //    if (allGoalsFulfilled) 
    //    {
    //        std::cout << "All Goals Fulfilled! AI successfully planned resource gathering.\n";
    //        break;
    //    }
    //}
}

void Base::Update(float aDeltaTime)
{
    if (!myGoal.IsFulfilled(myWorld.GetWorldState()))
    {
        aDeltaTime;
        MainSingleton::GetInstance().SetWorldState(myWorldState);

        GiveOrder();

        myPreviousWorldState = myWorldState;
    }
}

void Base::GiveResource(eFarmerObjective aResource)
{
    switch (aResource)
    {
    case eFarmerObjective::Gold:
    {
        myWorldState.gold++;
        myWorld.SetWorldState(myWorldState);
        break;
    }
    case eFarmerObjective::Stone:
    {
        myWorldState.stone++;
        myWorld.SetWorldState(myWorldState);
        break;
    }
    case eFarmerObjective::Wood:
    {
        myWorldState.wood++;
        myWorld.SetWorldState(myWorldState);
        break;
    }
    case eFarmerObjective::Food:
    {
        myWorldState.food++;
        myWorld.SetWorldState(myWorldState);
        break;
    }
    case eFarmerObjective::Building:
    {
        /*myWorldState.food -= 2;
        myWorldState.stone -= 5;
        myWorldState.wood -= 5;*/
        myWorldState.houses++;
        myWorldState.maxNumOfFarmers += 5;
        myWorldState.maxNumOfSoldiers += 3;
        myWorld.SetWorldState(myWorldState);
        break;
    }
    default:
        break;
    }

}

void Base::DebugWriteResources()
{
    std::cout << "*******************" << std::endl;
    std::cout << "Iteration #" << myCurrentActionIndex << std::endl;
    std::cout << "Current World State" << std::endl;
    std::cout << "Food = " << myWorldState.food << std::endl;
    std::cout << "Wood = " << myWorldState.wood << std::endl;
    std::cout << "Stone = " << myWorldState.stone << std::endl;
    std::cout << "Gold = " << myWorldState.gold<< std::endl;
    std::cout << "Houses = " << myWorldState.houses<< std::endl;
    std::cout << "Soldiers = " << myWorldState.numOfSoldiers<< std::endl;
    std::cout << "Max Soldiers = " << myWorldState.maxNumOfSoldiers << std::endl;
    std::cout << "Farmers = " << myWorldState.numOfFarmers<< std::endl;
    std::cout << "Max Farmers = " << myWorldState.maxNumOfFarmers << std::endl;
    std::cout << "*******************" << std::endl;
}

void Base::GiveOrder()
{
    auto hud = MainSingleton::GetInstance().GetHUD();
    HUDMessage msg;


    IDAPlanner planner(2); // Max depth

    myPlannedActions.clear();
    Action bestAction = planner.FastPlanAction(myWorld, myGoal);
    myPlannedActions.push_back(bestAction);

    if (!mySavedActions.empty())
    {
        auto it = mySavedActions.begin(); // Use an iterator for safe erasure

        while (it != mySavedActions.end())
        {
            if (it->GetType() == eActionType::SpawnFarmer)
            {
                if (myWorldState.food >= 2 && myWorldState.numOfFarmers < myWorldState.maxNumOfFarmers)
                {
                    msg.text = "Spawn Farmer";
                    hud->AddMessage(msg);
                    SpawnFarmer();
                    it = mySavedActions.erase(it); // Erase the current action and move to the next one
                    continue; // Avoid checking further conditions after erasure
                }
            }

            if (it->GetType() == eActionType::SpawnSoldier)
            {
                if (myWorldState.food >= 1 && myWorldState.gold >= 3 && myWorldState.numOfSoldiers < myWorldState.maxNumOfSoldiers)
                {
                    msg.text = "Spawn Soldier";
                    hud->AddMessage(msg);
                    SpawnSoldier();
                    it = mySavedActions.erase(it); // Erase the current action and move to the next one
                    continue; // Avoid checking further conditions after erasure
                }
            }

            if (!mySavedActions.empty() && it->GetType() == eActionType::BuildHouse)
            {
                for (auto farmer : myFarmers)
                {
                    if (farmer->IsIdle())
                    {
                        if (myWorldState.stone >= 5 && myWorldState.food >= 2 && myWorldState.wood >= 5)
                        {
                            myWorldState.food -= 2;
                            myWorldState.stone -= 5;
                            myWorldState.wood -= 5;
                            myWorld.SetWorldState(myWorldState);

                            auto actionType = it->GetType(); // Copy before erase
                            it = mySavedActions.erase(it);   // Safe to erase
                            farmer->GiveObjective(actionType);

                            continue; // Avoid checking further conditions after erasure
                        }
                    }
                }
            }

            if (it != mySavedActions.end())
            {
                ++it; // Move to the next element
            }
        }
    }

    if (!myPlannedActions.empty())
    {
        if (myPlannedActions.front().GetType() == eActionType::SpawnFarmer)
        {
                mySavedActions.push_back(myPlannedActions.front());
                myPlannedActions.erase(myPlannedActions.begin());
        }
        else if (myPlannedActions.front().GetType() == eActionType::SpawnSoldier)
        {
            mySavedActions.push_back(myPlannedActions.front());
            myPlannedActions.erase(myPlannedActions.begin());
        }
        else
        {
            for (auto farmer : myFarmers)
            {
                if (farmer->IsIdle())
                {
                    if (myPlannedActions.empty()) return;

                    if (myPlannedActions.front().GetType() == eActionType::SpawnFarmer)
                    {
                        mySavedActions.push_back(myPlannedActions.front());
                        myPlannedActions.erase(myPlannedActions.begin());
                        continue;
                    }
                    else if (myPlannedActions.front().GetType() == eActionType::SpawnSoldier)
                    {
                        mySavedActions.push_back(myPlannedActions.front());
                        myPlannedActions.erase(myPlannedActions.begin());
                        continue;
                    }
                    else if (myPlannedActions.front().GetType() == eActionType::BuildHouse)
                    {
                        mySavedActions.push_back(myPlannedActions.front());
                        myPlannedActions.erase(myPlannedActions.begin());
                        continue;
                    }

                    myCurrentActionIndex++;
                    farmer->GiveObjective(myPlannedActions.front().GetType());
                    myPlannedActions.erase(myPlannedActions.begin());
                }
            }
        }
    }
}
void Base::RemoveFarmer()
{
    myWorldState.numOfFarmers--;
    myWorld.SetWorldState(myWorldState);
}

void Base::SpawnFarmer()
{
    int gridWidth = MainSingleton::GetInstance().GetWidth();
    int gridHeight = MainSingleton::GetInstance().GetHeight();

    std::shared_ptr<Entity> farmer = Entity::CreateEntity(EntityType::Farmer);
    MainSingleton::GetInstance().AddEntity(farmer);
    std::shared_ptr<Farmer> farmerEntity = std::dynamic_pointer_cast<Farmer>(farmer);
    farmerEntity->SetGridPosition({ (gridWidth / 2) - 2,(gridHeight / 2) - 2 }, true);
    farmerEntity->GiveObjective(eActionType::Count);

    myWorldState.numOfFarmers++;
    myWorldState.population++;
    myWorldState.food -= 2;
    myWorld.SetWorldState(myWorldState);
    myFarmers.push_back(farmerEntity);
}

void Base::SpawnSoldier()
{
    int gridWidth = MainSingleton::GetInstance().GetWidth();
    int gridHeight = MainSingleton::GetInstance().GetHeight();

    std::shared_ptr<Entity> soldier = Entity::CreateEntity(EntityType::Soldier);
    MainSingleton::GetInstance().AddEntity(soldier);
    std::shared_ptr<Soldier> soldierEntity = std::dynamic_pointer_cast<Soldier>(soldier);
    soldierEntity->SetGridPosition({ (gridWidth / 2) - 2,(gridHeight / 2) - 2 }, true);
    soldierEntity->GiveObjective(eActionType::Count);

    myWorldState.numOfSoldiers++;
    myWorldState.population++;
    myWorldState.food -= 1;
    myWorldState.gold -= 3;
    myWorld.SetWorldState(myWorldState);
    mySoldiers.push_back(soldierEntity);
}

