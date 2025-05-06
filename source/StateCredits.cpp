#include "StateCredits.h"
#include <DreamEngine/windows/settings.h>
#include "MainSingleton.h"

StateCredits::StateCredits()
{
	UIData creditScene = UnityLoader::LoadUI(DreamEngine::Settings::ResolveAssetPath("Json/Credits.json"));
	myScene = new UIScene(creditScene);
	myStateName = eStateName::Credit;
}

StateCredits::~StateCredits()
{
	delete myScene;
}

void StateCredits::Init()
{
	myScene->Init();
}

void StateCredits::Update(float aDeltaTime)
{
	MainSingleton::GetInstance()->SetInGame(false);
	MainSingleton::GetInstance()->SetShouldCaptureCursor(false);
	MainSingleton::GetInstance()->SetShouldRenderCursor(true);
	if (MainSingleton::GetInstance()->GetInputManager().IsKeyDown(DE::eKeyCode::Escape))
	{
		MainSingleton::GetInstance()->GetAudioManager().StopAudio(eAudioEvent::MenuClick);
		MainSingleton::GetInstance()->GetAudioManager().PlayAudio(eAudioEvent::MenuClick, DE::Vector3f(0.0f));
		MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ nullptr, eMessageType::Return });
	}
	myScene->Update(aDeltaTime);
}

void StateCredits::Render()
{
	myScene->Render();
}