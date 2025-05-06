#include "State.h"
#include <DreamEngine/engine.h>

void State::ExitState()
{
	auto& engine = *DreamEngine::Engine::GetInstance();

	engine.EndFrame();
	engine.Shutdown();
	exit(0);
}
