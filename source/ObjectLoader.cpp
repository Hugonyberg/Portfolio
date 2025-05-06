#include <DreamEngine/graphics/ModelFactory.h>
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/graphics/TextureManager.h>
#include <DreamEngine/graphics/ModelDrawer.h>
#include <DreamEngine/graphics/GraphicsStateStack.h>
#include <DreamEngine/graphics/GeometryBuffer.h>
#include <DreamEngine/graphics/DX11.h>
#include "DreamEngine/graphics/PointLight.h"
#include "DreamEngine/graphics/SpotLight.h"
#include "DreamEngine/graphics/DirectionalLight.h"
#include <DreamEngine/graphics/AmbientLight.h>
#include <DreamEngine/math/Intersection.h>
#include <DreamEngine/math/Collider.h>
#include <DreamEngine/engine.h>
#include <DreamEngine/windows/settings.h>
#include <DreamEngine/ThreadPool.h>

#include "JsonSoreter.hpp" 

#include "ObjectLoader.h"
#include "Setdressing.h"
#include "ColliderComponent.h"
#include "MainSingleton.h"
#include "Player.h"
#include "DeferredDecal.h"
#include "RigidBodyComponent.h"
#include "Blackboard.h"
#include "TextureLoading.h"
#include "StateStack.h"
#include "RenderCommands.h"
#include "EnemySpawner.h"
#include "Leviathan.h"

#include "Boat.h"

#include "WorldGrid.h"
#include "Heatmap.h"
#include "Minimap.h"

#include <thread>
#include <mutex>
#include <memory>

std::mutex loadingModelMutexa;
std::mutex pushingModelsa;

