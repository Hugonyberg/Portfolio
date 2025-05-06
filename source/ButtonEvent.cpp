#include "ButtonEvent.h"
#include "MainSingleton.h"

#include <DreamEngine/engine.h>
#include <DreamEngine/graphics/GraphicsEngine.h>

ButtonEvent::ButtonEvent()
{}

ButtonEvent::~ButtonEvent()
{
	auto& postMaster = MainSingleton::GetInstance()->GetPostMaster();
	postMaster.Unsubscribe(myMessageType, this);
}

void ButtonEvent::Init(eUIButton aState)
{
	auto& postMaster = MainSingleton::GetInstance()->GetPostMaster();

	switch(aState)
	{
	case eUIButton::ScreenSizeFull:
	{
		myMessageType = eMessageType::FullScreen; 
		break;
	}
	case eUIButton::Resolution1280x720:
	{
		myMessageType = eMessageType::Resolution1280x720;
		break;
	}
	case eUIButton::Resolution1920x1080:
	{
		myMessageType = eMessageType::Resolution1920x1080;
		break;
	}
	case eUIButton::Resolution2560x1440:
	{
		myMessageType = eMessageType::Resolution2560x1440;
		break;
	}
	default:
	{
#ifndef _RETAIL
		std::cout << "Did not find the correct ButtonAction in ButtonOnPress" << std::endl;
#endif
		break;
	}
	}

	postMaster.Subscribe(myMessageType, this); 
}

void ButtonEvent::Receive(const Message & aMsg)
{
	auto engine = DreamEngine::Engine::GetInstance();

	switch(aMsg.messageType) 
	{
	case eMessageType::Resolution1280x720:
	{
		MainSingleton::GetInstance()->SetResolution({1280,720});
		MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ nullptr, eMessageType::ChangeResolution});

		break;
	}
	case eMessageType::Resolution1920x1080:
	{
		MainSingleton::GetInstance()->SetResolution({ 1920,1080 });
		MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ nullptr, eMessageType::ChangeResolution });

		break;
	}
	case eMessageType::Resolution2560x1440:
	{
		MainSingleton::GetInstance()->SetResolution({ 2560,1440 });
		MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ nullptr, eMessageType::ChangeResolution });

		break;
	}
	//case eMessageType::FullScreen:
	//{
	//	auto resolution = MainSingleton::GetInstance()->GetResolution(); 
	//	MainSingleton::GetInstance()->SetResolution({ resolution });

	//	auto tempBool = MainSingleton::GetInstance()->GetFullScreenBool();
	//	tempBool = !tempBool; 

	//	MainSingleton::GetInstance()->SetFullScreenBool(tempBool);

	//	engine->GetGraphicsEngine().SetFullScreen(tempBool);
	//	break;
	//}
	default:
	break;
	}

	//RECT desktop;
	//// Get a handle to the desktop window
	//const HWND hDesktop = GetDesktopWindow();
	//// Get the size of screen to the variable desktop
	//GetWindowRect(hDesktop, &desktop);

	/*POINT fullScreenSize{ desktop.right, desktop.bottom };
	auto tempBool = MainSingleton::GetInstance()->GetFullScreenBool();
	
	auto resolution = MainSingleton::GetInstance()->GetResolution();

	if (tempBool)
	{
		engine->SetResolution(resolution, tempBool);
	}*/

	int width = (int) MainSingleton::GetInstance()->GetScreenSize().x;
	int hight = (int) MainSingleton::GetInstance()->GetScreenSize().y;

	int xPos = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - hight) / 2;
	SetWindowPos(GetActiveWindow(), 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}
