#pragma once
#include "Enemy.h"

#include <vector>
#include <PhysX\PxPhysicsAPI.h>
#include <DreamEngine\utilities\CountTimer.h>
#include <DreamEngine/graphics/ModelInstance.h>

class EnemySteeringBehaviour;

class Leviathan : public Enemy
{
public:
    enum class eState
    {
        eSearching,
        ePathing,
        eChasing,
        count
    };

	Leviathan(const EnemyType& anEnemyType): Enemy(anEnemyType){}
	~Leviathan(); 

    void Init(const float anAttackCoolDown, const float anAttackingSpeed,const float anAttackRange, const float aSeekingSpeed, const float aSeekRange);
    void Update(float aDeltaTime) override;

    void SetAnimatedModel() override;

    void Respawn() override;
    void Spawn(DE::Vector3f aSpawnPoint) override{ aSpawnPoint; } //not being used

public:
    void UpdateStateMachine(float aDeltaTime);
    void SetWayPoints(DE::Vector3f aWaypoint);
    DE::Vector3f GetBoatPosition();
    void CheckMastheadLight(bool aState) { myMastheadLightActive = aState; }

protected:
    void SetAnimationState(float aDeltaTime) override;
    void CountDownCooldowns(float aDeltaTime) override;

private:
    eState myState; 
    EnemySteeringBehaviour* mySteeringBehavior;

    std::vector<DE::Vector3f> myWayPointPositions;
    int myWayPointIndex = 0;

    CU::CountdownTimer myAttackCooldown;

    DE::Vector3f mySpawnPosition;

    float myAttackSpeed;
    float myAttackRange;
    float mySeekingSpeed;
    float mySeekingRange;

    bool myMastheadLightActive = false;

};

