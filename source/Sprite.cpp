#include "Sprite.h"
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/graphics/SpriteDrawer.h>
#include <DreamEngine/graphics/TextureManager.h>
#include <DreamEngine/engine.h> 
#include "MainSingleton.h"

Sprite::Sprite(UIData& someData, const int& aID, const DreamEngine::Vector2ui& aScreenSize)
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::Vector2ui intResolution = engine.GetRenderSize(); 
	DreamEngine::Vector2f resolution = {(float)intResolution.x, (float)intResolution.y}; 

	myFilePath = someData.spritePaths[aID];
	mySprite.myTexture = engine.GetTextureManager().GetTexture(myFilePath.c_str());

	myScreenScale.x = static_cast<float>(mySprite.myTexture->CalculateTextureSize().x) / static_cast<float>(aScreenSize.x);
	myScreenScale.y = static_cast<float>(mySprite.myTexture->CalculateTextureSize().y) / static_cast<float>(aScreenSize.y);

	mySpriteInstance.myPivot = someData.uIData[aID].pivot;
	mySpriteInstance.myPosition = someData.uIData[aID].position;
	mySpriteInstance.myPosition += resolution * 0.5f;
	mySpriteInstance.mySize.x = someData.uIData[aID].scale.x * myScreenScale.x * resolution.x;
	mySpriteInstance.mySize.y = someData.uIData[aID].scale.y * myScreenScale.y * resolution.y;

	mySpriteInstance.myColor = DreamEngine::Color(1.0f, 1.0f, 1.0f);
}

void Sprite::SetCustomTexture(std::wstring aTexturePath, DE::Vector2f aPosition, DE::Vector2f aScale, const DreamEngine::Vector2ui& aScreenSize)
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::Vector2ui intResolution = engine.GetRenderSize();
	DreamEngine::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };

	myFilePath = aTexturePath;
	mySprite.myTexture = engine.GetTextureManager().GetTexture(myFilePath.c_str());

	myScreenScale.x = static_cast<float>(mySprite.myTexture->CalculateTextureSize().x) / static_cast<float>(aScreenSize.x);
	myScreenScale.y = static_cast<float>(mySprite.myTexture->CalculateTextureSize().y) / static_cast<float>(aScreenSize.y);

	mySpriteInstance.myPivot = DE::Vector2f(0.5f);
	mySpriteInstance.myPosition = aPosition;
	mySpriteInstance.mySize.x = aScale.x * myScreenScale.x * resolution.x;
	mySpriteInstance.mySize.y = aScale.y * myScreenScale.y * resolution.y;

	mySpriteInstance.myColor = DreamEngine::Color(1.0f, 1.0f, 1.0f);
}

void Sprite::Init()
{
	if(myFilePath == L"2D/ControlsView.png")
	{
		MainSingleton::GetInstance()->GetPostMaster().Subscribe(eMessageType::TogglePauseMenu, this);
		myShouldRender = false;
	}
	else if (myFilePath == L"2D/S_UI_MenuPopupBackdrop.png" || myFilePath == L"2D/S_UI_GuideText.png")
	{
		auto& postMaster = MainSingleton::GetInstance()->GetPostMaster();
		postMaster.Subscribe(eMessageType::ActivateIntroMenu, this);
		postMaster.Subscribe(eMessageType::DeactivateIntroMenu, this);
		postMaster.Subscribe(eMessageType::ResetIntroMenu, this);
		myShouldRender = false;
	}
}

void Sprite::Render()
{
	if(!myShouldRender) { return; }

	auto& engine = *DreamEngine::Engine::GetInstance(); 
	DreamEngine::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());

	spriteDrawer.Draw(mySprite, mySpriteInstance); 
}

void Sprite::Receive(const Message& aMsg)
{
	if(aMsg.messageType == eMessageType::TogglePauseMenu)
	{
		myShouldRender = aMsg.messageData; 
	}
	else if(aMsg.messageType == eMessageType::ActivateIntroMenu)
	{
		myShouldRender = true; 
	}
	else if(aMsg.messageType == eMessageType::DeactivateIntroMenu)
	{
		myShouldRender = false; 
	}
	else if(aMsg.messageType == eMessageType::ResetIntroMenu)
	{
		myShouldRender = false; 
	}
}