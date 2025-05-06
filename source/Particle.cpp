#include "Particle.h"
#include "ParticleSystem.h"
#include <DreamEngine/utilities/UtilityFunctions.h>
#include <DreamEngine/engine.h>
#include <DreamEngine/graphics/TextureManager.h>
#include <DreamEngine/graphics/SpriteDrawer.h>
#include <DreamEngine/windows/settings.h>


Particle::Particle()
{
	myDecayTime = 1.0f;
	myDir = { 1,1,0 };
	myParticleInstance.myPosition = { 0.0f,0.0f };
	myParticleInstance.myColor = { 1,1,1,1 };
	myParticleInstance.mySize = 1.0f;
	myRotationSpeed = 1.0f;
	myForce = {0, 0, 0};
	mySpawnRadius = 5;
	myMinSpawnSquare = { 0,0 };
	myMaxSpawnSquare = { 3,3 };
	mySpawnInCircle = false;
	mySpawnInSquare = false;
	myShouldMove = true;
	myShouldRotate = true;
	myShouldShrink = true;
	myShouldFade = true;
	myIndex = 0;
	myTimer = 0;
	myStartIndex = 0;
}

Particle::~Particle()
{

}

void Particle::SetDecayTime(float aValue)
{
	myDecayTime = aValue;
}

void Particle::SetPos(DreamEngine::Vector3f aPos)
{
	myParticleInstance.myPosition = { aPos.x, aPos.y };
}

void Particle::SetDir(DreamEngine::Vector3f aDir)
{
	myDir = aDir;
}

void Particle::SetSize(DreamEngine::Vector2f aSize)
{
	myParticleInstance.mySize = aSize;
}

void Particle::SetColor(DreamEngine::Vector4f aColor)
{
	myParticleInstance.myColor = { aColor.x, aColor.y, aColor.z, aColor.y };
}

void Particle::SetColor(DreamEngine::Color aColor)
{
	myParticleInstance.myColor = aColor;
}

DreamEngine::Color Particle::GetColor()
{
	return myParticleInstance.myColor;
}

float Particle::GetAlpha()
{
	return myParticleInstance.myColor.myA;
}

void Particle::SetAlpha(float aValue)
{
	myParticleInstance.myColor.myA = aValue;
}

void Particle::SetRotationSpeed(float aValue)
{
	myRotationSpeed = aValue;
}

void Particle::Update(float aDeltaTime)
{
	if (myParticleInstance.myColor.myA < 0)
	{
		return;
	}
	if (myShouldMove == true)
	{
		myParticleInstance.myPosition += {(myDir.x + myForce.x) * aDeltaTime, (myDir.y + myForce.y) * aDeltaTime};
	}
	if (myShouldFade == true)
	{
		myParticleInstance.myColor.myA -= aDeltaTime / myDecayTime;
	}
	if (myShouldRotate == true)
	{
		myParticleInstance.myRotation += myRotationSpeed * aDeltaTime;
	}
	if (myShouldShrink == true)
	{
		if (myParticleInstance.mySize.x >= 0)
		{
			myParticleInstance.mySize.x -=  aDeltaTime / myDecayTime;
			myParticleInstance.mySize.y -=  aDeltaTime / myDecayTime;
		}
	}
}

void Particle::PlaySpriteSheet(float aDeltaTime, int aSpiteSheetPictureAmount, int myStartUV,  bool shouldStop)
{
	myTimer += aDeltaTime;
	myDir = {0,0,0};

	if (shouldStop == true && myIndex == aSpiteSheetPictureAmount)
	{
		myParticleInstance.myColor.myA = 0;
		return;
	}
	// Cycle the sheet
	if (myTimer >= 0.05f)
	{
		myIndex++;
		if (myIndex > aSpiteSheetPictureAmount || myIndex < myStartUV)
		{
			myIndex = static_cast<unsigned short>(myStartUV);
			
			if (mySpawnInCircle == true)
			{
				SpawnRandomInCircle(mySpawnRadius);
			}
			else if (mySpawnInSquare == true)
			{
				SpawnRandomInSquare(myMinSpawnSquare, myMaxSpawnSquare);
			}
			else
			{
				myParticleInstance.myPosition = { myRespawnPos.x, myRespawnPos.y };
			}
		}
		myTimer = 0.0f;
	}
}

unsigned short Particle::GetUVIndex()
{
	return myIndex;
}

//void Particle::SetUVIndex(unsigned short aValue)
//{
//	myIndex = aValue;
//}

DreamEngine::Vector3f Particle::GetDirection()
{
	return myDir;
}

void Particle::SetForce(DreamEngine::Vector3f aForce)
{
	myForce = aForce;
}

void Particle::SetOrigin(DreamEngine::Vector3f aPos)
{
	myRespawnPos = aPos;
}

void Particle::SetRotation(float aRotation)
{
	aRotation *= 3.1415f / 180;
	myParticleInstance.myRotation = aRotation;
}

void Particle::SpawnRandomInSquare(DreamEngine::Vector2f aMin, DreamEngine::Vector2f aMax)
{
	myParticleInstance.myPosition = { myRespawnPos.x + (UtilityFunctions::GetRandomFloat(aMin.x * 100, aMax.x * 100))/100.f, myRespawnPos.y + (UtilityFunctions::GetRandomFloat(aMin.y * 100, aMax.y * 100)) / 100.f };
}

