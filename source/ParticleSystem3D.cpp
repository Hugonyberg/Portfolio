#include "ParticleSystem3D.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <DreamEngine/engine.h>
#include <DreamEngine/graphics/TextureManager.h>
#include <DreamEngine/graphics/SpriteDrawer.h>
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/windows/settings.h>
#include <DreamEngine/graphics/sprite.h>
#include <DreamEngine/math/Transform.h>
#include "MainSingleton.h"

ParticleSystem3D::ParticleSystem3D()
{
	myPos = { 0,0,0 };
	myPosPtr = nullptr;
	myLifeTime = 10.0f;
	mySpawnRate = 0.0f;
	myShouldDie = true;
	myParticleStartInstance.myColor = { 1,1,1,1 };
#ifndef _RETAIL
	myDebugParticleData.myColor = { 1,1,1,1 };
#endif // !_RETAIL
	myParticleScale = { 1,1,1 };
	myParticleStartInstance.myTransform.SetRotation(0);
	myParticleStartDecayTime = 1.0f;
	mySpriteSheet = false;
	myParticlesShouldMove = true;
	myParticlesShouldRotate = false;
	myParticlesShouldShrink = false;
	myParticlesShouldFade = true;
	mySpriteSheetEndUV = 63;
	myParticleAmount = 0;
#ifndef _RETAIL
	myDebugParticleAmount = 0;
#endif
	myLoadedData = false;
	myTime = 0;
	myCount = 0;
	myPlay = true;
	mySpriteSheetStartUV = 0;
	myAddingUVX = 0;
	myAddingUVY = 0;
	mySpriteSheetHeight = 0;
	mySpriteSheetWidth = 0;
	myParticleDirMin = 1;
	myParticleDirMax = 100;
	myShouldStopAfterSpriteSheet = false;
	mySpawnRadius = 10.f;
	mySpawnTrigger = false;
	myWorldScale = 1.f;
	myShouldSwapUV = false;
	myIsFloatingDamageNumbers = false;
	myTimeUntillNextSpriteSheet = 0.01f;
}

ParticleSystem3D::~ParticleSystem3D()
{
}

void ParticleSystem3D::SetDebug(bool aBool)
{
	myDebug = aBool;
}

void ParticleSystem3D::SetLifeTime(float aValue)
{
	myLifeTime = aValue;
}

void ParticleSystem3D::SetSpawnRate(float aValue)
{
	mySpawnRate = aValue;
}

void ParticleSystem3D::SetPos(DreamEngine::Vector3f aPos)
{
	myPos = aPos;
}

void ParticleSystem3D::SetPos(DreamEngine::Vector3f* aPos)
{
	myPosPtr = aPos;
}

std::vector<Particle3D>& ParticleSystem3D::GetParticles()
{
	return myParticles;
}

