#pragma once
#include <DreamEngine/graphics/Camera.h> 

class DebuggingCamera
{
public:

	DebuggingCamera();

	void FreeCam(float aDeltaTime, DreamEngine::Camera* aCamera);

private:
};

