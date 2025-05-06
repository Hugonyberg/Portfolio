#include "HUDElement.h"

#include <DreamEngine/engine.h>
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/graphics/SpriteDrawer.h>
#include <DreamEngine/graphics/TextureManager.h>

#include "MainSingleton.h"
#include <string>
#include <DreamEngine/windows/settings.h>

HUDElement::HUDElement(UIData& someData, const int& aID, const DreamEngine::Vector2ui& aScreenSize, eHUDAction aHUDAction)
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::Vector2ui intResolution = engine.GetRenderSize();
	myInitScreenSize = {(float)intResolution.x, (float)intResolution.y};
	myHUDAction = aHUDAction;

	if (myHUDAction == eHUDAction::ScreenCenterText)
		return;

	std::wstring filePath = someData.spritePaths[aID];
	filePath = DreamEngine::Settings::ResolveAssetPathW(filePath); 
	myActiveSpriteData.myTexture = engine.GetTextureManager().TryGetTexture(filePath.c_str());
	switch (aHUDAction) 
	{
		case eHUDAction::EnemySpawn1: 
		{
			myInactiveSpriteData.myTexture = engine.GetTextureManager().TryGetTexture(DE::Settings::ResolveAssetPathW(L"2D/S_UI_Enemypoint1Inactive.png").c_str());
			break;
		}
		case eHUDAction::EnemySpawn2: 
		{
			myInactiveSpriteData.myTexture = engine.GetTextureManager().TryGetTexture(DE::Settings::ResolveAssetPathW(L"2D/S_UI_Enemypoint2Inactive.png").c_str());
			break;
		}
		case eHUDAction::EnemySpawn3: 
		{
			myInactiveSpriteData.myTexture = engine.GetTextureManager().TryGetTexture(DE::Settings::ResolveAssetPathW(L"2D/S_UI_Enemypoint3Inactive.png").c_str());
			break;
		}
		case eHUDAction::CompanionTurretCommand: 
		{
			myInactiveSpriteData.myTexture = engine.GetTextureManager().TryGetTexture(DE::Settings::ResolveAssetPathW(L"2D/S_UI_CompanionTurretCooldown.png").c_str());
			break;
		}
		case eHUDAction::CompanionHealthCommand: 
		{
			myInactiveSpriteData.myTexture = engine.GetTextureManager().TryGetTexture(DE::Settings::ResolveAssetPathW(L"2D/S_UI_CompanionHealthCooldown.png").c_str());
			break;
		}
		case eHUDAction::GrapplingHookIndicator: 
		{
			myInactiveSpriteData.myTexture = engine.GetTextureManager().TryGetTexture(DE::Settings::ResolveAssetPathW(L"2D/S_UI_GrappleInactive.png").c_str());
			break;
		}
		default:
		{
			break;
		}
	}

	myScreenScale.x = static_cast<float>(myActiveSpriteData.myTexture->CalculateTextureSize().x) / static_cast<float>(aScreenSize.x); 
	myScreenScale.y = static_cast<float>(myActiveSpriteData.myTexture->CalculateTextureSize().y) / static_cast<float>(aScreenSize.y); 

	mySpriteInstance.myPivot = someData.uIData[aID].pivot;
	mySpriteInstance.myPosition = someData.uIData[aID].position;
	mySpriteInstance.myPosition += myInitScreenSize * 0.5f;
	mySpriteInstance.mySize.x = someData.uIData[aID].scale.x * myScreenScale.x * myInitScreenSize.x;
	mySpriteInstance.mySize.y = someData.uIData[aID].scale.y * myScreenScale.y * myInitScreenSize.y;

	mySize.x = mySpriteInstance.mySize.x;
	mySize.y = mySpriteInstance.mySize.y;
	auto colour = mySpriteInstance.myColor;
	mySpriteInstance.myColor = DreamEngine::Color(1.0f, 1.0f, 1.0f);
}

HUDElement::~HUDElement()
{
	MainSingleton::GetInstance()->GetPostMaster().Unsubscribe(myMessageType, this);
}

void HUDElement::Init()
{
	SetSubscribeState(myHUDAction); 
}

