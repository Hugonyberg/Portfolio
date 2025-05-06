#pragma once
#include <DreamEngine/graphics/sprite.h>
#include <DreamEngine/graphics/ModelInstance.h>
#include <DreamEngine/graphics/AnimatedModelInstance.h>
#include <DreamEngine/graphics/ModelInstancer.h>

struct RenderData3DSprite
{
	DE::SpriteSharedData* sharedData;
	DE::Sprite3DInstanceData* spriteInstance;
};

struct RenderData3D
{
	const DE::ModelInstance* modelInstance = 0;
};

//struct InstancedRenderData3D
//{
//	std::vector<DE::ModelInstancer> modelInstancer;
//	std::vector<DE::ModelInstancer*> referenceModelInstancers;
//};

struct AnimatedRenderData3D
{
	const DE::AnimatedModelInstance* animatedModelInstance;
};

struct BatchedSpriteRenderData
{
	DE::SpriteSharedData sharedData;
	std::vector<DE::Sprite3DInstanceData> instances;
};