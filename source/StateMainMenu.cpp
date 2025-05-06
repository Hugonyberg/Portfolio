#include "StateMainMenu.h"
#include "MainSingleton.h"
#include <DreamEngine/windows/settings.h>

StateMainMenu::StateMainMenu()
{
	UIData creditScene = UnityLoader::LoadUI(DreamEngine::Settings::ResolveAssetPath("Json/MainMenu.json"));
	myScene = new UIScene(creditScene);
	myStateName = eStateName::Menu;
}

void StateMainMenu::Init()
{
	myScene->Init();

	bool deactivationMessageData = false;
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &deactivationMessageData, eMessageType::ShowHud });
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &deactivationMessageData, eMessageType::TogglePauseMenu });

	MainSingleton::GetInstance()->GetAudioManager().StopAudio(eAudioEvent::Level2Music);
	MainSingleton::GetInstance()->GetAudioManager().StopAudio(eAudioEvent::Level1Music);
	MainSingleton::GetInstance()->GetAudioManager().PlayAudio(eAudioEvent::MainMenuMusic, DE::Vector3f(0.0f));
}

void StateMainMenu::Update(float aDeltaTime)
{
	MainSingleton::GetInstance()->SetInGame(false);
	MainSingleton::GetInstance()->SetShouldCaptureCursor(false);
	MainSingleton::GetInstance()->SetShouldRenderCursor(true);
	myScene->Update(aDeltaTime);
}

void StateMainMenu::Render()
{
	myScene->Render();
}