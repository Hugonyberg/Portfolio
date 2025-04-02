#pragma once
#include "Entity.h"

#include "Action.h"
#include "BehaviorTree.h"
#include "SteeringBehavior.h"

#include <deque>

enum class eSoldierAction
{
	Searching,
	Killing,
	OnRoute,
	ReturningToBase,
	Idle,
};

enum class eSoldierTarget
{
	Enemy,
	ThreatArea,
	None,
};

enum class eSoldierObjective
{
	Patrol,
	Fight,
	None,
};

class Soldier : public Entity
{
public:
	Soldier();
	~Soldier() override = default;

	void InitializeSoldierBehavior();

	void Update(float aDeltaTime) override;
	void Render(Tga::SpriteDrawer& aSpriteDrawer) override;

	void SetTargetPosition(GridLocation aLocation);
	void GiveObjective(eActionType anObjective);

	bool IsIdle();
	eSoldierAction GetCurrentAction() { return myCurrentAction; }
	eSoldierTarget GetCurrentTarget() { return myCurrentTarget; }
	eSoldierObjective GetCurrentObjective() { return myCurrentObjective; }

	void FollowPath();

	void GeneratePatrolPath(GridLocation aCenter, int aPatrolRadius);
	void UpdatePatrol();
	void UpdateWander();

	void ChooseClosestTarget(eSoldierObjective anObjective);

	/* DebugFunctions*/
	void DebugDrawPath();
	/* DebugFunctions*/

private:
	void EvaluateObjective();
	std::shared_ptr<Entity> GetClosestEntity(std::vector<std::shared_ptr<Entity>> someEntities);
	int DistanceToClosestFarmer(const GridLocation& aMove);

	std::shared_ptr<Entity> myTargetEntity;

	std::shared_ptr<BT::BehaviorTree> myBehaviorTree;
	SteeringBehavior mySteering;
	SteeringParams mySteeringParams;

	std::vector<GridLocation> myPath; // Stores A* path
	int myPathIndex = 0; // Current step in the path
	std::vector<GridLocation> myPatrolTargets;
	int myCurrentPatrolIndex = 0;
	int myPatrolRadius = 5; // Default patrol radius
	GridLocation myPatrolCenter;

	std::deque<GridLocation> myRecentlyVisited; // Stores the last 10 locations
	const size_t myMaxRecentTiles = 10; // Max history size

	eSoldierAction myCurrentAction = eSoldierAction::Idle;
	eSoldierTarget myCurrentTarget = eSoldierTarget::None;
	eSoldierObjective myCurrentObjective = eSoldierObjective::None;

	Tga::SpriteSharedData myCarryingTexture{};
	Tga::SpriteSharedData myInventoryTexture{};
	Tga::Sprite2DInstanceData myInventoryData{};

	bool myArrivedAtObjective = false;
	bool myReversePath = false;

	float myTimePassed = 0.0f;
};