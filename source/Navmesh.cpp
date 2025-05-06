#include "stdafx.h"

#include "Navmesh.h"
#include <map>
#include <queue>
#include <DreamEngine\math\Intersection.h>

static const float extraPassThreshholdDistForSmoothing = 100.0f;



bool DreamEngine::Pathfinding::CastRayAgainstNavmesh(DE::Ray<float>& aRay, std::shared_ptr<Navmesh> aNavmesh, Vector3f& aOutIntersectionPoint)
{
	float distToClosestHit = FLT_MAX;
	for (auto& node : aNavmesh->nodes)
	{
		float outDist = FLT_MAX;
		DE::Vector3f outPoint;
		if (DE::IntersectionTriangleRay(aNavmesh->meshVertices[node.indices[0]], aNavmesh->meshVertices[node.indices[1]], aNavmesh->meshVertices[node.indices[2]], aRay, outPoint, outDist) && outDist < distToClosestHit) 
		{
			aOutIntersectionPoint = outPoint;
			distToClosestHit = outDist;
		}
	}
	if (distToClosestHit != FLT_MAX) 
	{
		return true;
	}
	return false;
}

DE::Vector3f DreamEngine::Pathfinding::GetClosestPointOnLine(DE::Vector3f aFirstPoint, DE::Vector3f aSecondPoint, DE::Vector3f anExternalPoint)
{
	DE::Vector3f A = aFirstPoint;
	DE::Vector3f B = aSecondPoint;
	DE::Vector3f C = anExternalPoint;

	// Compute the vector from A to B
	DE::Vector3f AB = B - A;
	// Compute the vector from A to C
	DE::Vector3f AC = C - A;

	// Compute the projection scalar t
	float t = AC.Dot(AB) / AB.Dot(AB);

	// Clamp t to the range [0, 1] if you want the closest point on the segment rather than the infinite line
	t = std::max<float>(0.0f, std::min<float>(1.0f, t));

	// Compute the closest point on the line
	return A + t * AB;
}

DE::Vector3f DreamEngine::Pathfinding::GetClosestPointOnNavmesh(const DE::Vector3f aPoint, std::shared_ptr<Navmesh> aNavmesh)
{
	DE::Vector3f closestPoint;
	float sqrDistToClosestPoint = FLT_MAX;
	for (auto& node : aNavmesh->nodes)
	{
		DE::Vector3f nextClosestPoint = GetClosestPointOnLine(aNavmesh->meshVertices[node.indices[0]], aNavmesh->meshVertices[node.indices[1]], aPoint);
		float nextSqrDist = (aPoint - nextClosestPoint).LengthSqr();
		if (nextSqrDist < sqrDistToClosestPoint)
		{
			closestPoint = nextClosestPoint;
			sqrDistToClosestPoint = nextSqrDist;
		}

		nextClosestPoint = GetClosestPointOnLine(aNavmesh->meshVertices[node.indices[1]], aNavmesh->meshVertices[node.indices[2]], aPoint);
		nextSqrDist = (aPoint - nextClosestPoint).LengthSqr();
		if (nextSqrDist < sqrDistToClosestPoint)
		{
			closestPoint = nextClosestPoint;
			sqrDistToClosestPoint = nextSqrDist;
		}

		nextClosestPoint = GetClosestPointOnLine(aNavmesh->meshVertices[node.indices[2]], aNavmesh->meshVertices[node.indices[0]], aPoint);
		nextSqrDist = (aPoint - nextClosestPoint).LengthSqr();
		if (nextSqrDist < sqrDistToClosestPoint)
		{
			closestPoint = nextClosestPoint;
			sqrDistToClosestPoint = nextSqrDist;
		}
	}

	return closestPoint;
}

DE::Vector3f DreamEngine::Pathfinding::GetClosestPointOnNavmesh(const DE::Vector3f aPoint, Navmesh aNavmesh)
{
	DE::Vector3f closestPoint;
	float sqrDistToClosestPoint = FLT_MAX;
	for (auto& node : aNavmesh.nodes)
	{
		DE::Vector3f nextClosestPoint = GetClosestPointOnLine(aNavmesh.meshVertices[node.indices[0]], aNavmesh.meshVertices[node.indices[1]], aPoint);
		float nextSqrDist = (aPoint - nextClosestPoint).LengthSqr();
		if (nextSqrDist < sqrDistToClosestPoint)
		{
			closestPoint = nextClosestPoint;
			sqrDistToClosestPoint = nextSqrDist;
		}

		nextClosestPoint = GetClosestPointOnLine(aNavmesh.meshVertices[node.indices[1]], aNavmesh.meshVertices[node.indices[2]], aPoint);
		nextSqrDist = (aPoint - nextClosestPoint).LengthSqr();
		if (nextSqrDist < sqrDistToClosestPoint)
		{
			closestPoint = nextClosestPoint;
			sqrDistToClosestPoint = nextSqrDist;
		}

		nextClosestPoint = GetClosestPointOnLine(aNavmesh.meshVertices[node.indices[2]], aNavmesh.meshVertices[node.indices[0]], aPoint);
		nextSqrDist = (aPoint - nextClosestPoint).LengthSqr();
		if (nextSqrDist < sqrDistToClosestPoint)
		{
			closestPoint = nextClosestPoint;
			sqrDistToClosestPoint = nextSqrDist;
		}
	}

	return closestPoint;
}

