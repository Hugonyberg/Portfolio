#pragma once
#include "UnityLoader.h"
#include "Sprite.h"
#include "Button.h"
#include "HUDElement.h"

#include <vector>
#include <DreamEngine/graphics/sprite.h>

class HUDLoader : public Observer
{
public:
	HUDLoader(UIData aUIData); 
	~HUDLoader();
	void Init();
	void Update(float aDeltaTime);
	void Render();

	void Receive(const Message& aMsg) override; 

private:
	std::vector<HUDElement> myHUDElements;
	std::vector<Sprite> mySprites;
	std::vector<Button> myButtons;

	bool ESCActive = false;
	bool myShowNoHUD = false;
	bool myIsPlaying = false;
	bool myShowIntro = false;
	bool myFinishedIntro = false;
};

