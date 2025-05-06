#pragma once
#include <DreamEngine/graphics/ModelInstance.h>
#include <DreamEngine/graphics/ModelInstancer.h>
#include <DreamEngine\utilities\CountTimer.h>
#include <DreamEngine\graphics\sprite.h>
#include <DreamEngine/shaders/ModelShader.h>
#include <DreamEngine\math\Collider.h>

#include "UnityLoader.h" 
#include "NavmeshCreationTool.h"
#include "Observer.h"

#include "BoatComponent.h"

#include "WorldCommon.h"

class GameObject;
class Player;
class Boat;
class Setdressing;
class DeferredDecal;
class Leviathan;
class EnemySpawner;
class Heatmap;
class WorldGrid;
class Minimap;

namespace DreamEngine
{
	class PointLight;
	class SpotLight;
	class DirectionalLight;
	class AmbientLight;
	class AnimatedModelInstance;
	class GeometryBuffer;
};

class ObjectLoader : public Observer
{
public:
	ObjectLoader(LevelData& aLevelData);
	~ObjectLoader();

	void Init(); 
	void InitEnvironment(LevelData& aLevelData);
	void InitBoat(LevelData& aLevelData);
	void Update(float aDeltaTime);
	void ComputeViewFrustumPlanes();
	void Receive(const Message& aMessage) override;
	void Render(bool renderShadow, DE::GeometryBuffer& aGeometryBuffer);
	void RenderMinimapGeometry();
	void UpdateImGui(); // Call All objects ImGui from here
	bool IsInViewFrustum(DE::Vector3f aObjCenter, float aObjRadius);
	void RenderColliderVisualizations(); 
	void ClearInstancersMap();

	void AddToInstancer(LevelData& aLevelData, int aId);

	template<class TempObject>
	void UpdateObject(TempObject& aObject, float aDeltaTime);

	void RenderObjectLights(DreamEngine::GraphicsEngine& aGraphicsEngine);

	DreamEngine::ModelInstance GetModel(){ *myModelInstances[0]; }
	std::shared_ptr<Player> GetPlayer() { return myPlayer; }
	std::shared_ptr<Boat> GetBoat() { return myBoat; }
	DreamEngine::NavmeshCreationTool& GetNavmeshHandler() { return myNavmeshHandler; }
	std::vector<std::shared_ptr<DreamEngine::PointLight>> GetPointLights() { return myPointLights; }
	std::vector<std::shared_ptr<DreamEngine::SpotLight>> GetSpotLights() { return mySpotLights; }

	std::shared_ptr<DreamEngine::DirectionalLight> GetDirectionalLight() { return myDirectionalLight; }
	std::shared_ptr<DreamEngine::AmbientLight> GetAmbientLight() { return myAmbientLight; }

private:
	BoatComponentType ParseTagToComponentType(const std::string& aTag);

	std::unordered_map<std::wstring, DreamEngine::ModelInstancer> myInstancer;
	std::vector<std::shared_ptr<DreamEngine::ModelInstance>> myModelInstances;
	std::vector<std::shared_ptr<Setdressing>> mySetdressingObjects;
	std::vector<std::shared_ptr<Setdressing>> myNotCulledSetdressingObjects;
	std::vector<std::shared_ptr<Setdressing>> mySetdressingObjectsWithCollision;
	DE::SpriteSharedData myFogData;

	std::vector<std::shared_ptr<DreamEngine::PointLight>> myPointLights;
	std::vector<std::shared_ptr<DreamEngine::SpotLight>> mySpotLights;
	std::shared_ptr<DreamEngine::DirectionalLight> myDirectionalLight;
	std::shared_ptr<DreamEngine::AmbientLight> myAmbientLight;

	std::vector<std::shared_ptr<DeferredDecal>> myDeferredDecals;

	std::shared_ptr<Player> myPlayer;
	std::shared_ptr<Leviathan> myLeviathan;
	std::vector<std::shared_ptr<EnemySpawner>> myEnemySpawners;

	DreamEngine::NavmeshCreationTool myNavmeshHandler;
	std::vector<std::shared_ptr<DreamEngine::ModelInstance>> myNavmeshObjects;
	std::shared_ptr<DreamEngine::Navmesh> myNavmesh;
	std::shared_ptr<DreamEngine::Navmesh> myDetailedNavmesh;
	std::array<DE::Plane<float>, 6> myViewFrustumPlanes;
	std::vector<DE::ModelInstancer*> myModelInstancer;
	std::unordered_map<std::string, DE::ModelInstancer> myInstancersMap;

	std::string myLevelName;

	std::shared_ptr<Boat> myBoat;

	Minimap* myMinimap;
	std::unique_ptr<DreamEngine::FullscreenEffect> myMinimapCullingFSE;
	std::unique_ptr<WorldGrid> myWorldGrid;
	std::array<std::unique_ptr<Heatmap>, static_cast<int>(eHeatmapType::DefaultTypeAndCount)> myHeatmaps;

#pragma region Debug
	DE::Vector3f myLineFrom;
	DE::Vector3f myLineTo;
#pragma endregion

	CU::CountdownTimer mySelectionKeyHeldLenienceTimer;

	float myCullingRange;
	bool myStartedBgSound = false;
	bool myDetachPlayerUpdate = false;
};