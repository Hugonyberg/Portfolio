#pragma once
#include <DreamEngine/utilities/CountTimer.h>
#include <DreamEngine/text/text.h>
#include "Observer.h"

namespace DreamEngine
{
	class GraphicsEngine;
}

class Resource : public Observer
{
public:
	Resource();
	~Resource();
	void Update(float aDeltaTime);
	void Render(DE::GraphicsEngine& aGraphicsEngine);
	void PassivelyDecrement(float aDeltaTime);
	void Reset();
	void AddResource(const int aResourceAmount);
	void ReduceResource(const int aResourceAmount);
	const int& GetCurrentResource() const;
	void SetCurrentResource(const int aResourceAmount);
	const int& GetMaxResource() const;
	void SetMaxResource(const int aResourceAmount);
	void Receive(const Message& aMessage) override;


private:
	std::shared_ptr<DE::Text> myText;
	int myMaxResource = 100;
	int myCurrentResource = 0;
	CU::CountdownTimer myCountDownTimer;
};