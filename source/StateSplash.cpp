#include "StateSplash.h"
#include "MainSingleton.h"
#include "Message.h"
#include <DreamEngine/graphics/TextureManager.h>
#include <DreamEngine/graphics/SpriteDrawer.h>
#include <DreamEngine/engine.h>


StateSplash::StateSplash()
{
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ nullptr, eMessageType::Resolution1920x1080 });
	myStateName = eStateName::Splash;
}

void StateSplash::Init()
{
	MainSingleton::GetInstance()->SetInGame(false);
	auto& engine = *DreamEngine::Engine::GetInstance();
	engine.SetClearColor(DreamEngine::Color(0.0f, 0.0f, 0.0f, 1.0f));

	myTimer = CU::CountupTimer(2.0f);

	DreamEngine::Vector2f resolution = (DreamEngine::Vector2f)MainSingleton::GetInstance()->GetResolution();
	DreamEngine::Vector2f center = { resolution.x * 0.5f, resolution.y * 0.5f };

	mySharedDatasTga.myTexture = engine.GetTextureManager().GetTexture(L"2D/tga_logo_white2.png");
	mySharedDatasDreamEngine.myTexture = engine.GetTextureManager().GetTexture(L"2D/DreamEngine.png");
	mySharedDatasWinWinGames.myTexture = engine.GetTextureManager().GetTexture(L"2D/WIN WIN GAMES.png");
	mySharedDatasGameLoggo.myTexture = engine.GetTextureManager().GetTexture(L"2D/Game_Icon.png");

	myInstance.myPivot = {0.5f,0.5f};
	myInstance.myPosition = {center};
	MainSingleton::GetInstance()->GetAudioManager().PlayAudio(eAudioEvent::MainMenuMusic, DE::Vector3f(0.0f));
}

void StateSplash::Update(float aDeltaTime)
{
#ifndef _RETAIL
	if(MainSingleton::GetInstance()->GetInputManager().IsKeyDown(DreamEngine::eKeyCode::Escape))
	{
		MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ nullptr, eMessageType::MainMenuScene });
	}
#endif // _DEBUG

	myTimer.Update(aDeltaTime);
	
	if(myTimer.ReachedThreshold())
	{
		DreamEngine::Vector2f resolution = (DreamEngine::Vector2f)MainSingleton::GetInstance()->GetResolution();
		DreamEngine::SpriteSharedData tempSprite;

		switch(myImageCount)
		{
			case 0:
			{
				myInstance.mySize = {resolution.x * 0.333f, resolution.y * 0.18f};
				myImageCount++;
				break;
			}
			case 1:
			{
				myImageCount++;
				break;
			}
			case 2:
			{
				myTimer = CU::CountupTimer(0.8f);

				myInstance.mySize = {resolution.y * 0.5f, resolution.y * 0.5f};
				myImageCount++;
				break;
			}
			case 3:
			{
				myImageCount++;
				break;
			}
			case 4:
			{
				myInstance.mySize = {resolution.y * 0.5f, resolution.y * 0.5f};
				myImageCount++;
				break;
			}
			case 5:
			{
				myImageCount++;
				break;
			}
			case 6:
			{
				myImageCount++;
				break;
			}
			case 7:
			{
				myInstance.mySize = {resolution.y * 0.5f, resolution.y * 0.5f};
				myImageCount++;
				break;
			}
			case 8:
			{
				myImageCount++;
				break;
			}
			case 9:
			{
				myInstance.mySize = {resolution.y * 0.5f, resolution.y * 0.5f};
				myImageCount++;
				break;
			}
			case 10:
			{
				myImageCount++;
				break;
			}
			case 11:
			{
				MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ nullptr, eMessageType::MainMenuScene });
				break;
			}
		}

		myTimer.Reset();
	}
}

void StateSplash::Render()
{
	auto& engine = *DreamEngine::Engine::GetInstance(); 
	DreamEngine::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer()); 

	switch(myImageCount)
	{
	case 1:
	{
		spriteDrawer.Draw(mySharedDatasTga, myInstance);
		break;
	}
	case 3:
	{
		spriteDrawer.Draw(mySharedDatasDreamEngine, myInstance);
		break;
	}
	case 5:
	{
		spriteDrawer.Draw(mySharedDatasWinWinGames, myInstance);
		break;
	}
	case 7:
	{
		spriteDrawer.Draw(mySharedDatasGameLoggo, myInstance);
		break;
	}
	case 9:
	{
		MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ nullptr, eMessageType::MainMenuScene });
		break;
	}
	}
}
