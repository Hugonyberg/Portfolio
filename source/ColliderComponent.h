#pragma once
#include "Component.h"
#include <memory>

namespace DreamEngine 
{
	class Collider;
}
namespace DE = DreamEngine;

class ColliderComponent : public Component
{
public:
	ColliderComponent(GameObject& aObject) : Component(aObject), myCollider(nullptr) {};
	void Init() { myIsActive = true; };
	const std::shared_ptr<const DE::Collider> GetCollider() { return myCollider; };
	void SetCollider(std::shared_ptr<DE::Collider> aCollider) { myCollider = aCollider; };
private:
	std::shared_ptr<DE::Collider> myCollider;
};