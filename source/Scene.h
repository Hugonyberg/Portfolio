#pragma once
#include <DreamEngine/graphics/Camera.h>
#include <DreamEngine/graphics/ModelInstance.h>
#include <DreamEngine/graphics/ModelInstancer.h>
#include <DreamEngine/graphics/DX11.h>
#include <DreamEngine/graphics/FullscreenEffect.h> 
#include <DreamEngine\graphics\GeometryBuffer.h>

#include "UnityLoader.h"
#include "MainSingleton.h"
#include "Observer.h"
#include "ObjectLoader.h" 
#include "HUDLoader.h"

class GameObject;
class Scene: public Observer 
{
public:
	Scene(LevelData& aLevelData, int aLevelIndex);
	~Scene();

	void Init(); 
	void Scene::InitLight(); 
	void Update(float aDeltaTime);
	void Render();

	void Receive(const Message& aMsg) override; 
	int GetLevelID();
	std::shared_ptr<DreamEngine::Camera> GetCamera();

private:
	void RenderBloomAndColorCorrection(DreamEngine::GraphicsStateStack& aGStateStack);
	void RenderVinjett(DreamEngine::GraphicsStateStack& aGStateStack);
	void RenderDamageEffect(DreamEngine::GraphicsStateStack& aGStateStack);
	void RenderHealingEffect(DreamEngine::GraphicsStateStack& aGStateStack);
	void RenderHUDEffect(DreamEngine::GraphicsStateStack& aGStateStack);
	void RenderToBackBuffer(DreamEngine::GraphicsStateStack& aGStateStack);
	void RenderFogEffect(DreamEngine::GraphicsStateStack& aGStateStack);

	float myBossCameraOffset = -800.f;  //todo remove?
	int myCurrentIndex;
	std::shared_ptr<DreamEngine::Camera> myMainCamera;
	float myFoV = 90.0f;
	float myNearPlane = 10.0f;
	float myFarPlane = 10000.0f;
	DreamEngine::Vector3f myCameraOffset;
	DreamEngine::RenderTarget* myBackBuffer;

	DE::Matrix4x4f myWaterConfig{};

	std::shared_ptr<DreamEngine::AmbientLight> myAmbientLight;
	DreamEngine::Vector3f myDirectionalLightRotation;
	float myFogDensity;
	float mySaturation;
	float myBloom;
	float myVignette;
	float myExposure;
	DreamEngine::DirectionalLight myDirectionalLight;

	DreamEngine::DepthBuffer myShadowBuffer;
	DreamEngine::GeometryBuffer myGeometryBuffer;
	DreamEngine::Camera myShadowCam; 
	float myShadowMapScale;
	float myShadowCameraNearFarPlane;

	DreamEngine::Vector3f myContrast;
	DreamEngine::Vector3f myBlackPoint;
	DreamEngine::Vector3f myTint;
	int myCurrentLevel;

	std::unique_ptr<DreamEngine::FullscreenEffect> myFullscreenDownScale;
	std::unique_ptr<DreamEngine::FullscreenEffect> myFullscreenUpScale;
	std::unique_ptr<DreamEngine::FullscreenEffect> myFullscreenColorCorrection;
	std::unique_ptr<DreamEngine::FullscreenEffect> myFullscreenVinjett;
	std::unique_ptr<DreamEngine::FullscreenEffect> myFullscreenGBufferLightingPass;
	std::unique_ptr<DreamEngine::FullscreenEffect> myFullscreenGBufferRenderPass;
	std::unique_ptr<DreamEngine::FullscreenEffect> myFullscreenBackBufferRender;
	std::unique_ptr<DreamEngine::FullscreenEffect> myFullscreenDamageEffect;
	std::unique_ptr<DreamEngine::FullscreenEffect> myFullscreenHealingEffect;
	std::unique_ptr<DreamEngine::FullscreenEffect> myFullscreenBareHUDEffect;
	std::unique_ptr<DreamEngine::FullscreenEffect> myFullscreenFogEffect;

	DE::Texture* myDamageTextureOne;
	DE::Texture* myDamageTextureTwo;
	DE::Texture* myDamageTextureThree;
	DE::Texture* myDamageTextureFour;

	DE::Texture* myHealingTextureOne;
	DE::Texture* myHealingTextureTwo;
	DE::Texture* myHealingTextureThree;
	DE::Texture* myHealingTextureFour;
	DE::Texture* myHealingTextureFive;

	DE::Texture* myBareHUDTextureOne;
	DE::Texture* myBareHUDTextureTwo;
	DE::Texture* myBareHUDTextureThree;
	DE::Texture* myBareHUDTextureFour;

	DE::Texture* myTestTexture;
	DE::Texture* myTestTexture2;

	DE::Texture* myFogTexture;

	std::vector<DreamEngine::RenderTarget> myRenderTargets;
	DreamEngine::RenderTarget myPostColorCorrectionTarget;
	DreamEngine::RenderTarget myPostProcessHUDTarget;
	std::shared_ptr<ObjectLoader> myLoadedObjects;

	std::shared_ptr<Player> myPlayer;

	int myCurrentRenderPassNum = 1;
	float myCurrentAmbientFadeHeight = 8000.0f;

	float myWaterPanAlbedoX = 0.0f;
	float myWaterPanAlbedoY = 0.0f;
	float myWaterPanNormalX = 0.0f;
	float myWaterPanNormalY = 0.0f;

	DE::Vector4f myBareHudColor;
	DE::Vector4f myDamageHudColor;
	DE::Vector4f myHealingHudColor;
	std::vector<bool> myDrillLineOfSight;
	DreamEngine::Vector3f myFogColour;
	bool myShoudlRenderBareHud;
	bool myShoudlRenderDamageHud;
	bool myShoudlRenderHealingHud;
	bool myUseHardcodedDirectionalLight = false;
};