float DreamEngine::Pathfinding::GetYPosFromPoint(const DE::Vector3f aPoint, const int aNodeIndex, std::shared_ptr<Navmesh> aNavmesh)
{
	// Get the triangle vertices
	DE::Vector3f v1 = aNavmesh->meshVertices[aNavmesh->nodes[aNodeIndex].indices[0]];
	DE::Vector3f v2 = aNavmesh->meshVertices[aNavmesh->nodes[aNodeIndex].indices[1]];
	DE::Vector3f v3 = aNavmesh->meshVertices[aNavmesh->nodes[aNodeIndex].indices[2]];

	// Project everything onto the XZ plane (ignore the y-values for distance calculations)
	DE::Vector3f v0 = v2 - v1;
	DE::Vector3f v1p = v3 - v1;
	DE::Vector3f vp = aPoint - v1;

	// Compute dot products in the XZ plane
	float d00 = v0.x * v0.x + v0.z * v0.z;
	float d01 = v0.x * v1p.x + v0.z * v1p.z;
	float d11 = v1p.x * v1p.x + v1p.z * v1p.z;
	float d20 = vp.x * v0.x + vp.z * v0.z;
	float d21 = vp.x * v1p.x + vp.z * v1p.z;

	// Compute barycentric coordinates
	float denom = d00 * d11 - d01 * d01;
	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	// Interpolate y-coordinate based on barycentric coordinates
	float interpolatedY = u * v1.y + v * v2.y + w * v3.y;

	return interpolatedY;
}

float DreamEngine::Pathfinding::GetYPosFromProjectedPoint(const DE::Vector3f aPoint, const int aNodeIndex, std::shared_ptr<Navmesh> aNavmesh)
{
	return 0.0f;
}

float DreamEngine::Pathfinding::GetYPosFromPointDetailed(const DE::Vector3f aPoint, const int aNodeIndex, std::shared_ptr<Navmesh> aNavmesh)
{
	return DE::Pathfinding::GetYPosFromPoint(aPoint, aNodeIndex, aNavmesh);
}

int DreamEngine::Pathfinding::GetNodeIndexFromPoint(const DE::Vector3f aPoint, std::shared_ptr<Navmesh> aNavmesh)
{
	int lowestYIndex = -1;
	float sqrDistToLowestY = FLT_MAX;
	for (int i = 0; i < aNavmesh->nodes.size(); i++)
	{
		const DE::Vector3f& pointA = aNavmesh->meshVertices[aNavmesh->nodes[i].indices[0]];
		const DE::Vector3f& pointB = aNavmesh->meshVertices[aNavmesh->nodes[i].indices[1]];
		const DE::Vector3f& pointC = aNavmesh->meshVertices[aNavmesh->nodes[i].indices[2]];

		float w1 = ((pointB.z - pointC.z) * (aPoint.x - pointC.x) + (pointC.x - pointB.x) * (aPoint.z - pointC.z)) /
			((pointB.z - pointC.z) * (pointA.x - pointC.x) + (pointC.x - pointB.x) * (pointA.z - pointC.z));

		float w2 = ((pointC.z - pointA.z) * (aPoint.x - pointC.x) + (pointA.x - pointC.x) * (aPoint.z - pointC.z)) /
			((pointB.z - pointC.z) * (pointA.x - pointC.x) + (pointC.x - pointB.x) * (pointA.z - pointC.z));

		float w3 = 1.0f - w1 - w2;

		//const bool inside = w1 >= 0 && w2 >= 0 && w3 >= 0;
		float epsilon = 1e-5f;
		const bool inside = (w1 >= -epsilon) && (w2 >= -epsilon) && (w3 >= -epsilon);
		if (inside && (aPoint - aNavmesh->nodes[i].center).LengthSqr() < sqrDistToLowestY)
		{
			lowestYIndex = i;
			sqrDistToLowestY = (aPoint - aNavmesh->nodes[i].center).LengthSqr();
		}
	}
	return lowestYIndex;
}

