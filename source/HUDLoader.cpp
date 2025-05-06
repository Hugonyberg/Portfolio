#include "HUDLoader.h"
#include <DreamEngine/engine.h>
#include <DreamEngine/graphics/texture.h>
#include <DreamEngine/graphics/TextureManager.h>
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/graphics/GraphicsStateStack.h>

#include "MainSingleton.h"
#include "JsonSoreter.hpp"

HUDLoader::HUDLoader(UIData aUIData)
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::Vector2ui intResolution = engine.GetRenderSize();
	DreamEngine::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };

	int UIBackgroundID = GetFirstIdFromTag(aUIData.tags, "UIBackground");
	std::wstring filePath = aUIData.spritePaths[UIBackgroundID];
	DreamEngine::Vector2ui screenSize = engine.GetTextureManager().GetTexture(filePath.c_str())->CalculateTextureSize();

	std::vector<int> UISpriteID = GetIdFromTag(aUIData.tags, "UISprite");
	for(size_t i = 0; i < UISpriteID.size(); i++)
	{
		Sprite sprite(aUIData, UISpriteID[i], screenSize);
		mySprites.push_back(sprite);
	}
	Sprite introSprite1;
	introSprite1.SetCustomTexture(std::wstring(L"2D/S_UI_MenuPopupBackdrop.png"), DE::Vector2f(resolution.x / 2.0f, resolution.y / 2.0f), DE::Vector2f(1.0f), screenSize);
	mySprites.push_back(introSprite1);
	Sprite introSprite2;
	introSprite2.SetCustomTexture(std::wstring(L"2D/S_UI_GuideText.png"), DE::Vector2f(resolution.x / 2.0f, resolution.y / 2.0f + resolution.y * 0.075f), DE::Vector2f(2.0f), screenSize);
	mySprites.push_back(introSprite2);

	std::vector<int> HUDID = GetIdFromTag(aUIData.tags, "HUD");
	for(size_t i = 0; i < HUDID.size(); i++)
	{
		HUDElement hud(aUIData, HUDID[i], screenSize, aUIData.HUD[HUDID[i]]);
		myHUDElements.push_back(hud);
	}

	std::vector<int> UIButtonID = GetIdFromTag(aUIData.tags, "UIButton");
	for(int i = 0; i < UIButtonID.size(); i++)
	{
		Button button(aUIData, UIButtonID[i], screenSize, aUIData.UIbutton[UIButtonID[i]]);
		myButtons.push_back(button);
	}
	Button acceptButton(aUIData, -1, screenSize, eUIButton::IntroAccept);
	myButtons.push_back(acceptButton);

	HUDElement screenCenterText(aUIData, -1, screenSize, eHUDAction::ScreenCenterText);
	myHUDElements.push_back(screenCenterText);
}

HUDLoader::~HUDLoader()
{

}

void HUDLoader::Init()
{
	auto& postMaster = MainSingleton::GetInstance()->GetPostMaster(); 
	postMaster.Subscribe(eMessageType::ShowHud, this); 
	postMaster.Subscribe(eMessageType::TogglePauseMenu, this);
	postMaster.Subscribe(eMessageType::Resume, this);
	postMaster.Subscribe(eMessageType::ActivateIntroMenu, this);
	postMaster.Subscribe(eMessageType::ResetIntroMenu, this);
	postMaster.Subscribe(eMessageType::DeactivateIntroMenu, this);
	postMaster.Subscribe(eMessageType::Mute, this);

	for(size_t i = 0; i < myHUDElements.size(); i++)
	{
		myHUDElements[i].Init(); 
	}
	for(size_t i = 0; i < mySprites.size(); i++)
	{
		mySprites[i].Init();
	}
}

