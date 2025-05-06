#pragma once
#include <DreamEngine/graphics/sprite.h> 

class Mouse
{
public:
	Mouse();

	void Update();
	void Render();

private:
	DreamEngine::Sprite2DInstanceData mySpriteInstance;
	DreamEngine::SpriteSharedData mySprite;
};

