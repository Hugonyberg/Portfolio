#pragma once
#include "BoatComponent.h"

class Player;

class MapComponent : public BoatComponent 
{
public:
    MapComponent();
    ~MapComponent() override = default;

    void Init() override;
    void Update(const float aDeltaTime) override;

    void OnInteractSouthButton(Player& aPlayer) override;

private:
    float myPingCooldown = 5.0f;  // seconds between pings
    float myTimeSinceLastPing = 0.0f;
};