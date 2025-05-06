#include "StateLoadingScreen.h"
#include "MainSingleton.h"
#include "Message.h"
#include <DreamEngine\engine.h>
#include <DreamEngine/graphics/TextureManager.h>
#include <DreamEngine/graphics/SpriteDrawer.h>

StateLoadingScreen::StateLoadingScreen()
{}

void StateLoadingScreen::Init()
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	engine.SetClearColor(DreamEngine::Color(0.0f, 0.0f, 0.0f, 1.0f));

	DreamEngine::Vector2ui intResolution = engine.GetRenderSize();
	DreamEngine::Vector2f resolution = {(float)intResolution.x,(float)intResolution.y};
	DreamEngine::Vector2f center = {resolution.x * 0.5f, resolution.y * 0.5f};

	myTimer = CU::CountupTimer(0.0000000001f);

	mySharedData.myTexture = engine.GetTextureManager().GetTexture(L"2D/S_Loading.png");

	myInstance.myPivot = {0.5f,0.5f};
	myInstance.myPosition = {center};
	myInstance.mySize = {resolution.x, resolution.y};
	MainSingleton::GetInstance()->SetInGame(false);
	myStateName = eStateName::LoadingScreen;
}

void StateLoadingScreen::Update(float aDeltaTime)
{
	MainSingleton::GetInstance()->SetInGame(false);
	MainSingleton::GetInstance()->SetShouldRenderCursor(false);
	if(myTimer.ReachedThreshold())
	{
		switch(myStateToLoad)
		{
			case State::eStateName::LevelOne:
			{
				MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ nullptr, eMessageType::LevelOne });
				break;
			}
			case State::eStateName::LevelTwo:
			{
				MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ nullptr, eMessageType::LevelTwo });
				break;
			}
			case State::eStateName::PlayerGym:
			{
				MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ nullptr, eMessageType::PlayerGymScene });
				break;
			}
			case State::eStateName::AssetGym:
			{
				MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ nullptr, eMessageType::AssetGymScene });
				break;
			}
		}
	}
	myTimer.Update(aDeltaTime);
}

void StateLoadingScreen::Render()
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());
	spriteDrawer.Draw(mySharedData, myInstance);
}

void StateLoadingScreen::SetStateToLoad(State::eStateName aState)
{
	myStateToLoad = aState;
}