#include "stdafx.h"
#include "GameWorld.h"
#include "MainSingleton.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <DreamEngine/ThreadPool.h>
#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/Wwise_IDs.h>
#include "WwiseHandle.h"

#ifndef _DEBUG
#define AK_OPTIMIZED
#define AK_DISABLE_MEMDEBUG
//#define AK_MEMDEBUG
#endif

static bool allowCursorCapture = true;

static UINT globalMessage;
std::atomic<bool> lock;
std::atomic<bool> syncDone = false;
std::atomic<bool> gameReady = false;

void Go(void);

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
//{
//	Go();
//	return 0;
//}

int main(const int /*argc*/, const char* /*argc*/[])
{
	Go();
	return 0;
}

LRESULT WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (MainSingleton::GetInstance()->GetInputManager().UpdateEvents(message, wParam))
	{
		return 0;
	}
	globalMessage = message;
	switch (message)
	{
		// this message is read when the window is closed
		case WM_DESTROY:
		{
			// close the application entirely
			PostQuitMessage(0);
			return 0;
		}
	}

	return 0;
}

void CallUpdate(GameWorld* aGameWorld)
{
	DreamEngine::Engine& engine = *DreamEngine::Engine::GetInstance();
	while (aGameWorld->GameShouldQuit() == false)
	{
		if (syncDone == true)
		{
			syncDone = false;
			aGameWorld->Update(engine.GetDeltaTime());
			gameReady = true;
		}
		if (aGameWorld->GameShouldQuit())
		{
			break;
		}
		if (globalMessage == WM_NCDESTROY)
		{
			break;
		}
	}
}

void Go()
{
	DreamEngine::LoadSettings(TGE_PROJECT_SETTINGS_FILE);

	DreamEngine::EngineConfiguration winconf;
	
	winconf.myApplicationName = L"Echoes in the Murk";
	winconf.myWinProcCallback = [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {return WinProc(hWnd, message, wParam, lParam); };
#ifdef _DEBUG
	winconf.myActivateDebugSystems = DreamEngine::DebugFeature::Fps | DreamEngine::DebugFeature::Mem | DreamEngine::DebugFeature::Filewatcher | DreamEngine::DebugFeature::Cpu | DreamEngine::DebugFeature::Drawcalls | DreamEngine::DebugFeature::OptimizeWarnings;
	winconf.myActivateDebugSystems = DreamEngine::DebugFeature::Cpu;
#else
	winconf.myActivateDebugSystems = DreamEngine::DebugFeature::Filewatcher;
#endif

	if (!DreamEngine::Engine::Start(winconf))
	{
		ERROR_PRINT("Fatal error! Engine could not start!");
		system("pause");
		return;
	}

	if (!MainSingleton::Start())
	{
		system("pause");
		return;
	}
	
	{
		GameWorld gameWorld;

#pragma region Wwise
		WwiseHandle wwiseHandle;
		wwiseHandle.Init();
		AKRESULT res;
		AkBankID outInitBankId;
		res = AK::SoundEngine::LoadBank("Init.bnk", outInitBankId);
		AkBankID outGeneralBankId;
		res = AK::SoundEngine::LoadBank("TestForSDKBank.bnk", outGeneralBankId);
		AK::SoundEngine::SetOutputVolume(0, 0.75f);
		AK::SoundEngine::AddDefaultListener(0);
		AK::SoundEngine::RegisterGameObj(0, "DefaultObject");
#pragma endregion

		DreamEngine::Engine& engine = *DreamEngine::Engine::GetInstance();
		gameWorld.Init();

		bool changeRes = false;
		std::thread logicThread(&CallUpdate, &gameWorld);

		auto screenSize = DE::Vector2ui(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		auto defaultRes = DE::Vector2ui(1920, 1080);
		engine.SetResolution(defaultRes, screenSize);
		MainSingleton::GetInstance()->SetResolution(defaultRes);
		MainSingleton::GetInstance()->SetScreenSize(screenSize);
		MainSingleton::GetInstance()->GetInputManager().SetWindowHandle(*DE::Engine::GetInstance()->GetHWND());
		MainSingleton::GetInstance()->GetInputManager().ConfineCursorToWindow();

#ifdef _RETAIL
		MainSingleton::GetInstance()->GetInputManager().HideMouse();
#endif
		while (engine.BeginFrame())
		{
			#ifndef _RETAIL // Toggle for Mouse Handling during gameplay
			if (MainSingleton::GetInstance()->GetInputManager().IsKeyDown(DE::eKeyCode::NumLockMinus) || MainSingleton::GetInstance()->GetInputManager().IsKeyDown(DE::eKeyCode::P))
			{
				allowCursorCapture = !allowCursorCapture;
			}
			#endif // _DEBUG

			if(changeRes == true)
			{
				gameWorld.ResetResolutionBool();
				changeRes = false;
			}
			
			if(gameWorld.GameShouldChangeResolution() == true)
			{
				MainSingleton::GetInstance()->GetInputManager().ReleaseCursorFromWindow();
				engine.SetResolution(MainSingleton::GetInstance()->GetResolution(), MainSingleton::GetInstance()->GetScreenSize());
				MainSingleton::GetInstance()->GetInputManager().ConfineCursorToWindow();
				changeRes = true;
			}

			engine.SetFullScreen(false);

			MainSingleton::GetInstance()->GetInputManager().Update();
			syncDone = true;
			gameWorld.Render();
			AK::SoundEngine::RenderAudio();

			while (!gameReady); // Wait for UpdateLoop

			engine.EndFrame();

			gameReady = false;
			if (gameWorld.GameShouldQuit())
			{
				break;
			}
			MainSingleton::GetInstance()->SetWasActiveWindowLastFrame(GetForegroundWindow() == *engine.GetHWND());
		}
		logicThread.join();
		wwiseHandle.DeInit();
	}

	DreamEngine::Engine::GetInstance()->Shutdown();
}