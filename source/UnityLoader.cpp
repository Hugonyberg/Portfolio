#include "UnityLoader.h"
#include <assert.h>
#include <fstream> 

#include <DreamEngine/engine.h>
#include <DreamEngine/debugging/MessageBoxDebugg.h>
#include "MainSingleton.h"
#include "DreamEngine/graphics/ModelFactory.h"
#include <DreamEngine/shaders/ModelShader.h>
#include "DreamEngine/graphics/TextureManager.h"
#include <AtlBase.h>
#include <atlconv.h>

constexpr float UI_POS_SCALE{0.5f};

LevelData UnityLoader::LoadLevel(const std::string& aFilePath)
{
	std::ifstream file(aFilePath);

	if(!file)
	{
		std::wstring wFilePath(aFilePath.begin(), aFilePath.end());

		MessageBoxDebugg(L"Cannot Load Json Level", wFilePath); 
	}
	assert(file.is_open() && "Failed to open json file for LoadLevel");

	
	const nlohmann::json json = nlohmann::json::parse(file);
	file.close();

	LevelData levelData;
	int ID{0};


	// Find the last slash '/'
	size_t lastSlash = aFilePath.find_last_of("/\\");
	// Find the last dot '.'
	size_t lastDot = aFilePath.find_last_of('.');
	// Extract the part between '/' and '.'
	aFilePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
	levelData.levelName = aFilePath.substr(lastSlash + 1, lastDot - lastSlash - 1);

	for(const auto& data : json["scene_objects"])
	{
		LoadData(data, levelData, ID);
	}

	return levelData;
}

UIData UnityLoader::LoadUI(const std::string& aFilePath)
{
	std::ifstream file(aFilePath);

	if(!file)
	{
		std::wstring wFilePath(aFilePath.begin(), aFilePath.end());

		MessageBoxDebugg(L"Cannot Load Json UI", wFilePath);
	}

	assert(file.is_open() && "Failed to open json file for LoadUI");

	const nlohmann::json json = nlohmann::json::parse(file);
	file.close();

	UIData uIData;
	int ID{0};

	for(const auto& data : json["scene_objects"])
	{
		LoadUIData(data, uIData, ID);
	}

	return uIData;
}

