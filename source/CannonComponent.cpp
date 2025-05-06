#include "stdafx.h"
#include "CannonComponent.h"

#include "DreamEngine\graphics\ModelFactory.h"
#include "DreamEngine\graphics\ModelInstance.h"
#include <DreamEngine\graphics\ModelDrawer.h>

#include "Player.h"
#include "MainSingleton.h"

#include <DreamEngine\utilities\InputManager.h>

void CannonComponent::TurnTowards(DE::Vector3f aPos, DE::Transform& aTransformToTurn)
{
    DE::Vector3f diff = aPos - aTransformToTurn.GetPosition();
    float horizontalDistance = sqrt(diff.x * diff.x + diff.z * diff.z);
    float yaw = atan2(diff.x, diff.z);         // Rotation around Y-axis
    float pitch = -atan2(diff.y, horizontalDistance);  // Rotation around X-axis

    constexpr float PI = 3.14159265358979323846f;
    yaw = yaw * (180.0f / PI);
    pitch = pitch * (180.0f / PI);

    aTransformToTurn.SetRotation({ pitch, yaw, 0.0f });
}

void CannonComponent::Init()
{
    myCannonBallModel = std::make_shared<DreamEngine::ModelInstance>(DE::ModelFactory::GetInstance().GetModelInstance(L"3D/spherePrimitive.fbx"));
    myCannonBallModel->SetTransform(myModelInstance->GetTransform());
    myCannonBallModel->SetScale(15.0f);

    myHarpoonModel = std::make_shared<DreamEngine::ModelInstance>(DE::ModelFactory::GetInstance().GetModelInstance(L"3D/cubePrimitive.fbx"));
    myHarpoonModel->SetTransform(myModelInstance->GetTransform());
    myHarpoonModel->SetScale(25.0f);

    myShootTimer = CU::CountdownTimer(0.3f);
    myCurrentAmmo = AmmoType::Cannonball;

    // Setting Cannon rotation to 120 degrees
    myMaxRotation = myTransform.GetRotation() + DE::Vector3f{ 0.0f, 60.0f, 0.0f };
    myMinRotation = myTransform.GetRotation() - DE::Vector3f{ 0.0f, 60.0f, 0.0f };

    myCurrentAmmoCount = myMaxAmmo;
    for (size_t i = 0; i < myMaxAmmo; i++)
    {
        Cannonball ball;
        ball.isActive = false;
        ball.transform = DE::Transform();
        myCannonballs.push_back(ball);
    }
}

void CannonComponent::Update(float aDeltaTime)
{
    /*if (myHarpoonActive)
    {
        myShootTimer.Update(aDeltaTime);
        float t = myShootTimer.GetCurrentValue();
        float flightTime = 3.0f;
        float half = 1.5f;
        DE::Vector3f harpoonPos = myHarpoonModel->GetTransform().GetPosition();
        DE::Vector3f gunPos = myTransform.GetPosition()
            + myTransform.GetMatrix().GetForward().GetNormalized() * -150.0f;

        if (t >= half)
        {
            if (!myHarpoonInitialized)
            {
                myHarpoonStartPos = harpoonPos;
                myHarpoonStartPos = true;
            }

            float elapsed = flightTime - t;
            float alpha = elapsed / half;
            alpha = 1.0f - (1.0f - alpha) * (1.0f - alpha);

            DE::Vector3f forward = myHarpoonModel->GetTransform().GetMatrix().GetForward().GetNormalized();
            DE::Vector3f endPos = myHarpoonStartPos + forward * myHarpoonSpeed * half;
            myHarpoonModel->SetLocation(myHarpoonStartPos + (endPos - myHarpoonStartPos) * alpha);
        }
        else
        {
            DE::Vector3f toGun = (gunPos - harpoonPos).GetNormalized();
            float dist = (gunPos - harpoonPos).Length();
            float move = myHarpoonSpeed * aDeltaTime;
            if (move >= dist)
            {
                myHarpoonModel->SetLocation(gunPos);
                myHarpoonActive = false;
            }
            else
            {
                myHarpoonModel->SetLocation(harpoonPos + toGun * move);
            }
        }

        if (myShootTimer.IsDone())
        {
            myHarpoonActive = false;
            myHarpoonInitialized = false;
        }

        return;
    }*/

    if (myHarpoonActive)
    {
        if (myShootTimer.GetCurrentValue() >= 1.5f)
        {
            // outgoing
            DE::Vector3f forward = myHarpoonModel->GetTransform().GetMatrix().GetForward().GetNormalized();
            myHarpoonModel->SetLocation(myHarpoonModel->GetTransform().GetPosition() + forward * myHarpoonSpeed * aDeltaTime); 
        }
        else
        {
            // returning
            DE::Vector3f harpoonPos = myHarpoonModel->GetTransform().GetPosition();
            DE::Vector3f gunPos = myTransform.GetPosition() + myTransform.GetMatrix().GetForward() * -1.0f * 150.0f;
            DE::Vector3f toGun = (gunPos - harpoonPos).GetNormalized();

            myHarpoonModel->SetLocation(harpoonPos + toGun * myHarpoonSpeed * aDeltaTime);
        }

        myShootTimer.Update(aDeltaTime);

        if (myShootTimer.IsDone())
        {
            myHarpoonActive = false;
        }
        return; // Ignore rotation and cannonballs when harpoon is shot
    }

    if (myShouldRotate)
    {
        DE::Vector3f rotation = myLocalTransform.GetRotation();
        
        if (myTurnDirection < 0)
        {
            rotation.y -= 50.0f * aDeltaTime;
        }
        else 
        {
            rotation.y += 50.0f * aDeltaTime;
        }

        if (myMinRotation.y < rotation.y && myMaxRotation.y > rotation.y)
        {
            myLocalTransform.SetRotation({ rotation.x, rotation.y, rotation.z});
        }
    }

    for (auto& ball : myCannonballs)
    {
        if (ball.isActive)
        {
            ball.transform.SetPosition(ball.transform.GetPosition() + ball.transform.GetMatrix().GetForward().GetNormalized() * myCannonBallSpeed * aDeltaTime);
            ball.lifeTimer.Update(aDeltaTime);

            if (ball.lifeTimer.IsDone())
            {
                ball.lifeTimer.Reset();
                ball.isActive = false;
            }
        }
    }

    if (!myShootTimer.IsDone())
    {
        myShootTimer.Update(aDeltaTime);
    }
}

