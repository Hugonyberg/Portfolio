#pragma once
#include <vector>
#include <memory>
#include <DreamEngine/graphics/ModelInstance.h>
#include <DreamEngine/graphics/AnimatedModelInstance.h>
#include <DreamEngine/graphics/AnimationPlayer.h>

namespace DreamEngine
{
	class ModelShader;
	class GraphicsStateStack;
	class Texture;
}

enum class eStaticVFXType
{
	Laser = 0,
	GrapplingHook = 1,
	MuzzleFlash = 2,
	WaterPlane = 3,
};

enum class eAnimatedVFXType
{
	Fresnel = 0,
	PlayerShaderGun = 1,
	FresnelRed = 2,
	FresnelBlue = 3,
	FresnelGreen = 4,
};

class ShaderTool
{
public:
	ShaderTool();
	~ShaderTool();
	bool LoadShaders();
	void Update(float aDeltaTime);
	DreamEngine::ModelShader* GetModelShader(int anIndex);
	DreamEngine::ModelInstance GetModelInstances(int anIndex);
	void DrawShaderTool(int anIndex);
	void DrawShaderToolAnimated(int anIndex);
	void SetVFXActive(int aIndex, bool aShouldRender);
	void SetAnimatedVFXActive(int aIndex, bool aShouldRender);
	void Render(DE::Matrix4x4f aJoinPos1, DE::Vector3f aJoinPos2, DE::Vector3f aHandJointPos, DE::Vector3f aTargetPos, DreamEngine::GraphicsStateStack& aGraphicsStateStack);
	void Render(DreamEngine::GraphicsStateStack& aGraphicsStateStack, float aTimer, std::shared_ptr<DreamEngine::AnimatedModelInstance> aAnimatedModel, int aAnimatedShaderIndex, int aCurrentAmmoType = 0);
	void Render(DreamEngine::GraphicsStateStack& aGraphicsStateStack, float aTimer, DreamEngine::ModelInstance aModel, int aShaderIndex);
	void RenderWaterPlanes(DreamEngine::GraphicsStateStack& aGraphicsStateStack);
	void RenderUnityLoadedModels(DreamEngine::GraphicsStateStack& aGraphicsStateStack);
	void SetPlayerPos(DE::Vector3f aPlayerPos);
	void SetPlayerDirection(DE::Vector3f aPlayerDir);

	void SetWaterPlanePos(DE::Vector3f aBoatPos);
	void AddModelInstance(DreamEngine::ModelInstance aVFXModelInstance, std::vector<DE::Texture*> someTextures);
	void AddShaderInstance(DreamEngine::ModelShader* aModelShader);

private:
	std::vector<DreamEngine::ModelInstance> myModelInstances;
	std::vector<DreamEngine::ModelInstance> myUnityModelInstances;
	std::vector<DreamEngine::ModelInstance> myWaterPlaneModelInstances;
	std::vector<std::shared_ptr<DreamEngine::AnimatedModelInstance>> myAnimatedModelInstances;
	std::vector<DreamEngine::ModelShader*> myModelShaders;
	std::vector<DreamEngine::ModelShader*> myUnityModelShaders;
	std::vector<DreamEngine::ModelShader*> myAnimatedModelShaders;
	std::vector<DE::Texture*> myUnityTextures;
	DE::AnimationPlayer myDrillAnimationPlayer;
	std::vector<bool> myShouldRenderStaticVFX;
	std::vector<bool> myShouldRenderAnimatedVFX;
	DE::Vector3f myPlayerPos;
	DE::Vector3f myPlayerDir;
	eStaticVFXType myStaticVFXType;
	eAnimatedVFXType myAnimatedVFXType;
	DE::Texture* myGunTextureOne;
	DE::Texture* myGunTextureTwo;
	DE::Texture* myGunTextureThree;

	DE::Texture* myLaserTextureOne;
	DE::Texture* myLaserTextureTwo;

	DE::Texture* myGrapplingTextureOne;
	DE::Texture* myGrapplingTextureTwo;

	DE::Texture* myMuzzleFlashTextureOne;
	DE::Texture* myMuzzleFlashTextureTwo;

	DE::Texture* myVolumetricLightTextureOne;
	DE::Texture* myVolumetricLightTextureTwo;

	std::vector<DE::TextureResource> myGunOriginalTextures;
	float myTotalVFXTimer;
	int myWaterGridSize;
	float myWaterPlaneSize;
	bool myLoadedLaserTextures;
	bool myLoadedGrapplingTextures;
};