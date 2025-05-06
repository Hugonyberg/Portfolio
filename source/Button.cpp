#include "Button.h"
#include "MainSingleton.h"

#include <DreamEngine/engine.h>
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/graphics/SpriteDrawer.h>
#include <DreamEngine/graphics/TextureManager.h>
#include <DreamEngine/graphics/DX11.h>
#include <DreamEngine/graphics/DepthBuffer.h> 

#include <AK\SpatialAudio\Common\AkSpatialAudio.h>
#include <AK\SoundEngine\Common\AkTypes.h>
#include "WwiseHandle.h"

Button::Button(UIData& someData, const int& aID, const DreamEngine::Vector2ui& aScreenSize, eUIButton aButtonState)
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::Vector2ui intResolution = engine.GetRenderSize();
	myInitScreenSize = {(float)intResolution.x, (float)intResolution.y};

	if (aButtonState == eUIButton::IntroAccept)
	{
		mySprite.myTexture = engine.GetTextureManager().GetTexture(L"2D/S_UI_GuideButton.png");

		myScreenScale.x = static_cast<float>(mySprite.myTexture->CalculateTextureSize().x) / static_cast<float>(aScreenSize.x);
		myScreenScale.y = static_cast<float>(mySprite.myTexture->CalculateTextureSize().y) / static_cast<float>(aScreenSize.y);

		mySpriteInstance.myPivot = DE::Vector2f(0.5f);
		mySpriteInstance.myPosition = DE::Vector2f(myInitScreenSize.x / 2.0f, myInitScreenSize.y / 8.0f);
		mySpriteInstance.mySize.x = 2.0f * myScreenScale.x * myInitScreenSize.x;
		mySpriteInstance.mySize.y = 2.0f * myScreenScale.y * myInitScreenSize.y;

		mySizeX = mySpriteInstance.mySize.x;
		mySizeY = mySpriteInstance.mySize.y;

		myButtonPosX = mySpriteInstance.myPosition.x;
		myButtonPosY = mySpriteInstance.myPosition.y;

		mySpriteInstance.myColor = DreamEngine::Color(1.0f, 1.0f, 1.0f);
		SetButtonState(aButtonState);
		return;
	}

	std::wstring filePath = someData.spritePaths[aID];
	mySprite.myTexture = engine.GetTextureManager().GetTexture(filePath.c_str());

	myScreenScale.x = static_cast<float>(mySprite.myTexture->CalculateTextureSize().x) / static_cast<float>(aScreenSize.x);
	myScreenScale.y = static_cast<float>(mySprite.myTexture->CalculateTextureSize().y) / static_cast<float>(aScreenSize.y);

	mySpriteInstance.myPivot = someData.uIData[aID].pivot;
	mySpriteInstance.myPosition = someData.uIData[aID].position;
	mySpriteInstance.myPosition += myInitScreenSize * 0.5f;
	mySpriteInstance.mySize.x = someData.uIData[aID].scale.x * myScreenScale.x * myInitScreenSize.x;
	mySpriteInstance.mySize.y = someData.uIData[aID].scale.y * myScreenScale.y * myInitScreenSize.y;

	mySizeX = mySpriteInstance.mySize.x;
	mySizeY = mySpriteInstance.mySize.y;

	myButtonPosX = mySpriteInstance.myPosition.x;
	myButtonPosY = mySpriteInstance.myPosition.y;

	mySpriteInstance.myColor = DreamEngine::Color(1.0f, 1.0f, 1.0f); 

	SetButtonState(aButtonState);
}

