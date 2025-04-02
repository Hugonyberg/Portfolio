#pragma once

#include "GridLocation.h"
#include "CommonFuncs.h"


namespace Tga 
{
    class SpriteDrawer; 
}

enum class EntityType 
{
    Undefined,
    /*Resources*/
    GoldMine,
    StoneMine,
    Tree,
    Sheep,
    /*Structures*/
    Base,
    House,
    /*AI*/
    Farmer,
    Soldier,
    Goblin,
    Wolf,
    Count
};

class Entity 
{
public:
    Entity();
    virtual ~Entity();

    virtual void Update(float aDeltaTime);

    virtual void Render(Tga::SpriteDrawer& aSpriteDrawer);

    EntityType GetEntityType() const;
    void SetEntityType(EntityType aEntityType);

    void SetGridPosition(GridLocation aLocation, bool anIsCharacter);
    GridLocation GetGridPosition() const;
    GridLocation GetClosestWalkableGridPosition() const;

    std::vector<GridLocation> GetOverlappingCells();
    GridLocation GetClosestInteractLocation(GridLocation aLoc);

    std::vector<GridLocation>& GetClosestInteractLocations(GridLocation aLoc, std::vector<GridLocation>& aPositionContainer);

    static std::shared_ptr<Entity> CreateEntity(EntityType aEntityType);

    const Tga::Sprite2DInstanceData GetSpriteData() const { return mySpriteData; }

    void SetHealth(int aHealth);
    const int GetHealth() const { return myHealth; }

    void SetMaxHealth(int aMaxHealth) { myMaxHealth = aMaxHealth; }
    const int GetMaxHealth() const { return myMaxHealth; }

protected:
    void SetEnvironmentSpriteData();
    void SetCharacterSpriteData();

    Tga::SpriteSharedData myTexture{};
    Tga::SpriteSharedData myDecliningTexture{};
    Tga::SpriteSharedData myDepletedTexture{};
    Tga::Sprite2DInstanceData mySpriteData{};

    GridLocation myGridPosition;

    EntityType myEntityType;

    bool myRecentlyDestroyed = false;
    bool myAliveLastFrame = true;

    int myHealth = 1;
    int myMaxHealth = 1;
};