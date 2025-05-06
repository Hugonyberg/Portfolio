#include "TextureLoading.h"
#include "DreamEngine/graphics/ModelFactory.h"
#include<DreamEngine/windows/settings.h>

namespace CustomLoad
{
	DreamEngine::ModelInstance LoadModel(std::wstring& aPath, bool aAssignMaterial)
	{
		aPath = DreamEngine::Settings::ResolveAssetPathW(aPath);

		if (!std::filesystem::exists(aPath) || aPath == L"../Assets/3D/") {
			aPath = L"../Assets/3D/cubePrimitive.fbx";
		}
		aAssignMaterial;
		DreamEngine::ModelInstance outModel;
		DreamEngine::ModelFactory& mf = DreamEngine::ModelFactory::GetInstance();
		static std::unordered_map<std::wstring, DreamEngine::ModelInstance> instances;	// Since GetModelInstance doesn't work as intended, we do its job for it.
		auto it = instances.find(aPath);										// We save the models we've already found previously, and don't look for it
		if (it == instances.end()) {											// if it already exists in the unordered map.
			outModel = mf.GetModelInstance(aPath);				// Should probably move later but works for now.
			instances.insert(std::make_pair(aPath, outModel));
		}
		else {
			outModel = it->second;
		}
		return outModel;
	}

	DreamEngine::AnimatedModelInstance LoadAnimatedModel(std::wstring& aPath)
	{
		if (!std::filesystem::exists(aPath) || aPath == L"../EngineAssets/") {
			aPath = L"Models/Pengin_ANIM_Idle.fbx";
		}
		DreamEngine::AnimatedModelInstance outModel;

		DreamEngine::ModelFactory& mf = DreamEngine::ModelFactory::GetInstance();
		static std::unordered_map<std::wstring, DreamEngine::AnimatedModelInstance> instances;	
		auto it = instances.find(aPath);											
		if (it == instances.end()) {													
			outModel = mf.GetAnimatedModelInstance(aPath);							
			instances.insert(std::make_pair(aPath, outModel));
		}
		else {
			outModel = it->second;
		}
		return outModel;
	}

	void LoadTexture(DreamEngine::ModelInstance& aModelInstance, std::wstring& aPath)
	{
		std::shared_ptr<DreamEngine::Model> Mdl = aModelInstance.GetModel();
		CustomLoad::AssignMaterials(aPath, Mdl.get(), aModelInstance);
	}
	void LoadTexture(DreamEngine::AnimatedModelInstance& aModelInstance, std::wstring& aPath)
	{

		std::shared_ptr<DreamEngine::Model> Mdl = aModelInstance.GetModel();
		CustomLoad::AssignMaterials(aPath, Mdl.get(), aModelInstance);
	}