ObjectLoader::ObjectLoader(LevelData& aLevelData)
{
	DE::ThreadPool& updateThreadPool = DE::Engine::GetInstance()->GetUpdateThreadPool();
	ClearInstancersMap();
	mySelectionKeyHeldLenienceTimer = CU::CountdownTimer(0.25f);
	auto& modelFactory = DreamEngine::ModelFactory::GetInstance();
	myCullingRange = 45000.f;
	std::vector<int> setdressingIds = GetIdFromTag(aLevelData.tags, "Setdressing");

	std::vector<int> setdressingWithColliderIds = GetIdFromTag(aLevelData.tags, "SetdressingWithCollider");

	// To push transforms to batch
	for (int i = 0; i < myModelInstancer.size(); i++)
	{
		myModelInstancer[i]->RebuildInstances();
	}

	if (aLevelData.gridBoundaries.first.x < aLevelData.gridBoundaries.second.x)
	{
		float approximateCellScale = (aLevelData.gridBoundaries.second.x - aLevelData.gridBoundaries.first.x) / (float)worldGridIndexWidth;
		myWorldGrid = std::make_unique<WorldGrid>(aLevelData.gridBoundaries.first, aLevelData.gridBoundaries.second, approximateCellScale);

		myHeatmaps[static_cast<int>(eHeatmapType::EnemyMovement)] = std::make_unique<Heatmap>(aLevelData.gridBoundaries.first, aLevelData.gridBoundaries.second, static_cast<float>(eHeatmapCellScale::EnemyMovement), DE::Color(1.0f, 0.0f, 0.0f));
		myHeatmaps[static_cast<int>(eHeatmapType::LeviathanMovement)] = std::make_unique<Heatmap>(aLevelData.gridBoundaries.first, aLevelData.gridBoundaries.second, static_cast<float>(eHeatmapCellScale::LeviathanMovement), DE::Color(1.0f, 0.0f, 0.0f));
		myHeatmaps[static_cast<int>(eHeatmapType::BoatMovement)] = std::make_unique<Heatmap>(aLevelData.gridBoundaries.first, aLevelData.gridBoundaries.second, static_cast<float>(eHeatmapCellScale::BoatMovement), DE::Color(0.0f, 1.0f, 0.0f));
	}
	else
	{
		float approximateCellScale = (aLevelData.gridBoundaries.first.x - aLevelData.gridBoundaries.second.x) / (float)worldGridIndexWidth;
		myWorldGrid = std::make_unique<WorldGrid>(aLevelData.gridBoundaries.second, aLevelData.gridBoundaries.first, approximateCellScale);

		myHeatmaps[static_cast<int>(eHeatmapType::EnemyMovement)] = std::make_unique<Heatmap>(aLevelData.gridBoundaries.second, aLevelData.gridBoundaries.first, static_cast<float>(eHeatmapCellScale::EnemyMovement), DE::Color(1.0f, 0.0f, 0.0f));
		myHeatmaps[static_cast<int>(eHeatmapType::LeviathanMovement)] = std::make_unique<Heatmap>(aLevelData.gridBoundaries.second, aLevelData.gridBoundaries.first, static_cast<float>(eHeatmapCellScale::LeviathanMovement), DE::Color(1.0f, 0.0f, 0.0f));
		myHeatmaps[static_cast<int>(eHeatmapType::BoatMovement)] = std::make_unique<Heatmap>(aLevelData.gridBoundaries.second, aLevelData.gridBoundaries.first, static_cast<float>(eHeatmapCellScale::BoatMovement), DE::Color(0.0f, 1.0f, 0.0f));
	}
	Minimap* oldMinimap = MainSingleton::GetInstance()->GetMinimap();
	if (oldMinimap != nullptr) delete oldMinimap;
	myMinimap = new Minimap(myWorldGrid.get());
	MainSingleton::GetInstance()->SetMinimap(myMinimap);

	myMinimapCullingFSE = std::make_unique<DreamEngine::FullscreenEffect>();
	if (!myMinimapCullingFSE->Init("Shaders/MinimapCullingPS.cso"))
	{
		assert("Couldn't load MinimapCullingPS!");
	}

	
	std::vector<int> enemySpawnerIDs = GetIdFromTag(aLevelData.tags, "EnemySpawner");
	for (auto& enemySpawnerID : enemySpawnerIDs)
	{
		std::shared_ptr<EnemySpawner> spawner = std::make_shared<EnemySpawner>(aLevelData.enemySpawners[enemySpawnerID].spawnerRange);

		spawner->Init(	aLevelData.enemySpawners[enemySpawnerID].amountShooting,
						aLevelData.enemySpawners[enemySpawnerID].amountMelee,
						aLevelData.transforms[enemySpawnerID]);

		myEnemySpawners.push_back(spawner);
	}
	

	std::vector<int> leviathanID = GetIdFromTag(aLevelData.tags, "Leviathan");
	if (leviathanID.empty())
		MessageBox(NULL, L"Missing Leviathan in JSON", L"ERROR", MB_OK);
	else
	{
		myLeviathan = MainSingleton::GetInstance()->GetEnemyFactory().CreateLeviathan();
		myLeviathan->SetTransform(aLevelData.transforms[leviathanID[1]]);
		myLeviathan->SetIsActive(true);
		myLeviathan->SetModelInstance(std::make_shared<DreamEngine::ModelInstance>(DreamEngine::ModelFactory::GetInstance().GetModelInstance(L"3D/SM_CH_Leviathan.fbx")));
		
		myLeviathan->Init(	aLevelData.leviathanInfo.attackCoolDown,
					aLevelData.leviathanInfo.attackingSpeed,
					aLevelData.leviathanInfo.attackRange,
					aLevelData.leviathanInfo.seekingSpeed,
					aLevelData.leviathanInfo.seekRange);


		for (const auto& pair : aLevelData.leviathanInfo.waypoints) 
		{
			myLeviathan->SetWayPoints(pair.second);
		}
	}

	std::vector<int> spotLightIDs = GetIdFromTag(aLevelData.tags, "SpotLight");
	for (auto& spotLightID : spotLightIDs)
	{
		std::shared_ptr<DreamEngine::SpotLight> spotLight = std::make_shared<DreamEngine::SpotLight>();

		spotLight->SetTransform(aLevelData.transforms[spotLightID]);
		spotLight->SetColor(aLevelData.spotLights[spotLightID].color);
		spotLight->SetIntensity(aLevelData.spotLights[spotLightID].intensity);
		float range = aLevelData.spotLights[spotLightID].range;
		spotLight->SetRange(range);

		spotLight->myObjPtr = new DreamEngine::ModelInstance(modelFactory.GetModelInstance(L"3D/spherePrimitive.fbx"));
		spotLight->myObjPtr->SetTransform(spotLight->GetTransform());
		spotLight->myObjPtr->SetScale({ range, range, range });

		mySpotLights.push_back(spotLight);
	}

	std::vector<int> pointLightIDs = GetIdFromTag(aLevelData.tags, "PointLight");
			for (auto& pointLightID : pointLightIDs)
			{
			updateThreadPool.PushJob([&] 
				{
				});
					std::shared_ptr<DreamEngine::PointLight> pointLight = std::make_shared<DreamEngine::PointLight>();

					pointLight->SetTransform(aLevelData.transforms[pointLightID]);
					pointLight->SetColor(aLevelData.pointLights[pointLightID].color);
					pointLight->SetIntensity(aLevelData.pointLights[pointLightID].intensity);
					float range = aLevelData.pointLights[pointLightID].range;
					pointLight->SetRange(range);

					pointLight->myObjPtr = new DreamEngine::ModelInstance(modelFactory.GetModelInstance(L"3D/spherePrimitive.fbx"));
					pointLight->myObjPtr->SetTransform(pointLight->GetTransform());
					pointLight->myObjPtr->SetScale({ range, range, range });

					myPointLights.push_back(pointLight);
			}
		
	updateThreadPool.PushJob([&]
		{
		});
			{

			}
	updateThreadPool.PushJob([&]
		{
		});

			{ // Get Directional Light
				std::shared_ptr<DE::DirectionalLight> directionalLight = std::make_shared<DE::DirectionalLight>();

				directionalLight->SetTransform(aLevelData.directionalLight.transform);
				directionalLight->SetColor(aLevelData.directionalLight.color);
				directionalLight->SetIntensity(aLevelData.directionalLight.intensity);

				myDirectionalLight = directionalLight;
			}
	std::vector<int> cubemapIds = GetIdFromTag(aLevelData.tags, "Cubemap");
			for (auto& cubemapId : cubemapIds)
			{
			updateThreadPool.PushJob([&]
				{
				});

				std::wstring path = DreamEngine::Settings::ResolveAssetPathW(aLevelData.fbxPaths[cubemapId]);

				myAmbientLight = std::make_shared<DE::AmbientLight>(DreamEngine::AmbientLight(path, { 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f }, 1.0f));
			}

	InitEnvironment(aLevelData);
	InitBoat(aLevelData);
	// REMOVE SOME WHEN PLAYER IMPORT FROM UNITY
	myPlayer = std::make_shared<Player>();
	if (myBoat)	{ myPlayer->SetBoatPointer(myBoat); } // THIS BEFORE Point
	myPlayer->Init();
	// REMOVE SOME WHEN PLAYER IMPORT FROM UNITY

	std::vector<int> decalBoxIds = GetIdFromTag(aLevelData.tags, "DecalBox");
	for(auto& decalBoxId : decalBoxIds)
	{
		std::shared_ptr<DeferredDecal> decal = std::make_shared<DeferredDecal>();
		decal->SetTransform(aLevelData.transforms[decalBoxId]);
		decal->Init(aLevelData.decalPaths[decalBoxId]);
		myDeferredDecals.push_back(decal);
	}


	myLevelName = aLevelData.levelName;

	if (myLevelName == "LevelOne")
	{
		//myNavmeshHandler.LoadPolyMeshDetailBIN("Level1_Negative_Detailed.bin"); // For some reason Detailed mesh needs a Negative Z forward, but I have to invert it.
		//myNavmeshHandler.LoadPolyMeshBIN("Level1_Positive_Poly.bin"); // For some reason Poly mesh needs a Positive Z forward, but I have to invert it.
		//
		//myNavmeshHandler.HandleNavmeshStruct(true);
		//myNavmeshHandler.HandleNavmeshStructFromDetailedPolyMesh();
	}
	if (myLevelName == "LevelTwo")
	{
		/*FBX IMPORTED NAVMESH*/
		std::shared_ptr<DreamEngine::ModelInstance> navmeshObject = std::make_shared<DreamEngine::ModelInstance>(DE::ModelFactory::GetInstance().GetModelInstance(L"NavMesh/Level02_NavMesh_WorldAbsolut_AppliedTransform.fbx"));
		//myNavmeshObjects.push_back(navmeshObject);
		//myNavmeshHandler.Init(myNavmeshObjects, true);
		/*FBX IMPORTED NAVMESH*/
	}
	
	//myNavmesh = std::make_shared<DE::Navmesh>(myNavmeshHandler.GetPreviousBuiltNavmesh());
	//myDetailedNavmesh = std::make_shared<DE::Navmesh>(myNavmeshHandler.GetPreviousBuiltDetailedNavmesh());
	// Model loading is finished at this point
}

