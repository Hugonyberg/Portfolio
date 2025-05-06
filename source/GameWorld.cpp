#include "stdafx.h"
#include "GameWorld.h"

#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/graphics/SpriteDrawer.h>
#include <DreamEngine/graphics/TextureManager.h>
#include <DreamEngine/debugging/DebugDrawer.h>
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/graphics/GraphicsStateStack.h>
#include "DreamEngine/engine.h"
#include <DreamEngine/graphics/DX11.h>
#include <PhysX\PxPhysicsAPI.h>
#include <PhysX\PxRigidActor.h> 
#include <fstream>
#include <iostream>
#include <memory>
#include <filesystem>

#include "MainSingleton.h"
#include "UnityLoader.h"
#include "StateStack.h"
#include "State.h"
#include "UIScene.h"
#include "PxCollisionFiltering.h"
#include "Minimap.h"


GameWorld::GameWorld()
{
	myStateStack = std::make_shared<StateStack>();
	MainSingleton::GetInstance()->SetStateStack(myStateStack);
	myMouse = new Mouse;
}

GameWorld::~GameWorld() 
{
	MainSingleton::GetInstance()->GetPostMaster().Unsubscribe(eMessageType::ChangeResolution, this);
	MainSingleton::GetInstance()->GetPostMaster().Unsubscribe(eMessageType::ExitProgram, this);
	delete myHud;
}

void GameWorld::CleanUpPhysics()
{
	PX_RELEASE(myPhysXScene);
	PX_RELEASE(myPhysXDispatcher);
}

