#include "stdafx.h"
#include "Grid.h"

#include <tge/engine.h>
#include <tge\graphics\GraphicsEngine.h>
#include <tge/texture/TextureManager.h>

#include <tge\drawers\SpriteDrawer.h>
#include <tge\drawers\LineDrawer.h>
#include <tge\drawers\DebugDrawer.h>
#include <tge\primitives\LinePrimitive.h>

#include <tge/math/Vector.h>
#include <tge/math/Color.h>

#include "AStar.h"

#include "MainSingleton.h"

Grid::Grid(int aScreenWidth, int aScreenHeight, float aTileSize)
{
    auto& engine = *Tga::Engine::GetInstance();
    myTileTexture.myTexture = engine.GetTextureManager().GetTexture(L"Sprites/Terrain/Ground/Resized_Tilemap_Flat.png");
    myWaterTexture.myTexture = engine.GetTextureManager().GetTexture(L"Sprites/Terrain/Water/Water.png");
    myBridgeTexture.myTexture = engine.GetTextureManager().GetTexture(L"Sprites/Terrain/Bridge/Bridge_Solo.png");
    
    myTileSize = aTileSize;
    myWidth = aScreenWidth / static_cast<int>(myTileSize);
    myHeight = aScreenHeight / static_cast<int>(myTileSize);
    
    myMinX = 1;
    myMaxX = myWidth - 2;
    myMinY = 2;
    myMaxY = myHeight - 2;

    // Left Bridge 1
    myBridgePositions.push_back({ (myMinX)*myTileSize, (myMaxY / 4) * myTileSize });
    // Left Bridge 2
    myBridgePositions.push_back({ (myMinX)*myTileSize, (myMaxY - myMaxY / 4) * myTileSize });
    // Right Bridge 1
    myBridgePositions.push_back({ (myMaxX + 1) * myTileSize, (myMaxY / 4) * myTileSize });
    // Right Bridge 2
    myBridgePositions.push_back({ (myMaxX + 1) * myTileSize, (myMaxY - myMaxY / 4) * myTileSize });
    
    MainSingleton::GetInstance().SetBridgePositions(myBridgePositions);

    GenerateGrid();
}

void Grid::GenerateGrid()
{
    myTiles.clear();
    for (int y = 0; y <= myHeight; ++y) 
    {
        for (int x = 0; x < myWidth; ++x) 
        {

            GridTile tile;
            tile.spriteInstance.myPosition = { x * myTileSize, y * myTileSize };
            tile.spriteInstance.mySize = { myTileSize, myTileSize };
            tile.spriteInstance.myPivot = { 0.0f, 1.0f };

            if (x <= myMinX || y <= myMinY || x >= myMaxX || y >= myMaxY)
            {
                tile.isWalkable = false;
                myTiles.push_back(tile); // Water Tile
                continue; // Skip first and last tiles in x and y
            }

            tile.isWalkable = true;

            if (x == myMinX + 1 || y == myMinY + 1 || x == myMaxX - 1 || y == myMaxY - 1)
            {
                // Edge logic
                if (x == myMinX + 1 && y == myMinY + 1)
                {
                    tile.spriteInstance.myUV = { 1.0f, 0.67f }; // Left Bottom Corner
                }
                else if (x == myMaxX - 1 && y == myMaxY - 1)
                {
                    tile.spriteInstance.myUV = { 0.67f, 0.0f }; // Right Top Corner
                }
                else if (x == myMinX + 1 && y == myMaxY - 1)
                {
                    tile.spriteInstance.myUV = { 1.0f, 0.0f }; // Left Top Corner
                }
                else if (x == myMaxX - 1 && y == myMinY + 1)
                {
                    tile.spriteInstance.myUV = { 0.67f, 0.67f }; // Right Bottom Corner
                }
                else if (x == myMinX + 1) // Left Edge
                {
                    tile.spriteInstance.myUV = { 0.0f, 0.33f }; 
                }
                else if (x == myMaxX - 1) // Right Edge
                {
                    tile.spriteInstance.myUV = { 0.67f, 0.33f }; // Why?
                }
                else if (y == myMinY + 1) // Bottom Edge
                {
                    tile.spriteInstance.myUV = { 0.33f, 0.67f }; // Why?
                }
                else if (y == myMaxY - 1) // Top Edge
                {
                    tile.spriteInstance.myUV = { 0.33f, 0.0f }; // Why?
                }
            }
            else 
            {
                tile.spriteInstance.myUV = { 0.333f, 0.333f }; // Main Grass Tile UV
            }

            tile.spriteInstance.myUVScale = { 0.333f, 0.333f }; // All myTiles use same scaling

            myTiles.push_back(tile);
        }
    }
}

