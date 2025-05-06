#pragma once
#include "State.h"
#include "UIScene.h"

class StateMainMenu : public State
{
public:
	StateMainMenu();
	void Init() override;
	void Update(float aDeltaTime) override;
	void Render() override;

private:
	UIScene* myScene;
};