void ParticleSystem3D::Update(float aDeltaTime)
{
	if (myPlay == true)
	{
		myTotalTime += aDeltaTime;
		myTime += aDeltaTime;

		if (myParticles.size() > 0)
		{
#ifndef _RETAIL
			if (myDebugParticleAmount > myParticleAmount)
			{
				Particle3D particle;
				if (myPosPtr != NULL)
				{
					particle.Init(*myPosPtr, myParticleStartInstance.myColor, myParticleScale * myWorldScale, myParticleStartDecayTime, {(float)myParticlesShouldMove,(float)myParticlesShouldRotate,(float)myParticlesShouldShrink,(float)myParticlesShouldFade}, static_cast<int>(myParticleDirMin * myWorldScale), static_cast<int>(myParticleDirMax * myWorldScale), myCameraTransform, myActionCamera, myParticlesShouldGrow);
				}
				else
				{
					particle.Init(myPos, myParticleStartInstance.myColor, myParticleScale * myWorldScale, myParticleStartDecayTime, { (float)myParticlesShouldMove,(float)myParticlesShouldRotate,(float)myParticlesShouldShrink,(float)myParticlesShouldFade }, static_cast<int>(myParticleDirMin * myWorldScale), static_cast<int>(myParticleDirMax * myWorldScale), myCameraTransform, myActionCamera, myParticlesShouldGrow);
				}
				particle.SetForce(myForce * myWorldScale);
				particle.SetAlpha(0);
				myParticles.push_back(particle);
				myParticleInstances.push_back(particle.GetSpriteInstance());
			}

			while (myDebugParticleAmount < myParticleAmount)
			{
				myParticles[myParticles.size() - 1].SetAlpha(0);
				myParticles.pop_back();
				myParticleInstances.pop_back();
				myParticleAmount--;
			}
#endif

#ifndef _RETAIL

			
			for (size_t i = 0; i < myParticles.size(); i++)
			{
				myParticles[i].SetForce(myForce * myWorldScale);
				myParticles[i].SetSpawnInCircle(myParticleSpawnInCircle);
				myParticles[i].SetSpawnInSquare(myParticleSpawnInSquare);
				myParticles[i].SetTimeUntillNextSpriteSheet(myTimeUntillNextSpriteSheet);
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
			

#endif // !_RETAIL


			for (int i = 0; i < myParticles.size(); i++)
			{
				if (mySpriteSheet == true)
				{
					if (myUvs.size() == 0)
					{
						InitSpriteSheet();
					}
					if (myParticles[i].GetAlpha() > 0)
					{
						myParticles[i].PlaySpriteSheet(aDeltaTime, mySpriteSheetEndUV, mySpriteSheetStartUV, myShouldStopAfterSpriteSheet, myShouldSwapUV);
					}
					else
					{
						myParticles[i].SetUVIndex(0);
					}
					if (myShouldSwapUV == true)
					{
						if (myParticles.size() > i)
						{
							myParticleInstances[i]->myTextureRect = { myUvs[myParticles[i].GetUVIndex()].myStart.x, myUvs[myParticles[i].GetUVIndex()].myStart.y, myUvs[myParticles[i].GetUVIndex()].myEnd.x, myUvs[myParticles[i].GetUVIndex()].myEnd.y };
						}
					}
				}

				if (mySpawnRadius > 0)
				{
					myParticles[i].SetSpawnRadius(mySpawnRadius * myWorldScale);
				}
				if (myParticleMinSpawnSquare.x < myParticleMaxSpawnSquare.x && myParticleMinSpawnSquare.y < myParticleMaxSpawnSquare.y)
				{
					myParticles[i].SetMinSpawnSquare(myParticleMinSpawnSquare * myWorldScale);
					myParticles[i].SetMaxSpawnSquare(myParticleMaxSpawnSquare * myWorldScale);
				}
				if (myParticlesShouldMove != *myParticles[0].GetShouldMovePtr())
				{
					SetShouldMove(myParticlesShouldMove, myParticles[i]);
				}
				if (myParticlesShouldRotate != *myParticles[0].GetShouldRotatePtr())
				{
					SetShouldRotate(myParticlesShouldRotate, myParticles[i]);
				}
				if (myParticlesShouldShrink != *myParticles[0].GetShouldShrinkPtr())
				{
					SetShouldShrink(myParticlesShouldShrink, myParticles[i]);
				}
				if (myParticlesShouldGrow != *myParticles[0].GetShouldShrinkPtr())
				{
					SetShouldShrink(myParticlesShouldShrink, myParticles[i]);
				}
				if (myParticlesShouldFade != *myParticles[0].GetShouldFadePtr())
				{
					SetShouldFade(myParticlesShouldFade, myParticles[i]);
				}

				myParticles[i].Update(aDeltaTime);

				myParticleInstances[i] = myParticles[i].GetSpriteInstance();
				if (mySpawnRate < 0.001f)
				{
					mySpawnRate = 0.001f;
				}
				while (myTime > mySpawnRate || mySpawnTrigger == true)
				{
					if (myTime > mySpawnRate && myShouldStopAfterSpriteSheet == false || mySpawnTrigger == true)
					{
						myTime -= mySpawnRate;

						if (myCount >= myParticleAmount)
						{
							myCount = 0;
						}

						if (myParticles[myCount].GetAlpha() <= 0)
						{

							if (myPosPtr != 0)
							{
								myParticles[myCount].Init(*myPosPtr, myParticleStartInstance.myColor, myParticleScale * myWorldScale, myParticleStartDecayTime, { (float)myParticlesShouldMove,(float)myParticlesShouldRotate,(float)myParticlesShouldShrink,(float)myParticlesShouldFade }, static_cast<int>(myParticleDirMin * myWorldScale), static_cast<int>(myParticleDirMax * myWorldScale), myCameraTransform, myActionCamera, myParticlesShouldGrow);
							}
							else
							{
								myParticles[myCount].Init(myPos, myParticleStartInstance.myColor, myParticleScale * myWorldScale, myParticleStartDecayTime, { (float)myParticlesShouldMove,(float)myParticlesShouldRotate,(float)myParticlesShouldShrink,(float)myParticlesShouldFade }, static_cast<int>(myParticleDirMin * myWorldScale), static_cast<int>(myParticleDirMax * myWorldScale), myCameraTransform, myActionCamera, myParticlesShouldGrow);
							}/*
							DreamEngine::Vector3f pos;
							DreamEngine::Vector3f rot;
							DreamEngine::Vector3f scale;*/
#ifndef _RETAIL
							//myDebugParticleData.myTransform.DecomposeMatrix(pos, rot, scale);
#endif // !_RETAIL
						//myParticles[myCount].SetRotation(rot);
						}

						myCount++;
					}
					else
					{
						mySpawnTrigger = false;
						//myPlay = false;
						break;
					}
					mySpawnTrigger = false;
				}
			}


			if (myLifeTime <= myTotalTime && myShouldDie == true)
			{
				for (int i = 0; i < myParticles.size(); i++)
				{
					myParticles[i].SetAlpha(0);
				}
				myPlay = false;
			}
		}
	}
}

float ParticleSystem3D::GetLifeTime()
{
	return myLifeTime;
}

float ParticleSystem3D::GetTotalTime()
{
	return myTotalTime;
}

void ParticleSystem3D::SetShouldDie(bool aBool)
{
	myShouldDie = aBool;
}

void ParticleSystem3D::SetForce(DreamEngine::Vector2f aForce)
{
	myForce.x = aForce.x;
	myForce.y = aForce.y;
}

void ParticleSystem3D::ResetParticles()
{
	myTotalTime = 0;
	for (int i = 0; i < myParticles.size(); i++)
	{
		if (myPosPtr != 0)
		{
			myParticleStartInstance.myColor.myA = 1;
			myParticles[i].Init(*myPosPtr, myParticleStartInstance.myColor, myParticleScale, myParticleStartDecayTime, { (float)myParticlesShouldMove,(float)myParticlesShouldRotate,(float)myParticlesShouldShrink,(float)myParticlesShouldFade }, myParticleDirMin, myParticleDirMax, myCameraTransform, myActionCamera, myParticlesShouldGrow);
		}
		else
		{
			myParticleStartInstance.myColor.myA = 1;
			myParticles[i].Init(myPos, myParticleStartInstance.myColor, myParticleScale, myParticleStartDecayTime, { (float)myParticlesShouldMove,(float)myParticlesShouldRotate,(float)myParticlesShouldShrink,(float)myParticlesShouldFade }, myParticleDirMin, myParticleDirMax, myCameraTransform, myActionCamera, myParticlesShouldGrow);
		}
	}
}

void ParticleSystem3D::Init(int aSize)
{
	myParticleAmount = aSize;

#ifndef _RETAIL
	myDebugParticleAmount = myParticleAmount;
#endif
	myTotalTime = 0;
	for (size_t i = 0; i < myParticleAmount; i++)
	{
		Particle3D particle;
		if (myPosPtr != 0)
		{
			particle.Init(*myPosPtr, myParticleStartInstance.myColor, myParticleScale, myParticleStartDecayTime, { (float)myParticlesShouldMove,(float)myParticlesShouldRotate,(float)myParticlesShouldShrink,(float)myParticlesShouldFade }, myParticleDirMin, myParticleDirMax, myCameraTransform, myActionCamera, myParticlesShouldGrow);
			//particle.SetOrigin(*myPosPtr);
		}
		else
		{
			particle.Init(myPos, myParticleStartInstance.myColor, myParticleScale, myParticleStartDecayTime, { (float)myParticlesShouldMove,(float)myParticlesShouldRotate,(float)myParticlesShouldShrink,(float)myParticlesShouldFade }, myParticleDirMin, myParticleDirMax, myCameraTransform, myActionCamera, myParticlesShouldGrow);
			//particle.SetOrigin(myPos);
		}
		particle.SetParticleSize(ourParticleData.myTexture->CalculateTextureSize());
		particle.SetAlpha(0);
		particle.SetForce(myForce);
		myParticles.push_back(particle);
		myParticleInstances.push_back(particle.GetSpriteInstance());
	}
}

void ParticleSystem3D::Render(DreamEngine::SpriteDrawer& aSpriteDrawer)
{
	if (myPlay == true)
	{
		int temp = (int)myParticles.size();
		DreamEngine::SpriteBatchScope scope = aSpriteDrawer.BeginBatch(ourParticleData);
		for (int i = temp; i >= 1; i--)
		{
			scope.Draw(*myParticles[i - 1].GetSpriteInstance());
		}
	}
}

void ParticleSystem3D::RenderToGBuffer(DreamEngine::SpriteDrawer& aSpriteDrawer)
{
	if (myPlay == true)
	{
		int temp = (int)myParticles.size();
		DreamEngine::SpriteBatchScope scope = aSpriteDrawer.BeginBatchToGBuffer(ourParticleData);
		for (int i = temp; i >= 1; i--)
		{
			scope.Draw(*myParticles[i - 1].GetSpriteInstance());
		}
	}
}

void ParticleSystem3D::SetSpriteModel(const DreamEngine::SpriteSharedData aSpriteModel)
{
	ourParticleData.myTexture = aSpriteModel.myTexture;
}

DreamEngine::SpriteSharedData ParticleSystem3D::GetParticleData()
{
	return ourParticleData;
}

DreamEngine::SpriteSharedData* ParticleSystem3D::GetParticleDataPointer()
{
	return &ourParticleData;
}

void ParticleSystem3D::Clear()
{
	for (size_t i = 0; i < myParticles.size(); i++)
	{
		myParticles[i].SetAlpha(0);
	}
	myParticles.clear();
}

void ParticleSystem3D::InitSpriteSheet()
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
			myUvs.push_back(Particle3DUV::UV({ myAddingUVX * i, myAddingUVY * j }, { (myAddingUVX * i) + myAddingUVX, (myAddingUVY * j) + myAddingUVY }));
		}
	}
}

