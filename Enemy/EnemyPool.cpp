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

struct SpawnConfig
{
    int spawnThreshold;
    float spacing;
    float backSpacing;
    int heightThreshold;
    float verticalSpacing;
    float verticalOffset;
    float baseBackOffset;
};

void EnemyPool::AddDefaultSteeringRays()
{
    SeparationRaycastInfo newRay;
    newRay.weight = 1.0f;
    newRay.separationRayLength = 300.f;
    newRay.isColliding = false;
    newRay.collidingDistance = 0.f;

    newRay.label = rayPresets[0].label;
    newRay.yawOffset = rayPresets[0].yawOffset;
    newRay.pitchOffset = rayPresets[0].pitchOffset;
    myFlyingSteering.AddSeparationRay(newRay);

    newRay.label = rayPresets[1].label;
    newRay.yawOffset = rayPresets[1].yawOffset;
    newRay.pitchOffset = rayPresets[1].pitchOffset;
    myFlyingSteering.AddSeparationRay(newRay);

    newRay.label = rayPresets[2].label;
    newRay.yawOffset = rayPresets[2].yawOffset;
    newRay.pitchOffset = rayPresets[2].pitchOffset;
    myFlyingSteering.AddSeparationRay(newRay);

    newRay.label = rayPresets[3].label;
    newRay.yawOffset = rayPresets[3].yawOffset;
    newRay.pitchOffset = rayPresets[3].pitchOffset;
    myFlyingSteering.AddSeparationRay(newRay);

    newRay.label = rayPresets[4].label;
    newRay.yawOffset = rayPresets[4].yawOffset;
    newRay.pitchOffset = rayPresets[4].pitchOffset;
    myFlyingSteering.AddSeparationRay(newRay);

    newRay.label = rayPresets[6].label;
    newRay.yawOffset = rayPresets[6].yawOffset;
    newRay.pitchOffset = rayPresets[6].pitchOffset;
    myFlyingSteering.AddSeparationRay(newRay);

    newRay.label = rayPresets[8].label;
    newRay.yawOffset = rayPresets[8].yawOffset;
    newRay.pitchOffset = rayPresets[8].pitchOffset;
    myFlyingSteering.AddSeparationRay(newRay);

    newRay.label = rayPresets[9].label;
    newRay.yawOffset = rayPresets[9].yawOffset;
    newRay.pitchOffset = rayPresets[9].pitchOffset;
    myFlyingSteering.AddSeparationRay(newRay);
}

void EnemyPool::AddDefaultGroundSteeringRays()
{
    SeparationRaycastInfo newRay;
    newRay.weight = 1.0f;
    newRay.separationRayLength = 300.f;
    newRay.isColliding = false;
    newRay.collidingDistance = 0.f;

    newRay.label = rayPresets[1].label;
    newRay.yawOffset = rayPresets[1].yawOffset;
    newRay.pitchOffset = rayPresets[1].pitchOffset;
    myGroundSteering.AddSeparationRay(newRay);

    newRay.label = rayPresets[2].label;
    newRay.yawOffset = rayPresets[2].yawOffset;
    newRay.pitchOffset = rayPresets[2].pitchOffset;
    myGroundSteering.AddSeparationRay(newRay);
}

