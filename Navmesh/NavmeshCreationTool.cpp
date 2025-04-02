#include "stdafx.h"

#include "NavmeshCreationTool.h"

#include <imgui\imgui.h>

#include <DreamEngine\engine.h>
#include <DreamEngine/windows/ErrorManager.h>
#include <DreamEngine/graphics/Model.h>
#include <DreamEngine\debugging\LineDrawer.h>
#include <DreamEngine\debugging\LinePrimitive.h>

#include <DreamEngine/math/Matrix.h>
#include <DreamEngine\math\Intersection.h>
#include <DreamEngine\math\Collider.h>

#include "GameObject.h"
#include "MainSingleton.h"

#include <unordered_set>

#include <fstream>
#include <filesystem>

#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>  
#include <ctime>    

namespace fs = std::filesystem;

using namespace DreamEngine;

static rcConfig cfg;

struct pair_hash
{
	template <class T1, class T2>
	std::size_t operator () (const std::pair<T1, T2>& pair) const {
		return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
	}
};

float RandomFloat()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);  // Generates a float between 0 and 1
}

static void SaveNavmesh(const rcPolyMesh* pmesh, const rcPolyMeshDetail* dmesh, const char* filename)
{
	std::ofstream file(filename, std::ios::binary);

	if (!file.is_open())
	{
		ERROR_PRINT("Can't open file to write navmesh");
		return;
	}

	file.write(reinterpret_cast<const char*>(&pmesh->nverts), sizeof(pmesh->nverts));
	file.write(reinterpret_cast<const char*>(pmesh->verts), pmesh->nverts * 3 * sizeof(unsigned short));

	file.write(reinterpret_cast<const char*>(&pmesh->npolys), sizeof(pmesh->npolys));
	file.write(reinterpret_cast<const char*>(pmesh->polys), pmesh->npolys * pmesh->nvp * 2 * sizeof(unsigned short));
	file.write(reinterpret_cast<const char*>(pmesh->flags), pmesh->npolys * sizeof(unsigned short));
	file.write(reinterpret_cast<const char*>(pmesh->areas), pmesh->npolys * sizeof(unsigned char));

	// Write PolyMeshDetail data
	file.write(reinterpret_cast<const char*>(&dmesh->nmeshes), sizeof(dmesh->nmeshes));
	file.write(reinterpret_cast<const char*>(dmesh->meshes), dmesh->nmeshes * 4 * sizeof(unsigned int));

	file.write(reinterpret_cast<const char*>(&dmesh->nverts), sizeof(dmesh->nverts));
	file.write(reinterpret_cast<const char*>(dmesh->verts), dmesh->nverts * 3 * sizeof(float));

	file.write(reinterpret_cast<const char*>(&dmesh->ntris), sizeof(dmesh->ntris));
	file.write(reinterpret_cast<const char*>(dmesh->tris), dmesh->ntris * 4 * sizeof(unsigned char));

	file.close();
}

static void ExportNavMeshToOBJ(const rcPolyMesh& pmesh, const rcPolyMeshDetail& dmesh, const std::string& filename) 
{
	
	// pmesh
	{
		std::ofstream file(filename + ".obj");
		if (!file.is_open()) {
			std::cerr << "Failed to open file for writing: " << filename << std::endl;
			return;
		}

		// Export vertices
		for (int i = 0; i < pmesh.nverts; ++i) {
			const unsigned short* v = &pmesh.verts[i * 3];
			file << "v " << v[0] * pmesh.cs << " " << v[1] * pmesh.ch << " " << v[2] * pmesh.cs << "\n";
		}

		// Export polygons
		for (int i = 0; i < pmesh.npolys; ++i) {
			file << "f ";
			for (int j = 0; j < pmesh.nvp; ++j) {
				if (pmesh.polys[i * 2 * pmesh.nvp + j] == RC_MESH_NULL_IDX)
					break;
				file << (pmesh.polys[i * 2 * pmesh.nvp + j] + 1) << " ";
			}
			file << "\n";
		}

		file.close();
	}

	// dmesh
	{
		std::ofstream file(filename + "_d.obj");

		if (!file.is_open())
		{
			// error
			return;
		}

		for (size_t i = 0; i < dmesh.nverts; ++i)
		{
			const float* v = &dmesh.verts[i * 3];
			file << "v " << v[0] << " " << v[1] << " " << v[2] << "\n";
		}

		for (size_t i = 0; i < dmesh.nmeshes; ++i)
		{
			const unsigned int* m = &dmesh.meshes[i * 4];
			const int vbase = m[0];
			const int tbase = m[2];
			const int ntris = m[3];

			for (size_t j = 0; j < ntris; ++j)
			{
				const unsigned char* t = &dmesh.tris[(tbase + j) * 4];
				const int v0 = vbase + t[0] + 1;
				const int v1 = vbase + t[1] + 1;
				const int v2 = vbase + t[2] + 1;

				file << "f " << v0 << " " << v1 << " " << v2 << "\n";
			}
		}
		file.close();
	}
}

DreamEngine::NavmeshCreationTool::~NavmeshCreationTool()
{
	//if (myPolyMeshDetail.verts)
	//{
	//	delete[] myPolyMeshDetail.verts;
	//}
	//if (myPolyMeshDetail.tris)
	//{
	//	delete[] myPolyMeshDetail.tris;
	//}
	//if (myPolyMeshDetail.meshes)
	//{
	//	delete[] myPolyMeshDetail.meshes;
	//}
}

void NavmeshCreationTool::Init(std::vector<std::shared_ptr<DreamEngine::ModelInstance>> someSceneObjects, bool aShouldBuildDirectly)
{
	myMeshes = someSceneObjects;

	memset(&cfg, 0, sizeof(cfg));
	cfg.cs = 1.f * 25; // m_cellSize
	cfg.ch = 0.2f; // cell height
	cfg.walkableSlopeAngle = 43.f;

	cfg.walkableHeight = (int)ceilf(myAgentHeight / cfg.ch);
	cfg.walkableClimb = (int)floorf(myAgentMaxClimb / cfg.ch);
	cfg.walkableRadius = (int)ceilf(myAgentRadius / cfg.cs);
	cfg.maxVertsPerPoly = 3; // Trying with 3 for triangles

	cfg.maxEdgeLen = (int)(12.f / cfg.cs);
	cfg.maxSimplificationError = 1.3f;

	cfg.minRegionArea = (int)rcSqr(8);
	cfg.mergeRegionArea = (int)rcSqr(20);

	cfg.tileSize = 64;

	cfg.detailSampleDist = cfg.cs * 6.0f;
	cfg.detailSampleMaxError = cfg.ch * 1.0f;

	if (aShouldBuildDirectly)
	{
		myAgentHeight = 2.0f;
		myAgentMaxClimb = 0.9f;
		myAgentRadius = 0.2f;


		cfg.cs = 0.1f;
		cfg.ch = 0.1f;
		cfg.walkableSlopeAngle = 45.f;
		cfg.walkableHeight = (int)ceilf(myAgentHeight / cfg.ch);
		cfg.walkableClimb = (int)floorf(myAgentMaxClimb / cfg.ch);
		cfg.walkableRadius = (int)ceilf(myAgentRadius / cfg.cs);
		cfg.maxEdgeLen = 120;
		cfg.maxSimplificationError = 1.3f;
		cfg.minRegionArea = 64;		// Note: area = size*size
		cfg.mergeRegionArea = 400;	// Note: area = size*size
		cfg.maxVertsPerPoly = 3;
		cfg.detailSampleDist = 0.6f;
		cfg.detailSampleMaxError = 0.1f;
		cfg.tileSize = 0; // ??

		// cfg DEFAULT SETTINGS for scaled down matrix
		BuildNavmesh();
	}

}

void NavmeshCreationTool::HandleImGUI()
{
	ImGui::Begin("Navmesh");
	{
		ImGui::DragFloat("Cell size", &cfg.cs);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("The xz-plane cell size to use for fields. [Limit: > 0] [Units: wu]");
		}

		ImGui::DragFloat("Cell height", &cfg.ch);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("The y-axis cell size to use for fields. [Limit: > 0] [Units: wu]");
		}

		ImGui::SliderFloat("Walkable slope angle", &cfg.walkableSlopeAngle, 0.f, 90.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("The maximum slope that is considered walkable. [Limits: 0 <= value < 90] [Units: Degrees]");
		}

		ImGui::DragInt("Max Edge Lenght", &cfg.maxEdgeLen);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("The maximum allowed length for contour edges along the border of the mesh. [Limit: >=0] [Units: vx]");
		}

		ImGui::Separator();
		ImGui::SliderFloat("Max Simplification Error", &cfg.maxSimplificationError, 0.f, 10.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("The maximum distance a simplified contour's border edges should deviate the original raw contour. [Limit: >=0] [Units: vx]");
		}

		ImGui::DragInt("Min Region Area", &cfg.minRegionArea);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("The minimum number of cells allowed to form isolated island areas. [Limit: >=0] [Units: vx]");
		}

		ImGui::DragInt("Merge Region Area", &cfg.mergeRegionArea);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Any regions with a span count smaller than this value will, if possible, be merged with larger regions. [Limit: >=0] [Units: vx]");
		}

		ImGui::DragInt("Max Verts per Polygon", &cfg.maxVertsPerPoly);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("The maximum number of vertices allowed for polygons generated during the contour to polygon conversion process. [Limit: >= 3]");
		}

		ImGui::DragFloat("Detail sample distance", &cfg.detailSampleDist);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Sets the sampling distance to use when generating the detail mesh. (For height detail only.) [Limits: 0 or >= 0.9] [Units: wu]");
		}

		ImGui::DragFloat("detail sample max error", &cfg.detailSampleMaxError);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("The maximum distance the detail mesh surface should deviate from heightfield data. (For height detail only.) [Limit: >=0] [Units: wu]");
		}

		if (ImGui::Button("Generate SoloMesh"))
		{
			BuildNavmesh();
		}

		if (ImGui::Button("Save PolyMesh to Navmesh Struct"))
		{
			HandleNavmeshStruct(false);
		}
		ImGui::Separator();
		ImGui::Text("Agent");
		ImGui::DragFloat("Agent Height", &myAgentHeight);
		ImGui::DragFloat("Agent Radius", &myAgentRadius);
		ImGui::DragFloat("Agent Max Climb", &myAgentMaxClimb);
		ImGui::Separator();
		ImGui::Separator();

		ImGui::Text("NavmeshStruct");
		ImGui::Checkbox("Show Navmesh Struct", &myNavmeshStruct);

		ImGui::Text("PolyMeshDetail");
		ImGui::Checkbox("Show PolyMeshDetail", &myShowPolyMeshDetail);
		ImGui::DragFloat("Debug Draw X offset PMB", &myDrawXOffsetDetail);
		ImGui::DragFloat("Debug Draw Y offset PMB", &myDrawYOffsetDetail);
		ImGui::DragFloat("Debug Draw Z offset PMB", &myDrawZOffsetDetail);
		ImGui::Separator();
		ImGui::Text("PolyMesh");
		ImGui::Checkbox("Show PolyMesh", &myShowPolyMesh);
		ImGui::DragFloat("Debug Draw X offset PM", &myDrawXOffset);
		ImGui::DragFloat("Debug Draw Y offset PM", &myDrawYOffset);
		ImGui::DragFloat("Debug Draw Z offset PM", &myDrawZOffset);
		ImGui::Checkbox("Show Detailed Grid", &myShowDetailedNavmeshGrid);
		ImGui::Checkbox("Show Polymesh Grid", &myShowNavmeshGrid);

		ImGui::Separator();
		ImGui::Checkbox("Use Debug Ray", &myUseDebugRay);

		if (myUseDebugRay)
		{
			ImGui::Indent();

			ImGui::Checkbox("Ignore Y and Collect Intersections", &myIgnoreYAndCollectIntersections);
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Allows user to shoot a ray from screen space towards navmesh to view intersections along ray. Ignoring Y.");
			}
			if (myIgnoreYAndCollectIntersections) myFindClosestIntersectionOnly = false;

			ImGui::Checkbox("Find Closest Intersection Only", &myFindClosestIntersectionOnly);
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Allows user to shoot a ray from screen space towards navmesh to set intersection points for path");
			}
			if (myFindClosestIntersectionOnly) myIgnoreYAndCollectIntersections = false;

			if (myFindClosestIntersectionOnly) // Only show these options if Use FindClosestIntersectionOnly is enabled
			{
				ImGui::Indent();
				if (ImGui::Button("Reset Path"))
				{
					myPathFrom = { 0.f,0.f,0.f };
					myPathTo = { 0.f,0.f,0.f };
					myCanPathfind = false;
					myShowPath = false;
				}
					ImGui::Indent();
					if (myPathFrom != DE::Vector3f{ 0.f,0.f,0.f } && myPathTo != DE::Vector3f{ 0.f,0.f,0.f } && !myShowPath)
					{
						if (ImGui::Button("Pathfind from points with Funneling"))
						{
							myPath = DE::Pathfinding::FindShortestPath(myPathFrom, myPathTo, myNavmesh, true);	// NAVMESH
							myShowPath = true;
							myHavePathToGive = true;
						}

						if (ImGui::Button("Pathfind from without Funneling"))
						{
							myPath = DE::Pathfinding::FindShortestPath(myPathFrom, myPathTo, myNavmesh, false);	// NAVMESH
							myShowPath = true;
							myHavePathToGive = true;
						}
					}
					ImGui::Unindent();
				ImGui::Unindent();
			}

			ImGui::Unindent(); // Undo indentation
		}

	}
	ImGui::End();
}

