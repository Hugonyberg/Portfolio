#include "stdafx.h"
#include "SteerComponent.h"

#include "Player.h"
#include "MainSingleton.h"

#include <DreamEngine\utilities\InputManager.h>

SteerComponent::SteerComponent()
{
}

void SteerComponent::Init()
{
}

void SteerComponent::Update(const float aDeltaTime)
{
}

void SteerComponent::OnInteractSouthButton(Player& aPlayer)
{
	if (!aPlayer.GetPlayerLock())
	{
		aPlayer.TogglePlayerLock();
		aPlayer.SetLockPosition(GetInteractionPoint(), myTransform.GetMatrix().GetRight()); // TODO : Change to forward when or If model is corrected
	}
}

void SteerComponent::OnInteractEastButton(Player& aPlayer)
{
	aPlayer.TogglePlayerLock();
}

void SteerComponent::HandlePlayerInput(Player& aPlayer)
{
	auto& input = MainSingleton::GetInstance()->GetInputManager();

	if (input.IsKeyDown(DE::eKeyCode::A) || input.IsKeyHeld(DE::eKeyCode::A))
	{
		if (myCurrentTurnSpeed > myMinTurnSpeed)
		{
			myCurrentTurnSpeed -= 0.5f;
		}
	}

	if (input.IsKeyDown(DE::eKeyCode::D) || input.IsKeyHeld(DE::eKeyCode::D))
	{
		if (myCurrentTurnSpeed < myMaxTurnSpeed)
		{
			myCurrentTurnSpeed += 0.5f;
		}
	}

	aPlayer.GetBoat()->SetTurnSpeed(myCurrentTurnSpeed * (3.14159265f / 180.f));
}
