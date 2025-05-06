#pragma once
#include "GameObject.h"
#include "Observer.h"

#include <DreamEngine\graphics\ModelInstance.h>

class Player;

enum class BoatComponentType
{
	Cannon,		// Two Cannons
	Light,		// Front Headlight
	Repair,		// Box with planks to repair ship
	Steer,		// Steering Wheel
	Ammo,		// Box with Cannon Ammo
	Speed,		// Speed Lever
	Map,		// Radar/map
	WeakPoint,	// Points on ship to be repaired 
	Count,
};

class BoatComponent : public GameObject, public Observer
{
public:
    virtual ~BoatComponent() = default;

	virtual void Init() override;

	virtual void Update(float aDeltaTime) override;
	virtual void Render(DE::GraphicsEngine& aGraphicsEngine) override;

    virtual void OnInteractSouthButton(Player& aPlayer) = 0;      // “press” or “grab”
	virtual void OnInteractEastButton(Player& aPlayer) { };      // “press” or “grab”
    virtual void OnInteractNorthButton(Player& aPlayer) { }   // default no-op

    virtual void TriggerContact(Player& aPlayer) { }   // default no-op

	virtual void HandlePlayerInput(Player& aPlayer) { }

	virtual bool WantsPlayerLock() const { return false; }

	//hannah has been here too ;u;
	BoatComponentType GetComponentType(){ return myBoatComponentType; }
	void SetComponentType(BoatComponentType aType){ myBoatComponentType = aType; }

	void SetInteractionPoint(DE::Vector3f aPoint) { myInteractionPoint = aPoint; }
	const DE::Vector3f GetInteractionPoint() const { return myInteractionPoint; }

	void Receive(const Message& aMessage) override 
	{

	}

	void SetModelInstance(std::shared_ptr<DE::ModelInstance> aInstance) 
	{
		myModelInstance = aInstance;
		myTransform = myModelInstance->GetTransform();
	}

	void CaptureLocalTransform(const DreamEngine::Transform& boatXform)
	{
		DreamEngine::Transform worldXform = myModelInstance->GetTransform();

		DE::Vector3f worldPosition = worldXform.GetPosition();
		DE::Vector3f localPosition = boatXform.InverseTransformPosition(worldPosition);

		DE::Quaternionf boatQuat = boatXform.GetQuaternion();
		DE::Quaternionf inverseBoatQuat = boatQuat.GetConjugate();
		DE::Quaternionf worldQuat = worldXform.GetQuaternion();
		DE::Quaternionf localQuat = inverseBoatQuat * worldQuat;

		/*DE::Vector3f boatScale = boatXform.GetScale();
		DE::Vector3f worldScale = worldXform.GetScale();
		DE::Vector3f localScale = {worldScale.x / boatScale.x, worldScale.y / boatScale.y, worldScale.z / boatScale.z};*/
		DE::Vector3f localScale = worldXform.GetScale();

		myLocalTransform = DreamEngine::Transform{
			localPosition,
			localQuat,
			localScale
		};
		
		// Handle Interaction point
		myInteractionPointLocal = boatXform.InverseTransformPosition(myInteractionPoint);
	}

	void UpdateFromBoatTransform(const DreamEngine::Transform& boatXform) 
	{
		DE::Vector3f localPosition = myLocalTransform.GetPosition();
		DE::Vector3f worldPosition = boatXform.TransformPosition(localPosition);
		myInteractionPoint = boatXform.TransformPosition(myInteractionPointLocal);

		DE::Quaternionf boatQuat = boatXform.GetQuaternion();
		DE::Quaternionf localQuat = myLocalTransform.GetQuaternion();
		DE::Quaternionf worldQuat = boatQuat * localQuat;

		/*DE::Vector3f boatScale = boatXform.GetScale();
		DE::Vector3f localScale = myLocalTransform.GetScale();
		DE::Vector3f worldScale = boatScale * localScale;*/

		DE::Vector3f worldScale = myLocalTransform.GetScale();

		DreamEngine::Transform newXform{ worldPosition, worldQuat, worldScale };
		myModelInstance->SetTransform(newXform);
		myTransform = myModelInstance->GetTransform();
	}

protected:
	std::shared_ptr<DE::ModelInstance> myModelInstance;
	BoatComponentType myBoatComponentType = BoatComponentType::Count;
	
	DreamEngine::Transform myLocalTransform;
	
private:

	DE::Vector3f myInteractionPoint;
	DE::Vector3f myInteractionPointLocal;
};