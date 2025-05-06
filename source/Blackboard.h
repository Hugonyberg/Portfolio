#pragma once

#include <memory>
#include <vector>

#include "Navmesh.h"
#include "Player.h"

class Blackboard
{
public:
    void HandlePlayerWithinPath();

    const bool IsPlayerWithinPath() { return myPlayerWithinPath; }

    std::shared_ptr<DE::Navmesh> GetNavmesh() const { return myNavmesh; }
    void SetNavmesh(const std::shared_ptr<DE::Navmesh>& aNavmesh) { myNavmesh = aNavmesh; }

    std::shared_ptr<DE::Navmesh> GetDetailedNavmesh() const { return myDetailedNavmesh; }
    void SetDetailedNavmesh(const std::shared_ptr<DE::Navmesh>& aDetailedNavmesh) { myDetailedNavmesh = aDetailedNavmesh; }

    std::shared_ptr<Player> GetPlayer() const { return myPlayer; }
    void SetPlayer(const std::shared_ptr<Player>& aPlayer) { myPlayer = aPlayer; }

    void SetCamera(DE::Camera* aCameraPtr);
    DE::Camera* GetCameraPtr() { return myCameraPtr; }

private:
    std::shared_ptr<DE::Navmesh> myNavmesh;
    std::shared_ptr<DE::Navmesh> myDetailedNavmesh;
    std::shared_ptr<Player> myPlayer;
    DE::Camera* myCameraPtr;

    bool myPlayerWithinPath;
};