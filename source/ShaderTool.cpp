#include "ShaderTool.h"
#include "DreamEngine/engine.h"
#include "DreamEngine/graphics/ModelDrawer.h"
#include "DreamEngine/graphics/GraphicsEngine.h"
#include "DreamEngine/graphics/ModelFactory.h"
#include "DreamEngine/graphics/TextureManager.h"
#include <DreamEngine/shaders/ModelShader.h>
#include <DreamEngine/graphics/GraphicsStateStack.h>

#include <imgui/imgui.h>

ShaderTool::ShaderTool()
{
	myTotalVFXTimer = 0;
	myWaterGridSize = 4;
	myWaterPlaneSize = 20000;
}

ShaderTool::~ShaderTool()
{
	for (int i = 0; i < myAnimatedModelShaders.size(); i++)
	{
		delete myAnimatedModelShaders[i];
	}
	for (int i = 0; i < myModelShaders.size(); i++)
	{
		delete myModelShaders[i];
	}
}

bool ShaderTool::LoadShaders()
{
	DreamEngine::ModelInstance tempInstance = DE::ModelFactory::GetInstance().GetModelInstance(L"3D/spherePrimitive.fbx");
	tempInstance.SetScale(1);
	tempInstance.SetLocation(DE::Vector3f{ 0, 100.f, 0 });

	{
		DreamEngine::ModelShader* tempModelShader = new DreamEngine::ModelShader(DreamEngine::Engine::GetInstance());
		if (!tempModelShader->Init(L"Shaders/ShaderDesignerAnimated_VS.cso", L"Shaders/Fresnel_testPS.cso"))
		{
			return false;
		}
		myAnimatedModelShaders.push_back(tempModelShader);
	}
	{
		DreamEngine::ModelShader* tempModelShader = new DreamEngine::ModelShader(DreamEngine::Engine::GetInstance());
		if (!tempModelShader->Init(L"Shaders/ShaderDesignerAnimated_VS.cso", L"Shaders/PlayerShader_PS.cso"))
		{
			return false;
		}

		myGunTextureOne = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_Player01.dds");
		myGunTextureTwo = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_Player02.dds");
		myGunTextureThree = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_Player03.dds");

		myAnimatedModelShaders.push_back(tempModelShader);
	}
	{
		DreamEngine::ModelShader* tempModelShader = new DreamEngine::ModelShader(DreamEngine::Engine::GetInstance());
		if (!tempModelShader->Init(L"Shaders/ShaderDesignerVS.cso", L"Shaders/LaserShader_PS.cso"))
		{
			return false;
		}

		myLaserTextureOne = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_LaserShader01.dds");
		myLaserTextureTwo = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_LaserShader02.dds");

		myModelShaders.push_back(tempModelShader);
	}
	{
		DreamEngine::ModelShader* tempModelShader = new DreamEngine::ModelShader(DreamEngine::Engine::GetInstance());
		if (!tempModelShader->Init(L"Shaders/ShaderDesignerVS.cso", L"Shaders/AnchorShader_PS.cso"))
		{
			return false;
		}

		myGrapplingTextureOne = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_AnchorShader01.dds");
		myGrapplingTextureTwo = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/TEX_AnchorShader02.dds");

		myModelShaders.push_back(tempModelShader);
	}
	{
		DreamEngine::ModelShader* tempModelShader = new DreamEngine::ModelShader(DreamEngine::Engine::GetInstance());
		if (!tempModelShader->Init(L"Shaders/ShaderDesignerVS.cso", L"Shaders/MuzzleFlash_PS.cso"))
		{
			return false;
		}

		myModelShaders.push_back(tempModelShader);
	}
	{
		DreamEngine::ModelShader* tempModelShader = new DreamEngine::ModelShader(DreamEngine::Engine::GetInstance());
		if (!tempModelShader->Init(L"Shaders/ShaderDesignerAnimated_VS.cso", L"Shaders/Fresnel_Red_PS.cso"))
		{
			return false;
		}
		myAnimatedModelShaders.push_back(tempModelShader);
	}

	{
		DreamEngine::ModelShader* tempModelShader = new DreamEngine::ModelShader(DreamEngine::Engine::GetInstance());
		if (!tempModelShader->Init(L"Shaders/ShaderDesignerAnimated_VS.cso", L"Shaders/Fresnel_Blue_PS.cso"))
		{
			return false;
		}
		myAnimatedModelShaders.push_back(tempModelShader);
	}

	{
		DreamEngine::ModelShader* tempModelShader = new DreamEngine::ModelShader(DreamEngine::Engine::GetInstance());
		if (!tempModelShader->Init(L"Shaders/ShaderDesignerAnimated_VS.cso", L"Shaders/Fresnel_Green_PS.cso"))
		{
			return false;
		}
		myAnimatedModelShaders.push_back(tempModelShader);
	}

	tempInstance = DE::ModelFactory::GetInstance().GetModelInstance(L"3D/SM_Shader_Test_Plane.fbx");

	DreamEngine::ModelShader* tempModelShader = new DreamEngine::ModelShader(DreamEngine::Engine::GetInstance());
	if (!tempModelShader->Init(L"Shaders/Water_VS.cso", L"Shaders/Water_PS.cso"))
	{
		return false;
	}
	myModelShaders.push_back(tempModelShader);
	myVolumetricLightTextureOne = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/T_WaterTilingTest_c.dds");
	myVolumetricLightTextureTwo = DE::Engine::GetInstance()->GetTextureManager().GetTexture(L"ShaderTextures/SM_Shader_Test_Plane_n.dds");

	DE::Vector3f position = 0;
	position.y = -3700.0f;

	for (int i = 0; i < myWaterGridSize; i++)
	{
		for (int i = 0; i < myWaterGridSize; i++)
		{
			{
				tempInstance.SetTexture(0, 1, myVolumetricLightTextureOne);
				tempInstance.SetTexture(0, 2, myVolumetricLightTextureTwo);
				tempInstance.SetLocation(position);
				tempInstance.SetScale(100);
				myWaterPlaneModelInstances.push_back(tempInstance);
				position.x -= myWaterPlaneSize;
			}
		}
		position.z -= myWaterPlaneSize;
		position.x = 0;
	}


	for (int i = 0; i < myModelInstances.size(); i++)
	{
		myShouldRenderStaticVFX.push_back(false);
	}

	for (int i = 0; i < myAnimatedModelInstances.size(); i++)
	{
		myShouldRenderAnimatedVFX.push_back(false);
	}

	myLoadedLaserTextures = false;
	myLoadedGrapplingTextures = false;

	return true;
}