ObjectLoader::~ObjectLoader()
{
	MainSingleton::GetInstance()->GetPostMaster().Unsubscribe(eMessageType::MainMenuScene, this);
	myPlayer->Deallocate();
	myModelInstancer.clear();
	myModelInstances.clear();
	myNotCulledSetdressingObjects.clear();
	mySetdressingObjects.clear();
	mySetdressingObjectsWithCollision.clear();
	myNavmeshObjects.clear();
	myDeferredDecals.clear();
	mySpotLights.clear();
	myPointLights.clear();
}

void ObjectLoader::Init()
{
	for (int i = 0; i < mySetdressingObjects.size(); i++)
	{
		DE::Engine::GetInstance()->GetUpdateThreadPool().PushJob([&, i] 
			{
			});
				mySetdressingObjects[i]->Init();
	}

	for (int i = 0; i < myNotCulledSetdressingObjects.size(); i++)
	{
		DE::Engine::GetInstance()->GetUpdateThreadPool().PushJob([&, i]
			{
			});
				myNotCulledSetdressingObjects[i]->Init();
	}

	for (int i = 0; i < mySetdressingObjectsWithCollision.size(); i++)
	{
		DE::Engine::GetInstance()->GetUpdateThreadPool().PushJob([&, i]
			{
			});
				mySetdressingObjectsWithCollision[i]->Init();
	}


	while (DE::Engine::GetInstance()->GetUpdateThreadPool().GetUnfinishedJobs() > 0)
	{
		//Wait for jobs to finish
	}
	
	//************************************************************** TEMP FOR WAVE MANAGER TESTING
	MainSingleton::GetInstance()->GetPostMaster().Subscribe(eMessageType::MainMenuScene, this);

	myDetachPlayerUpdate = false;
	MainSingleton::GetInstance()->GetActiveCamera()->SetPosition(MainSingleton::GetInstance()->GetActiveCamera()->GetTransform().GetPosition() + DE::Vector3f(0.0f, 1000.0f, 0.0f));
	MainSingleton::GetInstance()->SetIsInCutscene(true);
}

