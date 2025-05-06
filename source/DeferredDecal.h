#pragma once
#include <memory>
#include <string>
#include "GameObject.h"

namespace DreamEngine 
{
	class ModelInstance;
	class TextureResource;
}

class DeferredDecal : public GameObject
{
public:
	DeferredDecal() = default;
	~DeferredDecal() = default;
	void Init(std::wstring aSpritePath);

	void Render(DreamEngine::GraphicsEngine& aGraphicsEngine) override;

private:
	std::shared_ptr<DreamEngine::ModelInstance> myBoundingBox;
	DreamEngine::TextureResource* myTexture;
};