#pragma once
#include <DreamEngine/graphics/Camera.h>
#include <DreamEngine/graphics/ModelInstance.h>
#include "ShaderTool.h"
#include "VFXManager.h"
#include "GameObject.h"
#include "Observer.h"
#include "PlayerStats.h"

#include "Boat.h"

class Player : public GameObject, public Observer
{
public:
	// Init stage functions
	Player() = default;
	Player(const Player& anOtherPlayer) = delete;
	Player& operator=(const Player& anOtherPlayer) = delete;
	Player(Player&& anOtherPlayer) = default;
	Player& operator=(Player&& anOtherPlayer) = default;
	~Player();
	void Deallocate();
	void Init();
	
	void SetBoatPointer(std::shared_ptr<Boat> aBoat) { myBoat = aBoat; }

	std::shared_ptr<Boat> GetBoat() { return myBoat; }

	// Update stage functions
	void Update(const float aDeltaTime) override;
	void Receive(const Message& aMessage) override;

	// Render stage functions
	void Render(DE::GraphicsEngine& aGraphicsEngine) override;

	void TogglePlayerLock() { myStats.isLocked = !myStats.isLocked; }
	bool GetPlayerLock() { return myStats.isLocked; }
	void SetLockPosition(DE::Vector3f aPosition, DE::Vector3f aDirection);
	void GiveObject(eObjectType aType);
	void RemoveObject();
	bool IsCarryingObject() { return myStats.isCarryingObject; }
	eObjectType GetInventory() { return myStats.inventory; }

	void CaptureLocalTransform(const DreamEngine::Transform& boatXform);
	void SyncTransformWithBoat();

private:
	// Init stage functions
	void SetUpComponents();
	void SetUpSubscriptions();
	void SetUpModel();
	void InitializeMemberVariables();

	// Update stage functions
	bool CheckIfGrounded();
	void MovePlayer(float aDeltaTime);
	void HandleInteractingObject(float aDeltaTime);
	void Interact();
	void ResetInputMessages();

	std::shared_ptr<DE::Camera> myCamera;
	PlayerStats myStats;
	std::shared_ptr<DE::ModelInstance> myModelInstance;
	std::shared_ptr<DE::ModelInstance> myInventoryModel;
	std::shared_ptr<DE::ModelInstance> myAmmoModel;
	std::shared_ptr<DE::ModelInstance> myRepairModel;
	
	DE::Transform myLocalTransform;
	std::shared_ptr<Boat> myBoat;
	std::shared_ptr<BoatComponent> myInteractedComponent;
};