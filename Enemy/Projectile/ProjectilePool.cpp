
ProjectilePool::ProjectilePool(size_t aSize)
{
	myPool.reserve(aSize);
	myVFXTimer = 0;
	for (size_t i = 0; i < aSize; i++)
	{
		myPool.push_back(std::make_unique<Projectile>());

		myPool.back()->AddComponent<ColliderComponent>();
		auto* colliderCmp = myPool.back()->GetComponent<ColliderComponent>();
		colliderCmp->SetCollider(std::make_shared<DE::SphereCollider<float>>(DE::SphereCollider<float>(myPool.back()->GetTransform(), 0, myProjectileSize)));

		myPool.back()->SetDamage(myProjectileDamage);
		myPool.back()->Deactivate();
		myPool.back()->SetEnemysProjectile(false);
	}

	myNextAvailable = 0;
}

Projectile* ProjectilePool::GetProjectile(DreamEngine::Vector3f aPosition, DreamEngine::Vector3f aDirection, DE::Vector3f aTargetPosition)
{
	size_t index = FindNextAvailable();
	if (index != std::numeric_limits<size_t>::max())
	{
		myPool[index]->Activate(aPosition, aDirection, aTargetPosition); 
		myNextAvailable = (index + 1) % myPool.size(); // Update the next available index

		return myPool[index].get();
	}

	return nullptr;
}

void ProjectilePool::DeactivateProjectile(Projectile* aProjectile)
{
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
		}
	}
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

	return std::numeric_limits<size_t>::max(); // Return an invalid index if all projectiles are active
}