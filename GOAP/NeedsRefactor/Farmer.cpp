#include "stdafx.h"

#include "Grid.h"
#include "Entity.h"
#include "Farmer.h"
#include "FarmerBehavior.h"

#include "AStar.h"

#include "MainSingleton.h"

#include <iostream>
#include <unordered_set>

#include <tge\engine.h>
#include <tge\graphics\GraphicsEngine.h>
#include <tge\drawers\DebugDrawer.h>
#include <tge/texture/TextureManager.h>
#include <tge/drawers/SpriteDrawer.h>


Farmer::Farmer()
{
    myEntityType = EntityType::Farmer;
    myCurrentAction = eFarmerAction::Idle;
    myCurrentTarget = eFarmerTarget::None;
    myCurrentObjective = eFarmerObjective::None;

    auto& textureManager = Tga::Engine::GetInstance()->GetTextureManager();
    myCarryingTexture.myTexture = textureManager.GetTexture(L"Sprites/Factions/Knights/Troops/Pawn/Purple/Pawn_Purple_Carry.png");
    myInventoryData.mySize = MainSingleton::GetInstance().GetTileSize() * 1.5f, MainSingleton::GetInstance().GetTileSize() * 1.5f;
    myInventoryData.myPivot = { 0.5f, 1.0f };

    InitializeFarmerBehavior();
}


void Farmer::InitializeFarmerBehavior()
{
    auto moveToTarget = std::make_shared<MoveToTargetLeaf>(this);
    auto harvestResource = std::make_shared<HarvestResourceLeaf>(this);
    auto deliverResource = std::make_shared<DeliverResourceLeaf>(this);
    auto idle = std::make_shared<IdleLeaf>(this);

    auto gatherSequence = std::make_shared<BT::Sequence>();
    gatherSequence->addChild(moveToTarget);
    gatherSequence->addChild(harvestResource);
    gatherSequence->addChild(deliverResource);

    auto rootSelector = std::make_shared<BT::Selector>();
    rootSelector->addChild(gatherSequence);
    rootSelector->addChild(idle);

    myBehaviorTree = std::make_shared<BT::BehaviorTree>(rootSelector);

    float size = mySpriteData.mySize.x;
    size *= MainSingleton::GetInstance().GetTileSize();
    mySteeringParams.arrivalRadius = size;
    mySteeringParams.separationRadius = size;
    mySteeringParams.maxSpeed = 3.0f;
    mySteeringParams.currentSeekWeight = 20.f;
}


void Farmer::Update(float aDeltaTime)
{
    if (myHealth <= 0)
    {
        if (myAliveLastFrame)
        {
            myHasItem = false;
            myAliveLastFrame = false;

            MainSingleton::GetInstance().GetBase()->RemoveFarmer();
        }
        return;
    }

    auto grid = MainSingleton::GetInstance().GetGrid();
    myGridPosition = grid->GetTileFromPosition(mySpriteData.myPosition);

    //FollowPath();
    EvaluateObjective();
    FollowWander();

    float distanceToTarget = 10000.0f;

    if (!myTargetPositions.empty())
    {
        distanceToTarget = MATH::CalculateDistance(myTargetPositions.front().ToPosition(MainSingleton::GetInstance().GetTileSize()), mySpriteData.myPosition);
    }

    if (distanceToTarget < (mySpriteData.mySize.x /** 4.f*/))
    {
        mySteeringParams.currentTarget = myTargetPositions.front().ToPosition(MainSingleton::GetInstance().GetTileSize());
    }
    else if (myPathIndex < myPath.size())
    {
        mySteeringParams.currentTarget = myPath[myPathIndex].ToPosition(MainSingleton::GetInstance().GetTileSize());
    }
    else
    {
        mySteeringParams.currentTarget = (MainSingleton::GetInstance().GetBase()->GetClosestInteractLocation(myGridPosition)).ToPosition(MainSingleton::GetInstance().GetTileSize());
    }

    mySteeringParams.currentPosition = mySpriteData.myPosition;
    mySteering.SetSteeringParameters(mySteeringParams);
    mySpriteData.myPosition += aDeltaTime * mySteering.UpdateSteering();
    myInventoryData.myPosition = mySpriteData.myPosition;
}

