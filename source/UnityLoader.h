#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <DreamEngine/math/Transform.h>
#include <DreamEngine/math/color.h>
#include "ColliderCommon.h"
#include "DreamEngine/graphics/TextureManager.h"

struct MainCamera
{
	DreamEngine::Transform transform;
	float foV;
	float farP;
	float nearP;
};

struct MainDirectionalLight
{
	DreamEngine::Transform transform;
	DreamEngine::Color color;
	float intensity;
};

struct PointLightData
{
	DreamEngine::Transform transform;
	DreamEngine::Color color;
	float intensity;
	float range;
};

struct SpotLightData
{
	DreamEngine::Transform transform;
	DreamEngine::Color color;
	float intensity;
	float range;
};

struct ColliderData 
{
	eColliderType type;
	DE::Vector3f size;
	DE::Vector3f offset;
	DE::Vector3f scale;
	std::vector<eCollisionLayer> includeLayers;
	std::vector<eCollisionLayer> excludeLayers;
	MeshData meshData;
	float radius;
	int layerOverride;
	bool isTrigger;
};

struct UIInstanceData
{
	DreamEngine::Vector2f position{}; 
	DreamEngine::Vector2f pivot{}; 
	DreamEngine::Vector2f scale{}; 
};

enum class eUIButton
{
	MainMenu,
	LevelSelect,
	Settings,
	Credits,
	Exit,

	Mute,
	MainVolume,
	SfxVolume,
	MusicVolume,
	FOV,
	ScreenSizeFull,
	Resolution1280x720,
	Resolution1920x1080,
	Resolution2560x1440,

	LevelOne,
	LevelTwo,

	PlayerGym,
	AssetGym,

	Back,

	Resume,
	IntroAccept,

	Count
};

enum class eHUDAction
{
	HealthBackground,
	HealthForeground,
	MoneyCurrencyText,
	MoneyCurrencySprite,
	GrapplingHookIndicator,

	AmmoForeground,
	AmmoBackground,
	AmmoNumber,

	CompanionTurretCommand,
	CompanionTurretCommandKeySprite,
	CompanionHealthCommand,
	CompanionHealthCommandKeySprite,

	Crosshair,

	TopLeftTextBackgroundSquare,
	DefensePointA,
	DefensePointB,
	DefensePointC,
	EnemySpawn1,
	EnemySpawn2,
	EnemySpawn3,
	WaveNumberCounter,
	EnemyAmountCounter,

	ScreenCenterText,

	ControlsView
};

struct LeviathanInfo
{
	float attackCoolDown;
	float attackingSpeed;
	float attackRange;
	float seekingSpeed;
	float seekRange;
	std::map<int, DE::Vector3f> waypoints;
};

struct EnemySpawnerInfo
{
	int amountShooting;
	int amountMelee;
	bool isShooting;
	bool isMelee;
	float spawnerRange; 
};


struct MapExit
{
	DreamEngine::Vector3f exitPoint; 
	DreamEngine::Vector3f spawnPoint; 
	int biomeNumberToSwapTo;
	int spawnPointInOtherBiome;
	int spawnPointNumber;
};

struct LevelData // Dont change before telling Alexander when you change it!!!!!!!!!!!!!!!!!!!!!!!!
{
	MainCamera mainCamera;
	MainDirectionalLight directionalLight;
	std::string levelName;
	std::unordered_map<int, std::string> names;
	std::unordered_map<int, std::string> tags;
	std::unordered_map<int, DreamEngine::Transform> transforms;
	std::unordered_map<int, std::wstring> fbxPaths;
	std::unordered_map<int, ColliderData> colliders;
	std::unordered_map<int, SpotLightData> spotLights;
	std::unordered_map<int, PointLightData> pointLights;
	std::unordered_map<int, std::wstring> decalPaths;
	std::unordered_map<int, EnemySpawnerInfo> enemySpawners;
	LeviathanInfo leviathanInfo;
	std::unordered_map<int, std::string> boatIDs;
	std::unordered_map<int, std::string> boatModelPaths;
	std::unordered_map<int, DE::Vector3f> componentInteractionPoint;
	std::pair<DE::Vector2f, DE::Vector2f> gridBoundaries;
};

struct UIData // Alexander will now take these and make them binary as well. Wohoo. Also please tell him if anything gets changed.
{
	DreamEngine::Vector2f screenSize;
	std::unordered_map<int, std::string> tags;
	std::unordered_map<int, UIInstanceData> uIData;
	std::unordered_map<int, std::wstring> spritePaths;
	std::unordered_map<int, std::vector<int>> connectedSprites;
	std::unordered_map<int, eUIButton> UIbutton;
	std::unordered_map<int, eHUDAction> HUD;
};

struct VFXData
{
	//std::vector<std::vector<DE::Texture*>>;
};

class UnityLoader
{
public:
	static LevelData LoadLevel(const std::string& aFilePath);
	static UIData LoadUI(const std::string& aFilePath);

private:
	static void LoadData(const nlohmann::json& someInData, LevelData& someOutData, int& aID);
	static void LoadUIData(const nlohmann::json& aData, UIData& someData, int& aID);
	static DreamEngine::Vector4f GetVector4f(const nlohmann::json& aJson);
	static DreamEngine::Vector4f GetColor4f(const nlohmann::json& aJson);
	static DreamEngine::Vector3f GetVector3f(const nlohmann::json& aJson);
	static DreamEngine::Vector2f GetVector2f(const nlohmann::json& aJson);
	static std::wstring StringToWString(const std::string& str);
	static DreamEngine::Color GetColor(const nlohmann::json& aJson);
	static MeshData GetMeshData(const nlohmann::json& aJson);
};