void Button::Update(float aDeltaTime)
{
	if(myButtonType == eUIButton::Count) { return; }
	if(mySettingsSlider != nullptr)
	{
		mySettingsSlider->Update();
	}
	aDeltaTime; 
	auto& input = MainSingleton::GetInstance()->GetInputManager();


	if(!input.IsKeyDown(DreamEngine::eKeyCode::LeftMouseButton)) { return; }
	

	if(!ClickAction()) { return; }

	auto& playerPtr = MainSingleton::GetInstance()->GetPlayer();
	MainSingleton::GetInstance()->GetAudioManager().StopAudio(eAudioEvent::MenuClick);
	if (playerPtr != nullptr) MainSingleton::GetInstance()->GetAudioManager().PlayAudio(eAudioEvent::MenuClick, playerPtr->GetTransform()->GetPosition());
	else MainSingleton::GetInstance()->GetAudioManager().PlayAudio(eAudioEvent::MenuClick, DE::Vector3f(0.0f));


	if(myButtonMessage.messageType == eMessageType::TogglePauseMenu)
	{
		myShowImage = !myShowImage;
		bool& tempBool = myShowImage;
		myButtonMessage.messageData = &tempBool;
	}
	else if (myButtonMessage.messageType == eMessageType::LoadingScreen)
	{
		myButtonMessage.messageData = &myStateNamePlaceholder;
	}
	
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage(myButtonMessage); 
}

void Button::Render()
{
	if(myButtonType == eUIButton::Count) { return; }
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());

	spriteDrawer.Draw(mySprite, mySpriteInstance);

	if(mySettingsSlider != nullptr)
	{
		mySettingsSlider->Render();
	}
}

bool Button::ClickAction()
{
	auto& input = MainSingleton::GetInstance()->GetInputManager();

	DreamEngine::Vector2ui windowSize = DreamEngine::Engine::GetInstance()->GetWindowSize();
	DreamEngine::Vector2f resolution = (DreamEngine::Vector2f)DreamEngine::Engine::GetInstance()->GetRenderSize();
	float screenResizeModx = (float)windowSize.x / (float)myInitScreenSize.x;
	float screenResizeMody = (float)windowSize.y / (float)myInitScreenSize.y;

	mySizeX = mySpriteInstance.mySize.x * screenResizeModx;
	mySizeY = mySpriteInstance.mySize.y * screenResizeMody;

	myButtonPosX = mySpriteInstance.myPosition.x * screenResizeModx;
	myButtonPosY = mySpriteInstance.myPosition.y * screenResizeMody;

	float mousePosX = (float)input.GetMousePosition().x;
	float mousePosY = resolution.y * screenResizeMody - (float)input.GetMousePosition().y;

	if(mousePosX > (myButtonPosX - mySizeX / 2) && mousePosX < (myButtonPosX + mySizeX / 2))
	{
		if(mousePosY > (myButtonPosY - mySizeY / 2) && mousePosY < (myButtonPosY + mySizeY / 2))
		{
			return true;
		}
	}
	return false;
}



