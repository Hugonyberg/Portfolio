#pragma once
#include "BoatComponent.h"

class Player;

class SteerComponent : public BoatComponent 
{
public:
    SteerComponent();
    ~SteerComponent() override = default;

    void Init() override;
    void Update(const float aDeltaTime) override;

    void OnInteractSouthButton(Player& aPlayer) override;
    void OnInteractEastButton(Player& aPlayer) override;

    void HandlePlayerInput(Player& aPlayer) override;

private:
    bool myIsGrabbing = false;
    Player* myPlayer = nullptr;

    float myMaxTurnSpeed = 30.0f;
    float myMinTurnSpeed = -30.0f;
    float myCurrentTurnSpeed = 0.0f;
};