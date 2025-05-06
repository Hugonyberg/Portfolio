#include "Mouse.h"
#include "MainSingleton.h"
#include <DreamEngine/graphics/TextureManager.h>
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/graphics/SpriteDrawer.h>
#include <DreamEngine/graphics/DX11.h>
#include <DreamEngine/graphics/DepthBuffer.h>
#include <DreamEngine/engine.h>

Mouse::Mouse()
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	mySprite.myTexture = engine.GetTextureManager().GetTexture(L"/2D/Mouse.png");	//can be wrong
	mySpriteInstance.myPivot = {0.0f, 0.0f};
	mySpriteInstance.mySize = mySprite.myTexture->CalculateTextureSize();

	#ifndef _RETAIL
	ShowCursor(true);
	#endif // !_RETAIL
}

void Mouse::Update()
{
	auto& input = MainSingleton::GetInstance()->GetInputManager();
	bool fullScreen = MainSingleton::GetInstance()->GetFullScreenBool();
	DreamEngine::Vector2f screenSize = {(float)MainSingleton::GetInstance()->GetScreenSize().x,(float)MainSingleton::GetInstance()->GetScreenSize().y}; 

	DreamEngine::Engine& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::Vector2f resolution = {(float)engine.GetRenderSize().x, (float)engine.GetRenderSize().y};
	DreamEngine::Vector2f initialSize = {(float)engine.GetTargetSize().x, (float)engine.GetTargetSize().y};
	DreamEngine::Vector2f windowSize = {(float)engine.GetWindowSize().x, (float)engine.GetWindowSize().y};
	DreamEngine::Vector2f screenResizeMod = {windowSize.x / initialSize.x,windowSize.y / initialSize.y};

	DreamEngine::Vector2f mouse = {(float)input.GetMousePosition().x, (float)input.GetMousePosition().y};

	if(windowSize == initialSize)
	{
		mouse = {mouse.x , resolution.y - mouse.y};
	}
	else
	{
		mouse = {mouse.x /= screenResizeMod.x, mouse.y /= screenResizeMod.y};

		if(fullScreen)
		{
			RECT desktopRect;
			if(GetWindowRect(GetDesktopWindow(), &desktopRect))
			{
				float mouseScaleY = (float)desktopRect.bottom - (float)desktopRect.top;

				mouse.y = mouseScaleY / (mouseScaleY / resolution.y) - mouse.y;
			}
		}
		else
		{
			mouse = {mouse.x, resolution.y - mouse.y};
		}
	}

	mySpriteInstance.myPosition = {mouse};
	MainSingleton::GetInstance()->SetMousePos(mySpriteInstance.myPosition);
}

void Mouse::Render()
{
#ifndef _DEBUG
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());

	auto& graphicStateStack = DreamEngine::Engine::GetInstance()->GetGraphicsEngine().GetGraphicsStateStack();
	graphicStateStack.SetAlphaTestThreshold(0.05f);
	graphicStateStack.SetBlendState(DreamEngine::BlendState::AlphaBlend);

	DreamEngine::DX11::ourDepthBuffer->Clear(); 

	spriteDrawer.Draw(mySprite, mySpriteInstance); 
#endif
}