#pragma once
#include "GridLocation.h"
#include "Entity.h"

class Grid;
class Base;
class HUD;

class GameWorld
{
public:
	GameWorld(); 
	~GameWorld();

	void Init();
	void Update(float aDeltaTime); 
	void Render();

private:
	void InitEnvironment();
	void SetBase();
	void PopulateGoldMine(GridLocation aMiddlePoint);
	void PopulateStoneMine(GridLocation aMiddlePoint);
	void SpawnMine(GridLocation aLocation, EntityType aMineType);
	void PopulateForrest(GridLocation aMiddlePoint);

	void SpawnGoblin();
	void SpawnSheep();

	Tga::Sprite2DInstanceData myTGELogoInstance = {};
	Tga::SpriteSharedData mySharedData = {};

	std::shared_ptr<HUD> myHUD;

	std::shared_ptr<Grid> myGrid;

	/*Environment*/
	std::shared_ptr<Base> myBase;

	float myTimeToNextGoblin = 5.0f;
	int myMaxGoblins = 5;
	int myNumOfGoblins = 0;
	int myBridgeIterator = 0;

};