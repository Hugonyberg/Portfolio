#include "InputMapper.h"
#include <DreamEngine\utilities\InputManager.h>
#include "Message.h"
#include <DreamEngine/math/vector2.h>
#include "MainSingleton.h"

InputMapper::InputMapper()
{
	MapActionToMessage(eInputAction::MouseMove,				eMessageType::PlayerLookAround);
	MapActionToMessage(eInputAction::WASD,					eMessageType::PlayerMove);
	MapActionToMessage(eInputAction::LeftShift,				eMessageType::PlayerSprint);
	MapActionToMessage(eInputAction::Spacebar,				eMessageType::PlayerJump);
	//MapActionToMessage(eInputAction::Ctrl,					eMessageType::PlayerGroundSlide);
	//MapActionToMessage(eInputAction::Q,						eMessageType::PlayerUseGrapplingHook);
	//MapActionToMessage(eInputAction::MouseRightClick,		eMessageType::PlayerUseGrapplingHook);
	//MapActionToMessage(eInputAction::MouseLeftClickOrHeld,	eMessageType::PlayerShoot);
	//MapActionToMessage(eInputAction::MouseLeftReleased,eMessageType::PlayerShootReleased);
	//MapActionToMessage(eInputAction::MouseScroll,			eMessageType::PlayerAmmoSwap);
	//MapActionToMessage(eInputAction::R,						eMessageType::PlayerReload);
	MapActionToMessage(eInputAction::E,						eMessageType::PlayerInteract);
	//MapActionToMessage(eInputAction::C,						eMessageType::CompanionTurret);
	//MapActionToMessage(eInputAction::V,						eMessageType::CompanionFetch);
	//MapActionToMessage(eInputAction::One,					eMessageType::PlayerSelectBeam);
	//MapActionToMessage(eInputAction::Two,					eMessageType::PlayerSelectRocket);
	//MapActionToMessage(eInputAction::Three,					eMessageType::PlayerSelectGoo);
	//MapActionToMessage(eInputAction::Escape,				eMessageType::TogglePauseMenu);

	MapActionToMessage(eInputAction::GamepadRightStick,		eMessageType::PlayerLookAround);
	MapActionToMessage(eInputAction::GamepadLeftStick,		eMessageType::PlayerMove);
	MapActionToMessage(eInputAction::GamepadWestButton,				eMessageType::PlayerSprint);
	MapActionToMessage(eInputAction::GamepadSouthButton,				eMessageType::PlayerJump);
	//MapActionToMessage(eInputAction::GamepadLeftShoulderButton,		eMessageType::PlayerUseGrapplingHook);
	//MapActionToMessage(eInputAction::GamepadEastButton,				eMessageType::PlayerGroundSlide);
	//MapActionToMessage(eInputAction::GamepadRightTrigger,	eMessageType::PlayerShoot);
	//MapActionToMessage(eInputAction::GamepadRightTriggerReleased,	eMessageType::PlayerShootReleased);
	//MapActionToMessage(eInputAction::GamepadDpadNorthSouth,		eMessageType::PlayerAmmoSwap);
	//MapActionToMessage(eInputAction::GamepadRightShoulderButton,	eMessageType::PlayerReload);
	MapActionToMessage(eInputAction::GamepadNorthButton,				eMessageType::PlayerInteract);
	//MapActionToMessage(eInputAction::GamepadDpadWest,		eMessageType::CompanionTurret);
	//MapActionToMessage(eInputAction::GamepadDpadEast,		eMessageType::CompanionFetch);
	//MapActionToMessage(eInputAction::GamepadStart,			eMessageType::TogglePauseMenu);
}

void InputMapper::Update()
{
	myActionsTriggeredThisFrame.clear();

	bool tookMouseInput = CheckMouseInputs();
	bool tookKeyboardInput = CheckKeyboardInputs();
	if (!tookKeyboardInput)
	{
		CheckGamepadInputs(tookMouseInput);
	}

	TriggerActions();
}

