#pragma once
#include <vector>
#include <memory>
#include <DreamEngine/graphics/ModelInstance.h>
#include "Projectile.h"
#include "Observer.h"

namespace DreamEngine
{
    class Texture;
}

class ProjectilePool : public Observer
{
public:
    ProjectilePool() = delete;
    ProjectilePool(size_t aSize, bool isEnemy);
    ~ProjectilePool();

    Projectile* GetProjectile(DreamEngine::Vector3f aPosition, DreamEngine::Vector3f aDirection, DE::Transform* aTargetTransform);
    Projectile* GetProjectile(DreamEngine::Vector3f aPosition, DreamEngine::Vector3f aDirection, DreamEngine::Vector3f aTargetTransform, char anID);
    void ReleaseProjectile(Projectile* aProjectile);

    void Update(float aDeltaTime);
    void RenderProjectileVFX(DreamEngine::GraphicsEngine& aGraphicsEngine);
    void Render(DreamEngine::GraphicsEngine& aGraphicsEngine);
    void RenderColliders();

    void Receive(const Message& aMsg) override;
    void ResetProjectiles();
    void ResetVFX();

private:
    std::vector<std::unique_ptr<Projectile>> myPool;  // Pool of projectile objects
    size_t myNextAvailable;  // Index of the next available projectile in the pool
    bool myMakeEnemyProjectiles;

    // Private method to find the next available projectile
    size_t FindNextAvailable();
    DE::ModelInstance myVFXMuzzleFlashModelInstance;
    DE::Texture* myMuzzleFlashTextureOne;
    DE::Texture* myMuzzleFlashTextureTwo;

    float myVFXTimer;
};