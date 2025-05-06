#include "Setdressing.h"
#include "Message.h"
#include "MainSingleton.h"
#include "UnityLoader.h"
#include "RigidBodyComponent.h"
#include "RenderCommands.h"
#include "UnityLoader.h"

#include <PhysX\PxPhysicsAPI.h>
#include <DreamEngine\engine.h>
#include <DreamEngine/graphics/ModelDrawer.h>
#include <DreamEngine\math\Matrix4x4.h>

Setdressing::Setdressing()
{
}

Setdressing::~Setdressing()
{
}

void Setdressing::InitCollider(ColliderData& someColliderData)
{
	AddComponent<RigidBodyComponent>();
	auto* physXScene = MainSingleton::GetInstance()->GetPhysXScene();
	physx::PxShape* shape = nullptr;
	physx::PxRigidStatic* body = nullptr;
	switch (someColliderData.type)
	{
		case eColliderType::Sphere: 
		{
			shape = DE::Engine::GetPhysXPhysics()->createShape(physx::PxSphereGeometry(someColliderData.radius), *MainSingleton::GetInstance()->GetPhysXMaterials()[0]);

			auto filter = MainSingleton::GetInstance()->GetCollisionFiltering();
			filter.setupFiltering(shape, filter.Environment, filter.Player | filter.Enemy);
			
			// Convert offset to a PhysX vector
			physx::PxVec3 localOffset = physx::PxVec3(someColliderData.offset.x, someColliderData.offset.y, someColliderData.offset.z);
			// Get rotation quaternion
			physx::PxQuat rotation = physx::PxQuat(myTransform.GetQuaternion().x, myTransform.GetQuaternion().y, myTransform.GetQuaternion().z, myTransform.GetQuaternion().w);
			// Rotate offset into world space
			physx::PxVec3 rotatedOffset = rotation.rotate(localOffset);
			// Apply rotated offset
			physx::PxVec3 finalPosition = physx::PxVec3(myTransform.GetPosition().x, myTransform.GetPosition().y, myTransform.GetPosition().z) + rotatedOffset;
			// Create transform with correct position and rotation
			physx::PxTransform transform = physx::PxTransform(finalPosition, rotation);
			body = DE::Engine::GetPhysXPhysics()->createRigidStatic(transform);
			body->setName("Environment");
			body->attachShape(*shape);
			myBoundingSphere = DE::SphereCollider<float>(&myTransform, 0.0f, someColliderData.radius);
			break;
		}
		case eColliderType::Box: 
		{
			shape = DE::Engine::GetPhysXPhysics()->createShape(physx::PxBoxGeometry(someColliderData.size.x, someColliderData.size.y, someColliderData.size.z), *MainSingleton::GetInstance()->GetPhysXMaterials()[0]);

			auto filter = MainSingleton::GetInstance()->GetCollisionFiltering();
			filter.setupFiltering(shape, filter.Environment, filter.Player | filter.Enemy);
			
			// Convert offset to a PhysX vector
			physx::PxVec3 localOffset = physx::PxVec3(someColliderData.offset.x, someColliderData.offset.y, someColliderData.offset.z);
			// Get rotation quaternion
			physx::PxQuat rotation = physx::PxQuat(myTransform.GetQuaternion().x, myTransform.GetQuaternion().y, myTransform.GetQuaternion().z, myTransform.GetQuaternion().w);
			// Rotate offset into world space
			physx::PxVec3 rotatedOffset = rotation.rotate(localOffset);
			// Apply rotated offset
			physx::PxVec3 finalPosition = physx::PxVec3(myTransform.GetPosition().x, myTransform.GetPosition().y, myTransform.GetPosition().z) + rotatedOffset;
			// Create transform with correct position and rotation
			physx::PxTransform transform = physx::PxTransform(finalPosition, rotation);
			body = DE::Engine::GetPhysXPhysics()->createRigidStatic(transform);
			body->setName("Environment");
			body->attachShape(*shape);
			float radius = someColliderData.size.Length();
			myBoundingSphere = DE::SphereCollider<float>(&myTransform, 0.0f, radius);
			break;
		}
		case eColliderType::Mesh: 
		{
			body = DE::Engine::GetPhysXPhysics()->createRigidStatic(physx::PxTransform(myTransform.GetPosition().x + someColliderData.offset.x, myTransform.GetPosition().y + someColliderData.offset.y, myTransform.GetPosition().z + someColliderData.offset.z, physx::PxQuat(myTransform.GetQuaternion().x, myTransform.GetQuaternion().y, myTransform.GetQuaternion().z, myTransform.GetQuaternion().w)));
			physx::PxTriangleMeshDesc meshDesc;

			meshDesc.points.count = static_cast<physx::PxU32>(someColliderData.meshData.vertices.size());
			meshDesc.points.stride = sizeof(physx::PxVec3);
			meshDesc.triangles.count = static_cast<physx::PxU32>(someColliderData.meshData.indices[0].size() / 3);
			meshDesc.triangles.stride = sizeof(physx::PxU32) * 3;

			meshDesc.points.data = someColliderData.meshData.vertices.data();
			meshDesc.triangles.data = someColliderData.meshData.indices[0].data();
			auto scale = DE::Engine::GetPhysXPhysics()->getTolerancesScale();
			scale.length *= 10.0f;
			physx::PxCookingParams params(scale);

			physx::PxDefaultMemoryOutputStream outputStream;
			if (!PxCookTriangleMesh(params, meshDesc, outputStream))
			{
				assert(false && L"FAILED");
			}

			physx::PxDefaultMemoryInputData inputData(outputStream.getData(), outputStream.getSize());

			physx::PxTriangleMesh* aTriangleMesh = DE::Engine::GetPhysXPhysics()->createTriangleMesh(inputData);
			shape = DE::Engine::GetPhysXPhysics()->createShape(physx::PxTriangleMeshGeometry(aTriangleMesh), *MainSingleton::GetInstance()->GetPhysXMaterials()[0]);
			
			auto filter = MainSingleton::GetInstance()->GetCollisionFiltering();
			filter.setupFiltering(shape, filter.Environment, filter.Player | filter.Enemy);
			body->setName("Environment");

			body->attachShape(*shape);
			float minX = someColliderData.meshData.vertices[0].x;
			float minY = someColliderData.meshData.vertices[0].y;
			float minZ = someColliderData.meshData.vertices[0].z;
			float maxX = someColliderData.meshData.vertices[0].x;
			float maxY = someColliderData.meshData.vertices[0].y;
			float maxZ = someColliderData.meshData.vertices[0].z;
			for (auto& vertex : someColliderData.meshData.vertices) 
			{
				if (vertex.x < minX)
					minX = vertex.x;
				if (vertex.x > maxX)
					maxX = vertex.x;
				if (vertex.y < minY)
					minY = vertex.y;
				if (vertex.y > maxY)
					maxY = vertex.y;
				if (vertex.z < minZ)
					minZ = vertex.z;
				if (vertex.z > maxZ)
					maxZ = vertex.z;
			}
			float radius = sqrt(
				(maxX - minX) * (maxX - minX) +
				(maxY - minY) * (maxY - minY) +
				(maxZ - minZ) * (maxZ - minZ)
			) / 2.0f;
			myBoundingSphere = DE::SphereCollider<float>(&myTransform, 0.0f, radius);
			break;
		}
		default: 
		{
			shape = nullptr;
			break;
		}
	}
	if (shape == nullptr) 
	{
		return;
	}

	physXScene->addActor(*body);
	GetComponent<RigidBodyComponent>()->SetBody(body);
}

