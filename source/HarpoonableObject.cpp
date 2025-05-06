#include "HarpoonableObject.h"
#include "MainSingleton.h"
#include "UnityLoader.h"
#include "RigidBodyComponent.h"

#include <PhysX\PxPhysicsAPI.h>
#include <DreamEngine\engine.h>
#include <DreamEngine/graphics/ModelDrawer.h>
#include <DreamEngine\math\Matrix4x4.h>

void HarpoonableObject::InitCollider()
{
	AddComponent<RigidBodyComponent>();
	auto* physXScene = MainSingleton::GetInstance()->GetPhysXScene();

	physx::PxShape* shape = DE::Engine::GetPhysXPhysics()->createShape(physx::PxSphereGeometry(myRadius), *MainSingleton::GetInstance()->GetPhysXMaterials()[0]);
	auto filter = MainSingleton::GetInstance()->GetCollisionFiltering();
	filter.setupFiltering(shape, filter.HarpoonableObjects, filter.Harpoon);
	physx::PxTransform transform = physx::PxTransform(physx::PxVec3(myTransform.GetPosition().x, myTransform.GetPosition().y, myTransform.GetPosition().z), 
													  physx::PxQuat(myTransform.GetQuaternion().x, myTransform.GetQuaternion().y, myTransform.GetQuaternion().z, myTransform.GetQuaternion().w));
	
	physx::PxRigidStatic* body = DE::Engine::GetPhysXPhysics()->createRigidStatic(transform);
	body->setName("HarpoonableObject");
	body->attachShape(*shape);
	myBoundingSphere = DE::SphereCollider<float>(&myTransform, 0.0f, myRadius);

	if (shape == nullptr)
	{
		return;
	}

	physXScene->addActor(*body);
	GetComponent<RigidBodyComponent>()->SetBody(body);
}

const DE::SphereCollider<float>& HarpoonableObject::GetBoundingSphere() const
{
	return myBoundingSphere;
}

void HarpoonableObject::Update(float aDeltaTime)
{
	aDeltaTime;
}

void HarpoonableObject::Render(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
	aGraphicsEngine.GetModelDrawer().DrawGBCalc(*myModelInstance.get());
}

void HarpoonableObject::SetModelInstance(std::shared_ptr<DreamEngine::ModelInstance>& aModelInstance)
{
	myModelInstance = aModelInstance;
}

std::shared_ptr<DreamEngine::ModelInstance> HarpoonableObject::GetModelInstance()
{
	return myModelInstance;
}