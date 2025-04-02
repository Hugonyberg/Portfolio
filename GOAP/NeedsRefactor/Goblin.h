#pragma once
#include "Entity.h"

#include "Action.h"
#include "BehaviorTree.h"
#include "SteeringBehavior.h"

#include <deque>

enum class eGoblinAction
{
	Searching,
	Killing,
	OnRoute,
	Idle,
};

enum class eGoblinTarget
{
	Enemy,
	ThreatArea,
	None,
};

enum class eGoblinObjective
{
	Patrol,
	Fight,
	None,
};

class Goblin : public Entity
{
public:
	Goblin();
	~Goblin() override = default;

	void Update(float aDeltaTime) override;
	void Render(Tga::SpriteDrawer& aSpriteDrawer) override;

	void SetTargetPosition(GridLocation aLocation);

	eGoblinAction GetCurrentAction() { return myCurrentAction; }
	eGoblinTarget GetCurrentTarget() { return myCurrentTarget; }
	eGoblinObjective GetCurrentObjective() { return myCurrentObjective; }

	void FollowPath();

	void ChooseClosestTarget(eGoblinObjective anObjective);

	void UpdateWander();

	const bool IsDead() const { return myIsDead; }
	void SetIsDead(bool anIsDead) { myIsDead = anIsDead; }

	/* DebugFunctions*/
	void DebugDrawPath();
	/* DebugFunctions*/

private:
	std::shared_ptr<Entity> myTargetEntity;

	std::shared_ptr<BT::BehaviorTree> myBehaviorTree;
	SteeringBehavior mySteering;
	SteeringParams mySteeringParams;

	std::vector<GridLocation> myPath; // Stores A* path
	int myPathIndex = 0; // Current step in the path
	std::deque<GridLocation> myRecentlyVisited; // Stores the last 10 locations
	const size_t myMaxRecentTiles = 20; // Max history size

	eGoblinAction myCurrentAction = eGoblinAction::Idle;
	eGoblinTarget myCurrentTarget = eGoblinTarget::None;
	eGoblinObjective myCurrentObjective = eGoblinObjective::None;

	bool myArrivedAtObjective = false;
	bool myRecentlySpawned = true;
	bool myIsDead = false;

	float myTimePassed = 0.0f;

};