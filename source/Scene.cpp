#include "Scene.h"
#include <DreamEngine/graphics/ModelFactory.h>
#include <DreamEngine/graphics/ModelInstance.h>
#include <DreamEngine/graphics/GraphicsEngine.h>
#include "DreamEngine/graphics/TextureManager.h"
#include <DreamEngine/graphics/GraphicsStateStack.h>
#include <DreamEngine/graphics/ModelDrawer.h>
#include <DreamEngine/graphics/RenderTarget.h>
#include <DreamEngine/graphics/DX11.h>
#include <DreamEngine/debugging/DebugDrawer.h> 
#include <DreamEngine/utilities/StringCast.h> 
#include <DreamEngine/engine.h>
#include <DreamEngine/windows/settings.h>

#include "MainSingleton.h"
#include "JsonSoreter.hpp"
#include "GameObject.h"
#include "ParticleManager.h"
#include "DebuggingCamera.h" 

#include <fstream>
#include <imgui\imgui.h>
#include "Player.h"
#include <mutex>

#include "RigidBodyComponent.h"
#include "ProjectilePool.h"
#include "Boat.h"

std::mutex threadSafe;

static ParticleManager particleManager;
static const int ShadowMapResolution = 2730; //Biggest possible resolution for most modern GPUs is 2730 

Scene::Scene(LevelData& aLevelData, int aLevelIndex)
{
	DE::Vector2f resolution = DE::Vector2f(MainSingleton::GetInstance()->GetResolution().x, MainSingleton::GetInstance()->GetResolution().y);
	DE::Vector2f screenSize = DE::Vector2f(MainSingleton::GetInstance()->GetScreenSize().x, MainSingleton::GetInstance()->GetScreenSize().y);
	myShoudlRenderBareHud = true;
	myShoudlRenderDamageHud = false;
	myShoudlRenderHealingHud = false;
	MainSingleton::GetInstance()->GetPostMaster().Subscribe(eMessageType::ChangeResolution, this);

	myBackBuffer = DreamEngine::DX11::ourBackBuffer;

	{
		myMainCamera = std::make_shared<DE::Camera>();
		myNearPlane = 1.0f;
		myFarPlane = aLevelData.mainCamera.farP * 100.f;
		myFoV = MainSingleton::GetInstance()->GetFOV();
		myMainCamera->SetPerspectiveProjection(myFoV, resolution, myNearPlane, myFarPlane);
		myMainCamera->SetTransform(aLevelData.mainCamera.transform);
	}

	particleManager.SetCameraTransform(&myMainCamera->GetTransform());
	if (myLoadedObjects != nullptr)
	{
		myLoadedObjects->ClearInstancersMap();
	}
	myLoadedObjects = std::make_shared<ObjectLoader>(aLevelData);
	myPlayer = myLoadedObjects->GetPlayer();
	myDirectionalLight = DreamEngine::DirectionalLight();

	myExposure = 0.0f;
	mySaturation = 1.0f;
	myContrast = { 1.0f,1.0f,1.0f };
	myBlackPoint = { 0.0f,0.0f,0.0f };
	myTint = { 1.0f,1.0f,1.0f };
	myBloom = 0.25f;
	myDirectionalLightRotation = DreamEngine::Rotator(215, 45, 0);

	myWaterPanAlbedoX = -0.010f;
	myWaterPanAlbedoY = -0.010f;

	myFogColour = {0.5f,0.5f,0.5f};
	myFogDensity = 0.5f;

	switch (aLevelIndex)
	{
		case 6:// lvl 1
		{			
			myExposure = 0.0f;
			mySaturation = 1.0f;
			myContrast = { 1.0f, 1.0f, 1.0f };
			myBlackPoint = { 0.0f, 0.0f, 0.0f };
			myTint = { 1.0f, 1.0f, 1.0f };
			myBloom = 0.25f;

			myFogColour = { 0.078f,0.443f,0.733f };
			myCurrentAmbientFadeHeight = 8000.0f;
			myUseHardcodedDirectionalLight = true;
			myDirectionalLight.SetColor({ 92.0f / 255.0f, 119.0f / 255.0f, 118.0f / 255.0f });
			myDirectionalLight.SetIntensity(1.0f);
			myDirectionalLight.SetTransform(
				{
					{ 0, 4550.0f ,0 },
					//DreamEngine::Rotator(320, 45, 0)
					DreamEngine::Rotator(153.109f,98.5f,190.1f)
				}
			);
			myDirectionalLightRotation = myDirectionalLight.GetTransform().GetRotation();

			break;
		}
		case 7: // lvl 2
		{
			myExposure = 0.0f;
			mySaturation = 1.0f;
			myContrast = { 1.0f, 1.0f, 1.0f };
			myBlackPoint = { 0.0f, 0.0f, 0.0f };
			myTint = { 1.0f, 1.0f, 1.0f };
			myBloom = 0.25f;

			myFogColour = { 0.815f,0.545f,0.27f };
			myCurrentAmbientFadeHeight = 8000.0f;
			myUseHardcodedDirectionalLight = true;
			myDirectionalLight.SetColor({ 125.0f / 255.0f, 130.0f / 255.0f, 96.0f / 255.0f });
			myDirectionalLight.SetIntensity(1.0f);
			myDirectionalLight.SetTransform(
				{
					{ 0, 4000.0f, 0 },
					//DreamEngine::Rotator(320, 45, 0)
					DreamEngine::Rotator(221.6f, 272.5f, 0.9f)
				}
			);
			myDirectionalLightRotation = myDirectionalLight.GetTransform().GetRotation();

			break;
		}
		case 8:// playergym
		{
			myExposure = 0.0f;
			mySaturation = 1.0f;
			myContrast = { 1.0f, 1.0f, 1.0f };
			myBlackPoint = { 0.0f, 0.0f, 0.0f };
			myTint = { 1.0f, 1.0f, 1.0f };
			myBloom = 0.25f;

			myUseHardcodedDirectionalLight = true;
			myDirectionalLight.SetColor({ 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f });
			myDirectionalLight.SetIntensity(1.0f);
			myDirectionalLight.SetTransform(
				{
					{ 0,0,0 },
					//DreamEngine::Rotator(320, 45, 0)
					DreamEngine::Rotator(132.595f, 110.595f, 180.0f)
				}
			);
			myDirectionalLightRotation = myDirectionalLight.GetTransform().GetRotation();

			break;
		}
		case 9:// ass gym
		{
			myExposure = 0.0f;
			mySaturation = 1.0f;
			myContrast = { 1.0f, 1.0f, 1.0f };
			myBlackPoint = { 0.0f, 0.0f, 0.0f };
			myTint = { 1.0f, 1.0f, 1.0f };
			myBloom = 0.25f;

			myUseHardcodedDirectionalLight = true;
			myDirectionalLight.SetColor({ 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f });
			myDirectionalLight.SetIntensity(1.0f);
			myDirectionalLight.SetTransform(
				{
					{ 0,0,0 },
					//DreamEngine::Rotator(320, 45, 0)
					DreamEngine::Rotator(170.595f, 145.3f, 203.7f)
				}
			);
			myDirectionalLightRotation = myDirectionalLight.GetTransform().GetRotation();

			break;
		}
	default:
		break;
	}

	particleManager.SetWorldScale(50);

#ifdef _EDITOR
	myEditor.SetEditorLevel(this);
#endif // _EDITOR
	myCurrentIndex = aLevelIndex;

	myCameraOffset = { 0.0f, 130.0f, 0.0f };


	myWaterConfig(1, 1) = 0.4f;		//Amplitude
	myWaterConfig(1, 2) = -2.0f;	//Speed
	myWaterConfig(1, 3) = 3.5f;		//Frequency

}

