#include "ParticleSystem.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <DreamEngine/engine.h>
#include <DreamEngine/graphics/TextureManager.h>
#include <DreamEngine/graphics/SpriteDrawer.h>
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/graphics/sprite.h>
#include <DreamEngine/windows/settings.h>

ParticleSystem::ParticleSystem()
{
	myPos = { 0,0,0 };
	myPosPtr = nullptr;
	myLifeTime = 10.0f;
	mySpawnRate = 0.0f;
	myShouldDie = true;
	myParticleStartInstance.myColor = { 1,1,1,1 };
	myParticleStartInstance.mySize = 1.0f;
	myParticleStartInstance.myRotation = 0;
	myParticleStartDecayTime = 1.0f;
	mySpriteSheet = false;
	myParticlesShouldMove = true;
	myParticlesShouldRotate	= true;
	myParticlesShouldShrink	= true;
	myParticlesShouldFade = true;
	mySpriteSheetPictureAmount = 63;
	myParticleAmount = 0;
	myLoadedData = false;
	myTime = 0;
	myCount = 0;
	myPlay = true;
	mySpriteSheetStartUV = 0;
	myAddingUVX = 0;
	myAddingUVY = 0;
	mySpriteSheetHeight = 0;
	mySpriteSheetWidth = 0;
}

ParticleSystem::~ParticleSystem()
{
	int temp = 0;
	temp;
}

void ParticleSystem::SetDebug(bool aBool)
{
	myDebug = aBool;
}

void ParticleSystem::SetLifeTime(float aValue)
{
	myLifeTime = aValue;
}

void ParticleSystem::SetSpawnRate(float aValue)
{
	mySpawnRate = aValue;
}

void ParticleSystem::SetPos(DreamEngine::Vector3f aPos)
{
	myPos = aPos;
}

void ParticleSystem::SetPos(DreamEngine::Vector3f* aPos)
{
	myPosPtr = aPos;
}

std::vector<Particle>& ParticleSystem::GetParticles()
{
	return myParticles;
}

