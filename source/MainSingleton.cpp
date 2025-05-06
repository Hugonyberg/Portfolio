#include "MainSingleton.h"
#include <DreamEngine\engine.h>
#include <DreamEngine\graphics\TextureManager.h>
#include <DreamEngine\graphics\sprite.h>
MainSingleton* MainSingleton::myInstance = nullptr;

bool MainSingleton::Start()
{
	if (!myInstance)
	{
		myInstance = new MainSingleton();
		return myInstance->InternalStart();
	}
	else
	{
		return false;
	}
}

void MainSingleton::SwapModelBuffers()
{
	myRenderCommands = myLogicCommands;/*
	myInstancedRenderCommands = myInstancedLogicCommands;*/
}

bool MainSingleton::InternalStart()
{
	myLastID = 0;
	auto* engine = DE::Engine::GetInstance();

	myMinimapSprites[0].myTexture = engine->GetTextureManager().GetTexture(L"2D/S_UI_MiniMapBG.png");
	myMinimapSprites[1].myTexture = engine->GetTextureManager().GetTexture(L"2D/S_UI_MiniMapBoat.png");
	myMinimapSprites[2].myTexture = engine->GetTextureManager().GetTexture(L"2D/S_UI_MiniMapArrow.png");
	myMinimapSprites[3].myTexture = engine->GetTextureManager().GetTexture(L"2D/S_UI_MiniMapKeyItem.png");
	myMinimapSprites[4].myTexture = engine->GetTextureManager().GetTexture(L"2D/S_UI_MiniMapSurvivor	.png");
	myMinimapSprites[5].myTexture = engine->GetTextureManager().GetTexture(L"2D/S_UI_MiniMapBorder.png");
	myMinimapSprites[6].myTexture = engine->GetTextureManager().GetTexture(L"2D/S_UI_MiniMapBlip.png");	
	return true;
}