int DreamEngine::Pathfinding::GetNodeIndexFromPoint(const DE::Vector3f aPoint, Navmesh aNavmesh)
{
	int lowestYIndex = -1;
	float sqrDistToLowestY = FLT_MAX;
	for (int i = 0; i < aNavmesh.nodes.size(); i++)
	{
		const DE::Vector3f& pointA = aNavmesh.meshVertices[aNavmesh.nodes[i].indices[0]];
		const DE::Vector3f& pointB = aNavmesh.meshVertices[aNavmesh.nodes[i].indices[1]];
		const DE::Vector3f& pointC = aNavmesh.meshVertices[aNavmesh.nodes[i].indices[2]];

		float w1 = ((pointB.z - pointC.z) * (aPoint.x - pointC.x) + (pointC.x - pointB.x) * (aPoint.z - pointC.z)) /
			((pointB.z - pointC.z) * (pointA.x - pointC.x) + (pointC.x - pointB.x) * (pointA.z - pointC.z));

		float w2 = ((pointC.z - pointA.z) * (aPoint.x - pointC.x) + (pointA.x - pointC.x) * (aPoint.z - pointC.z)) /
			((pointB.z - pointC.z) * (pointA.x - pointC.x) + (pointC.x - pointB.x) * (pointA.z - pointC.z));

		float w3 = 1.0f - w1 - w2;

		const bool inside = w1 >= 0 && w2 >= 0 && w3 >= 0;
		if (inside && (aPoint - aNavmesh.nodes[i].center).LengthSqr() < sqrDistToLowestY)
		{
			lowestYIndex = i;
			sqrDistToLowestY = (aPoint - aNavmesh.nodes[i].center).LengthSqr();
		}
	}
	return lowestYIndex;
}


int DreamEngine::Pathfinding::GetNodeIndexFromPointDetailed(const DE::Vector3f aPoint, std::shared_ptr<Navmesh> aNavmesh)
{
	return DE::Pathfinding::GetNodeIndexFromPoint(aPoint, aNavmesh);
}

float DreamEngine::Pathfinding::GetNavmeshYAtPosition(const DE::Vector3f enemyPos, std::shared_ptr<Navmesh> aNavmesh)
{
	int lowestYIndex = -1;
	float sqrDistToLowestY = 10000;

	std::pair<int, int> cell = aNavmesh->grid.GetCellFromPosition(enemyPos);
	int row = cell.first;
	int col = cell.second;

	int cellIndex = row * aNavmesh->grid.cols + col;

	// Access the vector of node indices for that cell.
	std::vector<int>& nodeIndicesInCell = aNavmesh->grid.cells[cellIndex];

	//for (int i = 0; i < aNavmesh->nodes.size(); i++)
	for (int i : nodeIndicesInCell)
	{
		const DE::Vector3f& pointA = aNavmesh->meshVertices[aNavmesh->nodes[i].indices[0]];
		const DE::Vector3f& pointB = aNavmesh->meshVertices[aNavmesh->nodes[i].indices[1]];
		const DE::Vector3f& pointC = aNavmesh->meshVertices[aNavmesh->nodes[i].indices[2]];

		float w1 = ((pointB.z - pointC.z) * (enemyPos.x - pointC.x) + (pointC.x - pointB.x) * (enemyPos.z - pointC.z)) /
			((pointB.z - pointC.z) * (pointA.x - pointC.x) + (pointC.x - pointB.x) * (pointA.z - pointC.z));

		float w2 = ((pointC.z - pointA.z) * (enemyPos.x - pointC.x) + (pointA.x - pointC.x) * (enemyPos.z - pointC.z)) /
			((pointB.z - pointC.z) * (pointA.x - pointC.x) + (pointC.x - pointB.x) * (pointA.z - pointC.z));

		float w3 = 1.0f - w1 - w2;

		const bool inside = w1 >= 0 && w2 >= 0 && w3 >= 0;
		if (inside && (enemyPos - aNavmesh->nodes[i].center).LengthSqr() < sqrDistToLowestY)
		{
			lowestYIndex = i;
			sqrDistToLowestY = (enemyPos - aNavmesh->nodes[i].center).LengthSqr();
		}
	}

	// If no valid node is found, fall back to the enemy's current y
	if (lowestYIndex == -1)
		return enemyPos.y;

	return GetYPosFromPoint(enemyPos, lowestYIndex, aNavmesh);
}

