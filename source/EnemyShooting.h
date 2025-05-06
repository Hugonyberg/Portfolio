#pragma once
#include "Enemy.h"

#include <DreamEngine/graphics/ModelInstance.h>
#include <DreamEngine\utilities\CountTimer.h>
#include <PhysX\PxPhysicsAPI.h> 
#include <vector>

class ProjectilePool;
class EnemySteeringBehaviour; 
class EnemyShooting : public Enemy 
{
public:
    enum class eState
    {
        eAttacking,
        eIdle,
        eArriving,
        count
    };

    EnemyShooting(const EnemyType& anEnemyType) : Enemy(anEnemyType){}
    ~EnemyShooting();

    void Init();
    void Update(float aDeltaTime) override;

    void SetAnimatedModel() override;

    void Respawn() override;
    void Spawn(DE::Vector3f aSpawnPoint) override;
public:
    void SetAttackWayPoints(DE::Vector3f aPosition);
    void SetBoatTransform(DE::Transform aTransform) { myBoatTransform = aTransform; }
    void SetState(eState aState){ myState = aState; }
    const eState GetState() { return myState; }

    void RenderProjectiles(DreamEngine::GraphicsEngine& aGraphicsEngine);

protected:
    void SetAnimationState(float aDeltaTime) override;
    void SetOffsetPosition(DE::Vector3f aBoatPos);

    void CountDownCooldowns(float aDeltaTime) override;

    void UpdateStateMachine(float aDeltaTime); 
    void UpdatePhysics(float aDeltaTime);

private:
    eState myState = eState::count;
    EnemySteeringBehaviour* mySteeringBehavior = nullptr;
    ProjectilePool* myProjectilePool = nullptr;

    CU::CountdownTimer myAttackCooldown;

    std::vector<DE::Vector3f> myAttackWayPoint;

    DE::Vector3f mySpawningPosition; 
    DE::Transform myBoatTransform; 
    DE::Vector3f myOffsetPosition; 

    float myMaxSpeed;
    float myRandomizeUp;
    float myRandomizeRight;
};

