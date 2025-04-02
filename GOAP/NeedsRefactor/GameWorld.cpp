#include "stdafx.h"

#include <unordered_set>
#include <functional>
#include <random>

#include "GameWorld.h"
#include <tge/graphics/GraphicsEngine.h>
#include <tge/drawers/SpriteDrawer.h>
#include <tge/texture/TextureManager.h>
#include <tge/drawers/DebugDrawer.h>

#include <tge\input\InputManager.h>

#include "HUD.h"

#include "Grid.h"
#include "MainSingleton.h"

#include "Base.h"
#include "Farmer.h"

GameWorld::GameWorld()
{}

GameWorld::~GameWorld() 
{}

void GameWorld::Init()  
{
	auto& engine = *Tga::Engine::GetInstance();

	Tga::Vector2ui intResolution = engine.GetRenderSize();
	Tga::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };


	//float tileSize = resolution.y / 25.f;
	float tileSize = 25.f;
	int gridWidth = intResolution.x / static_cast<int>(tileSize);
	int gridHeight = intResolution.y / static_cast<int>(tileSize);
	
	MainSingleton::GetInstance().SetTileSize(tileSize);
	MainSingleton::GetInstance().SetWidth(gridWidth);
	MainSingleton::GetInstance().SetHeight(gridHeight);

	myGrid = std::make_shared<Grid>(Grid(intResolution.x, intResolution.y, tileSize));
	MainSingleton::GetInstance().SetGrid(myGrid);

	MainSingleton::GetInstance().InitResourceTextures();
	MainSingleton::GetInstance().SetMaxNumberOfGoblins(myMaxGoblins);

	myHUD = std::make_shared<HUD>(HUD());
	myHUD->Init();
	MainSingleton::GetInstance().SetHUD(myHUD);

	InitEnvironment();
}

void GameWorld::Update(float aDeltaTime)
{

	auto entities = MainSingleton::GetInstance().GetEntities();
	MainSingleton::GetInstance().SetActiveSoldierOverlappingPositions();
	//auto entitiesCopy = entities;
	for (auto& entity : entities)
	{
		if (!entity) continue;
		entity->Update(aDeltaTime);
	}

	if (myTimeToNextGoblin <= 0.0f && MainSingleton::GetInstance().GetNumberOfGoblins() < myMaxGoblins)
	{
		SpawnGoblin();
		MainSingleton::GetInstance().SetNumberOfGoblins(MainSingleton::GetInstance().GetNumberOfGoblins() + 1);
		myTimeToNextGoblin = 5.0f;
	}
	else if (myNumOfGoblins < myMaxGoblins)
	{
		myTimeToNextGoblin -= aDeltaTime;
	}

	myGrid->Update(aDeltaTime);
	myHUD->Update(aDeltaTime);
}

void GameWorld::Render()
{
	auto &engine = *Tga::Engine::GetInstance();
	Tga::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());

	myGrid->Render();

	auto& entities = MainSingleton::GetInstance().GetEntities();
	for (auto entity : entities)
	{
		entity->Render(spriteDrawer);

		std::shared_ptr<Farmer> e = std::dynamic_pointer_cast<Farmer>(entity);

		if (e)
		{
			//e->DebugDrawPath();
		}
	}

	myHUD->Render(spriteDrawer);
}

void GameWorld::InitEnvironment()
{
	int gridWidth = MainSingleton::GetInstance().GetWidth();
	int gridHeight = MainSingleton::GetInstance().GetHeight();
	
	SetBase();

	PopulateStoneMine({ gridWidth / 4, gridHeight / 4 });
	PopulateGoldMine({ gridWidth / 4, gridHeight - (gridHeight / 4) });
	PopulateStoneMine({ gridWidth - (gridWidth / 4), gridHeight - (gridHeight / 4) });
	PopulateGoldMine({ gridWidth - (gridWidth / 4), gridHeight / 4 });

	PopulateStoneMine({ gridWidth - (gridWidth / 4), gridHeight - (gridHeight / 3) });
	PopulateGoldMine({ gridWidth / 2, gridHeight - (gridHeight / 3) });

	PopulateForrest({ gridWidth / 4, gridHeight / 2 });
	PopulateForrest({ gridWidth - (gridWidth / 4), gridHeight / 2 });

	SpawnSheep();
}