void ParticleSystem::Update(float aDeltaTime)
{
	if (myPlay == true)
	{


		myTotalTime += aDeltaTime;
		myTime += aDeltaTime;

		if (myParticles.size() > 0)
		{
			if (myParticles.size() < myParticleAmount)
			{
				Particle particle;
				if (myPosPtr != NULL)
				{
					particle.Init(*myPosPtr, myParticleStartInstance.myColor, myParticleStartInstance.mySize, myParticleStartDecayTime, { (float)myParticlesShouldMove,(float)myParticlesShouldRotate,(float)myParticlesShouldShrink,(float)myParticlesShouldFade });
				}
				else
				{
					particle.Init(myPos, myParticleStartInstance.myColor, myParticleStartInstance.mySize, myParticleStartDecayTime, { (float)myParticlesShouldMove,(float)myParticlesShouldRotate,(float)myParticlesShouldShrink,(float)myParticlesShouldFade });
				}
				particle.SetForce(myForce);
				particle.SetAlpha(0);
				myParticles.push_back(particle);
				myParticleInstances.push_back(particle.GetSpriteInstance());
			}

			while (myParticles.size() > myParticleAmount)
			{
				myParticles[myParticles.size() - 1].SetAlpha(0);
				myParticles.pop_back();
				//delete myParticleInstances[myParticleInstances.size() - 1];
				myParticleInstances.pop_back();
			}

#ifndef _RETAIL

			if (myDebugParticleData.myColor == myParticles[0].GetSpriteInstance()->myColor && myDebugParticleData.mySize == myParticles[0].GetSpriteInstance()->mySize)
			{

			}
			else
			{
				for (size_t i = 0; i < myParticles.size(); i++)
				{
					myParticles[i].SetForce(myForce);
					myParticles[i].SetSpawnInCircle(myParticleSpawnInCircle);
					myParticles[i].SetSpawnInSquare(myParticleSpawnInSquare);
					if (mySpawnRadius > 0)
					{
						myParticles[i].SetSpawnRadius(mySpawnRadius);
					}
					if (myParticleMinSpawnSquare.x < myParticleMaxSpawnSquare.x && myParticleMinSpawnSquare.y < myParticleMaxSpawnSquare.y)
					{
						myParticles[i].SetMinSpawnSquare(myParticleMinSpawnSquare);
						myParticles[i].SetMaxSpawnSquare(myParticleMaxSpawnSquare);
					}
				}
				myParticleStartInstance.myColor = myDebugParticleData.myColor;
				myParticleStartInstance.mySize = myDebugParticleData.mySize;

				if (myParticlesShouldMove != *myParticles[0].GetShouldMovePtr())
				{
					SetShouldMove(myParticlesShouldMove);
				}
				if (myParticlesShouldRotate != *myParticles[0].GetShouldRotatePtr())
				{
					SetShouldRotate(myParticlesShouldRotate);
				}
				if (myParticlesShouldShrink != *myParticles[0].GetShouldShrinkPtr())
				{
					SetShouldShrink(myParticlesShouldShrink);
				}
				if (myParticlesShouldFade != *myParticles[0].GetShouldFadePtr())
				{
					SetShouldFade(myParticlesShouldFade);
				}
			}

#endif // !_RETAIL


			for (int i = 0; i < myParticles.size(); i++)
			{
				if (mySpriteSheet == true)
				{
					if (myUvs.size() == 0)
					{
						InitSpriteSheet();
					}

					myParticles[i].PlaySpriteSheet(aDeltaTime, mySpriteSheetPictureAmount, mySpriteSheetStartUV, myShouldStopAfterSpriteSheet);
					if (myParticles.size() > i)
					{
						myParticleInstances[i]->myTextureRect = { myUvs[myParticles[i].GetUVIndex()].myStart.x, myUvs[myParticles[i].GetUVIndex()].myStart.y, myUvs[myParticles[i].GetUVIndex()].myEnd.x, myUvs[myParticles[i].GetUVIndex()].myEnd.y };
					}
				}
				myParticles[i].SetSpawnInCircle(myParticleSpawnInCircle);
				myParticles[i].SetSpawnInSquare(myParticleSpawnInSquare);

				if (mySpawnRadius > 0)
				{
					myParticles[i].SetSpawnRadius(mySpawnRadius);
				}
				if (myParticleMinSpawnSquare.x < myParticleMaxSpawnSquare.x && myParticleMinSpawnSquare.y < myParticleMaxSpawnSquare.y)
				{
					myParticles[i].SetMinSpawnSquare(myParticleMinSpawnSquare);
					myParticles[i].SetMaxSpawnSquare(myParticleMaxSpawnSquare);
				}
				if (myParticlesShouldMove != *myParticles[0].GetShouldMovePtr())
				{
					SetShouldMove(myParticlesShouldMove);
				}
				if (myParticlesShouldRotate != *myParticles[0].GetShouldRotatePtr())
				{
					SetShouldRotate(myParticlesShouldRotate);
				}
				if (myParticlesShouldShrink != *myParticles[0].GetShouldShrinkPtr())
				{
					SetShouldShrink(myParticlesShouldShrink);
				}
				if (myParticlesShouldFade != *myParticles[0].GetShouldFadePtr())
				{
					SetShouldFade(myParticlesShouldFade);
				}

				myParticles[i].Update(aDeltaTime);


				myParticleInstances[i] = myParticles[i].GetSpriteInstance();
				if (myTime > mySpawnRate && myShouldStopAfterSpriteSheet == false)
				{

					myTime = 0;
					myCount++;

					if (myCount >= myParticleAmount)
					{
						myCount = 0;
					}

					if (myParticles[myCount].GetAlpha() <= 0)
					{

						if (myPosPtr != 0)
						{
							myParticles[myCount].Init(*myPosPtr, myParticleStartInstance.myColor, myParticleStartInstance.mySize, myParticleStartDecayTime, { (float)myParticlesShouldMove,(float)myParticlesShouldRotate,(float)myParticlesShouldShrink,(float)myParticlesShouldFade });
						}
						else
						{
							myParticles[myCount].Init(myPos, myParticleStartInstance.myColor, myParticleStartInstance.mySize, myParticleStartDecayTime, { (float)myParticlesShouldMove,(float)myParticlesShouldRotate,(float)myParticlesShouldShrink,(float)myParticlesShouldFade });
						}
						myParticles[myCount].SetRotation(myParticleStartInstance.myRotation);
					}
				}
			}


			if (myLifeTime <= myTotalTime && myShouldDie == true)
			{
				for (int i = 0; i < myParticles.size(); i++)
				{
					myParticles[i].SetAlpha(0);
				}
			}
		}
	}
}