void Farmer::Render(Tga::SpriteDrawer& aSpriteDrawer)
{
    if (myHealth >= myMaxHealth && myHasItem)
    {
        aSpriteDrawer.Draw(myCarryingTexture, mySpriteData);
        aSpriteDrawer.Draw(myInventoryTexture, myInventoryData);
    }
    else if (myHealth >= myMaxHealth)
    {
        aSpriteDrawer.Draw(myTexture, mySpriteData);
    }
    else
    {
        aSpriteDrawer.Draw(myDepletedTexture, mySpriteData);
    }
}

void Farmer::SetTargetPositions()
{
    UpdateWander();

    if (!myPath.empty())
    {
        myCurrentAction = eFarmerAction::OnRoute;
    }

    /*auto grid = MainSingleton::GetInstance().GetGrid(); USES PATH FINDING FULLY TO TARGET

    myPath = PathFinding::GetPathFromAStar(myGridPosition, aLocation, *grid);
    myPathIndex = 0;

    if (!myPath.empty())
    {
        myCurrentAction = eFarmerAction::OnRoute;
    }
    else
    {
        std::cout << "Path to objective empty" << std::endl;
    }*/
}

void Farmer::GiveObjective(eActionType anObjective)
{
    HUDMessage msg;

    switch (anObjective)
    {
    case eActionType::GatherGold:
        {
        myCurrentTarget = eFarmerTarget::Resource;
        myCurrentObjective = eFarmerObjective::Gold;
        msg.text = "Gather Gold";
        break;
        }
    case  eActionType::GatherStone:
        {
        myCurrentTarget = eFarmerTarget::Resource;
        myCurrentObjective = eFarmerObjective::Stone;
        msg.text = "Gather Stone";
        break;
        }
    case  eActionType::GatherWood:
        {
        myCurrentTarget = eFarmerTarget::Resource;
        myCurrentObjective = eFarmerObjective::Wood;
        msg.text = "Gather Wood";
        break;
        }
    case  eActionType::GatherFood:
        {
        msg.text = "Gather Food";
        myCurrentTarget = eFarmerTarget::Resource;
        myCurrentObjective = eFarmerObjective::Food;
        break;
        }
    case  eActionType::BuildHouse:
        {
        msg.text = "Build House";
        myCurrentTarget = eFarmerTarget::ConstructionSite;
        myCurrentObjective = eFarmerObjective::Building;
        break;
        }
    case  eActionType::Count: // Fall through to default
    default:
        myCurrentAction = eFarmerAction::Idle;
        myCurrentTarget = eFarmerTarget::None;
        myCurrentObjective = eFarmerObjective::None;
        break;

    }

    if (msg.text != "")
    {
        auto hud = MainSingleton::GetInstance().GetHUD();
        hud->AddMessage(msg);
    }

    if (myCurrentTarget == eFarmerTarget::ConstructionSite) BuildHouse();
    if (myCurrentTarget == eFarmerTarget::Resource)
    {
        ChooseClosestTarget(myCurrentObjective);
    }
}

bool Farmer::IsIdle()
{
    if (myCurrentAction == eFarmerAction::Idle)
    {
        return true;
    }
    return false;
}

void Farmer::EvaluateObjective()
{
    if (myArrivedAtObjective)
    {
        if (myCurrentTarget == eFarmerTarget::Resource)
        {
            Harvest();

            myCurrentAction = eFarmerAction::ReturningToBase;
            myCurrentTarget = eFarmerTarget::Base;
            myTargetEntity = MainSingleton::GetInstance().GetBase();
            if (myTargetEntity)
            {
                myTargetEntity->GetClosestInteractLocations(myGridPosition, myTargetPositions);
                SetTargetPositions();
            }
            myArrivedAtObjective = false;
            myPathIndex = 0;
        }
        else if (myCurrentTarget == eFarmerTarget::Base)
        {
            DeliverToBase();
            myArrivedAtObjective = false;
            myPath.clear();
            myPathIndex = 0;
        }
        else if (myCurrentTarget == eFarmerTarget::ConstructionSite)
        {
            myTargetEntity->SetHealth(myTargetEntity->GetHealth() + 1);
            myArrivedAtObjective = false;
            myPath.clear();
            MainSingleton::GetInstance().GetBase()->GiveResource(myCurrentObjective);
            myPathIndex = 0;
            myCurrentAction = eFarmerAction::Idle;
            myCurrentTarget = eFarmerTarget::None;
            myCurrentObjective = eFarmerObjective::None;
        }
    }
}

