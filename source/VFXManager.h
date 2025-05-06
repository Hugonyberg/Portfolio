#pragma once
#include <DreamEngine/graphics/sprite.h>
#include <vector>
#include "Observer.h"
#include "VFX.h"


namespace DreamEngine
{
	class SpriteDrawer;
	class Camera;
}

enum eVFX
{
		AlexandersBidrag,
		Leaf_B1,
		Leaf_B2,
		Leaf_B3,
		Pot,
		COUNT,
};

class VFXManager : public Observer
{
public:
	VFXManager();
	~VFXManager();

	void Init();
	std::shared_ptr<VFX> Create2DVFX(eVFX aVFXType, DreamEngine::Vector2f aPosition, bool isLooping = true, int aFPS = 60);
	void CreateVFX(const wchar_t* aTexturePath);

	bool CanPlayVFX() const;
	void Update(float aDeltaTime);
	const std::vector<std::shared_ptr<VFX>> GetVFXVector();
	void Render();
	void Receive(const Message& aMsg) override;


private:
	std::vector<std::shared_ptr<VFX>> myActiveSprites;

	//std::shared_ptr<Player> myPlayer;

	bool myHasActiveVFX = false;
};

