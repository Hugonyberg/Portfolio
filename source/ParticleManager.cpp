#include "ParticleManager.h"
#include <imgui/imgui.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <DreamEngine/Math/Transform.h>
#include "MainSingleton.h"

ParticleManager::ParticleManager()
{
	mySizeOf2DParticleSystems = 0;
	mySizeOf3DParticleSystems = 0;
	myPaticle2DDebugMode = false;
	myPaticle3DDebugMode = true;
	
	for (int i = 0; i < myParticleSystemNames.size(); i++)
	{
		myParticleSystemNames[i][0] = 'P';
	}
}

ParticleManager::~ParticleManager()
{
}

void ParticleManager::Add2D(ParticleSystem* aParicleSystem)
{
	my2DParticleSystems.push_back(aParicleSystem);
}

void ParticleManager::AddDebug2D(ParticleSystem aParicleSystem)
{
	my2DDebugParticleSystems.push_back(aParicleSystem);
}

void ParticleManager::Add3D(ParticleSystem3D* aParicleSystem)
{
	my3DParticleSystems.push_back(aParicleSystem);
	my3DParticleSystems[my3DParticleSystems.size()-1]->SetCameraTransform(myCameraTransform);
	//my3DParticleSystems[my3DParticleSystems.size() - 1]->SetWorldScale(myWorldScale);
}

void ParticleManager::AddDebug3D(ParticleSystem3D aParicleSystem)
{
	my3DDebugParticleSystems.push_back(aParicleSystem);
	my3DDebugParticleSystems[my3DDebugParticleSystems.size()-1].SetCameraTransform(myCameraTransform);
	my3DDebugParticleSystems[my3DDebugParticleSystems.size() - 1].SetWorldScale(myWorldScale);
}

void ParticleManager::Clear()
{
	for (int i = 0; i < my2DParticleSystems.size(); i++)
	{
		delete my2DParticleSystems[i];
	}
	my2DParticleSystems.clear();

	for (int i = 0; i < my3DParticleSystems.size(); i++)
	{
		delete my3DParticleSystems[i];
	}
	my3DParticleSystems.clear();
}

void ParticleManager::Render(DreamEngine::SpriteDrawer& aSpriteDrawer)
{
	int temp = (int)my2DParticleSystems.size();
	int temp3D = (int)my3DParticleSystems.size();
#ifndef _RETAIL
	int temptwo = (int)my2DDebugParticleSystems.size();
	int tempthree = (int)my3DDebugParticleSystems.size();
#endif // !_RETAIL
	for (int i = 0; i < temp; i++)
	{
		my2DParticleSystems[i]->Render(aSpriteDrawer);
	}
	for (int i = 0; i < temp3D; i++)
	{
		my3DParticleSystems[i]->Render(aSpriteDrawer);
	}
#ifndef _RETAIL
	if (myPaticle2DDebugMode == true)
	{
		for (int i = 0; i < temptwo; i++)
		{
			my2DDebugParticleSystems[i].Render(aSpriteDrawer);
		}
	}
	if (myPaticle3DDebugMode == true)
	{
		for (int i = 0; i < tempthree; i++)
		{
			if (myShouldResetParticleSystem == false)
			{
				my3DDebugParticleSystems[i].Render(aSpriteDrawer);
			}
		}
	}
#endif // !_RETAIL
}

void ParticleManager::RenderToGBuffer(DreamEngine::SpriteDrawer& aSpriteDrawer)
{
	int temp = (int)my2DParticleSystems.size();
	int temp3D = (int)my3DParticleSystems.size();
#ifndef _RETAIL
	int temptwo = (int)my2DDebugParticleSystems.size();
	int tempthree = (int)my3DDebugParticleSystems.size();
#endif // !_RETAIL
	for (int i = 0; i < temp; i++)
	{
		my2DParticleSystems[i]->Render(aSpriteDrawer);
	}
	for (int i = 0; i < temp3D; i++)
	{
		my3DParticleSystems[i]->Render(aSpriteDrawer);
	}
#ifndef _RETAIL
	if (myPaticle2DDebugMode == true)
	{
		for (int i = 0; i < temptwo; i++)
		{
			my2DDebugParticleSystems[i].Render(aSpriteDrawer);
		}
	}
	if (myPaticle3DDebugMode == true)
	{
		for (int i = 0; i < tempthree; i++)
		{
			if (myShouldResetParticleSystem == false)
			{
				my3DDebugParticleSystems[i].RenderToGBuffer(aSpriteDrawer);
			}
		}
	}
#endif // !_RETAIL
}

