#include "SettingsSlider.h"
#include "MainSingleton.h"

#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/graphics/SpriteDrawer.h>
#include <DreamEngine/graphics/TextureManager.h>
#include <DreamEngine/engine.h>

SettingsSlider::SettingsSlider(eSliderType aSliderType) : mySliderType(aSliderType)
{
}

SettingsSlider::~SettingsSlider()
{
	auto& postMaster = MainSingleton::GetInstance()->GetPostMaster();
	switch (mySliderType) 
	{
		case eSliderType::Main: 
		{
			postMaster.Unsubscribe(eMessageType::MainVolume, this);
			break;
		}
		case eSliderType::Sfx: 
		{
			postMaster.Unsubscribe(eMessageType::SfxVolume, this);
			break;
		}
		case eSliderType::Music: 
		{
			postMaster.Unsubscribe(eMessageType::MusicVolume, this);
			break;
		}
	}
}

void SettingsSlider::Init(DreamEngine::Vector2f aMinPos, DreamEngine::Vector2f aMaxPos)
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	auto& postMaster = MainSingleton::GetInstance()->GetPostMaster();
	switch (mySliderType)
	{
		case eSliderType::Main:
		{
			postMaster.Subscribe(eMessageType::MainVolume, this);
			mySpriteInstanceButton.myPosition = { aMinPos.x + ((aMaxPos.x - aMinPos.x) * 0.75f), aMinPos.y };
			break;
		}
		case eSliderType::Sfx:
		{
			postMaster.Subscribe(eMessageType::SfxVolume, this);
			mySpriteInstanceButton.myPosition = { aMinPos.x + ((aMaxPos.x - aMinPos.x) * 0.75f), aMinPos.y };
			break;
		}
		case eSliderType::Music:
		{
			postMaster.Subscribe(eMessageType::MusicVolume, this);
			mySpriteInstanceButton.myPosition = { aMinPos.x + ((aMaxPos.x - aMinPos.x) * 0.75f), aMinPos.y };
			break;
		}
		case eSliderType::FOV:
		{
			postMaster.Subscribe(eMessageType::FOV, this);
			mySpriteInstanceButton.myPosition = { aMinPos.x + ((aMaxPos.x - aMinPos.x) * 1.0f), aMinPos.y };
			break;
		}
	}

	mySlideMinPos = aMinPos;
	mySlideMaxPos = aMaxPos;
	myFollowCursor = false;

	mySpriteButton.myTexture = engine.GetTextureManager().GetTexture(L"/2D/S_UI_SettingsSliderDragThing.png");
	mySpriteInstanceButton.myPivot = {0.5f,0.5f};
	mySpriteInstanceButton.mySize = mySpriteButton.myTexture->CalculateTextureSize();
}

void SettingsSlider::Update()
{
	if(mySliderActive || myFollowCursor)
	{
		auto& input = MainSingleton::GetInstance()->GetInputManager();

		if(!input.IsKeyHeld(DreamEngine::eKeyCode::LeftMouseButton)
			&& !input.IsKeyUp(DreamEngine::eKeyCode::LeftMouseButton)
			&& !input.IsKeyDown(DreamEngine::eKeyCode::LeftMouseButton))
		{
			mySliderActive = false;
			return;
		}
		myFollowCursor = true;

		DreamEngine::Vector2ui windowSize = DreamEngine::Engine::GetInstance()->GetWindowSize();
		DreamEngine::Vector2f resolution = (DreamEngine::Vector2f)MainSingleton::GetInstance()->GetResolution();

		float screenResizeModx = (float)resolution.x / (float)windowSize.x;
		float screenResizeMody = (float)resolution.y / (float)windowSize.y;

		float myMaxSizeX = mySlideMaxPos.x * screenResizeModx;
		float myMinSizeX = mySlideMinPos.x * screenResizeModx;

		float sliderPosY = mySpriteInstanceButton.myPosition.y;
		float buttonSize = mySpriteInstanceButton.mySize.y * screenResizeMody;
		float myMaxSizeY = sliderPosY + (buttonSize / 2);
		float myMinSizeY = sliderPosY - (buttonSize / 2);

		float mousePosX = MainSingleton::GetInstance()->GetMousePos().x;
		float mousePosY = MainSingleton::GetInstance()->GetMousePos().y;

		mySpriteInstanceButton.myPosition.x = mousePosX;
		if (mySpriteInstanceButton.myPosition.x < mySlideMinPos.x) mySpriteInstanceButton.myPosition.x = mySlideMinPos.x;
		if (mySpriteInstanceButton.myPosition.x > mySlideMaxPos.x) mySpriteInstanceButton.myPosition.x = mySlideMaxPos.x;

		if(!input.IsKeyUp(DreamEngine::eKeyCode::LeftMouseButton)) { return; } 

		myFollowCursor = false;
		float difference = mySpriteInstanceButton.myPosition.x * screenResizeModx - myMinSizeX;
		float hundred = myMaxSizeX - myMinSizeX;
		float percentage = difference / hundred;

		switch (mySliderType)
		{
			case eSliderType::Main:
			{
				MainSingleton::GetInstance()->GetAudioManager().SetMainVolume(percentage);
				MainSingleton::GetInstance()->GetAudioManager().UpdateMusicClipsVolume();
				MainSingleton::GetInstance()->GetAudioManager().UpdateSFXClipsVolume();
				break;
			}
			case eSliderType::Sfx:
			{
				MainSingleton::GetInstance()->GetAudioManager().SetSFXVolume(percentage);
				MainSingleton::GetInstance()->GetAudioManager().UpdateSFXClipsVolume();
				break;
			}
			case eSliderType::Music:
			{
				MainSingleton::GetInstance()->GetAudioManager().SetMusicVolume(percentage);
				MainSingleton::GetInstance()->GetAudioManager().UpdateMusicClipsVolume();
				break;
			}
			case eSliderType::FOV:
			{
				MainSingleton::GetInstance()->SetFOV(75.0f + 30 * percentage);
				break;
			}
		}
	}
}

void SettingsSlider::Render()
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());

	spriteDrawer.Draw(mySpriteButton, mySpriteInstanceButton);
}

void SettingsSlider::Receive(const Message & aMsg)
{
	switch (mySliderType)
	{
		case eSliderType::Main:
		{
			if (aMsg.messageType == eMessageType::MainVolume)
			{
				mySliderActive = true;
			}			
			break;
		}
		case eSliderType::Sfx:
		{
			if (aMsg.messageType == eMessageType::SfxVolume)
			{
				mySliderActive = true;
			}
			break;
		}
		case eSliderType::Music:
		{
			if (aMsg.messageType == eMessageType::MusicVolume)
			{
				mySliderActive = true;
			}
			break;
		}
		case eSliderType::FOV:
		{
			if (aMsg.messageType == eMessageType::FOV)
			{
				mySliderActive = true;
			}
			break;
		}
	}
}