void GameWorld::SetBase()
{
	int gridWidth = MainSingleton::GetInstance().GetWidth();
	int gridHeight = MainSingleton::GetInstance().GetHeight();

	std::shared_ptr<Entity> baseEntity = Entity::CreateEntity(EntityType::Base);
	MainSingleton::GetInstance().AddEntity(baseEntity);
	myBase = std::dynamic_pointer_cast<Base>(baseEntity);
	myBase->SetGridPosition({ (gridWidth / 2) - 2,(gridHeight / 2) - 1 }, false);
	for (auto gridCell : myBase->GetOverlappingCells())
	{
		myGrid->SetTileWalkable(gridCell, false);
	}

	MainSingleton::GetInstance().SetBase(myBase);

	myBase->InitBase();
}

void GameWorld::PopulateGoldMine(GridLocation aMiddlePoint)
{
	SpawnMine(aMiddlePoint, EntityType::GoldMine);
}

void GameWorld::PopulateStoneMine(GridLocation aMiddlePoint)
{
	SpawnMine(aMiddlePoint, EntityType::StoneMine);
}

void GameWorld::SpawnMine(GridLocation aLocation, EntityType aMineType)
{
	int gridWidth = MainSingleton::GetInstance().GetWidth();
	int gridHeight = MainSingleton::GetInstance().GetHeight();
	float tileSize = MainSingleton::GetInstance().GetTileSize();

	int iterationRange = 5; // Defines how far from middle it can spawn
	std::vector<GridLocation> possiblePositions;

	std::vector<std::shared_ptr<Entity>> exclusionEntities = MainSingleton::GetInstance().GetEntities();

	std::unordered_set<GridLocation> exclusionZones;

	// Calculate exclusion zones
	for (const auto& entity : exclusionEntities)
	{
		for (const auto& cell : entity->GetOverlappingCells())
		{
			int exclusionRadius;

			if (entity->GetSpriteData().mySize.x >= entity->GetSpriteData().mySize.y)
			{
				exclusionRadius = static_cast<int>(entity->GetSpriteData().mySize.x / tileSize); // Base exclusion on size X 
			}
			else
			{
				exclusionRadius = static_cast<int>(entity->GetSpriteData().mySize.y / tileSize); // Base exclusion on size Y
			}

			for (int x = -exclusionRadius; x <= exclusionRadius; x++)
			{
				for (int y = -exclusionRadius; y <= exclusionRadius; y++)
				{
					exclusionZones.insert({ cell.x + x, cell.y + y });
				}
			}
		}
	}

	// Generate valid positions within iteration range
	for (int x = -iterationRange; x <= iterationRange; x++)
	{
		for (int y = -iterationRange; y <= iterationRange; y++)
		{
			GridLocation pos = { aLocation.x + x, aLocation.y + y };

			if (pos.x >= 0 && pos.x < gridWidth && pos.y >= 0 && pos.y < gridHeight)
			{
				if (myGrid->IsTileWalkable(pos) && exclusionZones.find(pos) == exclusionZones.end())
				{
					possiblePositions.push_back(pos);
				}
			}
		}
	}

	// If no valid position found, exit early
	if (possiblePositions.empty())
	{
		std::cout << "No valid position found for " << (aMineType == EntityType::GoldMine ? "Gold Mine" : "Stone Mine") << std::endl;
		return;
	}

	// Choose a random position from the list
	std::shuffle(possiblePositions.begin(), possiblePositions.end(), std::mt19937(std::random_device()()));
	GridLocation selectedPos = possiblePositions.front();

	// Spawn the mine
	std::shared_ptr<Entity> mineEntity = Entity::CreateEntity(aMineType);
	mineEntity->SetGridPosition(selectedPos, false);
	MainSingleton::GetInstance().AddEntity(mineEntity);

	// Mark occupied tiles as non-walkable
	for (auto gridCell : mineEntity->GetOverlappingCells())
	{
		myGrid->SetTileWalkable(gridCell, false);
	}
	std::cout << (aMineType == EntityType::GoldMine ? "Gold Mine" : "Stone Mine") << " placed at (" << selectedPos.x << ", " << selectedPos.y << ")" << std::endl;
}

