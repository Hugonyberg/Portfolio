#pragma once

#include <DreamEngine\math\Vector.h>
#include <DreamEngine\math\Collider.h>

#include <vector>
#include <array>
#include <memory>

#include <Windows.h>
#undef min
#undef max
#include <algorithm>
#include <cmath>

namespace DreamEngine
{
	enum class eNodeState
	{
		Unvisited,
		Open,
		Closed
	};

	struct Edge
	{
		DE::Vector3f firstPoint;
		DE::Vector3f secondPoint;
	};

	struct Node
	{
		DreamEngine::Vector3f	center;
		std::array<int, 3>		connections;
		std::array<int, 3>		indices;
		eNodeState state = eNodeState::Unvisited;
	};
	
	struct NavmeshGrid
	{
		DreamEngine::Vector2f minBound; 
		DreamEngine::Vector2f maxBound; 
		int rows;                     
		int cols;                     
		float cellWidth;              
		float cellHeight;             

		std::vector<std::vector<int>> cells;
		std::vector<std::vector<int>> edgeCells;

		std::pair<int, int> GetCellFromPosition(DE::Vector3f aPosition)
		{
			std::pair<int, int> cellCoord;
			int col = static_cast<int>((aPosition.x - minBound.x) / cellWidth);
			int row = static_cast<int>((aPosition.z - minBound.y) / cellHeight);

			col = std::max(0, std::min(col, cols - 1));
			row = std::max(0, std::min(row, rows - 1));

			cellCoord.first = row;
			cellCoord.second = col;
			return cellCoord;
		}
	};

	struct NavmeshEdge
	{
		DE::Vector3f start;
		DE::Vector3f end;
		DE::Vector3f normal;
	};

	struct Navmesh
	{
		std::vector<Node>	nodes;
		std::vector<DreamEngine::Vector3f>	meshVertices;
		std::vector<int>	meshIndices;
		NavmeshGrid grid;
		std::vector<NavmeshEdge> edges;
	};

	struct CostAndIndex
	{
		float cost;
		int index;
		bool operator<(const CostAndIndex& anOther) const
		{
			return cost > anOther.cost;
		}
	};

	class Pathfinding 
	{
	public:
		static bool CastRayAgainstNavmesh(DE::Ray<float>& aRay, std::shared_ptr<Navmesh> aNavmesh, Vector3f& aOutIntersectionPoint);
		static DE::Vector3f GetClosestPointOnLine(DE::Vector3f aFirstPoint, DE::Vector3f aSecondPoint, DE::Vector3f anExternalPoint);
		static DE::Vector3f GetClosestPointOnNavmesh(const DE::Vector3f aPoint, std::shared_ptr<Navmesh> aNavmesh);
		static DE::Vector3f GetClosestPointOnNavmesh(const DE::Vector3f aPoint, Navmesh aNavmesh);
		static float GetYPosFromPoint(const DE::Vector3f aPoint, const int aNodeIndex, std::shared_ptr<Navmesh> aNavmesh);
		static float GetYPosFromProjectedPoint(const DE::Vector3f aPoint, const int aNodeIndex, std::shared_ptr<Navmesh> aNavmesh);
		static float GetYPosFromPointDetailed(const DE::Vector3f aPoint, const int aNodeIndex, std::shared_ptr<Navmesh> aNavmesh);
		static int GetNodeIndexFromPoint(const DE::Vector3f aPoint, std::shared_ptr<Navmesh> aNavmesh);
		static int GetNodeIndexFromPoint(const DE::Vector3f aPoint, Navmesh aNavmesh);
		static int GetNodeIndexFromPointDetailed(const DE::Vector3f aPoint, std::shared_ptr<Navmesh> aNavmesh);
		static float GetNavmeshYAtPosition(const DE::Vector3f enemyPos, std::shared_ptr<Navmesh> aNavmesh);
		static std::vector<DE::Vector3f> FindShortestPath(DE::Vector3f aStart, DE::Vector3f anEnd, std::shared_ptr<Navmesh> aNavmesh);
		static std::vector<DE::Vector3f> FindShortestPath(DE::Vector3f aStart, DE::Vector3f anEnd, std::shared_ptr<Navmesh> aNavmesh, bool aUseFunnel);
		static std::vector<DE::Vector3f> FindShortestPath(DE::Vector3f aStart, DE::Vector3f anEnd, Navmesh aNavmesh, bool aUseFunnel);
		static void FunnelPath(const std::vector<Edge>& portals, std::vector<DE::Vector3f>& outPath);
		static void ComputeMiddlePoints(const std::vector<Edge>& portals, std::vector<DE::Vector3f>& outPath);
		static DE::Vector3f FindMidpoint(const DE::Vector3f& aFirstPos, const DE::Vector3f& aSecondPos);
		static bool IsToTheLeftOfLine(DE::Vector3f aLineStart, DE::Vector3f aLineEnd, DE::Vector3f aPoint);

		static bool GetNearestEdge(std::shared_ptr<Navmesh> aNavmesh, DE::Vector3f aPosition, float& anEdgeDistance, DE::Vector3f& anEdgeNormal);
		static float ComputePointToLineDistance(const DE::Vector3f& point, const DE::Vector3f& lineStart, const DE::Vector3f& lineEnd);

	private:
		static float GetHeuristic(DE::Vector3f a, DE::Vector3f b);
		static bool IsToTheRightOfLine(DE::Vector3f aLineStart, DE::Vector3f aLineEnd, DE::Vector3f aPoint);
	};
}