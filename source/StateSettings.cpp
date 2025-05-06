#include "StateSettings.h"
#include <DreamEngine/windows/settings.h> 

StateSettings::StateSettings()
{
	UIData creditScene = UnityLoader::LoadUI(DreamEngine::Settings::ResolveAssetPath("Json/Settings.json"));
	myScene = new UIScene(creditScene);
	myStateName = eStateName::Settings;
}

void StateSettings::Init()
{
	myScene->Init();
}

void StateSettings::Update(float aDeltaTime)
{
	MainSingleton::GetInstance()->SetInGame(false);
	MainSingleton::GetInstance()->SetShouldCaptureCursor(false);
	MainSingleton::GetInstance()->SetShouldRenderCursor(true);
	MainSingleton::GetInstance()->SetLastStateWasSettings(true);
	if (MainSingleton::GetInstance()->GetInputManager().IsKeyDown(DE::eKeyCode::Escape))
	{
		auto& playerPtr = MainSingleton::GetInstance()->GetPlayer();
		MainSingleton::GetInstance()->GetAudioManager().StopAudio(eAudioEvent::MenuClick);
		if (playerPtr != nullptr) MainSingleton::GetInstance()->GetAudioManager().PlayAudio(eAudioEvent::MenuClick, playerPtr->GetTransform()->GetPosition());
		else MainSingleton::GetInstance()->GetAudioManager().PlayAudio(eAudioEvent::MenuClick, DE::Vector3f(0.0f));
		MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ nullptr, eMessageType::Return });
	}
	myScene->Update(aDeltaTime);
}

void StateSettings::Render()
{
	myScene->Render();
}