#pragma once
#include <array>
#include <vector>
#include <DreamEngine\math\Vector3.h>
#include "ColliderCommon.h"

class GameObject;

class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();

	void CheckCollisions();
	void RegisterObject(GameObject* aGameObject, const eCollisionLayer aLayer);
	void DeregisterObject(GameObject* aGameObject, const eCollisionLayer aLayer);
	void DeregisterAll();
	void ClearColliders();
	bool CheckIfPosTooCloseToBubbleFence(const DE::Vector3f aPos) const;

private:
	std::array<std::vector<GameObject*>, 8> myRegisteredObjectsByLayer;
};