void HUDLoader::Update(float aDeltaTime)
{	
	for(size_t i = 0; i < myButtons.size() - 1; i++)
	{
		if(!ESCActive)
		{
			continue;
		}
		myButtons[i].Update(aDeltaTime);
	}
	if (myShowIntro)
	{
		myButtons.back().Update(aDeltaTime);
	}

	if(MainSingleton::GetInstance()->GetInputManager().IsKeyDown(DreamEngine::eKeyCode::Escape) && MainSingleton::GetInstance()->GetInGame() && !MainSingleton::GetInstance()->IsInCutscene() && !MainSingleton::GetInstance()->GetLastStateWasSettings())
	{
		ESCActive = !ESCActive;
		MainSingleton::GetInstance()->SetShouldCaptureCursor(!ESCActive);
		MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &ESCActive, eMessageType::TogglePauseMenu });
	}

	for(size_t i = 0; i < myHUDElements.size(); i++)
	{
		myHUDElements[i].Update(aDeltaTime);
	}
	MainSingleton::GetInstance()->SetLastStateWasSettings(false);
}

void HUDLoader::Render()
{
	if(!myShowNoHUD) { return; }

	auto& graphicsStateStack = DreamEngine::Engine::GetInstance()->GetGraphicsEngine().GetGraphicsStateStack();
	graphicsStateStack.Push();
	graphicsStateStack.SetAlphaTestThreshold(0.0f);
	//graphicsStateStack.SetAlphaTestThreshold(0.5f);
	graphicsStateStack.SetBlendState(DreamEngine::BlendState::AlphaBlend);
	graphicsStateStack.UpdateGpuStates(false);

	for(size_t i = 0; i < mySprites.size() - 2; i++)
	{
		mySprites[i].Render(); 
	}
	for(size_t i = 0; i < myHUDElements.size(); i++)
	{
		if(!ESCActive && myHUDElements[i].GetHUDAction() == eHUDAction::ControlsView) 
		{ 
			continue; 
		}
		myHUDElements[i].Render();
	}
	for(size_t i = 0; i < myButtons.size() - 1; i++)
	{
		if(!ESCActive && (myButtons[i].GetButtonMessage().messageType != eMessageType::TogglePauseMenu))
		{ 
			continue; 
		} 
		myButtons[i].Render();
	}
	if (myShowIntro)
	{
		mySprites[mySprites.size() - 2].Render();
		mySprites[mySprites.size() - 1].Render();
		myButtons.back().Render();
	}

	graphicsStateStack.Pop();
}

void HUDLoader::Receive(const Message& aMsg)
{
	switch (aMsg.messageType)
	{
		case eMessageType::ShowHud:
		{
			auto* info = static_cast<bool*>(aMsg.messageData);
			myShowNoHUD = *info;
			break;
		}
		case eMessageType::TogglePauseMenu:
		{
			auto* info = static_cast<bool*>(aMsg.messageData);
			ESCActive = *info;
			SetCursorPos(MainSingleton::GetInstance()->GetScreenSize().x / 2.0f, MainSingleton::GetInstance()->GetScreenSize().y / 2.0f);
			MainSingleton::GetInstance()->SetGameToPause(ESCActive);
			break;
		}
		case eMessageType::Resume:
		{
			ESCActive = !ESCActive;
			MainSingleton::GetInstance()->SetShouldCaptureCursor(!ESCActive);
			MainSingleton::GetInstance()->SetGameToPause(ESCActive);
			MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &ESCActive, eMessageType::TogglePauseMenu });
			break;
		}
		case eMessageType::ActivateIntroMenu:
		{
			if (!myFinishedIntro)
			{
				myShowIntro = true;
			}
			break;
		}
		case eMessageType::ResetIntroMenu:
		{
			myShowIntro = false;
			myFinishedIntro = false;
			break;
		}
		case eMessageType::DeactivateIntroMenu:
		{
			myShowIntro = false;
			myFinishedIntro = true;
			MainSingleton::GetInstance()->SetIsInCutscene(false);
			MainSingleton::GetInstance()->SetShouldCaptureCursor(true);
			MainSingleton::GetInstance()->SetShouldRenderCursor(false);
			SetCursorPos(MainSingleton::GetInstance()->GetScreenSize().x / 2.0f, MainSingleton::GetInstance()->GetScreenSize().y / 2.0f);
			break;
		}
		case eMessageType::Mute:
		{
			MainSingleton::GetInstance()->GetAudioManager().ToggleMute();
			MainSingleton::GetInstance()->GetAudioManager().UpdateMusicClipsVolume();
			MainSingleton::GetInstance()->GetAudioManager().UpdateSFXClipsVolume();
			break;
		}
	}
}