#pragma once
#include "BoatComponent.h"

class Player;

class SpeedComponent : public BoatComponent 
{
public:
    SpeedComponent();
    ~SpeedComponent() override = default;

    void Init() override;
    void Update(const float aDeltaTime) override;

    void OnInteractSouthButton(Player& aPlayer) override;

    void HandlePlayerInput(Player& aPlayer) override;

private:
    bool myIsFast = false;

};