void ObjectLoader::InitEnvironment(LevelData& aLevelData)
{
	/*DreamEngine::Engine::GetInstance()->GetUpdateThreadPool().PushJob([&]
		{
			
		});

	DreamEngine::Engine::GetInstance()->GetUpdateThreadPool().PushJob([&]
		{
			
		});
	while (DreamEngine::Engine::GetInstance()->GetUpdateThreadPool().GetUnfinishedJobs() > 0)
	{

	}*/
	
}

void ObjectLoader::InitBoat(LevelData& aLevelData)
{
	myBoat = std::make_shared<Boat>();

	for (auto const& [objectID, tag] : aLevelData.boatIDs)
	{
		if (tag == "Boat")
		{
			const auto& transform = aLevelData.transforms.at(objectID);
			myBoat->SetTransform(transform);

			auto itPath = aLevelData.boatModelPaths.find(objectID);
			if (itPath != aLevelData.boatModelPaths.end())
			{

				const std::string& narrowPath = itPath->second;
				std::wstring widePath{ narrowPath.begin(), narrowPath.end() };

				std::shared_ptr<DE::ModelInstance> hullModel = std::make_shared<DreamEngine::ModelInstance>(DE::ModelFactory::GetInstance().GetModelInstance(widePath));
				hullModel->SetTransform(transform);
				myBoat->SetModelInstance(hullModel);
			}
			break;
		}
	}

	for (auto const& [objectID, tag] : aLevelData.boatIDs)
	{
		if (tag == "Boat")
			continue;  

		BoatComponentType type = ParseTagToComponentType(tag);

		const auto& transform = aLevelData.transforms.at(objectID);

		// Extract Model Instance
		std::shared_ptr<DE::ModelInstance> modelInst = nullptr;
		auto itPath = aLevelData.boatModelPaths.find(objectID);
		if (itPath != aLevelData.boatModelPaths.end())
		{
			const std::string& narrowPath = itPath->second;
			std::wstring widePath{ narrowPath.begin(), narrowPath.end() };

			modelInst = std::make_shared<DreamEngine::ModelInstance>(DE::ModelFactory::GetInstance().GetModelInstance(widePath));
			modelInst->SetTransform(transform);
		}

		// Extract Interaction Point
		DE::Vector3f interactionPoint;
		auto itPoint = aLevelData.componentInteractionPoint.find(objectID);
		if (itPoint != aLevelData.componentInteractionPoint.end())
		{
			interactionPoint = itPoint->second;
		}

		auto comp = myBoat->CreateComponent(type, modelInst);
		comp->SetInteractionPoint(interactionPoint);
		comp->CaptureLocalTransform(*myBoat->GetTransform());
		myBoat->AddBoatComponent(comp);
	}

	myBoat->Init();
}

