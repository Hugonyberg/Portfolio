#pragma once
#include "BoatComponent.h"

class Player;

class AmmoComponent : public BoatComponent 
{
public:
    AmmoComponent();
    ~AmmoComponent() override = default;

    void Init() override;
    void Update(const float aDeltaTime) override;

    void OnInteractSouthButton(Player& aPlayer) override;

    void GiveAmmo(int anAmount) { myAmmoCount += anAmount; }
    void SetMaxAmmo(int anAmount) { myMaxAmmo = anAmount; }

private:
    int myAmmoCount;
    int myMaxAmmo = 5;
};