std::vector<DE::Vector3f> DreamEngine::Pathfinding::FindShortestPath(DE::Vector3f aStart, DE::Vector3f anEnd, std::shared_ptr<Navmesh> aNavmesh)
{
	std::map<int, int> cameFrom;
	std::map<int, float> costSoFar;
	std::priority_queue<CostAndIndex> frontier;
	std::vector<Node> copiedNodes = aNavmesh->nodes;

	aStart = GetClosestPointOnNavmesh(aStart, aNavmesh);
	anEnd = GetClosestPointOnNavmesh(anEnd, aNavmesh);

	int startIndex = GetNodeIndexFromPoint(aStart, aNavmesh);
	int endIndex = GetNodeIndexFromPoint(anEnd, aNavmesh);
	if (startIndex == -1 || endIndex == -1) 
	{
		return std::vector<DE::Vector3f>();
	}
	else if (startIndex == endIndex) 
	{
		std::vector<DE::Vector3f> earlyRetVec;
		earlyRetVec.push_back(anEnd);
		return earlyRetVec;
	}

	frontier.push({ 0, startIndex });
	copiedNodes[startIndex].state = eNodeState::Closed;
	for (int connectedIndex : copiedNodes[startIndex].connections)
	{
		if (connectedIndex < 0) 
		{
			continue;
		}
		copiedNodes[connectedIndex].state = eNodeState::Open;
	}
	cameFrom.insert({ startIndex, startIndex });
	costSoFar.insert({ startIndex, 0.f });

	while (!frontier.empty())
	{
		CostAndIndex currentLocation = frontier.top();
		frontier.pop();
		copiedNodes[currentLocation.index].state = eNodeState::Closed;

		if (currentLocation.index == endIndex)
		{
			std::vector<Node*> path;
			if (cameFrom.find(endIndex) == cameFrom.end())
			{
				return std::vector<DE::Vector3f>();
			}
			std::vector<Edge> portals;
			portals.reserve(path.size() * 3 + 1);
			int current = cameFrom[endIndex];
			path.push_back(&copiedNodes[endIndex]);
			while (current != startIndex)
			{
				path.push_back(&copiedNodes[current]);
				current = cameFrom[current];
			}
			path.push_back(&copiedNodes[startIndex]);
			std::reverse(path.begin(), path.end());

			portals.push_back({ aStart, aStart });
			bool skippedFirst = false;
			portals.push_back({ aNavmesh->meshVertices[aNavmesh->nodes[startIndex].indices[0]], aNavmesh->meshVertices[aNavmesh->nodes[startIndex].indices[1]] });
			portals.push_back({ aNavmesh->meshVertices[aNavmesh->nodes[startIndex].indices[1]], aNavmesh->meshVertices[aNavmesh->nodes[startIndex].indices[2]] });
			portals.push_back({ aNavmesh->meshVertices[aNavmesh->nodes[startIndex].indices[2]], aNavmesh->meshVertices[aNavmesh->nodes[startIndex].indices[0]] });
			for (auto node : path)
			{
				if (!skippedFirst)
				{
					skippedFirst = true;
					continue;
				}

				for (int i = (int)portals.size() - 3; i < portals.size(); i++)
				{
					int sharedVerts = 0;
					for (int y = 0; y < 3; y++)
					{
						if (aNavmesh->meshVertices[node->indices[y]] == portals[i].firstPoint)
						{
							sharedVerts++;
						}
						else if (aNavmesh->meshVertices[node->indices[y]] == portals[i].secondPoint)
						{
							sharedVerts++;
						}
					}
					if (sharedVerts < 2)
					{
						portals[i] = portals.back();
						portals.pop_back();
						i--;
					}
				}

				if (node != path.back())
				{
					portals.push_back({ aNavmesh->meshVertices[node->indices[0]], aNavmesh->meshVertices[node->indices[1]] });
					portals.push_back({ aNavmesh->meshVertices[node->indices[1]], aNavmesh->meshVertices[node->indices[2]] });
					portals.push_back({ aNavmesh->meshVertices[node->indices[2]], aNavmesh->meshVertices[node->indices[0]] });
				}
			}

			portals.push_back({ anEnd, anEnd });
			std::vector<DE::Vector3f> optimizedPath;
			FunnelPath(portals, optimizedPath);
			std::reverse(optimizedPath.begin(), optimizedPath.end());
			return optimizedPath;
		}

		for (int next : copiedNodes[currentLocation.index].connections)
		{
			if (next < 0 || copiedNodes[next].state != eNodeState::Open)
			{
				continue;
			}
			float newCost = costSoFar[currentLocation.index] + (std::abs(copiedNodes[next].center.x - copiedNodes[currentLocation.index].center.x) + std::abs(copiedNodes[next].center.y - copiedNodes[currentLocation.index].center.y));
			if (costSoFar.find(next) == costSoFar.end() || newCost < costSoFar[next])
			{
				costSoFar[next] = newCost;
				float priority = newCost + GetHeuristic(copiedNodes[next].center, copiedNodes[endIndex].center);
				frontier.push({ priority, next });
				for (int connectedIndex : copiedNodes[next].connections)
				{
					if (connectedIndex < 0) 
					{
						continue;
					}
					if (copiedNodes[connectedIndex].state == eNodeState::Unvisited)
					{
						copiedNodes[connectedIndex].state = eNodeState::Open;
					}
				}
				cameFrom[next] = currentLocation.index;
			}
		}
	}

	return std::vector<DE::Vector3f>();
}


