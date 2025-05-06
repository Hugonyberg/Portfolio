#pragma once
#include <DreamEngine/graphics/sprite.h>
#include "UnityLoader.h"
#include "Observer.h"

class Sprite : public Observer
{
public:
	Sprite() = default;
	Sprite(UIData& someData, const int& aID, const DreamEngine::Vector2ui& aScreenSize);

	void SetCustomTexture(std::wstring aTexturePath, DE::Vector2f aPosition, DE::Vector2f aScale, const DreamEngine::Vector2ui& aScreenSize);
	void Init(); 
	void Render(); 

	void Receive(const Message& aMsg) override; 

private:
	DreamEngine::Sprite2DInstanceData mySpriteInstance; 
	DreamEngine::SpriteSharedData mySprite; 
	DreamEngine::Vector2f myScreenScale; 

	std::wstring myFilePath;
	bool myShouldRender = true;
};