bool InputMapper::CheckKeyboardInputs()
{
	bool tookInput = false;
	auto& inputManager = MainSingleton::GetInstance()->GetInputManager();

	DE::Vector2i wasdResult;
	if (inputManager.IsKeyHeld(DreamEngine::eKeyCode::W) || inputManager.IsKeyDown(DreamEngine::eKeyCode::W))
	{
		wasdResult.y += 10000;
	}
	if (inputManager.IsKeyHeld(DreamEngine::eKeyCode::S) || inputManager.IsKeyDown(DreamEngine::eKeyCode::S))
	{
		wasdResult.y -= 10000;
	}
	if (inputManager.IsKeyHeld(DreamEngine::eKeyCode::A) || inputManager.IsKeyDown(DreamEngine::eKeyCode::A))
	{
		wasdResult.x -= 10000;
	}
	if (inputManager.IsKeyHeld(DreamEngine::eKeyCode::D) || inputManager.IsKeyDown(DreamEngine::eKeyCode::D))
	{
		wasdResult.x += 10000;
	}
	if (wasdResult.x != 0 || wasdResult.y != 0)
	{
		wasdResult = wasdResult / 100;
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::WASD, wasdResult));
		tookInput = true;
	}

	if (inputManager.IsKeyDown(DreamEngine::eKeyCode::Shift))
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::LeftShift, DE::Vector2i()));
		tookInput = true;
	}
	if (inputManager.IsKeyDown(DreamEngine::eKeyCode::Ctrl) || inputManager.IsKeyHeld(DE::eKeyCode::Ctrl))
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::Ctrl, DE::Vector2i()));
		tookInput = true;
	}
	if (inputManager.IsKeyDown(DreamEngine::eKeyCode::Space))
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::Spacebar, DE::Vector2i()));
		tookInput = true;
	}
	if (inputManager.IsKeyDown(DreamEngine::eKeyCode::Q))
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::Q, DE::Vector2i()));
		tookInput = true;
	}
	if (inputManager.IsKeyDown(DreamEngine::eKeyCode::R))
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::R, DE::Vector2i()));
		tookInput = true;
	}
	if (inputManager.IsKeyDown(DreamEngine::eKeyCode::E))
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::E, DE::Vector2i()));
		tookInput = true;
	}
	if(inputManager.IsKeyDown(DreamEngine::eKeyCode::C))
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::C, DE::Vector2i()));
		tookInput = true;
	}
	if(inputManager.IsKeyDown(DreamEngine::eKeyCode::V))
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::V, DE::Vector2i()));
		tookInput = true;
	}
	if(inputManager.IsKeyDown(DreamEngine::eKeyCode::One))
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::One, DE::Vector2i()));
		tookInput = true;
	}
	if(inputManager.IsKeyDown(DreamEngine::eKeyCode::Two))
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::Two, DE::Vector2i()));
		tookInput = true;
	}
	if(inputManager.IsKeyDown(DreamEngine::eKeyCode::Three))
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::Three, DE::Vector2i()));
		tookInput = true;
	}
	if (inputManager.IsKeyDown(DreamEngine::eKeyCode::Escape))
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::Escape, DE::Vector2i()));
		tookInput = true;
	}

	return tookInput;
}

bool InputMapper::CheckMouseInputs()
{
	bool tookInput = false;
	auto& inputManager = MainSingleton::GetInstance()->GetInputManager();

	if ((inputManager.GetMouseDelta().x != 0 || inputManager.GetMouseDelta().y != 0) && !MainSingleton::GetInstance()->IsInCutscene())
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::MouseMove, DE::Vector2i(static_cast<int>(inputManager.GetMouseDelta().x), static_cast<int>(inputManager.GetMouseDelta().y))));
		tookInput = true;
	}

	if (inputManager.IsKeyDown(DreamEngine::eKeyCode::LeftMouseButton))
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::MouseLeftClickOrHeld, DE::Vector2i(0, 0)));
		tookInput = true;
	}
	else if (inputManager.IsKeyHeld(DreamEngine::eKeyCode::LeftMouseButton))
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::MouseLeftClickOrHeld, DE::Vector2i(1, 0)));
		tookInput = true;
	}
	else if (inputManager.IsKeyUp(DreamEngine::eKeyCode::LeftMouseButton))
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::MouseLeftReleased, DE::Vector2i()));
		tookInput = true;
	}
	if (inputManager.IsKeyDown(DreamEngine::eKeyCode::RightMouseButton))
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::MouseRightClick, DE::Vector2i()));
		tookInput = true;
	}

	if (inputManager.GetMouseScrollChange() != 0.0f)
	{
		myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::MouseScroll, DE::Vector2i(static_cast<int>(inputManager.GetMouseScrollChange()), 0)));
		tookInput = true;
	}

	return tookInput;
}

