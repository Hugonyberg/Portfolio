#include "stdafx.h"

#include "Grid.h"
#include "Soldier.h"
#include "Entity.h"

#include "AStar.h"

#include "MainSingleton.h"

#include <iostream>
#include <unordered_set>

#include <tge\engine.h>
#include <tge\graphics\GraphicsEngine.h>
#include <tge\drawers\DebugDrawer.h>
#include <tge/texture/TextureManager.h>
#include <tge/drawers/SpriteDrawer.h>

Soldier::Soldier()
{
    InitializeSoldierBehavior();
}

void Soldier::InitializeSoldierBehavior()
{
    myEntityType = EntityType::Soldier;
    myCurrentAction = eSoldierAction::Idle;
    myCurrentTarget = eSoldierTarget::None;
    myCurrentObjective = eSoldierObjective::None;

    float size = mySpriteData.mySize.x;
    size *= MainSingleton::GetInstance().GetTileSize();
    mySteeringParams.arrivalRadius = size;
    mySteeringParams.separationRadius = size;
    mySteeringParams.maxSpeed = 4.5f;
    mySteeringParams.currentSeekWeight = 20.f;

    //GeneratePatrolPath(MainSingleton::GetInstance().GetBase()->GetClosestInteractLocation(myGridPosition), 20);
}

void Soldier::Update(float aDeltaTime)
{
    auto grid = MainSingleton::GetInstance().GetGrid();
    myGridPosition = grid->GetTileFromPosition(mySpriteData.myPosition);

    //UpdatePatrol();
    FollowPath();

    if (myPathIndex < myPath.size())
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
}

void Soldier::Render(Tga::SpriteDrawer& aSpriteDrawer)
{
    aSpriteDrawer.Draw(myTexture, mySpriteData);
}

void Soldier::SetTargetPosition(GridLocation aLocation)
{
    auto grid = MainSingleton::GetInstance().GetGrid();

    myPath = PathFinding::GetPathFromAStar(myGridPosition, aLocation, *grid);
    myPathIndex = 0;

    if (!myPath.empty())
    {
        myCurrentAction = eSoldierAction::OnRoute;
    }
    else
    {
        std::cout << "Path to objective empty" << std::endl;
    }
}

void Soldier::GiveObjective(eActionType anObjective)
{
    anObjective;
}

bool Soldier::IsIdle()
{
    if (myCurrentAction == eSoldierAction::Idle)
    {
        return true;
    }
    return false;
}

void Soldier::FollowPath()
{
    if (myPath.empty())
    {
        UpdateWander();
        return;
    }

    float distance = MATH::CalculateDistance(myPath[myPathIndex].ToPosition(MainSingleton::GetInstance().GetTileSize()), mySpriteData.myPosition);

    auto goblins = MainSingleton::GetInstance().GetActiveGoblins();
    for (auto goblin : goblins)
    {
        if (goblin->GetHealth() <= 0) continue;

        float distanceToGoblin = MATH::CalculateDistance(goblin->GetGridPosition().ToPosition(MainSingleton::GetInstance().GetTileSize()), mySpriteData.myPosition);
        if (distanceToGoblin < (mySpriteData.mySize.x / 2.f))
        {
            goblin->SetHealth(0);
            MainSingleton::GetInstance().SetNumberOfGoblins(MainSingleton::GetInstance().GetNumberOfGoblins() - 1);
        }
    }

    if (distance < (mySpriteData.mySize.x / 2.f))
    {
        myPathIndex++;

        if (myPathIndex >= myPath.size())
        {
            UpdateWander();
        }
    }
}

void Soldier::GeneratePatrolPath(GridLocation aCenter, int aPatrolRadius)
{
    auto grid = MainSingleton::GetInstance().GetGrid();
    myPatrolTargets.clear();
    myPatrolCenter = aCenter;
    myPatrolRadius = aPatrolRadius;

    // Define patrol bounds
    int minX = std::max(0, myPatrolCenter.x - myPatrolRadius);
    int maxX = std::min(MainSingleton::GetInstance().GetWidth() - 1, myPatrolCenter.x + myPatrolRadius);
    int minY = std::max(0, myPatrolCenter.y - myPatrolRadius);
    int maxY = std::min(MainSingleton::GetInstance().GetHeight() - 1, myPatrolCenter.y + myPatrolRadius);

    // Collect valid patrol points
    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            GridLocation loc = { x, y };
            if (grid->IsTileWalkable(loc))
            {
                myPatrolTargets.push_back(loc);
            }
        }
    }

    // Shuffle patrol order
    std::shuffle(myPatrolTargets.begin(), myPatrolTargets.end(), std::mt19937(std::random_device()()));

    // Set the first target
    if (!myPatrolTargets.empty())
    {
        myCurrentPatrolIndex = 0;
        myPath = PathFinding::GetPathFromAStar(myGridPosition, myPatrolTargets[myCurrentPatrolIndex], *grid);
    }
}

void Soldier::UpdatePatrol()
{
    auto grid = MainSingleton::GetInstance().GetGrid();

    if (myPath.empty()) // Reached destination
    {
        myCurrentPatrolIndex = (myCurrentPatrolIndex + 1) % myPatrolTargets.size(); // Cycle through patrol points
        myPath = PathFinding::GetPathFromAStar(myGridPosition, myPatrolTargets[myCurrentPatrolIndex], *grid);
        myPathIndex = 0;
    }
    else
    {
        FollowPath(); // Move step-by-step along A* path
    }
}