void UnityLoader::LoadData(const nlohmann::json& someInData, LevelData& someOutData, int& aID)
{
	if (someInData.size() == 0)
	{
		MessageBoxDebugg(L"Cannot Load Json Data", L"The JSON is empty");
	}

	const std::string tag = someInData["tag"].get<std::string>();

	if (tag == "MainCamera")
	{
		const DreamEngine::Vector3f scale = GetVector3f(someInData["scale"]);
		DreamEngine::Vector4f r = GetVector4f(someInData["rotation"]);
		const DreamEngine::Quaternionf rotation(r.w, r.x, r.y, r.z);
		const DreamEngine::Vector3f position = GetVector3f(someInData["position"]) * 100.f; //Position.x = 1.0f in Unity is equal to Position.x = 100.0f in TGE
		DreamEngine::Transform transform(position, rotation, scale);

		someOutData.mainCamera.transform = transform;
		someOutData.mainCamera.foV = someInData["fov"].get<float>();
		someOutData.mainCamera.farP = someInData["far"].get<float>() * 100.f;
		someOutData.mainCamera.nearP = someInData["near"].get<float>();
	}
	else if (tag == "DirectionalLight")
	{
		const DreamEngine::Vector3f scale = GetVector3f(someInData["scale"]);

		DreamEngine::Vector4f r = GetVector4f(someInData["rotation"]);
		const DreamEngine::Quaternionf rotation(r.w, r.x, r.y, r.z);
		const DreamEngine::Vector3f position = GetVector3f(someInData["position"]) * 100.f;

		DreamEngine::Transform transform(position, rotation, scale);
		transform.SetRotation(DE::Vector3f(-transform.GetRotation().x, transform.GetRotation().y + 170.f, -transform.GetRotation().z));

		someOutData.directionalLight.transform = transform;

		someOutData.directionalLight.color = someInData.contains("color")
			? GetColor(someInData["color"])
			: DreamEngine::Color{ 0.8f, 0.8f, 0.8f };

		someOutData.directionalLight.intensity = someInData.contains("intensity")
			? someInData["intensity"].get<float>()
			: 1.f;
	}
	else if (tag == "Cubemap")
	{
		const std::string tag = someInData["tag"].get<std::string>();
		someOutData.tags.emplace(aID, tag);

		const std::string path = someInData["path"].get<std::string>();
		std::wstring w = StringToWString(path);
		someOutData.fbxPaths.emplace(aID, w);
		++aID;
	}
	else if (tag == "Setdressing" || tag == "Companion")
	{
		const DreamEngine::Vector3f scale = GetVector3f(someInData["scale"]) /** 100.f*/;
		DreamEngine::Vector4f r = GetVector4f(someInData["rotation"]);
		const DreamEngine::Quaternionf rotation(r.w, r.x, r.y, r.z);
		const DreamEngine::Vector3f position = GetVector3f(someInData["position"]) * 100.f; //Position.x = 1.0f in Unity is equal to Position.x = 100.0f in DreamEngine
		DreamEngine::Transform transform(position, rotation, scale);
		someOutData.transforms.emplace(aID, transform);

		if (someInData.contains("path"))
		{
			const std::string path = someInData["path"].get<std::string>();

			std::wstring w = StringToWString(path);
			someOutData.fbxPaths.emplace(aID, w);
		}

		someOutData.tags.emplace(aID, tag);
		++aID;
	}
	else if (tag == "SetdressingWithCollider")
	{
		const DreamEngine::Vector3f scale = GetVector3f(someInData["scale"]) /** 100.f*/;
		DreamEngine::Vector4f r = GetVector4f(someInData["rotation"]);
		const DreamEngine::Quaternionf rotation(r.w, r.x, r.y, r.z);
		const DreamEngine::Vector3f position = GetVector3f(someInData["position"]) * 100.f; //Position.x = 1.0f in Unity is equal to Position.x = 100.0f in DreamEngine
		DreamEngine::Transform transform(position, rotation, scale);
		someOutData.transforms.emplace(aID, transform);

		if (someInData.contains("path"))
		{
			const std::string path = someInData["path"].get<std::string>();

			std::wstring w = StringToWString(path);
			someOutData.fbxPaths.emplace(aID, w);
		}

		if (someInData.contains("collider"))
		{
			ColliderData collider;
			std::string type = someInData["collider"]["type"].get<std::string>();
			if (type == "SphereCollider")
			{
				collider.type = eColliderType::Sphere;
				collider.isTrigger = false;
				collider.radius = someInData["collider"]["radius"].get<int>() * 100.0f;
				collider.offset = GetVector3f(someInData["collider"]["offset"]) * 100.0f;
			}
			else if (type == "BoxCollider")
			{
				collider.type = eColliderType::Box;
				collider.isTrigger = false;
				collider.scale = GetVector3f(someInData["scale"]);
				collider.size = GetVector3f(someInData["collider"]["size"]) * 50.0f;
				collider.offset = GetVector3f(someInData["collider"]["offset"]) * 100.0f;
			}
			else if (type == "MeshCollider")
			{
				collider.type = eColliderType::Mesh;
				collider.isTrigger = false;
				collider.layerOverride = someInData["collider"]["layerOverride"];
				collider.meshData = GetMeshData(someInData["collider"]["meshData"]);
			}
			someOutData.colliders.emplace(aID, collider);
		}

		someOutData.tags.emplace(aID, tag);
		++aID;
	}
	else if (tag == "DecalBox")
	{
		DreamEngine::Vector3f scale = GetVector3f(someInData["scale"]) /** 100.f*/;
		if (someInData.contains("path"))
		{
			const std::string path = someInData["path"].get<std::string>();
			std::wstring w = StringToWString(path);
			someOutData.decalPaths.emplace(aID, w);
			scale.x = someInData["spriteScale"]["x"].get<float>();
			scale.y = someInData["spriteScale"]["y"].get<float>();
		}

		DreamEngine::Vector4f r = GetVector4f(someInData["rotation"]);
		const DreamEngine::Quaternionf rotation(r.w, r.x, r.y, r.z);
		const DreamEngine::Vector3f position = GetVector3f(someInData["position"]) * 100.f; //Position.x = 1.0f in Unity is equal to Position.x = 100.0f in DreamEngine
		DreamEngine::Transform transform(position, rotation, scale);
		someOutData.transforms.emplace(aID, transform);

		someOutData.tags.emplace(aID, tag);
		++aID;
	}
	else if (tag == "EnemySpawner")
	{
		EnemySpawnerInfo es;
		es.amountShooting = someInData["amountOfShootingEnemies"].get<int>();
		es.amountMelee = someInData["amountOfMeleEnemies"].get<int>();

		es.amountMelee != 0 ? es.isMelee = true : es.isMelee = false;
		es.amountShooting != 0 ? es.isShooting = true : es.isShooting = false;

		es.spawnerRange = someInData["collider"]["radius"].get<float>() * 100.f;

		someOutData.enemySpawners.emplace(aID, es);
		someOutData.tags.emplace(aID, tag);
		++aID;
	}
	else if (tag == "Leviathan")
	{
		if (someInData.contains("attackCoolDown"))
			someOutData.leviathanInfo.attackCoolDown = someInData["attackCoolDown"].get<float>();

		if (someInData.contains("attackingSpeed"))
			someOutData.leviathanInfo.attackingSpeed = someInData["attackingSpeed"].get<float>();
		
		if (someInData.contains("seekingSpeed"))
			someOutData.leviathanInfo.seekingSpeed = someInData["seekingSpeed"].get<float>();

		if (someInData.contains("isAttackRangeCollision"))
		{
			if (someInData["isAttackRangeCollision"] == true)
				someOutData.leviathanInfo.attackRange = someInData["collider"]["radius"].get<float>() * 100.f;
		}

		if (someInData.contains("isAttackRangeCollision"))
		{
			if (someInData["isSeekRangeCollision"] == true)
			{
				someOutData.leviathanInfo.seekRange = someInData["collider"]["radius"].get<float>() * 100.f;

				const DreamEngine::Vector3f scale = GetVector3f(someInData["scale"]) /** 100.f*/;
				DreamEngine::Vector4f r = GetVector4f(someInData["rotation"]);
				const DreamEngine::Quaternionf rotation(r.w, r.x, r.y, r.z);
				const DreamEngine::Vector3f position = GetVector3f(someInData["position"]) * 100.f; //Position.x = 1.0f in Unity is equal to Position.x = 100.0f in DreamEngine
				DreamEngine::Transform transform(position, rotation, scale);
				someOutData.transforms.emplace(aID, transform);
			}
		}

		someOutData.tags.emplace(aID, tag);
		++aID;
	}
	else if (tag == "LevithanWayPoint")
	{
		int ID = someInData["waypointID"].get<int>();
		const DreamEngine::Vector3f position = GetVector3f(someInData["position"]) * 100.f; 

		someOutData.leviathanInfo.waypoints.emplace(ID, position); 

		someOutData.tags.emplace(aID, tag);
		++aID;
	}
	else if (tag == "GridBoundary")
	{
		DreamEngine::Vector3f position = GetVector3f(someInData["position"]);
		position.x = std::round(position.x);
		position.z = std::round(position.z);
		position *= 100.0f;
		if (someOutData.gridBoundaries.first == DE::Vector2f(0.0f))
		{
			someOutData.gridBoundaries.first = DE::Vector2f(position.x, position.z);
		}
		else
		{
			someOutData.gridBoundaries.second = DE::Vector2f(position.x, position.z);
		}
	}
	else if (tag == "PointLight")
	{
		const DreamEngine::Vector3f scale = GetVector3f(someInData["scale"]) /** 100.f*/;
		DreamEngine::Vector4f r = GetVector4f(someInData["rotation"]);
		const DreamEngine::Quaternionf rotation(r.w, r.x, r.y, r.z);
		const DreamEngine::Vector3f position = GetVector3f(someInData["position"]) * 100.f; //Position.x = 1.0f in Unity is equal to Position.x = 100.0f in DreamEngine
		DreamEngine::Transform transform(position, rotation, scale);
		someOutData.transforms.emplace(aID, transform);

		PointLightData pointLightData;
		pointLightData.transform = transform;
		const DreamEngine::Vector4f color = GetColor4f(someInData["color"]);
		pointLightData.color = DE::Color(color.x, color.y, color.z, color.w);
		pointLightData.range = (someInData["range"]).get<float>() * 100.f;
		pointLightData.intensity = (someInData["intensity"]).get<float>() * 150000.f;
		someOutData.pointLights.emplace(aID, pointLightData);

		someOutData.tags.emplace(aID, tag);
		++aID;
	}
	else if (tag == "SpotLight")
	{
		const DreamEngine::Vector3f scale = GetVector3f(someInData["scale"]) /** 100.f*/;
		DreamEngine::Vector4f r = GetVector4f(someInData["rotation"]);
		const DreamEngine::Quaternionf rotation(r.w, r.x, r.y, r.z);
		const DreamEngine::Vector3f position = GetVector3f(someInData["position"]) * 100.f; //Position.x = 1.0f in Unity is equal to Position.x = 100.0f in DreamEngine
		DreamEngine::Transform transform(position, rotation, scale);
		someOutData.transforms.emplace(aID, transform);

		SpotLightData spotLightData;
		spotLightData.transform = transform;
		const DreamEngine::Vector4f color = GetColor4f(someInData["color"]);
		spotLightData.color = DE::Color(color.x, color.y, color.z, color.w);
		spotLightData.range = (someInData["range"]).get<float>() * 100.f;
		spotLightData.intensity = (someInData["intensity"]).get<float>() * 150000.f;
		someOutData.spotLights.emplace(aID, spotLightData);

		someOutData.tags.emplace(aID, tag);
		++aID;
	}
	else if (tag == "Boat"		|| tag == "BoatCannon"	|| tag == "BoatAmmo" || 
			 tag == "BoatLight"	|| tag == "BoatSpeed"	|| tag == "BoatMap" || 
			 tag == "BoatRepair"|| tag == "BoatSteer"	|| tag == "BoatWeakPoint")
	{
		const DreamEngine::Vector3f scale = GetVector3f(someInData["scale"]) /** 100.f*/;
		DreamEngine::Vector4f r = GetVector4f(someInData["rotation"]);
		const DreamEngine::Quaternionf rotation(r.w, r.x, r.y, r.z);
		const DreamEngine::Vector3f position = GetVector3f(someInData["position"]) * 100.f; //Position.x = 1.0f in Unity is equal to Position.x = 100.0f in DreamEngine
		DreamEngine::Transform transform(position, rotation, scale);
		someOutData.transforms.emplace(aID, transform);
		someOutData.boatIDs.emplace(aID, tag);

		if (someInData.contains("children") && someInData["children"].is_array())
		{
			int arraySize = someInData["children"].size();
			if (arraySize > 0)
			{
				std::cout << arraySize << std::endl;
			}
			for (auto const& childNode : someInData["children"])
			{
				const std::string childTag = childNode["tag"].get<std::string>();
				if (childTag == "Untagged")
				{
					// GET MODEL from child
					if (childNode.contains("path") && childNode["path"].is_string())
					{
						std::string modelPath = childNode["path"].get<std::string>();
						someOutData.boatModelPaths.emplace(aID, modelPath);
					}
				}
				else if (childTag == "ComponentInteractionPoint")
				{
					// GET Interaction Point from child
					if (childNode.contains("position"))
					{
						DE::Vector3f interactionPosition = GetVector3f(childNode["position"]) * 100.f;
						someOutData.componentInteractionPoint.emplace(aID, interactionPosition);
					}
				}
			}
		}
		++aID;
	}
	else if (tag == "VFX")
	{
		if (someInData.contains("modelName"))
		{
			std::string temp = "3D/" + someInData["modelName"].get<std::string>() + ".fbx";
			CA2W cw2a(temp.c_str());
			std::wstring modelPath = cw2a;
			std::wstring texturePath;// = someInData["TexOne"].get<std::wstring>() + L".dds";
			DreamEngine::ModelInstance tempInstance = DE::ModelFactory::GetInstance().GetModelInstance(modelPath);
			std::vector<DE::Texture*> tempTextures;
			for (int i = 0; i < 8; i++)
			{
				switch (i)
				{
				case 0:
				{
					std::string temp = "ShaderTextures/" + someInData["textureOneName"].get<std::string>() + ".dds";
					CA2W cw2a(temp.c_str());
					texturePath = cw2a;
					break;
				}
				case 1:
				{
					std::string temp = "ShaderTextures/" + someInData["textureTwoName"].get<std::string>() + ".dds";
					CA2W cw2a(temp.c_str());
					texturePath = cw2a;
					break;
				}
				case 2:
				{
					std::string temp = "ShaderTextures/" + someInData["textureThreeName"].get<std::string>() + ".dds";
					CA2W cw2a(temp.c_str());
					texturePath = cw2a;
					break;
				}
				case 3:
				{
					std::string temp = "ShaderTextures/" + someInData["textureFourName"].get<std::string>() + ".dds";
					CA2W cw2a(temp.c_str());
					texturePath = cw2a;
					break;
				}
				case 4:
				{
					std::string temp = "ShaderTextures/" + someInData["textureFiveName"].get<std::string>() + ".dds";
					CA2W cw2a(temp.c_str());
					texturePath = cw2a;
					break;
				}
				case 5:
				{
					std::string temp = "ShaderTextures/" + someInData["textureSixName"].get<std::string>() + ".dds";
					CA2W cw2a(temp.c_str());
					texturePath = cw2a;
					break;
				}
				case 6:
				{
					std::string temp = "ShaderTextures/" + someInData["textureSevenName"].get<std::string>() + ".dds";
					CA2W cw2a(temp.c_str());
					texturePath = cw2a;
					break;
				}
				case 7:
				{
					std::string temp = "ShaderTextures/" + someInData["textureEightName"].get<std::string>() + ".dds";
					CA2W cw2a(temp.c_str());
					texturePath = cw2a;
					break;
				}
				default:
					break;
				}

				if (texturePath.size() >= 22)
				{
					 DE::Texture* newTexture = DE::Engine::GetInstance()->GetTextureManager().GetTexture(texturePath.c_str());
					tempTextures.push_back(newTexture);
				}
			}

			DreamEngine::ModelShader* tempModelShader = new DreamEngine::ModelShader(DreamEngine::Engine::GetInstance());
			temp = "Shaders/" + someInData["shaderName"].get<std::string>() + ".cso";
			CA2W cw2aa(temp.c_str());
			std::wstring tempShaderString = cw2aa;

			if (!tempModelShader->Init(L"Shaders/ShaderDesignerVS.cso", tempShaderString.c_str()))
			{
				//return false;
			}

			const DreamEngine::Vector3f scale = GetVector3f(someInData["scale"]) /** 100.f*/;
			DreamEngine::Vector4f r = GetVector4f(someInData["rotation"]);
			const DreamEngine::Quaternionf rotation(r.w, r.x, r.y, r.z);
			const DreamEngine::Vector3f position = GetVector3f(someInData["position"]) * 100.f; //Position.x = 1.0f in Unity is equal to Position.x = 100.0f in DreamEngine
			DreamEngine::Transform transform(position, rotation, scale);
			tempInstance.SetTransform(transform);
			someInData["modelName"].get<std::string>();
			for (int i = 0; i < tempTextures.size(); i++)
			{
				tempInstance.SetTexture(0, i, tempTextures[i]);
			}
			MainSingleton::GetInstance()->GetShaderTool().AddModelInstance(tempInstance, tempTextures);
			MainSingleton::GetInstance()->GetShaderTool().AddShaderInstance(tempModelShader);
		}
	}
	else
	{
		if (someInData.contains("children"))
		{
			for (const auto& dataChild : someInData["children"])
			{
				LoadData(dataChild, someOutData, aID);
			}
		}
	}
}