void GameWorld::Init()  
{
	myChangeResolution = false;
	MainSingleton::GetInstance()->GetPostMaster().Subscribe(eMessageType::ChangeResolution, this);
	MainSingleton::GetInstance()->GetPostMaster().Subscribe(eMessageType::ExitProgram, this);

	unsigned int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	unsigned int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	auto& engine = *DreamEngine::Engine::GetInstance();
	engine.GetTextureManager().GetTexture(L"2D/Decal_DrillSite_A.png");
	engine.GetTextureManager().GetTexture(L"2D/Decal_DrillSite_B.png");
	engine.GetTextureManager().GetTexture(L"2D/Decal_DrillSite_C.png");
	engine.GetTextureManager().GetTexture(L"2D/Decal_Poster_Break_2.png");
	engine.GetTextureManager().GetTexture(L"2D/Decal_Poster_Break_3.png");
	engine.GetTextureManager().GetTexture(L"2D/Decal_Poster_Break_4.png");
	engine.GetTextureManager().GetTexture(L"2D/Decal_Poster_pest_2.png");
	engine.GetTextureManager().GetTexture(L"2D/Decal_Poster_pest_3.png");
	engine.GetTextureManager().GetTexture(L"2D/Decal_Poster_pest_4.png");
	engine.GetTextureManager().GetTexture(L"2D/Decal_Poster_set.png");
	engine.GetTextureManager().GetTexture(L"2D/Decal_Poster_shoot_2.png");
	engine.GetTextureManager().GetTexture(L"2D/Decal_Poster_shoot_3.png");
	engine.GetTextureManager().GetTexture(L"2D/Decal_Poster_shoot_4.png");
	engine.GetTextureManager().GetTexture(L"2D/decals_bluewarning01.png");
	engine.GetTextureManager().GetTexture(L"2D/decals_bluewarning02.png");
	engine.GetTextureManager().GetTexture(L"2D/decals_bluewarning03.png");
	engine.GetTextureManager().GetTexture(L"2D/Decals_Dirt01.png");
	engine.GetTextureManager().GetTexture(L"2D/Decals_Dirt02.png");
	engine.GetTextureManager().GetTexture(L"2D/Decals_DirtDrip01.png");
	engine.GetTextureManager().GetTexture(L"2D/Decals_DirtDrip02.png");
	engine.GetTextureManager().GetTexture(L"2D/Decals_DirtDrip03.png");
	engine.GetTextureManager().GetTexture(L"2D/Decals_DirtDrip04.png");
	engine.GetTextureManager().GetTexture(L"2D/Decals_Goop01.png");
	engine.GetTextureManager().GetTexture(L"2D/decals_greenwarning01.png");
	engine.GetTextureManager().GetTexture(L"2D/decals_greenwarning02.png");
	engine.GetTextureManager().GetTexture(L"2D/decals_greenwarning03.png");
	engine.GetTextureManager().GetTexture(L"2D/decals_redwarning01.png");
	engine.GetTextureManager().GetTexture(L"2D/decals_redwarning02.png");
	engine.GetTextureManager().GetTexture(L"2D/decals_redwarning03.png");
	engine.GetTextureManager().GetTexture(L"2D/decals_warning01.png");
	engine.GetTextureManager().GetTexture(L"2D/decals_warning02.png");
	engine.GetTextureManager().GetTexture(L"2D/decals_warning03.png");
	engine.GetTextureManager().GetTexture(L"2D/DecalTest.png");
	engine.GetTextureManager().GetTexture(L"2D/gloo.png");
	engine.GetTextureManager().GetTexture(L"2D/S_UI_SettingsSliderDragThing.png");
	engine.GetTextureManager().GetTexture(L"2D/S_Loading.png"); 
	engine.GetTextureManager().GetTexture(L"2D/S_UI_MinimapTexture.png"); 

	myQuitGame = false;
	myStateStack->PushEnum(State::eStateName::Splash);

	UIData data;
	data = UnityLoader::LoadUI(DreamEngine::Settings::ResolveAssetPath("Json/HUD.json"));

	myHud = new HUDLoader(data);
	myHud->Init(); 

	bool deactivationMessageData = false;
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &deactivationMessageData, eMessageType::ShowHud });

	myCollisionFiltering = new CollisionFiltering; 
	MainSingleton::GetInstance()->SetCollisionFiltering(myCollisionFiltering);

	// PhysX scene creation
	{
		physx::PxSceneDesc sceneDesc(DE::Engine::GetPhysXPhysics()->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f * 150.0f, 0.0f);
		myPhysXDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = myPhysXDispatcher;

		sceneDesc.kineKineFilteringMode = physx::PxPairFilteringMode::eKILL;
		sceneDesc.staticKineFilteringMode = physx::PxPairFilteringMode::eKILL;

		sceneDesc.filterShader = FilterShader;

		sceneDesc.simulationEventCallback = &mySimulationEventCallback; 

		sceneDesc.flags |= physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
		sceneDesc.flags &= ~physx::PxSceneFlag::eENABLE_STABILIZATION;
		sceneDesc.flags |= physx::PxSceneFlag::eENABLE_CCD;


		myPhysXScene = DE::Engine::GetPhysXPhysics()->createScene(sceneDesc);
		myPhysXControllerManager = PxCreateControllerManager(*myPhysXScene);
		MainSingleton::GetInstance()->SetPhysXControllerManager(myPhysXControllerManager);
		MainSingleton::GetInstance()->SetPhysXScene(myPhysXScene);

		physx::PxPvdSceneClient* pvdClient = myPhysXScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
		MainSingleton::GetInstance()->AddPhysXMaterial(DE::Engine::GetPhysXPhysics()->createMaterial(0.0f, 0.0f, 0.0f));
		MainSingleton::GetInstance()->AddPhysXMaterial(DE::Engine::GetPhysXPhysics()->createMaterial(0.5f, 0.5f, 0.0f));
		MainSingleton::GetInstance()->AddPhysXMaterial(DE::Engine::GetPhysXPhysics()->createMaterial(1.0f, 1.0f, 0.0f));

#ifdef extraPhysXObjs
		physx::PxReal stackZ = 10.0f;

		for (physx::PxU32 i = 0; i < 5; i++) 
		{
			physx::PxShape* shape = DE::Engine::GetPhysXPhysics()->createShape(physx::PxBoxGeometry(2.0f, 2.0f, 2.0f), *myDefaultPhysXMaterial);
			for (physx::PxU32 i = 0; i < 10; i++)
			{
				for (physx::PxU32 j = 0; j < 10 - i; j++)
				{
					physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(10 - i), physx::PxReal(i * 2 + 1), 0) * 2.0f);
					physx::PxRigidDynamic* body = DE::Engine::GetPhysXPhysics()->createRigidDynamic(physx::PxTransform(physx::PxVec3(0, 0, stackZ -= 10.0f)).transform(localTm));
					body->attachShape(*shape);
					physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
					myPhysXScene->addActor(*body);
				}
			}
			shape->release();
		}

		physx::PxRigidDynamic* dynamic = physx::PxCreateDynamic(*DE::Engine::GetPhysXPhysics(), physx::PxTransform(physx::PxVec3(0, 40, 100)), physx::PxSphereGeometry(10), *myDefaultPhysXMaterial, 10.0f);
		dynamic->setAngularDamping(0.5f);
		dynamic->setLinearVelocity(physx::PxVec3(0, -50, -100));
		myPhysXScene->addActor(*dynamic);
