#pragma once
#include "GameObject.h"
#include <DreamEngine/shaders/ModelShader.h>
#include <DreamEngine/graphics/AnimationPlayer.h>


namespace DreamEngine
{
    class ModelInstance;
    class AnimatedModelInstance;
}

class Projectile : public GameObject
{
public:
    Projectile();
    ~Projectile();
    void SetModelInstance(std::shared_ptr<DreamEngine::AnimatedModelInstance> aModel);
    void SetDamage(const int aDamageAmount);
    void SetEnemysProjectile(const bool isEnemy);

    void Update(float aDeltaTime);
    void OnCollision(GameObject* anObject, eCollisionLayer aCollisionLayer) override;

    void Activate(DreamEngine::Vector3f aPosition, DreamEngine::Vector3f aDirection, DE::Transform* aTargetTransform);
    void Activate(DreamEngine::Vector3f aPosition, DreamEngine::Vector3f aDirection, DreamEngine::Vector3f aTargetPosition);
    void Deactivate();
    
    const int GetDamage() const;

    void HandlePlayerCollision();
    void HandleEnemyCollision();

    void Render(DreamEngine::GraphicsEngine& aGraphicsEngine) override;

    void SetDrillID(char aChar) { myDrillID = aChar; }

private:
    float GetCurvatureTowardPos(DE::Vector3f aPos);
    void TurnTowards(DE::Vector3f aPos);


    DE::AnimationPlayer myAnimationPlayer;
    std::shared_ptr<DreamEngine::AnimatedModelInstance> myAnimatedModelInstance;
    std::shared_ptr<DreamEngine::ModelInstance> myModelInstance;
    DE::Transform* myTargetTransform;
    DE::ModelShader* myModelVFX;
    DE::Vector3f myVelocity;
    DE::Vector3f myTargetPosition;

    float myTimeLimit = 10.f;
    float myTimer;
    float mySpeed = 2000.f; //testspeed
    float myScale = 2.f;

    int myDamage = 1;

    char myDrillID;
    bool myRotationSet;
    bool myEnemyProjectile;
};