void ParticleManager::Update(float aDeltaTime)
{
	int temp = (int)my2DParticleSystems.size();
	int temp3D = (int)my3DParticleSystems.size();
#ifndef _RETAIL
	int tempDebug = (int)my2DDebugParticleSystems.size();
	int tempDebug3D = (int)my3DDebugParticleSystems.size();
#endif // !_RETAIL
	for (int i = 0; i < temp; i++)
	{
		my2DParticleSystems[i]->Update(aDeltaTime);
	}
	for (int i = 0; i < temp3D; i++)
	{
		my3DParticleSystems[i]->SetCameraTransform(myCameraTransform);
		my3DParticleSystems[i]->SetActionCamera(myActionCamera);
		my3DParticleSystems[i]->Update(aDeltaTime);
	}
#ifndef NDEBUG
	if (mySizeOf2DParticleSystems > 0)
	{
		ImGui2DDebugMenu();
		for (int i = 0; i < tempDebug; i++)
		{
			my2DDebugParticleSystems[i].Update(aDeltaTime);
		}
	}
	if (mySizeOf3DParticleSystems > 0)
	{
		ImGui3DDebugMenu();
		for (int i = 0; i < tempDebug3D; i++)
		{
			//my3DDebugParticleSystems[i].SetPos(myPlayerPosition);
			my3DDebugParticleSystems[i].SetCameraTransform(myCameraTransform);
			my3DDebugParticleSystems[i].SetActionCamera(myActionCamera);
			my3DDebugParticleSystems[i].Update(aDeltaTime);
		}
	}
#endif //NDEBUG
}

void ParticleManager::SetCameraTransform(DreamEngine::Transform* aCameraTransform)
{
	myCameraTransform = aCameraTransform;
}

void ParticleManager::SetActionCamera(bool aActionCamera)
{
	myActionCamera = aActionCamera;
}

void ParticleManager::SetWorldScale(float aScale)
{
	myWorldScale = aScale;
	for (int i = 0; i < my3DParticleSystems.size(); i++)
	{
		my3DParticleSystems[i]->SetWorldScale(myWorldScale);
	}
	for (int i = 0; i < my3DDebugParticleSystems.size(); i++)
	{
		my3DDebugParticleSystems[i].SetWorldScale(myWorldScale);
	}
}

void ParticleManager::SetPlayerPosition(DE::Vector3f aPlayerPos)
{
	myPlayerPosition = aPlayerPos;
}

