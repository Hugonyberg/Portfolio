#include "DebuggingCamera.h"
#include "MainSingleton.h"
#include <DreamEngine/engine.h> 

DebuggingCamera::DebuggingCamera()
{
}

void DebuggingCamera::FreeCam(float aDeltaTime, DreamEngine::Camera* aCamera)
{
#ifdef _RETAIL
	aCamera;
	aDeltaTime;
#endif // _RETAIL

#ifndef _RETAIL

	static float camspeed = .1f;
	static float rotspeed = 4.f;
	static bool myIsTrapped = false;

	auto& input = MainSingleton::GetInstance()->GetInputManager(); 

	float camSpeed = 50000.f * aDeltaTime * camspeed;
	float camRotSpeed = 3.f * aDeltaTime * rotspeed;

	DreamEngine::Vector3f camMovement = DreamEngine::Vector3f::Zero;
	DreamEngine::Rotator camRotation = DreamEngine::Rotator::Zero;
	if(myIsTrapped)
	{
		if(input.IsKeyHeld(DreamEngine::eKeyCode::W))
		{
			camMovement += aCamera->GetTransform().GetMatrix().GetForward() * 100.0f;
		}
		if(input.IsKeyHeld(DreamEngine::eKeyCode::S))
		{
			camMovement += aCamera->GetTransform().GetMatrix().GetForward() * -100.0f;
		}
		if(input.IsKeyHeld(DreamEngine::eKeyCode::A))
		{
			camMovement += aCamera->GetTransform().GetMatrix().GetRight() * -100.0f;
		}
		if(input.IsKeyHeld(DreamEngine::eKeyCode::D))
		{
			camMovement += aCamera->GetTransform().GetMatrix().GetRight() * 100.0f;
		}
		if(input.IsKeyHeld(DreamEngine::eKeyCode::Space))
		{
			camMovement += aCamera->GetTransform().GetMatrix().GetUp() * 100.0f;
		}
		if(input.IsKeyHeld(DreamEngine::eKeyCode::Ctrl))
		{
			camMovement += aCamera->GetTransform().GetMatrix().GetUp() * -100.0f;
		}

		aCamera->SetPosition(aCamera->GetTransform().GetPosition() + camMovement * camSpeed * aDeltaTime);

		const DreamEngine::Vector2f mouseDelta = {(float)input.GetMouseDelta().x, (float)input.GetMouseDelta().y};

		camRotation.X = mouseDelta.Y;
		camRotation.Y = mouseDelta.X;

		aCamera->GetTransform().AddRotation(camRotation * camRotSpeed);

		aCamera->SetRotation(aCamera->GetTransform().GetRotation());
	}

	if(input.IsKeyDown(DreamEngine::eKeyCode::RightMouseButton))
	{
		// Capture mouse.
		if(!myIsTrapped)
		{
			input.HideMouse();
			input.CaptureMouse();
			myIsTrapped = true;
		}
	}

	if(input.IsKeyUp(DreamEngine::eKeyCode::RightMouseButton))
	{
		// When we let go, recapture.
		if(myIsTrapped)
		{
			input.ShowMouse();
			input.ReleaseMouse();
			myIsTrapped = false;
		}
	}

	if(input.IsKeyDown(DreamEngine::eKeyCode::Shift))
	{
		// When we hold shift, "sprint".
		camspeed *= 4;
	}

	if(input.IsKeyUp(DreamEngine::eKeyCode::Shift))
	{
		// When we let go, "walk".
		camspeed /= 4;
	}
#endif
}