void HUDElement::Update(float aDeltaTime)
{
	if(!MainSingleton::GetInstance()->GetInGame()) { return; }

	if (myHUDAction == eHUDAction::ScreenCenterText)
	{
		if (!myActiveTimer.IsDone())
		{
			myActiveTimer.Update(aDeltaTime);
			if (myActiveTimer.IsDone() && (myMessageType == eMessageType::TriggerMissionAccomplished || myMessageType == eMessageType::TriggerMissionFailed))
			{
				MainSingleton::GetInstance()->SetShouldRenderCursor(true);
				MainSingleton::GetInstance()->SetShouldCaptureCursor(false);
			}

			float firstValue = myShouldLerpDown ? 1.0f : 0.0f;
			float secondValue = myShouldLerpDown ? 0.0f : 1.0f;
			float lerpedAlpha = UtilityFunctions::Lerp(firstValue, secondValue, myLerpTimer.GetCurrentValue() / myLerpTimer.GetResetValue());
			myTexts.back().SetColor(DE::Color(myTexts.back().GetColor().myR, myTexts.back().GetColor().myG, myTexts.back().GetColor().myB, lerpedAlpha));

			myLerpTimer.Update(aDeltaTime);
			if (myLerpTimer.IsDone())
			{
				myShouldLerpDown = !myShouldLerpDown;
				myLerpTimer.Reset();
			}
		}
	}
}

void HUDElement::Render()
{
	if(myShouldRender)
	{
		auto& engine = *DreamEngine::Engine::GetInstance();
		DreamEngine::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());

		if (myIsTextElement) 
		{
			if (myHUDAction == eHUDAction::ScreenCenterText && myActiveTimer.IsDone())
			{
				return;
			}
			for (auto& text : myTexts) 
			{
				text.Render();
			}
		}
		else 
		{
			if (myIsActive) 
			{
				spriteDrawer.Draw(myActiveSpriteData, mySpriteInstance);
			}
			else 
			{
				spriteDrawer.Draw(myInactiveSpriteData, mySpriteInstance);
			}
		}
	}
}

bool HUDElement::IsHovered()
{
	auto& input = MainSingleton::GetInstance()->GetInputManager();

	DreamEngine::Vector2ui windowSize = DreamEngine::Engine::GetInstance()->GetWindowSize();
	DreamEngine::Vector2f resolution = (DreamEngine::Vector2f)DreamEngine::Engine::GetInstance()->GetRenderSize();
	float screenResizeModx = (float)windowSize.x / (float)myInitScreenSize.x;
	float screenResizeMody = (float)windowSize.y / (float)myInitScreenSize.y;

	mySize.x = mySpriteInstance.mySize.x * screenResizeModx;
	mySize.y = mySpriteInstance.mySize.y * screenResizeMody;

	myPos.x = mySpriteInstance.myPosition.x * screenResizeModx;
	myPos.y = mySpriteInstance.myPosition.y * screenResizeMody;

	float mousePosX = (float)input.GetMousePosition().x;
	float mousePosY = resolution.y * screenResizeMody - (float)input.GetMousePosition().y;

	if(mousePosX > (myPos.x - mySize.x / 2) && mousePosX < (myPos.x + mySize.x / 2))
	{
		if(mousePosY > (myPos.y - mySize.y / 2) && mousePosY < (myPos.y + mySize.y / 2))
		{
			bool activationMessageData = true;
			MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &activationMessageData, myMessageType });
			return true;
		}
	}

	bool deactivationMessageData = false;
	MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &deactivationMessageData, eMessageType::TogglePauseMenu });
	return false;
}

