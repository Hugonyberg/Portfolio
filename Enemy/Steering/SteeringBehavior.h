struct WeightedForce
{
	DE::Vector3f direction;
	float weight;

	WeightedForce() : direction(0.f, 0.f, 0.f), weight(0.f) {}
	WeightedForce(const DE::Vector3f& dir, float w) : direction(dir), weight(w) {}
};

struct SeparationRaycastInfo
{
	const char* label = "Custom";
	float yawOffset;
	float pitchOffset;
	DE::Vector3f direction;
	float weight;
	float separationRayLength;
	bool isColliding;
	float collidingDistance;
};

struct RayPreset
{
	float yawOffset;
	float pitchOffset;
	const char* label;
};

static const RayPreset rayPresets[] =
{
	{  0.0f,   0.0f, "Forward"      },
	{ 90.0f,   0.0f, "Right"        },
	{-90.0f,   0.0f, "Left"         },
	{  0.0f, -90.0f, "Down"         },
	{  0.0f,  90.0f, "Up"           },
	{180.0f,   0.0f, "Back"         },

	{  0.0f, -45.0f, "Forward-Down" },
	{  0.0f,  45.0f, "Forward-Up"   },
	{ 45.0f,   0.0f, "Forward-Right"},
	{-45.0f,   0.0f, "Forward-Left" },

	{ 45.0f, -45.0f, "Right-Down"   },
	{-45.0f, -45.0f, "Left-Down"    },
	{ 45.0f,  45.0f, "Right-Up"     },
	{-45.0f,  45.0f, "Left-Up"      },

	{180.0f, -45.0f, "Back-Down"    },
	{180.0f,  45.0f, "Back-Up"      },
	{135.0f,   0.0f, "Back-Right"   },
	{-135.0f,  0.0f, "Back-Left"    },
};

class SteeringBehavior
{
public:
	SteeringBehavior();

	void SetActiveGroundEnemies(std::vector<std::shared_ptr<GroundEnemy>> someEnemies);
	void SetActiveFlyingEnemies(std::vector<std::shared_ptr<FlyingEnemy>> someEnemies);

	DE::Vector3f UpdateSteering();
	void SetUpdateParameters(float aDeltaTime, const DE::Vector3f& aPosition, const DE::Vector3f& aVelocity, const DE::Vector3f& aTarget, const float aSpeed);

	WeightedForce GetSeparationSteering();
	WeightedForce GetObjectSeparationSteering(); // Raycasts
	WeightedForce GetSeekSteering();
	WeightedForce GetNavmeshSteering();
	WeightedForce GetSpreadSteering();

	void AddSeparationRay(const SeparationRaycastInfo& aRay);
	std::vector<SeparationRaycastInfo>& GetSeparationRays() { return mySeparationRays; }
	void SetRays(std::vector<SeparationRaycastInfo>& someRays);

	void SetNavmesh(std::shared_ptr<DE::Navmesh> aNavmesh) { myNavmesh = aNavmesh; }

	const DE::Vector3f& GetPosition() const { return myPosition; }
	void SetPosition(const DE::Vector3f& position) { myPosition = position; }

	const DE::Vector3f& GetVelocity() const { return myVelocity; }
	void SetVelocity(const DE::Vector3f& velocity) { myVelocity = velocity; }

	const DE::Vector3f& GetSeekDirection() const { return mySeekDirection; }
	void SetSeekDirection(const DE::Vector3f& direction) { mySeekDirection = direction; }

	const DE::Vector3f& GetTarget() const { return myTarget; }
	void SetTarget(const DE::Vector3f& target) { myTarget = target; }

	float GetSpeed() const { return mySpeed; }
	void SetSpeed(float speed) { mySpeed = speed; }

	float GetScale() const { return myScale; }
	void SetScale(float aScale) { myScale = aScale; }

	float GetArrivalRadius() const { return myArrivalRadius; }
	void SetArrivalRadius(float radius) { myArrivalRadius = radius; }

	float GetSeparationRadius() const { return mySeparationRadius; }
	void SetSeparationRadius(float radius) { mySeparationRadius = radius; }

	float GetObjectSeparationWeight() const { return myObjectSeparationWeight; }
	void SetObjectSeparationWeight(float aWeight) { myObjectSeparationWeight = aWeight; }

	float GetEnemySeparationWeight() const { return myEnemySeparationWeight; }
	void SetEnemySeparationWeight(float aWeight) { myEnemySeparationWeight = aWeight; }

	float GetSmoothingFactor() const { return mySteeringSmoothingFactor; }
	void SetSmoothingFactor(float aFactor) { mySteeringSmoothingFactor = aFactor; }

	float GetMaxTurnRate() const { return myMaxTurnRate; }
	void SetMaxTurnRate(float aTurnRate) { myMaxTurnRate = aTurnRate; }

	float GetSeekWeight() const { return mySeekWeight; }
	void SetSeekWeight(float aWeight) { mySeekWeight = aWeight; }

	bool GetSeekFlag() const { return mySeekFlag; }
	void SetSeekFlag(bool aFlag) { mySeekFlag = aFlag; }

	bool GetSeparationFlag() const { return mySeparationFlag; }
	void SetSeparationFlag(bool aFlag) { mySeparationFlag = aFlag; }

	bool GetObjectSeparationFlag() const { return myObjectSeparationFlag; }
	void SetObjectSeparationFlag(bool aFlag) { myObjectSeparationFlag = aFlag; }

	bool GetSpreadFlag() const { return mySpreadFlag; }
	void SetSpreadFlag(bool aFlag) { mySpreadFlag = aFlag; }

	bool GetNavmeshSteeringFlag() const { return myNavmeshSteeringFlag; }
	void SetNavmeshSteeringFlag(bool aFlag) { myNavmeshSteeringFlag = aFlag; }

	physx::PxRigidDynamic* GetBody() const { return myBody; }
	void SetBody(physx::PxRigidDynamic* body) { myBody = body; }

	void DebugRender(DreamEngine::GraphicsEngine& aGraphicsEngine);

	void SetObjectSeparationOffset(float anOffset) { myObjectSeparationOffset = anOffset; }

private:
	std::vector<SeparationRaycastInfo> mySeparationRays;

	physx::PxRigidDynamic* myBody;

	std::vector<std::shared_ptr<FlyingEnemy>> myActiveFlyingEnemies;
	std::vector<std::shared_ptr<GroundEnemy>> myActiveGroundEnemies;

	std::shared_ptr<DE::Navmesh> myNavmesh;

	DE::Vector3f myPosition;
	DE::Vector3f myVelocity;
	DE::Vector3f myTarget;
	float mySpeed;
	float myScale;

	float myArrivalRadius;
	float mySeparationRadius;

	float myEnemySeparationWeight;
	float myObjectSeparationWeight;
	float myNavmeshWeight;
	float mySeekWeight;

	float mySteeringSmoothingFactor;
	float myMaxTurnRate;

	bool mySeekFlag = true;
	bool mySeparationFlag = true;
	bool myObjectSeparationFlag = true;
	bool mySpreadFlag = false;
	bool myNavmeshSteeringFlag = false;
};