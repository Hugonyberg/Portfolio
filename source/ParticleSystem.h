#pragma once
#include <DreamEngine/math/Vector3.h>
#include "DreamEngine/graphics/sprite.h"
#include <vector>
#include <string>
#include "Particle.h"

namespace DreamEngine
{
	class SpriteDrawer;
	class TextureManager;
	class Engine;
	class ParticleSystem;
}
//class Particle;
namespace ParticleUV
{
	struct UV
	{
		UV(DreamEngine::Vector2f aStart, DreamEngine::Vector2f aEnd) { myStart = aStart; myEnd = aEnd; }
		DreamEngine::Vector2f myStart;
		DreamEngine::Vector2f myEnd;
	};
}

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	void SetDebug(bool aBool);
	void SetLifeTime(float aValue);
	void SetSpawnRate(float aValue);
	void SetPos(DreamEngine::Vector3f aPos);
	void SetPos(DreamEngine::Vector3f* aPos);
	std::vector<Particle>& GetParticles();
	void Update(float aDeltaTime);
	float GetLifeTime();
	float GetTotalTime();
	void SetShouldDie(bool aBool);
	void SetForce(DreamEngine::Vector2f aForce);
	void ResetParticles();
	void Init(int aSize);
	void Render(DreamEngine::SpriteDrawer& aSpriteDrawer);
	void SetSpriteModel(const DreamEngine::SpriteSharedData aSpriteModel);
	DreamEngine::SpriteSharedData GetParticleData();
	DreamEngine::SpriteSharedData* GetParticleDataPointer();
	void Clear();
	void InitSpriteSheet();
	std::string GetSpritePaths();
	void LoadParticleSystem(std::string aParticleSystemPath);
	void ReplaceSpritePath(std::string aPath);
	void SetParticleMinSpawnSquare(DreamEngine::Vector2f aMinValue);
	void SetParticleMaxSpawnSquare(DreamEngine::Vector2f aMaxValue);
	void SetStartRotation(float aRotation);
	int* GetParticleAmountPtr();
	void SetParticleStartAlpha(float aAlpha);
	void SetShouldMove(bool aBool);
	void SetShouldRotate(bool aBool);
	void SetShouldShrink(bool aBool);
	void SetShouldFade(bool aBool);
	void SetStartUV(int aStartUV);
	void SetSpriteSheetAmount(int aSpriteSheetAmount);
	void Play(bool aBool);
	void SetShouldStopAfterSpriteSheet(bool aBool);

#ifndef _RETAIL
	float* GetLifeTimePtr();
	float* GetSpawnRatePtr();
	bool* GetSpawnInCirclePtr();
	bool* GetSpawnInSquarePtr();
	bool* GetShoudlDiePtr();
	float* GetParticleStartDecayTime();
	DreamEngine::Vector3f* GetForcePtr();
	DreamEngine::Sprite2DInstanceData* GetDebugParticleData();
	void SetDebugParticleData(DreamEngine::Sprite2DInstanceData& aSpriteData);
	float* GetParticleMinSpawnSquare();
	float* GetParticleMaxSpawnSquare();
	float* GetParticleSpawnRadius();
	bool* GetParticleShouldRotatePtr();
	bool* GetParticleShouldMovePtr();
	bool* GetParticleShouldShrinkPtr();
	bool* GetParticleShouldFadePtr();
	bool* GetSpriteSheetBoolPtr();
	int* GetSpriteSheetPictureAmountPtr();
	float* GetParticleSpawnRotationPtr();


#endif // !_RETAIL


private:
	std::vector<DreamEngine::Sprite2DInstanceData*> myParticleInstances;
	DreamEngine::Sprite2DInstanceData myParticleStartInstance;
	DreamEngine::SpriteSharedData ourParticleData;
	std::vector<Particle> myParticles;
	DreamEngine::Vector3f myPos;
	float myTotalTime;
	DreamEngine::Vector3f* myPosPtr;
	DreamEngine::Vector3f myParticleForce;
	DreamEngine::Vector3f myForce;
	std::string mySpritePaths;
	DreamEngine::Vector2f myParticleMinSpawnSquare;
	DreamEngine::Vector2f myParticleMaxSpawnSquare;
	std::vector<ParticleUV::UV> myUvs;
	float myTime;
	float myLifeTime;
	float mySpawnRate;
	float myParticleStartDecayTime;
	float mySpawnRadius;
	float myAddingUVX;
	float myAddingUVY;
	int myCount;
	int myParticleAmount;
	int mySpriteSheetHeight;
	int mySpriteSheetWidth;
	int mySpriteSheetPictureAmount;
	int mySpriteSheetStartUV;
	bool myShouldDie;
	bool myParticleSpawnInCircle;
	bool myParticleSpawnInSquare;
	bool mySpriteSheet;
	bool myParticlesShouldMove;
	bool myParticlesShouldRotate;
	bool myParticlesShouldShrink;
	bool myLoadedData;
	bool myPlay;
	bool myShouldStopAfterSpriteSheet;
	bool myDebug;
	bool myParticlesShouldFade;

#ifndef _RETAIL
	DreamEngine::Sprite2DInstanceData myDebugParticleData;
#endif // _RETAIL

};