void ObjectLoader::Update(float aDeltaTime)
{
	if (!myStartedBgSound) 
	{
		myStartedBgSound = true;
	}

	DE::ThreadPool& updateThreadPool = DE::Engine::GetInstance()->GetUpdateThreadPool();
 
	auto prevRot = MainSingleton::GetInstance()->GetActiveCamera()->GetTransform().GetRotation();
	if (prevRot.z < -1.0f || prevRot.z > 1.0f)
	{
		prevRot.x += 180.0f;
		prevRot.y -= 180.0f;
		prevRot.z = 0.0f;
	}

	MainSingleton::GetInstance()->GetActiveCamera()->SetRotation({45.0f, -180.0f, 0.0f}); // TODO: Base on IMGUI for easy tweak
	//MainSingleton::GetInstance()->GetActiveCamera()->SetRotation(prevRot);

	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ nullptr, eMessageType::ActivateIntroMenu });
	MainSingleton::GetInstance()->SetShouldRenderCursor(true);

#ifndef _RETAIL
	if (MainSingleton::GetInstance()->GetInputManager().IsKeyDown(DE::eKeyCode::P) == true)
	{
		myDetachPlayerUpdate = !myDetachPlayerUpdate;
	}
#endif

	myNavmeshHandler.Update();
	myBoat->Update(aDeltaTime);
	myPlayer->Update(aDeltaTime);

	for (auto& enemySpawner : myEnemySpawners)
	{
		int index = 0;
		for (auto& weakpoint : myBoat->GetWeakPointPositions())
		{
			enemySpawner->SetBoatWeakPoint(weakpoint, index);
			index++;
		}
		enemySpawner->Update(aDeltaTime, *myBoat->GetTransform());
	}
	if (myLeviathan != nullptr && myLeviathan->IsActive())
		myLeviathan->Update(aDeltaTime);

	while (updateThreadPool.GetQueueSize() > 0)
	{
		//wait untill updateloop is done.
	}

	UpdateObject(mySetdressingObjects, aDeltaTime);
	UpdateObject(myNotCulledSetdressingObjects, aDeltaTime);

	mySelectionKeyHeldLenienceTimer.Update(aDeltaTime);
	if (MainSingleton::GetInstance()->GetActiveCamera() != nullptr)
		ComputeViewFrustumPlanes();
}

