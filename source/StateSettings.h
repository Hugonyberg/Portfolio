#pragma once
#include "State.h"
#include "UIScene.h"

class StateSettings : public State
{
public:
	StateSettings();
	void Init() override;
	void Update(float aDeltaTime) override;
	void Render() override;

private:
	UIScene* myScene;
};