void HUDElement::SetSubscribeState(eHUDAction aHUDAction)
{
	auto& postMaster = MainSingleton::GetInstance()->GetPostMaster();

	switch(aHUDAction)
	{
		case eHUDAction::HealthForeground:
		{
			myShouldRender = true;
			postMaster.Subscribe(eMessageType::PlayerHealthChange, this);
			myOriginSizeX = mySpriteInstance.mySize.x;
			myOriginSizeY = mySpriteInstance.mySize.y;
			myOriginPosY = mySpriteInstance.myPosition.y - (myOriginSizeY / 2.0f);
			myOriginPosX = mySpriteInstance.myPosition.x - (myOriginSizeX / 2.0f);
			break;
		}
		case eHUDAction::MoneyCurrencyText:
		{
			myShouldRender = true;
			myIsTextElement = true;
			postMaster.Subscribe(eMessageType::PlayerCurrencyChange, this);

			myTexts.push_back(DreamEngine::Text(L"Text/Tektur-Bold.ttf", DreamEngine::FontSize_24));
			myTexts.back().SetText("0");
			myTexts.back().SetPosition(DreamEngine::Vector2f{ mySpriteInstance.myPosition.x + myTexts.back().GetWidth() / (2.0f * (float)myTexts.back().GetText().length()), mySpriteInstance.myPosition.y - (myTexts.back().GetHeight() * 0.25f) });
			myTexts.back().SetColor(DE::Color(1.0f, 1.0f, 1.0f));

			break;
		}
		case eHUDAction::GrapplingHookIndicator:
		{
			myShouldRender = true;
			postMaster.Subscribe(eMessageType::PlayerGrappleCooldownStateChanged, this);
			break;
		}
		case eHUDAction::AmmoForeground:
		{
			myShouldRender = true;
			postMaster.Subscribe(eMessageType::PlayerAmmoCountUpdated, this);
			myOriginSizeX = mySpriteInstance.mySize.x;
			myOriginSizeY = mySpriteInstance.mySize.y;
			myOriginPosY = mySpriteInstance.myPosition.y - (myOriginSizeY / 2.0f);
			myOriginPosX = mySpriteInstance.myPosition.x - (myOriginSizeX / 2.0f);
			break;
		}
		case eHUDAction::AmmoNumber:
		{
			myShouldRender = true;
			myIsTextElement = true;
			postMaster.Subscribe(eMessageType::PlayerAmmoCountUpdated, this);

			myTexts.push_back(DreamEngine::Text(L"Text/Tektur-Bold.ttf", DreamEngine::FontSize_24));
			myTexts.back().SetText("-/-");
			myTexts.back().SetPosition(DreamEngine::Vector2f{mySpriteInstance.myPosition.x - myTexts.back().GetWidth(), mySpriteInstance.myPosition.y - (myTexts.back().GetHeight() * 0.25f)});
			myTexts.back().SetColor(DE::Color(90.0f / 255.0f, 225.0f / 255.0f, 254.0f / 255.0f));

			break;
		}
		case eHUDAction::CompanionTurretCommand:
		{
			myShouldRender = true;
			postMaster.Subscribe(eMessageType::CompanionTurretCooldownToggle, this);
			break;
		}
		case eHUDAction::CompanionHealthCommand:
		{
			myShouldRender = true;
			postMaster.Subscribe(eMessageType::CompanionHealthCooldownToggle, this);
			break;
		}
		case eHUDAction::DefensePointA:
		{
			myShouldRender = true;
			myIsTextElement = true;
			postMaster.Subscribe(eMessageType::DefensePointAToggle, this);
			postMaster.Subscribe(eMessageType::DefensePointAHealthChanged, this);

			myTexts.push_back(DreamEngine::Text(L"Text/Tektur-Bold.ttf", DreamEngine::FontSize_16));
			myTexts.back().SetText("A");
			myTexts.back().SetPosition(DreamEngine::Vector2f{ mySpriteInstance.myPosition.x - (myTexts.back().GetWidth() * 0.5f), mySpriteInstance.myPosition.y - (myTexts.back().GetHeight() * 0.25f) });
			myTexts.back().SetColor(DE::Color(247.0f / 255.0f, 90.0f / 255.0f, 22.0f / 255.0f));

			myTexts.push_back(DreamEngine::Text(L"Text/Tektur-Bold.ttf", DreamEngine::FontSize_10));
			myTexts.back().SetText("100%");
			myTexts.back().SetPosition(DreamEngine::Vector2f{ mySpriteInstance.myPosition.x - (myTexts.back().GetWidth() * 0.5f), mySpriteInstance.myPosition.y - (myTexts.back().GetHeight() * 0.25f) - 18.0f });
			myTexts.back().SetColor(DE::Color(247.0f / 255.0f, 90.0f / 255.0f, 22.0f / 255.0f));
			break;
		}
		case eHUDAction::DefensePointB:
		{
			myShouldRender = true;
			myIsTextElement = true;
			postMaster.Subscribe(eMessageType::DefensePointBToggle, this);
			postMaster.Subscribe(eMessageType::DefensePointBHealthChanged, this);

			myTexts.push_back(DreamEngine::Text(L"Text/Tektur-Bold.ttf", DreamEngine::FontSize_16));
			myTexts.back().SetText("B");
			myTexts.back().SetPosition(DreamEngine::Vector2f{ mySpriteInstance.myPosition.x - (myTexts.back().GetWidth() * 0.5f), mySpriteInstance.myPosition.y - (myTexts.back().GetHeight() * 0.25f) });
			myTexts.back().SetColor(DE::Color(20.0f / 255.0f, 157.0f / 255.0f, 255.0f / 255.0f));

			myTexts.push_back(DreamEngine::Text(L"Text/Tektur-Bold.ttf", DreamEngine::FontSize_10));
			myTexts.back().SetText("100%");
			myTexts.back().SetPosition(DreamEngine::Vector2f{ mySpriteInstance.myPosition.x - (myTexts.back().GetWidth() * 0.5f), mySpriteInstance.myPosition.y - (myTexts.back().GetHeight() * 0.25f) - 18.0f });
			myTexts.back().SetColor(DE::Color(20.0f / 255.0f, 157.0f / 255.0f, 255.0f / 255.0f));
			break;
		}
		case eHUDAction::DefensePointC:
		{
			myShouldRender = true;
			myIsTextElement = true;
			postMaster.Subscribe(eMessageType::DefensePointCToggle, this);
			postMaster.Subscribe(eMessageType::DefensePointCHealthChanged, this);

			myTexts.push_back(DreamEngine::Text(L"Text/Tektur-Bold.ttf", DreamEngine::FontSize_16));
			myTexts.back().SetText("C");
			myTexts.back().SetPosition(DreamEngine::Vector2f{ mySpriteInstance.myPosition.x - (myTexts.back().GetWidth() * 0.5f), mySpriteInstance.myPosition.y - (myTexts.back().GetHeight() * 0.25f) });
			myTexts.back().SetColor(DE::Color(20.0f / 255.0f, 255.0f / 255.0f, 98.0f / 255.0f));

			myTexts.push_back(DreamEngine::Text(L"Text/Tektur-Bold.ttf", DreamEngine::FontSize_10));
			myTexts.back().SetText("100%");
			myTexts.back().SetPosition(DreamEngine::Vector2f{ mySpriteInstance.myPosition.x - (myTexts.back().GetWidth() * 0.5f), mySpriteInstance.myPosition.y - (myTexts.back().GetHeight() * 0.25f) - 18.0f });
			myTexts.back().SetColor(DE::Color(20.0f/255.0f, 255.0f / 255.0f, 98.0f / 255.0f));
			break;
		}
		case eHUDAction::EnemySpawn1:
		{
			myShouldRender = true;
			postMaster.Subscribe(eMessageType::EnemySpawn1Toggle, this);
			break;
		}
		case eHUDAction::EnemySpawn2:
		{
			myShouldRender = true;
			postMaster.Subscribe(eMessageType::EnemySpawn2Toggle, this);
			break;
		}
		case eHUDAction::EnemySpawn3:
		{
			myShouldRender = true;
			postMaster.Subscribe(eMessageType::EnemySpawn3Toggle, this);
			break;
		}
		case eHUDAction::WaveNumberCounter:
		{
			myShouldRender = true;
			myIsTextElement = true;
			postMaster.Subscribe(eMessageType::WaveNumberChanged, this);

			myTexts.push_back(DreamEngine::Text(L"Text/Tektur-Bold.ttf", DreamEngine::FontSize_18));
			myTexts.back().SetText("WAVE 1/5");
			myTexts.back().SetPosition(DreamEngine::Vector2f{ mySpriteInstance.myPosition.x - (myTexts.back().GetWidth() * 0.5f), mySpriteInstance.myPosition.y - (myTexts.back().GetHeight() * 0.25f) });
			myTexts.back().SetColor(DE::Color(1.0f, 1.0f, 1.0f));
			break;
		}
		case eHUDAction::EnemyAmountCounter:
		{
			myShouldRender = true;
			myIsTextElement = true;
			postMaster.Subscribe(eMessageType::EnemyNumberChanged, this);

			myTexts.push_back(DreamEngine::Text(L"Text/Tektur-Bold.ttf", DreamEngine::FontSize_14));
			myTexts.back().SetText("ENEMIES: 0");
			myTexts.back().SetPosition(DreamEngine::Vector2f{ mySpriteInstance.myPosition.x - (myTexts.back().GetWidth() * 0.5f), mySpriteInstance.myPosition.y - (myTexts.back().GetHeight() * 0.25f) });
			myTexts.back().SetColor(DE::Color(1.0f, 1.0f, 1.0f));
			break;
		}
		case eHUDAction::Crosshair:
		{
			myShouldRender = true;
			postMaster.Subscribe(eMessageType::SetCrosshairState, this);
			break;
		}

		case eHUDAction::ScreenCenterText:
		{
			myShouldRender = true;
			myIsTextElement = true;
			postMaster.Subscribe(eMessageType::TriggerMissionAccomplished, this);
			postMaster.Subscribe(eMessageType::TriggerMissionFailed, this);
			postMaster.Subscribe(eMessageType::TriggerWaveIncoming, this);

			myTexts.push_back(DreamEngine::Text(L"Text/Tektur-Bold.ttf", DreamEngine::FontSize_48));
			myTexts.back().SetText("");
			myTexts.back().SetPosition(DreamEngine::Vector2f{ myInitScreenSize.x / 2.0f, myInitScreenSize.y / 2.0f });
			myTexts.back().SetColor(DE::Color(1.0f, 1.0f, 1.0f));

			myActiveTimer.SetResetValue(5.0f);
			break;
		}
	
		case eHUDAction::ControlsView:
		{
			myShouldRender = false;
			postMaster.Subscribe(eMessageType::TogglePauseMenu, this);
			break;
		}
	}
}