void ObjectLoader::ComputeViewFrustumPlanes()
{
	auto camera = MainSingleton::GetInstance()->GetActiveCamera();
	float nearPlane = 0.0f;
	float farPlane = 0.0f;
	camera->GetProjectionPlanes(nearPlane, farPlane);
	auto cameraMatrix = camera->GetTransform().GetMatrix();
	auto forward = cameraMatrix.GetForward();
	auto right = cameraMatrix.GetRight();
	auto up = cameraMatrix.GetUp();
	auto resolution = DE::Vector3f(DE::Engine::GetInstance()->GetRenderSize().x, DE::Engine::GetInstance()->GetRenderSize().y);

	auto farplaneCenter = camera->GetTransform().GetPosition() + cameraMatrix.GetForward() * farPlane;
	auto farplaneTopLeft = farplaneCenter + up * farPlane * resolution.y * 0.5f - right * farPlane * resolution.x * 0.5f;
	auto farplaneTopRight = farplaneCenter + up * farPlane * resolution.y * 0.5f + right * farPlane * resolution.x * 0.5f;
	auto farplaneBottomLeft = farplaneCenter - up * farPlane * resolution.y * 0.5f - right * farPlane * resolution.x * 0.5f;
	auto farplaneBottomRight = farplaneCenter - up * farPlane * resolution.y * 0.5f + right * farPlane * resolution.x * 0.5f;

	auto nearplaneCenter = camera->GetTransform().GetPosition() + cameraMatrix.GetForward() * nearPlane;
	auto nearplaneTopLeft = nearplaneCenter + up * resolution.y * 0.5f - right * resolution.x * 0.5f;
	auto nearplaneTopRight = nearplaneCenter + up * resolution.y * 0.5f + right * resolution.x * 0.5f;
	auto nearplaneBottomLeft = nearplaneCenter - up * resolution.y * 0.5f - right * resolution.x * 0.5f;
	auto nearplaneBottomRight = nearplaneCenter - up * resolution.y * 0.5f + right * resolution.x * 0.5f;

	myViewFrustumPlanes[0].InitWithPointAndNormal(nearplaneCenter, forward * -1.0f);
	myViewFrustumPlanes[1].InitWithPointAndNormal(farplaneCenter, forward);

	myViewFrustumPlanes[2].InitWith3Points(nearplaneBottomRight, nearplaneTopRight, farplaneTopRight);
	myViewFrustumPlanes[3].InitWith3Points(nearplaneTopRight, nearplaneTopLeft, farplaneTopLeft);
	myViewFrustumPlanes[4].InitWith3Points(nearplaneTopLeft, nearplaneBottomLeft, farplaneBottomLeft);
	myViewFrustumPlanes[5].InitWith3Points(nearplaneBottomLeft, nearplaneBottomRight, farplaneBottomRight);
}

void ObjectLoader::Receive(const Message& aMessage)
{
	switch (aMessage.messageType)
	{
		case eMessageType::MainMenuScene:
		{
			myModelInstancer.clear();
			myModelInstances.clear();
			myNotCulledSetdressingObjects.clear();
			mySetdressingObjects.clear();
			mySetdressingObjectsWithCollision.clear();
			myNavmeshObjects.clear();
			myDeferredDecals.clear();
			mySpotLights.clear();
			myPointLights.clear();
		}
	}
}

