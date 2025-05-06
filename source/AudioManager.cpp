#include "AudioManager.h"
#include "MainSingleton.h"

AudioManager::AudioManager()
{
	myAudioCollection[eAudioEvent::MainMenuMusic] = { false, eAudioType::Music, DreamEngine::Audio() };
	myAudioCollection[eAudioEvent::MainMenuMusic].audioFile.Init("Audio/BGM_MainMenu.wav", true);

	myAudioCollection[eAudioEvent::Level1Music] = {false, eAudioType::Music, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::Level1Music].audioFile.Init("Audio/BGM_Level1.wav", true);

	myAudioCollection[eAudioEvent::Level2Music] = {false, eAudioType::Music, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::Level2Music].audioFile.Init("Audio/BGM_Level2.wav", true);

	myAudioCollection[eAudioEvent::CompanionIntroduction] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionIntroduction].audioFile.Init("Audio/GunnarIntorduction.mp3", false);

	myAudioCollection[eAudioEvent::CompanionShoot] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionShoot].audioFile.Init("Audio/Companion_Shoot.wav", false);

	myAudioCollection[eAudioEvent::CompanionHealing1] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionHealing1].audioFile.Init("Audio/Healingwounds.mp3", false);
	
	myAudioCollection[eAudioEvent::CompanionHealing2] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionHealing2].audioFile.Init("Audio/AllBetterNow.mp3", false);

	myAudioCollection[eAudioEvent::CompanionRevive] = { false, eAudioType::SFX, DreamEngine::Audio() };
	myAudioCollection[eAudioEvent::CompanionRevive].audioFile.Init("Audio/WelcomeBackAgent.wav", false);
	
	myAudioCollection[eAudioEvent::CompanionTurretMode] = { false, eAudioType::SFX, DreamEngine::Audio() };
	myAudioCollection[eAudioEvent::CompanionTurretMode].audioFile.Init("Audio/TurretmodeActivated.wav", false);
	
	myAudioCollection[eAudioEvent::CompanionFetch] = { false, eAudioType::SFX, DreamEngine::Audio() };
	myAudioCollection[eAudioEvent::CompanionFetch].audioFile.Init("Audio/Activate_FetchHealthPack.wav", false);

	myAudioCollection[eAudioEvent::CompanionDrillsBeingAttackedA1] = { false, eAudioType::SFX, DreamEngine::Audio() };
	myAudioCollection[eAudioEvent::CompanionDrillsBeingAttackedA1].audioFile.Init("Audio/DrillAIsUnderAttack_done.wav", false);

	myAudioCollection[eAudioEvent::CompanionDrillsBeingAttackedA2] = { false, eAudioType::SFX, DreamEngine::Audio() };
	myAudioCollection[eAudioEvent::CompanionDrillsBeingAttackedA2].audioFile.Init("Audio/TheyAreAttackingA_done.wav", false);

	myAudioCollection[eAudioEvent::CompanionDrillsBeingAttackedB1] = { false, eAudioType::SFX, DreamEngine::Audio() };
	myAudioCollection[eAudioEvent::CompanionDrillsBeingAttackedB1].audioFile.Init("Audio/DrillBIsUnderAttack_done.wav", false);

	myAudioCollection[eAudioEvent::CompanionDrillsBeingAttackedB2] = { false, eAudioType::SFX, DreamEngine::Audio() };
	myAudioCollection[eAudioEvent::CompanionDrillsBeingAttackedB2].audioFile.Init("Audio/TheyAreAttackingB_done.wav", false);

	myAudioCollection[eAudioEvent::CompanionDrillsBeingAttackedC1] = { false, eAudioType::SFX, DreamEngine::Audio() };
	myAudioCollection[eAudioEvent::CompanionDrillsBeingAttackedC1].audioFile.Init("Audio/DrillCIsUnderAttack_done.wav", false);

	myAudioCollection[eAudioEvent::CompanionDrillsBeingAttackedC2] = { false, eAudioType::SFX, DreamEngine::Audio() };
	myAudioCollection[eAudioEvent::CompanionDrillsBeingAttackedC2].audioFile.Init("Audio/TheyAreAttackingC_done.wav", false);

	myAudioCollection[eAudioEvent::CompanionVL1] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL1].audioFile.Init("Audio/Erradicating.wav", false);

	myAudioCollection[eAudioEvent::CompanionVL2] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL2].audioFile.Init("Audio/LaughTheBestWhoLaughsTheSisst.wav", false);
	
	myAudioCollection[eAudioEvent::CompanionVL3] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL3].audioFile.Init("Audio/AsYouBeddarYouMayLigga.wav", false);
	
	myAudioCollection[eAudioEvent::CompanionVL4] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL4].audioFile.Init("Audio/EyeForAnEyeToothForATooth.wav", false);
	
	myAudioCollection[eAudioEvent::CompanionVL5] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL5].audioFile.Init("Audio/Laughing.wav", false);
	
	myAudioCollection[eAudioEvent::CompanionVL6] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL6].audioFile.Init("Audio/HiddenMvp.wav", false);
	
	myAudioCollection[eAudioEvent::CompanionVL7] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL7].audioFile.Init("Audio/HiddenMvp.mp3", false);
	
	myAudioCollection[eAudioEvent::CompanionVL8] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL8].audioFile.Init("Audio/Hopsan.mp3", false);

	myAudioCollection[eAudioEvent::CompanionVL9] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL9].audioFile.Init("Audio/HurkelDurkel.mp3", false);
	
	myAudioCollection[eAudioEvent::CompanionVL10] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL10].audioFile.Init("Audio/Laughing.mp3", false);
	
	myAudioCollection[eAudioEvent::CompanionVL11] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL11].audioFile.Init("Audio/LieDown.mp3", false);	
	
	myAudioCollection[eAudioEvent::CompanionVL12] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL12].audioFile.Init("Audio/LookOutFor.mp3", false);
	
	myAudioCollection[eAudioEvent::CompanionVL13] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL13].audioFile.Init("Audio/Missing.mp3", false);
	
	myAudioCollection[eAudioEvent::CompanionVL14] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL14].audioFile.Init("Audio/Ne.mp3", false);
	
	myAudioCollection[eAudioEvent::CompanionVL15] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL15].audioFile.Init("Audio/NevertooLateSwedish.mp3", false);
	
	myAudioCollection[eAudioEvent::CompanionVL16] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL16].audioFile.Init("Audio/NeverTooLateTooGiveUp.mp3", false);
	
	myAudioCollection[eAudioEvent::CompanionVL17] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL17].audioFile.Init("Audio/Oj.mp3", false);
	
	myAudioCollection[eAudioEvent::CompanionVL18] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL18].audioFile.Init("Audio/Peesoup.mp3", false);
	
	myAudioCollection[eAudioEvent::CompanionVL19] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL19].audioFile.Init("Audio/Pungratta.mp3", false);
	
	myAudioCollection[eAudioEvent::CompanionVL20] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL20].audioFile.Init("Audio/RubbStubb.mp3", false);
	
	myAudioCollection[eAudioEvent::CompanionVL21] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL21].audioFile.Init("Audio/Tennis.mp3", false);
	
	myAudioCollection[eAudioEvent::CompanionVL22] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL22].audioFile.Init("Audio/Trolls.mp3", false);
	
	myAudioCollection[eAudioEvent::CompanionVL23] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL23].audioFile.Init("Audio/WhenItCrayon.mp3", false);
	
	myAudioCollection[eAudioEvent::CompanionVL24] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::CompanionVL24].audioFile.Init("Audio/Woopwoop.mp3", false);

	myAudioCollection[eAudioEvent::PlayerActivateGrapple] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::PlayerActivateGrapple].audioFile.Init("Audio/Player_ActivateGrapplingHook.wav", false);

	myAudioCollection[eAudioEvent::PlayerReelInGrapple] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::PlayerReelInGrapple].audioFile.Init("Audio/ReelingInGrapplingHook.wav", false);
	
	myAudioCollection[eAudioEvent::PlayerReload1] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::PlayerReload1].audioFile.Init("Audio/Gun_Reload2.wav", false);
	
	myAudioCollection[eAudioEvent::PlayerReload2] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::PlayerReload2].audioFile.Init("Audio/Gun_Reload3.wav", false);

	myAudioCollection[eAudioEvent::PlayerDeath] = { false, eAudioType::SFX, DreamEngine::Audio() };
	myAudioCollection[eAudioEvent::PlayerDeath].audioFile.Init("Audio/Player_Death.wav", false);

	myAudioCollection[eAudioEvent::PlayerRespawn] = { false, eAudioType::SFX, DreamEngine::Audio() };
	myAudioCollection[eAudioEvent::PlayerRespawn].audioFile.Init("Audio/Player_Revive.wav", false);

	myAudioCollection[eAudioEvent::PlayerHeal] = { false, eAudioType::SFX, DreamEngine::Audio() };
	myAudioCollection[eAudioEvent::PlayerHeal].audioFile.Init("Audio/Player_Heal.wav", false);
	
	myAudioCollection[eAudioEvent::PlayerShootRocket] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::PlayerShootRocket].audioFile.Init("Audio/Player_ShootRocket.wav", false);
	
	myAudioCollection[eAudioEvent::PlayerShootBeam] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::PlayerShootBeam].audioFile.Init("Audio/Gun_ShootBeam.wav", true);
	
	myAudioCollection[eAudioEvent::PlayerGooSplat] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::PlayerGooSplat].audioFile.Init("Audio/Gun_GooSplat.wav", false);

	myAudioCollection[eAudioEvent::PlayerGooBounce] = { false, eAudioType::SFX, DreamEngine::Audio() };
	myAudioCollection[eAudioEvent::PlayerGooBounce].audioFile.Init("Audio/Gun_GooBounce.wav", false);

	myAudioCollection[eAudioEvent::PlayerShootGoo1] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::PlayerShootGoo1].audioFile.Init("Audio/Gun_ShootGoo1.wav", false);
	
	myAudioCollection[eAudioEvent::PlayerShootGoo2] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::PlayerShootGoo2].audioFile.Init("Audio/Gun_ShootGoo2.wav", false);
	
	myAudioCollection[eAudioEvent::Explosion1] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::Explosion1].audioFile.Init("Audio/Gun_RocketExplosion.wav", false);
	
	myAudioCollection[eAudioEvent::Explosion2] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::Explosion2].audioFile.Init("Audio/Explosion.wav", false);
	
	myAudioCollection[eAudioEvent::DrillingA] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::DrillingA].audioFile.Init("Audio/Drilling.wav", true);
	
	myAudioCollection[eAudioEvent::DrillingB] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::DrillingB].audioFile.Init("Audio/Drilling.wav", true);
	
	myAudioCollection[eAudioEvent::DrillingC] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::DrillingC].audioFile.Init("Audio/Drilling.wav", true);
	
	myAudioCollection[eAudioEvent::GroundEnemyAttack1] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::GroundEnemyAttack1].audioFile.Init("Audio/GroundEnemy_Attack1.wav", false);

	myAudioCollection[eAudioEvent::GroundEnemyAttack2] = { false, eAudioType::SFX, DreamEngine::Audio() };
	myAudioCollection[eAudioEvent::GroundEnemyAttack2].audioFile.Init("Audio/GroundEnemy_Attack2.wav", false);
	
	myAudioCollection[eAudioEvent::GroundEnemyDie] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::GroundEnemyDie].audioFile.Init("Audio/GroundEnemy_Death.wav", false);
	
	myAudioCollection[eAudioEvent::NextWaveIncoming] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::NextWaveIncoming].audioFile.Init("Audio/Environment_WaveSpawnWarning.wav", false);
	
	myAudioCollection[eAudioEvent::StartGame] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::StartGame].audioFile.Init("Audio/Environment_GameStart.wav", false);	
	
	myAudioCollection[eAudioEvent::PurchaseUpgrade] = {false, eAudioType::SFX, DreamEngine::Audio()};
	myAudioCollection[eAudioEvent::PurchaseUpgrade].audioFile.Init("Audio/Environment_UpgradeStationPurchase.wav", false);

	myAudioCollection[eAudioEvent::MenuClick] = { false, eAudioType::SFX, DreamEngine::Audio() };
	myAudioCollection[eAudioEvent::MenuClick].audioFile.Init("Audio/Menu_ButtonClick.wav", false);

}

