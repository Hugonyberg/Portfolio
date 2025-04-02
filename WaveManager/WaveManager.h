
class WaveManager
{
public:
    WaveManager();

    void Init(std::shared_ptr<DE::Navmesh> aNavmesh, std::shared_ptr<DE::Navmesh> aDetailedNavmesh, std::string& aLevelName);
    void Update(float aDeltaTime);
    bool AreAllWavesComplete() const;
    void Render(DreamEngine::GraphicsEngine& aGraphicsEngine);

    void RestartWave(const std::array<std::vector<Pulse>, 5>& aWaveConfig, int aPoolSize);
    void RestartCurrentWave();
    void HandleImGui();

    void SetSpawnPoints(std::vector<std::shared_ptr<SpawnPoint>>& someSpawnPoints) { mySpawnPoints = someSpawnPoints; }
    void SetDefensePoints(std::vector<std::shared_ptr<DefensePoint>>& someDefensePoints) { myDefensePoints = someDefensePoints; }

    std::shared_ptr<EnemyPool> GetEnemyPool() { return myEnemyPool; }

    std::vector<DE::Vector3f> GetPathfindingPoints() { return myPath; }
    void SetPathfindingPoints(std::vector<DE::Vector3f>& aPath);

    void SetParticleManagerPointer(ParticleManager* aManager);

    std::shared_ptr<Blackboard> GetBlackboard() { return myBlackboard; }

    void DebugRender(DreamEngine::GraphicsEngine& aGraphicsEngine);

private:
    void UpdateHUDInfo();
    void CalculateAndInitNavmeshPaths();
    void CalculateCirclePoints(const DE::Vector3f& origin, float radius, float spacing);

    void SaveToFile(const std::string& aFilename);
    void LoadFromFile(const std::string& aFilename);
    char myFileNameInput[256] = "DefaultName";

    std::unordered_map<std::string, std::vector<DE::Vector3f>> myPaths;

    std::array<std::unique_ptr<Wave>, 5> myWaves; 
    size_t myCurrentWaveIndex;                   
    std::shared_ptr<EnemyPool> myEnemyPool;

    std::array<std::vector<Pulse>, 5> myWaveConfig; 
    int myPoolSize = 50;

    std::vector<DE::Vector3f> myPath;
    std::vector<std::shared_ptr<DefensePoint>> myDefensePoints;
    std::vector<std::shared_ptr<SpawnPoint>> mySpawnPoints;

    std::shared_ptr<Blackboard> myBlackboard;

    ParticleManager* myParticleManager;

    float myTimeToFirstPulse[5];

    bool myShouldStart = false;
};