bool ObjectLoader::IsInViewFrustum(DE::Vector3f aObjCenter, float aObjRadius)
{
	for (int i = 0; i < 6; i++)
	{
		const DE::Plane<float>& plane = myViewFrustumPlanes[i]; 
		float planeD = -plane.GetNormal().Dot(plane.GetFirstPoint());
		float distance = plane.GetNormal().Dot(aObjCenter) + planeD;
		if (distance > aObjRadius)
		{
			return false;
		}
	}
	return true;
}

void ObjectLoader::Render(bool renderShadow, DE::GeometryBuffer& aGeometryBuffer)
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	auto& graphicsEngine = engine.GetGraphicsEngine();
	auto& modelDrawer = graphicsEngine.GetModelDrawer();
	
	MainSingleton::GetInstance()->GetStateStack()->Render(modelDrawer, renderShadow);
	
	if (!renderShadow && MainSingleton::GetInstance()->GetActiveCamera() != nullptr)
	{
		aGeometryBuffer.SetAsActiveTarget(DE::GeometryBuffer::eGeometryBufferTexture::Albedo);
		aGeometryBuffer.SetAsResourceOnSlot(DE::GeometryBuffer::eGeometryBufferTexture::WorldPosition, 6);
		auto& graphicsStateStack = graphicsEngine.GetGraphicsStateStack();
		graphicsStateStack.Push();
		graphicsStateStack.SetCamera(*MainSingleton::GetInstance()->GetActiveCamera());
		graphicsStateStack.SetRasterizerState(DE::RasterizerState::FrontfaceCulling);
		graphicsStateStack.SetDepthStencilState(DE::DepthStencilState::ReadOnlyGreater);
		graphicsStateStack.SetBlendState(DE::BlendState::AlphaBlend);
		graphicsStateStack.UpdateGpuStates();
		for (auto& decal : myDeferredDecals)
		{
			if (!IsInViewFrustum(decal->GetTransform()->GetPosition(), 500.0f)) continue;
			decal->Render(graphicsEngine);
		}
		graphicsStateStack.Pop();
		aGeometryBuffer.SetAsActiveTarget(DreamEngine::DX11::ourDepthBuffer);
	}
	
	if (myBoat != nullptr) myBoat->Render(graphicsEngine);

	if (myLeviathan != nullptr && myLeviathan->IsActive())
		myLeviathan->Render(graphicsEngine);

	if (!myEnemySpawners.empty())
	{
		for (auto& enemyspawn : myEnemySpawners)
		{
			enemyspawn->Render(graphicsEngine);
		}
	}

	if(myPlayer != nullptr && myPlayer->IsActive() && !renderShadow && !MainSingleton::GetInstance()->IsInCutscene())
	{
		myPlayer->Render(graphicsEngine);
		myPlayer->SetIsActive(true);
	}
	MainSingleton::GetInstance()->GetShaderTool().SetPlayerDirection(myBoat->GetTransform()->GetMatrix().GetForward());
}

void ObjectLoader::RenderMinimapGeometry()
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	auto& graphicsEngine = engine.GetGraphicsEngine();
	auto& modelDrawer = graphicsEngine.GetModelDrawer();

	myMinimap->GetDepthBuffer().Clear(1.0f, 0);
	myMinimap->GetGeometryTarget().Clear();
	myMinimap->GetGeometryTarget().SetAsActiveTarget(&myMinimap->GetDepthBuffer());
	auto& graphicsStateStack = graphicsEngine.GetGraphicsStateStack();
	graphicsStateStack.Push();
	graphicsStateStack.SetCamera(myMinimap->GetCamera());
	MainSingleton::GetInstance()->GetStateStack()->Render(modelDrawer, false);

	if (myBoat != nullptr) myBoat->Render(graphicsEngine);

	if (myLeviathan != nullptr && myLeviathan->IsActive())
		myLeviathan->Render(graphicsEngine);

	if (!myEnemySpawners.empty())
	{
		for (auto& enemyspawn : myEnemySpawners)
		{
			enemyspawn->Render(graphicsEngine);
		}
	}

	if (myPlayer != nullptr && myPlayer->IsActive())
	{
		myPlayer->Render(graphicsEngine);
		myPlayer->SetIsActive(true);
	}

	myMinimap->GetCanvas().Clear();
	myMinimap->GetCanvas().SetAsActiveTarget(/*DE::DX11::ourDepthBuffer*/);
	myMinimap->GetGeometryTarget().SetAsResourceOnSlot(1);
	myMinimap->GetColorTexture().SetAsResourceOnSlot(2);
	//graphicsStateStack.SetCustomShaderParameters(DE::Vector4f{ myBoat->GetTransform()->GetPosition().x, myBoat->GetTransform()->GetPosition().z, myMinimap->GetSmoothstep().x, myMinimap->GetSmoothstep().y });
	graphicsStateStack.SetCustomShaderParameters(DE::Vector4f{ myBoat->GetTransform()->GetPosition().x / 100.0f, myBoat->GetTransform()->GetPosition().z / 100.0f, myMinimap->GetRadius(), 0.0f });
	graphicsStateStack.UpdateGpuStates();
	myMinimapCullingFSE->Render();
	graphicsStateStack.Pop();
}

