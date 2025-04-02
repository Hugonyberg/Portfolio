

void Wave::Init(const std::vector<Pulse>& aPulseConfig, float aTimeToFirstPulse)
{
    myPulses = aPulseConfig;
    myTimeBetweenPulses = aTimeToFirstPulse;
    myPulseTimer.SetResetValue(aTimeToFirstPulse);
    myPulseTimer.SetCurrentValue(aTimeToFirstPulse);

    float totalTime = aTimeToFirstPulse;
    for (const auto& pulse : aPulseConfig)
    {
        totalTime += pulse.timeToNextPulse;
    }

    myTotalTime.SetThresholdValue(totalTime);
    myTotalTime.SetCurrentValue(0.0f);
    myCurrentPulseIndex = 0;
    myIsComplete = false;
}

void Wave::Update(float aDeltaTime, std::shared_ptr<EnemyPool> anEnemyPool)
{
    if (myIsComplete)
        return;

    myPulseTimer.Update(aDeltaTime);
    myTotalTime.Update(aDeltaTime);

    // Check if total time has passed and all enemies are destroyed
    if (myTotalTime.ReachedThreshold() && anEnemyPool->NoActiveEnemies() && myCurrentPulseIndex == myPulses.size())
    {
         myIsComplete = true;
        return;
    }

    // Check if it's time for the next pulse
    if (myPulseTimer.IsDone() && myCurrentPulseIndex < myPulses.size())
    {
        TriggerPulse(anEnemyPool);
        myPulseTimer.SetResetValue(myPulses[myCurrentPulseIndex].timeToNextPulse);
        myPulseTimer.Reset();
        ++myCurrentPulseIndex;
    }
}

bool Wave::IsComplete() const
{
    return myIsComplete;
}

void Wave::TriggerPulse(std::shared_ptr<EnemyPool> anEnemyPool)
{
    if (myCurrentPulseIndex >= myPulses.size())
        return;
    if (myCurrentPulseIndex == 0)
        ResetHUDInfo();

    const Pulse& currentPulse = myPulses[myCurrentPulseIndex];
    UpdateHUDInfo(currentPulse);

    // Spawn flying enemies
    for (size_t i = 0; i < currentPulse.numberOfShooterEnemies; ++i)
    {
        anEnemyPool->SpawnFlyingEnemy(i, currentPulse.spawnPointLabel[0], currentPulse.defensePointLabel[0], eEnemyType::FlyingShooting);
    }
    for (size_t i = 0; i < currentPulse.numberOfKamikazeEnemies; ++i)
    {
        anEnemyPool->SpawnFlyingEnemy(i, currentPulse.spawnPointLabel[0], currentPulse.defensePointLabel[0], eEnemyType::FlyingChaser);
    }

    // Spawn ground enemies
    for (size_t i = 0; i < currentPulse.numberOfGroundEnemies; ++i)
    {
        anEnemyPool->SpawnGroundEnemy(i, currentPulse.spawnPointLabel[0], currentPulse.defensePointLabel[0]);
    }
}