void GameWorld::PopulateForrest(GridLocation aMiddlePoint)
{

	int gridWidth = MainSingleton::GetInstance().GetWidth();
	int gridHeight = MainSingleton::GetInstance().GetHeight();
	float tileSize = MainSingleton::GetInstance().GetTileSize();

	size_t treeCount = 0;
	size_t maxTreeCount = gridWidth;

	std::vector<GridLocation> possiblePositions;
	GridLocation startPosition = aMiddlePoint;
	bool allowSpacing = true;
	int iterations = (gridWidth / 2) - 4;

	std::vector<std::shared_ptr<Entity>> exclusionEntities = MainSingleton::GetInstance().GetEntities();

	std::unordered_set<GridLocation> exclusionZones;

	// Calculate exclusion zones
	for (const auto& entity : exclusionEntities)
	{
		for (const auto& cell : entity->GetOverlappingCells())
		{
			int exclusionRadius;

			if (entity->GetSpriteData().mySize.x >= entity->GetSpriteData().mySize.y)
			{
				exclusionRadius = static_cast<int>(entity->GetSpriteData().mySize.x / tileSize); // Base exclusion on size X 
			}
			else
			{
				exclusionRadius = static_cast<int>(entity->GetSpriteData().mySize.y / tileSize); // Base exclusion on size Y
			}

			for (int x = -exclusionRadius; x <= exclusionRadius; x++)
			{
				for (int y = -exclusionRadius; y <= exclusionRadius; y++)
				{
					exclusionZones.insert({ cell.x + x, cell.y + y });
				}
			}
		}
	}

	int maxY = gridHeight - 2;
	int maxX = gridWidth - 2;

	// Exclude tiles around the bridges
	std::vector<GridLocation> bridgePositions = {
		{ maxX, maxY / 4 },                // Left Bridge 1
		{ maxX, maxY - maxY / 4 },       // Left Bridge 2
		{ maxX, maxY / 4 },                // Right Bridge 1
		{ maxX, maxY - maxY / 4 }        // Right Bridge 2
	};

	int exclusionRadius = 4; // 4 tiles around each bridge

	for (const auto& bridgePos : bridgePositions)
	{
		for (int x = -exclusionRadius; x <= exclusionRadius; x++)
		{
			for (int y = -exclusionRadius; y <= exclusionRadius; y++)
			{
				exclusionZones.insert({ bridgePos.x + x, bridgePos.y + y });
			}
		}
	}

	for (int x = -iterations; x <= iterations; x++) // Adjust the range for a larger/smaller cluster
	{
		for (int y = -iterations; y <= iterations; y++)
		{
			if (allowSpacing && (x % 2 == 0 && y % 2 == 0)) // Adds spacing if enabled
				continue;

			GridLocation pos = { startPosition.x + x, startPosition.y + y };
			if (pos.x >= 0 && pos.x < gridWidth && pos.y >= 0 && pos.y < gridHeight)
			{
				if (myGrid->IsTileWalkable(pos) && exclusionZones.find(pos) == exclusionZones.end()) // Ensure it's a walkable tile
				{
					possiblePositions.push_back(pos);
				}
			}
		}
	}

	std::shuffle(possiblePositions.begin(), possiblePositions.end(), std::mt19937(std::random_device()()));

	for (const auto& pos : possiblePositions)
	{
		if (treeCount >= maxTreeCount)
			break;

		if (myGrid->IsTileWalkable(pos) && myGrid->IsTileWalkable({ pos.x, pos.y + 1 })) // Ensure it's a walkable tile
		{
			std::shared_ptr<Entity> treeEntity = Entity::CreateEntity(EntityType::Tree);
			treeEntity->SetGridPosition(pos, false);
			MainSingleton::GetInstance().AddEntity(treeEntity);

			for (auto gridCell : treeEntity->GetOverlappingCells())
			{
				myGrid->SetTileWalkable(gridCell, false);
			}
		}
		else continue; 

		treeCount++;
	}

}

