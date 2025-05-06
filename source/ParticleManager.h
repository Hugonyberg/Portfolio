#pragma once
#include "ParticleSystem.h"
#include "ParticleSystem3D.h"
#include "DreamEngine/graphics/SpriteDrawer.h"
#include <vector>
#include "DreamEngine/math/Transform.h"

class ParticleSystem;
class ParticleManager
{
public:
	ParticleManager();
	~ParticleManager();
	void Add2D(ParticleSystem* aParicleSystem);
	void AddDebug2D(ParticleSystem aParicleSystem);
	void Add3D(ParticleSystem3D* aParicleSystem);
	void AddDebug3D(ParticleSystem3D aParicleSystem);
	void Clear();
	void Render(DreamEngine::SpriteDrawer& aSpriteDrawer);
	void RenderToGBuffer(DreamEngine::SpriteDrawer& aSpriteDrawer);
	void Update(float aDeltaTime);
	void SetCameraTransform(DreamEngine::Transform* aCameraTransform);
	void SetActionCamera(bool aActionCamera);
	void SetWorldScale(float aScale);
	void SetPlayerPosition(DE::Vector3f aPlayerPos);

#ifndef _RETAIL
	void ImGui2DDebugMenu();
	void ImGui3DDebugMenu();
#endif // !_RETAIL
private:
	std::vector<ParticleSystem*> my2DParticleSystems;
	std::vector<ParticleSystem> my2DDebugParticleSystems;
	std::vector<ParticleSystem3D*> my3DParticleSystems;
	std::vector<ParticleSystem3D> my3DDebugParticleSystems;
	std::array<char[50], 10> myParticleSystemNames;
	float myWorldScale;
	int myCurrentParticleSystem;
	int mySizeOf2DParticleSystems;
	int mySizeOf3DParticleSystems;
	bool myPaticle2DDebugMode;
	bool myPaticle3DDebugMode;
	bool myActionCamera;
	bool myShouldResetParticleSystem;
	DreamEngine::Transform* myCameraTransform;
	DE::Vector3f myPlayerPosition;
};

