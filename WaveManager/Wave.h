
struct Pulse
{
    size_t numberOfShooterEnemies;  // Number of flying enemies in this pulse
    size_t numberOfKamikazeEnemies; // Number of flying enemies in this pulse
    size_t numberOfGroundEnemies;   // Number of ground enemies in this pulse
    float timeToNextPulse;          // Interval between this and next pulse
    char spawnPointLabel[2];        // A, B or C(1, 2, 3 in-game). 
    char defensePointLabel[2];      // A, B or C. 
};

class Wave
{
public:
    Wave();
    void Init(const std::vector<Pulse>& aPulseConfig, float aTimeToFirstPulse);
    void Update(float aDeltaTime, std::shared_ptr<EnemyPool> anEnemyPool);
    bool IsComplete() const;

    void SetBlackboard(std::shared_ptr<Blackboard> aBlackboard) { myBlackboard = aBlackboard; }

private:
    void TriggerPulse(std::shared_ptr<EnemyPool> anEnemyPool);
    void ResetHUDInfo();
    void UpdateHUDInfo(const Pulse& aPulse);

    std::vector<Pulse> myPulses;     // Pulses within the wave
    float myTimeBetweenPulses;       // Time between pulses
    CU::CountdownTimer myPulseTimer; // Timer to track the next pulse
    CU::CountupTimer myTotalTime;    // Total Wave Time
    size_t myCurrentPulseIndex;      // Index of the current pulse
    bool myIsComplete;               // Whether the wave has completed all pulses

    std::shared_ptr<Blackboard> myBlackboard;
};