void Grid::Render()
{
    auto& engine = *Tga::Engine::GetInstance();
    Tga::SpriteDrawer& spriteDrawer = engine.GetGraphicsEngine().GetSpriteDrawer();
    Tga::LineDrawer lineDrawer = engine.GetGraphicsEngine().GetLineDrawer();
    Tga::DebugDrawer& debugDrawer = engine.GetDebugDrawer();
    debugDrawer;

    float enemyInfluenceDecreaser = 0.5f;
    float guardInfluenceDecreaser = 3.0f;
    float farmerInfluenceDecreaser = 1.0f;
    float influenceThreshold = 0.1f;

    // 1 Batch render all ground tiles
    {
        Tga::SpriteBatchScope batchScope = spriteDrawer.BeginBatch(myTileTexture);
        for (auto& tile : myTiles)
        {
            if (tile.spriteInstance.myPosition.x / myTileSize <= myMinX ||
                tile.spriteInstance.myPosition.y / myTileSize <= myMinY ||
                tile.spriteInstance.myPosition.x / myTileSize >= myMaxX ||
                tile.spriteInstance.myPosition.y / myTileSize >= myMaxY)
            {
                continue; // Skip non-walkable tiles
            }

            float displayEnemyInfluence = (tile.enemyInfluence >= influenceThreshold) ? (1.0f + tile.enemyInfluence) : 1.0f;
            float displayGuardInfluence = (tile.guardInfluence >= influenceThreshold) ? (1.0f + tile.guardInfluence) : 1.0f;
            float displayFarmerInfluence = (tile.farmerInfluence >= influenceThreshold) ? (1.0f + tile.farmerInfluence) : 1.0f;

#ifndef _DEBUG
            displayEnemyInfluence =  1.0f;
            displayGuardInfluence =  1.0f;
            displayFarmerInfluence = 1.0f;
#endif

            tile.spriteInstance.myColor = Tga::Color(displayEnemyInfluence, displayFarmerInfluence, displayGuardInfluence, 1.0f);

            // Decrease the influence if below the threshold, clamping it to 0
            if (myShouldDecreaseInfluence)
            {
                if (tile.enemyInfluence < influenceThreshold)
                {
                    tile.enemyInfluence = 0.0f;
                }
                else
                {
                    tile.enemyInfluence -= enemyInfluenceDecreaser;
                }

                if (tile.guardInfluence < influenceThreshold)
                {
                    tile.guardInfluence = 0.0f;
                }
                else
                {
                    tile.guardInfluence -= guardInfluenceDecreaser;
                }

                if (tile.farmerInfluence < influenceThreshold)
                {
                    tile.farmerInfluence = 0.0f;
                }
                else
                {
                    tile.farmerInfluence -= farmerInfluenceDecreaser;
                }
            }

            batchScope.Draw(tile.spriteInstance);
        }
    } 

    // 2 Batch render all water tiles
    {
        Tga::SpriteBatchScope batchScope = spriteDrawer.BeginBatch(myWaterTexture);
        for (auto& tile : myTiles)
        {
            if (tile.spriteInstance.myPosition.x / myTileSize <= myMinX ||
                tile.spriteInstance.myPosition.y / myTileSize <= myMinY ||
                tile.spriteInstance.myPosition.x / myTileSize >= myMaxX ||
                tile.spriteInstance.myPosition.y / myTileSize >= myMaxY)
            {
                batchScope.Draw(tile.spriteInstance);
            }
        }
    } // Batch flushes here

    // 3 Batch render all bridges
    {
        Tga::SpriteBatchScope batchScope = spriteDrawer.BeginBatch(myBridgeTexture);
        Tga::Sprite2DInstanceData bridgeInstance;
        bridgeInstance.mySize = { myTileSize * 3, myTileSize };
        bridgeInstance.myPivot = { 0.5f, 0.25f };

        // Render bridges
        for (int i = 0; i < 4; i++)
        {
            bridgeInstance.myPosition = myBridgePositions[i];
            batchScope.Draw(bridgeInstance);
        }
    } // Batch flushes here

    // 4 Debug Draw Grid (if needed)
    /*for (int x = 0; x <= myWidth; ++x)
    {
        debugDrawer.DrawLine({ x * myTileSize, 0 }, { x * myTileSize, myHeight * myTileSize }, { 1.f, 1.0f, 1.0f, 1.f });

    }

    for (int y = 0; y <= myHeight; ++y)
    {
        debugDrawer.DrawLine({ 0, y * myTileSize }, { myWidth * myTileSize, y * myTileSize }, { 1.f, 1.0f, 1.0f, 1.f });
    }*/
}

