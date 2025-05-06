#pragma once
#include "Message.h"
#include <vector>
#include <utility>
#include <DreamEngine/math/vector2.h>

enum class eInputAction
{
	MouseMove,
	WASD,
	LeftShift,
	Ctrl,
	Spacebar,
	Q,
	MouseLeftClickOrHeld,
	MouseLeftReleased,
	MouseRightClick,
	MouseScroll,
	R,
	E,
	C,
	V,
	One,
	Two,
	Three,
	Escape,
	GamepadRightStick,
	GamepadLeftStick,
	GamepadWestButton,
	GamepadSouthButton,
	GamepadNorthButton,
	GamepadEastButton,
	GamepadRightTrigger,
	GamepadRightTriggerReleased,
	GamepadRightShoulderButton,
	GamepadLeftShoulderButton,
	GamepadDpadNorthSouth,
	GamepadDpadWest,
	GamepadDpadEast,
	GamepadStart,
	DefaultTypeAndCount
};

class InputMapper
{
public:
	InputMapper();
	void Update();
	bool CheckKeyboardInputs();
	bool CheckMouseInputs();
	void CheckGamepadInputs(bool tookMouseInput);
	void MapActionToMessage(eInputAction anInputAction, eMessageType aMessageType);

private:
	void TriggerActions();
	std::vector<std::pair<eInputAction, DE::Vector2i>> myActionsTriggeredThisFrame;
	std::vector<std::pair<eInputAction, eMessageType>> myMappedActionMessages;
	bool rightTriggerWasDownLastFrame = false;
};