#pragma once

class GameObject;
class Component
{
public:
	Component(GameObject& aGameObject);
	virtual ~Component() {};

	virtual void Init() { __noop; };
	virtual void Update(float aDeltaTime) { __noop; aDeltaTime; };

	//virtual Component* MakeCopy(GameObject* aGameObject);
	GameObject& GetGameObject() { return myGameObject; };
	inline int GetID() { return myComponentID; };

	void SetActive(bool aActive) { myIsActive = aActive; }
	bool IsActive() { return myIsActive; }
protected:
	GameObject& myGameObject;
	int myComponentID = 0;
	bool myIsActive = true;
};