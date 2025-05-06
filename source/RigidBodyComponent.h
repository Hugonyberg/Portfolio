#pragma once
#include "Component.h"
#include <memory>

namespace physx 
{
	class PxRigidActor;
}

class RigidBodyComponent : public Component
{
public:
	RigidBodyComponent(GameObject& aObject) : Component(aObject), myBody(nullptr) {};
	void Init() { myIsActive = true; };
	physx::PxRigidActor* GetBody() { return myBody; };
	void SetBody(physx::PxRigidActor* aBody) { myBody = aBody; };
private:
	physx::PxRigidActor* myBody;
};