#ifndef _RETAIL
void ParticleManager::ImGui2DDebugMenu()
{
	if (ImGui::Begin("2DDebugParticleSystem"))
	{
		ImGui::DragInt("Amount of particleSystems", &mySizeOf2DParticleSystems, 1, 1, 10);
		while (mySizeOf2DParticleSystems > my2DDebugParticleSystems.size())
		{
			ParticleSystem newParticleSystem;
			newParticleSystem.LoadParticleSystem("ParticleSystems/TestParticleSystem3D.json");
			//newParticleSystem.SetPos(myPlayer.GetMyPosPointer()); ***Add when player has function for position.***
			newParticleSystem.SetPos(DreamEngine::Vector3f(0, 0, 0));
			my2DDebugParticleSystems.push_back(newParticleSystem);
		}

		ImGui::DragInt("Index of 2DParticleSystems", &myCurrentParticleSystem, 0, 1, 9);
		static char newNameParticleSystem[50] = "";
		ImGui::InputText("Particle Name 2D", &newNameParticleSystem[0], CHAR_MAX);
		if (ImGui::Button("Set Name 2D"))
		{
			for (int i = 0; i < sizeof(newNameParticleSystem); i++)
			{
				myParticleSystemNames[myCurrentParticleSystem][i] = newNameParticleSystem[i];
			}
		}

		for (int i = 0; i < my2DDebugParticleSystems.size(); i++)
		{
			if (ImGui::TreeNode(myParticleSystemNames[i]))
			{
				ImGui::Text("Welcome to my 2D ParticleSystem");
				if (ImGui::DragFloat4("Color R,G,B,A", (&my2DDebugParticleSystems[i].GetDebugParticleData()->myColor.myR), 0.1f, 0, 10.f))
				{
					my2DDebugParticleSystems[i].SetDebugParticleData(*my2DDebugParticleSystems[i].GetDebugParticleData());
				}
				ImGui::DragFloat("LifeTime in Seconds", my2DDebugParticleSystems[i].GetLifeTimePtr(), 0.1f, 0, 60.f);
				ImGui::DragFloat("SpawnRate in Seconds", my2DDebugParticleSystems[i].GetSpawnRatePtr(), 0.1f, 0, 60.f);
				
				ImGui::DragFloat2("Force/Wind, X, Y", &my2DDebugParticleSystems[i].GetForcePtr()->x);
				ImGui::DragFloat("DecayTime in seconds", my2DDebugParticleSystems[i].GetParticleStartDecayTime(), 0.1f, 0, 60.f);
				ImGui::DragInt("Amount of Particles", my2DDebugParticleSystems[i].GetParticleAmountPtr(), 1, 1, 20000);
				if (ImGui::DragFloat2("Size X,Y", (&my2DDebugParticleSystems[i].GetDebugParticleData()->mySize.x), 0.1f, 0, 60.f))
				{
					my2DDebugParticleSystems[i].SetDebugParticleData(*my2DDebugParticleSystems[i].GetDebugParticleData());
				}
				ImGui::Checkbox("SpawnInCircle, True = Spawn within circle", my2DDebugParticleSystems[i].GetSpawnInCirclePtr());
				ImGui::DragFloat("Size of spawn radius", my2DDebugParticleSystems[i].GetParticleSpawnRadius(), 0.1f, 0, 60.f);
				ImGui::DragFloat("Spawn Starting Rotation", my2DDebugParticleSystems[i].GetParticleSpawnRotationPtr(), 0.1f, 0, 359.9f);
				ImGui::DragInt("Amount of Pictures in SpriteSheet", my2DDebugParticleSystems[i].GetSpriteSheetPictureAmountPtr(), 1, 0, 63);
				ImGui::Checkbox("SpawnInSquare, True = Spawn within square", my2DDebugParticleSystems[i].GetSpawnInSquarePtr());
				ImGui::DragFloat2("Min value values, X, Y", my2DDebugParticleSystems[i].GetParticleMinSpawnSquare());
				ImGui::DragFloat2("Max value values, X, Y", my2DDebugParticleSystems[i].GetParticleMaxSpawnSquare());
				if (my2DDebugParticleSystems[i].GetParticles().size() > 0)
				{
					ImGui::Checkbox("ShouldMove, False = Frozen movement", my2DDebugParticleSystems[i].GetParticleShouldMovePtr());
					ImGui::Checkbox("ShouldRotate, False = Frozen rotation", my2DDebugParticleSystems[i].GetParticleShouldRotatePtr());
					ImGui::Checkbox("ShouldShrink, True = Makes size smaller", my2DDebugParticleSystems[i].GetParticleShouldShrinkPtr());
					ImGui::Checkbox("ShouldFade, False = Frozen alpha", my2DDebugParticleSystems[i].GetParticleShouldFadePtr());

				}
				ImGui::Checkbox("SpriteSheet, False = No SpriteSheet", my2DDebugParticleSystems[i].GetSpriteSheetBoolPtr());
				ImGui::Checkbox("ShouldDie, False = lives forever", my2DDebugParticleSystems[i].GetShoudlDiePtr());
				if (ImGui::Button("Restart the ParticleSystem"))
				{
					my2DDebugParticleSystems[i].Clear();
					my2DDebugParticleSystems[i].Init((*my2DDebugParticleSystems[i].GetParticleAmountPtr()));
				}

				static char loadParticleSystem[50] = "";
				static std::string particleSystemPathName = "";
				ImGui::InputText("Particle 2D System Name", &loadParticleSystem[0], CHAR_MAX);

				if (ImGui::Button("Load ParticleSystem 2D"))
				{
					particleSystemPathName = loadParticleSystem;
					if (particleSystemPathName != "")
					{
						particleSystemPathName = "ParticleSystems/" + particleSystemPathName + ".json";
						my2DDebugParticleSystems[i].LoadParticleSystem(particleSystemPathName);
					}
				}

				static char loadParticle[50] = "";
				static std::string particlePathName = "";
				ImGui::InputText("Particle Name", &loadParticle[0], CHAR_MAX);

				if (ImGui::Button("Load Particle 2D"))
				{
					particlePathName = loadParticle;
					if (particlePathName != "")
					{
						particlePathName = "Sprites/" + particlePathName + ".dds";
						my2DDebugParticleSystems[i].ReplaceSpritePath(particlePathName);
					}
				}

				static char jsonNameCharArr[50] = "";
				static std::string jsonName = "";
				ImGui::InputText("Json Name", &jsonNameCharArr[0], CHAR_MAX);

				if (ImGui::Button("Save to Json 2D"))
				{
					jsonName = jsonNameCharArr;
					std::ofstream writeToJson("ParticleSystems/" + jsonName + ".json");
					DreamEngine::Color color = my2DDebugParticleSystems[i].GetDebugParticleData()->myColor;
					float lifeTime = *my2DDebugParticleSystems[i].GetLifeTimePtr();
					float spawnRate = *my2DDebugParticleSystems[i].GetSpawnRatePtr();
					DreamEngine::Vector3f force = *my2DDebugParticleSystems[i].GetForcePtr();
					bool shouldDie = *my2DDebugParticleSystems[i].GetShoudlDiePtr();
					DreamEngine::Vector2f size = my2DDebugParticleSystems[i].GetDebugParticleData()->mySize;
					float decayTime = *my2DDebugParticleSystems[i].GetParticles()[0].GetDecayTimePtr();
					int particleAmount = *my2DDebugParticleSystems[i].GetParticleAmountPtr();
					bool spawnInCircle = *my2DDebugParticleSystems[i].GetParticles()[0].GetSpawnInCirclePtr();
					bool spawnInSquare = *my2DDebugParticleSystems[i].GetParticles()[0].GetSpawnInSquarePtr();
					float spawnRadius = *my2DDebugParticleSystems[i].GetParticles()[0].GetSpawnRadiusPtr();
					float rotationSpeed = *my2DDebugParticleSystems[i].GetParticles()[0].GetRotationSpeedPtr();
					DreamEngine::Vector2f minSpawnSquare = *my2DDebugParticleSystems[i].GetParticles()[0].GetSpawnSquareMinPtr();
					DreamEngine::Vector2f maxSpawnSquare = *my2DDebugParticleSystems[i].GetParticles()[0].GetSpawnSquareMaxPtr();
					DreamEngine::Vector3f respawnPos = *my2DDebugParticleSystems[i].GetParticles()[0].GetRespawnPosPtr();
					DreamEngine::Vector3f dir = *my2DDebugParticleSystems[i].GetParticles()[0].GetDirPtr();
					int spriteSheetPictureAmount = *my2DDebugParticleSystems[i].GetSpriteSheetPictureAmountPtr();
					bool shouldMove = *my2DDebugParticleSystems[i].GetParticleShouldMovePtr();
					bool shouldRotate = *my2DDebugParticleSystems[i].GetParticleShouldRotatePtr();
					bool shouldShrink = *my2DDebugParticleSystems[i].GetParticleShouldShrinkPtr();
					bool shouldFade = *my2DDebugParticleSystems[i].GetParticleShouldFadePtr();
					bool spriteSheet = *my2DDebugParticleSystems[i].GetSpriteSheetBoolPtr();
					float spawnRotation = *my2DDebugParticleSystems[i].GetParticleSpawnRotationPtr();
					std::string spritePath = my2DDebugParticleSystems[i].GetSpritePaths();
					nlohmann::json json;
					json = { {"colorR", color.myR},
							{"colorG", color.myG},
							{"colorB", color.myB},
							{"colorA", color.myA},
							{"LifeTime",  lifeTime},
							{"SpawnRate", spawnRate},
							{"ForceX", force.x},
							{"ForceY", force.y},
							{"ForceZ", force.z},
							{"ShouldDie", shouldDie},
							{"SizeX", size.x},
							{"SizeY", size.y},
							{"DecayTime", decayTime},
							{"ParticleAmount", particleAmount},
							{"SpawnInCircle", spawnInCircle},
							{"SpawnInSquare", spawnInSquare},
							{"Radius", spawnRadius},
							{"RotationSpeed", rotationSpeed},
							{"MinSpawnSquareX", minSpawnSquare.x},
							{"MinSpawnSquareY", minSpawnSquare.y},
							{"MaxSpawnSquareX", maxSpawnSquare.x},
							{"MaxSpawnSquareY", maxSpawnSquare.y},
							{"RespawnPosX", respawnPos.x},
							{"RespawnPosY", respawnPos.y},
							{"RespawnPosZ", respawnPos.z},
							{"DirectionX", dir.x},
							{"SpriteSheetPictureAmount", spriteSheetPictureAmount},
							{"ShouldMove", shouldMove},
							{"ShouldRotate", shouldRotate},
							{"ShouldShrink", shouldShrink},
							{"ShouldFade", shouldFade},
							{"SpriteSheet", spriteSheet},
							{"SpawnRotation", spawnRotation},
							{"SpritePath", spritePath},

					};

					writeToJson << json;
					writeToJson.close();
				}
				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}
void ParticleManager::ImGui3DDebugMenu()
{
	if (ImGui::Begin("3DDebugParticleSystem"))
	{
		ImGui::DragInt("Amount of particleSystems 3D", &mySizeOf3DParticleSystems, 1, 1, 10);
		while (mySizeOf3DParticleSystems > my3DDebugParticleSystems.size())
		{
			ParticleSystem3D newParticleSystem;
			newParticleSystem.SetCameraTransform(myCameraTransform);
			newParticleSystem.LoadParticleSystem("ParticleSystems/TestParticleSystem3D.json");
			newParticleSystem.SetPos(DreamEngine::Vector3f(-2910, 300, -2088));
			newParticleSystem.SetWorldScale(myWorldScale);
			my3DDebugParticleSystems.push_back(newParticleSystem);
		}

		ImGui::DragInt("Index of particleSystems", &myCurrentParticleSystem, 0, 1, 9);
		static char newNameParticleSystem[50] = "";
		ImGui::InputText("Particle Name 3D", &newNameParticleSystem[0], CHAR_MAX);
		if (ImGui::Button("Set Name 3D"))
		{
			for (int i = 0; i < sizeof(newNameParticleSystem); i++)
			{
				myParticleSystemNames[myCurrentParticleSystem][i] = newNameParticleSystem[i];
			}
		}

		for (int i = 0; i < my3DDebugParticleSystems.size(); i++)
		{
			if (ImGui::TreeNode(myParticleSystemNames[i]))
			{
				ImGui::Text("Welcome to my ParticleSystem");
				if (ImGui::DragFloat4("Color R,G,B,A", (&my3DDebugParticleSystems[i].GetDebugParticleData()->myColor.myR), 0.001f, 0, 10.f))
				{
					my3DDebugParticleSystems[i].SetDebugParticleData(*my3DDebugParticleSystems[i].GetDebugParticleData());
				}
				ImGui::DragFloat("LifeTime in Seconds", my3DDebugParticleSystems[i].GetLifeTimePtr(), 0.01f, 0, 60.f);
				ImGui::DragFloat("SpawnRate in Seconds", my3DDebugParticleSystems[i].GetSpawnRatePtr(), 0.001f, 0, 60.f);
				ImGui::DragInt("Direction Min", my3DDebugParticleSystems[i].GetParticleDirMin(), 1, 0, *my3DDebugParticleSystems[i].GetParticleDirMax());
				ImGui::DragInt("Direction Max", my3DDebugParticleSystems[i].GetParticleDirMax(), 1, *my3DDebugParticleSystems[i].GetParticleDirMin(), 10000);
				ImGui::DragFloat3("Force/Wind, X, Y", &my3DDebugParticleSystems[i].GetForcePtr()->x);
				ImGui::DragFloat("DecayTime in seconds", my3DDebugParticleSystems[i].GetParticleStartDecayTime(), 0.1f, 0, 60.f);
				ImGui::DragInt("Amount of Particles", my3DDebugParticleSystems[i].GetParticleAmountPtr(), 1, 1, 200000);
				if (ImGui::DragFloat("WorldScale", (&myWorldScale), 0.01f, 1, 10000.f))
				{
					//my3DDebugParticleSystems[i].SetDebugParticleData(*my3DDebugParticleSystems[i].GetDebugParticleData());
					my3DDebugParticleSystems[i].SetWorldScale(myWorldScale);
				}
				ImGui::Checkbox("SpawnInCircle, True = Spawn within circle", my3DDebugParticleSystems[i].GetSpawnInCirclePtr());
				ImGui::DragFloat("Size of spawn radius", my3DDebugParticleSystems[i].GetParticleSpawnRadius(), 0.01f, 0, 60.f);
				ImGui::DragFloat3("Spawn Starting Rotation", &my3DDebugParticleSystems[i].GetParticleSpawnRotationPtr()->x, 0.01f, 0, 359.9f);
				ImGui::DragInt("Amount of Pictures in SpriteSheet", my3DDebugParticleSystems[i].GetSpriteSheetPictureAmountPtr(), 1, 0, 63);
				ImGui::Checkbox("SpawnInSquare, True = Spawn within square", my3DDebugParticleSystems[i].GetSpawnInSquarePtr());
				ImGui::Text("Min Position");
				ImGui::DragFloat("Min X", my3DDebugParticleSystems[i].GetParticleMinSpawnSquareX(), 0.1f, -100000.f, *my3DDebugParticleSystems[i].GetParticleMaxSpawnSquareX() - 0.1f);
				ImGui::DragFloat("Min Y", my3DDebugParticleSystems[i].GetParticleMinSpawnSquareY(), 0.1f, -100000.f, *my3DDebugParticleSystems[i].GetParticleMaxSpawnSquareY() - 0.1f);
				ImGui::DragFloat("Min Z", my3DDebugParticleSystems[i].GetParticleMinSpawnSquareZ(), 0.1f, -100000.f, *my3DDebugParticleSystems[i].GetParticleMaxSpawnSquareZ() - 0.1f);
				ImGui::Text("Max Position");
				ImGui::DragFloat("Max X", my3DDebugParticleSystems[i].GetParticleMaxSpawnSquareX(), 0.1f, *my3DDebugParticleSystems[i].GetParticleMinSpawnSquareX() + 0.1f, 100000.f);
				ImGui::DragFloat("Max Y", my3DDebugParticleSystems[i].GetParticleMaxSpawnSquareY(), 0.1f, *my3DDebugParticleSystems[i].GetParticleMinSpawnSquareY() + 0.1f, 100000.f);
				ImGui::DragFloat("Max Z", my3DDebugParticleSystems[i].GetParticleMaxSpawnSquareZ(), 0.1f, *my3DDebugParticleSystems[i].GetParticleMinSpawnSquareZ() + 0.1f, 100000.f);
				if (my3DDebugParticleSystems[i].GetParticles().size() > 0)
				{
					ImGui::Checkbox("ShouldMove, False = Frozen movement", my3DDebugParticleSystems[i].GetParticleShouldMovePtr());
					ImGui::Checkbox("ShouldRotate, False = Frozen rotation", my3DDebugParticleSystems[i].GetParticleShouldRotatePtr());
					ImGui::Checkbox("ShouldShrink, True = Makes size smaller", my3DDebugParticleSystems[i].GetParticleShouldShrinkPtr());
					ImGui::Checkbox("ShouldGrow, True = Makes size bigger", my3DDebugParticleSystems[i].GetParticleShouldGrowPtr());
					ImGui::Checkbox("ShouldFade, False = Frozen alpha", my3DDebugParticleSystems[i].GetParticleShouldFadePtr());

				}
				ImGui::Checkbox("SpriteSheet, False = No SpriteSheet", my3DDebugParticleSystems[i].GetSpriteSheetBoolPtr());
				if (*my3DDebugParticleSystems[i].GetSpriteSheetBoolPtr() == true)
				{
					my3DDebugParticleSystems[i].SetShouldSwapUV(true);
					ImGui::DragFloat("TimeBetweenSprites", my3DDebugParticleSystems[i].GetTimeUntillNextSpriteSheetPtr(), 0.001f, 0.001f);
				}
				else
				{
					my3DDebugParticleSystems[i].SetShouldSwapUV(false);
				}
				ImGui::Checkbox("ShouldDie, False = lives forever", my3DDebugParticleSystems[i].GetShoudlDiePtr());
				if (myShouldResetParticleSystem == true)
				{
					my3DDebugParticleSystems[i].Clear();
					my3DDebugParticleSystems[i].Init((*my3DDebugParticleSystems[i].GetParticleAmountPtr()));
					my3DDebugParticleSystems[i].SetWorldScale(myWorldScale);
					myShouldResetParticleSystem = false;
				}
				if (ImGui::Button("Restart the ParticleSystem 3D"))
				{
					myShouldResetParticleSystem = true;
				}

				static char loadParticleSystem[50] = "";
				static std::string particleSystemPathName = "";
				ImGui::InputText("Particle System Name 3D", &loadParticleSystem[0], CHAR_MAX);

				if (ImGui::Button("Load ParticleSystem 3D"))
				{
					particleSystemPathName = loadParticleSystem;
					if (particleSystemPathName != "")
					{
						particleSystemPathName = "ParticleSystems/" + particleSystemPathName + ".json";
						my3DDebugParticleSystems[i].LoadParticleSystem(particleSystemPathName);
					}
				}

				static char loadParticle[50] = "";
				static std::string particlePathName = "";
				ImGui::InputText("Particle Name 3D", &loadParticle[0], CHAR_MAX);

				if (ImGui::Button("Load Particle 3D"))
				{
					particlePathName = loadParticle;
					if (particlePathName != "")
					{
						particlePathName = "../Assets/2D/" + particlePathName + ".dds";
						my3DDebugParticleSystems[i].ReplaceSpritePath(particlePathName);
					}
				}

				static char jsonNameCharArr[50] = "";
				static std::string jsonName = "";
				ImGui::InputText("Json Name", &jsonNameCharArr[0], CHAR_MAX);

				if (ImGui::Button("Save to Json 3D"))
				{
					jsonName = jsonNameCharArr;
					std::ofstream writeToJson("ParticleSystems/" + jsonName + ".json");
					int particleAmount = *my3DDebugParticleSystems[i].GetParticleAmountPtr();
					if (particleAmount > 0)
					{
						DreamEngine::Color color = my3DDebugParticleSystems[i].GetDebugParticleData()->myColor;
						float lifeTime = *my3DDebugParticleSystems[i].GetLifeTimePtr();
						float spawnRate = *my3DDebugParticleSystems[i].GetSpawnRatePtr();
						DreamEngine::Vector3f force = *my3DDebugParticleSystems[i].GetForcePtr();
						//force /= myWorldScale;
						bool shouldDie = *my3DDebugParticleSystems[i].GetShoudlDiePtr();
						DreamEngine::Vector3f scale = *my3DDebugParticleSystems[i].GetParticleScaleVector();
							float decayTime = *my3DDebugParticleSystems[i].GetParticles()[0].GetDecayTimePtr();
							bool spawnInCircle = *my3DDebugParticleSystems[i].GetParticles()[0].GetSpawnInCirclePtr();
							bool spawnInSquare = *my3DDebugParticleSystems[i].GetParticles()[0].GetSpawnInSquarePtr();
							float spawnRadius = *my3DDebugParticleSystems[i].GetParticles()[0].GetSpawnRadiusPtr();
						spawnRadius /= myWorldScale;
						DreamEngine::Vector3f rotationSpeed = *my3DDebugParticleSystems[i].GetParticles()[0].GetRotationSpeedPtr();
						DreamEngine::Vector3f minSpawnSquare = *my3DDebugParticleSystems[i].GetParticles()[0].GetSpawnSquareMinPtr();
						minSpawnSquare /= myWorldScale;
						DreamEngine::Vector3f maxSpawnSquare = *my3DDebugParticleSystems[i].GetParticles()[0].GetSpawnSquareMaxPtr();
						maxSpawnSquare /= myWorldScale;
						DreamEngine::Vector3f respawnPos = *my3DDebugParticleSystems[i].GetParticles()[0].GetRespawnPosPtr();
						DreamEngine::Vector3f dir = *my3DDebugParticleSystems[i].GetParticles()[0].GetDirPtr();
						//dir /= myWorldScale;
						int spriteSheetPictureAmount = *my3DDebugParticleSystems[i].GetSpriteSheetPictureAmountPtr();
						bool shouldMove = *my3DDebugParticleSystems[i].GetParticleShouldMovePtr();
						bool shouldRotate = *my3DDebugParticleSystems[i].GetParticleShouldRotatePtr();
						bool shouldShrink = *my3DDebugParticleSystems[i].GetParticleShouldShrinkPtr();
						bool shouldFade = *my3DDebugParticleSystems[i].GetParticleShouldFadePtr();
						bool spriteSheet = *my3DDebugParticleSystems[i].GetSpriteSheetBoolPtr();
						DreamEngine::Vector3f spawnRotation = *my3DDebugParticleSystems[i].GetParticleSpawnRotationPtr();
						std::string spritePath = my3DDebugParticleSystems[i].GetSpritePaths();
						nlohmann::json jsonbajs;
						jsonbajs = { {"colorR", color.myR},
								{"colorG", color.myG},
								{"colorB", color.myB},
								{"colorA", color.myA},
								{"LifeTime",  lifeTime},
								{"SpawnRate", spawnRate},
								{"ForceX", force.x},
								{"ForceY", force.y},
								{"ForceZ", force.z},
								{"ShouldDie", shouldDie},
								{"SizeX", scale.x},
								{"SizeY", scale.y},
								{"SizeZ", scale.z},
								{"DecayTime", decayTime},
								{"ParticleAmount", particleAmount},
								{"SpawnInCircle", spawnInCircle},
								{"SpawnInSquare", spawnInSquare},
								{"Radius", spawnRadius},
								{"RotationSpeedX", rotationSpeed.x},
								{"RotationSpeedY", rotationSpeed.y},
								{"RotationSpeedZ", rotationSpeed.z},
								{"MinSpawnSquareX", minSpawnSquare.x},
								{"MinSpawnSquareY", minSpawnSquare.y},
								{"MinSpawnSquareZ", minSpawnSquare.z},
								{"MaxSpawnSquareX", maxSpawnSquare.x},
								{"MaxSpawnSquareY", maxSpawnSquare.y},
								{"MaxSpawnSquareZ", maxSpawnSquare.z},
								{"RespawnPosX", respawnPos.x},
								{"RespawnPosY", respawnPos.y},
								{"RespawnPosZ", respawnPos.z},
								{"DirectionX", dir.x},
								{"DirectionY", dir.y},
								{"DirectionZ", dir.z},
								{"SpriteSheetPictureAmount", spriteSheetPictureAmount},
								{"ShouldMove", shouldMove},
								{"ShouldRotate", shouldRotate},
								{"ShouldShrink", shouldShrink},
								{"ShouldFade", shouldFade},
								{"SpriteSheet", spriteSheet},
								{"SpawnRotationX", spawnRotation.x},
								{"SpawnRotationY", spawnRotation.y},
								{"SpawnRotationZ", spawnRotation.z},
								{"SpritePath", spritePath},
						};
						writeToJson << jsonbajs;
					}
					writeToJson.close();
				}
				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}
#endif // !_RETAIL
