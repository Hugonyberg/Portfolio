#include "stdafx.h"

#include "Grid.h"
#include "Goblin.h"
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

Goblin::Goblin()
{
    float size = mySpriteData.mySize.x;
    size *= MainSingleton::GetInstance().GetTileSize();
    mySteeringParams.arrivalRadius = size;
    mySteeringParams.separationRadius = size;
    mySteeringParams.maxSpeed = 3.5f;
    mySteeringParams.currentSeekWeight = 20.f;
}

void Goblin::Update(float aDeltaTime)
{
    if (myHealth <= 0) return;

    auto grid = MainSingleton::GetInstance().GetGrid();
    myGridPosition = grid->GetTileFromPosition(mySpriteData.myPosition);

    if (myRecentlySpawned)
    {
        if (!grid->IsTileWalkable(myGridPosition))
        {
            if (mySpriteData.myPosition.x < 100.0f)
            {
                mySpriteData.myPosition += aDeltaTime * Tga::Vector2f{ 25.0f, 0.0f };
            }
            else
            {
                mySpriteData.myPosition += aDeltaTime * Tga::Vector2f{ -25.0f, 0.0f };
            }
            return;
        }
        myRecentlySpawned = false;
    }

    FollowPath();
    //grid->AddEnemyInfluenceToGridTile(myGridPosition, 2, 0.2f);

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

void Goblin::Render(Tga::SpriteDrawer& aSpriteDrawer)
{
    if (myHealth >= myMaxHealth)
    {
        aSpriteDrawer.Draw(myTexture, mySpriteData);
    }
    else
    {
        aSpriteDrawer.Draw(myDepletedTexture, mySpriteData);
    }
}

void Goblin::SetTargetPosition(GridLocation aLocation)
{
    aLocation;
}

void Goblin::FollowPath()
{
    if (myPath.empty())
    {
        UpdateWander();
        return;
    }

    float distance = MATH::CalculateDistance(myPath[myPathIndex].ToPosition(MainSingleton::GetInstance().GetTileSize()), mySpriteData.myPosition);

    auto farmers = MainSingleton::GetInstance().GetActiveFarmers();
    for (auto farmer : farmers)
    {
        if (farmer->GetHealth() <= 0) continue;

        float distanceToFarmer = MATH::CalculateDistance(farmer->GetGridPosition().ToPosition(MainSingleton::GetInstance().GetTileSize()), mySpriteData.myPosition);
        if (distanceToFarmer < (mySpriteData.mySize.x / 2.f))
        {
            farmer->SetHealth(0);
        }
    }

    if (distance < (mySpriteData.mySize.x / 2.f))
    {
        myPathIndex++;

        // If the goblin reaches the end of its current move, pick a new one
        if (myPathIndex >= myPath.size())
        {
            UpdateWander();
        }
    }
}

void Goblin::ChooseClosestTarget(eGoblinObjective anObjective)
{
    anObjective;
}

void Goblin::UpdateWander()
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

    std::vector<GridLocation> positiveMoves;          // composite > 0.
    std::vector<GridLocation> neutralInfluencedMoves;   // composite == 0, but nonzero influences.
    std::vector<GridLocation> completelyNeutralMoves;   // both influences are exactly 0.

    for (const auto& move : validMoves)
    {
        const GridTile* tile = grid->GetTile(move);
        float composite = tile->farmerInfluence - tile->guardInfluence;
        if (composite > 0.0f)
        {
            positiveMoves.push_back(move);
        }
        else if (composite == 0.0f)
        {
            if (tile->farmerInfluence == 0.0f && tile->guardInfluence == 0.0f)
            {
                completelyNeutralMoves.push_back(move);
            }
            else
            {
                neutralInfluencedMoves.push_back(move);
            }
        }
    }

    // Choose the next move based on the available groups.
    GridLocation nextMove;
    if (!positiveMoves.empty())
    {
        // Sort by composite score (farmerInfluence - guardInfluence) descending.
        std::sort(positiveMoves.begin(), positiveMoves.end(), [&](const GridLocation& a, const GridLocation& b)
            {
                const GridTile* tileA = grid->GetTile(a);
                const GridTile* tileB = grid->GetTile(b);
                float scoreA = tileA->farmerInfluence - tileA->guardInfluence;
                float scoreB = tileB->farmerInfluence - tileB->guardInfluence;
                return scoreA > scoreB;
            });
        nextMove = positiveMoves.front();
    }
    else if (!neutralInfluencedMoves.empty())
    {
        // Among neutral moves, choose the one with the highest farmer influence.
        std::sort(neutralInfluencedMoves.begin(), neutralInfluencedMoves.end(), [&](const GridLocation& a, const GridLocation& b)
            {
                const GridTile* tileA = grid->GetTile(a);
                const GridTile* tileB = grid->GetTile(b);
                return tileA->farmerInfluence > tileB->farmerInfluence;
            });
        nextMove = neutralInfluencedMoves.front();
    }
    else if (!completelyNeutralMoves.empty())
    {
        // If all moves are completely neutral, pick one randomly.
        nextMove = completelyNeutralMoves[rand() % completelyNeutralMoves.size()];
    }
    else
    {
        // Fallback, If no influence exists. It will wander randomly 
        if (validMoves.empty())
        {
            nextMove = myRecentlyVisited.back();
        }
        else
        {
            nextMove = validMoves[rand() % validMoves.size()];
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

void Goblin::DebugDrawPath()
{
    if (myPath.empty()) return;

    auto& debugDrawer = Tga::Engine::GetInstance()->GetDebugDrawer();
    float tileSize = MainSingleton::GetInstance().GetTileSize();

    for (int i = 0; i < myPath.size() - 1; i++)
    {
        debugDrawer.DrawLine(myPath[i].ToPosition(tileSize), myPath[i + 1].ToPosition(tileSize), { 1.f,0.f,0.f,1.f });
    }
}