void AudioManager::PlayAudio(eAudioEvent anAudioEvent, DE::Vector3f anOriginPosition, float aPositionX)
{
	return;
	myAudioCollection[anAudioEvent].audioFile.SetPosition(aPositionX);
	if(!myAudioCollection[anAudioEvent].isPlaying)
	{
		float volumeDistanceScalar = 1.0f;
		if (myAudioCollection[anAudioEvent].audioType == eAudioType::SFX)
		{
			DE::Vector3f playerPos = MainSingleton::GetInstance()->GetPlayer() != nullptr ? MainSingleton::GetInstance()->GetPlayer()->GetTransform()->GetPosition() : DE::Vector3f(0.0f);
			float distToPlayer = DE::Vector3f(playerPos - anOriginPosition).Length();
			distToPlayer *= -1.0f;
			distToPlayer = UtilityFunctions::Max(0.0f, distToPlayer + 2000.0f);
			volumeDistanceScalar = UtilityFunctions::Lerp(0.0f, 1.0f, distToPlayer / 2000.0f);
		}
		float volume = myMainVolume * volumeDistanceScalar;
		volume *= myIsMuted ? 0.0f : 1.0f;
		myAudioCollection[anAudioEvent].audioType == eAudioType::SFX ? volume *= mySFXVolume : volume *= myMusicVolume;
		myAudioCollection[anAudioEvent].audioFile.Play(volume);
		myAudioCollection[anAudioEvent].isPlaying = true;
	}
}