std::vector<DE::Vector3f> DreamEngine::Pathfinding::FindShortestPath(DE::Vector3f aStart, DE::Vector3f anEnd, std::shared_ptr<Navmesh> aNavmesh, bool aUseFunnel)
{
	std::map<int, int> cameFrom;
	std::map<int, float> costSoFar;
	std::priority_queue<CostAndIndex> frontier;
	std::vector<Node> copiedNodes = aNavmesh->nodes;

	aStart = GetClosestPointOnNavmesh(aStart, aNavmesh);
	anEnd = GetClosestPointOnNavmesh(anEnd, aNavmesh);

	int startIndex = GetNodeIndexFromPoint(aStart, aNavmesh);
	int endIndex = GetNodeIndexFromPoint(anEnd, aNavmesh);
	if (startIndex == -1 || endIndex == -1)
	{
		return std::vector<DE::Vector3f>();
	}
	else if (startIndex == endIndex)
	{
		std::vector<DE::Vector3f> earlyRetVec;
		earlyRetVec.push_back(anEnd);
		return earlyRetVec;
	}

	frontier.push({ 0, startIndex });
	copiedNodes[startIndex].state = eNodeState::Closed;
	for (int connectedIndex : copiedNodes[startIndex].connections)
	{
		if (connectedIndex < 0)
		{
			continue;
		}
		copiedNodes[connectedIndex].state = eNodeState::Open;
	}
	cameFrom.insert({ startIndex, startIndex });
	costSoFar.insert({ startIndex, 0.f });

	while (!frontier.empty())
	{
		CostAndIndex currentLocation = frontier.top();
		frontier.pop();
		copiedNodes[currentLocation.index].state = eNodeState::Closed;

		if (currentLocation.index == endIndex)
		{
			std::vector<Node*> path;
			if (cameFrom.find(endIndex) == cameFrom.end())
			{
				return std::vector<DE::Vector3f>();
			}
			std::vector<Edge> portals;
			portals.reserve(path.size() * 3 + 1);
			int current = cameFrom[endIndex];
			path.push_back(&copiedNodes[endIndex]);
			while (current != startIndex)
			{
				path.push_back(&copiedNodes[current]);
				current = cameFrom[current];
			}
			path.push_back(&copiedNodes[startIndex]);
			std::reverse(path.begin(), path.end());

			portals.push_back({ aStart, aStart });
			bool skippedFirst = false;
			portals.push_back({ aNavmesh->meshVertices[aNavmesh->nodes[startIndex].indices[0]], aNavmesh->meshVertices[aNavmesh->nodes[startIndex].indices[1]] });
			portals.push_back({ aNavmesh->meshVertices[aNavmesh->nodes[startIndex].indices[1]], aNavmesh->meshVertices[aNavmesh->nodes[startIndex].indices[2]] });
			portals.push_back({ aNavmesh->meshVertices[aNavmesh->nodes[startIndex].indices[2]], aNavmesh->meshVertices[aNavmesh->nodes[startIndex].indices[0]] });
			for (auto node : path)
			{
				if (!skippedFirst)
				{
					skippedFirst = true;
					continue;
				}

				for (int i = (int)portals.size() - 3; i < portals.size(); i++)
				{
					int sharedVerts = 0;
					for (int y = 0; y < 3; y++)
					{
						if (aNavmesh->meshVertices[node->indices[y]] == portals[i].firstPoint)
						{
							sharedVerts++;
						}
						else if (aNavmesh->meshVertices[node->indices[y]] == portals[i].secondPoint)
						{
							sharedVerts++;
						}
					}
					if (sharedVerts < 2)
					{
						portals[i] = portals.back();
						portals.pop_back();
						i--;
					}
				}

				if (node != path.back())
				{
					portals.push_back({ aNavmesh->meshVertices[node->indices[0]], aNavmesh->meshVertices[node->indices[1]] });
					portals.push_back({ aNavmesh->meshVertices[node->indices[1]], aNavmesh->meshVertices[node->indices[2]] });
					portals.push_back({ aNavmesh->meshVertices[node->indices[2]], aNavmesh->meshVertices[node->indices[0]] });
				}
			}

			portals.push_back({ anEnd, anEnd });
			if (aUseFunnel)
			{
				std::vector<DE::Vector3f> optimizedPath;
				FunnelPath(portals, optimizedPath);
				//std::reverse(optimizedPath.begin(), optimizedPath.end());
				return optimizedPath;
			}
			else
			{
				std::vector<DE::Vector3f> middlePointPath;
				ComputeMiddlePoints(portals, middlePointPath);
				//std::reverse(middlePointPath.begin(), middlePointPath.end());
				return middlePointPath;
			}
		}

		for (int next : copiedNodes[currentLocation.index].connections)
		{
			if (next < 0 || copiedNodes[next].state != eNodeState::Open)
			{
				continue;
			}
			float newCost = costSoFar[currentLocation.index] + (std::abs(copiedNodes[next].center.x - copiedNodes[currentLocation.index].center.x) + std::abs(copiedNodes[next].center.y - copiedNodes[currentLocation.index].center.y));
			if (costSoFar.find(next) == costSoFar.end() || newCost < costSoFar[next])
			{
				costSoFar[next] = newCost;
				float priority = newCost + GetHeuristic(copiedNodes[next].center, copiedNodes[endIndex].center);
				frontier.push({ priority, next });
				for (int connectedIndex : copiedNodes[next].connections)
				{
					if (connectedIndex < 0)
					{
						continue;
					}
					if (copiedNodes[connectedIndex].state == eNodeState::Unvisited)
					{
						copiedNodes[connectedIndex].state = eNodeState::Open;
					}
				}
				cameFrom[next] = currentLocation.index;
			}
		}
	}

	return std::vector<DE::Vector3f>();
}