void ShaderTool::Update(float aDeltaTime)
{
	myTotalVFXTimer += aDeltaTime;

	if (ImGui::Begin("ShaderTimer"))
	{
		ImGui::DragFloat("Time:", &myTotalVFXTimer, 0);
	}
	ImGui::End();
}

DreamEngine::ModelShader* ShaderTool::GetModelShader(int anIndex)
{
	return myModelShaders[anIndex];
}

DreamEngine::ModelInstance ShaderTool::GetModelInstances(int anIndex)
{
	return myModelInstances[anIndex];
}

void ShaderTool::DrawShaderTool(int anIndex)
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::ModelDrawer& modelDrawer = engine.GetGraphicsEngine().GetModelDrawer();
	modelDrawer.DrawShaderTool(myModelInstances[anIndex], myModelShaders[anIndex]);
}

void ShaderTool::DrawShaderToolAnimated(int anIndex)
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::ModelDrawer& modelDrawer = engine.GetGraphicsEngine().GetModelDrawer();
	modelDrawer.DrawShaderToolAnimated(*myAnimatedModelInstances[anIndex], myAnimatedModelShaders[anIndex]);
}

void ShaderTool::SetVFXActive(int aIndex, bool aShouldRender)
{
	myShouldRenderStaticVFX[aIndex] = aShouldRender;
}

void ShaderTool::SetAnimatedVFXActive(int aIndex, bool aShouldRender)
{
	myShouldRenderAnimatedVFX[aIndex] = aShouldRender;
}

