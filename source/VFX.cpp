#include "VFX.h"
#include <DreamEngine/graphics/SpriteDrawer.h> 
#include <DreamEngine/graphics/texture.h>
#include <dreamengine/math/Matrix4x4.h>
#include <imgui\imgui.h>
#include "MainSingleton.h"
#include <DreamEngine\engine.h>
#include <DreamEngine/graphics/GraphicsStateStack.h>
#include <DreamEngine/graphics/GraphicsEngine.h>
VFX::VFX()
{
}

VFX::VFX(DreamEngine::Texture* aTexture, DreamEngine::Vector2f aPosition, bool isLooping, unsigned short aFPS, unsigned short aHeight, unsigned short aWidth)
{
	mySpriteInstance.myIsHidden = true;
	my2DInstance.myIsHidden = false;
	myScale = 1.f;
	mySharedData.myTexture = aTexture;
	my2DInstance.myPosition = aPosition;
	my2DInstance.mySize = mySharedData.myTexture->CalculateTextureSize() / 15.0f;

	myHeight = aHeight;
	myWidth = aWidth;
	myIsLooping = isLooping;

	mySpriteHoldTime = 1.0f / aFPS;

	addingUVY = 1.0f / myWidth;
	addingUVX = 1.0f / myHeight;

	for (int j = 0; j < myWidth; j++)
	{
		for (int i = 0; i < myHeight; i++)
		{
			myUvs.push_back(UV({ addingUVX * i, addingUVY * j }, { (addingUVX * i) + addingUVX, (addingUVY * j) + addingUVY }));
		}
	}
	my2DInstance.myTextureRect = { myUvs[myIndex].myStart.x, myUvs[myIndex].myStart.y, myUvs[myIndex].myEnd.x, myUvs[myIndex].myEnd.y };
	my2DInstance.myPivot = { 0.5f, 0.5f };
}

VFX::VFX(DreamEngine::Texture* aTexture, DreamEngine::Vector3f aPosition, bool isLooping, unsigned short aFPS, unsigned short aWidth, unsigned short aHeight, float aSizeMod)
{
	mySpriteInstance.myIsHidden = false;
	my2DInstance.myIsHidden = true;
	myScale = 1;
	mySharedData.myTexture = aTexture;
	mySpriteInstance.myTransform = DreamEngine::Matrix4x4f::CreateScaleMatrix({ (float)mySharedData.myTexture->CalculateTextureSize().x * aSizeMod, (float)mySharedData.myTexture->CalculateTextureSize().y * aSizeMod , 1.f });
	
	auto& graphicsEngine = DreamEngine::Engine::GetInstance()->GetGraphicsEngine();
	auto& graphicsStateStack = graphicsEngine.GetGraphicsStateStack();
	my2DInstance.myPivot = { 0.5f, 0.5f };
	const auto& camTrans = graphicsStateStack.GetCamera().GetTransform();

	mySpriteInstance.myTransform = mySpriteInstance.myTransform.CreateLookAtDirectionViewMatrix(camTrans.GetPosition(),camTrans.GetMatrix().GetForward() * -1.f, camTrans.GetMatrix().GetUp())  * mySpriteInstance.myTransform;

	myHeight = aHeight;
	myWidth = aWidth;
	myIsLooping = isLooping;

	mySpriteHoldTime = 1.0f / aFPS;

	addingUVY = 1.0f / myWidth;
	addingUVX = 1.0f / myHeight;

	mySpriteInstance.myTransform.SetPosition(aPosition);

	for (int j = 0; j < myWidth; j++)
	{
		for (int i = 0; i < myHeight; i++)
		{
			myUvs.push_back(UV({ addingUVX * i, addingUVY * j }, { (addingUVX * i) + addingUVX, (addingUVY * j) + addingUVY }));
		}
		mySpriteInstance.myTextureRect = { myUvs[myIndex].myStart.x, myUvs[myIndex].myStart.y, myUvs[myIndex].myEnd.x, myUvs[myIndex].myEnd.y };
	}
}

VFX::~VFX()
{
}

void VFX::Init(DreamEngine::Vector3f aPosition)
{
	//mySpriteInstance.myTransform.SetPosition(aPosition);
	mySpriteInstance.myTransform.SetPosition({ aPosition.x, aPosition.y + 100.f, aPosition.z });

	mySpriteInstance.myTransform.SetForward({ 0.25f,0.75f,0.25f });

	mySpriteInstance.myIsHidden = false;
}

void VFX::Update(float aDeltaTime)
{
	myTimer += aDeltaTime;
	if (myTimer >= mySpriteHoldTime)
	{
		if (myIndex < myUvs.size())
		{
			mySpriteInstance.myTextureRect	= { myUvs[myIndex].myStart.x, myUvs[myIndex].myStart.y, myUvs[myIndex].myEnd.x, myUvs[myIndex].myEnd.y };
			my2DInstance.myTextureRect		= { myUvs[myIndex].myStart.x, myUvs[myIndex].myStart.y, myUvs[myIndex].myEnd.x, myUvs[myIndex].myEnd.y };
			myIndex++;


			if (myIndex >= myUvs.size())
			{
				if (myIsLooping)
				{
					myIndex = 0;
				}
				else
				{
					myIsDone = true;
				}
			}
		}
		myTimer = 0;
	}
}

void VFX::Render(DreamEngine::SpriteDrawer& aSpriteDrawer)
{
	if (!myIsDone)
	{
		aSpriteDrawer.Draw(mySharedData, my2DInstance);
		aSpriteDrawer.Draw(mySharedData, mySpriteInstance);
	}
}

void VFX::Stop()
{
	myIsDone = true;
}

bool VFX::GetIsDone() const
{
	return myIsDone;
}

void VFX::SetScale(float aMultiplier)
{
	myScale *= aMultiplier;
	mySpriteInstance.myTransform = DreamEngine::Matrix4x4f::CreateScaleMatrix({ aMultiplier , aMultiplier, 1 }) * mySpriteInstance.myTransform;
	mySpriteInstance.myTransform.SetPosition(mySpriteInstance.myTransform.GetPosition() + DreamEngine::Vector3f((-50.f * myScale), (50.f * myScale), 0));
}

void VFX::SetPosition(DreamEngine::Vector3f aPosition)
{
	mySpriteInstance.myTransform.SetPosition(aPosition);
}


