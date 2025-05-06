#include "stdafx.h"

#include "AmmoComponent.h"
#include "Player.h"

AmmoComponent::AmmoComponent()
{
}

void AmmoComponent::Init()
{
	myAmmoCount = myMaxAmmo;
}

void AmmoComponent::Update(const float aDeltaTime)
{
}

void AmmoComponent::OnInteractSouthButton(Player& aPlayer)
{
	if (!aPlayer.IsCarryingObject())
	{
		if (myAmmoCount > 0)
		{
			aPlayer.GiveObject(eObjectType::Ammo);
			myAmmoCount--;
		}
	}
}
