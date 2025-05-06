#pragma once
#include <DreamEngine\utilities\InputManager.h>
#include <DreamEngine\graphics\Camera.h>
#include "InputMapper.h"
#include "PostMaster.h"
#include "EnemyFactory.h"
#include "AudioManager.h"
#include "PxCollisionFiltering.h"
#include "RenderCommands.h"
#include "Player.h"
#include "ShaderTool.h"

static bool ourVisualizeCollidersFlag = false;

namespace physx
{
	class PxControllerManager;
	class PxScene;
}

class StateStack;
class Minimap;

class MainSingleton
{
public:

	static bool Start();
	~MainSingleton() = default;

	static MainSingleton* GetInstance() { return myInstance; };
	int GetNewID() { return myLastID++; };
	DE::InputManager& GetInputManager() { return myInputManager; };
	InputMapper& GetInputMapper() { return myInputMapper; };
	PostMaster& GetPostMaster() { return myPostMaster; }
	const EnemyFactory& GetEnemyFactory() { return myEnemyFactory; };
	AudioManager& GetAudioManager() { return myAudioManager; }

	physx::PxControllerManager* GetPhysXControllerManager() { return myPhysXControllerManager; }
	void SetPhysXControllerManager(physx::PxControllerManager* aManager) { myPhysXControllerManager = aManager; }	
	physx::PxScene* GetPhysXScene() { return myPhysXScene; }
	void SetPhysXScene(physx::PxScene* aScene) { myPhysXScene = aScene; }
	std::vector<physx::PxMaterial*>& GetPhysXMaterials() { return myPhysXMaterials; }
	void AddPhysXMaterial(physx::PxMaterial* aMaterial) { myPhysXMaterials.push_back(aMaterial); }

	std::shared_ptr<DE::Camera> GetActiveCamera() { return myActiveCamera; }
	void SetActiveCamera(std::shared_ptr<DE::Camera> aCamera) { myActiveCamera = aCamera; }

	const bool& GetWasActiveWindowLastFrame() { return myWasActiveWindowLastFrame; }
	void SetWasActiveWindowLastFrame(bool wasActive) { myWasActiveWindowLastFrame = wasActive; }

	const bool& GetFullScreenBool() { return myFullScreenIsActive; }
	void SetFullScreenBool(bool isWindowFullscreen) { myFullScreenIsActive = isWindowFullscreen; }

	const int& GetCurrentLevelNum() { return myCurrentLevelNum; }
	void SetCurrentLevelNum(int aLevelNum) { myCurrentLevelNum = aLevelNum; }
	
	const DE::Vector2f& GetMousePos() { return myMousePos; }
	void SetMousePos(DE::Vector2f aMousePos) { myMousePos = aMousePos; }

	const bool& GetShouldRenderCursor() { return myShouldRenderCursor; }
	void SetShouldRenderCursor(bool aShouldRenderCursor) { myShouldRenderCursor = aShouldRenderCursor; }

	const bool& GetShouldCaptureCursor() { return myShouldCaptureCursor; }
	void SetShouldCaptureCursor(bool aShouldCaptureCursor) { myShouldCaptureCursor = aShouldCaptureCursor; }

	DE::Vector2ui& GetResolution() { return myResolution; }
	void SetResolution(DE::Vector2ui aResolution) { myResolution = aResolution; }

	DE::Vector2ui GetScreenSize() { return myScreenSize; } 
	void SetScreenSize(DE::Vector2ui aScreenSize) { myScreenSize = aScreenSize; } 

	bool& GetTheradOnEngine() { return myTheadIsOnEngine; }
	void SetThreadOnEnigne(bool aThreadOnEngine) { myTheadIsOnEngine = aThreadOnEngine; }

	bool& GetGameToPause() { return myPauseGame; }
	void SetGameToPause(bool aAction) { myPauseGame = aAction; }