void DreamEngine::NavmeshCreationTool::RenderNavmeshCentersAndConnections()
{
	DreamEngine::LineDrawer& lineDrawer = DreamEngine::Engine::GetInstance()->GetGraphicsEngine().GetLineDrawer();
	DreamEngine::LinePrimitive lineToDraw;
	DreamEngine::Matrix4x4f scaleMatrix;
	DreamEngine::Vector4f scalingVertex;

	if (!myPolyMesh.verts || !myPolyMesh.polys)
	{
		return;
	}

	float xOffset = myDrawXOffset;  // X offset for vertices
	float yOffset = myDrawYOffset;  // Y offset for vertices
	float zOffset = myDrawZOffset;  // Z offset for vertices

	// Iterate over each polygon (triangle) in the navmesh
	for (int i = 0; i < myPolyMesh.npolys; ++i)
	{
		// Calculate the center of the current triangle
		const unsigned short* p = &myPolyMesh.polys[i * 2 * myPolyMesh.nvp];

		DreamEngine::Vector3f v0(myPolyMesh.verts[p[0] * 3] * myPolyMesh.cs + myPolyMesh.bmin[0], myPolyMesh.verts[p[0] * 3 + 1] * myPolyMesh.ch + myPolyMesh.bmin[1], myPolyMesh.verts[p[0] * 3 + 2] * myPolyMesh.cs + myPolyMesh.bmin[2]);
		DreamEngine::Vector3f v1(myPolyMesh.verts[p[1] * 3] * myPolyMesh.cs + myPolyMesh.bmin[0], myPolyMesh.verts[p[1] * 3 + 1] * myPolyMesh.ch + myPolyMesh.bmin[1], myPolyMesh.verts[p[1] * 3 + 2] * myPolyMesh.cs + myPolyMesh.bmin[2]);
		DreamEngine::Vector3f v2(myPolyMesh.verts[p[2] * 3] * myPolyMesh.cs + myPolyMesh.bmin[0], myPolyMesh.verts[p[2] * 3 + 1] * myPolyMesh.ch + myPolyMesh.bmin[1], myPolyMesh.verts[p[2] * 3 + 2] * myPolyMesh.cs + myPolyMesh.bmin[2]);


		DreamEngine::Vector3f center = (v0 + v1 + v2) / 3.0f;

		// Apply offset to draw above the ground
		center.x += xOffset;
		center.y += yOffset;
		center.z += zOffset;

		// Draw the center as a point 
		lineToDraw.fromPosition = center;
		lineToDraw.toPosition = center + DreamEngine::Vector3f(0.f, 20.f, 0.f);  // offset to mark the center
		lineToDraw.color = { 1.0f, 0.0f, 0.0f, 1.0f };  // Red color for centers
		scalingVertex = lineToDraw.fromPosition;
		scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
		lineToDraw.fromPosition = scalingVertex;

		scalingVertex = lineToDraw.toPosition;
		scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
		lineToDraw.toPosition = scalingVertex;
		lineDrawer.Draw(lineToDraw);

		// iterate over the polygon's neighbors (stored after the vertices in the polys array)
		for (int j = 0; j < myPolyMesh.nvp; ++j)  // nvp is the number of vertices per polygon
		{
			unsigned short neighbor = myPolyMesh.polys[i * 2 * myPolyMesh.nvp + myPolyMesh.nvp + j];  // Neighbor polygon index

			if (neighbor != RC_MESH_NULL_IDX && neighbor < myPolyMesh.npolys) // If a valid neighbor exists
			{
				// Calculate the center of the neighbor polygon
				const unsigned short* np = &myPolyMesh.polys[neighbor * 2 * myPolyMesh.nvp];

				DreamEngine::Vector3f nv0(myPolyMesh.verts[np[0] * 3] * myPolyMesh.cs + myPolyMesh.bmin[0], myPolyMesh.verts[np[0] * 3 + 1] * myPolyMesh.ch + myPolyMesh.bmin[1], myPolyMesh.verts[np[0] * 3 + 2] * myPolyMesh.cs + myPolyMesh.bmin[2]);
				DreamEngine::Vector3f nv1(myPolyMesh.verts[np[1] * 3] * myPolyMesh.cs + myPolyMesh.bmin[0], myPolyMesh.verts[np[1] * 3 + 1] * myPolyMesh.ch + myPolyMesh.bmin[1], myPolyMesh.verts[np[1] * 3 + 2] * myPolyMesh.cs + myPolyMesh.bmin[2]);
				DreamEngine::Vector3f nv2(myPolyMesh.verts[np[2] * 3] * myPolyMesh.cs + myPolyMesh.bmin[0], myPolyMesh.verts[np[2] * 3 + 1] * myPolyMesh.ch + myPolyMesh.bmin[1], myPolyMesh.verts[np[2] * 3 + 2] * myPolyMesh.cs + myPolyMesh.bmin[2]);

				DreamEngine::Vector3f neighborCenter = (nv0 + nv1 + nv2) / 3.0f;

				// Apply offset to neighbor center
				neighborCenter.x += xOffset;
				neighborCenter.y += yOffset;
				neighborCenter.z += zOffset;

				// Draw the line between the current triangle's center and its neighbor's center
				lineToDraw.fromPosition = center;
				lineToDraw.toPosition = neighborCenter;
				lineToDraw.color = { 0.0f, 1.0f, 0.0f, 1.0f };  // Green color for connections

				scalingVertex = lineToDraw.fromPosition;
				scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
				lineToDraw.fromPosition = scalingVertex;

				scalingVertex = lineToDraw.toPosition;
				scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
				lineToDraw.toPosition = scalingVertex;

				lineDrawer.Draw(lineToDraw);
			}
		}

		v0.x += xOffset;
		v0.y += yOffset;
		v0.z += zOffset;

		v1.x += xOffset;
		v1.y += yOffset;
		v1.z += zOffset;

		v2.x += xOffset;
		v2.y += yOffset;
		v2.z += zOffset;

		// Draw lines between the vertices of the polygon (v0 -> v1 -> v2 -> v0)
		lineToDraw.fromPosition = v0;
		lineToDraw.toPosition = v1;
		lineToDraw.color = { 1.0f, 1.0f, 0.0f, 1.0f };  // Yellow color for polygon edges
		scalingVertex = lineToDraw.fromPosition;
		scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
		lineToDraw.fromPosition = scalingVertex;

		scalingVertex = lineToDraw.toPosition;
		scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
		lineToDraw.toPosition = scalingVertex;
		lineDrawer.Draw(lineToDraw);

		lineToDraw.fromPosition = v1;
		lineToDraw.toPosition = v2;
		scalingVertex = lineToDraw.fromPosition;
		scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
		lineToDraw.fromPosition = scalingVertex;

		scalingVertex = lineToDraw.toPosition;
		scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
		lineToDraw.toPosition = scalingVertex;
		lineDrawer.Draw(lineToDraw);

		lineToDraw.fromPosition = v2;
		lineToDraw.toPosition = v0;
		scalingVertex = lineToDraw.fromPosition;
		scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
		lineToDraw.fromPosition = scalingVertex;

		scalingVertex = lineToDraw.toPosition;
		scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
		lineToDraw.toPosition = scalingVertex;
		lineDrawer.Draw(lineToDraw);
	}
}

void DreamEngine::NavmeshCreationTool::RenderNavmeshDetailedFromRecastStruct()
{
	DreamEngine::LineDrawer& lineDrawer = DreamEngine::Engine::GetInstance()->GetGraphicsEngine().GetLineDrawer();
	DreamEngine::LinePrimitive lineToDraw;
	DreamEngine::Matrix4x4f scaleMatrix;
	DreamEngine::Vector4f scalingVertex;

	float yOffset = myDrawYOffsetDetail;

	if (!myPolyMeshDetail.verts || !myPolyMeshDetail.tris)
	{
		return;
	}

	// Internal edges
	for (int i = 0; i < myPolyMeshDetail.nmeshes; ++i)
	{
		const unsigned int* m = &myPolyMeshDetail.meshes[i * 4];
		const unsigned int bverts = m[0];
		const unsigned int btris = m[2];
		const int ntris = (int)m[3];
		const float* verts = &myPolyMeshDetail.verts[bverts * 3];
		const unsigned char* tris = &myPolyMeshDetail.tris[btris * 4];

		for (int j = 0; j < ntris; ++j)
		{
			const unsigned char* t = &tris[j * 4];
			for (int k = 0, kp = 2; k < 3; kp = k++)
			{
				unsigned char ef = (t[3] >> (kp * 2)) & 0x3;

				// Draw both internal and external edges
				if (ef == 0) // Internal edge
				{
					lineToDraw.fromPosition = { verts[t[kp] * 3], verts[t[kp] * 3 + 1] + yOffset, verts[t[kp] * 3 + 2] };
					lineToDraw.toPosition = { verts[t[k] * 3], verts[t[k] * 3 + 1] + yOffset, verts[t[k] * 3 + 2] };
					scalingVertex = lineToDraw.fromPosition;
					scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
					lineToDraw.fromPosition = scalingVertex;

					scalingVertex = lineToDraw.toPosition;
					scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
					lineToDraw.toPosition = scalingVertex;

					// Calculate the average height between the two vertices
					float avgHeight = (lineToDraw.fromPosition.y + lineToDraw.toPosition.y) / 2.0f;

					// Determine the color based on the height
					if (avgHeight <= -500.0f)
					{
						lineToDraw.color = { 1.0f, 1.0f, 0.0f, 1.0f };  // Yellow
					}
					else if (avgHeight >= 500.0f)
					{
						lineToDraw.color = { 1.0f, 0.0f, 0.0f, 1.0f };  // Red
					}
					else
					{
						// Interpolate between yellow and red to show height differences
						float h = (avgHeight + 500.0f) / 1000.0f;  // Normalize to [0, 1]
						lineToDraw.color = 
						{
							1.0f,       
							1.0f - h,   
							0.0f,       
							1.0f        
						};
					}

					lineDrawer.Draw(lineToDraw);
				}
				else // External edge
				{
					lineToDraw.fromPosition = { verts[t[kp] * 3], verts[t[kp] * 3 + 1] + yOffset, verts[t[kp] * 3 + 2] };
					lineToDraw.toPosition = { verts[t[k] * 3], verts[t[k] * 3 + 1] + yOffset, verts[t[k] * 3 + 2] };
					scalingVertex = lineToDraw.fromPosition;
					scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
					lineToDraw.fromPosition = scalingVertex;

					scalingVertex = lineToDraw.toPosition;
					scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
					lineToDraw.toPosition = scalingVertex;

					// Calculate the average height between the two vertices
					float avgHeight = (lineToDraw.fromPosition.y + lineToDraw.toPosition.y) / 2.0f;

					// Determine the color based on the height
					if (avgHeight <= -500.0f)
					{
						lineToDraw.color = { 1.0f, 1.0f, 0.0f, 1.0f };  // Yellow
					}
					else if (avgHeight >= 500.0f)
					{
						lineToDraw.color = { 1.0f, 0.0f, 0.0f, 1.0f };  // Red
					}
					else
					{
						// Interpolate between yellow and red to show height differences
						float h = (avgHeight + 500.0f) / 1000.0f;  // Normalize to [0, 1]
						lineToDraw.color = 
						{
							1.0f,        
							1.0f - h,    
							0.0f,        
							1.0f         
						};
					}

					lineDrawer.Draw(lineToDraw);
				}
			}
		}
	}
}