	void AssignMaterials(const std::wstring& someFilePath, std::shared_ptr<DreamEngine::Model> Mdl, DreamEngine::ModelInstancer& MI)
	{
		std::wstring baseFileName = someFilePath;
		std::wstring path = someFilePath;
		static std::unordered_map<std::wstring, DreamEngine::TextureResource*> textures;
		for (int i = 0; i < Mdl->GetMeshCount(); i++)
		{
			DreamEngine::Engine& engine = *DreamEngine::Engine::GetInstance();
			DreamEngine::TextureResource* albedoTexture = nullptr;
			std::wstring materialFileName = path;

			int aSizeForHeightAndWith;
			int templength = someFilePath.length();
			if (templength > 10)
			{
				if (someFilePath[templength - 3] == L'T' && someFilePath[templength - 2] == L'i'&&someFilePath[templength - 1] == L'l')
				{
					albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/Wall_Tile_01_c.dds");
				}
				else if (someFilePath[templength - 3] == L'C' && someFilePath[templength - 2] == L'o' && someFilePath[templength - 1] == L'r')
				{
					albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/Wall_Corrugated_01_c.dds");
				}
				else if (someFilePath[templength - 4] == L'P' && someFilePath[templength - 3] == L'i' && someFilePath[templength - 2] == L'p' && someFilePath[templength - 1] == L'e')
				{
					albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/PipeKit_01_c.dds");
				}
				else if (someFilePath[templength - 3] == L'F' && someFilePath[templength - 2] == L'a' && someFilePath[templength - 1] == L'n')
				{
					albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/Fans_01_c.dds");
				}
				else if (someFilePath[templength - 4] == L'R' && someFilePath[templength - 3] == L'a' && someFilePath[templength - 2] == L'i' && someFilePath[templength - 1] == L'l')
				{
					albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_rails_01_c.dds");
				}
				else if (someFilePath[templength - 4] == L'S' && someFilePath[templength - 3] == L'i' && someFilePath[templength - 2] == L'g' && someFilePath[templength - 1] == L'n')
				{
					albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/decalatlas_01_c.dds");
				}
				else if (someFilePath[templength - 4] == L'R' && someFilePath[templength - 3] == L'o' && someFilePath[templength - 2] == L'o' && someFilePath[templength - 1] == L'f')
				{
					albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/Wall_02_c.dds");
				}
				else if (someFilePath[templength - 4] == L'D' && someFilePath[templength - 3] == L'i' && someFilePath[templength - 2] == L'r' && someFilePath[templength - 1] == L't')
				{
					albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/GroundDirt_01_c.dds");
				}
				else if (someFilePath[templength - 4] == L'C' && someFilePath[templength - 3] == L'a' && someFilePath[templength - 2] == L'v' && someFilePath[templength - 1] == L'e')
				{
					albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/CaveWall_01_c.dds");
				}
				else if (someFilePath[templength - 6] == L'C' && someFilePath[templength - 5] == L'a' && someFilePath[templength - 4] == L'v' && someFilePath[templength - 3] == L'e')
				{
					albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/CaveWall_02_c.dds");
				}
				else if (someFilePath[templength - 5] == L'c' && someFilePath[templength - 4] == L'a' && someFilePath[templength - 3] == L'b' && someFilePath[templength - 2] == L'l' && someFilePath[templength - 1] == L'e')
				{
					albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/cable_c.dds");
				}
				else if (someFilePath[templength - 10] == L'a' && someFilePath[templength - 9] == L't' && someFilePath[templength - 8] == L'e')
				{
					albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/wall_01_c.dds");
				}
				else if (someFilePath[templength - 11] == L'L' && someFilePath[templength - 10] == L'a' && someFilePath[templength - 9] == L'm' && someFilePath[templength - 8] == L'p' && someFilePath[templength - 7] == L'W' && someFilePath[templength - 6] == L'a' && someFilePath[templength - 5] == L'l' && someFilePath[templength - 4] == L'l' && someFilePath[templength - 1] == L'1')
				{
					albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/sm_en_lampwall_01_c.dds");
				}
				else if (someFilePath[templength - 11] == L'L' && someFilePath[templength - 10] == L'a' && someFilePath[templength - 9] == L'm' && someFilePath[templength - 8] == L'p' && someFilePath[templength - 7] == L'W' && someFilePath[templength - 6] == L'a' && someFilePath[templength - 5] == L'l' && someFilePath[templength - 4] == L'l' && someFilePath[templength - 1] == L'2')
				{
					albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/sm_en_lampwall_02_c.dds");
				}
			}

			if (albedoTexture == nullptr)
			{
				std::wstring fnAlbedo = materialFileName + L"_C.dds";
				if (std::filesystem::exists(fnAlbedo))
				{
					albedoTexture = engine.GetTextureManager().GetTexture(fnAlbedo.c_str());
				}
			}

			if (albedoTexture == nullptr)
			{
				std::wstring fnAlbedo = materialFileName + L"_D.dds";
				if (std::filesystem::exists(fnAlbedo))
				{
					albedoTexture = engine.GetTextureManager().GetTexture(fnAlbedo.c_str());
				}
			}

			if (albedoTexture == nullptr)
			{
				std::wstring fnAlbedo = baseFileName + L"_C.dds";
				if (std::filesystem::exists(fnAlbedo))
				{
					albedoTexture = engine.GetTextureManager().GetTexture(fnAlbedo.c_str());
				}
			}

			if (albedoTexture == nullptr)
			{
				std::wstring fnAlbedo = baseFileName + L"_D.dds";
				if (std::filesystem::exists(fnAlbedo))
				{
					albedoTexture = engine.GetTextureManager().GetTexture(fnAlbedo.c_str());
				}
			}

			if (albedoTexture == nullptr)
				albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_c.dds");

			MI.SetTexture(i, 0, albedoTexture);

			DreamEngine::TextureResource* normalTexture = nullptr;

			if (templength > 10)
			{
				if (someFilePath[templength - 3] == L'T' && someFilePath[templength - 2] == L'i' && someFilePath[templength - 1] == L'l')
				{
					normalTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/Wall_Tile_01_N.dds");
				}
				else if (someFilePath[templength - 3] == L'C' && someFilePath[templength - 2] == L'o' && someFilePath[templength - 1] == L'r')
				{
					normalTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_N.dds");
				}
				else if (someFilePath[templength - 4] == L'P' && someFilePath[templength - 3] == L'i' && someFilePath[templength - 2] == L'p' && someFilePath[templength - 1] == L'e')
				{
					normalTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_N.dds");
				}
				else if (someFilePath[templength - 3] == L'F' && someFilePath[templength - 2] == L'a' && someFilePath[templength - 1] == L'n')
				{
					normalTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_N.dds");
				}
				else if (someFilePath[templength - 4] == L'R' && someFilePath[templength - 3] == L'a' && someFilePath[templength - 2] == L'i' && someFilePath[templength - 1] == L'l')
				{
					normalTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_N.dds");
				}
				else if (someFilePath[templength - 4] == L'S' && someFilePath[templength - 3] == L'i' && someFilePath[templength - 2] == L'g' && someFilePath[templength - 1] == L'n')
				{
					normalTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/decalatlas_01_n.dds");
				}
				else if (someFilePath[templength - 4] == L'R' && someFilePath[templength - 3] == L'o' && someFilePath[templength - 2] == L'o' && someFilePath[templength - 1] == L'f')
				{
					normalTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_N.dds");
				}
				else if (someFilePath[templength - 4] == L'D' && someFilePath[templength - 3] == L'i' && someFilePath[templength - 2] == L'r' && someFilePath[templength - 1] == L't')
				{
					normalTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/GroundDirt_01_n.dds");
				}
				else if (someFilePath[templength - 4] == L'C' && someFilePath[templength - 3] == L'a' && someFilePath[templength - 2] == L'v' && someFilePath[templength - 1] == L'e')
				{
					normalTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_N.dds");
				}
				else if (someFilePath[templength - 6] == L'C' && someFilePath[templength - 5] == L'a' && someFilePath[templength - 4] == L'v' && someFilePath[templength - 3] == L'e')
				{
					normalTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_N.dds");
				}
				else if (someFilePath[templength - 5] == L'c' && someFilePath[templength - 4] == L'a' && someFilePath[templength - 3] == L'b' && someFilePath[templength - 2] == L'l' && someFilePath[templength - 1] == L'e')
				{
					normalTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/cable_n.dds");
				}
				else if (someFilePath[templength - 10] == L'a' && someFilePath[templength - 9] == L't' && someFilePath[templength - 8] == L'e')
				{
					normalTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_N.dds");
				}
			}

			if (normalTexture == nullptr)
			{
				std::wstring fnNormal = materialFileName + L"_N.dds";
				if (std::filesystem::exists(fnNormal))
				{
					normalTexture = engine.GetTextureManager().GetTexture(fnNormal.c_str());
				}
			}

			if (normalTexture == nullptr)
			{
				std::wstring fnNormal = baseFileName + L"_N.dds";
				if (std::filesystem::exists(fnNormal))
				{
					normalTexture = engine.GetTextureManager().GetTexture(fnNormal.c_str());
				}
			}

			if (normalTexture == nullptr)
				normalTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_N.dds", false);

			MI.SetTexture(i, 1, normalTexture);

			DreamEngine::TextureResource* materialTexture = nullptr;

			if (templength > 10)
			{
				if (someFilePath[templength - 3] == L'T' && someFilePath[templength - 2] == L'i' && someFilePath[templength - 1] == L'l')
				{
					materialTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/Wall_Tile_01_M.dds");
				}
				else if (someFilePath[templength - 3] == L'C' && someFilePath[templength - 2] == L'o' && someFilePath[templength - 1] == L'r')
				{
					materialTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/Wall_Corrugated_01_M.dds");
				}
				else if (someFilePath[templength - 4] == L'P' && someFilePath[templength - 3] == L'i' && someFilePath[templength - 2] == L'p' && someFilePath[templength - 1] == L'e')
				{
					materialTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/PipeKit_01_m.dds");
				}
				else if (someFilePath[templength - 3] == L'F' && someFilePath[templength - 2] == L'a' && someFilePath[templength - 1] == L'n')
				{
					materialTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/Fans_01_m.dds");
				}
				else if (someFilePath[templength - 4] == L'R' && someFilePath[templength - 3] == L'a' && someFilePath[templength - 2] == L'i' && someFilePath[templength - 1] == L'l')
				{
					materialTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/rails_01_m.dds");
				}
				else if (someFilePath[templength - 4] == L'S' && someFilePath[templength - 3] == L'i' && someFilePath[templength - 2] == L'g' && someFilePath[templength - 1] == L'n')
				{
					materialTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/decalatlas_01_m.dds");
				}
				else if (someFilePath[templength - 4] == L'R' && someFilePath[templength - 3] == L'o' && someFilePath[templength - 2] == L'o' && someFilePath[templength - 1] == L'f')
				{
					materialTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/Wall_02_m.dds");
				}
				else if (someFilePath[templength - 4] == L'D' && someFilePath[templength - 3] == L'i' && someFilePath[templength - 2] == L'r' && someFilePath[templength - 1] == L't')
				{
					materialTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/GroundDirt_01_m.dds");
				}
				else if (someFilePath[templength - 4] == L'C' && someFilePath[templength - 3] == L'a' && someFilePath[templength - 2] == L'v' && someFilePath[templength - 1] == L'e')
				{
					materialTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/CaveWall_01_m.dds");
				}
				else if (someFilePath[templength - 6] == L'C' && someFilePath[templength - 5] == L'a' && someFilePath[templength - 4] == L'v' && someFilePath[templength - 3] == L'e')
				{
					materialTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/CaveWall_02_m.dds");
				}
				else if (someFilePath[templength - 5] == L'c' && someFilePath[templength - 4] == L'a' && someFilePath[templength - 3] == L'b' && someFilePath[templength - 2] == L'l' && someFilePath[templength - 1] == L'e')
				{
					materialTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/cable_m.dds");
				}
				else if (someFilePath[templength - 10] == L'a' && someFilePath[templength - 9] == L't' && someFilePath[templength - 8] == L'e')
				{
					materialTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/wall_01_m.dds");
				}
				else if (someFilePath[templength - 10] == L'r' && someFilePath[templength - 9] == L'c' && someFilePath[templength - 1] == L'1')
				{
					materialTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/SM_en_torchlamp_01_m.dds");
				}
			}

			if (materialTexture == nullptr)
			{
				std::wstring fnMaterial = materialFileName + L"_M.dds";
				materialTexture = engine.GetTextureManager().TryGetTexture(fnMaterial.c_str(), false);
			}

			if (materialTexture == nullptr)
			{
				std::wstring fnMaterial = baseFileName + L"_M.dds";
				materialTexture = engine.GetTextureManager().TryGetTexture(fnMaterial.c_str(), false);
			}

			if (materialTexture == nullptr)
				materialTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_M.dds", false);

			MI.SetTexture(i, 2, materialTexture);

			DreamEngine::TextureResource* emissiveTexture = nullptr;

			if (emissiveTexture == nullptr)
			{
				std::wstring fnEmissive = materialFileName + L"_fx.dds";
				if (std::filesystem::exists(fnEmissive))
				{
					emissiveTexture = engine.GetTextureManager().GetTexture(fnEmissive.c_str());
				}
			}

			if (emissiveTexture == nullptr)
			{
				emissiveTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_fx.dds", false);
			}

			MI.SetTexture(i, 3, emissiveTexture);
		}
	}
	void AssignMaterials(const std::wstring& someFilePath, DreamEngine::Model* Mdl, DreamEngine::ModelInstance& MI)
	{
		std::wstring baseFileName = someFilePath;
		std::wstring path = someFilePath;
		static std::unordered_map<std::wstring, DreamEngine::TextureResource*> textures;
		for (int i = 0; i < Mdl->GetMeshCount(); i++)
		{
			DreamEngine::Engine& engine = *DreamEngine::Engine::GetInstance();

			std::wstring materialFileName = path;

			DreamEngine::TextureResource* albedoTexture = nullptr;

			if (albedoTexture == nullptr)
			{
				std::wstring fnAlbedo = materialFileName + L"_C.dds";
				if (std::filesystem::exists(fnAlbedo))
				{
					albedoTexture = engine.GetTextureManager().GetTexture(fnAlbedo.c_str());
				}
			}

			if (albedoTexture == nullptr)
			{
				std::wstring fnAlbedo = materialFileName + L"_D.dds";
				if (std::filesystem::exists(fnAlbedo))
				{
					albedoTexture = engine.GetTextureManager().GetTexture(fnAlbedo.c_str());
				}
			}

			if (albedoTexture == nullptr)
			{
				std::wstring fnAlbedo = baseFileName + L"_C.dds";
				if (std::filesystem::exists(fnAlbedo))
				{
					albedoTexture = engine.GetTextureManager().GetTexture(fnAlbedo.c_str());
				}
			}

			if (albedoTexture == nullptr)
			{
				std::wstring fnAlbedo = baseFileName + L"_D.dds";
				if (std::filesystem::exists(fnAlbedo))
				{
					albedoTexture = engine.GetTextureManager().GetTexture(fnAlbedo.c_str());
				}
			}

			if (albedoTexture == nullptr)
				albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_c.dds");

			MI.SetTexture(i, 0, albedoTexture);

			DreamEngine::TextureResource* normalTexture = nullptr;

			if (normalTexture == nullptr)
			{
				std::wstring fnNormal = materialFileName + L"_N.dds";
				if (std::filesystem::exists(fnNormal))
				{
					normalTexture = engine.GetTextureManager().GetTexture(fnNormal.c_str());
				}
			}

			if (normalTexture == nullptr)
			{
				std::wstring fnNormal = baseFileName + L"_N.dds";
				if (std::filesystem::exists(fnNormal))
				{
					normalTexture = engine.GetTextureManager().GetTexture(fnNormal.c_str());
				}
			}

			if (normalTexture == nullptr)
				normalTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_N.dds", false);

			MI.SetTexture(i, 1, normalTexture);

			DreamEngine::TextureResource* materialTexture = nullptr;

			if (materialTexture == nullptr)
			{
				std::wstring fnMaterial = materialFileName + L"_M.dds";
				materialTexture = engine.GetTextureManager().TryGetTexture(fnMaterial.c_str(), false);
			}

			if (materialTexture == nullptr)
			{
				std::wstring fnMaterial = baseFileName + L"_M.dds";
				materialTexture = engine.GetTextureManager().TryGetTexture(fnMaterial.c_str(), false);
			}

			if (materialTexture == nullptr)
				materialTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_M.dds", false);

			MI.SetTexture(i, 2, materialTexture);

			DreamEngine::TextureResource* emissiveTexture = nullptr;

			if (emissiveTexture == nullptr)
			{
				std::wstring fnEmissive = materialFileName + L"_fx.dds";
				if (std::filesystem::exists(fnEmissive))
				{
					emissiveTexture = engine.GetTextureManager().GetTexture(fnEmissive.c_str());
				}
			}

			if (emissiveTexture == nullptr)
			{
				emissiveTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_fx.dds", false);
			}

			MI.SetTexture(i, 3, emissiveTexture);
		}
	}
	void AssignMaterials(const std::wstring& someFilePath, DreamEngine::Model* Mdl, DreamEngine::AnimatedModelInstance& MI)
	{
		std::wstring baseFileName = someFilePath;
		std::wstring path = someFilePath;
		static std::unordered_map<std::wstring, DreamEngine::TextureResource*> textures;
		for (int i = 0; i < Mdl->GetMeshCount(); i++)
		{
			DreamEngine::Engine& engine = *DreamEngine::Engine::GetInstance();

			std::wstring materialFileName = path;

			DreamEngine::TextureResource* albedoTexture = nullptr;

			if (albedoTexture == nullptr)
			{
				std::wstring fnAlbedo = materialFileName + L"_C.dds";
				if (std::filesystem::exists(fnAlbedo))
				{
					albedoTexture = engine.GetTextureManager().GetTexture(fnAlbedo.c_str());
				}
			}

			if (albedoTexture == nullptr)
			{
				std::wstring fnAlbedo = materialFileName + L"_D.dds";
				if (std::filesystem::exists(fnAlbedo))
				{
					albedoTexture = engine.GetTextureManager().GetTexture(fnAlbedo.c_str());
				}
			}

			if (albedoTexture == nullptr)
			{
				std::wstring fnAlbedo = baseFileName + L"_C.dds";
				if (std::filesystem::exists(fnAlbedo))
				{
					albedoTexture = engine.GetTextureManager().GetTexture(fnAlbedo.c_str());
				}
			}

			if (albedoTexture == nullptr)
			{
				std::wstring fnAlbedo = baseFileName + L"_D.dds";
				if (std::filesystem::exists(fnAlbedo))
				{
					albedoTexture = engine.GetTextureManager().GetTexture(fnAlbedo.c_str());
				}
			}

			if (albedoTexture == nullptr)
				albedoTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_c.dds");

			MI.SetTexture(i, 0, albedoTexture);

			DreamEngine::TextureResource* normalTexture = nullptr;

			if (normalTexture == nullptr)
			{
				std::wstring fnNormal = materialFileName + L"_N.dds";
				if (std::filesystem::exists(fnNormal))
				{
					normalTexture = engine.GetTextureManager().GetTexture(fnNormal.c_str());
				}
			}

			if (normalTexture == nullptr)
			{
				std::wstring fnNormal = baseFileName + L"_N.dds";
				if (std::filesystem::exists(fnNormal))
				{
					normalTexture = engine.GetTextureManager().GetTexture(fnNormal.c_str());
				}
			}

			if (normalTexture == nullptr)
				normalTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_N.dds", false);

			MI.SetTexture(i, 1, normalTexture);

			DreamEngine::TextureResource* materialTexture = nullptr;

			if (materialTexture == nullptr)
			{
				std::wstring fnMaterial = materialFileName + L"_M.dds";
				materialTexture = engine.GetTextureManager().TryGetTexture(fnMaterial.c_str(), false);
			}

			if (materialTexture == nullptr)
			{
				std::wstring fnMaterial = baseFileName + L"_M.dds";
				materialTexture = engine.GetTextureManager().TryGetTexture(fnMaterial.c_str(), false);
			}

			if (materialTexture == nullptr)
				materialTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_M.dds", false);

			MI.SetTexture(i, 2, materialTexture);

			DreamEngine::TextureResource* emissiveTexture = nullptr;

			if (emissiveTexture == nullptr)
			{
				std::wstring fnEmissive = materialFileName + L"_fx.dds";
				if (std::filesystem::exists(fnEmissive))
				{
					emissiveTexture = engine.GetTextureManager().GetTexture(fnEmissive.c_str());
				}
			}

			if (emissiveTexture == nullptr)
			{
				emissiveTexture = engine.GetTextureManager().GetTexture(L"../Assets/3D/T_Default_fx.dds", false);
			}

			MI.SetTexture(i, 3, emissiveTexture);
		}
	}
}