void UnityLoader::LoadUIData(const nlohmann::json & aData, UIData & someData, int& aID)
{
	const std::string tag = aData["tag"].get<std::string>();
	someData.tags.emplace(aID, tag);

	UIInstanceData u;
	// UI_POS_SCALE here is a handcrafted tweak for UI pos scaling. 
	// Should work for all imports. If not this needs to be investigated. 
	u.position = GetVector2f(aData["position"]) * 100.f * UI_POS_SCALE;
	u.scale = GetVector2f(aData["scale"]);
	if(aData.contains("pivot"))
	{
		u.pivot = GetVector2f(aData["pivot"]);
	}

	someData.uIData.emplace(aID, u);

	if(aData.contains("path"))
	{
		const std::string path = aData["path"].get<std::string>();
		std::wstring w = StringToWString(path);
		someData.spritePaths.emplace(aID, w);
	}

	if(aData["tag"].get<std::string>() == "HUD")
	{
		if(aData["healthForeground"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::HealthForeground);
		}
		if(aData["moneyCurrencyTextPosition"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::MoneyCurrencyText);
		}
		if(aData["grapplingHookIcon"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::GrapplingHookIndicator);
		}
		if (aData["ammoForeground"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::AmmoForeground);
		}
		if (aData["ammoNumberPosition"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::AmmoNumber);
		}
		if (aData["companionTurretCommand"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::CompanionTurretCommand);
		}
		if (aData["companionHealthCommand"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::CompanionHealthCommand);
		}
		if (aData["crosshair"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::Crosshair);
		}
		if (aData["defensePointAPosition"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::DefensePointA);
		}
		if (aData["defensePointBPosition"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::DefensePointB);
		}
		if (aData["defensePointCPosition"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::DefensePointC);
		}
		if (aData["enemySpawn1"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::EnemySpawn1);
		}
		if (aData["enemySpawn2"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::EnemySpawn2);
		}
		if (aData["enemySpawn3"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::EnemySpawn3);
		}
		if (aData["waveNumberCounterPosition"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::WaveNumberCounter);
		}
		if (aData["enemyAmountCounterPosition"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::EnemyAmountCounter);
		}

		if(aData["controlsView"] == true)
		{
			someData.HUD.emplace(aID, eHUDAction::ControlsView);
		}
	}

	if(aData["tag"].get<std::string>() == "UIBackground")
	{
	}

	if(aData["tag"].get<std::string>() == "UIButton")
	{
		if(aData["mainMenuButton"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::MainMenu);
		}
		if(aData["levelSelectButton"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::LevelSelect);
		}
		if(aData["settingsButton"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::Settings);
		}
		if(aData["creditsButton"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::Credits);
		}
		if(aData["exitButton"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::Exit);
		}
		if(aData["muteButton"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::Mute);
		}
		if(aData["mainVolumeSlider"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::MainVolume);
		}
		if(aData["sfxVolumeSlider"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::SfxVolume);
		}
		if(aData["musicVolumeSlider"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::MusicVolume);
		}
		if(aData["isFOVSlider"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::FOV);
		}
		if(aData["screenSizeFullButton"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::ScreenSizeFull);
		}
		if(aData["resolution1280x720"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::Resolution1280x720);
		}
		if(aData["resolution1920x1080"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::Resolution1920x1080);
		}
		if(aData["resolution2560x1440"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::Resolution2560x1440);
		}
		if(aData["levelOneButton"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::LevelOne);
		}
		if(aData["levelTwoButton"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::LevelTwo);
		}
		if(aData["backButton"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::Back);
		}
		if(aData["resumeButton"] == true)
		{
			someData.UIbutton.emplace(aID, eUIButton::Resume);
		}
		if(aData["playerGymButton"] == true)
		{
#ifdef _RETAIL
			someData.UIbutton.emplace(aID, eUIButton::Count);
			return;
#endif
			someData.UIbutton.emplace(aID, eUIButton::PlayerGym);
		}
		if(aData["assetGymButton"] == true)
		{
#ifdef _RETAIL
			someData.UIbutton.emplace(aID, eUIButton::Count);
			return;
#endif
			someData.UIbutton.emplace(aID, eUIButton::AssetGym);
		}

	}

	++aID;
}