float ParticleSystem::GetLifeTime()
{
	return myLifeTime;
}

float ParticleSystem::GetTotalTime()
{
	return myTotalTime;
}

void ParticleSystem::SetShouldDie(bool aBool)
{
	myShouldDie = aBool;
}

void ParticleSystem::SetForce(DreamEngine::Vector2f aForce)
{
	myForce.x = aForce.x;
	myForce.y = aForce.y;
}

void ParticleSystem::ResetParticles()
{
	myTotalTime = 0;
	for (int i = 0; i < myParticles.size(); i++)
	{
		if (myPosPtr != 0)
		{
			myParticleStartInstance.myColor.myA = 1;
			myParticles[i].Init(*myPosPtr, myParticleStartInstance.myColor, myParticleStartInstance.mySize, myParticleStartDecayTime, { (float)myParticlesShouldMove,(float)myParticlesShouldRotate,(float)myParticlesShouldShrink,(float)myParticlesShouldFade });
		}
		else
		{
			myParticleStartInstance.myColor.myA = 1;
			myParticles[i].Init(myPos, myParticleStartInstance.myColor, myParticleStartInstance.mySize, myParticleStartDecayTime, { (float)myParticlesShouldMove,(float)myParticlesShouldRotate,(float)myParticlesShouldShrink,(float)myParticlesShouldFade });
		}
	}
}

void ParticleSystem::Init(int aSize)
{
	
	myParticleAmount = aSize;
	myTotalTime = 0;
	for (size_t i = 0; i < myParticleAmount; i++)
	{
		Particle particle;
		if (myPosPtr != 0)
		{
			particle.Init(*myPosPtr, myParticleStartInstance.myColor, myParticleStartInstance.mySize, myParticleStartDecayTime, { (float)myParticlesShouldMove,(float)myParticlesShouldRotate,(float)myParticlesShouldShrink,(float)myParticlesShouldFade });
			particle.SetOrigin(*myPosPtr);
		}
		else
		{
			particle.Init(myPos, myParticleStartInstance.myColor, myParticleStartInstance.mySize, myParticleStartDecayTime, { (float)myParticlesShouldMove,(float)myParticlesShouldRotate,(float)myParticlesShouldShrink,(float)myParticlesShouldFade });
			particle.SetOrigin(myPos);
		}
		particle.SetAlpha(0);
		particle.SetForce(myForce);
		myParticles.push_back(particle);
		myParticleInstances.push_back(particle.GetSpriteInstance());
	}
}

void ParticleSystem::Render(DreamEngine::SpriteDrawer& aSpriteDrawer)
{
	if (mySpriteSheet == false)
	{
		int temp = (int)myParticles.size();
		DreamEngine::SpriteBatchScope scope = aSpriteDrawer.BeginBatch(ourParticleData);
		for (int i = temp; i >= 1; i--)
		{
			scope.Draw(*myParticles[i-1].GetSpriteInstance());
		}
	}
	else
	{
		for (int i = 0; i < myParticles.size(); i++)
		{
			aSpriteDrawer.Draw(ourParticleData, *myParticles[i].GetSpriteInstance());
		}
	}
}