void Particle::SpawnRandomInCircle(float aRadius)
{
	myDir = { ((float)UtilityFunctions::GetRandomFloat(1, 100) / 10) - 5.0f, ((float)UtilityFunctions::GetRandomFloat(1, 100) / 10) - 5.f,0 };
	myDir.Normalize();
	int randomNum = (int)UtilityFunctions::GetRandomFloat(0, aRadius);
	myParticleInstance.myPosition.x = myRespawnPos.x + (myDir.x * randomNum);
	myParticleInstance.myPosition.y = myRespawnPos.y + (myDir.y * randomNum);
}

void Particle::Init(DreamEngine::Vector3f aPos, DreamEngine::Color aColor, DreamEngine::Vector2f aSize, float aDecayTime, DreamEngine::Vector4f aShouldDoStuff)
{
	myDecayTime = aDecayTime;
	myRespawnPos = aPos;
	myParticleInstance.myColor = aColor;
	myParticleInstance.mySize = aSize;
	myShouldMove = aShouldDoStuff.x;
	myShouldRotate = aShouldDoStuff.y;
	myShouldShrink = aShouldDoStuff.z;
	myShouldFade = aShouldDoStuff.w;
	
	if (mySpawnInCircle == true)
	{
		SpawnRandomInCircle(mySpawnRadius);
	}
	else if (mySpawnInSquare == true)
	{
		SpawnRandomInSquare(myMinSpawnSquare, myMaxSpawnSquare);
	}
	else
	{
		myParticleInstance.myPosition = { myRespawnPos.x, myRespawnPos.y };
	}
	if (myShouldRotate == true)
	{
		myRotationSpeed = ((float)UtilityFunctions::GetRandomFloat(1, 100) / 10) - 5.0f;
		myParticleInstance.myRotation = ((float)UtilityFunctions::GetRandomFloat(1, 360));
	}
	if (myShouldMove == true)
	{
		myDir = { ((float)UtilityFunctions::GetRandomFloat(1, 100) / 100) - 0.5f, ((float)UtilityFunctions::GetRandomFloat(1, 100) / 100) -0.5f,0 };
	}
	myIndex = myStartIndex;
}

DreamEngine::Sprite2DInstanceData* Particle::GetSpriteInstance()
	{
		return &myParticleInstance;
	}

	void Particle::SetSpawnRadius(float aRadius)
	{
		mySpawnRadius = aRadius;
	}

	void Particle::SetMinSpawnSquare(DreamEngine::Vector2f aMinPos)
	{
		myMinSpawnSquare = aMinPos;
	}

	void Particle::SetMaxSpawnSquare(DreamEngine::Vector2f aMaxPos)
	{
		myMaxSpawnSquare = aMaxPos;
	}

	void Particle::SetSpawnInCircle(bool aBool)
	{
		mySpawnInCircle = aBool;
	}

	void Particle::SetSpawnInSquare(bool aBool)
	{
		mySpawnInSquare = aBool;
	}

	bool* Particle::GetShouldRotatePtr()
	{
		return &myShouldRotate;
	}

	bool* Particle::GetShouldMovePtr()
	{
		return &myShouldMove;
	}

	bool* Particle::GetShouldShrinkPtr()
	{
		return &myShouldShrink;
	}

	bool* Particle::GetShouldFadePtr()
	{
		return &myShouldFade;
	}

	void Particle::SetShouldMove(bool anBool)
	{
		myShouldMove = anBool;
	}

	void Particle::SetShouldRotate(bool anBool)
	{
		if (anBool == false)
		{
			myParticleInstance.myRotation = 0;
		}
		myShouldRotate = anBool;
	}

	void Particle::SetShouldShrink(bool anBool)
	{
		myShouldShrink = anBool;
	}

	void Particle::SetShouldFade(bool anBool)
	{
		myShouldFade = anBool;
	}

	void Particle::SetUVIndex(int aNumber)
	{
		myIndex = static_cast<unsigned short>(aNumber);
		myStartIndex = myIndex;
	}

	float* Particle::GetDecayTimePtr()
	{
		return &myDecayTime;
	}
#ifndef _RETAIL


	bool* Particle::GetSpawnInCirclePtr()
	{
		return &mySpawnInCircle;
	}


	bool* Particle::GetSpawnInSquarePtr()
	{
		return &mySpawnInSquare;
	}


	float* Particle::GetSpawnRadiusPtr()
	{
		return &mySpawnRadius;
	}

	float* Particle::GetRotationSpeedPtr()
	{
		return &myRotationSpeed;
	}


	DreamEngine::Vector2f* Particle::GetSpawnSquareMinPtr()
	{
		return &myMinSpawnSquare;
	}

	DreamEngine::Vector2f* Particle::GetSpawnSquareMaxPtr()
	{
		return &myMaxSpawnSquare;
	}

	DreamEngine::Vector3f* Particle::GetRespawnPosPtr()
	{
		return &myRespawnPos;
	}

	DreamEngine::Vector3f* Particle::GetDirPtr()
	{
		return &myDir;
	}

	void Particle::SetSpriteInstance(DreamEngine::Sprite2DInstanceData& aSpriteData)
	{
		myParticleInstance.myColor = aSpriteData.myColor;
		myParticleInstance.mySize = aSpriteData.mySize;
	}




#endif // !_RETAIL