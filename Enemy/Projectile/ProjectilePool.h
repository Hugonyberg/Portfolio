
class ProjectilePool
{
public:
    ProjectilePool() = delete;
    ProjectilePool(size_t aSize);
    ~ProjectilePool();

    Projectile* GetProjectile(DreamEngine::Vector3f aPosition, DreamEngine::Vector3f aDirection, DE::Vector3f aTargetPosition);
    void DeactivateProjectile(Projectile* aProjectile);

    void Update(float aDeltaTime);
    void RenderProjectileVFX(DreamEngine::GraphicsEngine& aGraphicsEngine);
    void Render(DreamEngine::GraphicsEngine& aGraphicsEngine);
    void RenderColliders();

    void ResetProjectiles();

private:
    std::vector<std::unique_ptr<Projectile>> myPool;  // Pool of projectile objects
    size_t myNextAvailable;  // Index of the next available projectile in the pool
    bool myMakeEnemyProjectiles;

    // Private method to find the next available projectile
    size_t FindNextAvailable();
};