#pragma once
#include "DreamEngine/graphics/sprite.h"
#include <DreamEngine/math/Transform.h>
namespace DreamEngine
{
	class SpriteDrawer;
	class TextureManager;
	class Engine;
	class ParticleSystem;
}

class Particle3D
{
public:
	Particle3D();
	~Particle3D();
	void Init(DreamEngine::Vector3f aPos, DreamEngine::Color aColor, DreamEngine::Vector3f aSizeScale, float aDecayTime, DreamEngine::Vector4f aShouldDoStuff, int aDirMin, int aDirMax, DreamEngine::Transform* aCameraTransform, bool aActionCamera, bool aShouldGrow);
	void SetDecayTime(float aValue);
	void SetPos(DreamEngine::Vector3f aPos);
	void SetDir(DreamEngine::Vector3f aDir);
	void SetSize(DreamEngine::Vector4f aSizeScale);
	void SetColor(DreamEngine::Vector4f aColor);
	void SetColor(DreamEngine::Color aColor);
	DreamEngine::Color GetColor();
	float GetAlpha();
	void SetAlpha(float aValue);
	void SetRotationSpeed(DreamEngine::Vector3f aRotationSpeed);
	void SetForce(DreamEngine::Vector3f aForce);
	void SetOrigin(DreamEngine::Vector3f aPos);
	void SetRotation(DreamEngine::Vector3f aRotation);
	void SpawnRandomInSquare(DreamEngine::Vector3f aMax, DreamEngine::Vector3f aMin);

	void SpawnRandomInCircle(float aRadius);

	void Update(float aDeltaTime);
	void PlaySpriteSheet(float aDeltaTime, int aSpiteSheetPictureAmount, int myStartUV, bool shouldStop = false, bool myShouldSwapUV = true);
	void RotateTowardsCamera(DreamEngine::Transform* aCameraTransform, DE::Vector3f aPosition);

	unsigned short GetUVIndex();
	//void SetUVIndex(unsigned short aValue);
	DreamEngine::Vector3f GetDirection();
	DreamEngine::Sprite3DInstanceData* GetSpriteInstance();
	void SetSpawnRadius(float aRadius);
	void SetMinSpawnSquare(DreamEngine::Vector3f aMinPos);
	void SetMaxSpawnSquare(DreamEngine::Vector3f aMaxPos);
	void SetSpawnInCircle(bool aBool);
	void SetSpawnInSquare(bool aBool);
	bool* GetShouldRotatePtr();
	bool* GetShouldMovePtr();
	bool* GetShouldShrinkPtr();
	bool* GetShouldGrowPtr();
	bool* GetShouldFadePtr();
	void SetShouldMove(bool aBool);
	void SetShouldRotate(bool aBool);
	void SetShouldShrink(bool aBool);
	void SetShouldGrow(bool aBool);
	void SetShouldFade(bool aBool);
	void SetUVIndex(int aNumber);
	void SetTimeUntillNextSpriteSheet(float aTime);
	float* GetDecayTimePtr();
	void SetParticleSize(DreamEngine::Vector2ui aSize);
	DreamEngine::Vector2f GetParticleSize();
	DreamEngine::Vector3f GetPivotPoint();
	void SetWorldScale(float aScale);

#ifndef _RETAIL
	bool* GetSpawnInCirclePtr();
	bool* GetSpawnInSquarePtr();
	float* GetSpawnRadiusPtr();
	DreamEngine::Vector3f* GetRotationSpeedPtr();
	DreamEngine::Vector3f* GetSpawnSquareMinPtr();
	DreamEngine::Vector3f* GetSpawnSquareMaxPtr();
	DreamEngine::Vector3f* GetRespawnPosPtr();
	DreamEngine::Vector3f* GetDirPtr();
	void SetSprite3DInstance(DreamEngine::Sprite3DInstanceData& aSpriteData);

#endif // !_RETAIL

private:
	DreamEngine::Sprite3DInstanceData myParticle3DInstance;
	DreamEngine::Matrix4x4f myScale;
	DreamEngine::Matrix4x4f myParticleOffset;
	DreamEngine::Vector3f myMinSpawnSquare;
	DreamEngine::Vector3f myMaxSpawnSquare;
	DreamEngine::Vector3f myRespawnPos;
	DreamEngine::Vector3f myForce;
	DreamEngine::Vector3f myDir;
	DreamEngine::Vector3f myRotationSpeed;
	DreamEngine::Vector3f myDebugTempRot;
	DreamEngine::Vector2f myParticleSize;
	float mySpawnRadius;
	float myDecayTime;
	float myTimer;
	float myWorldScale;
	float myTimeUntillNextSpriteSheet;
	int myDebugRotInDegrees;
	unsigned short myIndex;
	unsigned short myStartIndex;
	bool mySpawnInCircle;
	bool mySpawnInSquare;
	bool myShouldMove;
	bool myShouldRotate;
	bool myShouldShrink;
	bool myShouldGrow;
	bool myShouldFade;
};

