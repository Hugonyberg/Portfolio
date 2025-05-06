#include "StateTestingGym.h"
#include "MainSingleton.h"

#include <DreamEngine/windows/settings.h>
#include <DreamEngine/graphics/ModelFactory.h>
#include <mutex>

std::mutex threadSafeThree;
StateTestingGym::StateTestingGym(bool aIsPlayerGym) : myIsPlayerGym(aIsPlayerGym) {}

void StateTestingGym::DeleteState()
{
	threadSafeThree.lock();
	delete myScene; 
	threadSafeThree.unlock();
}

void StateTestingGym::Init()
{
	if (myIsPlayerGym) 
	{
		LevelData playerGymScene = UnityLoader::LoadLevel(DreamEngine::Settings::ResolveAssetPath("Json/PlayerGym.json"));
		myScene = new Scene(playerGymScene, (int)eStateName::PlayerGym);

		myStateName = eStateName::PlayerGym;
	}
	else 
	{
		LevelData assetGymScene = UnityLoader::LoadLevel(DreamEngine::Settings::ResolveAssetPath("Json/AssetGym.json"));
		myScene = new Scene(assetGymScene, (int)eStateName::AssetGym);
		myStateName = eStateName::AssetGym;
	}

	bool activationMessageData = true;
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &activationMessageData, eMessageType::ShowHud });
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &activationMessageData, eMessageType::Reset });

	MainSingleton::GetInstance()->GetAudioManager().StopAudio(eAudioEvent::MainMenuMusic); 
	MainSingleton::GetInstance()->GetAudioManager().PlayAudio(eAudioEvent::StartGame, DE::Vector3f(0.0f));

	myScene->Init();
}

void StateTestingGym::Update(float aDeltaTime)
{
	MainSingleton::GetInstance()->SetInGame(true);
	threadSafeThree.lock();
	myScene->Update(aDeltaTime);
	threadSafeThree.unlock();
}

void StateTestingGym::Render()
{
	threadSafeThree.lock();
	myScene->Render();
	threadSafeThree.unlock();
}

void StateTestingGym::Receive(const Message & aMsg)
{
	aMsg; 
}