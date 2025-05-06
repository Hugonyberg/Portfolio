#pragma once
#include <vector>
#include <string>
#include "Component.h"
#include <DreamEngine/math/Transform.h>
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <bitset>

enum class eCollisionLayer;
enum class eTag
{
	Player = 0,
	Enemy,
	EnemyProjectile,
	Boss,
	BreakableObject,
	AbilityShrine,
	ResourceStone,
	CombatBubbel,
	CombatBubbelExtraCollider,
	TriggerBox,
	ProgressionBlocker,
	SetDressing,
	DefaultTypeAndCount
};

class GameObject
{
public:
	GameObject();
	virtual ~GameObject() {
		for (int i = 0; i < myComponents.size(); i++)
		{
			delete myComponents[i];
		}
		myComponents.clear();
	}


	virtual void Init()
	{
		for (int i = 0; i < myComponents.size(); i++)
		{
			myComponents[i]->Init();
		}
	};
	virtual void Update(float aDeltaTime)
	{
		for (int i = 0; i < myComponents.size(); i++)
		{
			myComponents[i]->Update(aDeltaTime);
		}
	};
	virtual void Render(DreamEngine::GraphicsEngine& aGraphicsEngine) = 0;
	virtual void RenderLight(DreamEngine::GraphicsEngine& aGraphicsEngine) { __noop; }

	const int GetID() { return myID; }
	const std::string& GetName() { return myName; }
	const eTag& GetTag() { return myTag; }
	bool IsActive() { return myIsActive; };
	bool WasActiveLastFrame() { return myWasActiveLastFrame; };
	void SetIsActive(bool anIsActive) { myIsActive = anIsActive; };
	void SetWasActive(bool anIsActive) { myWasActiveLastFrame = anIsActive; };

	DreamEngine::Transform* const GetTransform() { return &myTransform; };
	void SetTransform(const DreamEngine::Transform& aTransform) { myTransform = aTransform; }

	std::bitset<8>* const GetCollisionLayers() { return &myCollisionLayers; };
	void SetCollisionLayers(const std::bitset<8> someCollisionLayers) { myCollisionLayers = someCollisionLayers; }

	virtual void OnCollision(GameObject*, eCollisionLayer) { __noop; }

	template <typename T>
	inline void AddComponent();
	template <typename T>
	inline T* GetComponent();
	template <typename T>
	inline T* GetComponent() const;
	template <typename T>
	inline bool TryGetComponent(T*& aOutComponent);
	template <typename T>
	inline bool HasComponent();

protected:
	const int myID;
	eTag myTag = eTag::DefaultTypeAndCount;
	std::string myName;
	DreamEngine::Transform myTransform;
	std::bitset<8> myCollisionLayers;
	std::vector<Component*> myComponents;
	bool myIsActive = true;
	bool myWasActiveLastFrame = false;
};

template<typename T>
inline void GameObject::AddComponent()
{
	myComponents.emplace_back(new T(*this));
}
template<typename T>
inline T* GameObject::GetComponent()
{
	for (int i = 0; i < myComponents.size(); i++)
	{
		if (myComponents[i] == nullptr)
		{
			continue;
		}
		T* component = dynamic_cast<T*>(myComponents[i]);
		if (component != nullptr)
		{
			return component;
		}
	}
	return nullptr;
}
template<typename T>
inline T* GameObject::GetComponent() const
{
	for (int i = 0; i < myComponents.size(); i++)
	{
		if (myComponents[i] == nullptr)
		{
			continue;
		}
		T* component = dynamic_cast<T*>(myComponents[i]);
		if (component != nullptr)
		{
			return component;
		}
	}
	return nullptr;
}
template<typename T>
inline bool GameObject::TryGetComponent(T*& aOutComponent)
{
	for (int i = 0; i < myComponents.size(); i++)
	{
		if (myComponents[i] == nullptr)
		{
			continue;
		}
		T* component = dynamic_cast<T*>(myComponents[i]);
		if (component != nullptr)
		{
			aOutComponent = component;
			return true;
		}
	}
	return false;
}
template<typename T>
inline bool GameObject::HasComponent()
{
	for (int i = 0; i < myComponents.size(); i++)
	{
		if (myComponents[i] == nullptr)
		{
			continue;
		}
		T* component = dynamic_cast<T*>(myComponents[i]);
		if (component != nullptr)
		{
			return true;
		}
	}
	return false;
}