std::string ParticleSystem3D::GetSpritePaths()
{
	return mySpritePaths;
}

void ParticleSystem3D::LoadParticleSystem(std::string aParticleSystemPath)
{
	myLoadedData = true;
	std::ifstream file(aParticleSystemPath);
	if (std::filesystem::exists(aParticleSystemPath))
	{
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
			myParticleMaxSpawnSquare = { jsonData["MaxSpawnSquareX"],jsonData["MaxSpawnSquareY"], 0 };
			if (!jsonData["MaxSpawnSquareZ"].is_null())
			{
				myParticleMaxSpawnSquare.z = jsonData["MaxSpawnSquareZ"];
			}
		}
		if (!jsonData["MinSpawnSquareX"].is_null())
		{
			myParticleMinSpawnSquare = { jsonData["MinSpawnSquareX"],jsonData["MinSpawnSquareY"],0 };
			if (!jsonData["MinSpawnSquareZ"].is_null())
			{
				myParticleMinSpawnSquare.z = jsonData["MinSpawnSquareZ"];
			}
		}
		if (!jsonData["Radius"].is_null())
		{
			mySpawnRadius = jsonData["Radius"];
		}
		if (!jsonData["RespawnPosX"].is_null())
		{
			DreamEngine::Vector3f tempPos = { jsonData["RespawnPosX"], jsonData["RespawnPosY"], 0 };
			if (!jsonData["RespawnPosZ"].is_null())
			{
				tempPos.z = jsonData["RespawnPosZ"];
			}
			myParticleStartInstance.myTransform.SetPosition(tempPos);
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
			DreamEngine::Vector3f tempScale = { jsonData["SizeX"], jsonData["SizeY"], 0 };
			if (!jsonData["SizeZ"].is_null())
			{
				tempScale.z = jsonData["SizeZ"];
			}
			DreamEngine::Vector3f pos;
			DreamEngine::Vector3f rot;
			DreamEngine::Vector3f scale;
			myParticleStartInstance.myTransform.DecomposeMatrix(pos, rot, scale);
			myParticleStartInstance.myTransform = DreamEngine::Matrix4x4f::CreateScaleMatrix(tempScale);
			myParticleStartInstance.myTransform.SetPosition(pos);
			myParticleStartInstance.myTransform.SetRotation(rot);
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
			mySpriteSheetEndUV = jsonData["SpriteSheetPictureAmount"];
		}
		if (!jsonData["colorR"].is_null())
		{
			myParticleStartInstance.myColor = { jsonData["colorR"],jsonData["colorG"] ,jsonData["colorB"] ,jsonData["colorA"] };
		}
		if (!jsonData["ForceX"].is_null())
		{
			myForce = { jsonData["ForceX"],jsonData["ForceY"], 0 };
			if (!jsonData["ForceZ"].is_null())
			{
				myForce.z = jsonData["ForceZ"];
			}
		}
		if (!jsonData["SpawnRotation"].is_null())
		{
			float tempRot = jsonData["SpawnRotation"];
			myParticleStartInstance.myTransform.SetRotation(tempRot);
		}

#ifndef _RETAIL
		if (!jsonData["SizeX"].is_null())
		{
			DreamEngine::Vector2f size = { jsonData["SizeX"], jsonData["SizeY"] };

		}
		if (!jsonData["RespawnPosX"].is_null())
		{
			DreamEngine::Vector3f tempPos = { jsonData["RespawnPosX"], jsonData["RespawnPosY"], 0 };
			if (!jsonData["RespawnPosZ"].is_null())
			{
				tempPos.z = jsonData["RespawnPosZ"];
			}
			myParticleStartInstance.myTransform.SetPosition(tempPos);
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
}

void ParticleSystem3D::ReplaceSpritePath(std::string aPath)
{
	auto& engine = *DreamEngine::Engine::GetInstance();
	DreamEngine::SpriteSharedData newSprite;

	std::wstring mySpritePathWString = std::wstring(aPath.begin(), aPath.end());
	ourParticleData.myTexture = engine.GetTextureManager().GetTexture(DreamEngine::Settings::ResolveAssetPathW(mySpritePathWString).c_str());

	mySpritePaths = aPath;

}

void ParticleSystem3D::SetParticleMinSpawnSquare(DreamEngine::Vector3f aMinValue)
{
	myParticleMinSpawnSquare = aMinValue;
}

void ParticleSystem3D::SetParticleMaxSpawnSquare(DreamEngine::Vector3f aMaxValue)
{
	myParticleMaxSpawnSquare = aMaxValue;
}

void ParticleSystem3D::SetStartRotation(float aRotation)
{
	myParticleStartInstance.myTransform.SetRotation(aRotation);
}

#ifndef _RETAIL
int* ParticleSystem3D::GetParticleAmountPtr()
{
	return &myDebugParticleAmount;
}
#endif

void ParticleSystem3D::SetParticleStartAlpha(float aAlpha)
{
	myParticleStartInstance.myColor.myA = aAlpha;
}

void ParticleSystem3D::SetShouldMove(bool aBool, Particle3D& aParticle)
{
	
	aParticle.SetShouldMove(aBool);
	
}

void ParticleSystem3D::SetShouldRotate(bool aBool, Particle3D& aParticle)
{
	aParticle.SetShouldRotate(aBool);
}

void ParticleSystem3D::SetShouldShrink(bool aBool, Particle3D& aParticle)
{
	aParticle.SetShouldShrink(aBool);
}

void ParticleSystem3D::SetShouldFade(bool aBool, Particle3D& aParticle)
{
	aParticle.SetShouldFade(aBool);
}

void ParticleSystem3D::SetStartUV(int aStartUV)
{
	mySpriteSheetStartUV = aStartUV;
	myParticles[0].SetUVIndex(aStartUV);
}

void ParticleSystem3D::SetEndUV(int aSpriteSheetAmount)
{
	mySpriteSheetEndUV = aSpriteSheetAmount;
}

void ParticleSystem3D::Play(bool aBool)
{
	myPlay = aBool;
	if (aBool == true)
	{
		myParticleStartInstance.myColor.myA = 1;
	}
}

void ParticleSystem3D::SetShouldStopAfterSpriteSheet(bool aBool)
{
	myShouldStopAfterSpriteSheet = aBool;
}

void ParticleSystem3D::SetCameraTransform(DreamEngine::Transform* aCameraTransform)
{
	myCameraTransform = aCameraTransform;
}

void ParticleSystem3D::SetActionCamera(bool aActionCamera)
{
	myActionCamera = aActionCamera;
}

void ParticleSystem3D::SetWorldScale(float aScale)
{
	myWorldScale = aScale;
	for (int i = 0; i < myParticles.size(); i++)
	{
		myParticles[i].SetWorldScale(myWorldScale);
	}
}

void ParticleSystem3D::SetSpawnTrigger(bool aBool)
{
	mySpawnTrigger = aBool;
}

void ParticleSystem3D::Receive(const Message& aMessage)
{
	if (aMessage.messageType == eMessageType::SpawnParticle)
	{
		std::array<int, 2>* tempArray = static_cast<std::array<int,2>*>(aMessage.messageData);
		int tempID = *tempArray->begin();
		if (tempID == myOwnerGameObjectID)
		{
			mySpawnTrigger = true;
		
			int tempDamage = (*tempArray)[1];
			switch (tempDamage)
			{
			case 5:
			{
				myParticles[myCount].SetUVIndex(0);
				break;
			}
			case 10:
			{
				myParticles[myCount].SetUVIndex(1);
				break;
			}
			case 15:
			{
				myParticles[myCount].SetUVIndex(2);
				break;
			}
			case 20:
			{
				myParticles[myCount].SetUVIndex(3);
				break;
			}
			case 25:
			{
				myParticles[myCount].SetUVIndex(4);
				break;
			}
			case 26:
			{
				myParticles[myCount].SetUVIndex(5);
				break;
			}
			case 27:
			{
				myParticles[myCount].SetUVIndex(6);
				break;
			}
			case 28:
			{
				myParticles[myCount].SetUVIndex(7);
				break;
			}
			case 29:
			{
				myParticles[myCount].SetUVIndex(8);
				break;
			}
			case 30:
			{
				myParticles[myCount].SetUVIndex(9);
				break;
			}
			case 31:
			{
				myParticles[myCount].SetUVIndex(10);
				break;
			}
			case 32:
			{
				myParticles[myCount].SetUVIndex(11);
				break;
			}
			case 33:
			{
				myParticles[myCount].SetUVIndex(12);
				break;
			}
			case 34:
			{
				myParticles[myCount].SetUVIndex(13);
				break;
			}
			case 35:
			{
				myParticles[myCount].SetUVIndex(14);
				break;
			}
			case 36:
			{
				myParticles[myCount].SetUVIndex(15);
				break;
			}
			case 37:
			{
				myParticles[myCount].SetUVIndex(16);
				break;
			}
			case 38:
			{
				myParticles[myCount].SetUVIndex(17);
				break;
			}
			case 39:
			{
				myParticles[myCount].SetUVIndex(18);
				break;
			}
			case 40:
			{
				myParticles[myCount].SetUVIndex(19);
				break;
			}
			case 41:
			{
				myParticles[myCount].SetUVIndex(20);
				break;
			}
			case 42:
			{
				myParticles[myCount].SetUVIndex(21);
				break;
			}
			case 43:
			{
				myParticles[myCount].SetUVIndex(22);
				break;
			}
			case 44:
			{
				myParticles[myCount].SetUVIndex(23);
				break;
			}
			case 45:
			{
				myParticles[myCount].SetUVIndex(24);
				break;
			}
			case 46:
			{
				myParticles[myCount].SetUVIndex(25);
				break;
			}
			case 47:
			{
				myParticles[myCount].SetUVIndex(26);
				break;
			}
			case 48:
			{
				myParticles[myCount].SetUVIndex(27);
				break;
			}
			case 49:
			{
				myParticles[myCount].SetUVIndex(28);
				break;
			}
			case 50:
			{
				myParticles[myCount].SetUVIndex(29);

				break;
			}
			default:
				break;
			}

			myParticleInstances[myCount] = myParticles[myCount].GetSpriteInstance();
			myParticleInstances[myCount]->myTextureRect = { myUvs[myParticles[myCount].GetUVIndex()].myStart.x, myUvs[myParticles[myCount].GetUVIndex()].myStart.y, myUvs[myParticles[myCount].GetUVIndex()].myEnd.x, myUvs[myParticles[myCount].GetUVIndex()].myEnd.y };
		}
	}
}

void ParticleSystem3D::SetGameID(int anID)
{
	myOwnerGameObjectID = anID;
}

void ParticleSystem3D::SubscribeToObserver()
{
	MainSingleton::GetInstance()->GetPostMaster().Subscribe(eMessageType::SpawnParticle, this);
}

void ParticleSystem3D::SetParticleSystemAsFloatingDamageNumbers(bool aIsFloatingDamageNumbers)
{
	myIsFloatingDamageNumbers = aIsFloatingDamageNumbers;
}

void ParticleSystem3D::SetShouldSwapUV(bool aShoudlSwapUV)
{
	myShouldSwapUV = aShoudlSwapUV;
}

void ParticleSystem3D::SetParticleInstancePos(int anIndex, DE::Vector3f aPos)
{
	myParticleInstances[anIndex]->myTransform.SetPosition(aPos);
}

void ParticleSystem3D::RotateParticleTowardsCamera(int anIndex)
{
	myParticles[anIndex].RotateTowardsCamera(myCameraTransform, myCameraTransform->GetPosition());
}

void ParticleSystem3D::RotateAllParticlesTowardsCamera()
{
	for (int i = 0; i < myParticles.size(); i++)
	{
		myParticles[i].RotateTowardsCamera(myCameraTransform, myCameraTransform->GetPosition());
	}
}

#ifndef _RETAIL

float* ParticleSystem3D::GetLifeTimePtr()
{
	return &myLifeTime;
}

float* ParticleSystem3D::GetSpawnRatePtr()
{
	return &mySpawnRate;
}

bool* ParticleSystem3D::GetSpawnInCirclePtr()
{
	return &myParticleSpawnInCircle;
}

bool* ParticleSystem3D::GetSpawnInSquarePtr()
{
	return &myParticleSpawnInSquare;
}

bool* ParticleSystem3D::GetShoudlDiePtr()
{
	return &myShouldDie;
}

float* ParticleSystem3D::GetParticleStartDecayTime()
{
	return &myParticleStartDecayTime;
}

DreamEngine::Vector3f* ParticleSystem3D::GetForcePtr()
{
	return &myForce;
}

DreamEngine::Sprite3DInstanceData* ParticleSystem3D::GetDebugParticleData()
{
	return &myDebugParticleData;
}

void ParticleSystem3D::SetDebugParticleData(DreamEngine::Sprite3DInstanceData& aSpriteData)
{
	myParticleStartInstance.myColor = aSpriteData.myColor;
	myParticleStartInstance.myTransform.SetPosition(aSpriteData.myTransform.GetPosition());
	DreamEngine::Vector3f pos;
	DreamEngine::Vector3f rot;
	DreamEngine::Vector3f scale;
	aSpriteData.myTransform.DecomposeMatrix(pos, rot, scale);
	myParticleScale =scale;
}

//float* ParticleSystem3D::GetParticleMinSpawnSquare()
//{
//	return &myParticleMinSpawnSquare.x;
//}
//
//float* ParticleSystem3D::GetParticleMaxSpawnSquare()
//{
//	return &myParticleMaxSpawnSquare.x;
//}

float* ParticleSystem3D::GetParticleMinSpawnSquareX()
{
	return &myParticleMinSpawnSquare.x;
}

float* ParticleSystem3D::GetParticleMinSpawnSquareY()
{
	return &myParticleMinSpawnSquare.y;
}

float* ParticleSystem3D::GetParticleMinSpawnSquareZ()
{
	return &myParticleMinSpawnSquare.z;
}

float* ParticleSystem3D::GetParticleMaxSpawnSquareX()
{
	return &myParticleMaxSpawnSquare.x;
}

float* ParticleSystem3D::GetParticleMaxSpawnSquareY()
{
	return &myParticleMaxSpawnSquare.y;
}

float* ParticleSystem3D::GetParticleMaxSpawnSquareZ()
{
	return &myParticleMaxSpawnSquare.z;
}

float* ParticleSystem3D::GetTimeUntillNextSpriteSheetPtr()
{
	return &myTimeUntillNextSpriteSheet;
}

float* ParticleSystem3D::GetParticleSpawnRadius()
{
	return &mySpawnRadius;
}

bool* ParticleSystem3D::GetParticleShouldRotatePtr()
{
	return &myParticlesShouldRotate;
}

bool* ParticleSystem3D::GetParticleShouldMovePtr()
{
	return &myParticlesShouldMove;
}

bool* ParticleSystem3D::GetParticleShouldShrinkPtr()
{
	return &myParticlesShouldShrink;
}

bool* ParticleSystem3D::GetParticleShouldGrowPtr()
{
	return &myParticlesShouldGrow;
}

bool* ParticleSystem3D::GetParticleShouldFadePtr()
{
	return &myParticlesShouldFade;
}

bool* ParticleSystem3D::GetSpriteSheetBoolPtr()
{
	return &mySpriteSheet;
}

int* ParticleSystem3D::GetSpriteSheetPictureAmountPtr()
{
	return &mySpriteSheetEndUV;
}

DreamEngine::Vector3f* ParticleSystem3D::GetParticleSpawnRotationPtr()
{
	DreamEngine::Vector3f pos;
	DreamEngine::Vector3f rot;
	DreamEngine::Vector3f scale;
	myParticleStartInstance.myTransform.DecomposeMatrix(pos,rot,scale);
	myMegaTemporaryRot = rot;
	return &myMegaTemporaryRot;
}

DreamEngine::Vector3f* ParticleSystem3D::GetParticleScaleVector()
{
	return &myParticleScale;
}

int* ParticleSystem3D::GetParticleDirMin()
{
	return &myParticleDirMin;
}

int* ParticleSystem3D::GetParticleDirMax()
{
	return &myParticleDirMax;
}


#endif // !_RETAIL