void DreamEngine::NavmeshCreationTool::RenderNavmeshStruct()
{
	DreamEngine::LineDrawer& lineDrawer = DreamEngine::Engine::GetInstance()->GetGraphicsEngine().GetLineDrawer();
	DreamEngine::LinePrimitive lineToDraw;
	DreamEngine::Matrix4x4f scaleMatrix;
	DreamEngine::Vector4f scalingVertex;

	for (const Node& node : myNavmesh.nodes)
	{
		DreamEngine::Vector3f v0 = myNavmesh.meshVertices[node.indices[0]];
		DreamEngine::Vector3f v1 = myNavmesh.meshVertices[node.indices[1]];
		DreamEngine::Vector3f v2 = myNavmesh.meshVertices[node.indices[2]];

		// Draw lines between the vertices of the polygon (v0 -> v1 -> v2 -> v0)
		lineToDraw.fromPosition = v0;
		lineToDraw.toPosition = v1;
		lineToDraw.color = { 1.0f, 1.0f, 0.0f, 1.0f };  // Yellow color for polygon edges
		lineDrawer.Draw(lineToDraw);

		lineToDraw.fromPosition = v1;
		lineToDraw.toPosition = v2;
		lineDrawer.Draw(lineToDraw);

		lineToDraw.fromPosition = v2;
		lineToDraw.toPosition = v0;
		lineDrawer.Draw(lineToDraw);

		// Draw a vertical line to mark the center of the triangle
		lineToDraw.fromPosition = node.center;
		lineToDraw.toPosition = node.center + DreamEngine::Vector3f(0.f, 20.f, 0.f);
		lineToDraw.color = { 1.0f, 0.0f, 0.0f, 1.0f };  // Red color for centers

		lineDrawer.Draw(lineToDraw);

		// Draw connections between centers of neighboring triangles
		for (int i = 0; i < 3; ++i)
		{
			int connectionIndex = node.connections[i];
			if (connectionIndex != -1)
			{
				// Get the neighbor's center
				const DreamEngine::Vector3f& neighborCenter = myNavmesh.nodes[connectionIndex].center;

				// Draw a line between the current node's center and its neighbor's center
				lineToDraw.fromPosition = node.center;
				lineToDraw.toPosition = neighborCenter;
				lineToDraw.color = { 0.0f, 1.0f, 0.0f, 1.0f };  // Green color for connections
				lineDrawer.Draw(lineToDraw);
			}
		}
	}
}


void DreamEngine::NavmeshCreationTool::RenderDetailedNavmeshStruct()
{
	DreamEngine::LineDrawer& lineDrawer = DreamEngine::Engine::GetInstance()->GetGraphicsEngine().GetLineDrawer();
	DreamEngine::LinePrimitive lineToDraw;
	DreamEngine::Matrix4x4f scaleMatrix;
	DreamEngine::Vector4f scalingVertex;

	for (const Node& node : myDetailedNavmesh.nodes)
	{
		DreamEngine::Vector3f v0 = myDetailedNavmesh.meshVertices[node.indices[0]];
		DreamEngine::Vector3f v1 = myDetailedNavmesh.meshVertices[node.indices[1]];
		DreamEngine::Vector3f v2 = myDetailedNavmesh.meshVertices[node.indices[2]];

		// Draw lines between the vertices of the polygon (v0 -> v1 -> v2 -> v0)
		lineToDraw.fromPosition = v0;
		lineToDraw.toPosition = v1;
		lineToDraw.color = { 1.0f, 1.0f, 0.0f, 1.0f };  // Yellow color for polygon edges
		lineDrawer.Draw(lineToDraw);

		lineToDraw.fromPosition = v1;
		lineToDraw.toPosition = v2;
		lineDrawer.Draw(lineToDraw);

		lineToDraw.fromPosition = v2;
		lineToDraw.toPosition = v0;
		lineDrawer.Draw(lineToDraw);

		// Draw a vertical line to mark the center of the triangle
		lineToDraw.fromPosition = node.center;
		lineToDraw.toPosition = node.center + DreamEngine::Vector3f(0.f, 20.f, 0.f);
		lineToDraw.color = { 1.0f, 0.0f, 0.0f, 1.0f };  // Red color for centers

		lineDrawer.Draw(lineToDraw);
	}
}

void DreamEngine::NavmeshCreationTool::RenderNavmeshGrid()
{
	if (!myNavmesh.nodes.empty())
	{
		DreamEngine::LineDrawer& lineDrawer = DreamEngine::Engine::GetInstance()->GetGraphicsEngine().GetLineDrawer();
		DreamEngine::LinePrimitive lineToDraw;

		lineToDraw.color = { 0.0f, 0.5f, 0.1f, 1.0f };

		const auto& grid = myNavmesh.grid;

		// Draw vertical grid lines (columns)
		for (int j = 0; j <= grid.cols; ++j)
		{
			float x = grid.minBound.x + j * grid.cellWidth;
			// Draw a line from the bottom (minBound.y) to the top (maxBound.y) of the grid in Z direction
			lineToDraw.fromPosition = DreamEngine::Vector3f(x, 0.0f, grid.minBound.y);
			lineToDraw.toPosition = DreamEngine::Vector3f(x, 0.0f, grid.maxBound.y);
			lineDrawer.Draw(lineToDraw);
		}

		// Draw horizontal grid lines (rows)
		for (int i = 0; i <= grid.rows; ++i)
		{
			float z = grid.minBound.y + i * grid.cellHeight;
			// Draw a line from the left (minBound.x) to the right (maxBound.x) of the grid in X direction
			lineToDraw.fromPosition = DreamEngine::Vector3f(grid.minBound.x, 0.0f, z);
			lineToDraw.toPosition = DreamEngine::Vector3f(grid.maxBound.x, 0.0f, z);
			lineDrawer.Draw(lineToDraw);
		}
	}
}

void DreamEngine::NavmeshCreationTool::RenderDetailedNavmeshGrid()
{
	if (!myDetailedNavmesh.nodes.empty())
	{
		DreamEngine::LineDrawer& lineDrawer = DreamEngine::Engine::GetInstance()->GetGraphicsEngine().GetLineDrawer();
		DreamEngine::LinePrimitive lineToDraw;

		lineToDraw.color = { 0.0f, 0.5f, 0.1f, 1.0f };

		const auto& grid = myDetailedNavmesh.grid;

		// Draw vertical grid lines (columns)
		for (int j = 0; j <= grid.cols; ++j)
		{
			float x = grid.minBound.x + j * grid.cellWidth;
			// Draw a line from the bottom (minBound.y) to the top (maxBound.y) of the grid in Z direction
			lineToDraw.fromPosition = DreamEngine::Vector3f(x, 0.0f, grid.minBound.y);
			lineToDraw.toPosition = DreamEngine::Vector3f(x, 0.0f, grid.maxBound.y);
			lineDrawer.Draw(lineToDraw);
		}

		// Draw horizontal grid lines (rows)
		for (int i = 0; i <= grid.rows; ++i)
		{
			float z = grid.minBound.y + i * grid.cellHeight;
			// Draw a line from the left (minBound.x) to the right (maxBound.x) of the grid in X direction
			lineToDraw.fromPosition = DreamEngine::Vector3f(grid.minBound.x, 0.0f, z);
			lineToDraw.toPosition = DreamEngine::Vector3f(grid.maxBound.x, 0.0f, z);
			lineDrawer.Draw(lineToDraw);
		}
	}
}

void DreamEngine::NavmeshCreationTool::Render()
{
	if (myUseDebugRay)
	{
		RenderDebugRay();
		if (myShowPath)
		{
			DebugRenderPath();
		}
	}
	if (myNavmeshStruct)
	{
		RenderNavmeshStruct();
	}

	if (myShowPolyMeshDetail)
	{
		RenderNavmeshDetailedFromRecastStruct();
	}

	if (myShowPolyMesh)
	{
		RenderDetailedNavmeshStruct();
	}

	if (myShowNavmeshGrid)
	{
		RenderNavmeshGrid();
	}
	if (myShowDetailedNavmeshGrid)
	{
		RenderDetailedNavmeshGrid();
	}
}

void DreamEngine::NavmeshCreationTool::RenderDebugRay()
{

	DreamEngine::LineDrawer& lineDrawer = DreamEngine::Engine::GetInstance()->GetGraphicsEngine().GetLineDrawer();
	myLineToDraw.color = { 1.f,0.f,0.f,1.f };
	myLineToDrawFromClick.color = { 0.8f, 0.4f, 0.0f, 1.0f };

	lineDrawer.Draw(myLineToDraw);
	lineDrawer.Draw(myLineToDrawFromClick);

	DE::LinePrimitive intersectionPoint;
	if (!mySavedIntersections.empty() && myIgnoreYAndCollectIntersections)
	{
		for (auto point : mySavedIntersections)
		{
			intersectionPoint.fromPosition = point;
			intersectionPoint.toPosition = point;
			intersectionPoint.toPosition.y += 100.f;
			intersectionPoint.color = { 1.0f, 0.55f, 0.0f, 1.0f };

			lineDrawer.Draw(intersectionPoint);
		}
	}

	if (myPathTo != DE::Vector3f{0.0f,0.0f,0.0f} && myFindClosestIntersectionOnly)
	{
		intersectionPoint.fromPosition = myPathTo;
		intersectionPoint.toPosition = myPathTo;
		intersectionPoint.toPosition.y += 100.f;
		intersectionPoint.color = { 1.0f, 0.55f, 0.0f, 1.0f };

		lineDrawer.Draw(intersectionPoint);
	}
	if (myPathFrom != DE::Vector3f{ 0.0f,0.0f,0.0f } && myFindClosestIntersectionOnly)
	{
		intersectionPoint.fromPosition = myPathFrom;
		intersectionPoint.toPosition = myPathFrom;
		intersectionPoint.toPosition.y += 100.f;
		intersectionPoint.color = { 1.0f, 0.55f, 0.5f, 1.0f };

		lineDrawer.Draw(intersectionPoint);
	}

	{ // Draw Axes
		DE::LinePrimitive axis;
		DE::Vector3f origo = { 0.f,0.f,0.f };
		axis.fromPosition = origo;
		axis.fromPosition.y -= 10000.f;
		axis.toPosition = origo;
		axis.toPosition.y += 10000.f;
		axis.color = { 1.f,0.f,0.f,1.f };
		lineDrawer.Draw(axis);

		axis.fromPosition = origo;
		axis.fromPosition.x -= 10000.f;
		axis.toPosition = origo;
		axis.toPosition.x += 10000.f;
		axis.color = { 0.f,1.f,0.f,1.f };
		lineDrawer.Draw(axis);

		axis.fromPosition = origo;
		axis.fromPosition.z -= 10000.f;
		axis.toPosition = origo;
		axis.toPosition.z += 10000.f;
		axis.color = { 0.f,0.f,1.f,1.f };
		lineDrawer.Draw(axis);
	}
}

