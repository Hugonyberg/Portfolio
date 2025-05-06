#pragma once
#include <DreamEngine/graphics/sprite.h>

#include "UnityLoader.h"
#include "SettingsSlider.h"
#include "ButtonEvent.h"
#include "Message.h"
#include <DreamEngine/utilities/CountTimer.h>
#include "State.h"

class Button
{
public:
	Button(UIData& someData, const int& aID, const DreamEngine::Vector2ui& aScreenSize, eUIButton aButtonState);

	void Update(float aDeltaTime);
	void Render();

	bool ClickAction();
	
	void SetButtonState(eUIButton aButtonState);
	eUIButton GetButtonState();
	Message GetButtonMessage() { return myButtonMessage; }

private:
	DreamEngine::Sprite2DInstanceData mySpriteInstance;
	DreamEngine::SpriteSharedData mySprite;
	DreamEngine::Vector2f myScreenScale;

	DreamEngine::Vector2f myInitScreenSize;

	Message myButtonMessage; 
	ButtonEvent* myButtonEvent; 
	SettingsSlider* mySettingsSlider;
	State::eStateName myStateNamePlaceholder;
	
	float mySizeX;
	float mySizeY;
	float myButtonPosX;
	float myButtonPosY;

	eUIButton myButtonType;

	CU::CountupTimer myTimer;
	
	bool myShowImage;
};

