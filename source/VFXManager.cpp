#include "VFXManager.h"

#include <DreamEngine/engine.h>
#include <DreamEngine/graphics/SpriteDrawer.h>
#include <DreamEngine/graphics/TextureManager.h>
#include <DreamEngine/graphics/GraphicsStateStack.h>
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/windows/settings.h>

#include "MainSingleton.h" 
VFXManager::VFXManager()
{
}

VFXManager::~VFXManager()
{
}

void VFXManager::Init()
{
	//MainSingleton::GetInstance()->GetPostMaster().Subscribe(eMessageType::PlayerStartVFX, this);
	MainSingleton::GetInstance()->GetPostMaster().Subscribe(eMessageType::PlayerStopVFX, this);
}

std::shared_ptr<VFX> VFXManager::Create2DVFX(eVFX aVFXType, DreamEngine::Vector2f aPosition, bool isLooping , int aFPS )
{
	std::shared_ptr<VFX> tempVFX;
	switch (aVFXType)
	{
	case eVFX::AlexandersBidrag:
		tempVFX = std::make_shared<VFX>(DreamEngine::Engine::GetInstance()->GetTextureManager().GetTexture(L"../Assets/Textures/VFX/Circle_5x5.png"), aPosition, isLooping, aFPS, 9, 7);
		break;
	}
	return nullptr;
}

void VFXManager::CreateVFX(const wchar_t* aTexturePath)
{
	//        9876543210
	// XXXXXX_16x16.dds

	int aSizeForHeightAndWith;
	int pathLength = static_cast<int>(wcslen(aTexturePath));
	if (pathLength > 10)
	{
		if (aTexturePath[pathLength - 1] == L'g')
		{
			int firstNumber = static_cast<int>(aTexturePath[pathLength - 5]);
			int secondNumber = 48;
			aSizeForHeightAndWith = firstNumber - secondNumber;
		}
	}

	std::shared_ptr<VFX> tempVFX;
	unsigned short fps = 60;
	bool isLooping = false;
	DreamEngine::Vector3f postition;

	tempVFX = std::make_shared<VFX>(DreamEngine::Engine::GetInstance()->GetTextureManager().GetTexture(aTexturePath), postition, isLooping, fps, aSizeForHeightAndWith, aSizeForHeightAndWith, 10.f);
	tempVFX->GetInstance().myColor = { 1.f,1.f,1.f, 1.f };
	
	myActiveSprites.push_back(tempVFX);
	myHasActiveVFX = true;
}

bool VFXManager::CanPlayVFX() const
{
	if (myHasActiveVFX)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void VFXManager::Update(float aDeltaTime)
{
	for (auto it = myActiveSprites.begin(); it != myActiveSprites.end();)
	{
		it->get()->Update(aDeltaTime);
		if (it->get()->GetIsDone())
		{
			it = myActiveSprites.erase(it);
		}
		else
		{
			++it;
		}
	}
	myActiveSprites.shrink_to_fit();
}

const std::vector<std::shared_ptr<VFX>> VFXManager::GetVFXVector()
{
	return myActiveSprites;
}

void VFXManager::Render()
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::GraphicsStateStack& graphicsStateStack = DreamEngine::Engine::GetInstance()->GetGraphicsEngine().GetGraphicsStateStack();
	graphicsStateStack.Push();
	{
		graphicsStateStack.SetBlendState(DreamEngine::BlendState::AlphaBlend);
		graphicsStateStack.SetAlphaTestThreshold(0.4f);

		DreamEngine::SpriteDrawer& drawer(engine.GetGraphicsEngine().GetSpriteDrawer());

		for (int i = 0; i < myActiveSprites.size(); i++)
		{
			myActiveSprites[i]->Render(drawer);
		}
	}
	graphicsStateStack.Pop();
}

void VFXManager::Receive(const Message& aMsg)
{
	if (aMsg.messageType == eMessageType::PlayerStopVFX)
	{
		for (size_t i = 0; i < myActiveSprites.size(); i++)
		{
			myActiveSprites[i]->Stop();
		}
		myActiveSprites.clear();
	}
}