void AudioManager::StopAudio(eAudioEvent anAudioEvent)
{
	if(myAudioCollection[anAudioEvent].isPlaying)
	{
		myAudioCollection[anAudioEvent].audioFile.Stop();
		myAudioCollection[anAudioEvent].isPlaying = false;
	}
}

void AudioManager::UpdateAudioVolume(eAudioEvent anAudioEvent, DE::Vector3f anOriginPosition)
{
	if (!myAudioCollection[anAudioEvent].isPlaying)
	{
		PlayAudio(anAudioEvent, anOriginPosition);
	}
	float volumeDistanceScalar = 1.0f;
	if (myAudioCollection[anAudioEvent].audioType == eAudioType::SFX)
	{
		DE::Vector3f playerPos = MainSingleton::GetInstance()->GetPlayer() != nullptr ? MainSingleton::GetInstance()->GetPlayer()->GetTransform()->GetPosition() : DE::Vector3f(0.0f);
		float distToPlayer = DE::Vector3f(playerPos - anOriginPosition).Length();
		distToPlayer *= -1.0f;
		distToPlayer = UtilityFunctions::Max(0.0f, distToPlayer + 2000.0f);
		volumeDistanceScalar = UtilityFunctions::Lerp(0.0f, 1.0f, distToPlayer / 2000.0f);
	}
	float volume = myMainVolume * volumeDistanceScalar;
	volume *= myIsMuted ? 0.0f : 1.0f;
	myAudioCollection[anAudioEvent].audioType == eAudioType::SFX ? volume *= mySFXVolume : volume *= myMusicVolume;
	myAudioCollection[anAudioEvent].audioFile.SetVolume(volume);
}

