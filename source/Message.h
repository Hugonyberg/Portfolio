#pragma once
#include <string>

enum class eMessageType
{
	////////// PLAYER //////////
	PlayerLookAround,
	PlayerMove,
	PlayerSprint,
	PlayerJump,
	BoatMove,
	TogglePauseMenu,
	ActivateIntroMenu,
	ResetIntroMenu,
	PlayerStartVFX,
	PlayerStopVFX,
	PlayerDied,
	PlayerRespawned,
	PlayerInteract,

	////////// UI/Buttons //////////
	StartLevelScene,
	MainMenuScene,
	LevelSelectScene,
	SettingsScene,
	CreditsScene,
	AssetGymScene,
	PlayerGymScene,
	ExitProgram,

	LevelOne,
	LevelTwo,
	LoadingScreen,
	Return,
	Resume,
	DeactivateIntroMenu,

	Mute,
	MainVolume,
	SfxVolume,
	MusicVolume,
	FOV,
	Resolution1280x720,
	Resolution1920x1080,
	Resolution2560x1440,
	FullScreen,

	////////// HUD //////////
	ShowHud,
	BoatTransformUpdate,
	PlayerHealthChange,
	PlayerAmmoCountUpdated,
	PlayerCurrencyChange,
	PlayerGrappleCooldownStateChanged,
	CompanionTurretCooldownToggle,
	CompanionHealthCooldownToggle,
	DefensePointAToggle,
	DefensePointAHealthChanged,
	DefensePointBToggle,
	DefensePointBHealthChanged,
	DefensePointCToggle,
	DefensePointCHealthChanged,
	EnemySpawn1Toggle,
	EnemySpawn2Toggle,
	EnemySpawn3Toggle,
	WaveNumberChanged,
	EnemyNumberChanged,
	TriggerMissionFailed,
	TriggerMissionAccomplished,
	TriggerWaveIncoming,
	SetCrosshairState,

	////////// Enemy //////////
	EnemyTakeDamage,

	////////// TriggerBox :) //////////
	GameCompleted,
	
	////////// Particle //////////
	SpawnParticle,

	////////// System //////////
	ChangeResolution, 
	Reset, 

	////////// GameLoop //////////
	LevelCompleted,

	DefaultTypeAndCount
};

struct Message
{
	void* messageData = nullptr;
	eMessageType messageType = eMessageType::DefaultTypeAndCount;
};