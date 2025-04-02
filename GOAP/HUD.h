#pragma once

#include "WorldModelState.h"
#include <tge\text\text.h>
#include <unordered_set>

namespace Tga
{
	class SpriteDrawer;
}

struct HUDMessage
{
	std::string text;
	Tga::Vector2f position;  
	float speed;             

	HUDMessage()
	{
		text = "";
		position = { 0.f,0.f };
		speed = 300.0f;
	}
	HUDMessage(const std::string& aText)
		: text(aText), position(0.f,0.f), speed(5.0f)
	{}
};

class HUD
{
public:
	HUD();
	~HUD();

	void Init();
	void Update(float aDeltaTime);
	void Render(Tga::SpriteDrawer& aSpriteDrawer);

	void AddMessage(HUDMessage& aMessage);
private:
	void RenderResources(Tga::SpriteDrawer& aSpriteDrawer);
	void RenderMessages(Tga::SpriteDrawer& aSpriteDrawer);

	Tga::SpriteSharedData myBackgroundTexture{};

	Tga::SpriteSharedData myBannerTexture{};

	Tga::SpriteSharedData myFoodTexture{};
	Tga::SpriteSharedData myStoneTexture{};
	Tga::SpriteSharedData myGoldTexture{};
	Tga::SpriteSharedData myWoodTexture{};

	Tga::Text myText;

	Tga::SpriteSharedData myHousesTexture{};
	Tga::SpriteSharedData myPopulationTexture{};
	Tga::SpriteSharedData mySoldierTexture{};

	Tga::Sprite2DInstanceData mySpriteData{};

	Tga::Vector2f myStartingMessagePosition;
	std::vector<HUDMessage> myMessages;
	static const std::unordered_set<std::string> myAllowedMessages;

	WorldState myCurrentWorldState;
};