void ShaderTool::Render(DE::Matrix4x4f aSwordJointPos, DE::Vector3f aSwordEndJointPos, DE::Vector3f aHandJointPos, DE::Vector3f aTargetPos, DreamEngine::GraphicsStateStack& aGraphicsStateStack)
{
	DE::Vector3f swordDirRot;
	DE::Vector3f pos;
	DE::Vector3f scale;

	for (int i = 0; i < myShouldRenderStaticVFX.size(); i++)
	{
		myStaticVFXType = static_cast<eStaticVFXType>(i);
		switch (myStaticVFXType)
		{
		default:
			aGraphicsStateStack.SetShaderToolMiscVarsBufferData(0.f, 0.f, myTotalVFXTimer);
			break;
		}

		if (myShouldRenderStaticVFX[i] == true)
		{
			if (i != 1)
			{
				DrawShaderTool(i);
			}
		}
	}


	for (int i = 0; i < myShouldRenderAnimatedVFX.size(); i++)
	{
		myAnimatedVFXType = static_cast<eAnimatedVFXType>(i);
		switch (myAnimatedVFXType)
		{
		default:
			aGraphicsStateStack.SetShaderToolMiscVarsBufferData(0.f, 0.f, myTotalVFXTimer);
			break;
		}

		if (myShouldRenderAnimatedVFX[i] == true)
		{
			DrawShaderToolAnimated(i);
		}
	}

	aGraphicsStateStack.SetShaderToolMiscVarsBufferData(0.f, 0.f, myTotalVFXTimer);
}

void ShaderTool::Render(DreamEngine::GraphicsStateStack& aGraphicsStateStack, float aTimer, std::shared_ptr<DreamEngine::AnimatedModelInstance> aAnimatedModel, int aAnimatedShaderIndex, int aCurrentAmmoType)
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::ModelDrawer& modelDrawer = engine.GetGraphicsEngine().GetModelDrawer();
	aGraphicsStateStack.SetShaderToolMiscVarsBufferData(0, 0, aTimer);


	if (aAnimatedShaderIndex == static_cast<int>(eAnimatedVFXType::PlayerShaderGun))
	{
		for (int i = 0; i < aAnimatedModel->GetModel()->GetMeshCount(); i++)
		{
			if (myGunOriginalTextures.size() == 0)
			{
				for (int j = 0; j < 4; j++)
				{
					myGunOriginalTextures.push_back(*aAnimatedModel->GetTextures(i)[j]);
				}
			}

			aAnimatedModel->SetTexture(i, 0, myGunTextureOne);
			aAnimatedModel->SetTexture(i, 1, myGunTextureTwo);
			aAnimatedModel->SetTexture(i, 2, myGunTextureThree);
		}
	}

	aGraphicsStateStack.SetShaderToolMaterialVarsBufferData(DE::Vector4f{}, DE::Vector4f{}, DE::Vector4f{}, DE::Vector4f{}, aCurrentAmmoType);
	aGraphicsStateStack.UpdateGpuStates(false);
	modelDrawer.DrawShaderToolAnimated(*aAnimatedModel, myAnimatedModelShaders[aAnimatedShaderIndex]);

	if (aAnimatedShaderIndex == static_cast<int>(eAnimatedVFXType::PlayerShaderGun))
	{
		for (int i = 0; i < aAnimatedModel->GetModel()->GetMeshCount(); i++)
		{
			for (int j = 0; j < myGunOriginalTextures.size(); j++)
			{
				aAnimatedModel->SetTexture(i, j, &myGunOriginalTextures[j]);
			}
		}
	}
}

