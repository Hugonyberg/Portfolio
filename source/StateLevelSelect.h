#pragma once
#include "State.h"
#include "UIScene.h"

class StateLevelSelect : public State
{
public:
	StateLevelSelect();
	~StateLevelSelect();
	void Init() override;
	void Update(float aDeltaTime) override;
	void Render() override;

private:
	UIScene* myScene;
};

