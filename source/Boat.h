#pragma once
#include "GameObject.h"
#include "Observer.h"
#include "BoatComponent.h"

#include <DreamEngine\graphics\ModelInstance.h>

enum class eObjectType;

class Boat : public GameObject, public Observer
{
public:
	Boat() = default;
	~Boat() = default;

	void Init();

	void Update(const float aDeltaTime) override;
	void Receive(const Message& aMessage) override;

	void Render(DE::GraphicsEngine& aGraphicsEngine) override;
	void RenderLight(DreamEngine::GraphicsEngine& aGraphicsEngine) override;

	void DebugRender(DE::GraphicsEngine& aGraphicsEngine);

	void SetModelInstance(std::shared_ptr<DE::ModelInstance> aInstance);

	std::shared_ptr<BoatComponent> CreateComponent(BoatComponentType aType, std::shared_ptr<DE::ModelInstance> aInstance);

	void AddBoatComponent(std::shared_ptr<BoatComponent>);

	void ToggleSpeed() { myIsFast = !myIsFast; }
	void SetTurnSpeed(float aTurnSpeed) { myCurrentTurnSpeed = aTurnSpeed; }

	std::shared_ptr<BoatComponent> Boat::FindComponentInRange(const DE::Vector3f& worldPos);
	void GiveObject(eObjectType aType, int anAmount);

	std::array<DE::Vector3f,6> GetWeakPointPositions();

private:
	std::vector<std::shared_ptr<BoatComponent>> myBoatComponents;

	DE::Vector3f myFarCameraOffset;
	DE::Vector3f myNearCameraOffset;
	
	float myMaxSpeed = 750.0f;
	float myMinSpeed = 250.0f;
	float mySpeed = 750.0f;

	float myCurrentTurnSpeed = 0.0f;
	float mySpeedLerpFactor = 0.0f;

	bool myIsFast = false;

	DE::Vector3f myDeckSize { 600.0f,0.0f,1800.0f };

	std::shared_ptr<DE::ModelInstance> myModelInstance;
};