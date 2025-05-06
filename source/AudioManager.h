#pragma once
#include <DreamEngine\audio\audio.h>
#include <unordered_map>
#include <utility>
#include <DreamEngine\math\Vector3.h>

enum class eAudioEvent
{
	MainMenuMusic,
	Level1Music,
	Level2Music,
	
	CompanionIntroduction,
	CompanionHealing1,
	CompanionHealing2,
	CompanionRevive,
	CompanionTurretMode,
	CompanionFetch,
	CompanionDrillsBeingAttackedA1,
	CompanionDrillsBeingAttackedB1,
	CompanionDrillsBeingAttackedC1,
	CompanionDrillsBeingAttackedA2,
	CompanionDrillsBeingAttackedB2,
	CompanionDrillsBeingAttackedC2,
	CompanionVL1,
	CompanionVL2,
	CompanionVL3,
	CompanionVL4,
	CompanionVL5,
	CompanionVL6,
	CompanionVL7,
	CompanionVL8,
	CompanionVL9,
	CompanionVL10,
	CompanionVL11,
	CompanionVL12,
	CompanionVL13,
	CompanionVL14,
	CompanionVL15,
	CompanionVL16,
	CompanionVL17,
	CompanionVL18,
	CompanionVL19,
	CompanionVL20,
	CompanionVL21,
	CompanionVL22,
	CompanionVL23,
	CompanionVL24,
	CompanionShoot,

	PlayerShootBeam,
	PlayerShootRocket,
	PlayerActivateGrapple,
	PlayerReelInGrapple,
	PlayerGooSplat,
	PlayerGooBounce,
	PlayerShootGoo1,
	PlayerShootGoo2,
	PlayerReload1,
	PlayerReload2,
	PlayerDeath,
	PlayerRespawn,
	PlayerHeal,

	Explosion1,
	Explosion2,

	DrillingA,
	DrillingB,
	DrillingC,

	GroundEnemyAttack1,
	GroundEnemyAttack2,
	GroundEnemyDie,

	NextWaveIncoming,
	StartGame,
	PurchaseUpgrade,

	MenuClick,

	DefaultTypeAndCount
};

enum class eAudioType
{
	SFX,
	Music,
	DefaultTypeAndCount
};

struct AudioInstance
{
	bool isPlaying = false;
	eAudioType audioType = eAudioType::DefaultTypeAndCount;
	DreamEngine::Audio audioFile;
};

class AudioManager
{
public:
	AudioManager();
	~AudioManager() = default;

	void PlayAudio(eAudioEvent anAudioEvent, DE::Vector3f anOriginPosition, float aPositionX = 0);
	void StopAudio(eAudioEvent anAudioEvent);
	void UpdateAudioVolume(eAudioEvent anAudioEvent, DE::Vector3f anOriginPosition);

	void SetMainVolume(float aVolume = 1.0f);
	void SetSFXVolume(float aVolume = 1.0f);
	void SetMusicVolume(float aVolume = 1.0f);

	void StopNonMenuAudio();
	void UpdateMusicClipsVolume();
	void UpdateSFXClipsVolume();
	void ToggleMute();

private:
	std::unordered_map<eAudioEvent, AudioInstance> myAudioCollection;
	float myMainVolume = 0.75f;
	float mySFXVolume = 0.75f;
	float myMusicVolume = 0.75f;
	bool myIsMuted = false;
};