void InputMapper::CheckGamepadInputs(bool tookMouseInput)
{
	auto& inputManager = MainSingleton::GetInstance()->GetInputManager();
	std::vector<SDL_JoystickID> gamepadIds;
	inputManager.GetConnectedGamepadIds(gamepadIds);
	for (auto& gamepadId : gamepadIds) 
	{
		DE::Vector2i rightStickResult;
		if (!tookMouseInput && inputManager.GetRightThumbstickXAxis(gamepadId) != 0)
		{
			rightStickResult.x = inputManager.GetRightThumbstickXAxis(gamepadId);
		}
		if (!tookMouseInput && inputManager.GetRightThumbstickYAxis(gamepadId) != 0)
		{
			rightStickResult.y = -inputManager.GetRightThumbstickYAxis(gamepadId);
		}
		if (rightStickResult.x != 0 || rightStickResult.y != 0)
		{
			rightStickResult = rightStickResult / 10000.0f * 3;
			myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::GamepadRightStick, rightStickResult));
		}

		DE::Vector2i leftStickResult;
		if (inputManager.GetLeftThumbstickXAxis(gamepadId) != 0)
		{
			leftStickResult.x = inputManager.GetLeftThumbstickXAxis(gamepadId);
		}
		if (inputManager.GetLeftThumbstickYAxis(gamepadId) != 0)
		{
			leftStickResult.y = inputManager.GetLeftThumbstickYAxis(gamepadId);
		}
		if (leftStickResult.x != 0 || leftStickResult.y != 0)
		{
			DE::Vector2f fResult = DE::Vector2f(static_cast<float>(leftStickResult.x), static_cast<float>(leftStickResult.y)).GetNormalized() * 100.0f;
			leftStickResult = DE::Vector2i(static_cast<int>(fResult.x), static_cast<int>(fResult.y));
			myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::GamepadLeftStick, leftStickResult));
		}

		if (inputManager.IsGamepadButtonDown(gamepadId, DE::eGamepadButton::WestButton))
		{
			myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::GamepadWestButton, DE::Vector2i()));
		}
		if (inputManager.IsGamepadButtonDown(gamepadId, DE::eGamepadButton::SouthButton))
		{
			myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::GamepadSouthButton, DE::Vector2i()));
		}
		if (inputManager.IsGamepadButtonDown(gamepadId, DE::eGamepadButton::NorthButton))
		{
			myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::GamepadNorthButton, DE::Vector2i()));
		}
		if (inputManager.IsGamepadButtonDown(gamepadId, DE::eGamepadButton::EastButton) || inputManager.IsGamepadButtonHeld(gamepadId, DE::eGamepadButton::EastButton))
		{
			myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::GamepadEastButton, DE::Vector2i()));
		}

		if (!tookMouseInput && inputManager.GetRightTriggerValue(gamepadId) != 0)
		{
			if (rightTriggerWasDownLastFrame) 
			{
				myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::GamepadRightTrigger, DE::Vector2i(inputManager.GetRightTriggerValue(gamepadId), 0)));
			}
			else 
			{
				myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::GamepadRightTrigger, DE::Vector2i(0, 0)));
			}
			rightTriggerWasDownLastFrame = true;
		}
		else if (!tookMouseInput)
		{
			myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::GamepadRightTriggerReleased, DE::Vector2i()));
			rightTriggerWasDownLastFrame = false;
		}
		else 
		{
			rightTriggerWasDownLastFrame = false;
		}
		if (!tookMouseInput && inputManager.IsGamepadButtonDown(gamepadId, DE::eGamepadButton::RightShoulderButton))
		{
			myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::GamepadRightShoulderButton, DE::Vector2i()));
		}
		if (!tookMouseInput && inputManager.IsGamepadButtonDown(gamepadId, DE::eGamepadButton::LeftShoulderButton))
		{
			myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::GamepadLeftShoulderButton, DE::Vector2i()));
		}

		if (!tookMouseInput && inputManager.IsGamepadButtonDown(gamepadId, DE::eGamepadButton::DPadNorth))
		{
			myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::GamepadDpadNorthSouth, DE::Vector2i(1, 0)));
		}
		else if (!tookMouseInput && inputManager.IsGamepadButtonDown(gamepadId, DE::eGamepadButton::DPadSouth))
		{
			myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::GamepadDpadNorthSouth, DE::Vector2i(-1, 0)));
		}

		if (!tookMouseInput && inputManager.IsGamepadButtonDown(gamepadId, DE::eGamepadButton::DPadWest))
		{
			myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::GamepadDpadWest, DE::Vector2i()));
		}
		else if (!tookMouseInput && inputManager.IsGamepadButtonDown(gamepadId, DE::eGamepadButton::DPadEast))
		{
			myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::GamepadDpadEast, DE::Vector2i()));
		}

		if (inputManager.IsGamepadButtonDown(gamepadId, DE::eGamepadButton::Start))
		{
			myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::GamepadStart, DE::Vector2i(1, 0)));
		}
	}

	//// SDL
	//DE::Vector2i sdlRightStickResult;
	//if (!tookMouseInput && inputManager.GetSDLRightThumbstickXAxis(2) != 0)
	//{
	//	sdlRightStickResult.x = inputManager.GetSDLRightThumbstickXAxis(2);
	//}
	//if (!tookMouseInput && inputManager.GetSDLRightThumbstickYAxis(2) != 0)
	//{
	//	sdlRightStickResult.y = -inputManager.GetSDLRightThumbstickYAxis(2);
	//}
	//if (sdlRightStickResult.x != 0 || sdlRightStickResult.y != 0)
	//{
	//	sdlRightStickResult = sdlRightStickResult / 10000.0f * 3;
	//	myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::SDLGamepadRightStick, sdlRightStickResult));
	//}

	//DE::Vector2i sdlLeftStickResult;
	//if (inputManager.GetSDLLeftThumbstickXAxis(2) != 0)
	//{
	//	sdlLeftStickResult.x = inputManager.GetSDLLeftThumbstickXAxis(2);
	//}
	//if (inputManager.GetSDLLeftThumbstickYAxis(2) != 0)
	//{
	//	sdlLeftStickResult.y = inputManager.GetSDLLeftThumbstickYAxis(2);
	//}
	//if (sdlLeftStickResult.x != 0 || sdlLeftStickResult.y != 0)
	//{
	//	DE::Vector2f fResult = DE::Vector2f(static_cast<float>(sdlLeftStickResult.x), static_cast<float>(sdlLeftStickResult.y)).GetNormalized() * 100.0f;
	//	sdlLeftStickResult = DE::Vector2i(static_cast<int>(fResult.x), static_cast<int>(fResult.y));
	//	myActionsTriggeredThisFrame.push_back(std::pair<eInputAction, DE::Vector2i>(eInputAction::SDLGamepadLeftStick, sdlLeftStickResult));
	//}
}

void InputMapper::TriggerActions()
{
	for (int messagesIndex = 0; messagesIndex < myMappedActionMessages.size(); messagesIndex++)
	{
		for (int actionsIndex = 0; actionsIndex < myActionsTriggeredThisFrame.size(); actionsIndex++)
		{
			if (myMappedActionMessages[messagesIndex].first == myActionsTriggeredThisFrame[actionsIndex].first)
			{
				MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &myActionsTriggeredThisFrame[actionsIndex].second, myMappedActionMessages[messagesIndex].second });
			}
		}
	}
}

void InputMapper::MapActionToMessage(eInputAction anInputAction, eMessageType aMessageType)
{
	myMappedActionMessages.push_back(std::pair<eInputAction, eMessageType>(anInputAction, aMessageType));
}