#pragma once
#include <DreamEngine/graphics/sprite.h> 
#include <DreamEngine/text/text.h> 

#include <vector>

#include "Observer.h" 
#include "UnityLoader.h" 
#include "Message.h" 
#include <DreamEngine\utilities\CountTimer.h>

class HUDElement: public Observer
{
public:
	HUDElement(UIData& someData, const int& aID, const DreamEngine::Vector2ui& aScreenSize, eHUDAction aHUDAction); 
	~HUDElement();

	void Init();
	void Update(float aDeltaTime);
	void Render();

	bool IsHovered(); 
	void SetSubscribeState(eHUDAction aHUDAction);
	void Receive(const Message& aMsg) override;
	void ResizeImageVertically(int aCurrentAmount, int aMaxAmount); 
	void ResizeImageHorizontally(int aCurrentAmount, int aMaxAmount, bool aShrinkToTheLeft); 

	eHUDAction GetHUDAction() { return myHUDAction; }

private:
	DreamEngine::Sprite2DInstanceData mySpriteInstance; 
	DreamEngine::SpriteSharedData myActiveSpriteData; 
	DreamEngine::SpriteSharedData myInactiveSpriteData; 
	DreamEngine::Vector2f myScreenScale;

	DreamEngine::Vector2f myInitScreenSize;
	DreamEngine::Vector2f mySize;
	DreamEngine::Vector2f myPos;
	std::vector<DreamEngine::Text> myTexts; 

	float myOriginPosX;
	float myOriginPosY;
	float myOriginSizeX;
	float myOriginSizeY;

	eHUDAction myHUDAction; 
	eMessageType myMessageType; 

	int myDialogID;

	CU::CountdownTimer myActiveTimer;
	CU::CountdownTimer myLerpTimer;
	bool myShouldRender = false;
	bool myIsTextElement = false;
	bool myIsActive = true;
	bool myShouldLerpDown = true;
};