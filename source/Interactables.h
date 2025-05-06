#pragma once
#include "GameObject.h"
#include "DreamEngine/graphics/ModelInstance.h"
#include "DreamEngine/graphics/AnimationPlayer.h"
#include "DreamEngine/graphics/AnimatedModelInstance.h"
class Interactables : public GameObject
{
public:
	Interactables() = default;
	~Interactables() = default;
	virtual void Init() = 0;
	virtual void Update(float aDeltaTime);
	virtual void Render(DreamEngine::GraphicsEngine& aGraphicsEngine);
	virtual bool GetIsSelected() { return myIsSelected; }
	virtual void SetIsSelected(bool aSelectedBool) = 0;
	virtual std::shared_ptr<DreamEngine::AnimatedModelInstance> GetAnimatedModelInstance() { return myAnimatedModel; }

protected:
	std::shared_ptr<DreamEngine::AnimatedModelInstance> myAnimatedModel;
	std::vector<DE::AnimationPlayer> myAnimations;
	bool myIsSelected;
};
