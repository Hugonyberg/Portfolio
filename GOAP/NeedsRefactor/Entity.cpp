#include "stdafx.h"
#include "Entity.h"

#include <tge/engine.h>
#include <tge/texture/TextureManager.h>
#include <tge/drawers/SpriteDrawer.h>

#include "Base.h"
#include "MainSingleton.h"

Entity::Entity()
    : /*myGridCell(nullptr),*/ myEntityType(EntityType::Undefined) 
{
}

Entity::~Entity() 
{}

void Entity::Update(float aDeltaTime) 
{
    aDeltaTime;
    if (myHealth <= 0 && !myRecentlyDestroyed)
    {
        myRecentlyDestroyed = true;
        std::shared_ptr<Grid> grid = MainSingleton::GetInstance().GetGrid();
        for (auto& cell : GetOverlappingCells())
        {
            grid->SetTileWalkable(cell, true);
        }
    }
}

void Entity::Render(Tga::SpriteDrawer& aSpriteDrawer) 
{
    if (myHealth == myMaxHealth)
    {
        aSpriteDrawer.Draw(myTexture, mySpriteData);
    }
    else if (myHealth > 0)
    {
        aSpriteDrawer.Draw(myDecliningTexture, mySpriteData);
    }
    else
    {
        aSpriteDrawer.Draw(myDepletedTexture, mySpriteData);
    }
}

EntityType Entity::GetEntityType() const 
{
    return myEntityType;
}

void Entity::SetEntityType(EntityType aEntityType) 
{
    myEntityType = aEntityType;
}

void Entity::SetGridPosition(GridLocation aLocation, bool anIsCharacter)
{
    myGridPosition = aLocation;
    float tileSize = MainSingleton::GetInstance().GetTileSize();
    float worldX = aLocation.x * tileSize;
    float worldY = aLocation.y * tileSize;

    if (anIsCharacter)
    {
        mySpriteData.myPosition = { worldX + tileSize/2.f, worldY + tileSize };
    }
    else
    {
        mySpriteData.myPosition = { worldX, worldY };
    }
}

GridLocation Entity::GetGridPosition() const
{
    auto grid = MainSingleton::GetInstance().GetGrid();

    if (!grid->IsTileWalkable(myGridPosition))
    {
        for (auto t : grid->Neighbors(myGridPosition))
        {
            if (grid->IsTileWalkable(t))
            {
                return t;
            }
        }
    }
    else return myGridPosition;
    
    return GridLocation({ -1, -1 });
}

GridLocation Entity::GetClosestWalkableGridPosition() const
{
    auto grid = MainSingleton::GetInstance().GetGrid();

    if (!grid->IsTileWalkable(myGridPosition))
    {
        for (auto t : grid->Neighbors(myGridPosition))
        {
            if (grid->IsTileWalkable(t))
            {
                return t;
            }
        }
    }
    return GridLocation({ - 1, -1 });
}

std::vector<GridLocation> Entity::GetOverlappingCells()
{
    std::vector<GridLocation> overlappingCells;
    overlappingCells.push_back(myGridPosition);

    float tileSize = MainSingleton::GetInstance().GetTileSize();

    float worldX = mySpriteData.myPosition.x;
    float worldY = mySpriteData.myPosition.y;

    float worldX_End = worldX + mySpriteData.mySize.x;
    float worldY_End = worldY + mySpriteData.mySize.y;

    /*int startX = static_cast<int>(std::floor(worldX / tileSize));
    int startY = static_cast<int>(std::floor(worldY / tileSize));
    int endX = static_cast<int>(std::floor(worldX_End / tileSize));
    int endY = static_cast<int>(std::floor(worldY_End / tileSize));*/
    
    int startX = static_cast<int>(std::ceil(worldX / tileSize));
    int startY = static_cast<int>(std::ceil(worldY / tileSize));
    int endX = static_cast<int>(std::ceil(worldX_End / tileSize));
    int endY = static_cast<int>(std::ceil(worldY_End / tileSize));


    for (int y = startY; y < endY; ++y)
    {
        for (int x = startX; x < endX; ++x)
        {
            overlappingCells.push_back({ x, y });
        }
    }

    return overlappingCells;
}

GridLocation Entity::GetClosestInteractLocation(GridLocation aLoc)
{
    GridLocation location{ -1, -1};

    int bestDistance = std::numeric_limits<int>::max();
    auto grid = MainSingleton::GetInstance().GetGrid();

    for (auto cell : GetOverlappingCells())
    {
        std::vector<GridLocation> possibleMoves = {
            {cell.x + 1, cell.y}, {cell.x - 1, cell.y}, // Left and Right
            {cell.x, cell.y + 1}, {cell.x, cell.y - 1}  // Up and Down
        };

        for (auto walkableCell : possibleMoves)
        {
            if (grid->IsTileWalkable(walkableCell))
            {
                int distance = std::abs(walkableCell.x - aLoc.x) + std::abs(walkableCell.y - aLoc.y);
                if (distance < bestDistance)
                {
                    bestDistance = distance;
                    location = walkableCell;
                }
            }
        }
    }

    return location;
}

