#include "StateLevelSelect.h"
#include <DreamEngine/windows/settings.h>
#include "MainSingleton.h"
#include <DreamEngine/graphics/sprite.h> 
#include <DreamEngine\engine.h>
#include <DreamEngine\graphics\TextureManager.h>
#include <DreamEngine/graphics/SpriteDrawer.h>

StateLevelSelect::StateLevelSelect()
{
	UIData creditScene = UnityLoader::LoadUI(DreamEngine::Settings::ResolveAssetPath("Json/LevelSelect.json"));
	myScene = new UIScene(creditScene);
	myStateName = eStateName::LevelSelect;
}

StateLevelSelect::~StateLevelSelect()
{
}

void StateLevelSelect::Init()
{
	myScene->Init();
}

void StateLevelSelect::Update(float aDeltaTime)
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

void StateLevelSelect::Render()
{
	myScene->Render();
}