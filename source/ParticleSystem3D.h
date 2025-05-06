#pragma once
#include <DreamEngine/math/Vector3.h>
#include "DreamEngine/graphics/sprite.h"
#include <vector>
#include <string>
#include "Particle3D.h"
#include "DreamEngine/math/Transform.h"
#include "Observer.h"

namespace DreamEngine
{
	class SpriteDrawer;
	class TextureManager;
	class Engine;
	class ParticleSystem3D;
}
//class Particle;
namespace Particle3DUV
{
	struct UV
	{
		UV(DreamEngine::Vector2f aStart, DreamEngine::Vector2f aEnd) { myStart = aStart; myEnd = aEnd; }
		DreamEngine::Vector2f myStart;
		DreamEngine::Vector2f myEnd;
	};
}

class ParticleSystem3D : public Observer
{
public:
	ParticleSystem3D();
	~ParticleSystem3D();

	void SetDebug(bool aBool);
	void SetLifeTime(float aValue);
	void SetSpawnRate(float aValue);
	void SetPos(DreamEngine::Vector3f aPos);
	void SetPos(DreamEngine::Vector3f* aPos);
	std::vector<Particle3D>& GetParticles();
	void Update(float aDeltaTime);
	float GetLifeTime();
	float GetTotalTime();
	void SetShouldDie(bool aBool);
	void SetForce(DreamEngine::Vector2f aForce);
	void ResetParticles();
	void Init(int aSize);
	void Render(DreamEngine::SpriteDrawer& aSpriteDrawer);
	void RenderToGBuffer(DreamEngine::SpriteDrawer& aSpriteDrawer);
	void SetSpriteModel(const DreamEngine::SpriteSharedData aSpriteModel);
	DreamEngine::SpriteSharedData GetParticleData();
	DreamEngine::SpriteSharedData* GetParticleDataPointer();
	void Clear();
	void InitSpriteSheet();
	std::string GetSpritePaths();
	void LoadParticleSystem(std::string aParticleSystemPath);
	void ReplaceSpritePath(std::string aPath);
	void SetParticleMinSpawnSquare(DreamEngine::Vector3f aMinValue);
	void SetParticleMaxSpawnSquare(DreamEngine::Vector3f aMaxValue);
	void SetStartRotation(float aRotation);
	int* GetParticleAmountPtr();
	void SetParticleStartAlpha(float aAlpha);
	void SetShouldMove(bool aBool, Particle3D& aParticle);
	void SetShouldRotate(bool aBool, Particle3D& aParticle);
	void SetShouldShrink(bool aBool, Particle3D& aParticle);
	void SetShouldFade(bool aBool, Particle3D& aParticle);
	void SetStartUV(int aStartUV);
	void SetEndUV(int aSpriteSheetAmount);
	void Play(bool aBool);
	void SetShouldStopAfterSpriteSheet(bool aBool);
	void SetCameraTransform(DreamEngine::Transform* aCameraTransform);
	void SetActionCamera(bool aActionCamera);
	void SetWorldScale(float aScale);
	void SetSpawnTrigger(bool aBool);
	void Receive(const Message& aMessage) override;
	void SetGameID(int anID);
	void SubscribeToObserver();
	void SetParticleSystemAsFloatingDamageNumbers(bool aIsFloatingDamageNumbers);
	void SetShouldSwapUV(bool aShoudlSwapUV);
	void SetParticleInstancePos(int anIndex, DE::Vector3f aPos);
	void RotateParticleTowardsCamera(int anIndex);
	void RotateAllParticlesTowardsCamera();

#ifndef _RETAIL
	float* GetLifeTimePtr();
	float* GetSpawnRatePtr();
	bool* GetSpawnInCirclePtr();
	bool* GetSpawnInSquarePtr();
	bool* GetShoudlDiePtr();
	float* GetParticleStartDecayTime();
	DreamEngine::Vector3f* GetForcePtr();
	DreamEngine::Sprite3DInstanceData* GetDebugParticleData();
	void SetDebugParticleData(DreamEngine::Sprite3DInstanceData& aSpriteData);
	/*float* GetParticleMinSpawnSquare();
	float* GetParticleMaxSpawnSquare();*/

	float* GetParticleMinSpawnSquareX();
	float* GetParticleMinSpawnSquareY();
	float* GetParticleMinSpawnSquareZ();

	float* GetParticleMaxSpawnSquareX();
	float* GetParticleMaxSpawnSquareY();
	float* GetParticleMaxSpawnSquareZ();
	float* GetTimeUntillNextSpriteSheetPtr();
	float* GetParticleSpawnRadius();
	bool* GetParticleShouldRotatePtr();
	bool* GetParticleShouldMovePtr();
	bool* GetParticleShouldShrinkPtr();
	bool* GetParticleShouldGrowPtr();
	bool* GetParticleShouldFadePtr();
	bool* GetSpriteSheetBoolPtr();
	int* GetSpriteSheetPictureAmountPtr();
	DreamEngine::Vector3f* GetParticleSpawnRotationPtr();
	DreamEngine::Vector3f* GetParticleScaleVector();
	int* GetParticleDirMin();
	int* GetParticleDirMax();

#endif // !_RETAIL


private:
	DreamEngine::Sprite3DInstanceData myParticleStartInstance;
	std::vector<DreamEngine::Sprite3DInstanceData*> myParticleInstances;
	DreamEngine::SpriteSharedData ourParticleData;
	std::vector<Particle3D> myParticles;
	std::vector<Particle3DUV::UV> myUvs;
	DreamEngine::Transform* myCameraTransform;
	DreamEngine::Vector3f myPos;
	float myTotalTime;
	DreamEngine::Vector3f* myPosPtr;
	DreamEngine::Vector3f myParticleForce;
	DreamEngine::Vector3f myForce;
	DreamEngine::Vector3f myParticleScale;
	DreamEngine::Vector3f myParticleMinSpawnSquare;
	DreamEngine::Vector3f myParticleMaxSpawnSquare;
	DreamEngine::Vector3f myMegaTemporaryRot;
	std::string mySpritePaths;
	float myTime;
	float myLifeTime;
	float mySpawnRate;
	float myParticleStartDecayTime;
	float myAddingUVX;
	float myAddingUVY;
	float mySpawnRadius;
	float myWorldScale;
	float myTimeUntillNextSpriteSheet;
	int myCount;
	int myParticleAmount;
	int mySpriteSheetWidth;
	int mySpriteSheetHeight;
	int mySpriteSheetEndUV;
	int mySpriteSheetStartUV;
	int myParticleDirMin;
	int myParticleDirMax;
	int myOwnerGameObjectID;
#ifndef _RETAIL
	DreamEngine::Sprite3DInstanceData myDebugParticleData;
	int myDebugParticleAmount;
#endif // _RETAIL
	bool myShouldDie;
	bool myIsFloatingNumber;
	bool mySpawnTrigger;
	bool myParticleSpawnInCircle;
	bool myParticleSpawnInSquare;
	bool mySpriteSheet;
	bool myParticlesShouldMove;
	bool myParticlesShouldRotate;
	bool myParticlesShouldShrink;
	bool myParticlesShouldGrow;
	bool myParticlesShouldFade;
	bool myDebug;
	bool myLoadedData;
	bool myPlay;
	bool myShouldStopAfterSpriteSheet;
	bool myActionCamera;
	bool myShouldSwapUV;
	bool myIsFloatingDamageNumbers;
	bool myReadyToRender;
};