std::vector<DE::Vector3f> DreamEngine::Pathfinding::FindShortestPath(DE::Vector3f aStart, DE::Vector3f anEnd, Navmesh aNavmesh, bool aUseFunnel)
{
	std::map<int, int> cameFrom;
	std::map<int, float> costSoFar;
	std::priority_queue<CostAndIndex> frontier;
	std::vector<Node> copiedNodes = aNavmesh.nodes;

	aStart = GetClosestPointOnNavmesh(aStart, aNavmesh);
	anEnd = GetClosestPointOnNavmesh(anEnd, aNavmesh);

	int startIndex = GetNodeIndexFromPoint(aStart, aNavmesh);
	int endIndex = GetNodeIndexFromPoint(anEnd, aNavmesh);
	if (startIndex == -1 || endIndex == -1)
	{
		return std::vector<DE::Vector3f>();
	}
	else if (startIndex == endIndex)
	{
		std::vector<DE::Vector3f> earlyRetVec;
		earlyRetVec.push_back(anEnd);
		return earlyRetVec;
	}

	frontier.push({ 0, startIndex });
	copiedNodes[startIndex].state = eNodeState::Closed;
	for (int connectedIndex : copiedNodes[startIndex].connections)
	{
		if (connectedIndex < 0)
		{
			continue;
		}
		copiedNodes[connectedIndex].state = eNodeState::Open;
	}
	cameFrom.insert({ startIndex, startIndex });
	costSoFar.insert({ startIndex, 0.f });

	while (!frontier.empty())
	{
		CostAndIndex currentLocation = frontier.top();
		frontier.pop();
		copiedNodes[currentLocation.index].state = eNodeState::Closed;

		if (currentLocation.index == endIndex)
		{
			std::vector<Node*> path;
			if (cameFrom.find(endIndex) == cameFrom.end())
			{
				return std::vector<DE::Vector3f>();
			}
			std::vector<Edge> portals;
			portals.reserve(path.size() * 3 + 1);
			int current = cameFrom[endIndex];
			path.push_back(&copiedNodes[endIndex]);
			while (current != startIndex)
			{
				path.push_back(&copiedNodes[current]);
				current = cameFrom[current];
			}
			path.push_back(&copiedNodes[startIndex]);
			std::reverse(path.begin(), path.end());

			portals.push_back({ aStart, aStart });
			bool skippedFirst = false;
			portals.push_back({ aNavmesh.meshVertices[aNavmesh.nodes[startIndex].indices[0]], aNavmesh.meshVertices[aNavmesh.nodes[startIndex].indices[1]] });
			portals.push_back({ aNavmesh.meshVertices[aNavmesh.nodes[startIndex].indices[1]], aNavmesh.meshVertices[aNavmesh.nodes[startIndex].indices[2]] });
			portals.push_back({ aNavmesh.meshVertices[aNavmesh.nodes[startIndex].indices[2]], aNavmesh.meshVertices[aNavmesh.nodes[startIndex].indices[0]] });
			for (auto node : path)
			{
				if (!skippedFirst)
				{
					skippedFirst = true;
					continue;
				}

				for (int i = (int)portals.size() - 3; i < portals.size(); i++)
				{
					int sharedVerts = 0;
					for (int y = 0; y < 3; y++)
					{
						if (aNavmesh.meshVertices[node->indices[y]] == portals[i].firstPoint)
						{
							sharedVerts++;
						}
						else if (aNavmesh.meshVertices[node->indices[y]] == portals[i].secondPoint)
						{
							sharedVerts++;
						}
					}
					if (sharedVerts < 2)
					{
						portals[i] = portals.back();
						portals.pop_back();
						i--;
					}
				}

				if (node != path.back())
				{
					portals.push_back({ aNavmesh.meshVertices[node->indices[0]], aNavmesh.meshVertices[node->indices[1]] });
					portals.push_back({ aNavmesh.meshVertices[node->indices[1]], aNavmesh.meshVertices[node->indices[2]] });
					portals.push_back({ aNavmesh.meshVertices[node->indices[2]], aNavmesh.meshVertices[node->indices[0]] });
				}
			}

			portals.push_back({ anEnd, anEnd });
			if (aUseFunnel)
			{
				std::vector<DE::Vector3f> optimizedPath;
				FunnelPath(portals, optimizedPath);
				std::reverse(optimizedPath.begin(), optimizedPath.end());
				return optimizedPath;
			}
			else
			{
				std::vector<DE::Vector3f> middlePointPath;
				ComputeMiddlePoints(portals, middlePointPath);
				std::reverse(middlePointPath.begin(), middlePointPath.end());
				return middlePointPath;
			}
		}

		for (int next : copiedNodes[currentLocation.index].connections)
		{
			if (next < 0 || copiedNodes[next].state != eNodeState::Open)
			{
				continue;
			}
			float newCost = costSoFar[currentLocation.index] + (std::abs(copiedNodes[next].center.x - copiedNodes[currentLocation.index].center.x) + std::abs(copiedNodes[next].center.y - copiedNodes[currentLocation.index].center.y));
			if (costSoFar.find(next) == costSoFar.end() || newCost < costSoFar[next])
			{
				costSoFar[next] = newCost;
				float priority = newCost + GetHeuristic(copiedNodes[next].center, copiedNodes[endIndex].center);
				frontier.push({ priority, next });
				for (int connectedIndex : copiedNodes[next].connections)
				{
					if (connectedIndex < 0)
					{
						continue;
					}
					if (copiedNodes[connectedIndex].state == eNodeState::Unvisited)
					{
						copiedNodes[connectedIndex].state = eNodeState::Open;
					}
				}
				cameFrom[next] = currentLocation.index;
			}
		}
	}

	return std::vector<DE::Vector3f>();
}