void ObjectLoader::UpdateImGui()
{

}

void ObjectLoader::RenderColliderVisualizations()
{
	ColliderComponent* outPlayerComponent;
	if(myPlayer->IsActive() && myPlayer->TryGetComponent<ColliderComponent>(outPlayerComponent))
	{
		outPlayerComponent->GetCollider()->RenderVisualization();
	}


#ifdef _DEBUG
	DE::LineDrawer& lineDrawer = DE::Engine::GetInstance()->GetGraphicsEngine().GetLineDrawer();
	DE::LinePrimitive lineToDraw;
	DE::Vector4f color(1.0f, 1.0f, 1.0f, 1.0f);
	lineToDraw.fromPosition = myLineFrom;
	lineToDraw.toPosition = myLineTo;
	lineToDraw.color = color;
	lineDrawer.Draw(lineToDraw);
#endif
}

void ObjectLoader::ClearInstancersMap()
{
	myInstancersMap.clear();
}

void ObjectLoader::AddToInstancer(LevelData& aLevelData, int aId)
{
	auto path = aLevelData.fbxPaths[aId];
	if (path == L"")
	{
		return;
	}
	if (myInstancer.find(path) == myInstancer.end())
	{
		DreamEngine::ModelInstancer localInstancer;
		DreamEngine::ModelFactory& modelFactory = DreamEngine::ModelFactory::GetInstance();
		myInstancer[path] = modelFactory.GetModelInstancer(path);
		myInstancer[path].AddInstance(aLevelData.transforms[aId]);
	}
	else
	{
		myInstancer[path].AddInstance(aLevelData.transforms[aId]);
	}
}

void ObjectLoader::RenderObjectLights(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
	myBoat->RenderLight(aGraphicsEngine);
}

BoatComponentType ObjectLoader::ParseTagToComponentType(const std::string& aTag)
{
	if (aTag == "BoatCannon")			return BoatComponentType::Cannon;
	else if (aTag == "BoatLight")		return BoatComponentType::Light;
	else if (aTag == "BoatAmmo")		return BoatComponentType::Ammo;
	else if (aTag == "BoatSpeed")		return BoatComponentType::Speed;
	else if (aTag == "BoatMap")			return BoatComponentType::Map;
	else if (aTag == "BoatRepair")		return BoatComponentType::Repair;
	else if (aTag == "BoatSteer")		return BoatComponentType::Steer;
	else if (aTag == "BoatWeakPoint")	return BoatComponentType::WeakPoint;
	else								return BoatComponentType::Count; // or assert/fail
}

template<class TempObject> 
void ObjectLoader::UpdateObject(TempObject& aObject, float aDeltaTime) 
{
	RenderData3D renderCommand;
	for (int i = 0; i < aObject.size(); i++)
	{
		if (aObject[i]->IsActive() == false)
		{
			continue;
		}

		aObject[i]->Update(aDeltaTime);
	}
}
