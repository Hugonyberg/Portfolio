#include "stdafx.h"
#include "Boat.h"

#include "MainSingleton.h"
#include <DreamEngine\utilities\InputManager.h>

#include <DreamEngine\graphics\ModelDrawer.h>

#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine\debugging\LineDrawer.h>
#include <DreamEngine\debugging\LinePrimitive.h>

#include "CannonComponent.h"
#include "LightComponent.h"
#include "RepairComponent.h"
#include "SteerComponent.h"
#include "AmmoComponent.h"
#include "SpeedComponent.h"
#include "MapComponent.h"
#include "WeakPointComponent.h"

#include "Player.h"

#include <cmath>

void Boat::Init()
{
    // TODO : Set IMGUI Parameters
    myFarCameraOffset.x = 0.0f;
    myFarCameraOffset.y = 2500.0f;
    myFarCameraOffset.z = 2750.0f;

    myNearCameraOffset.x = 0.0f;
    myNearCameraOffset.y += 1250.0f;
    myNearCameraOffset.z += 1250.0f;
}

float Lerp(const float& a, const float& b, float t)
{
    return a + t * (b - a);
}

DE::Vector3f Lerp(const DE::Vector3f& a, const DE::Vector3f& b, float t)
{
    return {
        a.x + t * (b.x - a.x),
        a.y + t * (b.y - a.y),
        a.z + t * (b.z - a.z)
    };
}

void Boat::Update(const float aDeltaTime)
{
    constexpr float TURN_SPEED = 180.f * (3.14159265f / 180.f); // rad/sec

    DE::Vector3f newPosition = myTransform.GetPosition();
    DE::Vector3f newCameraPos = newPosition;

    const float targetLerp = myIsFast ? 1.0f : 0.0f;

    mySpeedLerpFactor = Lerp(mySpeedLerpFactor, targetLerp, aDeltaTime * 1.5f);

    mySpeed = Lerp(myMinSpeed, myMaxSpeed, mySpeedLerpFactor);

    DE::Vector3f cameraOffset = Lerp(myNearCameraOffset, myFarCameraOffset, mySpeedLerpFactor);
    newCameraPos += cameraOffset;

    MainSingleton::GetInstance()->GetActiveCamera()->SetPosition(newCameraPos);

    DE::Vector3f forward = myTransform.GetMatrix().GetForward();
    forward.y = 0.0f;
    newPosition = myTransform.GetPosition() + forward * mySpeed * aDeltaTime;

	if (MainSingleton::GetInstance()->GetInputManager().IsKeyHeld(DE::eKeyCode::UpArrow))
	{
        newPosition = myTransform.GetPosition() + forward * mySpeed * aDeltaTime;
	}

	if (MainSingleton::GetInstance()->GetInputManager().IsKeyHeld(DE::eKeyCode::DownArrow))
	{
        newPosition = myTransform.GetPosition() - forward * mySpeed * aDeltaTime;
	}


    DE::Vector3f yawRot = 0.f;
    // PLaying with pitch and roll
    if (MainSingleton::GetInstance()->GetInputManager().IsKeyHeld(DE::eKeyCode::LeftArrow))
    {
        yawRot.x += TURN_SPEED * aDeltaTime;
    }
    if (MainSingleton::GetInstance()->GetInputManager().IsKeyHeld(DE::eKeyCode::RightArrow))
    {
        yawRot.x -= TURN_SPEED * aDeltaTime;
    }

    if (MainSingleton::GetInstance()->GetInputManager().IsKeyHeld(DE::eKeyCode::UpArrow))
    {
        yawRot.z += TURN_SPEED * aDeltaTime;
    }
    if (MainSingleton::GetInstance()->GetInputManager().IsKeyHeld(DE::eKeyCode::DownArrow))
    {
        yawRot.z -= TURN_SPEED * aDeltaTime;
    }
    // PLaying with pitch and roll

    yawRot.y = myCurrentTurnSpeed;
    myTransform.SetRotation(myTransform.GetRotation() + yawRot);
    myTransform.SetPosition(newPosition);
    myModelInstance->SetTransform(myTransform);

    for (auto& comp : myBoatComponents)
    {
        comp->UpdateFromBoatTransform(myTransform);
        comp->Update(aDeltaTime);
    }
    MainSingleton::GetInstance()->GetShaderTool().SetWaterPlanePos(myTransform.GetPosition());

    DE::Vector3f boatTransformData = DE::Vector3f(myTransform.GetPosition().x, myTransform.GetRotation().y, myTransform.GetPosition().z);
    MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &boatTransformData, eMessageType::BoatTransformUpdate });
}

void Boat::Receive(const Message& aMessage)
{
    switch (aMessage.messageType)
    {
        case eMessageType::PlayerMove:
        {

            break;
        }
    }
}