void DreamEngine::Pathfinding::FunnelPath(const std::vector<Edge>& portals, std::vector<DE::Vector3f>& outPath)
{
	DE::Vector3f funnelApex = portals[0].firstPoint;
	DE::Vector3f funnelLeft = portals[0].firstPoint;
	DE::Vector3f funnelRight = portals[0].secondPoint;
	DE::Vector3f firstMidPoint = FindMidpoint(portals[1].firstPoint, portals[1].secondPoint);
	if (IsToTheRightOfLine(funnelApex, firstMidPoint, portals[1].firstPoint))
	{
		funnelRight = portals[1].firstPoint;
		funnelLeft = portals[1].secondPoint;
	}
	else
	{
		funnelRight = portals[1].secondPoint;
		funnelLeft = portals[1].firstPoint;
	}


	int apexIndex = 0, leftIndex = 1, rightIndex = 1;
	apexIndex; leftIndex; rightIndex;
	//outPath.push_back(funnelApex);

	for (int i = 2; i < portals.size(); ++i)
	{
		if (portals[i].firstPoint == funnelApex || portals[i].secondPoint == funnelApex)
		{
			continue;
		}

		DE::Vector3f portalLeft;
		DE::Vector3f portalRight;
		DE::Vector3f nextMidPoint = FindMidpoint(portals[i].firstPoint, portals[i].secondPoint);
		if (IsToTheRightOfLine(funnelApex, nextMidPoint, portals[i].firstPoint))
		{
			portalRight = portals[i].firstPoint;
			portalLeft = portals[i].secondPoint;
		}
		else
		{
			portalRight = portals[i].secondPoint;
			portalLeft = portals[i].firstPoint;
		}
		if (funnelLeft == funnelApex)
		{
			funnelLeft = portalLeft;
			leftIndex = i;
		}
		if (funnelRight == funnelApex)
		{
			funnelRight = portalRight;
			rightIndex = i;
		}

		if (IsToTheLeftOfLine(funnelApex, portalLeft, funnelLeft))
		{
			if (IsToTheRightOfLine(funnelApex, funnelRight, portalLeft))
			{
				// New Apex funnel right
				funnelApex = funnelRight;
				funnelLeft = funnelApex;
				i = rightIndex;
				outPath.push_back(funnelApex);
			}
			else
			{
				// Narrow left
				funnelLeft = portalLeft;
				leftIndex = i;
			}
		}
		if (IsToTheRightOfLine(funnelApex, portalRight, funnelRight))
		{
			if (IsToTheLeftOfLine(funnelApex, funnelLeft, portalRight))
			{
				// New Apex funnel left
				funnelApex = funnelLeft;
				funnelRight = funnelApex;
				i = leftIndex;
				outPath.push_back(funnelApex);
			}
			else
			{
				// Narrow right
				funnelRight = portalRight;
				rightIndex = i;
			}
		}
	}

	outPath.push_back(portals.back().firstPoint);
}