std::shared_ptr<Entity> Farmer::GetClosestEntity(std::vector<std::shared_ptr<Entity>> someEntities)
{
    std::shared_ptr<Entity> closestEntity;
    float closestDistance = std::numeric_limits<float>::infinity();

    for (auto e : someEntities)
    {
        float distanceToTarget = MATH::CalculateDistance(e->GetSpriteData().myPosition, mySpriteData.myPosition);
        
        if (distanceToTarget < closestDistance && e->GetHealth() > 0)
        {
            closestDistance = distanceToTarget;
            closestEntity = e;
        }
    }

    return closestEntity;
}

void Farmer::DebugDrawPath()
{
    if (myPath.empty()) return;
    
    auto& debugDrawer = Tga::Engine::GetInstance()->GetDebugDrawer();
    float tileSize = MainSingleton::GetInstance().GetTileSize();

    for (int i = 0; i < myPath.size() - 1; i++)
    {
        debugDrawer.DrawLine(myPath[i].ToPosition(tileSize), myPath[i + 1].ToPosition(tileSize), {1.f,0.f,0.f,1.f});
    }
}

void Farmer::FollowPath()
{
    if (myCurrentAction == eFarmerAction::OnRoute)
    {
        if (myTargetEntity)
        {
            if (myTargetEntity->GetHealth() <= 0)
            {
                ChooseClosestTarget(myCurrentObjective);
            }
        }
    }

    if (myPath.empty()) return;

    if (myPathIndex == myPath.size())
    {
        myArrivedAtObjective = true;
    }
    else if (myPathIndex < myPath.size())
    {
        float distance = MATH::CalculateDistance(myPath[myPathIndex].ToPosition(MainSingleton::GetInstance().GetTileSize()), mySpriteData.myPosition);

        if (distance < (mySpriteData.mySize.x/2.f))
        {
            myPathIndex++;
        }
    }
    return;
}

void Farmer::FollowWander()
{
    if (myCurrentAction == eFarmerAction::OnRoute)
    {
        if (myTargetEntity)
        {
            if (myTargetEntity->GetHealth() <= 0)
            {
                ChooseClosestTarget(myCurrentObjective);
            }

        }
        else
        {
            myTargetEntity = MainSingleton::GetInstance().GetBase();
            if (myTargetEntity)
            {
                myTargetEntity->GetClosestInteractLocations(myGridPosition, myTargetPositions);
                SetTargetPositions();
                myCurrentAction = eFarmerAction::Idle;
                myCurrentTarget = eFarmerTarget::None;
                myCurrentObjective = eFarmerObjective::None;
            }
        }
    }

    if (myPath.empty() && myCurrentAction == eFarmerAction::OnRoute) UpdateWander();
    if (myPath.empty() || myPathIndex >= myPath.size()) return;
    if (myTargetPositions.empty()) return;


    std::sort(myTargetPositions.begin(), myTargetPositions.end(),
        [this](const GridLocation& a, const GridLocation& b)
        {
            int distA = std::abs(a.x - myGridPosition.x) + std::abs(a.y - myGridPosition.y);
            int distB = std::abs(b.x - myGridPosition.x) + std::abs(b.y - myGridPosition.y);
            return distA < distB;
        });

    float distance = MATH::CalculateDistance(myPath[myPathIndex].ToPosition(MainSingleton::GetInstance().GetTileSize()), mySpriteData.myPosition);
    float distanceToTarget = MATH::CalculateDistance(myTargetPositions.front().ToPosition(MainSingleton::GetInstance().GetTileSize()), mySpriteData.myPosition);

    if (distanceToTarget < (mySpriteData.mySize.x / 1.5f))
    {
        if (myCurrentAction != eFarmerAction::Idle)
        {
            myArrivedAtObjective = true;
            myRecentlyVisited.clear();
        }
    }
    else if (distance < (mySpriteData.mySize.x / 4.f))
    {
        myPathIndex++;
        if (myPathIndex >= myPath.size() && myCurrentAction == eFarmerAction::OnRoute)
        {
            UpdateWander();
        }
    }
    return;
}

