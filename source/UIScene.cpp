#include "UIScene.h"
#include "JsonSoreter.hpp"
#include <DreamEngine/engine.h>
#include <DreamEngine\graphics\GraphicsEngine.h>
#include <DreamEngine/graphics/TextureManager.h> 
#include <DreamEngine/graphics/GraphicsStateStack.h> 
#include <DreamEngine/graphics/DX11.h> 

UIScene::UIScene(UIData& aLevelUIData)
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::Vector2ui intResolution = engine.GetRenderSize();

	int UIBackgroundID = GetFirstIdFromTag(aLevelUIData.tags, "UIBackground");
	std::wstring filePath = aLevelUIData.spritePaths[UIBackgroundID];
	DreamEngine::Vector2ui screenSize = engine.GetTextureManager().GetTexture(filePath.c_str())->CalculateTextureSize();

	std::vector<int> UISpriteID = GetIdFromTag(aLevelUIData.tags, "UISprite");
	for(size_t i = 0; i < UISpriteID.size(); i++)
	{
		Sprite sprite(aLevelUIData, UISpriteID[i], screenSize);
		mySprites.push_back(sprite);
	}

	std::vector<int> UIButtonID = GetIdFromTag(aLevelUIData.tags, "UIButton");
	for(int i = 0; i < UIButtonID.size(); i++)
	{
		Button button(aLevelUIData, UIButtonID[i], screenSize, aLevelUIData.UIbutton[UIButtonID[i]]);
		myButtons.push_back(button);
	}
}

UIScene::~UIScene()
{
}

void UIScene::Init()
{
}

void UIScene::Update(float aDeltaTime)
{
	if (GetActiveWindow() == *DE::Engine::GetInstance()->GetHWND())
	{
		MainSingleton::GetInstance()->GetInputManager().ConfineCursorToWindow();
	}
	aDeltaTime; 
	for(size_t i = 0; i < myButtons.size(); i++)
	{
		myButtons[i].Update(aDeltaTime);
	}
}

void UIScene::Render()
{
	auto& graphicStateStack = DreamEngine::Engine::GetInstance()->GetGraphicsEngine().GetGraphicsStateStack();
	graphicStateStack.SetAlphaTestThreshold(0.0f);
	graphicStateStack.SetBlendState(DreamEngine::BlendState::AlphaBlend);

	for(size_t i = 0; i < mySprites.size(); i++)
	{
		DreamEngine::DX11::ourDepthBuffer->Clear();
		mySprites[i].Render();
	}

	for(size_t i = 0; i < myButtons.size(); i++)
	{
		DreamEngine::DX11::ourDepthBuffer->Clear();
		myButtons[i].Render();
	}
}

std::vector<Button>& UIScene::GetButtons()
{
	return myButtons;
}
