#pragma once

#include <Recast/Include/Recast.h>
//#include <Detour/Include/DetourNavMesh.h>
#include <DreamEngine/graphics/ModelInstance.h>

#include "Navmesh.h"

namespace DreamEngine
{
	class NavmeshCreationTool
	{
	public:
		NavmeshCreationTool() = default;
		~NavmeshCreationTool();

		void Init(std::vector<std::shared_ptr<DreamEngine::ModelInstance>> someSceneObjects, bool aShouldBuildDirectly);
		void HandleImGUI();

		void RenderNavmeshCentersAndConnections();
		void RenderNavmeshDetailedFromRecastStruct();
		void RenderNavmeshStruct();
		void RenderDetailedNavmeshStruct();
		void RenderNavmeshGrid();
		void RenderDetailedNavmeshGrid();
		void Render();

		void RenderDebugRay();
		void DebugRenderPath();

		void Update();
		void SetSoloMesh(std::shared_ptr<DreamEngine::ModelInstance> aSoloMesh);

		void BuildNavmesh();

		void LoadPolyMeshDetailBIN(const std::string& filename);
		void LoadPolyMeshBIN(const std::string& filename);
		void LoadNavmeshFromOBJ(std::string aPath);
		void LoadDetailedNavmeshFromOBJ(std::string aPath);

		void HandleNavmeshStruct(bool shouldInvertZ);
		void HandleNavmeshStructFromDetailedPolyMesh();

		void PopulateDetailedNavmeshGrid();
		void PopulatePolyNavmeshGrid();

		std::vector<int> GetGridCellsForLine(const DE::Vector2f& start, const DE::Vector2f& end, const NavmeshGrid& grid);

		//void HandleNavmeshStructFromDetourNavmesh();
		Navmesh& GetPreviousBuiltNavmesh();
		Navmesh& GetPreviousBuiltDetailedNavmesh();

		std::vector<DE::Vector3f>& GetPathfindingPoints() { myHavePathToGive = false; return myPath; }
		bool ShouldGivePath() { return myHavePathToGive; }

	private:
		std::vector<std::shared_ptr<DreamEngine::ModelInstance>> myMeshes;


		rcPolyMesh myPolyMesh;
		rcPolyMeshDetail myPolyMeshDetail;

		Navmesh myNavmesh;
		Navmesh myDetailedNavmesh;
		//dtNavMesh* myDetourNavMesh;

		DreamEngine::Vector3f myMeshOffsetPos;

		bool CastRayAgainstNavmeshIgnoringY(DE::Ray<float>& aRay, Navmesh aNavmesh, std::vector<DE::Vector3f>& aOutIntersectionPoint);
		bool CastRayAgainstNavmesh(DE::Ray<float>& aRay, Navmesh aNavmesh, DE::Vector3f& aOutIntersectionPoint);

		DE::Ray<float> GetRayFromMouseCLickPerspective();

		DreamEngine::Vector3f ReturnCollisionPointFromNavmesh(DE::Ray<float> directionRay); // Returning first collision Point on navmesh from mouse click
		DreamEngine::Vector3f ReturnCollisionPointFromNavmeshIgnoringY(DE::Ray<float> directionRay);

		DreamEngine::Vector2f myMousePosition;

		DreamEngine::LinePrimitive myLineToDraw;
		DreamEngine::LinePrimitive myLineToDrawFromClick;

		DE::Vector3f myPathTo{ 0.0f,0.0f,0.0f };
		std::vector<DE::Vector3f> myPath;

		std::vector<DE::Vector3f> mySavedIntersections;
		DE::Vector3f myPathFrom{ 0.0f,0.0f,0.0f };
		float myDrawXOffsetDetail = 0.f;
		float myDrawYOffsetDetail = 0.f;
		float myDrawZOffsetDetail = 0.f;

		float myDrawXOffset = 0.f;
		float myDrawYOffset = 0.f;
		float myDrawZOffset = 0.f;

		float myAgentHeight = 2.0f;
		float myAgentMaxClimb = 0.9f;
		float myAgentRadius = 0.2f;

		bool myShowPolyMeshDetail = false;  // Variable to control PolyMeshDetail rendering
		bool myShowPolyMesh = false;        // Variable to control PolyMesh rendering
		bool myNavmeshStruct = false;        // Variable to control Navmesh Struct rendering
		bool myShowNavmeshGrid= false;        // Variable to control Navmesh Grid rendering
		bool myShowDetailedNavmeshGrid = false; // Variable to control Detailed Navmesh Grid rendering

		bool myUseDebugRay = false;
		bool myIgnoreYAndCollectIntersections = false;
		bool myFindClosestIntersectionOnly = false;
		
		bool myCanPathfind = false;
		bool myShowPath = false;
		bool myHavePathToGive = false;
	};
}