std::vector<GridLocation>& Entity::GetClosestInteractLocations(GridLocation aLoc, std::vector<GridLocation>& aPositionContainer)
{
    aPositionContainer.clear();
    auto grid = MainSingleton::GetInstance().GetGrid();

    for (const auto& cell : GetOverlappingCells())
    {
        std::vector<GridLocation> possibleMoves = 
        {
            {cell.x, cell.y},
            {cell.x + 1, cell.y},
            {cell.x - 1, cell.y},
            {cell.x, cell.y + 1},
            {cell.x, cell.y - 1}
        };

        if (grid->IsTileWalkable(possibleMoves.front()))
        {
            aPositionContainer.push_back(possibleMoves.front());
            return aPositionContainer;
        }

        for (const auto& walkableCell : possibleMoves)
        {
            if (grid->IsTileWalkable(walkableCell))
            {
                // Avoid duplicates
                if (std::find(aPositionContainer.begin(), aPositionContainer.end(), walkableCell) == aPositionContainer.end())
                {
                    aPositionContainer.push_back(walkableCell);
                }
            }
        }
    }
    aLoc;
    /*std::sort(aPositionContainer.begin(), aPositionContainer.end(),
        [&aLoc](const GridLocation& a, const GridLocation& b)
        {
            int distA = std::abs(a.x - aLoc.x) + std::abs(a.y - aLoc.y);
            int distB = std::abs(b.x - aLoc.x) + std::abs(b.y - aLoc.y);
            return distA < distB;
        });*/

    return aPositionContainer;
}

