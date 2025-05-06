#pragma once
#include "State.h"
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/graphics/sprite.h>
#include <DreamEngine/utilities/CountTimer.h>
#include <vector>

class StateSplash : public State
{
public:

	StateSplash();
	void Init() override;
	void Update(float aDeltaTime) override;
	void Render() override;

private:
	DreamEngine::SpriteSharedData mySharedDatasBlack = {};
	DreamEngine::SpriteSharedData mySharedDatasTga = {};
	DreamEngine::SpriteSharedData mySharedDatasDreamEngine = {};
	DreamEngine::SpriteSharedData mySharedDatasWinWinGames = {};
	DreamEngine::SpriteSharedData mySharedDatasGameLoggo = {};
	DreamEngine::Sprite2DInstanceData myInstance = {};
	CU::CountupTimer myTimer; 
	int myImageCount = 0;
};
