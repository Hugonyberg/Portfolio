#include "stdafx.h"
#include "LightComponent.h"

#include <DreamEngine\graphics\GraphicsEngine.h>
#include <DreamEngine\graphics\ModelDrawer.h>
#include <DreamEngine\graphics\GraphicsStateStack.h>
#include <DreamEngine\graphics\ModelFactory.h>

#include <DreamEngine\graphics\SpotLight.h>

#include "MainSingleton.h"

void LightComponent::Init()
{
    myLight = std::make_shared<DE::SpotLight>();
    myLight->SetColor(DE::Color(1.0f, 0.0f, 0.0f));
    myLight->SetIntensity(1000000.0f);
    myLight->SetRange(1000000.0f);
    myLight->SetInnerAngle(30.0f);
    myLight->SetOuterAngle(90.0f);

    myLight->myObjPtr = new DreamEngine::ModelInstance(DE::ModelFactory::GetInstance().GetModelInstance(L"3D/spherePrimitive.fbx"));
    myLight->myObjPtr->SetScale({ 1000000.0f, 1000000.0f, 1000000.0f });

    myLight->SetPosition(myTransform.GetPosition());
    myLight->myObjPtr->SetLocation(myTransform.GetPosition());
}

void LightComponent::OnInteractSouthButton(Player& aPlayer)
{
    myLightOn = !myLightOn;
}

void LightComponent::OnInteractNorthButton(Player& aPlayer)
{
    myLightOn = !myLightOn; // TODO
}

void LightComponent::RenderLight(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
    if (myLightOn)
    {
        DE::Vector3f lightPos = myTransform.GetPosition();
        lightPos.y += 200.0f;
        myLight->SetPosition(lightPos);
        myLight->myObjPtr->SetLocation(lightPos);

        auto& graphicsStateStack = aGraphicsEngine.GetGraphicsStateStack();
        auto& modelDrawer = aGraphicsEngine.GetModelDrawer();

        graphicsStateStack.AddSpotLight(*myLight);
        modelDrawer.DrawGBPSL(*myLight->myObjPtr);
        graphicsStateStack.ClearPointLightsAndSpotLights();
    }
}
