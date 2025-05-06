#include "stdafx.h"
#include "SpeedComponent.h"

#include "Player.h"


SpeedComponent::SpeedComponent()
{
}

void SpeedComponent::Init()
{
}

void SpeedComponent::Update(const float aDeltaTime)
{
}

void SpeedComponent::OnInteractSouthButton(Player& aPlayer)
{
	aPlayer.GetBoat()->ToggleSpeed();
}

void SpeedComponent::HandlePlayerInput(Player& aPlayer)
{

}
