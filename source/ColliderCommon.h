#pragma once
#include <PhysX\PxPhysicsAPI.h>
#include <vector>

enum class eColliderType
{
	Box,
	Sphere,
	ConvexMesh,
	Mesh // Concave is the assumed default
};

enum class eCollisionLayer
{
	Movement,
	PlayerAttacks,
	EnemyAttacks,
	TriggerBox,
	CombatBubble,
	DefaultTypeAndCount // Stop registering colliders to this layer once we have 8 enums options above this one, otherwise the collisionmanager will crash
};

struct MeshData 
{
	std::vector<physx::PxVec3> vertices;
	std::vector<std::vector<int>> indices;
	int vertexCount;
};