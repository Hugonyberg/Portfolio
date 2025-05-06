#pragma once
#include "BoatComponent.h"

class Player;
class WeakPointComponent : public BoatComponent
{
public:
	WeakPointComponent();
	~WeakPointComponent()override = default;

	void Init() override;
	void Update(const float aDeltaTime) override;

	void OnInteractSouthButton(Player& aPlayer) override;

	void TriggerContact(Player& aPlayer) override;

	void HandlePlayerInput(Player& aPlayer) override;

private:

	Player* myPlayer = nullptr;
	int myStage = 1; // can go up to 4
	bool myIsRepaired = true;
	//need a decal, mesh and a water VFX
};