void DreamEngine::NavmeshCreationTool::DebugRenderPath()
{
	DreamEngine::LineDrawer& lineDrawer = DreamEngine::Engine::GetInstance()->GetGraphicsEngine().GetLineDrawer();
	DreamEngine::LinePrimitive lineToDraw;
	lineToDraw.color = { 0.f, 0.f, 1.0f, 1.0f };


	for (size_t i = 1; i < myPath.size(); i++)
	{
		lineToDraw.fromPosition = myPath[i - 1];
		lineToDraw.toPosition = myPath[i];
		lineDrawer.Draw(lineToDraw);
	}
}

void DreamEngine::NavmeshCreationTool::Update()
{
#ifndef _RETAIL
	HandleImGUI();
#endif

	DreamEngine::InputManager& inputManager = MainSingleton::GetInstance()->GetInputManager();
	DE::Ray<float> cameraRay(DE::Vector3f{0.f,0.f,0.f}, DE::Vector3f{ 0.f,0.f,0.f });
	cameraRay = GetRayFromMouseCLickPerspective();
	
	if (inputManager.IsKeyDown(DE::eKeyCode::LeftMouseButton))
	{
		if (myFindClosestIntersectionOnly)
		{
			DE::Vector3f intersectionPoint;
			if (CastRayAgainstNavmesh(cameraRay, myNavmesh, intersectionPoint))
			{
				std::cout << "Ray hit the navmesh at: " << intersectionPoint.x << ", " << intersectionPoint.y << ", " << intersectionPoint.z << std::endl;
			
				if (myPathFrom == DE::Vector3f{ 0.0f, 0.0f, 0.0f })
				{
					myPathFrom = intersectionPoint;
				} 
				else if (myPathTo == DE::Vector3f{ 0.0f, 0.0f, 0.0f })
				{
					myPathTo = intersectionPoint;
				}
			}
			else
			{
				std::cout << "Ray did not hit the navmesh." << std::endl;
			}
		}
		else if (myIgnoreYAndCollectIntersections)
		{
			ReturnCollisionPointFromNavmeshIgnoringY(cameraRay);
		}
	}
}


void DreamEngine::NavmeshCreationTool::BuildNavmesh() // SIMPLE MESH TEST
{
	if (myMeshes.size() == 0)
	{
		printf("No Meshes/Model Instances in vector\n");
	}

	// Update cfg 
	cfg.walkableHeight = (int)ceilf(myAgentHeight / cfg.ch);
	cfg.walkableClimb = (int)floorf(myAgentMaxClimb / cfg.ch);
	cfg.walkableRadius = (int)ceilf(myAgentRadius / cfg.cs);

	std::vector<float> vertices;
	std::vector<int> indices;

	float bmin[3] = { 0 }, bmax[3] = { 0 };


	bool isUsedForNavmesh = true;

	for (const auto& object : myMeshes) 
	{

		DreamEngine::Matrix4x4f scaleMatrix;
		const DreamEngine::Matrix4x4f& transform = object->GetTransform().GetMatrix() * scaleMatrix.CreateScaleMatrix({ 0.01f, 0.01f, 0.01f });

		unsigned int numVerts = 0;

		if (isUsedForNavmesh)
		{
			for (const DreamEngine::Model::MeshData& mesh : object->GetModel()->GetMeshDataList())
			{
				for (uint32_t i = 0; i < mesh.numberOfVertices; ++i)
				{
					const DreamEngine::Vector4f pos = mesh.vertices[i].Position * transform;

					int something;
					something;
					vertices.push_back(pos.x);
					vertices.push_back(pos.y);
					vertices.push_back(pos.z);

					// Increase Box Extent
					if (pos.x < bmin[0]) bmin[0] = pos.x;
					if (pos.y < bmin[1]) bmin[1] = pos.y;
					if (pos.z < bmin[2]) bmin[2] = pos.z;

					// Decrease Box Extent
					if (pos.x > bmax[0]) bmax[0] = pos.x;
					if (pos.y > bmax[1]) bmax[1] = pos.y;
					if (pos.z > bmax[2]) bmax[2] = pos.z;
				}

				// @todo: probably there is no need to copy these
				for (uint32_t i = 0; i < mesh.numberOfIndices; ++i)
				{
					indices.push_back(mesh.indices[i] + numVerts);
				}
			}
			numVerts = static_cast<unsigned int>(vertices.size());
		}
	}

	if (vertices.size() <= 0 || indices.size() <= 0)
	{
		return;
	}

	rcContext* ctx = new rcContext();
	rcHeightfield* solid = rcAllocHeightfield();

	int width = (int)((bmax[0] - bmin[0]) / cfg.cs);
	int height = (int)((bmax[2] - bmin[2]) / cfg.cs);

	rcVcopy(cfg.bmin, bmin);
	rcVcopy(cfg.bmax, bmax);

	if (!solid) 
	{
		printf("error from calling: rcHeightfield* solid = rcAllocHeightfield()");
	}

	if (!rcCreateHeightfield(ctx, *solid, width, height, cfg.bmin, cfg.bmax, cfg.cs, cfg.ch)) 
	{
		printf("error calling: rcCreateHeightfield(...)");
	}

	unsigned char* triareas = new unsigned char[indices.size() / 3];
	rcMarkWalkableTriangles(ctx, cfg.walkableSlopeAngle, &vertices[0], (int)vertices.size() / 3, &indices[0], (int)indices.size() / 3, triareas);

	rcRasterizeTriangles(
		ctx,									// The Recast context
		&vertices[0],							// Pointer to the first vertex in the vertices array
		static_cast<int>(vertices.size() / 3),	// Number of vertices (each vertex has 3 floats)
		&indices[0],							// Pointer to the first index in the indices array
		triareas,								// Pointer to the first element in the triangle areas array
		static_cast<int>(indices.size() / 3),	// Number of triangles (each triangle has 3 indices)
		*solid,									// The heightfield to rasterize into
		cfg.walkableClimb						// The walkable climb height
	);

	rcFilterLowHangingWalkableObstacles(ctx, cfg.walkableClimb, *solid);
	rcFilterLedgeSpans(ctx, cfg.walkableHeight, cfg.walkableClimb, *solid);
	rcFilterWalkableLowHeightSpans(ctx, cfg.walkableHeight, *solid);

	rcCompactHeightfield* chf = rcAllocCompactHeightfield();
	if (!chf) 
	{
		printf("Error calling: rcCompactHeightfield* chf = rcAllocCompactHeightfield()");
	}
	if (!rcBuildCompactHeightfield(ctx, cfg.walkableHeight, cfg.walkableClimb, *solid, *chf)) 
	{
		printf("Error calling: rcBuildCompactHeightfield");
	}

	if (!rcErodeWalkableArea(ctx, cfg.walkableRadius, *chf)) 
	{
		printf("Error calling: rcErodeWalkableArea");
	}

	if (!rcBuildDistanceField(ctx, *chf)) 
	{
		printf("Error calling: rcBuildDistanceField");
	}

	if (!rcBuildRegions(ctx, *chf, cfg.borderSize, cfg.minRegionArea, cfg.mergeRegionArea)) 
	{
		printf("Error calling: rcBuildRegions");
	}

	rcContourSet* cset = rcAllocContourSet();
	if (!cset) 
	{
		printf("Error calling: rcContourSet* cset = rcAllocContourSet()");
	}

	if (!rcBuildContours(ctx, *chf, cfg.maxSimplificationError, cfg.maxEdgeLen, *cset)) 
	{
		printf("Error calling: rcBuildContours");
	}

	rcPolyMesh* pmesh = rcAllocPolyMesh();
	if (!pmesh) 
	{
		printf("Error calling: rcPolyMesh* pmesh = rcAllocPolyMesh()");
	}

	if (!rcBuildPolyMesh(ctx, *cset, cfg.maxVertsPerPoly, *pmesh)) 
	{
		printf("Error calling: rcBuildPolyMesh");
	}

	rcPolyMeshDetail* dmesh = rcAllocPolyMeshDetail();
	if (!dmesh) 
	{
		printf("Error calling: rcPolyMeshDetail* dmesh = rcAllocPolyMeshDetail()");
	}

	if (!rcBuildPolyMeshDetail(ctx, *pmesh, *chf, cfg.detailSampleDist, cfg.detailSampleMaxError, *dmesh)) 
	{
		printf("Error calling: rcBuildPolyMeshDetail");
	}

	printf("Done preparing and generating navmesh, saving in obj-file.");

	std::string name = "Path to parent"; 

	//Saving rcPolyMesh struct//
	// Free any existing data in myPolyMesh to avoid memory leaks
	if (myPolyMesh.verts) {
		delete[] myPolyMesh.verts;
		myPolyMesh.verts = nullptr;
	}

	if (myPolyMesh.polys) {
		delete[] myPolyMesh.polys;
		myPolyMesh.polys = nullptr;
	}

	if (myPolyMesh.regs) {
		delete[] myPolyMesh.regs;
		myPolyMesh.regs = nullptr;
	}

	if (myPolyMesh.flags) {
		delete[] myPolyMesh.flags;
		myPolyMesh.flags = nullptr;
	}

	if (myPolyMesh.areas) {
		delete[] myPolyMesh.areas;
		myPolyMesh.areas = nullptr;
	}

	// Allocate and copy data into the member variable myPolyMesh
	myPolyMesh.nverts = pmesh->nverts;
	myPolyMesh.npolys = pmesh->npolys;
	myPolyMesh.maxpolys = pmesh->maxpolys;
	myPolyMesh.nvp = pmesh->nvp;
	myPolyMesh.cs = pmesh->cs;
	myPolyMesh.ch = pmesh->ch;

	// Copy the bmin and bmax arrays
	std::memcpy(myPolyMesh.bmin, pmesh->bmin, sizeof(float) * 3);
	std::memcpy(myPolyMesh.bmax, pmesh->bmax, sizeof(float) * 3);

	// Allocate memory for the vertices and polygons
	myPolyMesh.verts = new unsigned short[myPolyMesh.nverts * 3];  // 3 values per vertex (x, y, z)
	myPolyMesh.polys = new unsigned short[myPolyMesh.maxpolys * 2 * myPolyMesh.nvp];  // 2 values per vertex (verts + neighbors)

	// Copy the data from the Recast poly mesh (pmesh)
	std::memcpy(myPolyMesh.verts, pmesh->verts, sizeof(unsigned short) * myPolyMesh.nverts * 3);
	std::memcpy(myPolyMesh.polys, pmesh->polys, sizeof(unsigned short) * myPolyMesh.maxpolys * 2 * myPolyMesh.nvp);

	// Optionally copy other fields like region, flags, etc., if needed
	myPolyMesh.regs = new unsigned short[myPolyMesh.maxpolys];
	std::memcpy(myPolyMesh.regs, pmesh->regs, sizeof(unsigned short) * myPolyMesh.maxpolys);

	myPolyMesh.flags = new unsigned short[myPolyMesh.maxpolys];
	std::memcpy(myPolyMesh.flags, pmesh->flags, sizeof(unsigned short) * myPolyMesh.maxpolys);

	myPolyMesh.areas = new unsigned char[myPolyMesh.maxpolys];
	std::memcpy(myPolyMesh.areas, pmesh->areas, sizeof(unsigned char) * myPolyMesh.maxpolys);
	//Saving rcPolyMesh struct//


	//Saving rcPolyMeshDetail struct//
	// Free any existing data in myPolyMeshDetail to avoid memory leaks
	if (myPolyMeshDetail.verts) {
		delete[] myPolyMeshDetail.verts;
		myPolyMeshDetail.verts = nullptr;
	}

	if (myPolyMeshDetail.tris) {
		delete[] myPolyMeshDetail.tris;
		myPolyMeshDetail.tris = nullptr;
	}

	if (myPolyMeshDetail.meshes) {
		delete[] myPolyMeshDetail.meshes;
		myPolyMeshDetail.meshes = nullptr;
	}

	// Allocate and copy data into the member variable myPolyMeshDetail
	myPolyMeshDetail.nverts = dmesh->nverts;
	myPolyMeshDetail.ntris = dmesh->ntris;
	myPolyMeshDetail.nmeshes = dmesh->nmeshes;


	// Allocate memory for the arrays
	myPolyMeshDetail.verts = new float[myPolyMeshDetail.nverts * 3];            // Number of floats (already includes x, y, z)
	myPolyMeshDetail.tris = new unsigned char[myPolyMeshDetail.ntris * 4];		// Each triangle has 4 unsigned char values
	myPolyMeshDetail.meshes = new unsigned int[myPolyMeshDetail.nmeshes * 4];   // Each sub-mesh has 4 unsigned int values

	// Copy the data from dmesh into the member variable
	std::memcpy(myPolyMeshDetail.verts, dmesh->verts, sizeof(float) * myPolyMeshDetail.nverts * 3);
	std::memcpy(myPolyMeshDetail.tris, dmesh->tris, sizeof(unsigned char) * myPolyMeshDetail.ntris * 4);
	std::memcpy(myPolyMeshDetail.meshes, dmesh->meshes, sizeof(unsigned int) * myPolyMeshDetail.nmeshes * 4);
	//Saving rcPolyMeshDetail struct//

	// Save NAVMESH DATA Begin 
	HandleNavmeshStruct(false);
	HandleNavmeshStructFromDetailedPolyMesh();
	// Save NAVMESH DATA End 

	ExportNavMeshToOBJ(*pmesh, *dmesh, name);

	rcFreeHeightField(solid);
	rcFreeCompactHeightfield(chf);
	rcFreeContourSet(cset);
	rcFreePolyMesh(pmesh);
	rcFreePolyMeshDetail(dmesh);
	delete[] triareas;
	delete ctx;
}