	bool& IsInCutscene() { return myIsInCutscene; }
	void SetIsInCutscene(bool anIsInCutsceneBool) { myIsInCutscene = anIsInCutsceneBool; }

	bool& GetInGame() { return myInGame; }
	void SetInGame(bool aIsInGame) { myInGame = aIsInGame; }

	bool& GetLastStateWasSettings() { return myLastStateWasSettings; }
	void SetLastStateWasSettings(bool aWasSettings) { myLastStateWasSettings = aWasSettings; }

	int& GetIconID() { return myIconId; }
	void SetIconID(int aID) { myIconId = aID; }
	
	float& GetFOV() { return myFOV; }
	void SetFOV(float anFOV) { myFOV = anFOV; }

	void SetPlayer(std::shared_ptr<Player> aPlayer) { myPlayer = aPlayer; };
	std::shared_ptr<Player> GetPlayer() { return myPlayer; };

	void SetMinimap(Minimap* aMinimap) { myMinimap = aMinimap; };
	Minimap* GetMinimap() { return myMinimap; };

	CollisionFiltering GetCollisionFiltering() { return *myCollisionFiltering; }
	void SetCollisionFiltering(CollisionFiltering* aPxCollisionFiltering) { myCollisionFiltering = aPxCollisionFiltering; } 

	std::vector<RenderData3D>* GetLogicCommands() { return &myLogicCommands; };
	std::vector<RenderData3D>* GetRenderCommands() { return &myRenderCommands; };

	std::vector<AnimatedRenderData3D>* GetAnimatedLogicCommands() { return &myAnimatedLogic; };
	std::vector<AnimatedRenderData3D>* GetAnimatedRenderCommands() { return &myAnimatedRender; };

	ShaderTool& GetShaderTool() { return myShaderTool; };
	std::array<DE::SpriteSharedData, 7>& GetMinimapSprites() { return myMinimapSprites; }

	void SwapModelBuffers();

	void SetStateStack(std::shared_ptr<StateStack> aStateStack) { myGameWorldStateStack = aStateStack; };
	std::shared_ptr<StateStack> GetStateStack() { return myGameWorldStateStack; };

private:
	MainSingleton() {};
	bool InternalStart();
	static MainSingleton* myInstance;
	int myLastID = 0;

	int myIconId = -1;

	DE::InputManager myInputManager;
	InputMapper myInputMapper;
	PostMaster myPostMaster;

	AudioManager myAudioManager;
	CollisionFiltering* myCollisionFiltering; 
	physx::PxControllerManager* myPhysXControllerManager;
	physx::PxScene* myPhysXScene;
	std::vector<physx::PxMaterial*> myPhysXMaterials;
	std::vector<RenderData3D> myLogicCommands;
	std::vector<RenderData3D> myRenderCommands;
	std::vector<AnimatedRenderData3D> myAnimatedLogic;
	std::vector<AnimatedRenderData3D> myAnimatedRender;
	std::array<DE::SpriteSharedData, 7> myMinimapSprites;

	std::shared_ptr<StateStack> myGameWorldStateStack;

	ShaderTool myShaderTool;

	std::shared_ptr<DE::Camera> myActiveCamera = nullptr;

	DE::Vector2f myMousePos; 
	DE::Vector2ui myResolution = { 1920, 1080 };
	DE::Vector2ui myScreenSize = { 1920, 1080 };

	EnemyFactory myEnemyFactory;

	std::shared_ptr<Player> myPlayer = nullptr;

	Minimap* myMinimap = nullptr;

	int myCurrentLevelNum = 1;
	bool myTheadIsOnEngine;
	bool myWasActiveWindowLastFrame = true;
	bool myFullScreenIsActive;
	bool myPauseGame;
	bool myInGame = false;
	bool myIsInCutscene = false;
	bool myShouldRenderCursor = true;
	bool myShouldCaptureCursor = false;
	bool myLastStateWasSettings = false;
	float myFOV = 105.0f;
};