void ParticleSystem::SetSpriteModel(const DreamEngine::SpriteSharedData aSpriteModel)
{
	ourParticleData.myTexture = aSpriteModel.myTexture;
}

DreamEngine::SpriteSharedData ParticleSystem::GetParticleData()
{
	return ourParticleData;
}

DreamEngine::SpriteSharedData* ParticleSystem::GetParticleDataPointer()
{
	return &ourParticleData;
}

void ParticleSystem::Clear()
{
	for (size_t i = 0; i < myParticles.size(); i++)
	{
		myParticles[i].SetAlpha(0);
	}
	myParticles.clear();
}

void ParticleSystem::InitSpriteSheet()
{
	mySpriteSheetHeight = 8;
	mySpriteSheetWidth = 8;
	
	myAddingUVX = 1.0f / mySpriteSheetWidth; // amount of sprites per row
	myAddingUVY = 1.0f / mySpriteSheetHeight; // amount of sprites per col
	myParticles[0].SetAlpha(1);
	//myParticles[0].SetUVIndex(0);
	if (myLoadedData == false)
	{
		myParticlesShouldMove = false;
		myParticlesShouldRotate = false;
		myParticlesShouldShrink = false;
		myParticlesShouldFade = false;
	}

	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			myUvs.push_back(ParticleUV::UV({ myAddingUVX * i, myAddingUVY * j }, { (myAddingUVX * i) + myAddingUVX, (myAddingUVY * j) + myAddingUVY }));
		}
	}
}

std::string ParticleSystem::GetSpritePaths()
{
	return mySpritePaths;
}