std::shared_ptr<Enemy> EnemyPool::SpawnEnemy(int indexOffset, char aSpawnId, char aDefensePointId, eEnemyType aType)
{
    size_t index = FindNextAvailable(myEnemies, aType);
    if (index == std::numeric_limits<size_t>::max())
        return nullptr;

    SpawnConfig config;
    
    config.spawnThreshold = aType.spawnConfig.spawnThreshold;
    config.spacing = aType.spawnConfig.spacing;
    config.backSpacing = aType.spawnConfig.backSpacing;
    config.heightThreshold = aType.spawnConfig.heightThreshold;
    config.verticalSpacing = aType.spawnConfig.verticalSpacing;
    config.verticalOffset = aType.spawnConfig.verticalOffset;

    int col = indexOffset % config.spawnThreshold;
    int vertical = (indexOffset / config.spawnThreshold) % config.heightThreshold;
    int row = indexOffset / (config.spawnThreshold * config.heightThreshold);

    auto* transform = myBlackboard->GetSpawnPointByID(aSpawnId)->GetTransform();
    const DE::Vector3f basePos = transform->GetPosition();
    const DE::Vector3f left = -transform->GetMatrix().GetRight();
    const DE::Vector3f back = -transform->GetMatrix().GetForward();
    const DE::Vector3f up = { 0.f, 1.f, 0.f };

    const DE::Vector3f offset =
        left * (config.spacing * col) +
        back * (config.backSpacing * row + config.baseBackOffset) +
        up * (config.verticalSpacing * vertical + config.verticalOffset);

    std::string mapID{ aSpawnId, aDefensePointId };

    auto& pathsMap = myBlackboard->GetPathsMap();
    if (auto it = pathsMap.find(mapID); it != pathsMap.end())
    {
        myEnemies[index]->SetPathfindingPoints(it->second);
    }
    else
    {
        myEnemies[index]->SetPathfindingPoints({});
    }

    myEnemies[index]->SetDefensePointTargetID(aDefensePointId);
    myEnemies[index]->Spawn(offset);
    myNextAvailable = (index + 1) % myEnemies.size();

    return myEnemies[index];
}

void EnemyPool::Update(float aDeltaTime)
{
#ifndef _RETAIL
    HandleImgui();
#endif // _RETAIL

    for (auto& enemy : myFlyingEnemies)
    {
        if (enemy->IsActive())
        {
            enemy->Update(aDeltaTime);
        }
    }

    for (auto& enemy : myGroundEnemies)
    {
        if (enemy->IsActive())
        {
            enemy->Update(aDeltaTime);
        }
    }
}

void EnemyPool::ResetEnemies()
{
    for (auto& enemy : myFlyingEnemies)
    {
        enemy->RemovePhysXBody();
        enemy->SetIsActive(false);
    }

    for (auto& enemy : myGroundEnemies)
    {
        enemy->RemovePhysXBody();
        enemy->SetIsActive(false);
    }
}

void EnemyPool::DebugRender(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
    if (myDebugDrawGround)
    {
        for (auto& enemy : myGroundEnemies)
        {
            if (enemy->IsActive())
            {
                enemy->DebugRender(aGraphicsEngine);
            }
        }
    }

    if (myDebugDrawFlying)
    {
        for (auto& enemy : myFlyingEnemies)
        {
            if (enemy->IsActive())
            {
                enemy->DebugRender(aGraphicsEngine);
            }
        }
    }
}

size_t EnemyPool::FindNextAvailable(const std::vector<std::shared_ptr<FlyingEnemy>>& someFlyingEnemies, eEnemyType aType)
{
    for (size_t i = 0; i < someFlyingEnemies.size(); ++i)
    {
        if (!someFlyingEnemies[i]->IsActive() && someFlyingEnemies[i]->GetEnemyType() == aType)
        {
            return i;
        }
    }

    return std::numeric_limits<size_t>::max(); // Return invalid index if no available enemy
}