void Soldier::UpdateWander()
{
    auto grid = MainSingleton::GetInstance().GetGrid();

    std::vector<GridLocation> possibleMoves =
    {
        {myGridPosition.x + 1, myGridPosition.y},
        {myGridPosition.x - 1, myGridPosition.y},
        {myGridPosition.x + 1, myGridPosition.y - 1},
        {myGridPosition.x - 1, myGridPosition.y + 1},
        {myGridPosition.x, myGridPosition.y + 1},
        {myGridPosition.x, myGridPosition.y - 1}
    };

    std::vector<GridLocation> soldierPositions = MainSingleton::GetInstance().GetActiveSoldierOverlappingPositions(this);
    
    std::vector<GridLocation> validMoves;
    for (const auto& move : possibleMoves)
    {
        if (!grid->IsTileWalkable(move) ||
            std::find(myRecentlyVisited.begin(), myRecentlyVisited.end(), move) != myRecentlyVisited.end())
        {
            continue;
        }

        bool isTooClose = false;
        if (!soldierPositions.empty())
        {
            for (const auto& soldierPos : soldierPositions)
            {
                if (move != myGridPosition && GridLocation::ManhattanDistance(move, soldierPos) <= 1)
                {
                    isTooClose = true;
                    break;
                }
            }
        }

        if (!isTooClose)
        {
            validMoves.push_back(move);
        }
    }

    GridLocation nextMove;
    if (!validMoves.empty())
    {
        // Store moves with enemy influence and those without separately.
        std::vector<GridLocation> enemyMoves;
        std::vector<GridLocation> nonEnemyMoves;
        for (const auto& move : validMoves)
        {
            const GridTile* tile = grid->GetTile(move);
            if (tile->enemyInfluence > 0.0f)
            {
                enemyMoves.push_back(move);
            }
            else
            {
                nonEnemyMoves.push_back(move);
            }
        }

        if (!enemyMoves.empty())
        {
            // When enemies are present, sort descending by enemyInfluence.
            std::sort(enemyMoves.begin(), enemyMoves.end(), [&](const GridLocation& a, const GridLocation& b) 
                {
                    const GridTile* tileA = grid->GetTile(a);
                    const GridTile* tileB = grid->GetTile(b);
                    return tileA->enemyInfluence > tileB->enemyInfluence;
                });
            nextMove = enemyMoves.front();
        }
        else if (!nonEnemyMoves.empty())
        {
            // Filter out moves with zero farmer influence.
            std::vector<GridLocation> filteredNonEnemyMoves;
            for (const auto& move : nonEnemyMoves)
            {
                const GridTile* tile = grid->GetTile(move);
                if (tile->farmerInfluence > 0.0f)
                {
                    filteredNonEnemyMoves.push_back(move);
                }
            }

            if (!filteredNonEnemyMoves.empty())
            {
                std::sort(filteredNonEnemyMoves.begin(), filteredNonEnemyMoves.end(), [&](const GridLocation& a, const GridLocation& b) 
                    {
                        const GridTile* tileA = grid->GetTile(a);
                        const GridTile* tileB = grid->GetTile(b);

                        // Choose the one with lower farmer influence.
                        return tileA->farmerInfluence < tileB->farmerInfluence;
                    });
                nextMove = filteredNonEnemyMoves.front();
            }
            else
            {
                std::sort(nonEnemyMoves.begin(), nonEnemyMoves.end(), [&](const GridLocation& a, const GridLocation& b) 
                    {
                        return DistanceToClosestFarmer(a) < DistanceToClosestFarmer(b);
                    });
                if (MainSingleton::GetInstance().GetBase()->GetNumberOfFarmers() > 0)
                {
                    nextMove = nonEnemyMoves.front();
                }
                else
                {
                    nextMove = nonEnemyMoves[rand() % nonEnemyMoves.size()];
                }
            }
        }
    }
    else
    {
        // If no valid moves, backtrack using the last visited tile.
        if (!myRecentlyVisited.empty())
        {
            nextMove = myRecentlyVisited.back();
        }
        else
        {
            // Fallback: Random walkable tile.
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

void Soldier::ChooseClosestTarget(eSoldierObjective anObjective)
{
    anObjective;
}

void Soldier::DebugDrawPath()
{
    if (myPath.empty()) return;

    auto& debugDrawer = Tga::Engine::GetInstance()->GetDebugDrawer();
    float tileSize = MainSingleton::GetInstance().GetTileSize();

    for (int i = 0; i < myPath.size() - 1; i++)
    {
        debugDrawer.DrawLine(myPath[i].ToPosition(tileSize), myPath[i + 1].ToPosition(tileSize), { 1.f,0.f,0.f,1.f });
    }
}

void Soldier::EvaluateObjective()
{
}

std::shared_ptr<Entity> Soldier::GetClosestEntity(std::vector<std::shared_ptr<Entity>> someEntities)
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

int Soldier::DistanceToClosestFarmer(const GridLocation& aMove)
{
    // GetClosestFarmerLocation expects a non-const reference; adjust your signature if needed.
    GridLocation mutableMove = aMove;
    const GridLocation& closestFarmer = MainSingleton::GetInstance().GetClosestFarmerLocation(mutableMove);
    return GridLocation::ManhattanDistance(aMove, closestFarmer);
}
