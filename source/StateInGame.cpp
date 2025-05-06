#include "StateInGame.h"
#include "MainSingleton.h"
#include "UnityLoader.h"
#include <DreamEngine/engine.h>
#include <DreamEngine/windows/settings.h>
#include <DreamEngine/engine.h>
#include <mutex>

std::mutex threadSafeTwo;

StateInGame::StateInGame()
{
}

StateInGame::~StateInGame()
{
	auto& PostMaster = MainSingleton::GetInstance()->GetPostMaster();
	PostMaster.Unsubscribe(eMessageType::LevelOne, this);
	PostMaster.Unsubscribe(eMessageType::LevelTwo, this);
	PostMaster.Unsubscribe(eMessageType::LevelCompleted, this);
}

void StateInGame::DeleteState()
{
	threadSafeTwo.lock();
	auto& PostMaster = MainSingleton::GetInstance()->GetPostMaster();
	PostMaster.Unsubscribe(eMessageType::LevelOne, this);
	PostMaster.Unsubscribe(eMessageType::LevelTwo, this);
	PostMaster.Unsubscribe(eMessageType::LevelCompleted, this);
	delete myScene;
	threadSafeTwo.unlock();
}

void StateInGame::Init()
{
	MainSingleton::GetInstance()->SetShouldRenderCursor(false);
	auto& PostMaster = MainSingleton::GetInstance()->GetPostMaster();
	PostMaster.Subscribe(eMessageType::LevelOne, this);
	PostMaster.Subscribe(eMessageType::LevelTwo, this);
	PostMaster.Subscribe(eMessageType::LevelCompleted, this);

	bool activationMessageData = true;
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &activationMessageData, eMessageType::ShowHud });
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &activationMessageData, eMessageType::Reset });

	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::Vector2ui intResolution = engine.GetRenderSize();
	DreamEngine::Vector2f resolution = {(float)intResolution.x,(float)intResolution.y};
	DreamEngine::Vector2f center = {resolution.x * 0.5f, resolution.y * 0.5f};


	std::pair<float, float> health = { 100.0f, 100.0f };
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &health, eMessageType::DefensePointAHealthChanged });
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &health, eMessageType::DefensePointBHealthChanged });
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &health, eMessageType::DefensePointCHealthChanged });

	bool deactivationMessageData = false;
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &deactivationMessageData, eMessageType::EnemySpawn1Toggle });
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &deactivationMessageData, eMessageType::EnemySpawn2Toggle });
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &deactivationMessageData, eMessageType::EnemySpawn3Toggle });
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &deactivationMessageData, eMessageType::DefensePointAToggle });
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &deactivationMessageData, eMessageType::DefensePointBToggle });
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &deactivationMessageData, eMessageType::DefensePointCToggle });

	int number = 0;
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &number, eMessageType::EnemyNumberChanged });
	number = 1;
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &number, eMessageType::WaveNumberChanged });

	MainSingleton::GetInstance()->GetAudioManager().StopAudio(eAudioEvent::MainMenuMusic);
	MainSingleton::GetInstance()->GetAudioManager().PlayAudio(eAudioEvent::StartGame, DE::Vector3f(0.0f));
}

void StateInGame::Update(float aDeltaTime)
{
	MainSingleton::GetInstance()->SetInGame(true);
	if(MainSingleton::GetInstance()->GetInputManager().IsKeyDown(DreamEngine::eKeyCode::Enter))
	{
		MainSingleton::GetInstance()->SetIsInCutscene(false);

		bool activationMessageData = true;
		MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &activationMessageData, eMessageType::ShowHud });
	}

	threadSafeTwo.lock();
	myScene->Update(aDeltaTime);
	threadSafeTwo.unlock();
}

void StateInGame::Render()
{
	threadSafeTwo.lock();
	myScene->Render();
	threadSafeTwo.unlock();
}

void StateInGame::Receive(const Message & aMsg)
{
	switch(aMsg.messageType)
	{
	case eMessageType::LevelOne:
	{
		MainSingleton::GetInstance()->GetAudioManager().StopAudio(eAudioEvent::Level1Music);
		MainSingleton::GetInstance()->GetAudioManager().PlayAudio(eAudioEvent::Level1Music, DE::Vector3f(0.0f));
		break;
	}
	case eMessageType::LevelTwo:
	{
		MainSingleton::GetInstance()->GetAudioManager().StopAudio(eAudioEvent::Level2Music);
		MainSingleton::GetInstance()->GetAudioManager().PlayAudio(eAudioEvent::Level2Music, DE::Vector3f(0.0f));
		break;
	}
	case eMessageType::LevelCompleted:
	{
		//win if player not dead
		
#ifdef _RETAIL
		Message msg;
		msg.messageType = eMessageType::MainMenuScene;
		MainSingleton::GetInstance()->GetPostMaster().TriggerMessage(msg);
#endif // RETAIL

		break;
	}
	}
}

void StateInGame::SelectLevel(int aSelectedLevel, LevelData aLevelData) 
{
	myScene = new Scene(aLevelData, aSelectedLevel);
	myScene->Init();
}