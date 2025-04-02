
WaveManager::WaveManager()
	: myCurrentWaveIndex(0)
{
	myEnemyPool = nullptr;
}

void WaveManager::Init(std::shared_ptr<DE::Navmesh> aNavmesh, std::shared_ptr<DE::Navmesh> aDetailedNavmesh, std::string& aLevelName)
{
    myBlackboard = std::make_shared<Blackboard>();
    myBlackboard->SetNavmesh(aNavmesh);
    myBlackboard->SetDetailedNavmesh(aDetailedNavmesh);

    myEnemyPool = std::make_shared<EnemyPool>(500, myBlackboard);
    myBlackboard->SetEnemyPool(myEnemyPool);

    myBlackboard->SetDefensePoints(myDefensePoints);
    myBlackboard->SetSpawnPoints(mySpawnPoints);
    CalculateAndInitNavmeshPaths();
    myBlackboard->SetPathsMap(myPaths);


    myBlackboard->SetPlayer(MainSingleton::GetInstance()->GetPlayer());

    if (myPath.empty())
    {
        CalculateCirclePoints({ 0.0f,0.0f,0.0f }, 1000.0f, 150.f);
        myBlackboard->SetPath(myPath);
    }
    else
    {
        myBlackboard->SetPath(myPath);
    }

    std::string filePath = "../Assets/WaveConfigs/DefaultName_WC.bin"; // Relative path from .sln

    if (aLevelName == "LevelOne")
    {
        filePath = "../Assets/WaveConfigs/Level01_WC.bin";
    }
    else if (aLevelName == "LevelTwo")
    {
        filePath = "../Assets/WaveConfigs/Level02_WC.bin";
    }

    LoadFromFile(filePath);

    // Initialize the WaveManager with the pulseConfig, hard code 15.0f as is our preparation time
    for (size_t i = 0; i < 5; i++)
    {
        myTimeToFirstPulse[i] = 15.0f;
    }

    for (size_t i = 0; i < myWaves.size(); ++i)
    {
        myWaves[i] = std::make_unique<Wave>();
        myWaves[i]->Init(myWaveConfig[i], myTimeToFirstPulse[i]); // Initialize each wave with a pulse config and time between pulses
        myWaves[i]->SetBlackboard(myBlackboard);
    }

#ifndef _DEBUG
    myShouldStart = true;
#endif // _DEBUG
}

void WaveManager::Update(float aDeltaTime)
{
#ifndef _RETAIL
    HandleImGui();
    if (!myShouldStart) return;
#endif // NDEBUG

    if(myCurrentWaveIndex >= myWaves.size())
    {
        MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ nullptr, eMessageType::TriggerMissionAccomplished });

        return;
    }

    myBlackboard->HandlePlayerWithinPath(); 

    // Update the current wave
    myWaves[myCurrentWaveIndex]->Update(aDeltaTime, myEnemyPool);

    if (myWaves[myCurrentWaveIndex]->IsComplete())
    {
        ++myCurrentWaveIndex;
        if (myCurrentWaveIndex < myWaves.size())
        {
            int waveNum = myCurrentWaveIndex + 1;
            MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &waveNum, eMessageType::WaveNumberChanged });
            MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &waveNum, eMessageType::TriggerWaveIncoming });
        }
    }

    UpdateHUDInfo();
    myEnemyPool->Update(aDeltaTime);
}

bool WaveManager::AreAllWavesComplete() const
{
    return myCurrentWaveIndex >= myWaves.size();
}

void WaveManager::Render(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
    myEnemyPool->Render(aGraphicsEngine);
}

void WaveManager::RestartWave(const std::array<std::vector<Pulse>, 5>& aWaveConfig, int aPoolSize)
{
    if (aPoolSize == myEnemyPool->GetCurrentSize())
    {
        myEnemyPool->ResetEnemies();
    }
    else
    {
        myEnemyPool = nullptr;
        myEnemyPool = std::make_shared<EnemyPool>(aPoolSize, myBlackboard);
        myEnemyPool->InitParticleSystem();
        myParticleManager->Add3D(myEnemyPool->GetParticleSystem());
    }

    for (size_t i = 0; i < myWaves.size(); ++i)
    {
        myWaves[i] = std::make_unique<Wave>();
        myWaves[i]->Init(aWaveConfig[i], myTimeToFirstPulse[static_cast<int>(i)]); 
        myWaves[i]->SetBlackboard(myBlackboard);
    }


    myCurrentWaveIndex = 0;
}