void Farmer::UpdateWander() 
{
    auto grid = MainSingleton::GetInstance().GetGrid();

    /*std::vector<GridLocation> possibleMoves =
    {
        {myGridPosition.x + 1, myGridPosition.y},
        {myGridPosition.x - 1, myGridPosition.y},
        {myGridPosition.x, myGridPosition.y + 1},
        {myGridPosition.x, myGridPosition.y - 1}
    };*/

    std::vector<GridLocation> possibleMoves =
    {
        {myGridPosition.x + 1, myGridPosition.y},
        {myGridPosition.x - 1, myGridPosition.y},
        {myGridPosition.x + 1, myGridPosition.y - 1},
        {myGridPosition.x - 1, myGridPosition.y + 1},
        {myGridPosition.x, myGridPosition.y + 1},
        {myGridPosition.x, myGridPosition.y - 1}
    };

    // Filter out walkable moves that are not recently visited
    std::vector<GridLocation> validMoves;
    for (const auto& move : possibleMoves)
    {
        if (grid->IsTileWalkable(move) &&
            std::find(myRecentlyVisited.begin(), myRecentlyVisited.end(), move) == myRecentlyVisited.end())
        {
            validMoves.push_back(move);
        }
    }

    GridLocation nextMove;
    if (!validMoves.empty())
    {
        bool allSafe = std::all_of(validMoves.begin(), validMoves.end(), [&](const GridLocation& loc) 
            {
            return grid->GetTile(loc)->enemyInfluence == 0.0f;
            });

        if (allSafe && myTargetEntity != nullptr)
        {
            // All moves are safe (0 enemy influence): choose the one closest to the target entity.
            std::sort(validMoves.begin(), validMoves.end(), [&](const GridLocation& a, const GridLocation& b) 
                {
                return GridLocation::ManhattanDistance(a, myTargetEntity->GetGridPosition()) <
                    GridLocation::ManhattanDistance(b, myTargetEntity->GetGridPosition());
                });
            nextMove = validMoves.front();
        }
        else
        {
            // Otherwise, sort by enemy influence (ascending).
            std::sort(validMoves.begin(), validMoves.end(), [&](const GridLocation& a, const GridLocation& b) 
                {
                return grid->GetTile(a)->enemyInfluence < grid->GetTile(b)->enemyInfluence;
                });
            nextMove = validMoves.front();
        }
    }
    else
    {
        if (!myRecentlyVisited.empty())
        {
            nextMove = myRecentlyVisited.back();
        }
        else
        {
            nextMove = grid->FindRandomWalkableTile();
        }
    }

    myRecentlyVisited.push_back(nextMove);
    if (myRecentlyVisited.size() > myMaxRecentTiles)
    {
        myRecentlyVisited.pop_front();
    }

    myPath = { nextMove };
    myPathIndex = 0;
}

void Farmer::Harvest()
{
    int targetHealth = 0;
    if (myTargetEntity) targetHealth = myTargetEntity->GetHealth();
    if (targetHealth > 0)
    {
        myTargetEntity->SetHealth(targetHealth - 1);
        switch (myCurrentObjective)
        {
            case eFarmerObjective::Gold:
            {
                myInventoryTexture = MainSingleton::GetInstance().GetResourceTexture(EntityType::GoldMine);
                myHasItem = true;
                break;
            }
            case eFarmerObjective::Stone:
            {
                myInventoryTexture = MainSingleton::GetInstance().GetResourceTexture(EntityType::StoneMine);
                myHasItem = true;
                break;
            }
            case eFarmerObjective::Wood:
            {
                myInventoryTexture = MainSingleton::GetInstance().GetResourceTexture(EntityType::Tree);
                myHasItem = true;
                break;
            }
            case eFarmerObjective::Food:
            {
                myInventoryTexture = MainSingleton::GetInstance().GetResourceTexture(EntityType::Sheep);
                myHasItem = true;
                break;
            }
            default:
                break;
        }
    }
    else
    {
        ChooseClosestTarget(myCurrentObjective);
    }

}

void Farmer::DeliverToBase()
{
    myHasItem = false;

    MainSingleton::GetInstance().GetBase()->GiveResource(myCurrentObjective);
    myTargetEntity = nullptr;
    // Set farmer back to idle
    myCurrentObjective = eFarmerObjective::None;
    myCurrentAction = eFarmerAction::Idle;
    myCurrentTarget = eFarmerTarget::None;
}