void AudioManager::SetMainVolume(float aVolume)
{
	myMainVolume = aVolume;
}

void AudioManager::SetSFXVolume(float aVolume)
{
	mySFXVolume = aVolume;
}

void AudioManager::SetMusicVolume(float aVolume)
{
	myMusicVolume = aVolume;
}

void AudioManager::StopNonMenuAudio()
{
	for (auto& audioInstance : myAudioCollection)
	{
		if (audioInstance.first != eAudioEvent::MainMenuMusic && audioInstance.first != eAudioEvent::MenuClick)
		{
			audioInstance.second.audioFile.Stop();
			audioInstance.second.isPlaying = false;
		}
	}
}

void AudioManager::UpdateMusicClipsVolume()
{
	for (auto& audioInstance : myAudioCollection)
	{
		if (audioInstance.second.isPlaying && audioInstance.second.audioType == eAudioType::Music)
		{
			float volume = myMainVolume;
			volume *= myIsMuted ? 0.0f : 1.0f;
			volume *= myMusicVolume;
			audioInstance.second.audioFile.SetVolume(volume);
		}
	}
}

void AudioManager::UpdateSFXClipsVolume()
{
	for (auto& audioInstance : myAudioCollection)
	{
		if (audioInstance.second.isPlaying && audioInstance.second.audioType == eAudioType::SFX)
		{
			float volume = myMainVolume;
			volume *= myIsMuted ? 0.0f : 1.0f;
			volume *= mySFXVolume;
			audioInstance.second.audioFile.SetVolume(volume);
		}
	}
}

void AudioManager::ToggleMute()
{
	myIsMuted = !myIsMuted;
}