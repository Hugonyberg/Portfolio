#pragma once
#include "Entity.h"

#include "Action.h"
#include "BehaviorTree.h"
#include "SteeringBehavior.h"

#include <deque>

enum class eFarmerAction
{
	Harvesting,
	Building,
	OnRoute,
	ReturningToBase,
	Idle,
};

enum class eFarmerTarget
{
	Resource,
	ConstructionSite,
	Base,
	None,
};

enum class eFarmerObjective
{
	Gold,
	Stone,
	Food,
	Wood,
	Building,
	None,
};

class Farmer : public Entity
{
public:
	Farmer();
	~Farmer() override = default;

	void InitializeFarmerBehavior();

	void Update(float aDeltaTime) override;
	void Render(Tga::SpriteDrawer& aSpriteDrawer) override;

	void SetTargetPositions();
	void GiveObjective(eActionType anObjective);

	bool IsIdle();
	eFarmerAction GetCurrentAction() { return myCurrentAction; }
	eFarmerTarget GetCurrentTarget() { return myCurrentTarget; }
	eFarmerObjective GetCurrentObjective() { return myCurrentObjective; }

	void FollowPath();
	void FollowWander();
	void UpdateWander();
	void Harvest();
	void DeliverToBase();
	void BuildHouse();
	void FindAndSetConstruction();

	void ChooseClosestTarget(eFarmerObjective anObjective);

	/* DebugFunctions*/
	void DebugDrawPath();
	/* DebugFunctions*/

private:
	void EvaluateObjective(); 
	std::shared_ptr<Entity> GetClosestEntity(std::vector<std::shared_ptr<Entity>> someEntities);

	std::shared_ptr<Entity> myTargetEntity;

	std::shared_ptr<BT::BehaviorTree> myBehaviorTree;
	SteeringBehavior mySteering;
	SteeringParams mySteeringParams;

	std::vector<GridLocation> myPath; // Stores A* path
	int myPathIndex = 0; // Current step in the path
	std::deque<GridLocation> myRecentlyVisited; // Stores the last 10 locations
	const size_t myMaxRecentTiles = 50; // Max history size

	std::vector<GridLocation> myTargetPositions;

	eFarmerAction myCurrentAction = eFarmerAction::Idle;
	eFarmerTarget myCurrentTarget = eFarmerTarget::None;
	eFarmerObjective myCurrentObjective = eFarmerObjective::None;

	bool myHasItem = false;

	Tga::SpriteSharedData myCarryingTexture{};
	Tga::SpriteSharedData myInventoryTexture{};
	Tga::Sprite2DInstanceData myInventoryData{};

	
	bool myArrivedAtObjective = false;

	float myTimePassed = 0.0f;
};