#include "stdafx.h"
#include "BoatComponent.h"

#include <DreamEngine\graphics\ModelDrawer.h>

void BoatComponent::Init()
{
}

void BoatComponent::Update(const float aDeltaTime)
{
}

void BoatComponent::Render(DE::GraphicsEngine& aGraphicsEngine)
{
	aGraphicsEngine.GetModelDrawer().DrawGBCalc(*myModelInstance.get());
}