void Boat::Render(DE::GraphicsEngine& aGraphicsEngine)
{
    aGraphicsEngine.GetModelDrawer().DrawGBCalc(*myModelInstance.get());

    for (auto& comp : myBoatComponents)
    {
        comp->Render(aGraphicsEngine);
    }
}

void Boat::RenderLight(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
    for (auto& comp : myBoatComponents)
    {
        comp->RenderLight(aGraphicsEngine);
    }
    
}

void Boat::DebugRender(DE::GraphicsEngine& aGraphicsEngine)
{
	/*DE::Vector3f startPosition = myTransform.GetPosition();
	DE::Vector3f rotation = myTransform.GetRotation();

	myTransform.GetMatrix().GetForward();

	myDeckSize.x;
	myDeckSize.z;
	
	DE::LineDrawer& lineDrawer = DE::Engine::GetInstance()->GetGraphicsEngine().GetLineDrawer();
	DE::LinePrimitive lineToDraw;
	DE::Vector4f color(1.0f, 1.0f, 1.0f, 1.0f);
	lineToDraw.fromPosition = myLineFrom;
	lineToDraw.toPosition = myLineTo;
	lineToDraw.color = color;
	lineDrawer.Draw(lineToDraw);*/
}

void Boat::SetModelInstance(std::shared_ptr<DE::ModelInstance> aInstance)
{
    myModelInstance = aInstance;
}

std::shared_ptr<BoatComponent> Boat::CreateComponent(BoatComponentType aType, std::shared_ptr<DE::ModelInstance> aInstance)
{
    std::shared_ptr<BoatComponent> comp;

    switch (aType) 
    {
    case BoatComponentType::Cannon:
        comp = std::make_shared<CannonComponent>();
        comp->SetComponentType(BoatComponentType::Cannon);
        break;
    case BoatComponentType::Light:
        comp = std::make_shared<LightComponent>();
        comp->SetComponentType(BoatComponentType::Light);
        break;
    case BoatComponentType::Repair:
        comp = std::make_shared<RepairComponent>();
        comp->SetComponentType(BoatComponentType::Repair);
        break;
    case BoatComponentType::Steer:
        comp = std::make_shared<SteerComponent>();
        comp->SetComponentType(BoatComponentType::Steer);
        break;
    case BoatComponentType::Ammo:
        comp = std::make_shared<AmmoComponent>();
        comp->SetComponentType(BoatComponentType::Ammo);
        break;
    case BoatComponentType::Speed:
        comp = std::make_shared<SpeedComponent>();
        comp->SetComponentType(BoatComponentType::Speed);
        break;
    case BoatComponentType::Map:
        comp = std::make_shared<MapComponent>();
        comp->SetComponentType(BoatComponentType::Map);
        break;
    case BoatComponentType::WeakPoint:
        comp = std::make_shared<WeakPointComponent>();
        comp->SetComponentType(BoatComponentType::WeakPoint);
        break;
    default:
        return nullptr;
    }

    // move the ModelInstance into the component
    comp->SetModelInstance(aInstance);
    comp->Init();
    return comp;
}

void Boat::AddBoatComponent(std::shared_ptr<BoatComponent> aComp)
{
    myBoatComponents.push_back(aComp);
}

std::shared_ptr<BoatComponent> Boat::FindComponentInRange(const DE::Vector3f& worldPos)
{
    float radiusSq = 50.0f * 50.0f;
    for (auto& comp : myBoatComponents)
    {
        DE::Vector3f compPos = comp->GetInteractionPoint();

        float dx = worldPos.x - compPos.x;
        float dz = worldPos.z - compPos.z;

        if (dx * dx + dz * dz <= radiusSq)
        {
            return comp;
        }
    }

    return nullptr;
}

void Boat::GiveObject(eObjectType aType, int anAmount)
{
    for (auto& comp : myBoatComponents)
    {
        switch (comp->GetComponentType())
        {
        case BoatComponentType::Ammo:
            {
                if (aType == eObjectType::Ammo)
                {
                    auto ammoComp = std::static_pointer_cast<AmmoComponent>(comp);
                    ammoComp->GiveAmmo(anAmount);
                }
            break;
            }
        case BoatComponentType::Repair:
            {
                if (aType == eObjectType::Repair)
                {
                    auto repairComp = std::static_pointer_cast<RepairComponent>(comp);
                    repairComp->GiveRepairKit(anAmount);
                }
            break;
            }
        default:
            break;
        }
    }
}

std::array<DE::Vector3f, 6> Boat::GetWeakPointPositions()
{
    std::array<DE::Vector3f, 6> positionList;
    int index = 0;

    for (auto& comp : myBoatComponents)
    {
        if (comp->GetComponentType() == BoatComponentType::WeakPoint)
        {
            positionList.at(index) = comp->GetTransform()->GetPosition();
            index++;
        }
    }

    return positionList;
}