void Setdressing::SetBoundingSphere()
{
	auto* bodyCmp = GetComponent<RigidBodyComponent>();
	if (bodyCmp == nullptr) 
	{
		float biggestRadius = 0.0f;
		for (int i = 0; i < myModelInstance->GetModel()->GetMeshCount(); i++)
		{
			if (myModelInstance->GetModel()->GetMeshData(i).bounds.Radius > biggestRadius) 
			{
				biggestRadius = myModelInstance->GetModel()->GetMeshData(i).bounds.Radius;
			}
		}
		myBoundingSphere = DE::SphereCollider<float>(&myTransform, 0.0f, biggestRadius * 1.25f);
	}
}

const DE::SphereCollider<float>& Setdressing::GetBoundingSphere() const
{
	return myBoundingSphere;
}

void Setdressing::SetModelInstance(std::shared_ptr<DreamEngine::ModelInstance>& aModelInstance)
{
	myModelInstance = aModelInstance;
}

void Setdressing::SetObjectHideID(int aID)
{
	myObjectHideID = aID; 
}

std::shared_ptr<DreamEngine::ModelInstance> Setdressing::GetModelInstance()
{
	return myModelInstance;
}

void Setdressing::Update(float aDeltaTime)
{
	aDeltaTime;
}

void Setdressing::Render(DreamEngine::GraphicsEngine & aGraphicsEngine)
{
	if (myObjectIsOverPlayer == true)
	{
		return;
	}
	aGraphicsEngine.GetModelDrawer().DrawGBCalc(*myModelInstance.get());
}

void Setdressing::RenderShadowToGBuffer(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
	aGraphicsEngine.GetModelDrawer().DrawGBCalc(*myModelInstance.get());
}

void Setdressing::Receive(const Message& aMsg)
{
	
}