void ParticleSystem::LoadParticleSystem(std::string aParticleSystemPath)
{
	myLoadedData = true;
	std::ifstream file(aParticleSystemPath);
	nlohmann::json jsonData = nlohmann::json::parse(file);
	file.close();

	if (!jsonData["ParticleAmount"].is_null())
	{
		myParticleAmount = jsonData["ParticleAmount"];
	}
	if (!jsonData["DecayTime"].is_null())
	{
		myParticleStartDecayTime = jsonData["DecayTime"];
	}
	if (!jsonData["LifeTime"].is_null())
	{
		myLifeTime = jsonData["LifeTime"];
	}
	if (!jsonData["MaxSpawnSquareX"].is_null())
	{
		myParticleMaxSpawnSquare = { jsonData["MaxSpawnSquareX"],jsonData["MaxSpawnSquareY"] };
	}
	if (!jsonData["MinSpawnSquareX"].is_null())
	{
		myParticleMinSpawnSquare = { jsonData["MinSpawnSquareX"],jsonData["MinSpawnSquareY"] };
	}
	if (!jsonData["Radius"].is_null())
	{
		mySpawnRadius = jsonData["Radius"];
	}
	if (!jsonData["RespawnPosX"].is_null())
	{
		myParticleStartInstance.myPosition = { jsonData["RespawnPosX"], jsonData["RespawnPosY"] };
	}
	if (!jsonData["ShouldDie"].is_null())
	{
		myShouldDie = jsonData["ShouldDie"];
	}
	if (!jsonData["ShouldMove"].is_null())
	{
		myParticlesShouldMove = jsonData["ShouldMove"];
	}
	if (!jsonData["ShouldRotate"].is_null())
	{
		myParticlesShouldRotate = jsonData["ShouldRotate"];
	}
	if (!jsonData["ShouldShrink"].is_null())
	{
		myParticlesShouldShrink = jsonData["ShouldShrink"];
	}
	if (!jsonData["ShouldFade"].is_null())
	{
		myParticlesShouldFade = jsonData["ShouldFade"];
	}
	if (!jsonData["SizeX"].is_null())
	{
		myParticleStartInstance.mySize = {jsonData["SizeX"], jsonData["SizeY"]};
	}
	if (!jsonData["SpawnInCircle"].is_null())
	{
		myParticleSpawnInCircle = jsonData["SpawnInCircle"];
	}
	if (!jsonData["SpawnInSquare"].is_null())
	{
		myParticleSpawnInSquare = jsonData["SpawnInSquare"];
	}
	if (!jsonData["SpawnRate"].is_null())
	{
		mySpawnRate = jsonData["SpawnRate"];
	}
	if (!jsonData["SpritePath"].is_null())
	{
		ReplaceSpritePath(jsonData["SpritePath"]);
	}
	if (!jsonData["SpriteSheet"].is_null())
	{
		mySpriteSheet = jsonData["SpriteSheet"];
	}
	if (!jsonData["SpriteSheetPictureAmount"].is_null())
	{
		mySpriteSheetPictureAmount = jsonData["SpriteSheetPictureAmount"];
	}
	if (!jsonData["colorR"].is_null())
	{
		myParticleStartInstance.myColor = {jsonData["colorR"],jsonData["colorG"] ,jsonData["colorB"] ,jsonData["colorA"] };
	}
	if (!jsonData["ForceX"].is_null())
	{
		myForce = { jsonData["ForceX"],jsonData["ForceY"], jsonData["ForceZ"]};
	}
	if (!jsonData["SpawnRotation"].is_null())
	{
		myParticleStartInstance.myRotation = jsonData["SpawnRotation"];
	}

#ifndef _RETAIL
	if (!jsonData["SizeX"].is_null())
	{
		myDebugParticleData.mySize = { jsonData["SizeX"], jsonData["SizeY"] };
	}
	if (!jsonData["RespawnPosX"].is_null())
	{
		myDebugParticleData.myPosition = { jsonData["RespawnPosX"], jsonData["RespawnPosY"] };
	}
	if (!jsonData["colorR"].is_null())
	{
		myDebugParticleData.myColor = { jsonData["colorR"],jsonData["colorG"] ,jsonData["colorB"] ,jsonData["colorA"] };
	}
#endif // !_RETAIL


	for (int i = 0; i < myParticles.size(); i++)
	{
		myParticles[i].SetAlpha(0);
		myParticles[i].SetForce(myForce);
		myParticleInstances[i] = myParticles[i].GetSpriteInstance();
		
	}

	myUvs.clear();

	Init(myParticleAmount);
}

void ParticleSystem::ReplaceSpritePath(std::string aPath)
	{
		auto& engine = *DreamEngine::Engine::GetInstance();
		DreamEngine::SpriteSharedData newSprite;

		std::wstring mySpritePathWString = std::wstring(aPath.begin(), aPath.end());
		ourParticleData.myTexture = engine.GetTextureManager().GetTexture(DreamEngine::Settings::ResolveAssetPathW(mySpritePathWString).c_str());

		mySpritePaths = aPath;

	}

void ParticleSystem::SetParticleMinSpawnSquare(DreamEngine::Vector2f aMinValue)
{
	myParticleMinSpawnSquare = aMinValue;
}

void ParticleSystem::SetParticleMaxSpawnSquare(DreamEngine::Vector2f aMaxValue)
{
	myParticleMaxSpawnSquare = aMaxValue;
}

void ParticleSystem::SetStartRotation(float aRotation)
{
	myParticleStartInstance.myRotation = aRotation;
}

int* ParticleSystem::GetParticleAmountPtr()
{
	return &myParticleAmount;
}

