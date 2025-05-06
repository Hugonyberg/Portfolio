#include "DeferredDecal.h"
#include <DreamEngine\engine.h>
#include <DreamEngine\graphics\GraphicsEngine.h>
#include <DreamEngine\graphics\TextureManager.h>
#include <DreamEngine\graphics\ModelFactory.h>
#include <DreamEngine\graphics\ModelInstance.h>
#include <DreamEngine\graphics\ModelDrawer.h>
#include <DreamEngine\windows\settings.h>

void DeferredDecal::Init(std::wstring aSpritePath)
{
	myTexture = DE::Engine::GetInstance()->GetTextureManager().GetTexture(aSpritePath.c_str());
	myBoundingBox = std::make_shared<DE::ModelInstance>(DE::ModelFactory::GetInstance().GetModelInstance(DE::Settings::ResolveAssetPathW(L"3D/cubePrimitive.fbx")));
	myBoundingBox->SetTransform(myTransform);
	myBoundingBox->SetScale(myTransform.GetScale() * 100.0f);
	myBoundingBox->SetRotation(myTransform.GetRotation() + DE::Rotator(180.0f, 0.0f, 0.0f));
}

void DeferredDecal::Render(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
	myTexture->SetAsResourceOnSlot(11);
	aGraphicsEngine.GetModelDrawer().DrawGBDecal(*myBoundingBox);
}