std::shared_ptr<Entity> Entity::CreateEntity(EntityType aEntityType)
{
    auto& textureManager = Tga::Engine::GetInstance()->GetTextureManager();
    
    std::shared_ptr<Entity> entity; 
    
    // Initialize texture based on the entity type.
    switch (aEntityType) 
    {
    case EntityType::GoldMine:
        {
            entity = std::make_shared<Entity>();
            entity->myTexture.myTexture = textureManager.GetTexture(L"Sprites/Resources/Gold Mine/GoldMine_Active.png");
            entity->myDecliningTexture.myTexture = textureManager.GetTexture(L"Sprites/Resources/Gold Mine/GoldMine_Active.png");
            entity->myDepletedTexture.myTexture = textureManager.GetTexture(L"Sprites/Resources/Gold Mine/GoldMine_Destroyed.png");
            entity->mySpriteData.myPivot = { 0.0f, 1.0f };
            entity->mySpriteData.mySize = { MainSingleton::GetInstance().GetTileSize() * 2.f, MainSingleton::GetInstance().GetTileSize() * 2.f };
            entity->SetHealth(50);
            entity->SetMaxHealth(50);
        }
        break;
    case EntityType::StoneMine:
        {
            entity = std::make_shared<Entity>();
            entity->myTexture.myTexture = textureManager.GetTexture(L"Sprites/Resources/Stone Mine/StoneMine_Active.png");
            entity->myDecliningTexture.myTexture = textureManager.GetTexture(L"Sprites/Resources/Stone Mine/StoneMine_Active.png");
            entity->myDepletedTexture.myTexture = textureManager.GetTexture(L"Sprites/Resources/Gold Mine/GoldMine_Destroyed.png");
            entity->mySpriteData.myPivot = { 0.0f, 1.0f };
            entity->mySpriteData.mySize = { MainSingleton::GetInstance().GetTileSize() * 2.f, MainSingleton::GetInstance().GetTileSize() * 2.f };
            entity->SetHealth(50);
            entity->SetMaxHealth(50);
        }
        break;
    case EntityType::Tree:
        {
            entity = std::make_shared<Entity>();
            entity->myTexture.myTexture = textureManager.GetTexture(L"Sprites/Resources/Trees/Tree_Solo.png");
            entity->myDecliningTexture.myTexture = textureManager.GetTexture(L"Sprites/Resources/Trees/Tree_Solo.png");
            entity->myDepletedTexture.myTexture = textureManager.GetTexture(L"Sprites/Resources/Trees/Tree_Dead.png");
            entity->mySpriteData.myPivot = { 0.0f, 1.0f };
            entity->mySpriteData.mySize = { MainSingleton::GetInstance().GetTileSize() * 1.f, MainSingleton::GetInstance().GetTileSize() * 2.f };
            entity->SetHealth(3);
            entity->SetMaxHealth(3);
        }
        break;
    case EntityType::Sheep:
        {
            entity = std::make_shared<Entity>();
            entity->myTexture.myTexture = textureManager.GetTexture(L"Sprites/Resources/Sheep/Sheep_Solo.png");
            entity->myDecliningTexture.myTexture = textureManager.GetTexture(L"Sprites/Resources/Sheep/DecliningSheep.png");
            entity->myDepletedTexture.myTexture = textureManager.GetTexture(L"Sprites/Resources/Sheep/Dead_Solo.png");
            entity->mySpriteData.myPivot = { 0.5f, 0.5f };
            entity->mySpriteData.mySize = { MainSingleton::GetInstance().GetTileSize() * 0.75f, MainSingleton::GetInstance().GetTileSize() * 0.75f };
            entity->SetHealth(10);
            entity->SetMaxHealth(10);
        }
        break;
    case EntityType::Base:
        {
            entity = std::make_shared<Base>();
            entity->myTexture.myTexture = textureManager.GetTexture(L"Sprites/Factions/Knights/Buildings/Castle/Castle_Purple.png");
            entity->mySpriteData.myPivot = { 0.0f, 1.0f };
            entity->SetMaxHealth(5);
            entity->SetHealth(5);
            entity->mySpriteData.mySize = { MainSingleton::GetInstance().GetTileSize() * 4.f, MainSingleton::GetInstance().GetTileSize() * 4.f };
        }
        break;
    case EntityType::House:
        {
            entity = std::make_shared<Entity>();
            entity->myTexture.myTexture = textureManager.GetTexture(L"Sprites/Factions/Knights/Buildings/House/House_Purple.png");
            entity->myDecliningTexture.myTexture = textureManager.GetTexture(L"Sprites/Factions/Knights/Buildings/House/House_Construction.png");
            entity->SetMaxHealth(2);
            entity->mySpriteData.myPivot = { 0.0f, 1.0f };
            entity->mySpriteData.mySize = { MainSingleton::GetInstance().GetTileSize() * 2.f, MainSingleton::GetInstance().GetTileSize() * 2.f };
        }
        break;
    case EntityType::Farmer:
        {
            entity = std::make_shared<Farmer>();
            entity->myTexture.myTexture = textureManager.GetTexture(L"Sprites/Factions/Knights/Troops/Pawn/Purple/Pawn_Purple_Solo.png");
            entity->myDepletedTexture.myTexture = textureManager.GetTexture(L"Sprites/Factions/Knights/Troops/Dead/Dead_Farmer_Solo.png");
            entity->mySpriteData.myPivot = { 0.5f, 0.5f };
            entity->SetMaxHealth(1);
            entity->mySpriteData.mySize = { MainSingleton::GetInstance().GetTileSize() * 0.75f, MainSingleton::GetInstance().GetTileSize() * 1.0f };
        }
        break;
    case EntityType::Soldier:
        {
            entity = std::make_shared<Soldier>();
            entity->myTexture.myTexture = textureManager.GetTexture(L"Sprites/Factions/Knights/Troops/Warrior/Purple/Warrior_Purple_Solo.png");
            entity->mySpriteData.myPivot = { 0.5f, 0.5f };
            entity->mySpriteData.mySize = { MainSingleton::GetInstance().GetTileSize() * 1.0f, MainSingleton::GetInstance().GetTileSize() * 1.25f };
        }
        break;
    case EntityType::Goblin:
        {      
            entity = std::make_shared<Goblin>();
            entity->myTexture.myTexture = textureManager.GetTexture(L"Sprites/Factions/Goblins/Troops/Torch/Red/Torch_Red_Solo.png");
            entity->myDepletedTexture.myTexture = textureManager.GetTexture(L"Sprites/Factions/Knights/Troops/Dead/Dead_Goblin_Solo.png");
            entity->mySpriteData.myPivot = { 0.5f, 0.5f };
            entity->SetMaxHealth(1);
            entity->mySpriteData.mySize = { MainSingleton::GetInstance().GetTileSize() * 1.0f, MainSingleton::GetInstance().GetTileSize() * 1.0f };
        }
    break;
    default:
        {
        // Optionally handle Undefined or Count.
        }
        break;
    }

    entity->SetEntityType(aEntityType);

    MainSingleton::GetInstance().AddActiveEntity(entity);

    return entity;
}

void Entity::SetHealth(int aHealth)
{
    myHealth = aHealth;
}

void Entity::SetEnvironmentSpriteData()
{
}

void Entity::SetCharacterSpriteData()
{
}




    