void Farmer::BuildHouse()
{
    auto& singleton = MainSingleton::GetInstance();
    int gridWidth = singleton.GetWidth();
    int gridHeight = singleton.GetHeight();
    auto grid = singleton.GetGrid();

    // Use the farmer's current position as the starting point.
    // (Uncomment the following line if not already in use.)
    GridLocation farmerPos = grid->GetTileFromPosition(mySpriteData.myPosition);

    // Helper lambda to check if a 2x2 block starting at (x, y) is valid.
    auto isValidBlock = [&](int x, int y) -> bool {
        // Ensure the block is completely within the grid.
        if (x < 0 || y < 0 || x + 1 >= gridWidth || y + 1 >= gridHeight)
            return false;

        // Check that all 4 cells of the 2x2 block are walkable.
        if (!grid->IsTileWalkable({ x, y }) ||
            !grid->IsTileWalkable({ x + 1, y }) ||
            !grid->IsTileWalkable({ x, y + 1 }) ||
            !grid->IsTileWalkable({ x + 2, y - 1 }) ||
            !grid->IsTileWalkable({ x - 1, y - 1 }) ||
            !grid->IsTileWalkable({ x - 1, y + 2 }) ||
            !grid->IsTileWalkable({ x +2, y + 2 }) ||
            !grid->IsTileWalkable({ x + 1, y + 1 }))
            return false;

        // Define the neighboring cells that border the block.
        // For a 2x2 block, these are the two cells directly above, below,
        // to the left, and to the right.
        std::vector<GridLocation> neighbors = 
        {
            { x,     y - 1 }, { x + 1, y - 1 }, // Top neighbors
            { x,     y + 2 }, { x + 1, y + 2 }, // Bottom neighbors
            { x - 1, y     }, { x - 1, y + 1 }, // Left neighbors
            { x + 2, y     }, { x + 2, y + 1 }  // Right neighbors
        };

        // Ensure each neighbor exists (is in bounds) and is walkable.
        for (const auto& n : neighbors)
        {
            if (n.x < 0 || n.y < 0 || n.x >= gridWidth || n.y >= gridHeight)
                return false;
            if (!grid->IsTileWalkable(n))
                return false;
        }
        return true;
        };

    // Search outward from the farmer's current grid position.
    bool found = false;
    GridLocation validBlockPos({ -1,-1 });
    int maxRadius = std::max(gridWidth, gridHeight);
    for (int radius = 0; radius < maxRadius && !found; ++radius)
    {
        // Iterate over a square area centered on the farmer's position.
        for (int x = farmerPos.x - radius; x <= farmerPos.x + radius && !found; ++x)
        {
            for (int y = farmerPos.y - radius; y <= farmerPos.y + radius; ++y)
            {
                if (isValidBlock(x, y))
                {
                    validBlockPos = { x, y };
                    found = true;
                    break;
                }
            }
        }
    }

    if (found)
    {
        std::shared_ptr<Entity> houseEntity = Entity::CreateEntity(EntityType::House);
        // Set the house's grid position to the top-left of the 2x2 block.
        houseEntity->SetGridPosition(validBlockPos, false);
        singleton.AddEntity(houseEntity);

        // Mark the 2x2 block as non-walkable.
        grid->SetTileWalkable(validBlockPos, false);
        grid->SetTileWalkable({ validBlockPos.x + 1, validBlockPos.y }, false);
        grid->SetTileWalkable({ validBlockPos.x, validBlockPos.y + 1 }, false);
        grid->SetTileWalkable({ validBlockPos.x + 1, validBlockPos.y + 1 }, false);

        myTargetEntity = houseEntity;
        if (myTargetEntity)
        {
            myTargetEntity->GetClosestInteractLocations(myGridPosition, myTargetPositions);
            SetTargetPositions();
        }
        myCurrentObjective = eFarmerObjective::Building;
    }
    else
    {
        // Handle the case when no valid placement is found.
        myCurrentAction = eFarmerAction::Idle;
        myCurrentTarget = eFarmerTarget::None;
        myCurrentObjective = eFarmerObjective::None;
        std::cout << "No Place for building found" << std::endl;
    }
}