void HUDElement::Receive(const Message & aMsg)
{
	switch(aMsg.messageType)
	{
		case eMessageType::PlayerHealthChange:
		{
			if(myHUDAction == eHUDAction::HealthForeground)
			{
				auto* info = (std::pair<int, int>*)aMsg.messageData;
				ResizeImageHorizontally(info->first, info->second, false);
			}
			break;
		}
		case eMessageType::PlayerCurrencyChange:
		{
			if(myHUDAction == eHUDAction::MoneyCurrencyText)
			{
				auto* info = (int*)aMsg.messageData;
				myTexts.back().SetText(std::to_string(*info));
				myTexts.back().SetPosition(DreamEngine::Vector2f{ mySpriteInstance.myPosition.x + myTexts.back().GetWidth() / (2.0f * (float)myTexts.back().GetText().length()), mySpriteInstance.myPosition.y - (myTexts.back().GetHeight() * 0.25f) });
				MainSingleton::GetInstance()->GetAudioManager().StopAudio(eAudioEvent::PurchaseUpgrade);
				MainSingleton::GetInstance()->GetAudioManager().PlayAudio(eAudioEvent::PurchaseUpgrade, MainSingleton::GetInstance()->GetPlayer()->GetTransform()->GetPosition());
			}
			break;
		}
		case eMessageType::PlayerGrappleCooldownStateChanged:
		{
			if(myHUDAction == eHUDAction::GrapplingHookIndicator)
			{
				auto* info = (bool*)aMsg.messageData;
				myIsActive = *info;
			}
			break;
		}
		case eMessageType::PlayerAmmoCountUpdated:
		{
			if(myHUDAction == eHUDAction::AmmoForeground)
			{
				auto* info = (std::pair<int, int>*)aMsg.messageData;
				if (info->second != 0) 
				{
					ResizeImageHorizontally(info->first, info->second, true);
				}
				else 
				{
					ResizeImageHorizontally(1, 1, true);
				}
			}
			else if (myHUDAction == eHUDAction::AmmoNumber) 
			{
				auto* info = (std::pair<int, int>*)aMsg.messageData;
				if (info->second != 0) 
				{
					myTexts[0].SetText(std::to_string(info->first) + "/" + std::to_string(info->second));
					myTexts.back().SetPosition(DreamEngine::Vector2f{ mySpriteInstance.myPosition.x - myTexts.back().GetWidth(), mySpriteInstance.myPosition.y - (myTexts.back().GetHeight() * 0.25f) });
				}
				else 
				{
					myTexts[0].SetText("-/-");
					myTexts.back().SetPosition(DreamEngine::Vector2f{ mySpriteInstance.myPosition.x - myTexts.back().GetWidth(), mySpriteInstance.myPosition.y - (myTexts.back().GetHeight() * 0.25f) });
				}
			}
			break;
		}
		case eMessageType::DefensePointAToggle:
		{
			if(myHUDAction == eHUDAction::DefensePointA)
			{
				auto* info = (bool*)aMsg.messageData;
				if (*info) 
				{
					for (auto& text : myTexts)
						text.SetColor(DE::Color(247.0f / 255.0f, 90.0f / 255.0f, 22.0f / 255.0f));
				}
				else 
				{
					for (auto& text : myTexts)
						text.SetColor(DE::Color(67.0f / 255.0f, 67.0f / 255.0f, 67.0f / 255.0f));
				}
			}
			break;
		}
		case eMessageType::DefensePointBToggle:
		{
			if(myHUDAction == eHUDAction::DefensePointB)
			{
				auto* info = (bool*)aMsg.messageData;
				if (*info) 
				{
					for(auto& text : myTexts)
						text.SetColor(DE::Color(20.0f / 255.0f, 157.0f / 255.0f, 255.0f / 255.0f));
				}
				else 
				{
					for (auto& text : myTexts)
						text.SetColor(DE::Color(67.0f / 255.0f, 67.0f / 255.0f, 67.0f / 255.0f));
				}
			}
			break;
		}
		case eMessageType::DefensePointCToggle:
		{
			if(myHUDAction == eHUDAction::DefensePointC)
			{
				auto* info = (bool*)aMsg.messageData;
				if (*info) 
				{
					for (auto& text : myTexts)
						text.SetColor(DE::Color(20.0f / 255.0f, 255.0f / 255.0f, 98.0f / 255.0f));
				}
				else 
				{
					for (auto& text : myTexts)
						text.SetColor(DE::Color(67.0f / 255.0f, 67.0f / 255.0f, 67.0f / 255.0f));
				}
			}
			break;
		}
		case eMessageType::DefensePointAHealthChanged:
		case eMessageType::DefensePointBHealthChanged:
		case eMessageType::DefensePointCHealthChanged:
		{
			if(myHUDAction == eHUDAction::DefensePointA || myHUDAction == eHUDAction::DefensePointB || myHUDAction == eHUDAction::DefensePointC)
			{
				auto* info = (std::pair<float, float>*)aMsg.messageData;
				myTexts.back().SetText(std::to_string((int)(info->first / info->second * 100.0f)) + "%");
				myTexts.back().SetPosition(DreamEngine::Vector2f{ mySpriteInstance.myPosition.x - (myTexts.back().GetWidth() * 0.5f), mySpriteInstance.myPosition.y - (myTexts.back().GetHeight() * 0.25f) - 18.0f });
			}
			break;
		}
		case eMessageType::EnemySpawn1Toggle:
		case eMessageType::EnemySpawn2Toggle:
		case eMessageType::EnemySpawn3Toggle:
		{
			if (myHUDAction == eHUDAction::EnemySpawn1 || myHUDAction == eHUDAction::EnemySpawn2 || myHUDAction == eHUDAction::EnemySpawn3)
			{
				auto* info = (bool*)aMsg.messageData;
				myIsActive = *info;
			}
			break;
		}
		case eMessageType::WaveNumberChanged:
		{
			if (myHUDAction == eHUDAction::WaveNumberCounter)
			{
				auto* info = (int*)aMsg.messageData;
				myTexts.back().SetText("Wave " + std::to_string(*info) + "/5");
				myTexts.back().SetPosition(DreamEngine::Vector2f{ mySpriteInstance.myPosition.x - (myTexts.back().GetWidth() * 0.5f), mySpriteInstance.myPosition.y - (myTexts.back().GetHeight() * 0.25f) });
			}
			break;
		}
		case eMessageType::EnemyNumberChanged:
		{
			if (myHUDAction == eHUDAction::EnemyAmountCounter)
			{
				auto* info = (int*)aMsg.messageData;
				myTexts.back().SetText("Enemies: " + std::to_string(*info));
				myTexts.back().SetPosition(DreamEngine::Vector2f{ mySpriteInstance.myPosition.x - (myTexts.back().GetWidth() * 0.5f), mySpriteInstance.myPosition.y - (myTexts.back().GetHeight() * 0.25f) });
			}
			break;
		}
		case eMessageType::CompanionTurretCooldownToggle:
		{
			if (myHUDAction == eHUDAction::CompanionTurretCommand)
			{
				auto* info = (bool*)aMsg.messageData;
				myIsActive = *info;
			}
			break;
		}
		case eMessageType::CompanionHealthCooldownToggle:
		{
			if (myHUDAction == eHUDAction::CompanionHealthCommand)
			{
				auto* info = (bool*)aMsg.messageData;
				myIsActive = *info;
			}
			break;
		}
		case eMessageType::SetCrosshairState:
		{
			if (myHUDAction == eHUDAction::Crosshair)
			{
				auto* info = (bool*)aMsg.messageData;
				mySpriteInstance.myColor = *info ? DE::Color(252.0f / 255.0f, 193.0f / 255.0f, 26.0f / 255.0f) : DE::Color(1.0f, 1.0f, 1.0f);
			}
			break;
		}
		case eMessageType::TogglePauseMenu:
		{
			auto* info = static_cast<bool*>(aMsg.messageData);
			myShouldRender = *info;
			if (MainSingleton::GetInstance()->GetInGame())
			{
				MainSingleton::GetInstance()->SetShouldRenderCursor(myShouldRender);
				MainSingleton::GetInstance()->SetShouldCaptureCursor(!myShouldRender);
			}

			break;
		}
		case eMessageType::LevelOne:
		{
			myShouldRender = false;
			break;
		}
		case eMessageType::LevelTwo:
		{
			myShouldRender = false;
			break;
		}
		case eMessageType::PlayerGymScene:
		{
			myShouldRender = false;
			break;
		}
		case eMessageType::PlayerDied:
		{
			myShouldRender = false;
			break;
		}
		case eMessageType::Reset:
		{
			break;
		}
		case eMessageType::TriggerMissionAccomplished:
		{
			if (myActiveTimer.IsDone())
			{
				myMessageType = eMessageType::TriggerMissionAccomplished;
				myTexts.back().SetText("MISSION ACCOMPLISHED");
				myTexts.back().SetColor(DE::Color(0.0f, 1.0f, 0.0f));
				myTexts.back().SetPosition(DreamEngine::Vector2f{ myInitScreenSize.x / 2.0f - myTexts.back().GetWidth() / 2.0f, myInitScreenSize.y * 0.65f });
				myActiveTimer.Reset();
			}
			break;
		}
		case eMessageType::TriggerMissionFailed:
		{
			if (myActiveTimer.IsDone())
			{
				myMessageType = eMessageType::TriggerMissionFailed;
				myTexts.back().SetText("MISSION FAILED");
				myTexts.back().SetColor(DE::Color(1.0f, 0.0f, 0.0f));
				myTexts.back().SetPosition(DreamEngine::Vector2f{ myInitScreenSize.x / 2.0f - myTexts.back().GetWidth() / 2.0f, myInitScreenSize.y * 0.65f });
				myActiveTimer.Reset();
			}
			break;
		}
		case eMessageType::TriggerWaveIncoming:
		{
			if (myActiveTimer.IsDone())
			{
				myMessageType = eMessageType::TriggerWaveIncoming;
				myTexts.back().SetText("NEXT WAVE INCOMING");
				myTexts.back().SetColor(DE::Color(1.0f, 1.0f, 1.0f));
				myTexts.back().SetPosition(DreamEngine::Vector2f{ myInitScreenSize.x / 2.0f - myTexts.back().GetWidth() / 2.0f, myInitScreenSize.y * 0.65f });
				myActiveTimer.Reset();
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void HUDElement::ResizeImageVertically(int aCurrentAmount, int aMaxAmount)
{
	float imageDiv = (float)aCurrentAmount / (float)aMaxAmount;
	mySpriteInstance.mySize.y = myOriginSizeY * imageDiv;
	mySpriteInstance.myPosition.y = myOriginPosY + ((myOriginSizeY / 2.0f) * imageDiv);
	mySpriteInstance.myTextureRect.myStartY = 1.0f - mySpriteInstance.mySize.y / myOriginSizeY;
}

void HUDElement::ResizeImageHorizontally(int aCurrentAmount, int aMaxAmount, bool aShrinkToTheLeft)
{
	if (aShrinkToTheLeft) 
	{
		float imageDiv = (float)aCurrentAmount / (float)aMaxAmount;
		mySpriteInstance.mySize.x = myOriginSizeX * imageDiv;
		mySpriteInstance.myPosition.x = myOriginPosX + ((myOriginSizeX / 2.0f) * imageDiv);
		mySpriteInstance.myTextureRect.myEndX = mySpriteInstance.mySize.x / myOriginSizeX;
	}
	else 
	{
		float imageDiv = (float)aCurrentAmount / (float)aMaxAmount;
		mySpriteInstance.mySize.x = myOriginSizeX * imageDiv;
		mySpriteInstance.myPosition.x = myOriginPosX + myOriginSizeX - ((myOriginSizeX / 2.0f) * imageDiv);
		mySpriteInstance.myTextureRect.myStartX = 1.0f - mySpriteInstance.mySize.x / myOriginSizeX;
	}
}