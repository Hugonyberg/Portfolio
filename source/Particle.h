#pragma once
#include "DreamEngine/graphics/sprite.h"


namespace DreamEngine
{
	class SpriteDrawer;
	class TextureManager;
	class Engine;
	class ParticleSystem;
}

class Particle
{
public:
	Particle();
	~Particle();
	void Init(DreamEngine::Vector3f aPos, DreamEngine::Color aColor, DreamEngine::Vector2f aSize, float aDecayTime, DreamEngine::Vector4f aShouldDoStuff);
	void SetDecayTime(float aValue);
	void SetPos(DreamEngine::Vector3f aPos);
	void SetDir(DreamEngine::Vector3f aDir);
	void SetSize(DreamEngine::Vector2f aSize);
	void SetColor(DreamEngine::Vector4f aColor);
	void SetColor(DreamEngine::Color aColor);
	DreamEngine::Color GetColor();
	float GetAlpha();
	void SetAlpha(float aValue);
	void SetRotationSpeed(float aValue);
	void SetForce(DreamEngine::Vector3f aForce);
	void SetOrigin(DreamEngine::Vector3f aPos);
	void SetRotation(float aRotation);
	void SpawnRandomInSquare(DreamEngine::Vector2f aX, DreamEngine::Vector2f aY);

	void SpawnRandomInCircle(float aRadius);
	
	void Update(float aDeltaTime);
	void PlaySpriteSheet(float aDeltaTime, int aSpiteSheetPictureAmount, int myStartUV, bool shouldStop = false);

	unsigned short GetUVIndex();
	//void SetUVIndex(unsigned short aValue);
	DreamEngine::Vector3f GetDirection();
	DreamEngine::Sprite2DInstanceData* GetSpriteInstance();
	void SetSpawnRadius(float aRadius);
	void SetMinSpawnSquare(DreamEngine::Vector2f aMinPos);
	void SetMaxSpawnSquare(DreamEngine::Vector2f aMaxPos);
	void SetSpawnInCircle(bool aBool);
	void SetSpawnInSquare(bool aBool);
	bool* GetShouldRotatePtr();
	bool* GetShouldMovePtr();
	bool* GetShouldShrinkPtr();
	bool* GetShouldFadePtr();
	void SetShouldMove(bool aBool);
	void SetShouldRotate(bool aBool);
	void SetShouldShrink(bool aBool);
	void SetShouldFade(bool aBool);
	void SetUVIndex(int aNumber);
	float* GetDecayTimePtr();

#ifndef _RETAIL
	bool* GetSpawnInCirclePtr();
	bool* GetSpawnInSquarePtr();
	float* GetSpawnRadiusPtr();
	float* GetRotationSpeedPtr();
	DreamEngine::Vector2f* GetSpawnSquareMinPtr();
	DreamEngine::Vector2f* GetSpawnSquareMaxPtr();
	DreamEngine::Vector3f* GetRespawnPosPtr();
	DreamEngine::Vector3f* GetDirPtr();
	void SetSpriteInstance(DreamEngine::Sprite2DInstanceData& aSpriteData);

#endif // !_RETAIL

private:
	DreamEngine::Sprite2DInstanceData myParticleInstance;
	DreamEngine::Vector2f myMinSpawnSquare;
	DreamEngine::Vector2f myMaxSpawnSquare;
	DreamEngine::Vector3f myRespawnPos;
	DreamEngine::Vector3f myForce;
	DreamEngine::Vector3f myDir;
	
	float myRotationSpeed;
	float mySpawnRadius;
	float myDecayTime;
	float myTimer;
	unsigned short myIndex;
	unsigned short myStartIndex;
	bool myShouldMove;
	bool myShouldRotate;
	bool myShouldShrink;
	bool myShouldFade;
	bool mySpawnInCircle;
	bool mySpawnInSquare;
};