DreamEngine::Vector4f UnityLoader::GetVector4f(const nlohmann::json & aJson)
{
	const float x = aJson["x"].get<float>();
	const float y = aJson["y"].get<float>();
	const float z = aJson["z"].get<float>();
	//float w = 0.f;
	//if (aJson.contains("w"))
	//{
	const float w = aJson["w"].get<float>();
	//}

	return DreamEngine::Vector4f{x, y, z, w}; 
}

DreamEngine::Vector4f UnityLoader::GetColor4f(const nlohmann::json& aJson)
{
	const float x = aJson["r"].get<float>();
	const float y = aJson["g"].get<float>();
	const float z = aJson["b"].get<float>();
	//float w = 0.f;
	//if (aJson.contains("w"))
	//{
	const float w = aJson["a"].get<float>();
	//}

	return DreamEngine::Vector4f{ x, y, z, w };
}

DreamEngine::Vector3f UnityLoader::GetVector3f(const nlohmann::json& aJson)
{
	const float x = aJson["x"].get<float>();
	const float y = aJson["y"].get<float>();
	const float z = aJson["z"].get<float>();

	return DreamEngine::Vector3f{x, y, z};
}

DreamEngine::Vector2f UnityLoader::GetVector2f(const nlohmann::json& aJson)
{
	const float x = aJson["x"].get<float>();
	const float y = aJson["y"].get<float>();

	return DreamEngine::Vector2f{x, y};
}

std::wstring UnityLoader::StringToWString(const std::string& str)
{
	return std::filesystem::path(str).wstring();
}

DreamEngine::Color UnityLoader::GetColor(const nlohmann::json& aJson)
{
	return
	{
		aJson["r"].get<float>(),
		aJson["g"].get<float>(),
		aJson["b"].get<float>(),
		aJson["a"].get<float>(),
	};
}

MeshData UnityLoader::GetMeshData(const nlohmann::json& aJson)
{
	MeshData data;
	data.vertexCount = aJson["vertexCount"].get<int>();
	for (const auto& vertex : aJson["vertices"])
	{
		auto deVec = GetVector3f(vertex) * 100.0f;
		data.vertices.push_back(physx::PxVec3(deVec.x, deVec.y, deVec.z));
	}
	for (const auto& indexCollection : aJson["indices"])
	{
		data.indices.push_back(std::vector<int>());
		for (const auto& index : indexCollection.items())
		{
			data.indices.back().push_back(index.value().get<int>());
		}
	}
	return data;
}