void DreamEngine::NavmeshCreationTool::LoadPolyMeshDetailBIN(const std::string& filename)
{
	std::ifstream inFile(filename, std::ios::binary);
	if (!inFile)
	{
		throw std::runtime_error("Failed to open file for reading");
	}

	// Free any existing memory to avoid memory leaks
	if (myPolyMeshDetail.verts)
	{
		delete[] myPolyMeshDetail.verts;
		myPolyMeshDetail.verts = nullptr;
	}
	if (myPolyMeshDetail.tris)
	{
		delete[] myPolyMeshDetail.tris;
		myPolyMeshDetail.tris = nullptr;
	}
	if (myPolyMeshDetail.meshes)
	{
		delete[] myPolyMeshDetail.meshes;
		myPolyMeshDetail.meshes = nullptr;
	}

	// Read number of vertices, triangles, and meshes
	inFile.read(reinterpret_cast<char*>(&myPolyMeshDetail.nverts), sizeof(int));
	inFile.read(reinterpret_cast<char*>(&myPolyMeshDetail.ntris), sizeof(int));
	inFile.read(reinterpret_cast<char*>(&myPolyMeshDetail.nmeshes), sizeof(int));

	// Allocate memory for the arrays
	myPolyMeshDetail.verts = new float[myPolyMeshDetail.nverts * 3];
	myPolyMeshDetail.tris = new unsigned char[myPolyMeshDetail.ntris * 4];
	myPolyMeshDetail.meshes = new unsigned int[myPolyMeshDetail.nmeshes * 4];

	// Read the data arrays from the file
	inFile.read(reinterpret_cast<char*>(myPolyMeshDetail.verts), sizeof(float) * myPolyMeshDetail.nverts * 3);
	inFile.read(reinterpret_cast<char*>(myPolyMeshDetail.tris), sizeof(unsigned char) * myPolyMeshDetail.ntris * 4);
	inFile.read(reinterpret_cast<char*>(myPolyMeshDetail.meshes), sizeof(unsigned int) * myPolyMeshDetail.nmeshes * 4);

	inFile.close();

	// Todo: Have LD Fix their exporting
	for (int i = 0; i < myPolyMeshDetail.nverts; ++i)
	{
		myPolyMeshDetail.verts[i * 3 + 3] *= -1.0f;  
	}
}

void DreamEngine::NavmeshCreationTool::LoadPolyMeshBIN(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file for reading: " + filename);
	}

	// ==== Clear existing data before loading new data ====
	if (myPolyMesh.verts) delete[] myPolyMesh.verts;
	if (myPolyMesh.polys) delete[] myPolyMesh.polys;
	if (myPolyMesh.regs) delete[] myPolyMesh.regs;
	if (myPolyMesh.flags) delete[] myPolyMesh.flags;
	if (myPolyMesh.areas) delete[] myPolyMesh.areas;

	// Reset pointers to prevent accidental access to freed memory
	myPolyMesh.verts = nullptr;
	myPolyMesh.polys = nullptr;
	myPolyMesh.regs = nullptr;
	myPolyMesh.flags = nullptr;
	myPolyMesh.areas = nullptr;

	// Read basic values
	file.read(reinterpret_cast<char*>(&myPolyMesh.nverts), sizeof(myPolyMesh.nverts));
	file.read(reinterpret_cast<char*>(&myPolyMesh.npolys), sizeof(myPolyMesh.npolys));
	file.read(reinterpret_cast<char*>(&myPolyMesh.maxpolys), sizeof(myPolyMesh.maxpolys));
	file.read(reinterpret_cast<char*>(&myPolyMesh.nvp), sizeof(myPolyMesh.nvp));
	file.read(reinterpret_cast<char*>(myPolyMesh.bmin), sizeof(myPolyMesh.bmin));
	file.read(reinterpret_cast<char*>(myPolyMesh.bmax), sizeof(myPolyMesh.bmax));
	file.read(reinterpret_cast<char*>(&myPolyMesh.cs), sizeof(myPolyMesh.cs));
	file.read(reinterpret_cast<char*>(&myPolyMesh.ch), sizeof(myPolyMesh.ch));
	file.read(reinterpret_cast<char*>(&myPolyMesh.borderSize), sizeof(myPolyMesh.borderSize));
	file.read(reinterpret_cast<char*>(&myPolyMesh.maxEdgeError), sizeof(myPolyMesh.maxEdgeError));

	// Allocate memory for arrays
	myPolyMesh.verts = new unsigned short[myPolyMesh.nverts * 3];
	myPolyMesh.polys = new unsigned short[myPolyMesh.npolys * 2 * myPolyMesh.nvp];
	myPolyMesh.regs = new unsigned short[myPolyMesh.npolys];
	myPolyMesh.flags = new unsigned short[myPolyMesh.npolys];
	myPolyMesh.areas = new unsigned char[myPolyMesh.npolys];


	// Read array data
	file.read(reinterpret_cast<char*>(myPolyMesh.verts), sizeof(unsigned short) * myPolyMesh.nverts * 3);
	file.read(reinterpret_cast<char*>(myPolyMesh.polys), sizeof(unsigned short) * myPolyMesh.npolys * 2 * myPolyMesh.nvp);
	file.read(reinterpret_cast<char*>(myPolyMesh.regs), sizeof(unsigned short) * myPolyMesh.npolys);
	file.read(reinterpret_cast<char*>(myPolyMesh.flags), sizeof(unsigned short) * myPolyMesh.npolys);
	file.read(reinterpret_cast<char*>(myPolyMesh.areas), sizeof(unsigned char) * myPolyMesh.npolys);

	file.close();
}

void DreamEngine::NavmeshCreationTool::LoadNavmeshFromOBJ(std::string aPath)
{
	std::ifstream file(aPath);
	if (!file.is_open()) {
		std::cerr << "Failed to open OBJ file: " << aPath << std::endl;
		return;
	}

	myNavmesh.nodes.clear();
	myNavmesh.meshVertices.clear();
	myNavmesh.meshIndices.clear();

	std::vector<DreamEngine::Vector3f> vertices;
	std::vector<std::vector<int>> faces;  // Store vertex indices for each face

	DreamEngine::Matrix4x4f scaleMatrix;
	DreamEngine::Vector4f scalingVertex;

	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream stream(line);
		std::string prefix;
		stream >> prefix;

		if (prefix == "v")
		{  // Vertex line
			float x, y, z;
			stream >> x >> y >> z;

			// Apply scaling
			DreamEngine::Vector3f vertex(x, y, -z); // Negate Z axis to convert from recast to our engine
			scalingVertex = vertex;
			scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);  // Apply scaling factor
			vertex = scalingVertex;

			vertices.push_back(vertex);
		}
		else if (prefix == "f")
		{  // Face line
			std::vector<int> faceIndices;
			int index;
			while (stream >> index)
			{
				faceIndices.push_back(index - 1);  // Convert from 1-indexed to 0-indexed
			}
			if (faceIndices.size() == 3)
			{  // Ensure the face is a triangle
				faces.push_back(faceIndices);
			}
		}
	}
	file.close();

	int vertexOffset = 0;  // Offset for mesh vertices

	// Iterate over faces to populate navmesh
	for (const auto& face : faces) {
		DreamEngine::Vector3f v0 = vertices[face[0]];
		DreamEngine::Vector3f v1 = vertices[face[1]];
		DreamEngine::Vector3f v2 = vertices[face[2]];

		// Calculate center of the triangle
		DreamEngine::Vector3f center = (v0 + v1 + v2) / 3.0f;

		myNavmesh.meshVertices.push_back(v0);
		myNavmesh.meshVertices.push_back(v1);
		myNavmesh.meshVertices.push_back(v2);

		myNavmesh.meshIndices.push_back(vertexOffset);
		myNavmesh.meshIndices.push_back(vertexOffset + 1);
		myNavmesh.meshIndices.push_back(vertexOffset + 2);

		Node newNode;
		newNode.center = center;
		newNode.indices = { vertexOffset, vertexOffset + 1, vertexOffset + 2 };
		newNode.connections = { -1, -1, -1 };  // Default to no connections

		myNavmesh.nodes.push_back(newNode);

		vertexOffset += 3;
	}

	// Find connections between nodes (shared edges)
	for (size_t i = 0; i < myNavmesh.nodes.size(); ++i)
	{
		for (size_t j = i + 1; j < myNavmesh.nodes.size(); ++j)
		{
			int sharedVertices = 0;
			int edge1[2] = { -1, -1 };
			int edge2[2] = { -1, -1 };

			for (int a = 0; a < 3; ++a)
			{
				for (int b = 0; b < 3; ++b)
				{
					if (myNavmesh.nodes[i].indices[a] == myNavmesh.nodes[j].indices[b])
					{
						if (sharedVertices == 0)
						{
							edge1[0] = a;
							edge2[0] = b;
						}
						else if (sharedVertices == 1)
						{
							edge1[1] = a;
							edge2[1] = b;
						}
						sharedVertices++;
					}
				}
			}

			if (sharedVertices == 2)
			{
				myNavmesh.nodes[i].connections[edge1[0]] = static_cast<int>(j);
				myNavmesh.nodes[j].connections[edge2[0]] = static_cast<int>(i);
			}
		}
	}

	std::cout << "Navmesh loaded from OBJ: " << myNavmesh.meshVertices.size() / 3 << " triangles.\n";
}

