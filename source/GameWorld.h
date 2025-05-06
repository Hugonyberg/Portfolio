#pragma once
#include <DreamEngine/graphics/Camera.h>
#include "ParticleSystem.h"
#include "ParticleSystem3D.h"
#include "ParticleManager.h"
#include "BinaryExporter.h"
#include "Observer.h"
#include "Scene.h"
#include "Mouse.h"
#include "RenderCommands.h"

namespace physx
{
	class PxScene;
	class PxDefaultCpuDispatcher;
	class PxMaterial;
	class PxControllerManager;
	class PxRigidActor; 
}

class StateStack;
class State;
class CollisionFiltering;

class GameWorld : public Observer
{
public:

	GameWorld(); 
	~GameWorld();
	void CleanUpPhysics();

	void Init();
	bool Update(float aDeltaTime);
	void Render();

	void Receive(const Message& aMsg) override; 

	bool GameShouldQuit() const;
	bool GameShouldChangeResolution(); 
	DE::Vector2ui GetResolution();
	void ResetResolutionBool(); 
private:
	Scene* myScene = nullptr;
	std::shared_ptr<StateStack> myStateStack;
	State* myState;

	physx::PxScene* myPhysXScene = NULL;
	physx::PxDefaultCpuDispatcher* myPhysXDispatcher = NULL;
	physx::PxMaterial* myDefaultPhysXMaterial = NULL;
	physx::PxControllerManager* myPhysXControllerManager = NULL;

	CollisionFiltering* myCollisionFiltering; 
	SimulationEventCallback mySimulationEventCallback;

	HUDLoader* myHud; //this needs to be here because of threading 
	Mouse* myMouse; 
	BinaryExporter myBinaryExport;

	bool myStatestackInit;
	bool myQuitGame;
	bool myChangeResolution; 
};