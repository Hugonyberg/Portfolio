#pragma once
#include "UnityLoader.h"
#include "MainSingleton.h"
#include "Sprite.h"
#include "Button.h"

#include <vector>

class UIScene
{
public:
	UIScene(UIData& aLevelUIData); 
	~UIScene();

	void Init();
	void Update(float aDeltaTime);
	void Render();
	std::vector<Button>& GetButtons();


private:

	std::vector<Sprite> mySprites;
	std::vector<Button> myButtons;
};

