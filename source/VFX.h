#pragma once
#include <DreamEngine/graphics/sprite.h>
#include <DreamEngine/graphics/texture.h>
#include <vector>

//class Player;

namespace DreamEngine
{
	class SpriteDrawer;
	class Camera;
}

struct UV
{
	UV(DreamEngine::Vector2f aStart, DreamEngine::Vector2f aEnd) { myStart = aStart; myEnd = aEnd; }
	DreamEngine::Vector2f myStart;
	DreamEngine::Vector2f myEnd;
};

class VFX
{
public:

	VFX();
	~VFX();
	VFX(DreamEngine::Texture* aTexture, DreamEngine::Vector2f aPosition, bool isLooping, unsigned short aFPS, unsigned short aHeight, unsigned short aWidth);
	VFX(DreamEngine::Texture* aTexture, DreamEngine::Vector3f aPosition, bool isLooping, unsigned short aFPS, unsigned short aHeight, unsigned short aWidth, float aSizeMod = 1);
	void Init(DreamEngine::Vector3f aPosition);
	void Update(float aDeltaTime);
	void Render(DreamEngine::SpriteDrawer& aSpriteDrawer);
	void Stop();
	bool GetIsDone() const;
	void SetScale(float aMultiplier);
	void SetPosition(DreamEngine::Vector3f aPosition);

	DreamEngine::Sprite3DInstanceData& GetInstance() { return mySpriteInstance; }

private:
	DreamEngine::Sprite3DInstanceData mySpriteInstance;
	DreamEngine::Sprite2DInstanceData my2DInstance;
	DreamEngine::SpriteSharedData mySharedData;

	std::vector<UV> myUvs;

	float myScale;
	float addingUVX;
	float addingUVY;

	float mySpriteHoldTime;
	float myTimer = 0;

	unsigned short myIndex = 0;

	unsigned short myHeight = 10;
	unsigned short myWidth = 10;

	bool myIsLooping = false;
	bool myIsDone = false;
};
