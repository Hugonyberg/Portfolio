#include "stdafx.h"

#include "RepairComponent.h"
#include "Player.h"

void RepairComponent::Init()
{
	myRepairKits = myMaxRepair;
}

void RepairComponent::OnInteractSouthButton(Player& aPlayer)
{
	if (!aPlayer.IsCarryingObject())
	{
		if (myRepairKits > 0)
		{
			aPlayer.GiveObject(eObjectType::Repair);
		    --myRepairKits;
		}
	}
}
