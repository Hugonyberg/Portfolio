
class Projectile : public GameObject
{
public:
    Projectile();
    ~Projectile();

    void Update(float aDeltaTime);
    void Render(DreamEngine::GraphicsEngine& aGraphicsEngine) override;

    void Activate(DreamEngine::Vector3f aPosition, DreamEngine::Vector3f aDirection, DreamEngine::Vector3f aTargetPosition);
    void Deactivate();
    
    void SetDamage(const int aDamageAmount);
    const int GetDamage() const;

    void SetDrillID(char aChar) { myDrillID = aChar; }

private:
    float GetCurvatureTowardPos(DE::Vector3f aPos);
    void TurnTowards(DE::Vector3f aPos);
    
    void HandleCollision();

    std::shared_ptr<DreamEngine::AnimatedModelInstance> myAnimatedModelInstance;
    std::shared_ptr<DreamEngine::ModelInstance> myModelInstance;
    DE::Vector3f myVelocity;
    DE::Vector3f myTargetPosition;

    float myTimeLimit = 10.f;
    float myTimer;

    float mySpeed;
    float myScale;

    int myDamage;

    char myDrillID;
};