Scene::~Scene()
{
	MainSingleton::GetInstance()->GetPostMaster().Unsubscribe(eMessageType::ChangeResolution, this); 
	particleManager.Clear();
}

void Scene::Init()
{
	MainSingleton::GetInstance()->SetActiveCamera(myMainCamera);
	myLoadedObjects->Init();
	
	myBackBuffer = DreamEngine::DX11::ourBackBuffer;  

	myFullscreenDownScale = std::make_unique<DreamEngine::FullscreenEffect>();
	if(!myFullscreenDownScale->Init("Shaders/PostProcessDownSamplePS.cso"))
	{
		assert("Couldn't load DownScale!");
	}
	myFullscreenUpScale = std::make_unique<DreamEngine::FullscreenEffect>();
	if(!myFullscreenUpScale->Init("Shaders/PostProcessUpSamplePS.cso"))
	{
		assert("Couldn't load UpScale_PS!");
	}
	myFullscreenColorCorrection = std::make_unique<DreamEngine::FullscreenEffect>();
	if(!myFullscreenColorCorrection->Init("Shaders/PostProcessColorCorrectionPS.cso"))
	{
		assert("Couldn't load Color Correction!");
	}
	myFullscreenGBufferLightingPass = std::make_unique<DreamEngine::FullscreenEffect>();
	if (!myFullscreenGBufferLightingPass->Init("Shaders/GBufferLightingPassPS.cso"))
	{
		assert("Couldn't load GBuffer Lighting Pass!");
	}
	myFullscreenGBufferRenderPass = std::make_unique<DreamEngine::FullscreenEffect>();
	if (!myFullscreenGBufferRenderPass->Init("Shaders/GBufferRenderPassPS.cso"))
	{
		assert("Couldn't load GBuffer Render Pass!");
	}
	myFullscreenVinjett = std::make_unique<DreamEngine::FullscreenEffect>();
	if (!myFullscreenVinjett->Init("Shaders/FullscreenVinjettPS.cso"))
	{
		assert("Couldn't load Vinjett Effect!");
	}
	myFullscreenBackBufferRender = std::make_unique<DreamEngine::FullscreenEffect>();
	if (!myFullscreenBackBufferRender->Init("Shaders/RenderToBackBufferPS.cso"))
	{
		assert("Couldn't load Vinjett Effect!");
	}

	myFullscreenDamageEffect = std::make_unique<DreamEngine::FullscreenEffect>();
	if (!myFullscreenDamageEffect->Init("Shaders/OnScreen_Damage_Effect_PS.cso", true))
	{
		assert("Couldn't load Vinjett Effect!");
	}
	myFullscreenHealingEffect = std::make_unique<DreamEngine::FullscreenEffect>();
	if (!myFullscreenHealingEffect->Init("Shaders/OnScreen_Healing_Effect_PS.cso", true))
	{
		assert("Couldn't load Vinjett Effect!");
	}
	myFullscreenBareHUDEffect = std::make_unique<DreamEngine::FullscreenEffect>();
	if (!myFullscreenBareHUDEffect->Init("Shaders/BareHUDShader_PS.cso", true))
	{
		assert("Couldn't load Vinjett Effect!");
	}
	myFullscreenFogEffect = std::make_unique<DreamEngine::FullscreenEffect>();
	if(!myFullscreenFogEffect->Init("Shaders/DepthShaderFog_PS.cso"))
	{
		assert("Couldn't load fog!");
	}

	myDamageTextureOne = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_On_Screen_Damage_1.dds");
	myDamageTextureTwo = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_On_Screen_Damage_2.dds");
	myDamageTextureThree = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_On_Screen_Damage_3.dds");
	myDamageTextureFour = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_On_Screen_Damage_4.dds");

	myHealingTextureOne = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_On_Screen_Healing_1.dds");
	myHealingTextureTwo = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_On_Screen_Healing_2.dds");
	myHealingTextureThree = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_On_Screen_Healing_3.dds"); 
	myHealingTextureFour = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_On_Screen_Healing_4.dds");
	myHealingTextureFive = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_On_Screen_Healing_5.dds");

	myBareHUDTextureOne = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_BareHUDShader01.dds");
	myBareHUDTextureTwo = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_BareHUDShader02.dds");
	myBareHUDTextureThree = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_BareHUDShader03.dds");
	myBareHUDTextureFour = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_BareHUDShader04.dds");


	myTestTexture = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_Test.dds");
	myTestTexture2 = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_Test2.dds");

	//auto resolution = DE::DX11::GetResolution();
	auto resolution = MainSingleton::GetInstance()->GetResolution();
	auto screenSize = MainSingleton::GetInstance()->GetScreenSize();
	myRenderTargets.resize(4);
	myPostColorCorrectionTarget = DreamEngine::RenderTarget::Create(resolution, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	myPostProcessHUDTarget = DreamEngine::RenderTarget::Create(resolution, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	myGeometryBuffer = DreamEngine::GeometryBuffer::Create(resolution);
	myRenderTargets[0] = DreamEngine::RenderTarget::Create(resolution, DXGI_FORMAT_R16G16B16A16_FLOAT);
	myRenderTargets[1] = DreamEngine::RenderTarget::Create(resolution / 2, DXGI_FORMAT_R16G16B16A16_FLOAT);
	myRenderTargets[2] = DreamEngine::RenderTarget::Create(resolution / 4, DXGI_FORMAT_R16G16B16A16_FLOAT);
	myRenderTargets[3] = DreamEngine::RenderTarget::Create(resolution / 8, DXGI_FORMAT_R16G16B16A16_FLOAT);

	InitLight(); 
	
	myDamageHudColor = DE::Vector4f{ 1,0,0,1 };
	myHealingHudColor = DE::Vector4f{ 0,1,0,1 };
	myBareHudColor = DE::Vector4f{ 0,0.3f,0.3f,1 };
	MainSingleton::GetInstance()->GetShaderTool().LoadShaders();
	particleManager.SetCameraTransform(&myMainCamera->GetTransform());
}

void Scene::InitLight()
{
	myAmbientLight = myLoadedObjects->GetAmbientLight();
	if (myAmbientLight == nullptr) 
	{
		std::wstring loc = DreamEngine::Settings::ResolveAssetPathW("cubemap/cave01.dds");
		myAmbientLight = std::make_shared<DE::AmbientLight>(DreamEngine::AmbientLight(loc, { 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f }, 1.0f));
	}

	if (myLoadedObjects->GetDirectionalLight() != nullptr)
	{
		if (!myUseHardcodedDirectionalLight)
		{
			myDirectionalLight.SetColor(myLoadedObjects->GetDirectionalLight()->GetColor());
			myDirectionalLight.SetTransform(myLoadedObjects->GetDirectionalLight()->GetTransform());
		}
		myDirectionalLight.SetIntensity(myLoadedObjects->GetDirectionalLight()->GetIntensity());
	}
	else if(!myUseHardcodedDirectionalLight)
	{
		myDirectionalLight.SetColor({150.0f / 255.0f, 150.0f / 255.0f, 175.0f / 255.0f});
		myDirectionalLight.SetIntensity(1.0f);
		myDirectionalLight.SetTransform(
			{
				{ 0,0,0 },
				//DreamEngine::Rotator(320, 45, 0)
				DreamEngine::Rotator(230,0,330)
			}
		);
	}

	myDirectionalLight.SetRotation(DE::Rotator(270.0f, 0.0f, 0.0f));

	myShadowMapScale = 22000.f;
	myShadowCameraNearFarPlane = 15000.f;
	
	myShadowCam.SetOrtographicProjection(-myShadowMapScale, myShadowMapScale, myShadowMapScale, -myShadowMapScale, 10.0f, -myShadowCameraNearFarPlane);
	myShadowCam.SetTransform(myDirectionalLight.GetTransform()); 
	myShadowBuffer = DreamEngine::DepthBuffer::Create({ShadowMapResolution * 6, ShadowMapResolution * 6}); 

	myVignette = 10.f;

	auto& engine = *DreamEngine::Engine::GetInstance(); 

	engine.GetGraphicsEngine().GetGraphicsStateStack().SetColorCorrectionBufferData(myExposure,mySaturation,myContrast,myBlackPoint,myTint);
	engine.GetGraphicsEngine().GetGraphicsStateStack().SetBloomBlending(myBloom);
}

void Scene::Update(float aDeltaTime)
{	
	MainSingleton::GetInstance()->GetInputMapper().Update();	
	MainSingleton::GetInstance()->GetShaderTool().Update(aDeltaTime);

	myLoadedObjects->Update(aDeltaTime); 

#ifndef _RETAIL
	if (MainSingleton::GetInstance()->GetInputManager().IsKeyDown(DE::eKeyCode::F6))
	{
		myCurrentRenderPassNum++;
		if (myCurrentRenderPassNum > 6) 
		{
			myCurrentRenderPassNum = 1;
		}
	}
#endif
	
	DreamEngine::Vector3f directionalLightPos = myDirectionalLight.GetTransform().GetPosition();
	DreamEngine::Vector3f directionalLightDirection = myDirectionalLight.GetTransform().GetRotation();
	DreamEngine::Color directionalLightColor = myDirectionalLight.GetColor();

#ifndef NDEBUG
	//DebuggingCamera debug;
	//debug.FreeCam(aDeltaTime, &myMainCamera);

	if (ImGui::Begin("Shadow Mapping"))
	{
		//ImGui::DragFloat("Shadow Map Scale", &myShadowMapScale, 50.05f, -100000.f, 100000.f);
		//ImGui::DragFloat("Shadow Cam Near and Far", &myShadowCameraNearFarPlane, 50.05f, -100000.f, 100000.f);
		//ImGui::DragFloat3("Camera position", &myCameraOffset.x, 1.0f, -5000.0f, 5000.0f);
		ImGui::DragFloat("Ambient Fade Height", &myCurrentAmbientFadeHeight, 10.0f, -10000.0f, 20000.0f);
		ImGui::DragFloat("Light Y Position", &directionalLightPos.y, 10.0f, -10000.0f, 20000.0f);
		ImGui::DragFloat3("Light Direction", &directionalLightDirection.x, 0.1f, 0.0f, 360.0f);
		ImGui::Dummy({ 0, 10 });
		if (ImGui::TreeNode("Directional Light Color"))
		{
			ImGui::ColorPicker3("Directional Light Color", &directionalLightColor.myR);
			ImGui::TreePop();
		}

		ImGui::DragFloat("Bloom Blending", &myBloom, 0.01f, 0.f, 1.f);
		ImGui::DragFloat("Exposure", &myExposure, 0.01f, -5.0f, 5.0f);
		ImGui::DragFloat("Saturation", &mySaturation, 0.01f, 0.0f, 3.0f);
		ImGui::DragFloat3("Contrast", &myContrast.x, 0.01f, 0.005f, 5.0f);
		ImGui::Dummy({ 0, 10 });
		if (ImGui::TreeNode("BlackPoint"))
		{
			ImGui::ColorPicker3("BlackPoint", &myBlackPoint.x);
			ImGui::TreePop();
		}
		ImGui::Dummy({ 0, 10 });
		if (ImGui::TreeNode("Tint"))
		{
			ImGui::ColorPicker3("Tint", &myTint.x);
			ImGui::TreePop();
		}

		ImGui::DragFloat("Vinjett", &myVignette, 0.005f, 0.f, 10.f);

		ImGui::ColorPicker3("FogColour", &myFogColour.x);
		ImGui::DragFloat("FogDensity", &myFogDensity, 0.1f, 0.0f, 5.0f);
	}
	ImGui::End();

	if (ImGui::Begin("WaterDirection"))
	{
		ImGui::DragFloat("AlbedoX", &myWaterPanAlbedoX, 0.001f, -1, 1);
		ImGui::DragFloat("AlbedoY", &myWaterPanAlbedoY, 0.001f, -1, 1);
		myWaterPanNormalX = myWaterPanAlbedoX * 0.5f;
		myWaterPanNormalY = myWaterPanAlbedoY * 0.5f;
		ImGui::DragFloat("Amplitude", &myWaterConfig(1,1), 1.00f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Speed", &myWaterConfig(1, 2), 0.01f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Frequency", &myWaterConfig(1, 3), 0.0001f, -1000.0f, 1000.0f);
	}
	ImGui::End();
#endif
	
	myDirectionalLight.SetRotation(directionalLightDirection);
	myDirectionalLight.SetRotation(DE::Rotator(270.0f, 0.0f, 0.0f));
	myDirectionalLight.SetPosition(directionalLightPos);
	myDirectionalLight.SetColor(directionalLightColor);
	
	//myShadowCam.SetPosition(DE::Vector3f{ myPlayer->GetTransform()->GetPosition().x + (myMainCamera->GetTransform().GetMatrix().GetForward().GetNormalized().x * 2), myPlayer->GetTransform()->GetPosition().y + (myMainCamera->GetTransform().GetMatrix().GetForward().GetNormalized().y * 2), myPlayer->GetTransform()->GetPosition().z + (myMainCamera->GetTransform().GetMatrix().GetForward().GetNormalized().z * 2)});
	myShadowCam.SetRotation(directionalLightDirection);
	threadSafe.lock();
	particleManager.Update(aDeltaTime);
	threadSafe.unlock();

#ifndef _RETAIL
	auto& inputMng = MainSingleton::GetInstance()->GetInputManager();
	if (inputMng.IsKeyDown(DE::eKeyCode::C) && inputMng.IsKeyHeld(DE::eKeyCode::O))
	{
		ourVisualizeCollidersFlag = !ourVisualizeCollidersFlag;
	}
	else if (inputMng.IsKeyHeld(DE::eKeyCode::C) && inputMng.IsKeyDown(DE::eKeyCode::O))
	{
		ourVisualizeCollidersFlag = !ourVisualizeCollidersFlag;
	}
#endif
}

void Scene::Render()
{
	auto& engine = *DreamEngine::Engine::GetInstance(); 
	auto& graphicsEngine = engine.GetGraphicsEngine();
	auto& modelDrawer = engine.GetGraphicsEngine().GetModelDrawer();
	auto& graphicsStateStack = graphicsEngine.GetGraphicsStateStack();
	DreamEngine::SpriteDrawer& spriteDrawer = graphicsEngine.GetSpriteDrawer();
	graphicsStateStack.SetCamera(*myMainCamera);
	
#ifndef NDEBUG
	ImGui::Begin("Shader Params");
	ImGui::Checkbox("Render BareHud", &myShoudlRenderBareHud);
	if (myShoudlRenderBareHud == true)
	{
		ImGui::DragFloat4("BareHud Color", &myBareHudColor.x, 0.01f, 0);
	}
	ImGui::Checkbox("Render Damage Hud", &myShoudlRenderDamageHud);
	if (myShoudlRenderDamageHud == true)
	{
		ImGui::DragFloat4("Damage Color", &myDamageHudColor.x, 0.01f, 0);
	}
	ImGui::Checkbox("Render Healing Hud", &myShoudlRenderHealingHud);
	if (myShoudlRenderHealingHud == true)
	{
		ImGui::DragFloat4("Healing Color", &myHealingHudColor.x, 0.01f, 0);
	}
	ImGui::End();
#endif

	myLoadedObjects->RenderMinimapGeometry();

	{	// Render to GBuffer
		ID3D11ShaderResourceView* const pSRV[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
		DreamEngine::DX11::ourContext->PSSetShaderResources(6, 6, pSRV);
		graphicsStateStack.Push();
		myGeometryBuffer.ClearTextures();
		myGeometryBuffer.SetAsActiveTarget(DreamEngine::DX11::ourDepthBuffer);
		graphicsStateStack.SetCamera(*myMainCamera);
		graphicsStateStack.SetAlphaTestThreshold(0.364f);
		graphicsStateStack.SetBlendState(DreamEngine::BlendState::Disabled);
		graphicsStateStack.SetRasterizerState(DreamEngine::RasterizerState::BackfaceCulling);
		graphicsStateStack.SetDepthStencilState(DreamEngine::DepthStencilState::WriteLessOrEqual);
		graphicsStateStack.UpdateGpuStates();
		myLoadedObjects->Render(false, myGeometryBuffer);
		graphicsStateStack.Pop();
	}


	{	// Render to Shadow Map
		ID3D11ShaderResourceView* const pSRVShadow[1] = { NULL };
		DreamEngine::DX11::ourContext->PSSetShaderResources(5, 1, pSRVShadow);
		//myShadowCam.SetPosition(DE::Vector3f{ myPlayer->GetTransform()->GetPosition().x, myDirectionalLight.GetTransform().GetPosition().y, myPlayer->GetTransform()->GetPosition().z });
		graphicsStateStack.Push();
		graphicsStateStack.SetAlphaTestThreshold(0.05f);

		graphicsStateStack.SetCamera(myShadowCam);
		graphicsStateStack.SetBlendState(DreamEngine::BlendState::Disabled);
		graphicsStateStack.SetRasterizerState(DreamEngine::RasterizerState::BackfaceCulling);

		myShadowBuffer.Clear();
		myShadowBuffer.SetAsActiveTarget();

		myLoadedObjects->Render(true, myGeometryBuffer);

		graphicsStateStack.Pop();
	}
	
	
	DreamEngine::Camera tempCam = *myMainCamera;
	{	// Render GBuffer Fullscreen Lighting Pass
		graphicsStateStack.Push();
		myRenderTargets[0].Clear();
		ID3D11ShaderResourceView* const pSRV[1] = { NULL };
		DreamEngine::DX11::ourContext->PSSetShaderResources(13, 1, pSRV);
		myRenderTargets[0].SetAsActiveTarget();
		for (int i = 0; i < 2; i++)
		{
			DE::DX11::ourContext->GenerateMips(myGeometryBuffer.mySRVs[i].Get());
		}
		myShadowBuffer.SetAsResourceOnSlot(5);
		myGeometryBuffer.SetAllAsResources(6);
		graphicsStateStack.SetCamera(tempCam);
		graphicsStateStack.SetBlendState(DreamEngine::BlendState::Disabled);
		myDirectionalLight.SetTransform(myShadowCam.GetTransform());
		myDirectionalLight.SetProjectionMatrix(myShadowCam.GetProjection()); // Only needed if we start rotating our directional light
		graphicsStateStack.SetDirectionalLight(myDirectionalLight);
		graphicsStateStack.SetAmbientLight(*myAmbientLight);
		graphicsStateStack.SetPassMode(myCurrentRenderPassNum);
		graphicsStateStack.SetAmbientFadeHeight(myCurrentAmbientFadeHeight);
		graphicsStateStack.UpdateGpuStates();
		myFullscreenGBufferRenderPass->Render();

		graphicsStateStack.Pop();
	}
	
	{	// Render Point and Spot lights using GBuffer data
		graphicsStateStack.Push();
		myRenderTargets[0].SetAsActiveTarget();
		myGeometryBuffer.SetAllAsResources(6);
		graphicsStateStack.SetCamera(tempCam);
		graphicsStateStack.SetBlendState(DreamEngine::BlendState::AdditiveBlend);
		graphicsStateStack.SetRasterizerState(DreamEngine::RasterizerState::FrontfaceCulling);
		graphicsStateStack.SetDepthStencilState(DreamEngine::DepthStencilState::ReadOnlyGreater);
		graphicsStateStack.UpdateGpuStates();
		for (auto& pointLight : myLoadedObjects->GetPointLights())
		{
			if (myLoadedObjects->IsInViewFrustum(pointLight->GetTransform().GetPosition(), pointLight->GetRange()))
			{
				graphicsStateStack.AddPointLight(*pointLight);
				modelDrawer.DrawGBPSL(*pointLight->myObjPtr);
				graphicsStateStack.ClearPointLightsAndSpotLights();
			}
		}
		for (auto& spotLight : myLoadedObjects->GetSpotLights())
		{
			if (myLoadedObjects->IsInViewFrustum(spotLight->GetTransform().GetPosition(), spotLight->GetRange()))
			{
				graphicsStateStack.AddSpotLight(*spotLight);
				modelDrawer.DrawGBPSL(*spotLight->myObjPtr);
				graphicsStateStack.ClearPointLightsAndSpotLights();
			}
		}

		myLoadedObjects->RenderObjectLights(graphicsEngine);

		graphicsStateStack.Pop();

		myRenderTargets[0].SetAsActiveTarget();
		myRenderTargets[0].SetAsActiveTarget(DreamEngine::DX11::ourDepthBuffer);
	}

	//myWaterPanAlbedoX = 0.01f;//myLoadedObjects->GetBoat()->GetTransform()->GetMatrix().GetRight().x * 0.00001f;
	//myWaterPanAlbedoY = 0.01f;//(myLoadedObjects->GetBoat()->GetTransform()->GetMatrix().GetRight().z * 0.00001f) * -1.0f;
	
	if (myWaterPanAlbedoX >= 0.15f)
	{
		myWaterPanAlbedoX = 0.15f;
	}
	else if (myWaterPanAlbedoX <= -0.15f)
	{
		myWaterPanAlbedoX = -0.15f;
	}

	if (myWaterPanAlbedoY >= 0.15f)
	{
		myWaterPanAlbedoY = 0.15f;
	}
	else if (myWaterPanAlbedoY <= -0.15f)
	{
		myWaterPanAlbedoY = -0.15f;
	}

	graphicsStateStack.SetCustomShaderParameters(DE::Vector4f{ myWaterPanAlbedoX,myWaterPanAlbedoY,myWaterPanNormalX,myWaterPanNormalY });
	graphicsStateStack.SetShaderToolObjectVarsBufferData(myWaterConfig, DE::Matrix4x4f{}, DE::Matrix4x4f{}, DE::Matrix4x4f{}, DE::Matrix4x4f{}, myMainCamera->GetTransform().GetPosition());
	MainSingleton::GetInstance()->GetShaderTool().RenderWaterPlanes(graphicsStateStack);

	MainSingleton::GetInstance()->GetShaderTool().RenderUnityLoadedModels(graphicsStateStack);

	graphicsStateStack.SetShaderToolMiscVarsBufferData(0.f, 0.f, engine.GetTotalTime());

	// Render Bloom and Color Correction
	myPostColorCorrectionTarget.Clear();
	RenderBloomAndColorCorrection(graphicsStateStack);
	
	RenderFogEffect(graphicsStateStack);
	
	myPostProcessHUDTarget.Clear();

	//engine.GetGraphicsEngine().GetGraphicsStateStack().SetVinjett(myVignette);
	//RenderVinjett(graphicsStateStack);

	if (myShoudlRenderDamageHud == true)
	{
		RenderDamageEffect(graphicsStateStack);
	}
	if (myShoudlRenderHealingHud == true)
	{
		RenderHealingEffect(graphicsStateStack);
	}
	if (myShoudlRenderBareHud == true)
	{
		RenderHUDEffect(graphicsStateStack);
	}

	DreamEngine::DX11::ourBackBuffer->SetAsActiveTarget(DE::DX11::ourDepthBuffer);
	RenderToBackBuffer(graphicsStateStack);
	myGeometryBuffer.SetAllAsResources(7);

	graphicsStateStack.SetShaderToolObjectVarsBufferData(DE::Matrix4x4f{}, DE::Matrix4x4f{}, DE::Matrix4x4f{}, DE::Matrix4x4f{}, DE::Matrix4x4f{}, myMainCamera->GetTransform().GetPosition());
	graphicsStateStack.SetDepthStencilState(DE::DepthStencilState::ReadOnly);
	graphicsStateStack.SetBlendState(DreamEngine::BlendState::AlphaBlend);
	graphicsStateStack.SetRasterizerState(DE::RasterizerState::BackfaceCulling);

	//auto dotP = myCamera->GetTransform().GetMatrix().GetForward().Dot(grappleVec.GetNormalized());
	
	threadSafe.lock();
	particleManager.Render(spriteDrawer);
	threadSafe.unlock();


	//graphicsStateStack.SetShaderToolMiscVarsBufferData(0.f, 0.f, engine.GetTotalTime());
	graphicsStateStack.SetDepthStencilState(DE::DepthStencilState::WriteLessOrEqual);
	graphicsStateStack.SetRasterizerState(DreamEngine::RasterizerState::BackfaceCulling);
	graphicsStateStack.SetBlendState(DreamEngine::BlendState::AdditiveBlend);

	
	{ // RENDER NAVMESH DEBUG LINES
		graphicsStateStack.Push();
		graphicsStateStack.SetCamera(*myMainCamera);
		DreamEngine::DX11::ourBackBuffer->SetAsActiveTarget();
		graphicsStateStack.SetRasterizerState(DreamEngine::RasterizerState::BackfaceCulling);
		graphicsStateStack.SetBlendState(DreamEngine::BlendState::Disabled);
		myLoadedObjects->GetNavmeshHandler().Render();
		graphicsStateStack.Pop();
	}

	if (ourVisualizeCollidersFlag)
	{
		graphicsStateStack.Push();
		graphicsStateStack.SetCamera(*myMainCamera);
		myLoadedObjects->RenderColliderVisualizations();
		graphicsStateStack.Pop();
	}
	else 
	{
		//#define VisualizeColliders
#ifdef VisualizeColliders
		graphicsStateStack.Push();
		graphicsStateStack.SetCamera(*myMainCamera);
		myLoadedObjects->RenderColliderVisualizations();
		graphicsStateStack.Pop();
#endif
	}
	graphicsStateStack.SetBlendState(DreamEngine::BlendState::AdditiveBlend);

	
	graphicsStateStack.SetShaderToolObjectVarsBufferData(myLoadedObjects->GetBoat()->GetTransform()->GetMatrix(), DE::Matrix4x4f{}, DE::Matrix4x4f{}, DE::Matrix4x4f{}, DE::Matrix4x4f{}, DE::Vector3f{});

	


	graphicsStateStack.SetCamera(*myMainCamera); 
	
	MainSingleton::GetInstance()->SwapModelBuffers();
}

void Scene::Receive(const Message & aMsg)
{
	switch(aMsg.messageType)
	{
		case eMessageType::ChangeResolution:
		{
			auto resolution = MainSingleton::GetInstance()->GetResolution(); 
			auto screenSize = MainSingleton::GetInstance()->GetScreenSize();

			myPostColorCorrectionTarget = DreamEngine::RenderTarget::Create(DE::Vector2ui(resolution.x, resolution.y), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
			myGeometryBuffer = DreamEngine::GeometryBuffer::Create(DE::Vector2ui(resolution.x, resolution.y));
			myRenderTargets[0] = DreamEngine::RenderTarget::Create(DE::Vector2ui(resolution.x, resolution.y), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
			myRenderTargets[1] = DreamEngine::RenderTarget::Create(DE::Vector2ui(resolution.x, resolution.y) / 2, DXGI_FORMAT_R16G16B16A16_FLOAT);
			myRenderTargets[2] = DreamEngine::RenderTarget::Create(DE::Vector2ui(resolution.x, resolution.y) / 4, DXGI_FORMAT_R16G16B16A16_FLOAT);
			myRenderTargets[3] = DreamEngine::RenderTarget::Create(DE::Vector2ui(resolution.x, resolution.y) / 8, DXGI_FORMAT_R16G16B16A16_FLOAT);
			myMainCamera->SetPerspectiveProjection(myFoV, DE::Vector2f(resolution.x, resolution.y), myNearPlane, myFarPlane);
			break;
		}
		default: 
		{

			break;
		}
	}
}

int Scene::GetLevelID()
{
	return myCurrentLevel; 
}

std::shared_ptr<DreamEngine::Camera> Scene::GetCamera()
{
	return myMainCamera;
}
void Scene::RenderVinjett(DreamEngine::GraphicsStateStack& aGStateStack)
{
	aGStateStack.Push();
	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	DreamEngine::DX11::ourContext->PSSetShaderResources(0, 1, pSRV);
	myPostProcessHUDTarget.SetAsActiveTarget();
	myPostColorCorrectionTarget.SetAsResourceOnSlot(12);
	myFullscreenVinjett->Render();
	aGStateStack.Pop();
}
void Scene::RenderDamageEffect(DreamEngine::GraphicsStateStack& aGStateStack)
{
	aGStateStack.Push();
	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	DreamEngine::DX11::ourContext->PSSetShaderResources(0, 1, pSRV);
	myPostColorCorrectionTarget.SetAsActiveTarget();
	aGStateStack.SetShaderToolMaterialVarsBufferData(DE::Vector4f(1, 1, 1, 1), DE::Vector4f(1, 1, 1, 1), DE::Vector4f(1, 1, 1, 1), myDamageHudColor, 0.f);
	aGStateStack.UpdateGpuStates(false);
	/*myHealingTextureOne->SetAsResourceOnSlot(0);
	myHealingTextureTwo->SetAsResourceOnSlot(1);
	myHealingTextureThree->SetAsResourceOnSlot(2);
	myHealingTextureFour->SetAsResourceOnSlot(3);*/
	myDamageTextureOne->SetAsResourceOnSlot(0);
	myDamageTextureTwo->SetAsResourceOnSlot(1);
	myDamageTextureThree->SetAsResourceOnSlot(2);
	myDamageTextureFour->SetAsResourceOnSlot(3);
	myFullscreenDamageEffect->Render(true);
	aGStateStack.Pop();
}
void Scene::RenderHealingEffect(DreamEngine::GraphicsStateStack& aGStateStack)
{
	aGStateStack.Push();
	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	DreamEngine::DX11::ourContext->PSSetShaderResources(0, 1, pSRV);
	myPostColorCorrectionTarget.SetAsActiveTarget();
	aGStateStack.SetShaderToolMaterialVarsBufferData(DE::Vector4f(1, 1, 1, 1), DE::Vector4f(1, 1, 1, 1), DE::Vector4f(1, 1, 1, 1), myHealingHudColor, 0.f);
	aGStateStack.UpdateGpuStates(false);
	myHealingTextureOne->SetAsResourceOnSlot(0);
	myHealingTextureTwo->SetAsResourceOnSlot(1);
	myHealingTextureThree->SetAsResourceOnSlot(2);
	myHealingTextureFour->SetAsResourceOnSlot(3);
	myHealingTextureFive->SetAsResourceOnSlot(4);
	myFullscreenHealingEffect->Render(true);
	aGStateStack.Pop();
}
void Scene::RenderHUDEffect(DreamEngine::GraphicsStateStack& aGStateStack)
{
	aGStateStack.Push();
	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	DreamEngine::DX11::ourContext->PSSetShaderResources(0, 1, pSRV);
	myPostColorCorrectionTarget.SetAsActiveTarget();
	aGStateStack.SetShaderToolMaterialVarsBufferData(DE::Vector4f(1, 1, 1, 1), DE::Vector4f(1, 1, 1, 1), DE::Vector4f(1, 1, 1, 1), myBareHudColor, 0.f);
	aGStateStack.UpdateGpuStates(false);
	myBareHUDTextureOne->SetAsResourceOnSlot(0);
	myBareHUDTextureTwo->SetAsResourceOnSlot(1);
	myBareHUDTextureThree->SetAsResourceOnSlot(2);
	myBareHUDTextureFour->SetAsResourceOnSlot(3);
	myFullscreenBareHUDEffect->Render(true);
	aGStateStack.Pop();
}
void Scene::RenderToBackBuffer(DreamEngine::GraphicsStateStack& aGStateStack)
{
	aGStateStack.Push();
	aGStateStack.SetBlendState(DreamEngine::BlendState::Disabled);
	aGStateStack.UpdateGpuStates(false);
	DreamEngine::DX11::ourBackBuffer->SetAsActiveTarget();
	myPostColorCorrectionTarget.SetAsResourceOnSlot(13);
	myFullscreenBackBufferRender->Render();
	aGStateStack.Pop();
}

void Scene::RenderFogEffect(DreamEngine::GraphicsStateStack& aGStateStack)
{
	aGStateStack.Push();
	aGStateStack.SetCamera(*myMainCamera); 
	ID3D11ShaderResourceView* const pSRV[1] = {NULL};
	DreamEngine::DX11::ourContext->PSSetShaderResources(0, 1, pSRV);
	myPostColorCorrectionTarget.SetAsActiveTarget();

	// Set fog parameters
	aGStateStack.SetCustomShaderParameters(DE::Vector4f(myFogDensity, myFogColour.x, myFogColour.y, myFogColour.z));
	aGStateStack.SetBlendState(DreamEngine::BlendState::AdditiveBlend);
	
	DE::DX11::ourDepthBuffer->SetAsResourceOnSlot(6);
	myGeometryBuffer.SetAsResourceOnSlot(DE::GeometryBuffer::eGeometryBufferTexture::Albedo, 7);
	aGStateStack.UpdateGpuStates(false);

	myFullscreenFogEffect->Render();

	aGStateStack.SetBlendState(DreamEngine::BlendState::Disabled);
	aGStateStack.Pop();
}

void Scene::RenderBloomAndColorCorrection(DreamEngine::GraphicsStateStack& aGStateStack)
{
	aGStateStack.Push();
	aGStateStack.SetCamera(*myMainCamera);
	aGStateStack.SetBlendState(DreamEngine::BlendState::Disabled);
	aGStateStack.SetSamplerState(DE::SamplerFilter::Trilinear, DE::SamplerAddressMode::Clamp);
	aGStateStack.SetColorCorrectionBufferData(myExposure, mySaturation, myContrast, myBlackPoint, myTint);
	aGStateStack.UpdateGpuStates();

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	DreamEngine::DX11::ourContext->PSSetShaderResources(0, 1, pSRV);
	// Downscaling
	for(int i = 1; i < myRenderTargets.size(); i++)
	{
		myRenderTargets[i].SetAsActiveTarget();
		myRenderTargets[i - 1].SetAsResourceOnSlot(0);
		myFullscreenDownScale->Render();
	}

	aGStateStack.SetBloomBlending(myBloom);
	aGStateStack.SetBlendState(DreamEngine::BlendState::AlphaBlend);
	aGStateStack.UpdateGpuStates();
	DreamEngine::DX11::ourContext->PSSetShaderResources(0, 1, pSRV);

	// Upscaling
	for(int i = (int)myRenderTargets.size() - 2; i > -1; i--)
	{
		myRenderTargets[i].SetAsActiveTarget();
		myRenderTargets[i + 1].SetAsResourceOnSlot(0);
		myFullscreenUpScale->Render();
	}
	
	aGStateStack.SetBlendState(DreamEngine::BlendState::Disabled);
	aGStateStack.UpdateGpuStates();
	DreamEngine::DX11::ourContext->PSSetShaderResources(12, 1, pSRV);
	myPostColorCorrectionTarget.SetAsActiveTarget();
	myRenderTargets[0].SetAsResourceOnSlot(0);
	myFullscreenColorCorrection->Render();
	aGStateStack.Pop();
}