void GameWorld::SpawnGoblin()
{
	float tileSize = MainSingleton::GetInstance().GetTileSize();

	std::shared_ptr<Entity> goblin = Entity::CreateEntity(EntityType::Goblin);
	MainSingleton::GetInstance().AddEntity(goblin);
	std::shared_ptr<Goblin> goblinEntity = std::dynamic_pointer_cast<Goblin>(goblin);

	auto& bridgePositions = MainSingleton::GetInstance().GetBridgePositions();

	if (myBridgeIterator >= bridgePositions.size())
	{
		myBridgeIterator = 0;
	}

	GridLocation spawnPosition = { static_cast<int>(bridgePositions[myBridgeIterator].x / tileSize), static_cast<int>(bridgePositions[myBridgeIterator].y / tileSize) };
	myBridgeIterator++;

	spawnPosition.x -= 2;
	spawnPosition.y -= 1;
	goblinEntity->SetGridPosition(spawnPosition, true);
}

void GameWorld::SpawnSheep()
{

	int gridWidth = MainSingleton::GetInstance().GetWidth();
	int gridHeight = MainSingleton::GetInstance().GetHeight();

	size_t sheepCount = 0;
	size_t maxSheepCount = 30;

	std::vector<GridLocation> possiblePositions;
	GridLocation startPosition = { gridWidth / 2, gridHeight / 2 };
	bool allowSpacing = true;
	int iterations = gridHeight/3;

	std::vector<std::shared_ptr<Entity>> exclusionEntities = MainSingleton::GetInstance().GetEntities();

	// Calculate exclusion zones
	std::unordered_set<GridLocation> exclusionZones;
	for (const auto& entity : exclusionEntities)
	{
		for (const auto& cell : entity->GetOverlappingCells())
		{
			int exclusionRadius = 1;

			for (int x = -exclusionRadius; x <= exclusionRadius; x++)
			{
				for (int y = -exclusionRadius; y <= exclusionRadius; y++)
				{
					exclusionZones.insert({ cell.x + x, cell.y + y });
				}
			}
		}
	}

	// Generate possible positions around startPosition based on set iterations
	for (int x = -iterations; x <= iterations; x++) 
	{
		for (int y = -iterations; y <= iterations; y++)
		{
			if (allowSpacing && (x % 2 == 0 && y % 2 == 0)) 
				continue;
			if (allowSpacing && (x % 3 == 0 && y % 3 == 0)) 
				continue;
			if (allowSpacing && (x % 5 == 0 && y % 3 == 0)) 
				continue;

			GridLocation pos = { startPosition.x + x, startPosition.y + y };
			if (pos.x >= 0 && pos.x < gridWidth && pos.y >= 0 && pos.y < gridHeight)
			{
				if (myGrid->IsTileWalkable(pos) && exclusionZones.find(pos) == exclusionZones.end()) 
				{
					possiblePositions.push_back(pos);
				}
			}
		}
	}

	std::shuffle(possiblePositions.begin(), possiblePositions.end(), std::mt19937(std::random_device()()));

	for (const auto& pos : possiblePositions)
	{
		if (sheepCount >= maxSheepCount)
			break;

		if (myGrid->IsTileWalkable(pos)) 
		{
			std::shared_ptr<Entity> sheepEntity = Entity::CreateEntity(EntityType::Sheep);
			sheepEntity->SetGridPosition(pos, true);
			MainSingleton::GetInstance().AddEntity(sheepEntity);
		}
		else continue;

		sheepCount++;
	}

}

