#pragma once
#include "GameObject.h"
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/graphics/ModelInstance.h>
#include <DreamEngine\math\Collider.h>


struct ColliderData;
class HarpoonableObject : public GameObject
{
public:
	HarpoonableObject() = default;
	~HarpoonableObject() = default;
	void InitCollider();
	const DE::SphereCollider<float>& GetBoundingSphere() const;

	void Update(float aDeltaTime) override;
	void Render(DreamEngine::GraphicsEngine& aGraphicsEngine) override;

	void SetModelInstance(std::shared_ptr<DreamEngine::ModelInstance>& aModelInstance);
	std::shared_ptr<DreamEngine::ModelInstance> GetModelInstance();

private:
	std::shared_ptr<DreamEngine::ModelInstance> myModelInstance;
	DE::SphereCollider<float> myBoundingSphere;
	float myRadius = 50.0f;
};