void Farmer::FindAndSetConstruction()
{
    auto& singleton = MainSingleton::GetInstance();
    int gridWidth = singleton.GetWidth();
    int gridHeight = singleton.GetHeight();
    auto grid = singleton.GetGrid();

    // Use the farmer's current position as the starting point.
    // (Uncomment the following line if not already in use.)
    GridLocation farmerPos = grid->GetTileFromPosition(mySpriteData.myPosition);

    // Helper lambda to check if a 2x2 block starting at (x, y) is valid.
    auto isValidBlock = [&](int x, int y) -> bool {
        // Ensure the block is completely within the grid.
        if (x < 0 || y < 0 || x + 1 >= gridWidth || y + 1 >= gridHeight)
            return false;

        // Check that all 4 cells of the 2x2 block are walkable.
        if (!grid->IsTileWalkable({ x, y }) ||
            !grid->IsTileWalkable({ x + 1, y }) ||
            !grid->IsTileWalkable({ x, y + 1 }) ||
            !grid->IsTileWalkable({ x + 1, y + 1 }))
            return false;

        // Define the neighboring cells that border the block.
        // For a 2x2 block, these are the two cells directly above, below,
        // to the left, and to the right.
        std::vector<GridLocation> neighbors = {
            { x,     y - 1 }, { x + 1, y - 1 }, // Top neighbors
            { x,     y + 2 }, { x + 1, y + 2 }, // Bottom neighbors
            { x - 1, y     }, { x - 1, y + 1 }, // Left neighbors
            { x + 2, y     }, { x + 2, y + 1 }  // Right neighbors
        };

        // Ensure each neighbor exists (is in bounds) and is walkable.
        for (const auto& n : neighbors)
        {
            if (n.x < 0 || n.y < 0 || n.x >= gridWidth || n.y >= gridHeight)
                return false;
            if (!grid->IsTileWalkable(n))
                return false;
        }
        return true;
        };

    // Search outward from the farmer's current grid position.
    bool found = false;
    GridLocation validBlockPos({ -1,-1 });
    int maxRadius = std::max(gridWidth, gridHeight);
    for (int radius = 0; radius < maxRadius && !found; ++radius)
    {
        // Iterate over a square area centered on the farmer's position.
        for (int x = farmerPos.x - radius; x <= farmerPos.x + radius && !found; ++x)
        {
            for (int y = farmerPos.y - radius; y <= farmerPos.y + radius; ++y)
            {
                if (isValidBlock(x, y))
                {
                    validBlockPos = { x, y };
                    found = true;
                    break;
                }
            }
        }
    }

    if (found)
    {
        std::shared_ptr<Entity> houseEntity = Entity::CreateEntity(EntityType::House);
        // Set the house's grid position to the top-left of the 2x2 block.
        houseEntity->SetGridPosition(validBlockPos, false);
        singleton.AddEntity(houseEntity);

        myTargetEntity = houseEntity;
        // Mark the 2x2 block as non-walkable.
        grid->SetTileWalkable(validBlockPos, false);
        grid->SetTileWalkable({ validBlockPos.x + 1, validBlockPos.y }, false);
        grid->SetTileWalkable({ validBlockPos.x, validBlockPos.y + 1 }, false);
        grid->SetTileWalkable({ validBlockPos.x + 1, validBlockPos.y + 1 }, false);
    }
    else
    {
        // Optionally handle the case when no valid placement is found.
        // For example, you might log an error or set a different action.
    }

    myCurrentAction = eFarmerAction::Idle;
    myCurrentTarget = eFarmerTarget::None;
    myCurrentObjective = eFarmerObjective::None;
}

void Farmer::ChooseClosestTarget(eFarmerObjective anObjective)
{
    switch (anObjective)
    {
    case eFarmerObjective::Gold:
    {
        myTargetEntity = GetClosestEntity(MainSingleton::GetInstance().GetActiveGoldMines());
        if (myTargetEntity)
        {
            myTargetEntity->GetClosestInteractLocations(myGridPosition, myTargetPositions);
            SetTargetPositions();
        }
        break;
    }
    case eFarmerObjective::Stone:
    {        
        myTargetEntity = GetClosestEntity(MainSingleton::GetInstance().GetActiveStoneMines());
        if (myTargetEntity)
        {
            myTargetEntity->GetClosestInteractLocations(myGridPosition, myTargetPositions);
            SetTargetPositions();
        }
        break;
    }
    case eFarmerObjective::Wood:
    {
        myTargetEntity = GetClosestEntity(MainSingleton::GetInstance().GetActiveTrees());
        if (myTargetEntity)
        {
            myTargetEntity->GetClosestInteractLocations(myGridPosition, myTargetPositions);
            SetTargetPositions();
        }
        break;
    }
    case eFarmerObjective::Food:
    {
        myTargetEntity = GetClosestEntity(MainSingleton::GetInstance().GetActiveSheep());
        if (myTargetEntity)
        {
            myTargetEntity->GetClosestInteractLocations(myGridPosition, myTargetPositions);
            SetTargetPositions();
        }
        break;
    }
    case eFarmerObjective::Building:
    {
        BuildHouse();
        break;
    }
    default:
        break;
    }


}
