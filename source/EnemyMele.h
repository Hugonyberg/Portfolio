#pragma once
#include "Enemy.h"

#include <DreamEngine/graphics/ModelInstance.h>
#include <PhysX\PxPhysicsAPI.h> 
#include <vector>
#include <array>

class Blackboard;
class EnemySteeringBehaviour;
class EnemyMele : public Enemy 
{
public:
    enum class eState
    {
        eAttacking,
        eIdle,
        eWalking,
        eArriving,
        count
    };
    
    EnemyMele(const EnemyType& anEnemyType) : Enemy(anEnemyType){}
	~EnemyMele();

    void Init();
    void Update(float aDeltaTime) override;

    void SetAnimatedModel() override;

    void Respawn() override;
    void Spawn(DE::Vector3f aSpawnPoint) override;
public:
    void SetAttackWayPoints(DE::Vector3f aWaypoint, int anIndex);
    void SetState(eState aState){ myState = aState; }
    const eState GetState() { return myState; }
    void SetBoatTransform(DE::Transform aBoatTransform) { myBoatTransform = aBoatTransform; }
protected:
    void SetAnimationState(float aDeltaTime) override;
    void CountDownCooldowns(float aDeltaTime) override;
    void UpdatePosition(float aDeltaTime);
    DE::Vector3f UpdatePhysix(float aDeltaTime);
    void CaptureLocalTransform(const DreamEngine::Transform& boatXform); 
private:
    eState myState = eState::count;
    EnemySteeringBehaviour* mySteeringBehavior; 

    std::array<DE::Vector3f, 6> myAttackWayPoint;

    DE::Transform myBoatTransform;
    DE::Transform myLocalTransform;

    float myMaxSpeed;

    int myAttackPointIndex = 0;
};

