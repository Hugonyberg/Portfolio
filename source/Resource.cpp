#include "Resource.h"
#include <DreamEngine/utilities/UtilityFunctions.h>
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/engine.h>
#include "Message.h"

Resource::Resource()
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::Vector2ui intResolution = engine.GetRenderSize();
	DreamEngine::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };

	myText = std::make_shared<DE::Text>(L"Text/arial.ttf", DE::FontSize_18);
	myText->SetText(std::to_string(myCurrentResource));
	myText->SetColor(DE::Color(1, 1, 1, 1));
	myText->SetPosition(DE::Vector2f{ 0.5f, 0.5f } * resolution);
}

Resource::~Resource()
{
}

void Resource::Update(float aDeltaTime)
{
	aDeltaTime;
}

void Resource::Render(DE::GraphicsEngine& aGraphicsEngine)
{
	aGraphicsEngine;
	myText->Render();
}

void Resource::PassivelyDecrement(float aDeltaTime)
{
	myCountDownTimer.Update(aDeltaTime);  

	if (myCountDownTimer.IsDone()) // By default is true every second if nothing else is stated in CountdownTimer constructor
	{
		if (myCurrentResource != 0)
		{
			myCurrentResource -= 1;
		}
		myCountDownTimer.Reset();
	}
}

void Resource::Reset()
{
	myCurrentResource = 0;
}

const int& Resource::GetCurrentResource() const
{
	return myCurrentResource;
}

void Resource::SetCurrentResource(const int aResourceAmount)
{
	if (aResourceAmount <= myMaxResource)
	{
		myCurrentResource = aResourceAmount;
	}
	else
	{
		myCurrentResource = myMaxResource;
	}
}

void Resource::AddResource(const int aResourceAmount)
{
	if (myCurrentResource + aResourceAmount < myMaxResource)
	{
		myCurrentResource += aResourceAmount;
	}
	else
	{
		myCurrentResource = myMaxResource;
	}
}

void Resource::ReduceResource(const int aResourceAmount)
{
	if (myCurrentResource - aResourceAmount >= 0)
	{
		myCurrentResource -= aResourceAmount;
	}
	else
	{
		myCurrentResource = 0;
	}
}

const int& Resource::GetMaxResource() const
{
	return myMaxResource;
}

void Resource::SetMaxResource(const int aResourceAmount)
{
	myMaxResource = aResourceAmount;
}

void Resource::Receive(const Message& aMessage)
{
}