void CannonComponent::OnInteractSouthButton(Player& aPlayer)
{
    auto& input = MainSingleton::GetInstance()->GetInputManager();

    if (!aPlayer.GetPlayerLock()) // Upon first Interact
    {
        aPlayer.TogglePlayerLock();
        aPlayer.SetLockPosition(GetInteractionPoint(), myTransform.GetMatrix().GetForward());
        return; // Skip shooting on initial hit
    }

    if (myShootTimer.IsDone())
    {
        if (input.IsKeyDown(DE::eKeyCode::E))
        {
            ShootCurrentAmmo(aPlayer);
        }
    }
}

void CannonComponent::OnInteractEastButton(Player& aPlayer)
{
    aPlayer.TogglePlayerLock();
}

void CannonComponent::OnInteractNorthButton(Player& aPlayer)
{
    ToggleAimMode(aPlayer); // e.g. hold to rotate turret
}

void CannonComponent::TriggerContact(Player& aPlayer)
{
    if (aPlayer.IsCarryingObject() && myCurrentAmmoCount < myMaxAmmo)
    {
        auto object = aPlayer.GetInventory();

        if (object == eObjectType::Ammo)
        {
            aPlayer.RemoveObject();
            myCurrentAmmoCount++;
        }
    }
}

void CannonComponent::HandlePlayerInput(Player& aPlayer)
{
    auto& input = MainSingleton::GetInstance()->GetInputManager();

    myShouldRotate = false;
    if (input.IsKeyDown(DE::eKeyCode::A) || input.IsKeyHeld(DE::eKeyCode::A))
    {
        myTurnDirection = -1;
        myShouldRotate = true;
    }
    
    if (input.IsKeyDown(DE::eKeyCode::D) || input.IsKeyHeld(DE::eKeyCode::D))
    {
        myTurnDirection = 1;
        myShouldRotate = true;
    }
}

void CannonComponent::Render(DE::GraphicsEngine& aGraphicsEngine)
{
    aGraphicsEngine.GetModelDrawer().DrawGBCalc(*myModelInstance.get());

    for (auto& ball : myCannonballs)
    {
        if (ball.isActive)
        {
            myCannonBallModel->SetTransform(ball.transform);
            aGraphicsEngine.GetModelDrawer().DrawGBCalc(*myCannonBallModel.get());
        }
    }

    if (myHarpoonActive)
    {
        aGraphicsEngine.GetModelDrawer().DrawGBCalc(*myHarpoonModel.get());
    }
}

void CannonComponent::ShootCurrentAmmo(Player&)
{
    myProjectileDirection = myTransform.GetMatrix().GetForward()* -1.0f; // TODO: Remove negative when forward is correct
    DE::Vector3f shootPos = myTransform.GetPosition() + myProjectileDirection * 150.0f;
    
    switch (myCurrentAmmo)
    {
    case AmmoType::Cannonball:
        {
            if (myCurrentAmmoCount <= 0) break;

            for (size_t i = 0; i < myMaxAmmo; i++)
            {
                if (!myCannonballs[i].isActive)
                {
                    myCannonballs[i].isActive = true;
                    myCannonballs[i].transform = myTransform;
                    myCannonballs[i].transform.SetScale(25.0f);
                    myCannonballs[i].lifeTimer.Reset();
                    TurnTowards(shootPos, myCannonballs[i].transform);
                    myCurrentAmmoCount--;
                    break;
                }
            }
            break;
        }
    case AmmoType::Harpoon:
        {
            DE::Transform cuzXFormIsConst = myTransform;
            TurnTowards(shootPos, cuzXFormIsConst);
            myHarpoonModel->SetTransform(cuzXFormIsConst);
            myHarpoonModel->SetScale(25.0f);
            myHarpoonActive = true;
            break;
        }
    }

    myShootTimer.Reset();
}

void CannonComponent::ToggleAimMode(Player&)
{
    auto& input = MainSingleton::GetInstance()->GetInputManager();

    if (input.IsKeyDown(DE::eKeyCode::F)) // Toggle between Ammo Types
    {
        if (myCurrentAmmo == AmmoType::Cannonball)
        {
            myCurrentAmmo = AmmoType::Harpoon;
            myShootTimer.SetResetValue(3.0f);
        }
        else
        {
            myCurrentAmmo = AmmoType::Cannonball;
            myShootTimer.SetResetValue(0.3f);
        }
    }
}