void EnemyPool::HandleImgui()
{
    if (!ImGui::Begin("Enemy Settings"))
        return;

    auto Slider = [](const char* label, float* value, float min, float max, const char* fmt = "%.1f") 
        {
        ImGui::SliderFloat(label, value, min, max, fmt);
        };

    auto ApplyFlyingSteeringToEnemy = [&](Enemy* enemy) 
        {
        enemy->SetMass(myFlyingMass);
        enemy->SetMaxForce(myFlyingMaxForce);
        myFlyingSteering.SetSeparationRadius(myFlyingSeparationRadius);
        myFlyingSteering.SetSeekWeight(myFlyingSeekWeight);
        myFlyingSteering.SetEnemySeparationWeight(myFlyingEnemySeparationWeight);
        myFlyingSteering.SetObjectSeparationWeight(myFlyingObjectSeparationWeight);
        myFlyingSteering.SetSmoothingFactor(mySteeringSmoothingFactor);
        myFlyingSteering.SetMaxTurnRate(myMaxTurnRate);

        switch (enemy->GetEnemyType())
        {
        case eEnemyType::FlyingChaser:
            myFlyingSteering.SetArrivalRadius(myFlyingChaserArrivalRadius);
            enemy->SetSpeed(myFlyingKamikazeSpeed);
            break;
        case eEnemyType::FlyingShooting:
            myFlyingSteering.SetArrivalRadius(myFlyingShootingArrivalRadius);
            enemy->SetSpeed(myFlyingSpeed);
            break;
        }
        enemy->HandleSteeringParameters(myFlyingSteering);
        };

    auto ApplyGroundSteeringToEnemy = [&](Enemy* enemy) 
        {
        enemy->SetSpeed(myGroundSpeed);
        enemy->SetMass(myGroundMass);
        enemy->SetMaxForce(myGroundMaxForce);
        myGroundSteering.SetSeparationRadius(myGroundSeparationRadius);
        myGroundSteering.SetArrivalRadius(myGroundArrivalRadius);
        myGroundSteering.SetSeekWeight(myGroundSeekWeight);
        myGroundSteering.SetEnemySeparationWeight(myGroundEnemySeparationWeight);
        myGroundSteering.SetObjectSeparationWeight(myGroundObjectSeparationWeight);
        myGroundSteering.SetSmoothingFactor(myGroundSteeringSmoothingFactor);
        myGroundSteering.SetMaxTurnRate(myGroundMaxTurnRate);
        enemy->SetSteering(myGroundSteering);
        };

    ImGui::Indent();
    ImGui::Text("Flying Enemies Settings");
    ImGui::Indent();
    Slider("Flying Speed", &myFlyingSpeed, 1.0f, 200.0f, "%1.f");
    Slider("Flying Kamikaze Speed", &myFlyingKamikazeSpeed, 1.0f, 200.0f, "%1.f");
    Slider("Flying Mass", &myFlyingMass, 1.0f, 200.0f, "%1.f");
    Slider("Flying Max Force", &myFlyingMaxForce, 1.0f, 200.0f, "%1.f");
    Slider("Flying Max Turn Rate", &myMaxTurnRate, 0.1f, 10.0f, "%0.1f");
    Slider("Flying Smoothing Factor", &mySteeringSmoothingFactor, 0.1f, 1.0f, "%0.05f");
    ImGui::Unindent();

    ImGui::Separator();
    ImGui::Text("Flying Separation Settings");
    Slider("Flying Separation Radius", &myFlyingSeparationRadius, 1.0f, 500.0f);
    Slider("Flying Shooting Arrival Radius", &myFlyingShootingArrivalRadius, 1.0f, 500.0f);
    Slider("Flying Chaser Arrival Radius", &myFlyingChaserArrivalRadius, 1.0f, 5000.0f);
    Slider("Flying Enemy Separation Weight", &myFlyingEnemySeparationWeight, 1.0f, 100.0f);
    Slider("Flying Object Separation Weight", &myFlyingObjectSeparationWeight, 1.0f, 100.0f);
    Slider("Flying Seek Weight", &myFlyingSeekWeight, 1.0f, 100.0f);

    ImGui::Separator();
    ImGui::Text("Flying Separation Ray Settings");
    ImGui::Checkbox("View Flying Debug Lines", &myDebugDrawFlying);
    if (ImGui::Button("Set Changed Flying Parameters"))
        myFlyingParamsChanged = true;

    ImGui::Text("Preset Separation Rays");
    for (int i = 0; i < 18; i++)
    {
        bool alreadyAdded = false;
        for (const auto& ray : myFlyingSteering.GetSeparationRays())
        {
            if (strcmp(ray.label, rayPresets[i].label) == 0)
            {
                alreadyAdded = true;
                break;
            }
        }

        if (!alreadyAdded)
        {
            if (ImGui::Button(rayPresets[i].label))
            {
                SeparationRaycastInfo newRay{
                    rayPresets[i].label,
                    rayPresets[i].yawOffset,
                    rayPresets[i].pitchOffset,
                    1.0f, 200.f, false, 0.f
                };
                myFlyingSteering.AddSeparationRay(newRay);
            }
        }
        else
        {
            ImGui::Text("%s (added)", rayPresets[i].label);
        }

        if ((i + 1) % 6 != 0)
            ImGui::SameLine();
    }

    if (ImGui::Button("Add Custom Separation Ray"))
    {
        SeparationRaycastInfo newRay{
            "Custom", 0.f, 0.f, 1.0f, 150.f, false, 0.f
        };
        myFlyingSteering.AddSeparationRay(newRay);
    }

    auto& rays = myFlyingSteering.GetSeparationRays();
    for (size_t i = 0; i < rays.size(); i++)
    {
        SeparationRaycastInfo& ray = rays[i];
        ImGui::PushID(static_cast<int>(i));
        ImGui::Text("%s Ray", ray.label);
        if (std::string(ray.label) == "Custom")
        {
            Slider("Yaw Offset", &ray.yawOffset, -180.f, 180.f);
            Slider("Pitch Offset", &ray.pitchOffset, -90.f, 90.f);
        }
        Slider("Weight", &ray.weight, 0.1f, 10.f);
        Slider("Length", &ray.separationRayLength, 50.f, 500.f);
        if (ImGui::Button("Remove"))
        {
            rays.erase(rays.begin() + i);
            ImGui::PopID();
            break;
        }
        ImGui::PopID();
    }

    ImGui::Separator();
    ImGui::Text("Ground Enemies Settings");
    ImGui::Checkbox("View Ground Debug Lines", &myDebugDrawGround);
    if (ImGui::Button("Set Ground Flying Parameters"))
        myGroundParamsChanged = true;

    Slider("Ground Speed", &myGroundSpeed, 1.0f, 200.0f);
    Slider("Ground Mass", &myGroundMass, 1.0f, 200.0f);
    Slider("Ground Max Force", &myGroundMaxForce, 1.0f, 200.0f);
    Slider("Ground Max Turn Rate", &myGroundMaxTurnRate, 0.1f, 10.0f);
    Slider("Ground Smoothing Factor", &myGroundSteeringSmoothingFactor, 0.1f, 1.0f, "%0.05f");

    ImGui::Text("Ground Separation Settings");
    Slider("Ground Separation Radius", &myGroundSeparationRadius, 1.0f, 500.0f);
    Slider("Ground Arrival Radius", &myGroundArrivalRadius, 1.0f, 500.0f);
    Slider("Ground Enemy Separation Weight", &myGroundEnemySeparationWeight, 1.0f, 100.0f);
    Slider("Ground Object Separation Weight", &myGroundObjectSeparationWeight, 1.0f, 100.0f);
    Slider("Ground Seek Weight", &myGroundSeekWeight, 1.0f, 100.0f);

    if (myFlyingParamsChanged)
    {
        for (auto& enemy : myFlyingEnemies)
            ApplyFlyingSteeringToEnemy(enemy.get());
    }

    if (myGroundParamsChanged)
    {
        for (auto& enemy : myGroundEnemies)
            ApplyGroundSteeringToEnemy(enemy.get());
    }

    ImGui::Unindent();
    ImGui::End();

    myFlyingParamsChanged = false;
    myGroundParamsChanged = false;
}
