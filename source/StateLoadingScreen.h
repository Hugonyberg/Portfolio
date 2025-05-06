#pragma once
#include "State.h"
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/graphics/sprite.h>
#include <DreamEngine/utilities/CountTimer.h>

class StateLoadingScreen : public State
{
public:

	StateLoadingScreen();
	void Init() override;
	void Update(float aDeltaTime) override;
	void Render() override;

	void SetStateToLoad(State::eStateName aState);

private:

	DreamEngine::SpriteSharedData mySharedData = {};
	DreamEngine::Sprite2DInstanceData myInstance = {};
	CU::CountupTimer myTimer;
	State::eStateName myStateToLoad; 
};

