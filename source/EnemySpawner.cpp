#include "EnemySpawner.h"
#include "EnemyMele.h"
#include "EnemyShooting.h"
#include "MainSingleton.h"
#include "RigidBodyComponent.h"

#include <PhysX\PxPhysicsAPI.h> 
#include <DreamEngine/graphics/ModelFactory.h>

EnemySpawner::EnemySpawner(const float aSpawnerRadiusRange) : myRadius(aSpawnerRadiusRange)
{
    mySpawnerCooldown.SetCurrentValue(60.0f);
}

EnemySpawner::~EnemySpawner()
{}

void EnemySpawner::Init(const int anAmountOfShooting, const int anAmountOfMelee, DE::Transform aTransform)
{
    myTransform = aTransform;

    for (int i = 0; i < anAmountOfShooting; i++)
    {
        std::shared_ptr<EnemyShooting> shooting = MainSingleton::GetInstance()->GetEnemyFactory().CreateEnemyShooting();

        auto transform = myTransform;
        auto pos = transform.GetPosition();
        pos.y = myEnemySpawnWater;
        transform.SetPosition(pos);
        shooting->SetTransform(transform);

        shooting->SetModelInstance(std::make_shared<DreamEngine::ModelInstance>(DreamEngine::ModelFactory::GetInstance().GetModelInstance(L"3D/SM_CH_Spitter.fbx")));
        shooting->Init();

        myShootingEnemies.push_back(shooting);
    }
    for (size_t i = 0; i < anAmountOfMelee; i++)
    {
        std::shared_ptr<EnemyMele> melee = MainSingleton::GetInstance()->GetEnemyFactory().CreateEnemyMelee();

        auto transform = myTransform;
        auto pos = transform.GetPosition();
        pos.y = myEnemySpawnSky; 
        transform.SetPosition(pos);
        melee->SetTransform(transform); 

        melee->SetModelInstance(std::make_shared<DreamEngine::ModelInstance>(DreamEngine::ModelFactory::GetInstance().GetModelInstance(L"3D/SM_CH_Splatter.fbx"))); 
        melee->Init();

        myMeleeEnemies.push_back(melee);
    }
}

void EnemySpawner::Update(float aDeltaTime, DE::Transform aVesselTransform)
{
    mySpawnerCooldown.Update(aDeltaTime);

    auto vesslePos = aVesselTransform.GetPosition();
    vesslePos.y = myTransform.GetPosition().y;


    float distans = (vesslePos - myTransform.GetPosition()).Length();
    if (distans < myRadius && !mySpawnerActive)
    {
        for (auto& melee : myMeleeEnemies)
        {
            if(melee->GetState() == EnemyMele::eState::count)
                melee->SetState(EnemyMele::eState::eArriving);
        }
        for (auto& shoot : myShootingEnemies)
        {
            if(shoot->GetState() == EnemyShooting::eState::count)
                shoot->SetState(EnemyShooting::eState::eArriving);
        }
    }

    if (CheckActiveSpawner()) 
        mySpawnerCooldown.Reset();

    for (auto& melee : myMeleeEnemies)
    {
        melee->SetBoatTransform(aVesselTransform);
        melee->Update(aDeltaTime);
    }
    for (auto& shoot : myShootingEnemies)
    {
        shoot->SetBoatTransform(aVesselTransform);
        shoot->Update(aDeltaTime);
    }
}

void EnemySpawner::Render(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
    for (auto& melee : myMeleeEnemies)
    {
        melee->Render(aGraphicsEngine);
    }
    for (auto& shoot : myShootingEnemies)
    {
        shoot->Render(aGraphicsEngine);
        shoot->RenderProjectiles(aGraphicsEngine);
    }
}

std::vector<DE::Vector3f> EnemySpawner::GetEnemiePositions()
{
    std::vector<DE::Vector3f> allPositions;

    for (auto& melee : myMeleeEnemies)
    {
        if(melee->IsActive())
            allPositions.push_back(melee->GetTransform()->GetPosition());
    }
    for (auto& shoot : myShootingEnemies)
    {
        if (shoot->IsActive())
            allPositions.push_back(shoot->GetTransform()->GetPosition());
    }

    return allPositions;
}

void EnemySpawner::SetBoatWeakPoint(DE::Vector3f aWaypoint, int anIndex)
{
    for (auto& melee : myMeleeEnemies)
    {
        melee->SetAttackWayPoints(aWaypoint, anIndex);
    }
    for (auto& shoot : myShootingEnemies)
    {
        shoot->SetAttackWayPoints(aWaypoint);
    }
}

bool EnemySpawner::CheckActiveSpawner()
{
    for (auto& melee : myMeleeEnemies)
    {
        if (melee->IsActive())
            return false;
    }
    for (auto& shoot : myShootingEnemies)
    {
        if (shoot->IsActive())
            return false;
    }

    return true;
}

void EnemySpawner::ResetSpawner()
{
    for (auto& melee : myMeleeEnemies)
    {
        melee->Respawn();
    }
    for (auto& shoot : myShootingEnemies)
    {
        shoot->Respawn();
    }
}