#endif
	}

	

	
}

bool GameWorld::Update(float aDeltaTime)
{
	if(aDeltaTime > 0.01666f)
	{
		aDeltaTime = 0.01666f;
	}
	if(MainSingleton::GetInstance()->GetGameToPause() && MainSingleton::GetInstance()->GetInGame())
	{
		aDeltaTime = 0.0f;
	}
	myPhysXScene->simulate(aDeltaTime);	// this never gets completed so fetchResults always returns false 
	myPhysXScene->fetchResults(true);

	if(myStateStack->Size() > 0)
	{
		myStateStack->GetCurrentState()->Update(aDeltaTime);
	}
	else if(myQuitGame)
	{
		CleanUpPhysics();
		return myQuitGame;
	}

	if (MainSingleton::GetInstance()->GetInGame()) 
	{
		myHud->Update(aDeltaTime);
		if (!MainSingleton::GetInstance()->IsInCutscene() && MainSingleton::GetInstance()->GetMinimap() != nullptr)
		{
			MainSingleton::GetInstance()->GetMinimap()->Update();
		}
	}

	myMouse->Update();

	return false;
}

void GameWorld::Render()
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	auto& graphicsEngine = engine.GetGraphicsEngine();
	auto& graphicsStateStack = graphicsEngine.GetGraphicsStateStack();

	if(myStateStack->Size() > 0)
	{
		myStateStack->GetCurrentState()->Render();
	}

	graphicsStateStack.Push(); 
	graphicsStateStack.SetDefaultCamera(); 

	if (MainSingleton::GetInstance()->GetInGame())
	{
		myHud->Render();
		if (MainSingleton::GetInstance()->GetMinimap() != nullptr)
		{
			MainSingleton::GetInstance()->GetMinimap()->Render();
		}
	}

	if (MainSingleton::GetInstance()->GetShouldRenderCursor())
	{
		myMouse->Render();
	}

	graphicsStateStack.Pop();
}

void GameWorld::Receive(const Message& aMsg)
{
	if(aMsg.messageType == eMessageType::ChangeResolution)
	{
		myChangeResolution = true;
	}
	else if (aMsg.messageType == eMessageType::ExitProgram)
	{
		myQuitGame = true;
	}
}

bool GameWorld::GameShouldQuit() const
{
	return myQuitGame;
}

bool GameWorld::GameShouldChangeResolution()
{
	return myChangeResolution; 
}

DE::Vector2ui GameWorld::GetResolution()
{
	auto resolution = MainSingleton::GetInstance()->GetScreenSize();
	return  resolution;
}

void GameWorld::ResetResolutionBool()
{
	myChangeResolution = false;
}