void DreamEngine::NavmeshCreationTool::LoadDetailedNavmeshFromOBJ(std::string aPath)
{
	std::ifstream file(aPath);
	if (!file.is_open()) {
		std::cerr << "Failed to open OBJ file: " << aPath << std::endl;
		return;
	}

	myDetailedNavmesh.nodes.clear();
	myDetailedNavmesh.meshVertices.clear();
	myDetailedNavmesh.meshIndices.clear();

	std::vector<DreamEngine::Vector3f> vertices;  // Temporary storage for vertices
	std::vector<std::vector<int>> faces;  // Store triangle vertex indices for each mesh

	DreamEngine::Matrix4x4f scaleMatrix;
	DreamEngine::Vector4f scalingVertex;

	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream stream(line);
		std::string prefix;
		stream >> prefix;

		if (prefix == "v") 
		{  // Vertex line
			float x, y, z;
			stream >> x >> y >> z;

			// Apply scaling to the vertex
			DreamEngine::Vector3f vertex(x, y, z); 
			scalingVertex = vertex;
			scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);  // Apply scaling factor (Needed for our Engine)
			vertex = scalingVertex;

			vertices.emplace_back(vertex);
		}
		else if (prefix == "f")
		{  // Face line
			std::vector<int> faceIndices;
			int index;
			while (stream >> index)
			{
				faceIndices.push_back(index - 1);  // Convert 1-indexed to 0-indexed
			}
			if (faceIndices.size() == 3) 
			{  // Ensure the face is a triangle
				faces.push_back(faceIndices);
			}
		}
	}
	file.close();

	int vertexOffset = 0;
	std::vector<Node> tempNodes;

	// Populate the detailed navmesh struct
	for (const auto& face : faces)
	{
		DreamEngine::Vector3f v0 = vertices[face[0]];
		DreamEngine::Vector3f v1 = vertices[face[1]];
		DreamEngine::Vector3f v2 = vertices[face[2]];

		// Calculate the center of the triangle
		DreamEngine::Vector3f center = (v0 + v1 + v2) / 3.0f;

		myDetailedNavmesh.meshVertices.push_back(v0);
		myDetailedNavmesh.meshVertices.push_back(v1);
		myDetailedNavmesh.meshVertices.push_back(v2);

		myDetailedNavmesh.meshIndices.push_back(vertexOffset);
		myDetailedNavmesh.meshIndices.push_back(vertexOffset + 1);
		myDetailedNavmesh.meshIndices.push_back(vertexOffset + 2);

		Node newNode;
		newNode.center = center;
		newNode.indices = { vertexOffset, vertexOffset + 1, vertexOffset + 2 };
		newNode.connections = { -1, -1, -1 };

		tempNodes.push_back(newNode);
		vertexOffset += 3;
	}

	// Reconstruct connections between triangles by checking for shared edges
	for (size_t i = 0; i < tempNodes.size(); ++i)
	{
		for (size_t j = i + 1; j < tempNodes.size(); ++j)
		{
			int sharedVertices = 0;
			int edge1[2] = { -1, -1 };
			int edge2[2] = { -1, -1 };

			for (int a = 0; a < 3; ++a)
			{
				for (int b = 0; b < 3; ++b)
				{
					if (tempNodes[i].indices[a] == tempNodes[j].indices[b])
					{
						if (sharedVertices == 0)
						{
							edge1[0] = a;
							edge2[0] = b;
						}
						else if (sharedVertices == 1)
						{
							edge1[1] = a;
							edge2[1] = b;
						}
						sharedVertices++;
					}
				}
			}

			if (sharedVertices == 2) {  // If 2 vertices are shared, it's a connection
				tempNodes[i].connections[edge1[0]] = static_cast<int>(j);
				tempNodes[j].connections[edge2[0]] = static_cast<int>(i);
			}
		}
	}

	// Add nodes to the detailed navmesh
	myDetailedNavmesh.nodes.insert(myDetailedNavmesh.nodes.end(), tempNodes.begin(), tempNodes.end());
	std::cout << "Loaded detailed navmesh from OBJ: " << myDetailedNavmesh.meshVertices.size() / 3 << " triangles.\n";
}

void DreamEngine::NavmeshCreationTool::HandleNavmeshStruct(bool shouldInvertZ)
{
	DreamEngine::Matrix4x4f scaleMatrix;
	DreamEngine::Vector4f scalingVertex;

	myNavmesh.nodes.clear();        
	myNavmesh.meshVertices.clear(); 
	myNavmesh.meshIndices.clear();  

	if (!myPolyMesh.verts || !myPolyMesh.polys)
	{
		return;
	}

	

	float xOffset = myDrawXOffset;  // X offset for imGui changes
	float yOffset = myDrawYOffset;  // Y offset for imGui changes
	float zOffset = myDrawZOffset;  // Z offset for imGui changes

	int vertexOffset = 0;  // Track the current index for meshVertices

	// Iterate over each polygon (triangle) in the navmesh
	for (int i = 0; i < myPolyMesh.npolys; ++i)
	{
		// Calculate the center (centroid) of the current triangle
		const unsigned short* p = &myPolyMesh.polys[i * 2 * myPolyMesh.nvp];

		// Create vertices with offset
		DreamEngine::Vector3f v0(myPolyMesh.verts[p[0] * 3] * myPolyMesh.cs + myPolyMesh.bmin[0], myPolyMesh.verts[p[0] * 3 + 1] * myPolyMesh.ch + myPolyMesh.bmin[1], myPolyMesh.verts[p[0] * 3 + 2] * myPolyMesh.cs + myPolyMesh.bmin[2]);
		DreamEngine::Vector3f v1(myPolyMesh.verts[p[1] * 3] * myPolyMesh.cs + myPolyMesh.bmin[0], myPolyMesh.verts[p[1] * 3 + 1] * myPolyMesh.ch + myPolyMesh.bmin[1], myPolyMesh.verts[p[1] * 3 + 2] * myPolyMesh.cs + myPolyMesh.bmin[2]);
		DreamEngine::Vector3f v2(myPolyMesh.verts[p[2] * 3] * myPolyMesh.cs + myPolyMesh.bmin[0], myPolyMesh.verts[p[2] * 3 + 1] * myPolyMesh.ch + myPolyMesh.bmin[1], myPolyMesh.verts[p[2] * 3 + 2] * myPolyMesh.cs + myPolyMesh.bmin[2]);

		
		// Apply offset to vertices
		v0.x += xOffset; v0.y += yOffset; v0.z += zOffset;
		v1.x += xOffset; v1.y += yOffset; v1.z += zOffset;
		v2.x += xOffset; v2.y += yOffset; v2.z += zOffset;

		scalingVertex = v0;
		scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
		v0 = scalingVertex;

		scalingVertex = v1;
		scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
		v1 = scalingVertex;

		scalingVertex = v2;
		scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
		v2 = scalingVertex;


		if (shouldInvertZ)
		{
			//// Todo: DEPENDS ON MESH FROM LD
			v0.z *= -1.f;
			v1.z *= -1.f;
			v2.z *= -1.f;
		}

		// Calculate the center of the triangle
		DreamEngine::Vector3f center = (v0 + v1 + v2) / 3.0f;

		// Add vertices to meshVertices and store their indices in meshIndices
		myNavmesh.meshVertices.push_back(v0);
		myNavmesh.meshVertices.push_back(v1);
		myNavmesh.meshVertices.push_back(v2);

		// Add the indices of the vertices (relative to their current position in meshVertices)
		myNavmesh.meshIndices.push_back(vertexOffset);
		myNavmesh.meshIndices.push_back(vertexOffset + 1);
		myNavmesh.meshIndices.push_back(vertexOffset + 2);

		// Update vertexOffset to account for the three new vertices
		vertexOffset += 3;

		// Create a new node for the current triangle
		Node newNode;
		newNode.center = center /*- myMeshOffsetPos*/;
		newNode.indices = { vertexOffset - 3, vertexOffset - 2, vertexOffset - 1 };

		// Initialize connections to -1 (no connection)
		newNode.connections = { -1, -1, -1 };

		// Iterate over the polygon's neighbors (stored after the vertices in the polys array)
		for (int j = 0; j < myPolyMesh.nvp; ++j)  // number of vertices per polygon
		{
			unsigned short neighbor = myPolyMesh.polys[i * 2 * myPolyMesh.nvp + myPolyMesh.nvp + j];  // Neighbor polygon index

			if (neighbor == RC_MESH_NULL_IDX && neighbor >= myPolyMesh.npolys)
			{
				NavmeshEdge newEdge;
				newEdge.start = j == 0 ? v0 : (j == 1 ? v1 : v2);
				newEdge.end = j == 0 ? v1 : (j == 1 ? v2 : v0);
				newEdge.normal = (newEdge.end - newEdge.start).Cross(DreamEngine::Vector3f(0.f, 1.f, 0.f)).GetNormalized(); // Approximate normal

				// Compute the edge center
				DreamEngine::Vector3f edgeCenter = (newEdge.start + newEdge.end) / 2.0f;
				newEdge.normal = (center - edgeCenter).GetNormalized();

				myNavmesh.edges.push_back(newEdge);
			}
			else // Has Neighbor
			{
				newNode.connections[j] = neighbor;
			}
		}

		// Add the node to the navmesh
		myNavmesh.nodes.push_back(newNode);
	}
	PopulatePolyNavmeshGrid();
}


void DreamEngine::NavmeshCreationTool::HandleNavmeshStructFromDetailedPolyMesh()
{
	myDetailedNavmesh.nodes.clear();
	myDetailedNavmesh.meshVertices.clear();
	myDetailedNavmesh.meshIndices.clear();

	DreamEngine::Matrix4x4f scaleMatrix;
	DreamEngine::Vector4f scalingVertex;

	if (!myPolyMeshDetail.verts) // Check that the detailed mesh is valid
		return;

	int vertexOffset = 0;  // Tracks current index in meshVertices

	// Iterate over each detailed mesh
	for (int i = 0; i < myPolyMeshDetail.nmeshes; ++i)
	{
		const unsigned int* mesh = &myPolyMeshDetail.meshes[i * 4];

		const int baseVertIndex = mesh[0];
		const int vertCount = mesh[1];
		const int baseTriIndex = mesh[2];
		const int triCount = mesh[3];

		// Add vertices for this detailed mesh
		for (int j = 0; j < vertCount; ++j)
		{
			const float* vert = &myPolyMeshDetail.verts[(baseVertIndex + j) * 3];
			DreamEngine::Vector3f vertex(vert[0], vert[1], vert[2]);

			scalingVertex = vertex;
			scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
			vertex = scalingVertex;

			myDetailedNavmesh.meshVertices.push_back(vertex);
		}

		// Add triangles for this detailed mesh and temporarily store them for connection analysis
		std::vector<Node> tempNodes;

		for (int j = 0; j < triCount; ++j)
		{
			const unsigned char* tri = &myPolyMeshDetail.tris[(baseTriIndex + j) * 4];

			Node newNode;
			DreamEngine::Vector3f center(0.0f, 0.0f, 0.0f);

			// Process each vertex of the triangle
			for (int k = 0; k < 3; ++k)
			{
				int vertIndex = baseVertIndex + tri[k];
				myDetailedNavmesh.meshIndices.push_back(vertexOffset + tri[k]);

				center += myDetailedNavmesh.meshVertices[vertIndex];
			}

			center /= 3.0f;  // Average to find the center
			newNode.center = center;

			// Store indices for each triangle's vertices
			newNode.indices = 
			{
				vertexOffset + tri[0],
				vertexOffset + tri[1],
				vertexOffset + tri[2]
			};

			// Initialize connections to no connection
			newNode.connections = { -1, -1, -1 };

			tempNodes.push_back(newNode);
		}

		// Find connections by checking shared edges
		for (int j = 0; j < triCount; ++j)
		{
			for (int k = j + 1; k < triCount; ++k)
			{
				int sharedEdgeCount = 0;
				int edge1[2] = { -1, -1 };
				int edge2[2] = { -1, -1 };

				// Compare vertices of two triangles to find shared edges
				for (int a = 0; a < 3; ++a)
				{
					for (int b = 0; b < 3; ++b)
					{
						if (tempNodes[j].indices[a] == tempNodes[k].indices[b])
						{
							if (sharedEdgeCount == 0)
							{
								edge1[0] = a;
								edge2[0] = b;
							}
							else if (sharedEdgeCount == 1)
							{
								edge1[1] = a;
								edge2[1] = b;
							}
							sharedEdgeCount++;
						}
					}
				}

				// If two vertices are shared, it?s a connection
				if (sharedEdgeCount == 2)
				{
					tempNodes[j].connections[edge1[0]] = k;
					tempNodes[k].connections[edge2[0]] = j;
				}
			}
		}

		// Push the temporary nodes with updated connections to the main navmesh nodes
		myDetailedNavmesh.nodes.insert(myDetailedNavmesh.nodes.end(), tempNodes.begin(), tempNodes.end());
		vertexOffset += vertCount; // Move vertex offset for the next mesh
	}
	PopulateDetailedNavmeshGrid();
}

