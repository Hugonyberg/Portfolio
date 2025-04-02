#pragma once
#include <tge\sprite\sprite.h>
#include "GridLocation.h"

#include <random>

struct GridTile
{
	Tga::Sprite2DInstanceData spriteInstance;
	bool isWalkable;
	float cost = 1.0f;
	float enemyInfluence = 0.0;
	float guardInfluence = 0.0;
	float farmerInfluence = 0.0;
};

class Grid
{
public:
	Grid(int aWidth, int aHeight, float aTileSize);

	void GenerateGrid();
	void Render();

	void Update(float aDeltaTime);
	void UpdateFarmerInfluence();
	void UpdateSoldierInfluence();
	void UpdateGoblinInfluence();

	std::vector<GridLocation> Neighbors(GridLocation loc) const;
	std::vector<GridLocation> UnwalkableNeighbors(GridLocation aPos) const;
	float Cost(GridLocation from, GridLocation to) const;

	void SetTileWalkable(GridLocation aLoc, bool anIsWalkable);
	bool IsTileWalkable(GridLocation aLoc) const;

	GridLocation GetTileFromPosition(Tga::Vector2f aPosition);

	void AddEnemyInfluenceToGridTile(GridLocation aCenter, int aRadius, float aDecayRate);
	void AddGuardInfluenceToGridTile(GridLocation aCenter, int aRadius, float aDecayRate);
	void AddFarmerInfluenceToGridTile(GridLocation aCenter, int aRadius, float aDecayRate);

	GridLocation FindRandomWalkableTile() const;

	void TestAStarPathfinding();
	
	const GridTile* GetTile(const GridLocation& aLoc);
private:

	int myWidth;
	int myHeight;
	float myTileSize;

	int myMinX;
	int myMaxX;
	int myMinY;
	int myMaxY;

	int myInfluenceIterator = 0;

	Tga::SpriteSharedData myTileTexture = {};
	Tga::SpriteSharedData myWaterTexture = {};
	Tga::SpriteSharedData myBridgeTexture = {};
	std::vector<GridTile> myTiles;

	std::vector<Tga::Vector2f> myBridgePositions;

	float myTimeToReduceInfluence = 0.5f;
	bool myShouldDecreaseInfluence = false;

	GridTile* GetTile(int x, int y);
};