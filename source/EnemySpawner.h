#pragma once
#include <vector>
#include <memory>
#include <DreamEngine\graphics\GraphicsEngine.h> 
#include <DreamEngine\math\Transform.h>
#include <DreamEngine\utilities\CountTimer.h>

class EnemyShooting;
class EnemyMele;

class EnemySpawner
{
public:
	EnemySpawner(const float aSpawnerRadiusRange);
	~EnemySpawner();

	void Init(const int anAmountOfShooting, const int anAmountOfMelee, DE::Transform aTransform);
	void Update(float aDeltaTime, DE::Transform aVesselTransform);
	void Render(DreamEngine::GraphicsEngine& aGraphicsEngine);

	std::vector<DE::Vector3f>GetEnemiePositions();
	void SetBoatWeakPoint(DE::Vector3f aWaypoint, int anIndex);

private:
	bool CheckActiveSpawner();
	void ResetSpawner(); 

private:
	std::vector<std::shared_ptr<EnemyShooting>> myShootingEnemies; 
	std::vector<std::shared_ptr<EnemyMele>> myMeleeEnemies;

	DE::Transform myTransform; 
	CU::CountdownTimer mySpawnerCooldown; 

	const float myRadius; 
	const float myEnemySpawnSky = 300.0f;
	const float myEnemySpawnWater = -300.0f;

	bool mySpawnerActive = false;
};