void Grid::Update(float aDeltaTime)
{
    auto actors = MainSingleton::GetInstance().GetActiveActors();

    myTimeToReduceInfluence -= aDeltaTime;
    if (myTimeToReduceInfluence <= 0.0f)
    {
        myTimeToReduceInfluence = 1.0f;
        myShouldDecreaseInfluence = true;
    }
    else
    {
        myShouldDecreaseInfluence = false;
    }

    for (auto& actor : actors)
    {
        std::shared_ptr<Farmer> farmer = std::dynamic_pointer_cast<Farmer>(actor);
        std::shared_ptr<Soldier> soldier = std::dynamic_pointer_cast<Soldier>(actor);
        std::shared_ptr<Goblin> goblin = std::dynamic_pointer_cast<Goblin>(actor);
        if (farmer)
        {
            if (farmer->GetHealth() <= 0) continue;

            int fIRadius = 6;
            float fIDecayRate = 0.5f;
            float maxInfluence = static_cast<float>(fIRadius);
            int startX = std::max(0, farmer->GetGridPosition().x - fIRadius);
            int startY = std::max(0, farmer->GetGridPosition().y - fIRadius);
            int endX = std::min(myWidth - 1, farmer->GetGridPosition().x + fIRadius);
            int endY = std::min(myHeight - 1, farmer->GetGridPosition().y + fIRadius);

            // Update influence for tiles within the radius.
            for (int y = startY; y <= endY; ++y)
            {
                for (int x = startX; x <= endX; ++x)
                {
                    GridLocation currentTile = { x, y };
                    int dx = x - farmer->GetGridPosition().x;
                    int dy = y - farmer->GetGridPosition().y;
                    int squaredDistance = dx * dx + dy * dy;
                    int radiusSquared = fIRadius * fIRadius;

                    if (squaredDistance <= radiusSquared)
                    {
                        // Use sqrt to get the actual Euclidean distance for a linear decay.
                        float distance = std::sqrt(static_cast<float>(squaredDistance));
                        float influence = std::max(0.0f, maxInfluence - (distance * fIDecayRate));
                        myTiles[y * myWidth + x].farmerInfluence =
                            std::max(myTiles[y * myWidth + x].farmerInfluence, influence);
                    }
                    //GridLocation currentTile = { x, y };
                    //// Using Manhattan distance.
                    //int distance = std::abs(farmer->GetGridPosition().x - x) + std::abs(farmer->GetGridPosition().y - y);

                    //if (distance <= fIRadius)
                    //{
                    //    float influence = std::max(0.0f, maxInfluence - (distance * fIDecayRate));
                    //    // Instead of adding, we take the maximum influence from any farmer.
                    //    myTiles[y * myWidth + x].farmerInfluence = std::max(myTiles[y * myWidth + x].farmerInfluence, influence);
                    //}
                }
            }
        }
        else if (soldier)
        {
            int fIRadius = 2;
            float fIDecayRate = 0.5f;
            float maxInfluence = static_cast<float>(fIRadius*3);
            int startX = std::max(0, soldier->GetGridPosition().x - fIRadius);
            int startY = std::max(0, soldier->GetGridPosition().y - fIRadius);
            int endX = std::min(myWidth - 1, soldier->GetGridPosition().x + fIRadius);
            int endY = std::min(myHeight - 1, soldier->GetGridPosition().y + fIRadius);

            // Update influence for tiles within the radius.
            for (int y = startY; y <= endY; ++y)
            {
                for (int x = startX; x <= endX; ++x)
                {
                    GridLocation currentTile = { x, y };
                    // Using Manhattan distance.
                    int distance = std::abs(soldier->GetGridPosition().x - x) + std::abs(soldier->GetGridPosition().y - y);

                    if (distance <= fIRadius)
                    {
                        float influence = std::max(0.0f, maxInfluence - (distance * fIDecayRate));
                        // Instead of adding, we take the maximum influence from any soldier.
                        myTiles[y * myWidth + x].guardInfluence = std::max(myTiles[y * myWidth + x].guardInfluence, influence);
                    }
                }
            }
        }
        else if (goblin)
        {
            if (goblin->GetHealth() <= 0) continue;

            int fIRadius = 3;
            float fIDecayRate = 0.25f;
            float maxInfluence = static_cast<float>(fIRadius);
            int startX = std::max(0, goblin->GetGridPosition().x - fIRadius);
            int startY = std::max(0, goblin->GetGridPosition().y - fIRadius);
            int endX = std::min(myWidth - 1, goblin->GetGridPosition().x + fIRadius);
            int endY = std::min(myHeight - 1, goblin->GetGridPosition().y + fIRadius);

            // Update influence for tiles within the radius.
            for (int y = startY; y <= endY; ++y)
            {
                for (int x = startX; x <= endX; ++x)
                {
                    GridLocation currentTile = { x, y };
                    // Using Manhattan distance.
                    int distance = std::abs(goblin->GetGridPosition().x - x) + std::abs(goblin->GetGridPosition().y - y);

                    if (distance <= fIRadius)
                    {
                        float influence = std::max(0.0f, maxInfluence - (distance * fIDecayRate));
                        // Instead of adding, we take the maximum influence from any enemy.
                        myTiles[y * myWidth + x].enemyInfluence = std::max(myTiles[y * myWidth + x].enemyInfluence, influence);
                    }
                }
            }
        }
    }
}