void DreamEngine::NavmeshCreationTool::PopulateDetailedNavmeshGrid()
{
	NavmeshGrid navmeshGrid;
	myDetailedNavmesh.grid.cells.clear();	// Clear navmesh grid cells

	navmeshGrid.cellHeight = 1000.0f;
	navmeshGrid.cellWidth = 1000.0f;

	// Calculate the overall 2D bounds for the navmesh based on its vertices:
	bool firstVertex = true;
	DE::Vector2f minBounds, maxBounds;
	for (const auto& vertex : myDetailedNavmesh.meshVertices)
	{
		DE::Vector2f point(vertex.x, vertex.z); // 2D grid so convert from 3D vertice

		if (firstVertex)
		{
			minBounds = point;
			maxBounds = point;
			firstVertex = false;
		}
		else
		{
			if (point.x < minBounds.x)
			{
				minBounds.x = point.x;
			}

			if (point.y < minBounds.y)
			{
				minBounds.y = point.y;
			}

			if (point.x > maxBounds.x)
			{
				maxBounds.x = point.x;
			}

			if (point.y > maxBounds.y)
			{
				maxBounds.y = point.y;
			}
		}
	}

	navmeshGrid.minBound = minBounds;
	navmeshGrid.maxBound = maxBounds;

	// Calculate grid dimensions based on cell width and height:
	float width = maxBounds.x - minBounds.x;
	float height = maxBounds.y - minBounds.y;
	navmeshGrid.cols = static_cast<int>(std::ceil(width / navmeshGrid.cellWidth));
	navmeshGrid.rows = static_cast<int>(std::ceil(height / navmeshGrid.cellHeight));

	navmeshGrid.cells.resize(navmeshGrid.rows * navmeshGrid.cols);

	// Populate the grid with triangle (or node) indices:
	for (size_t i = 0; i < myDetailedNavmesh.nodes.size(); ++i)
	{
		const Node& node = myDetailedNavmesh.nodes[i];

		// Get the 2D vertices of the triangle (projected on xz)
		DE::Vector2f v0(myDetailedNavmesh.meshVertices[node.indices[0]].x, myDetailedNavmesh.meshVertices[node.indices[0]].z);
		DE::Vector2f v1(myDetailedNavmesh.meshVertices[node.indices[1]].x, myDetailedNavmesh.meshVertices[node.indices[1]].z);
		DE::Vector2f v2(myDetailedNavmesh.meshVertices[node.indices[2]].x, myDetailedNavmesh.meshVertices[node.indices[2]].z);

		std::unordered_set<int> triangleCells;

		// Get grid cells for each edge of the triangle.
		std::vector<int> cellsEdge0 = GetGridCellsForLine(v0, v1, navmeshGrid);
		std::vector<int> cellsEdge1 = GetGridCellsForLine(v1, v2, navmeshGrid);
		std::vector<int> cellsEdge2 = GetGridCellsForLine(v2, v0, navmeshGrid);

		for (int cell : cellsEdge0)
			triangleCells.insert(cell);
		for (int cell : cellsEdge1)
			triangleCells.insert(cell);
		for (int cell : cellsEdge2)
			triangleCells.insert(cell);

		// Assign the node to each grid cell that its edges pass through.
		for (int cell : triangleCells)
		{
			navmeshGrid.cells[cell].push_back(static_cast<int>(i));
		}
	}

	myDetailedNavmesh.grid = navmeshGrid;
}

void DreamEngine::NavmeshCreationTool::PopulatePolyNavmeshGrid()
{
	NavmeshGrid navmeshGrid;
	myNavmesh.grid.cells.clear();	  // Clear navmesh grid cells
	
	navmeshGrid.cellHeight = 500.0f;
	navmeshGrid.cellWidth = 500.0f;

	// Calculate the overall 2D bounds
	bool firstVertex = true;
	DE::Vector2f minBounds, maxBounds;
	for (const auto& vertex : myNavmesh.meshVertices)
	{
		// Project the 3D vertex to a 2D point
		DE::Vector2f point(vertex.x, vertex.z);

		if (firstVertex)
		{
			minBounds = point;
			maxBounds = point;
			firstVertex = false;
		}
		else
		{
			minBounds.x = std::min(minBounds.x, point.x);
			minBounds.y = std::min(minBounds.y, point.y);
			maxBounds.x = std::max(maxBounds.x, point.x);
			maxBounds.y = std::max(maxBounds.y, point.y);
		}
	}
	navmeshGrid.minBound = minBounds;
	navmeshGrid.maxBound = maxBounds;

	// Calculate grid dimensions based on cell width and height:
	float width = maxBounds.x - minBounds.x;
	float height = maxBounds.y - minBounds.y;
	navmeshGrid.cols = static_cast<int>(std::ceil(width / navmeshGrid.cellWidth));
	navmeshGrid.rows = static_cast<int>(std::ceil(height / navmeshGrid.cellHeight));

	navmeshGrid.cells.resize(navmeshGrid.rows * navmeshGrid.cols);
	navmeshGrid.edgeCells.resize(navmeshGrid.rows * navmeshGrid.cols);

	// Populate the grid with triangle (or node) indices:
	for (size_t i = 0; i < myNavmesh.nodes.size(); ++i)
	{
		const Node& node = myNavmesh.nodes[i];

		// Get the 2D vertices of the triangle (projected on xz)
		DE::Vector2f v0(myNavmesh.meshVertices[node.indices[0]].x, myNavmesh.meshVertices[node.indices[0]].z);
		DE::Vector2f v1(myNavmesh.meshVertices[node.indices[1]].x, myNavmesh.meshVertices[node.indices[1]].z);
		DE::Vector2f v2(myNavmesh.meshVertices[node.indices[2]].x, myNavmesh.meshVertices[node.indices[2]].z);

		std::unordered_set<int> triangleCells;

		// Get grid cells for each edge of the triangle.
		std::vector<int> cellsEdge0 = GetGridCellsForLine(v0, v1, navmeshGrid);
		std::vector<int> cellsEdge1 = GetGridCellsForLine(v1, v2, navmeshGrid);
		std::vector<int> cellsEdge2 = GetGridCellsForLine(v2, v0, navmeshGrid);

		for (int cell : cellsEdge0)
			triangleCells.insert(cell);
		for (int cell : cellsEdge1)
			triangleCells.insert(cell);
		for (int cell : cellsEdge2)
			triangleCells.insert(cell);

		// Assign the triangle (node) to each grid cell that its edges pass through.
		for (int cell : triangleCells)
		{
			navmeshGrid.cells[cell].push_back(static_cast<int>(i));
		}
	}

	// Populate grid with edges
	for (size_t i = 0; i < myNavmesh.edges.size(); ++i)
	{
		const NavmeshEdge& edge = myNavmesh.edges[i];

		DE::Vector2f edgeMin(std::min(edge.start.x, edge.end.x), std::min(edge.start.z, edge.end.z));
		DE::Vector2f edgeMax(std::max(edge.start.x, edge.end.x), std::max(edge.start.z, edge.end.z));

		int startCol = std::max(0, std::min(static_cast<int>((edgeMin.x - minBounds.x) / navmeshGrid.cellWidth), navmeshGrid.cols - 1));
		int startRow = std::max(0, std::min(static_cast<int>((edgeMin.y - minBounds.y) / navmeshGrid.cellHeight), navmeshGrid.rows - 1));
		int endCol = std::max(0, std::min(static_cast<int>((edgeMax.x - minBounds.x) / navmeshGrid.cellWidth), navmeshGrid.cols - 1));
		int endRow = std::max(0, std::min(static_cast<int>((edgeMax.y - minBounds.y) / navmeshGrid.cellHeight), navmeshGrid.rows - 1));

		for (int row = startRow; row <= endRow; ++row)
		{
			for (int col = startCol; col <= endCol; ++col)
			{
				int cellIndex = row * navmeshGrid.cols + col;
				navmeshGrid.edgeCells[cellIndex].push_back(static_cast<int>(i));
			}
		}
	}

	myNavmesh.grid = navmeshGrid;
}

std::vector<int> DreamEngine::NavmeshCreationTool::GetGridCellsForLine(const DE::Vector2f& start, const DE::Vector2f& end, const NavmeshGrid& grid)
{
	std::vector<int> cells;

	// Calculate grid coordinates for the start and end points. Todo: Might give errors due to float from int
	int startCol = static_cast<int>((start.x - grid.minBound.x) / grid.cellWidth);
	int startRow = static_cast<int>((start.y - grid.minBound.y) / grid.cellHeight);
	int endCol = static_cast<int>((end.x - grid.minBound.x) / grid.cellWidth);
	int endRow = static_cast<int>((end.y - grid.minBound.y) / grid.cellHeight);

	// Clamp indices to grid boundaries
	startCol = std::max(0, std::min(startCol, grid.cols - 1));
	startRow = std::max(0, std::min(startRow, grid.rows - 1));
	endCol = std::max(0, std::min(endCol, grid.cols - 1));
	endRow = std::max(0, std::min(endRow, grid.rows - 1));

	int currentCol = startCol;
	int currentRow = startRow;
	cells.push_back(currentRow * grid.cols + currentCol);

	float dx = end.x - start.x;
	float dy = end.y - start.y;

	// Determine step directions
	int stepX = (dx > 0) ? 1 : (dx < 0 ? -1 : 0);
	int stepY = (dy > 0) ? 1 : (dy < 0 ? -1 : 0);

	// Avoid division by zero; if step is 0, use infinity.
	float tDeltaX = (stepX != 0) ? (grid.cellWidth / std::abs(dx)) : std::numeric_limits<float>::infinity();
	float tDeltaY = (stepY != 0) ? (grid.cellHeight / std::abs(dy)) : std::numeric_limits<float>::infinity();

	// Compute the t value at which we cross the first vertical/horizontal boundary.
	float nextBoundaryX = grid.minBound.x + (currentCol + (stepX > 0 ? 1 : 0)) * grid.cellWidth;
	float nextBoundaryY = grid.minBound.y + (currentRow + (stepY > 0 ? 1 : 0)) * grid.cellHeight;
	float tMaxX = (stepX != 0) ? ((nextBoundaryX - start.x) / dx) : std::numeric_limits<float>::infinity();
	float tMaxY = (stepY != 0) ? ((nextBoundaryY - start.y) / dy) : std::numeric_limits<float>::infinity();

	// Traverse the grid until we reach the end cell.
	while (currentCol != endCol || currentRow != endRow)
	{
		if (tMaxX < tMaxY)
		{
			currentCol += stepX;
			tMaxX += tDeltaX;
		}
		else
		{
			currentRow += stepY;
			tMaxY += tDeltaY;
		}

		// If we're outside the grid, break.
		if (currentCol < 0 || currentCol >= grid.cols || currentRow < 0 || currentRow >= grid.rows)
			break;
		int cellIndex = currentRow * grid.cols + currentCol;
		if (std::find(cells.begin(), cells.end(), cellIndex) == cells.end())
			cells.push_back(cellIndex);
	}

	return cells;
}

