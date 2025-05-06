#include "stdafx.h"
#include "WeakPointComponent.h"

#include "Player.h"

WeakPointComponent::WeakPointComponent()
{
}

void WeakPointComponent::Init()
{
	myBoatComponentType = BoatComponentType::WeakPoint;
}

void WeakPointComponent::Update(const float aDeltaTime)
{
	aDeltaTime;
}

void WeakPointComponent::OnInteractSouthButton(Player& aPlayer)
{
	aPlayer;
}

void WeakPointComponent::TriggerContact(Player& aPlayer)
{
    if (aPlayer.IsCarryingObject() && !myIsRepaired)
    {
        auto object = aPlayer.GetInventory();

        if (object == eObjectType::Repair)
        {
            aPlayer.RemoveObject();
            myIsRepaired = true;
            myStage = 1;
        }
    }
}

void WeakPointComponent::HandlePlayerInput(Player& aPlayer)
{
	aPlayer;
}
