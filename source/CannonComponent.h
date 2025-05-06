#pragma once
#include "BoatComponent.h"

#include <DreamEngine\utilities\CountTimer.h>

class Player;

struct Cannonball
{
    bool isActive;
    DE::Transform transform;
    CU::CountdownTimer lifeTimer = CU::CountdownTimer(2.5f);
};

class CannonComponent : public BoatComponent 
{
public:
    void Init() override;
    void Update(float aDeltaTime) override;

    void OnInteractSouthButton(Player& aPlayer) override;
    void OnInteractEastButton(Player& aPlayer) override;
    void OnInteractNorthButton(Player& aPlayer) override;

    void TriggerContact(Player& aPlayer) override;

    void HandlePlayerInput(Player& aPlayer) override;

    void Render(DE::GraphicsEngine& aGraphicsEngine) override;
private:
    void TurnTowards(DE::Vector3f aPos, DE::Transform& aTransformToTurn);

    enum class AmmoType { Cannonball, Harpoon } myCurrentAmmo;
    int myAmmoCount[2];
    void ShootCurrentAmmo(Player&);
    void ToggleAimMode(Player&);

    std::shared_ptr<DE::ModelInstance> myCannonBallModel;
    std::shared_ptr<DE::ModelInstance> myHarpoonModel;

    std::vector<Cannonball> myCannonballs;
    float myCannonBallSpeed = 5000.0f;
    int myCurrentAmmoCount;
    int myMaxAmmo = 5;

    bool myHarpoonActive = false;
    float myHarpoonSpeed = 1000.0f;
    DE::Vector3f myHarpoonStartPos;
    bool myHarpoonInitialized = false;

    DE::Vector3f myProjectileDirection;
    CU::CountdownTimer myShootTimer;

    DE::Vector3f myMaxRotation = { 0.0f, 90.0f, 0.0f };
    DE::Vector3f myMinRotation= { 0.0f, -90.0f, 0.0f };


    bool myShouldRotate;
    float myTurnSpeed = 180.f * (3.14159265f / 180.f); // rad/sec
    int myTurnDirection;
};