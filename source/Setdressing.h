#pragma once
#include "GameObject.h"
#include "Observer.h"
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/graphics/ModelInstance.h>
#include <DreamEngine\math\Collider.h>


struct ColliderData;
class Setdressing : public GameObject, public Observer
{
public:
	Setdressing();
	~Setdressing();
	void InitCollider(ColliderData& someColliderData);
	void SetBoundingSphere();
	const DE::SphereCollider<float>& GetBoundingSphere() const;

	void Update(float aDeltaTime) override;
	void Render(DreamEngine::GraphicsEngine& aGraphicsEngine) override; 
	void RenderShadowToGBuffer(DreamEngine::GraphicsEngine& aGraphicsEngine);
	void Receive(const Message& aMsg) override; 

	void SetModelInstance(std::shared_ptr<DreamEngine::ModelInstance>& aModelInstance); 
	void SetObjectHideID(int aID);

	std::shared_ptr<DreamEngine::ModelInstance> GetModelInstance();

private:

	std::shared_ptr<DreamEngine::ModelInstance> myModelInstance;
	DE::SphereCollider<float> myBoundingSphere;
	int myObjectHideID = -1; 
	bool myObjectIsOverPlayer = false;
};