void Grid::UpdateFarmerInfluence()
{
}

void Grid::UpdateSoldierInfluence()
{
}

void Grid::UpdateGoblinInfluence()
{
}

std::vector<GridLocation> Grid::Neighbors(GridLocation aPos) const
{
    std::vector<GridLocation> neighbors;
    std::vector<GridLocation> possibleMoves = {
        {aPos.x + 1, aPos.y}, {aPos.x - 1, aPos.y}, // Left and Right
        {aPos.x, aPos.y + 1}, {aPos.x, aPos.y - 1}  // Up and Down
    };

    for (const auto& move : possibleMoves)
    {
        if (move.x >= 0 && move.x < myWidth && move.y >= 0 && move.y < myHeight)
        {
            const GridTile& tile = myTiles[move.y * myWidth + move.x];
            if (tile.isWalkable)
            {
                neighbors.push_back(move);
            }
        }
    }

    return neighbors;
}

std::vector<GridLocation> Grid::UnwalkableNeighbors(GridLocation aPos) const
{
    std::vector<GridLocation> neighbors;
    std::vector<GridLocation> possibleMoves = {
        {aPos.x + 1, aPos.y}, {aPos.x - 1, aPos.y}, // Left and Right
        {aPos.x + 1, aPos.y + 1}, {aPos.x - 1, aPos.y + 1}, // UP: Left and Right 
        {aPos.x + 1, aPos.y - 1}, {aPos.x - 1, aPos.y - 1}, // DOWN: Left and Right
        {aPos.x, aPos.y + 1}, {aPos.x, aPos.y - 1}  // Up and Down
    };

    for (const auto& move : possibleMoves)
    {
        if (move.x >= 0 && move.x < myWidth && move.y >= 0 && move.y < myHeight)
        {
            const GridTile& tile = myTiles[move.y * myWidth + move.x];
            if (!tile.isWalkable)
            {
                neighbors.push_back(move);
            }
        }
    }

    return neighbors;
}