void ShaderTool::Render(DreamEngine::GraphicsStateStack& aGraphicsStateStack, float aTimer, DreamEngine::ModelInstance aModel, int aShaderIndex)
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::ModelDrawer& modelDrawer = engine.GetGraphicsEngine().GetModelDrawer();
	aGraphicsStateStack.SetShaderToolMiscVarsBufferData(0, 0, aTimer);
	if (myLoadedLaserTextures == false)
	{
		if (aShaderIndex == static_cast<int>(eStaticVFXType::Laser))
		{
			for (int i = 0; i < aModel.GetModel()->GetMeshCount(); i++)
			{
				if (myGunOriginalTextures.size() == 0)
				{
					for (int j = 0; j < 4; j++)
					{
						myGunOriginalTextures.push_back(*aModel.GetTextures(i)[j]);
					}
				}

				aModel.SetTexture(i, 0, myLaserTextureOne);
				aModel.SetTexture(i, 1, myLaserTextureTwo);
			}
			myLoadedLaserTextures = true;
		}
	}

	if (myLoadedGrapplingTextures == true)
	{
		if (aShaderIndex == static_cast<int>(eStaticVFXType::GrapplingHook))
		{
			for (int i = 0; i < aModel.GetModel()->GetMeshCount(); i++)
			{
				if (myGunOriginalTextures.size() == 0)
				{
					for (int j = 0; j < 4; j++)
					{
						myGunOriginalTextures.push_back(*aModel.GetTextures(i)[j]);
					}
				}

				aModel.SetTexture(i, 0, myGrapplingTextureOne);
				aModel.SetTexture(i, 1, myGrapplingTextureTwo);
			}
			myLoadedGrapplingTextures = true;
		}
	}

	aGraphicsStateStack.UpdateGpuStates(true);
	modelDrawer.DrawShaderTool(aModel, myModelShaders[aShaderIndex]);
}

void ShaderTool::RenderWaterPlanes(DreamEngine::GraphicsStateStack& aGraphicsStateStack)
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::ModelDrawer& modelDrawer = engine.GetGraphicsEngine().GetModelDrawer();
	aGraphicsStateStack.SetShaderToolMiscVarsBufferData(0, 0, myTotalVFXTimer);

	for (int i = 0; i < myWaterPlaneModelInstances.size(); i++)
	{
		modelDrawer.DrawShaderTool(myWaterPlaneModelInstances[i], myModelShaders[static_cast<int>(eStaticVFXType::WaterPlane)]);
	}

}

void ShaderTool::RenderUnityLoadedModels(DreamEngine::GraphicsStateStack& aGraphicsStateStack)
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::ModelDrawer& modelDrawer = engine.GetGraphicsEngine().GetModelDrawer();
	aGraphicsStateStack.SetShaderToolMiscVarsBufferData(0, 0, myTotalVFXTimer);

	for (int i = 0; i < myUnityModelInstances.size(); i++)
	{
		modelDrawer.DrawShaderTool(myUnityModelInstances[i], myUnityModelShaders[i]);
	}
}

void ShaderTool::SetPlayerPos(DE::Vector3f aPlayerPos)
{
	myPlayerPos = aPlayerPos;
}

void ShaderTool::SetPlayerDirection(DE::Vector3f aPlayerDir)
{
	myPlayerDir = aPlayerDir;
}

void ShaderTool::SetWaterPlanePos(DE::Vector3f aBoatPos)
{
	for (int i = 0; i < myWaterPlaneModelInstances.size(); i++)
	{
		for (int j = 0; j < myWaterGridSize; j++)
		{

		}
	}

	//myModelInstances[0].SetLocation(DE::Vector3f{ aBoatPos.x + 2000.f, myModelInstances[0].GetTransform().GetPosition().y, aBoatPos.z + 3000.0f});
	//myModelInstances[1].SetLocation(DE::Vector3f{ aBoatPos.x - 4000.f, myModelInstances[0].GetTransform().GetPosition().y, aBoatPos.z + 3000.0f});
	//myModelInstances[2].SetLocation(DE::Vector3f{ aBoatPos.x - 4000.f, myModelInstances[0].GetTransform().GetPosition().y, aBoatPos.z - 3000.0f});
}

void ShaderTool::AddModelInstance(DreamEngine::ModelInstance aVFXModelInstance, std::vector<DE::Texture*> someTextures)
{
	/*for (int i = 0; i < someTextures.size(); i++)
	{
		aVFXModelInstance.SetTexture(0,i+1, someTextures[i]);
	}*/
	someTextures;
	myUnityModelInstances.push_back(aVFXModelInstance);
}

void ShaderTool::AddShaderInstance(DreamEngine::ModelShader* aModelShader)
{
	myUnityModelShaders.push_back(aModelShader);
}
