#pragma once
#include <memory>
#include <vector>

#include "Entity.h"
#include "Base.h"
#include "Farmer.h"

#include "Grid.h"
#include "HUD.h"

#include <tge\input\InputManager.h>

class MainSingleton
{
public:
    static MainSingleton& GetInstance()
    {
        static MainSingleton instance;
        return instance;
    }

    MainSingleton(const MainSingleton&) = delete;
    MainSingleton& operator=(const MainSingleton&) = delete;

    void InitResourceTextures();
    Tga::SpriteSharedData GetResourceTexture(EntityType aType);

    void SetInputManager(std::shared_ptr<Tga::InputManager> anInputManager) { myInputManager = anInputManager; };
    std::shared_ptr<Tga::InputManager> GetInputManager() { return myInputManager; }

    const std::vector<std::shared_ptr<Entity>>& GetEntities() const { return myEntities; }
    void AddEntity(std::shared_ptr<Entity> anEntity) { myEntities.push_back(anEntity); }

    void AddActiveEntity(std::shared_ptr<Entity> anEntity);
    std::vector<std::shared_ptr<Entity>> GetActiveActors() { return myActiveActors; }
    std::vector<std::shared_ptr<Farmer>> GetActiveFarmers() { return myActiveFarmers; }
    std::vector<std::shared_ptr<Goblin>> GetActiveGoblins() { return myActiveGoblins; }
    std::vector<std::shared_ptr<Soldier>> GetActiveSoldiers() { return myActiveSoldiers; }
    std::vector<std::shared_ptr<Entity>> GetActiveTrees() { return myActiveTrees; }
    std::vector<std::shared_ptr<Entity>> GetActiveSheep() { return myActiveSheep; }
    std::vector<std::shared_ptr<Entity>> GetActiveGoldMines() { return myActiveGoldMines; }
    std::vector<std::shared_ptr<Entity>> GetActiveStoneMines() { return myActiveStoneMines; }

    void SetActiveSoldierOverlappingPositions();
    std::vector<GridLocation> GetActiveSoldierOverlappingPositions(Soldier* aSoldier);

    const GridLocation GetClosestFarmerLocation(GridLocation& aFromLocation) const;

    void SetTileSize(float aTileSize) { myTileSize = aTileSize; }
    const float GetTileSize() const { return myTileSize; }

    void SetWidth(int aWidth) { myWidth = aWidth; }
    const int GetWidth() const { return myWidth; }

    void SetNumberOfGoblins(int aNumberOfGoblins) { myNumberOfGoblins = aNumberOfGoblins; }
    const int GetNumberOfGoblins() const { return myNumberOfGoblins; }

    void SetMaxNumberOfGoblins(int aMaxNumberOfGoblins) { myMaxNumberOfGoblins = aMaxNumberOfGoblins; }
    const int GetMaxNumberOfGoblins() const { return myMaxNumberOfGoblins; }

    void SetHeight(int aHeight) { myHeight = aHeight; }
    const int GetHeight() const { return myHeight; }

    void SetWorldState(WorldState aWorldState) { myWorldState = aWorldState; }
    const WorldState GetWorldState() const { return myWorldState; }

    void SetBridgePositions(std::vector<Tga::Vector2f>& aBridgePositions) { myBridgePositions = aBridgePositions; }
    const std::vector<Tga::Vector2f>& GetBridgePositions() const { return myBridgePositions; }

    void SetGrid(std::shared_ptr<Grid> aGrid) { myGrid = aGrid; }
    std::shared_ptr<Grid> GetGrid() { return myGrid; }

    void SetHUD(std::shared_ptr<HUD> aHUD) { myHUD = aHUD; }
    std::shared_ptr<HUD> GetHUD() { return myHUD; }

    void SetBase(std::shared_ptr<Base> aBase) { myBase = aBase; }
    std::shared_ptr<Base> GetBase() { return myBase; }
    GridLocation GetBaseDropPosition()
    {
        GridLocation dropPosition = myBase->GetGridPosition();
        dropPosition.y += 2;
        dropPosition.x += 2;
        return dropPosition;
    }

private:
    MainSingleton() {}

    std::shared_ptr<Tga::InputManager> myInputManager;

    std::vector<std::shared_ptr<Entity>> myEntities;
    std::vector<std::shared_ptr<Entity>> myActiveActors;
    std::vector<std::shared_ptr<Farmer>> myActiveFarmers;
    std::vector<std::shared_ptr<Goblin>> myActiveGoblins;
    std::vector<std::shared_ptr<Soldier>> myActiveSoldiers;
    std::vector<std::shared_ptr<Entity>> myActiveTrees;
    std::vector<std::shared_ptr<Entity>> myActiveSheep;
    std::vector<std::shared_ptr<Entity>> myActiveGoldMines;
    std::vector<std::shared_ptr<Entity>> myActiveStoneMines;

    std::vector<GridLocation> mySoldierOverlappingPositions;

    int myNumberOfGoblins = 0;
    int myMaxNumberOfGoblins = 0;

    std::shared_ptr<Grid> myGrid;
    std::shared_ptr<Base> myBase;

    Tga::SpriteSharedData myResourceTextures[4]{};

    std::shared_ptr<HUD> myHUD;

    WorldState myWorldState;

    std::vector<Tga::Vector2f> myBridgePositions;

    float myTileSize;
    int myWidth;
    int myHeight;
};
