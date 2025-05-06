#include "stdafx.h"
#include "ProjectilePool.h"
#include "MainSingleton.h"
#include "ColliderComponent.h"
#include <DreamEngine\math\Collider.h>
#include <DreamEngine/graphics/ModelFactory.h>
#include <DreamEngine/graphics/TextureManager.h>
#include <DreamEngine/graphics/GraphicsStateStack.h>
#include <DreamEngine/graphics/GraphicsEngine.h>
#include "ShaderTool.h"

ProjectilePool::ProjectilePool(size_t aSize, bool isEnemy)
{
	myPool.reserve(aSize);
	myMakeEnemyProjectiles = isEnemy;
	myVFXTimer = 0;
	for (size_t i = 0; i < aSize; i++)
	{
		myPool.push_back(std::make_unique<Projectile>());

		myPool.back()->AddComponent<ColliderComponent>();
		auto* colliderCmp = myPool.back()->GetComponent<ColliderComponent>();
		colliderCmp->SetCollider(std::make_shared<DE::SphereCollider<float>>(DE::SphereCollider<float>(myPool.back()->GetTransform(), 0, 35.0f)));
		myPool[i]->SetDamage(1.0f);
		myPool[i]->Deactivate();
		myPool[i]->SetEnemysProjectile(false);
	}

	myVFXMuzzleFlashModelInstance = DE::ModelFactory::GetInstance().GetModelInstance(L"3D/SM_Ch_Dog.fbx");
	myMuzzleFlashTextureOne = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_MuzzleFlash01.dds");
	myMuzzleFlashTextureTwo = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_MuzzleFlash02.dds");

	myVFXMuzzleFlashModelInstance.SetTexture(0, 0, myMuzzleFlashTextureOne);
	myVFXMuzzleFlashModelInstance.SetTexture(0, 1, myMuzzleFlashTextureTwo);

	myNextAvailable = 0;
}

ProjectilePool::~ProjectilePool()
{
	for (size_t i = 0; i < myPool.size(); i++)
	{
	}
}

Projectile* ProjectilePool::GetProjectile(DreamEngine::Vector3f aPosition, DreamEngine::Vector3f aDirection, DE::Transform* aTargetTransform)
{
	size_t index = FindNextAvailable();
	if (index != std::numeric_limits<size_t>::max())
	{
		// You need to specify activation parameters or handle this inside the Projectile class
		myPool[index]->Activate(aPosition, aDirection, aTargetTransform); 

		// Update the next available index
		myNextAvailable = (index + 1) % myPool.size(); 

		return myPool[index].get();
	}

	return nullptr;
}

Projectile* ProjectilePool::GetProjectile(DreamEngine::Vector3f aPosition, DreamEngine::Vector3f aDirection, DreamEngine::Vector3f aTargetTransform, char anID)
{
	size_t index = FindNextAvailable();
	if (index != std::numeric_limits<size_t>::max())
	{
		// You need to specify activation parameters or handle this inside the Projectile class
		myPool[index]->Activate(aPosition, aDirection, aTargetTransform);
		myVFXMuzzleFlashModelInstance.SetTransform(*myPool[index]->GetTransform());
		myVFXTimer = 0.f;
		// Update the next available index
		myNextAvailable = (index + 1) % myPool.size();
		myPool[index]->SetDrillID(anID);

		return myPool[index].get();
	}

	return nullptr;
}

void ProjectilePool::ReleaseProjectile(Projectile* aProjectile)
{
	// Ensure the projectile is actually part of the pool
	for (auto& projectile : myPool)
	{
		if (projectile.get() == aProjectile)
		{
			projectile->Deactivate();
			break;
		}
	}
}

void ProjectilePool::Update(float aDeltaTime)
{
	for (size_t i = 0; i < myPool.size(); ++i)
	{
		if (myPool[i]->IsActive())
		{
			myPool[i]->Update(aDeltaTime);
			myPool[i]->SetWasActive(myPool[i]->IsActive());
		}
	}
	
	myVFXTimer += aDeltaTime;

}

void ProjectilePool::RenderProjectileVFX(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
	for (size_t i = 0; i < myPool.size(); ++i)
	{
		if (myVFXTimer > 0 && myVFXTimer < 0.5f)
		{
			MainSingleton::GetInstance()->GetShaderTool().Render(aGraphicsEngine.GetGraphicsStateStack(), myVFXTimer, myVFXMuzzleFlashModelInstance, static_cast<int>(eStaticVFXType::MuzzleFlash));
		}
	}
}

void ProjectilePool::Render(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
	for (size_t i = 0; i < myPool.size(); ++i)
	{
		if (myPool[i]->IsActive())
		{
			myPool[i]->Render(aGraphicsEngine);
		}
	}
}

void ProjectilePool::RenderColliders()
{
	for (auto& projectile : myPool)
	{
		ColliderComponent* outComponent;
		if (projectile->IsActive() == false || !projectile->TryGetComponent<ColliderComponent>(outComponent))
		{
			continue;
		}
		outComponent->GetCollider()->RenderVisualization();
	}
}


void ProjectilePool::Receive(const Message& aMsg)
{

}

void ProjectilePool::ResetProjectiles()
{
	for (int i = 0; i < myPool.size(); i++)
	{
		myPool[i]->Deactivate();
	}
}

void ProjectilePool::ResetVFX()
{
	myVFXTimer = 0;
}

size_t ProjectilePool::FindNextAvailable()
{
	for (size_t i = 0; i < myPool.size(); ++i)
	{
		if (!myPool[i]->IsActive())
		{
			return i;
		}
	}
	// Optionally handle the case where all projectiles are in use
	return std::numeric_limits<size_t>::max(); // Return an invalid index if all projectiles are active
}