void ParticleSystem::SetParticleStartAlpha(float aAlpha)
{
	myParticleStartInstance.myColor.myA = aAlpha;
}

	void ParticleSystem::SetShouldMove(bool aBool)
	{
		for (int i = 0; i < myParticles.size(); i++)
		{
			myParticles[i].SetShouldMove(aBool);
		}
	}

	void ParticleSystem::SetShouldRotate(bool aBool)
	{
		for (int i = 0; i < myParticles.size(); i++)
		{
			myParticles[i].SetShouldRotate(aBool);
		}
	}

	void ParticleSystem::SetShouldShrink(bool aBool)
	{
		for (int i = 0; i < myParticles.size(); i++)
		{
			myParticles[i].SetShouldShrink(aBool);
		}
	}

	void ParticleSystem::SetShouldFade(bool aBool)
	{
		for (int i = 0; i < myParticles.size(); i++)
		{
			myParticles[i].SetShouldFade(aBool);
		}
	}

	void ParticleSystem::SetStartUV(int aStartUV)
	{
		mySpriteSheetStartUV = aStartUV;
		myParticles[0].SetUVIndex(aStartUV);
	}

	void ParticleSystem::SetSpriteSheetAmount(int aSpriteSheetAmount)
	{
		mySpriteSheetPictureAmount = aSpriteSheetAmount;
	}

	void ParticleSystem::Play(bool aBool)
	{
		myPlay = aBool;
	}

	void ParticleSystem::SetShouldStopAfterSpriteSheet(bool aBool)
	{
		myShouldStopAfterSpriteSheet = aBool;
	}

#ifndef _RETAIL

	float* ParticleSystem::GetLifeTimePtr()
	{
		return &myLifeTime;
	}

	float* ParticleSystem::GetSpawnRatePtr()
	{
		return &mySpawnRate;
	}

	bool* ParticleSystem::GetSpawnInCirclePtr()
	{
		return &myParticleSpawnInCircle;
	}

	bool* ParticleSystem::GetSpawnInSquarePtr()
	{
		return &myParticleSpawnInSquare;
	}

	bool* ParticleSystem::GetShoudlDiePtr()
	{
		return &myShouldDie;
	}

	float* ParticleSystem::GetParticleStartDecayTime()
	{
		return &myParticleStartDecayTime;
	}

	DreamEngine::Vector3f* ParticleSystem::GetForcePtr()
	{
		return &myForce;
	}

	DreamEngine::Sprite2DInstanceData* ParticleSystem::GetDebugParticleData()
	{
		return &myDebugParticleData;
	}

	void ParticleSystem::SetDebugParticleData(DreamEngine::Sprite2DInstanceData& aSpriteData)
	{
		myParticleStartInstance.myColor = aSpriteData.myColor;
		myParticleStartInstance.mySize = aSpriteData.mySize;
		myParticleStartInstance.myPosition = aSpriteData.myPosition;
	}

	float* ParticleSystem::GetParticleMinSpawnSquare()
	{
		return &myParticleMinSpawnSquare.x;
	}

	float* ParticleSystem::GetParticleMaxSpawnSquare()
	{
		return &myParticleMaxSpawnSquare.x;
	}

	float* ParticleSystem::GetParticleSpawnRadius()
	{
		return &mySpawnRadius;
	}


	bool* ParticleSystem::GetParticleShouldRotatePtr()
	{
		return &myParticlesShouldRotate;
	}

	bool* ParticleSystem::GetParticleShouldMovePtr()
	{
		return &myParticlesShouldMove;
	}

	bool* ParticleSystem::GetParticleShouldShrinkPtr()
	{
		return &myParticlesShouldShrink;
	}

	bool* ParticleSystem::GetParticleShouldFadePtr()
	{
		return &myParticlesShouldFade;
	}

	bool* ParticleSystem::GetSpriteSheetBoolPtr()
	{
		return &mySpriteSheet;
	}

	int* ParticleSystem::GetSpriteSheetPictureAmountPtr()
	{
		return &mySpriteSheetPictureAmount;
	}

	float* ParticleSystem::GetParticleSpawnRotationPtr()
	{
		return &myParticleStartInstance.myRotation;
	}


#endif // !_RETAIL





