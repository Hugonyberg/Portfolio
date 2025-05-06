#pragma once
#include "State.h"
#include "UIScene.h"

class StateCredits : public State
{
public:
	StateCredits();
	~StateCredits();
	void Init() override;
	void Update(float aDeltaTime) override;
	void Render() override;

private:

	UIScene* myScene;
};