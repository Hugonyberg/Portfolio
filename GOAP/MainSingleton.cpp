#include "stdafx.h"
#include "MainSingleton.h"
#include <tge/texture/TextureManager.h>



void MainSingleton::AddActiveEntity(std::shared_ptr<Entity> anEntity)
{
	switch (anEntity->GetEntityType())
	{
	case EntityType::Farmer:
	{
		std::shared_ptr<Farmer> farmerEntity = std::dynamic_pointer_cast<Farmer>(anEntity);
		myActiveActors.push_back(farmerEntity);
		myActiveFarmers.push_back(farmerEntity);
		break;
	}
	case EntityType::Soldier:
	{
		std::shared_ptr<Soldier> soldierEntity = std::dynamic_pointer_cast<Soldier>(anEntity);
		myActiveActors.push_back(soldierEntity);
		myActiveSoldiers.push_back(soldierEntity);
		break;
	}
	case EntityType::Goblin:
	{
		std::shared_ptr<Goblin> goblinEntity = std::dynamic_pointer_cast<Goblin>(anEntity);
		myActiveActors.push_back(goblinEntity);
		myActiveGoblins.push_back(goblinEntity);
		break;
	}
	case EntityType::Tree:
	{
		myActiveTrees.push_back(anEntity);
		break;
	}
	case EntityType::Sheep:
	{
		myActiveSheep.push_back(anEntity);
		break;
	}
	case EntityType::GoldMine:
	{
		myActiveGoldMines.push_back(anEntity);
		break;
	}
	case EntityType::StoneMine:
	{
		myActiveStoneMines.push_back(anEntity);
		break;
	}
	default:
		break;
	}
}

void MainSingleton::SetActiveSoldierOverlappingPositions()
{
	if (myActiveSoldiers.empty()) return;

	mySoldierOverlappingPositions.clear();
	for (auto& soldier : myActiveSoldiers)
	{
		GridLocation loc = soldier->GetGridPosition();

		for (int dx = -1; dx <= 1; dx++)
		{
			for (int dy = -1; dy <= 1; dy++)
			{
				GridLocation pos{ loc.x + dx, loc.y + dy };
				if (std::find(mySoldierOverlappingPositions.begin(), mySoldierOverlappingPositions.end(), pos) == mySoldierOverlappingPositions.end())
				{
					mySoldierOverlappingPositions.push_back(pos);
				}
			}
		}
	}
}

std::vector<GridLocation> MainSingleton::GetActiveSoldierOverlappingPositions(Soldier* aSoldier)
{
	std::vector<GridLocation> soldierOverlappingPositions;

	if (myActiveSoldiers.empty())
	{
		return soldierOverlappingPositions;
	}

	for (auto& soldier : myActiveSoldiers)
	{
		if (soldier.get() == aSoldier) 
			continue;

		GridLocation loc = soldier->GetGridPosition();

		for (int dx = -1; dx <= 1; dx++)
		{
			for (int dy = -1; dy <= 1; dy++)
			{
				GridLocation pos{ loc.x + dx, loc.y + dy };
				if (std::find(soldierOverlappingPositions.begin(), soldierOverlappingPositions.end(), pos) == soldierOverlappingPositions.end())
				{
					soldierOverlappingPositions.push_back(pos);
				}
			}
		}
	}

	return soldierOverlappingPositions;
}

const GridLocation MainSingleton::GetClosestFarmerLocation(GridLocation& aFromLocation) const
{
	GridLocation closest{ -1, -1 };
	int minDistance = std::numeric_limits<int>::max();

	for (const auto& farmer : myActiveFarmers)
	{
		if (farmer->GetHealth() <= 0) 
			continue;

		int distance = GridLocation::ManhattanDistance(aFromLocation, farmer->GetGridPosition());
		if (closest.x == -1 || closest.y == -1 || distance < minDistance)
		{
			minDistance = distance;
			closest = farmer->GetGridPosition();
		}
	}

	return closest;
}

void MainSingleton::InitResourceTextures()
{
	auto& textureManager = Tga::Engine::GetInstance()->GetTextureManager();

	myResourceTextures[0].myTexture = textureManager.GetTexture(L"Sprites/Resources/Resources/G_Idle_(NoShadow).png"); // Gold
	myResourceTextures[1].myTexture = textureManager.GetTexture(L"Sprites/Resources/Resources/M_Idle_(NoShadow).png"); // Food
	myResourceTextures[2].myTexture = textureManager.GetTexture(L"Sprites/Resources/Resources/W_Idle_(NoShadow).png"); // Wood
	myResourceTextures[3].myTexture = textureManager.GetTexture(L"Sprites/Deco/06.png"); // Stone
}

Tga::SpriteSharedData MainSingleton::GetResourceTexture(EntityType aType)
{
	switch (aType)
	{
		case EntityType::GoldMine:
		{
			return myResourceTextures[0];
			break;
		}
		case EntityType::StoneMine:
		{
			return myResourceTextures[3];
			break;
		}
		case EntityType::Tree:
		{
			return myResourceTextures[2];
			break;
		}
		case EntityType::Sheep:
		{
			return myResourceTextures[1];
			break;
		}
		default:
			return myResourceTextures[0];
			break;
	}
}
