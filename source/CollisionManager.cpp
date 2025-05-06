#include "CollisionManager.h"
#include "GameObject.h"
#include <DreamEngine/math/Collider.h>
#include <DreamEngine/math/Intersection.h>
#include "ColliderComponent.h"

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::CheckCollisions()
{
	for (int layer = 0; layer < myRegisteredObjectsByLayer.size(); layer++)
	{
		auto size = myRegisteredObjectsByLayer[layer].size();
		if (size < 2)
		{
			continue;
		}

		GameObject** colliderObjects = &myRegisteredObjectsByLayer[layer][0];
		for (int i = 0; i < size; i++)
		{
			if(!colliderObjects[i]->IsActive() || (colliderObjects[i]->GetTag() != eTag::Enemy && colliderObjects[i]->GetTag() != eTag::Player && colliderObjects[i]->GetTag() != eTag::Boss))
			{
				continue;
			}
			for (int j = 0; j < size; j++)
			{
				eCollisionLayer currentLayer = static_cast<eCollisionLayer>(layer);
				if (i == j || !myRegisteredObjectsByLayer[layer][j]->IsActive() || (colliderObjects[i]->GetTag() == eTag::Enemy && myRegisteredObjectsByLayer[layer][j]->GetTag() == eTag::Enemy) || (currentLayer == eCollisionLayer::EnemyAttacks && colliderObjects[i]->GetTag() == eTag::EnemyProjectile && myRegisteredObjectsByLayer[layer][j]->GetTag() == eTag::EnemyProjectile))
				{
					continue;
				}

				// Check collision between i and j objs, below is just an example
				// We'd actually need to write a long function call to check which collider type each object needs to fetch
				// Then check the appropriate collision between the two
				ColliderComponent* iCollider = myRegisteredObjectsByLayer[layer][i]->GetComponent<ColliderComponent>();
				ColliderComponent* jCollider = myRegisteredObjectsByLayer[layer][j]->GetComponent<ColliderComponent>();
				if (DE::CheckCollisionSphereSphere(*static_cast<const DE::SphereCollider<float>*>(iCollider->GetCollider().get()), *static_cast<const DE::SphereCollider<float>*>(jCollider->GetCollider().get())))
				{
					myRegisteredObjectsByLayer[layer][i]->OnCollision(myRegisteredObjectsByLayer[layer][j], currentLayer);

					if ((myRegisteredObjectsByLayer[layer][j]->GetTag() != eTag::Enemy || colliderObjects[i]->GetTag() == eTag::Player && currentLayer == eCollisionLayer::Movement && myRegisteredObjectsByLayer[layer][j]->GetTag() == eTag::Enemy) && myRegisteredObjectsByLayer[layer][j]->GetTag() != eTag::Player)
					{
						myRegisteredObjectsByLayer[layer][j]->OnCollision(myRegisteredObjectsByLayer[layer][i], currentLayer);
					}
				}
			}
		}
	}
}

void CollisionManager::RegisterObject(GameObject* aGameObject, const eCollisionLayer aLayer)
{
	for (int i = 0; i < myRegisteredObjectsByLayer[static_cast<int>(aLayer)].size(); i++)
	{
		if (aGameObject->GetID() == myRegisteredObjectsByLayer[static_cast<int>(aLayer)][i]->GetID())
		{
			return;
		}
	}
	myRegisteredObjectsByLayer[static_cast<int>(aLayer)].push_back(aGameObject);
}

void CollisionManager::DeregisterObject(GameObject* aGameObject, const eCollisionLayer aLayer)
{
	for (int i = 0; i < myRegisteredObjectsByLayer[static_cast<int>(aLayer)].size(); i++)
	{
		if (myRegisteredObjectsByLayer[static_cast<int>(aLayer)][i]->GetID() == aGameObject->GetID())
		{
			myRegisteredObjectsByLayer[static_cast<int>(aLayer)].erase(myRegisteredObjectsByLayer[static_cast<int>(aLayer)].begin() + i);
			return;
		}
	}
}
void CollisionManager::DeregisterAll()
{
	for(int layer = 0; layer < static_cast<int>(eCollisionLayer::DefaultTypeAndCount) + 1; layer++)
	{
		myRegisteredObjectsByLayer[layer].clear();
	}
}

void CollisionManager::ClearColliders()
{
	for (int i = 0; i < myRegisteredObjectsByLayer.size(); i++)
	{
		myRegisteredObjectsByLayer[i].clear();
	}
}

bool CollisionManager::CheckIfPosTooCloseToBubbleFence(const DE::Vector3f aPos) const
{
	int movementLayerNum = static_cast<int>(eCollisionLayer::Movement);
	for (int i = 0; i < myRegisteredObjectsByLayer[movementLayerNum].size(); i++)
	{
		if (myRegisteredObjectsByLayer[movementLayerNum][i]->GetTag() == eTag::CombatBubbelExtraCollider) 
		{
			if ((myRegisteredObjectsByLayer[movementLayerNum][i]->GetTransform()->GetPosition() - aPos).LengthSqr() < 50.0f * 50.0f) 
			{
				return true;
			}
		}
	}
	return false;
}