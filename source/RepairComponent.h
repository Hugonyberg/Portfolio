#pragma once
#include "BoatComponent.h"

class Player;

class RepairComponent : public BoatComponent 
{
public:
    void Init() override;

    void OnInteractSouthButton(Player& aPlayer) override;

    void GiveRepairKit(int anAmount) { myRepairKits += anAmount; }
    void SetMaxRepairKits(int anAmount) { myMaxRepair = anAmount; }
private:
    int myRepairKits;
    int myMaxRepair = 5;
};