void DreamEngine::Pathfinding::ComputeMiddlePoints(const std::vector<Edge>& portals, std::vector<DE::Vector3f>& outPath)
{
	outPath.clear();

	for (const auto& portal : portals)
	{
		DE::Vector3f middlePoint = (portal.firstPoint + portal.secondPoint) * 0.5f;
		outPath.push_back(middlePoint);
	}
}

DE::Vector3f DreamEngine::Pathfinding::FindMidpoint(const DE::Vector3f& aFirstPos, const DE::Vector3f& aSecondPos)
{
	return DE::Vector3f((aFirstPos.x + aSecondPos.x) / 2.0f, 0.0f, (aFirstPos.z + aSecondPos.z) / 2.0f);
}

float DreamEngine::Pathfinding::GetHeuristic(DE::Vector3f a, DE::Vector3f b)
{
	return (float)std::abs(a.x - b.x) + (float)std::abs(a.z - b.z);
}

bool DreamEngine::Pathfinding::IsToTheLeftOfLine(DE::Vector3f aLineStart, DE::Vector3f aLineEnd, DE::Vector3f aPoint)
{
	auto result = (aLineStart.x - aPoint.x) * (aLineEnd.z - aPoint.z) - (aLineEnd.x - aPoint.x) * (aLineStart.z - aPoint.z);
	if (result > 0.0f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DreamEngine::Pathfinding::GetNearestEdge(std::shared_ptr<Navmesh> aNavmesh, DE::Vector3f aPosition, float& anEdgeDistance, DE::Vector3f& anEdgeNormal)
{
	if (!aNavmesh) return false;
	
	// Get the correct grid cell from position
	std::pair<int, int> cellCoord = aNavmesh->grid.GetCellFromPosition(aPosition);
	int row = cellCoord.first;
	int col = cellCoord.second;

	int cellIndex = row * aNavmesh->grid.cols + col;

	if (cellIndex < 0 || cellIndex >= aNavmesh->grid.edgeCells.size())
	{
		return false; // Position is outside the grid
	}

	float closestDistance = FLT_MAX;
	bool foundEdge = false;

	// Iterate through edges in this cell
	for (int edgeIndex : aNavmesh->grid.edgeCells[cellIndex])
	{
		const NavmeshEdge& edge = aNavmesh->edges[edgeIndex];
		float distance = ComputePointToLineDistance(aPosition, edge.start, edge.end);

		if (distance < closestDistance)
		{
			closestDistance = distance;
			anEdgeNormal = edge.normal;
			foundEdge = true;
		}
	}

	// If an edge was found, return the closest distance
	if (foundEdge)
	{
		anEdgeDistance = closestDistance;
		return true;
	}

	return false;
}

float DreamEngine::Pathfinding::ComputePointToLineDistance(const DE::Vector3f& point, const DE::Vector3f& lineStart, const DE::Vector3f& lineEnd)
{
	DE::Vector3f edgeVector = lineEnd - lineStart;
	float edgeLengthSquared = edgeVector.LengthSqr(); 

	if (edgeLengthSquared == 0.0f)
		return (point - lineStart).Length();

	// Compute the projection of point onto the infinite line
	DE::Vector3f pointVector = point - lineStart;
	float t = pointVector.Dot(edgeVector) / edgeLengthSquared;

	// Clamp t to stay within the segment (0 = lineStart, 1 = lineEnd)
	t = std::max(0.0f, std::min(1.0f, t));

	// Find the closest point on the segment
	DE::Vector3f closestPoint = lineStart + edgeVector * t;

	// Return the distance from the point to the closest point on the segment
	return (point - closestPoint).Length();
}

bool DreamEngine::Pathfinding::IsToTheRightOfLine(DE::Vector3f aLineStart, DE::Vector3f aLineEnd, DE::Vector3f aPoint)
{
	auto result = (aLineStart.x - aPoint.x) * (aLineEnd.z - aPoint.z) - (aLineEnd.x - aPoint.x) * (aLineStart.z - aPoint.z);
	if (result < 0.0f)
	{
		return true;
	}
	else
	{
		return false;
	}
}