float Grid::Cost(GridLocation from, GridLocation to) const
{
    from; // Maybe never should be used
    if (to.x >= 0 && to.x < myWidth && to.y >= 0 && to.y < myHeight)
    {
        return myTiles[to.y * myWidth + to.x].cost;
    }
    return FLT_MAX; // Unreachable cost
}

void Grid::SetTileWalkable(GridLocation aLoc, bool anIsWalkable)
{
    // Ensure the location is within grid bounds
    if (aLoc.x >= 0 && aLoc.x < myWidth && aLoc.y >= 0 && aLoc.y < myHeight)
    {
        int index = aLoc.y * myWidth + aLoc.x; // Convert 2D to 1D index
        myTiles[index].isWalkable = anIsWalkable;
    }
}

bool Grid::IsTileWalkable(GridLocation aLoc) const
{
    // Ensure the location is within grid bounds
    if (aLoc.x >= 0 && aLoc.x < myWidth && aLoc.y >= 0 && aLoc.y < myHeight)
    {
        int index = aLoc.y * myWidth + aLoc.x; // Convert 2D to 1D index
        return myTiles[index].isWalkable;
    }

    return false;
}

GridLocation Grid::GetTileFromPosition(Tga::Vector2f aPosition)
{
    int x = static_cast<int>(aPosition.x / myTileSize);
    int y = static_cast<int>(aPosition.y / myTileSize);

    // Ensure the location is within grid bounds
    if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
    {
        return GridLocation{ x, y };
    }

    // Return an invalid location if out of bounds
    return GridLocation{ -1, -1 };
}

void Grid::AddEnemyInfluenceToGridTile(GridLocation aCenter, int aRadius, float aDecayRate)
{
    float maxEnemyInfluence = 1.f;
    int startX = std::max(0, aCenter.x - aRadius);
    int startY = std::max(0, aCenter.y - aRadius);
    int endX = std::min(myWidth - 1, aCenter.x + aRadius);
    int endY = std::min(myHeight - 1, aCenter.y + aRadius);

    for (int y = startY; y <= endY; ++y)
    {
        for (int x = startX; x <= endX; ++x)
        {
            GridLocation currentTile = { x, y };

            int distance = std::abs(aCenter.x - x) + std::abs(aCenter.y - y);

            if (distance <= aRadius) 
            {
                float enemyInfluenceToAdd = std::max(0.0f, maxEnemyInfluence - (distance * aDecayRate));
                // Adds enemyInfluence up to max
                myTiles[y * myWidth + x].enemyInfluence = std::clamp(myTiles[y * myWidth + x].enemyInfluence + enemyInfluenceToAdd, 0.0f, maxEnemyInfluence);
            }
        }
    }
}

void Grid::AddGuardInfluenceToGridTile(GridLocation aCenter, int aRadius, float aDecayRate)
{
    float maxGuardInfluence = 1.f;
    int startX = std::max(0, aCenter.x - aRadius);
    int startY = std::max(0, aCenter.y - aRadius);
    int endX = std::min(myWidth - 1, aCenter.x + aRadius);
    int endY = std::min(myHeight - 1, aCenter.y + aRadius);

    for (int y = startY; y <= endY; ++y)
    {
        for (int x = startX; x <= endX; ++x)
        {
            GridLocation currentTile = { x, y };

            int distance = std::abs(aCenter.x - x) + std::abs(aCenter.y - y);

            if (distance <= aRadius)
            {
                float guardInfluenceToAdd = std::max(0.0f, maxGuardInfluence - (distance * aDecayRate));
                // Adds enemyInfluence up to max
                myTiles[y * myWidth + x].guardInfluence = std::clamp(myTiles[y * myWidth + x].guardInfluence + guardInfluenceToAdd, 0.0f, maxGuardInfluence);
            }
        }
    }
}

