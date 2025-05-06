#pragma once
#include <DreamEngine/graphics/sprite.h>
#include "Observer.h"

enum class eSliderType 
{
	Main,
	Sfx,
	Music,
	FOV
};

class SettingsSlider : public Observer
{
public:
	SettingsSlider() = delete;
	SettingsSlider(eSliderType aSliderType);
	~SettingsSlider();

	void Init(DreamEngine::Vector2f aMinPos, DreamEngine::Vector2f aMaxPos);
	void Update();
	void Render();

	void Receive(const Message& aMsg) override;

private:
	DreamEngine::Sprite2DInstanceData mySpriteInstanceButton;
	DreamEngine::SpriteSharedData mySpriteButton;

	eSliderType mySliderType;
	DreamEngine::Vector2f mySlideMinPos;
	DreamEngine::Vector2f mySlideMaxPos;
	bool mySliderActive = false;
	bool myFollowCursor = true;
};