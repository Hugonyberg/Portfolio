#pragma once
#include "Farmer.h"
#include "Soldier.h"
#include "Goblin.h"

#include "IDAPlanner.h"

class Base : public Entity
{
public:
	void InitBase();

	void Update(float aDeltaTime) override;

	void GiveResource(eFarmerObjective aResource);

	void DebugWriteResources();

	void RemoveFarmer();
	
	int GetNumberOfFarmers() const { return myWorld.GetNumberOfFarmers(); }
private:
	void GiveOrder();
	void SpawnFarmer();
	void SpawnSoldier();

	int myCurrentActionIndex;

	std::vector<std::shared_ptr<Farmer>> myFarmers;
	std::vector<std::shared_ptr<Soldier>> mySoldiers;
	std::vector<Action> myPlannedActions;
	std::vector<Action> mySavedActions;

	WorldState myWorldState;
	WorldState myPreviousWorldState;
	WorldModel myWorld;

	Goal myGoal;

	float myOrderTimer = 1.0f;
};