void Grid::AddFarmerInfluenceToGridTile(GridLocation aCenter, int aRadius, float aDecayRate)
{
    float maxFarmerInfluence = static_cast<float>(aRadius);
    int startX = std::max(0, aCenter.x - aRadius);
    int startY = std::max(0, aCenter.y - aRadius);
    int endX = std::min(myWidth - 1, aCenter.x + aRadius);
    int endY = std::min(myHeight - 1, aCenter.y + aRadius);

    for (int y = startY; y <= endY; ++y)
    {
        for (int x = startX; x <= endX; ++x)
        {
            GridLocation currentTile = { x, y };

            int distance = std::abs(aCenter.x - x) + std::abs(aCenter.y - y);

            if (distance <= aRadius)
            {
                float farmerInfluenceToAdd = std::max(0.0f, maxFarmerInfluence - (distance * aDecayRate));
                // Adds enemyInfluence up to max
                myTiles[y * myWidth + x].farmerInfluence = std::clamp(myTiles[y * myWidth + x].farmerInfluence + farmerInfluenceToAdd, 0.0f, maxFarmerInfluence);
            }
        }
    }
}

GridLocation Grid::FindRandomWalkableTile() const
{
    GridLocation randomTile;
    do
    {
        randomTile.x = std::rand() % myWidth;
        randomTile.y = std::rand() % myHeight;
    } while (!IsTileWalkable(randomTile)); // Keep searching until a valid tile is found

    return randomTile;
}

void Grid::TestAStarPathfinding()
{    
    // Seed the random generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    GridLocation start, goal;

    // Find a random walkable start position
    do 
    {
        start.x = std::rand() % myWidth;
        start.y = std::rand() % myHeight;
    } while (!myTiles[start.y * myWidth + start.x].isWalkable);

    // Find a random walkable goal position
    do 
    {
        goal.x = std::rand() % myWidth;
        goal.y = std::rand() % myHeight;
    } while (!myTiles[goal.y * myWidth + goal.x].isWalkable || start == goal);

    std::unordered_map<GridLocation, GridLocation> came_from;
    std::unordered_map<GridLocation, double> cost_so_far;

    // Run A* pathfinding
    if (PathFinding::AStarSearch(*this, start, goal, came_from, cost_so_far))
    {
        std::vector<GridLocation> path = PathFinding::ReconstructPath(start, goal, came_from);

        // Change the color of the path tiles
        for (const auto& loc : path)
        {
            int index = loc.y * myWidth + loc.x;
            myTiles[index].spriteInstance.myColor = { 1.0f, 0.0f, 0.0f, 1.0f }; // Red color
        }
    }

    myTiles[start.y * myWidth + start.x].spriteInstance.myColor = { 0.0f, 1.0f, 0.0f, 1.0f }; // Green (Start)
    myTiles[goal.y * myWidth + goal.x].spriteInstance.myColor = { 0.0f, 0.0f, 1.0f, 1.0f }; // Blue (Goal)
}

const GridTile* Grid::GetTile(const GridLocation& aLoc)
{
    if (aLoc.x >= 0 && aLoc.x < myWidth && aLoc.y >= 0 && aLoc.y < myHeight)
    {
        int index = aLoc.y * myWidth + aLoc.x;
        return &myTiles[index];
    }
    return nullptr;
}

GridTile* Grid::GetTile(int x, int y)
{
    if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
    {
        int index = y * myWidth + x;
        return &myTiles[index]; 
    }
    return nullptr; 
}