void WaveManager::RestartCurrentWave()
{
    RestartWave(myWaveConfig, myPoolSize);
}

void WaveManager::HandleImGui()
{
    if (ImGui::Begin("Wave Manager"))
    {
        ImGui::SetWindowFontScale(1.3f);
        ImGui::Separator();
        if (ImGui::Button("Start Waves"))
        {
            myShouldStart = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Restart Waves"))
        {
            RestartWave(myWaveConfig, myPoolSize);
        }
        ImGui::SetWindowFontScale(1.0f);

        for (size_t waveIndex = 0; waveIndex < myWaveConfig.size(); ++waveIndex)
        {
            ImGui::SetWindowFontScale(1.5f);
            if (ImGui::CollapsingHeader(("Wave " + std::to_string(waveIndex + 1)).c_str()))
            {
                ImGui::SetWindowFontScale(1.0f);
                ImGui::Separator();
                std::string timeFP = "Preparation Time [" + std::to_string(waveIndex + 1) + "]";
                ImGui::SliderFloat(timeFP.c_str(), &myTimeToFirstPulse[static_cast<int>(waveIndex)], 1.f, 30.0f, "%1.f");
                ImGui::Indent();
                for (size_t pulseIndex = 0; pulseIndex < myWaveConfig[waveIndex].size(); ++pulseIndex)
                {
                    Pulse& pulse = myWaveConfig[waveIndex][pulseIndex];

                    ImGui::PushID(static_cast<int>(waveIndex * 100 + pulseIndex)); // Ensure unique IDs for ImGui controls

                    ImGui::SetWindowFontScale(1.2f);
                    ImGui::Text("Pulse %d", static_cast<int>(pulseIndex + 1));
                    ImGui::SetWindowFontScale(1.0f);
                    ImGui::SliderInt("Flying Shooter Enemies", reinterpret_cast<int*>(&pulse.numberOfShooterEnemies), 0, 100);
                    ImGui::SliderInt("Flying Kamikaze Enemies", reinterpret_cast<int*>(&pulse.numberOfKamikazeEnemies), 0, 100);
                    ImGui::SliderInt("Ground Enemies", reinterpret_cast<int*>(&pulse.numberOfGroundEnemies), 0, 100);
                    if (pulseIndex < (myWaveConfig[waveIndex].size() - 1))
                    {
                        ImGui::SliderFloat("Time To Next Pulse", &pulse.timeToNextPulse, 0.f, 30.0f, "%1.f");
                    }

                    const char* pointLabels[] = { "A", "B", "C" };

                    int spawnPointIndex = 0;
                    int defensePointIndex = 0;

                    // Convert current string label to index
                    for (int i = 0; i < 3; i++)
                    {
                        if (pulse.spawnPointLabel[0] == pointLabels[i][0])
                            spawnPointIndex = i;
                        if (pulse.defensePointLabel[0] == pointLabels[i][0]) 
                            defensePointIndex = i;
                    }

                    ImGui::Text("Spawn Points:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(50);
                    if (ImGui::Combo("##Spawn", &spawnPointIndex, pointLabels, IM_ARRAYSIZE(pointLabels)))
                    {
                        pulse.spawnPointLabel[0] = pointLabels[spawnPointIndex][0];
                        pulse.spawnPointLabel[1] = '\0'; // Ensure null-termination
                    }

                    ImGui::SameLine();
                    ImGui::Text("Defense Point Target:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(50);
                    if (ImGui::Combo("##Defense", &defensePointIndex, pointLabels, IM_ARRAYSIZE(pointLabels)))
                    {
                        pulse.defensePointLabel[0] = pointLabels[defensePointIndex][0];
                        pulse.defensePointLabel[1] = '\0'; // Ensure null-termination
                    }

                    if (ImGui::Button("Remove Pulse"))
                    {
                        myWaveConfig[waveIndex].erase(myWaveConfig[waveIndex].begin() + pulseIndex);
                        ImGui::PopID();
                        break; // Avoid invalid iterator issues
                    }

                    ImGui::PopID();
                }

                if (ImGui::Button(("Add Pulse##" + std::to_string(waveIndex)).c_str()))
                {
                    std::cout << "Adding Pulse to Wave: " << waveIndex << std::endl;
                    myWaveConfig[waveIndex].push_back({ 0, 0, 0, 1.0f, "A", "A"});
                }
                ImGui::Unindent();
                ImGui::Separator();
            }
        }

        ImGui::Spacing();

        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("Global Settings");
        ImGui::SliderInt("Enemy Pool Size", &myPoolSize, 1, 500);

        
        ImGui::Separator();

        ImGui::SetWindowFontScale(1.3f);
        ImGui::InputText("Filename", myFileNameInput, IM_ARRAYSIZE(myFileNameInput));
        std::string filePath = "../Assets/WaveConfigs/" + std::string(myFileNameInput) + "_WC.bin"; // Relative path from .sln

        if (ImGui::Button("Save Current Settings to File"))
        {
            SaveToFile(filePath);
        }
        ImGui::SameLine();
        if (ImGui::Button("Load Settings from File"))
        {
            LoadFromFile(filePath);
        }

        // File Browser UI
        ImGui::Text("Saved Configurations:");
        std::filesystem::path saveDir = "../Assets/WaveConfigs"; // Path to bin files

        for (const auto& entry : std::filesystem::directory_iterator(saveDir))
        {
            if (entry.path().extension() == ".bin")
            {
                if (ImGui::Button(entry.path().filename().string().c_str()))
                {
                    LoadFromFile(entry.path().string());
                }
            }
        }
        ImGui::SetWindowFontScale(1.0f);
    }

    ImGui::End();
}

void WaveManager::SetPathfindingPoints(std::vector<DE::Vector3f>& aPath)
{
    myPath = aPath; 
    RestartCurrentWave(); 
    myBlackboard->SetPath(aPath);
}

void WaveManager::DebugRender(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
    if (myEnemyPool)
    {
        myEnemyPool->DebugRender(aGraphicsEngine);
    }
}

void WaveManager::SetParticleManagerPointer(ParticleManager* aManager)
{
    myParticleManager = aManager;
}

void WaveManager::UpdateHUDInfo()
{
    int amount = (int)myEnemyPool->GetActiveFlyingEnemies().size() + (int)myEnemyPool->GetActiveGroundEnemies().size();
    MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &amount, eMessageType::EnemyNumberChanged });
}

void WaveManager::CalculateAndInitNavmeshPaths()
{
    for (auto dp : myDefensePoints)
    {
        for (auto sp : mySpawnPoints)
        {
            std::vector<DE::Vector3f> path = DE::Pathfinding::FindShortestPath(
                sp->GetTransform()->GetPosition(),
                dp->GetTransform()->GetPosition(),
                myBlackboard->GetNavmesh(),
                false //Dont use funneling
            );

            std::string pathID;
            pathID.push_back(sp->GetCharID());
            pathID.push_back(dp->GetCharID());

            myPaths[pathID] = path;
        }
    }
}

void WaveManager::CalculateCirclePoints(const DE::Vector3f& origin, float radius, float spacing)
{
    myPath.clear();

    // Calculate the circumference
    float circumference = 2.0f * M_PI * radius;
    // Determine the number of points based on spacing
    size_t pointCount = static_cast<size_t>(circumference / spacing);
    // Calculate the angle step between each point
    float angleStep = 2.0f * M_PI / static_cast<float>(pointCount);

    for (size_t i = 0; i < pointCount; ++i)
    {
        float angle = angleStep * static_cast<float>(i);
        float x = origin.x + radius * std::cos(angle);
        float z = origin.z + radius * std::sin(angle);
        myPath.emplace_back(x, origin.y, z); // Keep y the same as the origin
    }
}

void WaveManager::SaveToFile(const std::string& aFilename)
{
    if (std::filesystem::exists(aFilename))
    {
        std::filesystem::permissions(aFilename, std::filesystem::perms::all);
    }

    std::filesystem::path directory = "../Assets/WaveConfigs";
    if (!std::filesystem::exists(directory))
    {
        std::filesystem::create_directories(directory); // Ensure folder exists
    }

    std::ofstream outFile(aFilename, std::ios::binary | std::ios::trunc);
    if (!outFile)
    {
        std::cerr << "Failed to open file for writing: " << aFilename << std::endl;
        return;
    }

    constexpr size_t MAX_WAVES = 5;
    outFile.write(reinterpret_cast<const char*>(&MAX_WAVES), sizeof(size_t));

    for (size_t waveIndex = 0; waveIndex < MAX_WAVES; ++waveIndex)
    {
        const auto& wavePulses = myWaveConfig[waveIndex];

        size_t numPulses = wavePulses.size();
        outFile.write(reinterpret_cast<const char*>(&numPulses), sizeof(size_t));

        for (const Pulse& pulse : wavePulses)
        {
            outFile.write(reinterpret_cast<const char*>(&pulse.numberOfShooterEnemies), sizeof(size_t));
            outFile.write(reinterpret_cast<const char*>(&pulse.numberOfKamikazeEnemies), sizeof(size_t));
            outFile.write(reinterpret_cast<const char*>(&pulse.numberOfGroundEnemies), sizeof(size_t));
            outFile.write(reinterpret_cast<const char*>(&pulse.timeToNextPulse), sizeof(float));

            outFile.write(pulse.spawnPointLabel, 2);
            outFile.write(pulse.defensePointLabel, 2);
        }
    }

    outFile.close();
    std::cout << "Wave configuration saved to " << aFilename << std::endl;
}

void WaveManager::LoadFromFile(const std::string& aFilename)
{
    std::ifstream inFile(aFilename, std::ios::binary);
    if (!inFile)
    {
        std::cerr << "Failed to open file for reading: " << aFilename << std::endl;
        return;
    }

    size_t numWaves;
    inFile.read(reinterpret_cast<char*>(&numWaves), sizeof(size_t));

    if (numWaves > 5) // Ensure we don't exceed the fixed size
    {
        std::cerr << "Error: Wave data exceeds maximum wave count!" << std::endl;
        inFile.close();
        return;
    }

    for (size_t waveIndex = 0; waveIndex < numWaves; ++waveIndex)
    {
        size_t numPulses;
        inFile.read(reinterpret_cast<char*>(&numPulses), sizeof(size_t));

        myWaveConfig[waveIndex].resize(numPulses);

        for (size_t pulseIndex = 0; pulseIndex < numPulses; ++pulseIndex)
        {
            Pulse& pulse = myWaveConfig[waveIndex][pulseIndex];

            inFile.read(reinterpret_cast<char*>(&pulse.numberOfShooterEnemies), sizeof(size_t));
            inFile.read(reinterpret_cast<char*>(&pulse.numberOfKamikazeEnemies), sizeof(size_t));
            inFile.read(reinterpret_cast<char*>(&pulse.numberOfGroundEnemies), sizeof(size_t));
            inFile.read(reinterpret_cast<char*>(&pulse.timeToNextPulse), sizeof(float));

            // Read and ensure null termination
            inFile.read(pulse.spawnPointLabel, 2);
            inFile.read(pulse.defensePointLabel, 2);
            pulse.spawnPointLabel[1] = '\0';
            pulse.defensePointLabel[1] = '\0';
        }
    }

    std::cout << "Wave configuration loaded from " << aFilename << std::endl;
    for (size_t waveIndex = 0; waveIndex < myWaveConfig.size(); ++waveIndex)
    {
        for (size_t pulseIndex = 0; pulseIndex < myWaveConfig[waveIndex].size(); ++pulseIndex)
        {
            Pulse& pulse = myWaveConfig[waveIndex][pulseIndex];
            std::cout << "Wave " << waveIndex + 1 << ", Pulse " << pulseIndex + 1
                << " | Spawn: " << pulse.spawnPointLabel
                << " | Defense: " << pulse.defensePointLabel << std::endl;
        }
    }

    // Find the last occurrence of the path separator
    size_t lastSlash = aFilename.find_last_of("/\\");
    size_t lastDot = aFilename.find_last_of('_');

    // Extract the filename without extension
    std::string filename = aFilename.substr(lastSlash + 1, lastDot - lastSlash - 1);

    strncpy_s(myFileNameInput, filename.c_str(), sizeof(myFileNameInput) - 1);
    myFileNameInput[sizeof(myFileNameInput) - 1] = '\0';

    inFile.close();
    std::cout << "Wave configuration loaded from " << aFilename << std::endl;
}