void Button::SetButtonState(eUIButton aButtonState)
{
	myButtonType = aButtonState; 

	switch(aButtonState)
	{
		case eUIButton::MainMenu:
		{
			myButtonMessage.messageType = eMessageType::MainMenuScene;
			break;
		}
		case eUIButton::LevelSelect:
		{
			myButtonMessage.messageType = eMessageType::LevelSelectScene;
			break;
		}
		case eUIButton::Settings:
		{
			myButtonMessage.messageType = eMessageType::SettingsScene;
			break; 
		}
		case eUIButton::Credits:
		{
			myButtonMessage.messageType = eMessageType::CreditsScene;
			break;
		}
		case eUIButton::Exit:
		{
			myButtonMessage.messageType = eMessageType::ExitProgram;
			break;
		}
		case eUIButton::Mute:
		{
			myButtonMessage.messageType = eMessageType::Mute;
			break;
		}
		case eUIButton::MainVolume:
		{
			DreamEngine::Vector2f minPos = {(mySpriteInstance.myPosition.x - (mySpriteInstance.mySize.x / 2)),myButtonPosY}; 
			DreamEngine::Vector2f maxPos = {(mySpriteInstance.myPosition.x + (mySpriteInstance.mySize.x / 2)),myButtonPosY}; 

			mySettingsSlider = new SettingsSlider(eSliderType::Main); 
			mySettingsSlider->Init(minPos, maxPos);

			myButtonMessage.messageType = eMessageType::MainVolume;
			break;
		}
		case eUIButton::SfxVolume:
		{
			DreamEngine::Vector2f minPos = {(mySpriteInstance.myPosition.x - (mySpriteInstance.mySize.x / 2)),myButtonPosY}; 
			DreamEngine::Vector2f maxPos = {(mySpriteInstance.myPosition.x + (mySpriteInstance.mySize.x / 2)),myButtonPosY}; 

			mySettingsSlider = new SettingsSlider(eSliderType::Sfx);
			mySettingsSlider->Init(minPos, maxPos);

			myButtonMessage.messageType = eMessageType::SfxVolume;
			break;
		}
		case eUIButton::MusicVolume:
		{
			DreamEngine::Vector2f minPos = {(mySpriteInstance.myPosition.x - (mySpriteInstance.mySize.x / 2)),myButtonPosY}; 
			DreamEngine::Vector2f maxPos = {(mySpriteInstance.myPosition.x + (mySpriteInstance.mySize.x / 2)),myButtonPosY}; 

			mySettingsSlider = new SettingsSlider(eSliderType::Music);
			mySettingsSlider->Init(minPos, maxPos);

			myButtonMessage.messageType = eMessageType::MusicVolume;
			break;
		}
		case eUIButton::FOV:
		{
			DreamEngine::Vector2f minPos = {(mySpriteInstance.myPosition.x - (mySpriteInstance.mySize.x / 2)),myButtonPosY}; 
			DreamEngine::Vector2f maxPos = {(mySpriteInstance.myPosition.x + (mySpriteInstance.mySize.x / 2)),myButtonPosY}; 

			mySettingsSlider = new SettingsSlider(eSliderType::FOV);
			mySettingsSlider->Init(minPos, maxPos);

			myButtonMessage.messageType = eMessageType::FOV;
			break;
		}
		case eUIButton::ScreenSizeFull:
		{
			myButtonEvent = new ButtonEvent;
			myButtonEvent->Init(eUIButton::ScreenSizeFull); 
			myButtonMessage.messageType = eMessageType::FullScreen;
			break;
		}
		case eUIButton::Resolution1280x720:
		{
			myButtonEvent = new ButtonEvent;
			myButtonEvent->Init(eUIButton::Resolution1280x720);
			myButtonMessage.messageType = eMessageType::Resolution1280x720;
			break;
		}
		case eUIButton::Resolution1920x1080:
		{
			myButtonEvent = new ButtonEvent;
			myButtonEvent->Init(eUIButton::Resolution1920x1080);
			myButtonMessage.messageType = eMessageType::Resolution1920x1080;
			break;
		}
		case eUIButton::Resolution2560x1440:
		{
			myButtonEvent = new ButtonEvent;
			myButtonEvent->Init(eUIButton::Resolution2560x1440);
			myButtonMessage.messageType = eMessageType::Resolution2560x1440;
			break;
		}
		case eUIButton::LevelOne:
		{
			myButtonMessage.messageType = eMessageType::LoadingScreen;
			myStateNamePlaceholder = State::eStateName::LevelOne;
			break;
		}
		case eUIButton::LevelTwo:
		{
			myButtonMessage.messageType = eMessageType::LoadingScreen;
			myStateNamePlaceholder = State::eStateName::LevelTwo;
			break;
		}
		case eUIButton::PlayerGym:
		{
			myButtonMessage.messageType = eMessageType::LoadingScreen;
			myStateNamePlaceholder = State::eStateName::PlayerGym;
			break;
		}
		case eUIButton::AssetGym:
		{
			myButtonMessage.messageType = eMessageType::LoadingScreen;
			myStateNamePlaceholder = State::eStateName::AssetGym;
			break;
		}
		case eUIButton::Back:
		{
			myButtonMessage.messageType = eMessageType::Return;
			break;
		}
		case eUIButton::Resume:
		{
			myButtonMessage.messageType = eMessageType::Resume;
			break;
		}
		case eUIButton::IntroAccept:
		{
			myButtonMessage.messageType = eMessageType::DeactivateIntroMenu;
			break;
		}
		default:
		{
#ifndef _RETAIL
			std::cout << "Didnt find a correct button state" << std::endl;
#endif
			break;
		}
	}
}

eUIButton Button::GetButtonState()
{
	return myButtonType;
}