void DreamEngine::NavmeshCreationTool::HandleNavmeshStructFromDetourNavmesh()
{
	myNavmesh.nodes.clear();          // Clear any existing nodes in the navmesh
	myNavmesh.meshVertices.clear();   // Clear previous mesh vertices
	myNavmesh.meshIndices.clear();    // Clear previous mesh indices

	DreamEngine::Matrix4x4f scaleMatrix;
	DreamEngine::Vector4f scalingVertex;

	if (!myDetourNavMesh) // Ensure the dtNavMesh is valid
	{
		return;
	}

	float xOffset = myDrawXOffset;  // X offset from ImGui
	float yOffset = myDrawYOffset;  // Y offset from ImGui
	float zOffset = myDrawZOffset;  // Z offset from ImGui

	int vertexOffset = 0;  // Track the current index for meshVertices

	// Iterate over each tile
	for (int i = 0; i < myDetourNavMesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = myDetourNavMesh->getTile(i);
		if (!tile || !tile->header) continue; // Skip empty

		// Iterate over each polygon in the tile
		for (int j = 0; j < tile->header->polyCount; ++j)
		{
			const dtPoly* poly = &tile->polys[j];
			Node newNode;

			// Calculate the center of the polygon
			DreamEngine::Vector3f center = { 0.0f, 0.0f, 0.0f };
			int vertCount = 0;

			// Extract vertices for the polygon
			for (int k = 0; k < poly->vertCount; ++k)
			{
				const unsigned short vertIndex = poly->verts[k];
				const float* vert = &tile->verts[vertIndex * 3];

				DreamEngine::Vector3f vertex(
					vert[0] + xOffset,
					vert[1] + yOffset,
					vert[2] + zOffset
				);

				scalingVertex = vertex;
				scalingVertex = scalingVertex * scaleMatrix.CreateScaleMatrix(100.f);
				vertex = scalingVertex;

				// Add the vertex to meshVertices and accumulate for center calculation
				myNavmesh.meshVertices.push_back(vertex);
				center += vertex;
				vertCount++;

				myNavmesh.meshIndices.push_back(vertexOffset++);
			}

			// Average the center position for the polygon
			if (vertCount > 0) center /= static_cast<float>(vertCount);
			newNode.center = center;

			newNode.indices = { vertexOffset - 3, vertexOffset - 2, vertexOffset - 1 };

			// Initialize connections to -1
			newNode.connections = { -1, -1, -1 };

			// Handle connections (neighboring polygons)
			for (int k = 0; k < poly->vertCount; ++k)
			{
				if (k >= 3) break; // Limit to 3 connections for a triangle

				const unsigned short nei = poly->neis[k];
				if (nei != 0 && (nei & DT_EXT_LINK) == 0)
				{
					int neighborIdx = nei - 1; // Neighbor polygon index in this tile
					if (neighborIdx >= 0 && neighborIdx < tile->header->polyCount)
					{
						newNode.connections[k] = neighborIdx;
					}
				}
			}

			myNavmesh.nodes.push_back(newNode);
		}
	}
}

Navmesh& DreamEngine::NavmeshCreationTool::GetPreviousBuiltNavmesh()
{
	if (myNavmesh.nodes.size() == 0)
	{
		printf("Navmesh Struct is empty. Either not built or error from building");
	}

	return myNavmesh;
}

Navmesh& DreamEngine::NavmeshCreationTool::GetPreviousBuiltDetailedNavmesh()
{
	if (myDetailedNavmesh.nodes.size() == 0)
	{
		printf("Detailed Navmesh Struct is empty. Either not built or error from building");
	}

	return myDetailedNavmesh;
}

bool IntersectRayWithEdge(const DE::Ray<float>& ray, const DE::Vector3f& edgeStart, const DE::Vector3f& edgeEnd, DE::Vector3f& intersectionPoint)
{
	DE::Vector2f rayOrigin2D(ray.GetOriginPoint().x, ray.GetOriginPoint().z);
	DE::Vector2f rayDirection2D(ray.GetDirection().x, ray.GetDirection().z);

	DE::Vector2f edgeStart2D(edgeStart.x, edgeStart.z);
	DE::Vector2f edgeEnd2D(edgeEnd.x, edgeEnd.z);
	DE::Vector2f edgeVector = edgeEnd2D - edgeStart2D;

	float determinant = rayDirection2D.Cross(edgeVector);
	if (abs(determinant) < 0.0001f) // Parallel case
		return false;

	float t = (edgeStart2D - rayOrigin2D).Cross(edgeVector) / determinant;
	float u = (edgeStart2D - rayOrigin2D).Cross(rayDirection2D) / determinant;

	if (t >= 0 && u >= 0 && u <= 1)
	{
		// Intersection point in 3D
		intersectionPoint = ray.GetOriginPoint() + ray.GetDirection() * t;

		// Calculate interpolated Y value based on edge positions
		intersectionPoint.y = edgeStart.y + u * (edgeEnd.y - edgeStart.y);
		return true;
	}
	return false;
}

bool DreamEngine::NavmeshCreationTool::CastRayAgainstNavmeshIgnoringY(DE::Ray<float>& aRay, Navmesh aNavmesh, std::vector<DE::Vector3f>& aOutIntersectionPoints)
{
	bool intersectionFound = false;

	for (const auto& node : aNavmesh.nodes)
	{
		// Iterate over each triangle in the node.
		for (size_t i = 0; i < node.indices.size(); i += 3)
		{
			// Get triangle vertices
			DE::Vector3f v0 = aNavmesh.meshVertices[node.indices[i]];
			DE::Vector3f v1 = aNavmesh.meshVertices[node.indices[i + 1]];
			DE::Vector3f v2 = aNavmesh.meshVertices[node.indices[i + 2]];

			// Test ray-triangle intersection for each triangle edge
			for (int edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
			{
				DE::Vector3f edgeStart = (edgeIndex == 0) ? v0 : ((edgeIndex == 1) ? v1 : v2);
				DE::Vector3f edgeEnd = (edgeIndex == 0) ? v1 : ((edgeIndex == 1) ? v2 : v0);

				DE::Vector3f intersectionPoint;
				if (IntersectRayWithEdge(aRay, edgeStart, edgeEnd, intersectionPoint))
				{
					aOutIntersectionPoints.push_back(intersectionPoint);
					intersectionFound = true;
				}
			}
		}
	}

	return intersectionFound;
}

bool DreamEngine::NavmeshCreationTool::CastRayAgainstNavmesh(DE::Ray<float>& aRay, Navmesh aNavmesh, DE::Vector3f& aOutIntersectionPoint)
{
	float distToClosestHit = FLT_MAX;
	bool foundIntersection = false;

	for (auto& node : aNavmesh.nodes)
	{
		float outDist = FLT_MAX;
		DE::Vector3f outPoint;

		// Perform 3D ray-triangle intersection
		if (DE::IntersectionTriangleRay(
			aNavmesh.meshVertices[node.indices[0]],
			aNavmesh.meshVertices[node.indices[1]],
			aNavmesh.meshVertices[node.indices[2]],
			aRay, outPoint, outDist) && outDist < distToClosestHit)
		{
			// Update closest intersection point
			aOutIntersectionPoint = outPoint;
			distToClosestHit = outDist;
			foundIntersection = true;
		}
	}

	return foundIntersection;
}

DE::Ray<float> DreamEngine::NavmeshCreationTool::GetRayFromMouseCLickPerspective()
{
	DreamEngine::Engine& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::Vector2ui intResolution = engine.GetWindowSize();
	DreamEngine::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };

	DreamEngine::InputManager& inputManager = MainSingleton::GetInstance()->GetInputManager();
	myMousePosition.x = static_cast<float>(inputManager.GetMousePosition().x);
	myMousePosition.y = static_cast<float>(inputManager.GetMousePosition().y);

	DreamEngine::Vector4f ndc = {
		(myMousePosition.x - resolution.x / 2.f) / (resolution.x / 2.f),
		((resolution.y - myMousePosition.y) - resolution.y / 2.f) / (resolution.y / 2.f),
		1.f, // Z
		1.f  // W 
	};
	
	auto camera = MainSingleton::GetInstance()->GetActiveCamera();

	DreamEngine::Matrix4x4f invProjectionMatrix = camera->GetProjection().GetInverse();
	DreamEngine::Vector4f rayViewSpace = ndc * invProjectionMatrix;

	if (rayViewSpace.w != 0.0f)
	{
		rayViewSpace.x /= rayViewSpace.w;
		rayViewSpace.y /= rayViewSpace.w;
		rayViewSpace.z /= rayViewSpace.w;
	}

	DreamEngine::Matrix4x4f invViewMatrix = camera->GetTransform().GetMatrix();
	DreamEngine::Vector4f rayWorldSpace4 = rayViewSpace * invViewMatrix;
	DreamEngine::Vector3f rayWorldSpace = rayWorldSpace4;

	DreamEngine::Vector3f from = camera->GetTransform().GetPosition();
	DreamEngine::Vector3f directionRay = rayWorldSpace - from;
	directionRay.Normalize();

	float nearPlane;
	float farPlane;
	camera->GetProjectionPlanes(nearPlane, farPlane);

	from += camera->GetTransform().GetMatrix().GetForward() * nearPlane * 1.1f;

	myLineToDraw.fromPosition = from;
	myLineToDraw.toPosition = from + directionRay * farPlane;


	DE::Ray<float> returnedRay(myLineToDraw.fromPosition, myLineToDraw.toPosition);
	//returnedRay.InitWithOriginAndDirection(from, from + directionRay * farPlane);

	return returnedRay;
}

DreamEngine::Vector3f DreamEngine::NavmeshCreationTool::ReturnCollisionPointFromNavmesh(DE::Ray<float> directionRay)
{
	DE::Vector3f outIntersectionPoint;

	if (CastRayAgainstNavmesh(directionRay, myDetailedNavmesh, outIntersectionPoint))
	{
		for (const auto& point : mySavedIntersections)
		{
			std::cout << "Intersection at: " << point.x << ", " << point.y << ", " << point.z << "\n";
		}
	}
	else
	{
		std::cout << "No intersections found.\n";
	}

	return outIntersectionPoint;
}

DreamEngine::Vector3f DreamEngine::NavmeshCreationTool::ReturnCollisionPointFromNavmeshIgnoringY(DE::Ray<float> directionRay)
{
	mySavedIntersections.clear();

	if (CastRayAgainstNavmeshIgnoringY(directionRay, myDetailedNavmesh, mySavedIntersections))
	{
		for (const auto& point : mySavedIntersections)
		{
			std::cout << "Intersection at: " << point.x << ", " << point.y << ", " << point.z << "\n";
		}
	}
	else
	{
		std::cout << "No intersections found.\n";
	}

	return DreamEngine::Vector3f();
}
