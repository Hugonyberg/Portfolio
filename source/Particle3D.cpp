#include "Particle3D.h"
#include <DreamEngine/utilities/UtilityFunctions.h>
#include <DreamEngine/math/Matrix3x3.h>
#include <DreamEngine/math/Matrix2x2.h>
#include <DreamEngine/math/Transform.h>
#include <PhysX/PxPhysicsAPI.h>
namespace RotationMathBS
{
	DreamEngine::Vector4f QuaternionMultiplication(const DreamEngine::Vector4f& aVector, const DreamEngine::Vector4f& aNother)
	{
		float w = aVector.x * aNother.x - aVector.y * aNother.y - aVector.z * aNother.z - aVector.w * aNother.w;
		float x = aVector.x * aNother.y + aVector.y * aNother.x + aVector.z * aNother.w - aVector.w * aNother.z;
		float y = aVector.x * aNother.z - aVector.y * aNother.w + aVector.z * aNother.x + aVector.w * aNother.y;
		float z = aVector.x * aNother.w + aVector.y * aNother.z - aVector.z * aNother.y + aVector.w * aNother.x;

		return { w, x, y, z };
	}

	DreamEngine::Vector4f Quaternion(const DreamEngine::Vector3f& aVector, const float anAngle)
	{
		float angle = anAngle; // Assuming Angle<T> is a custom class for angles.
		float halfAngle = angle / 2.0f; // Half of the angle.
		float sinHalfAngle = sin(halfAngle); // Sine of half the angle.
		DreamEngine::Vector3f axis = aVector.GetNormalized(); // Normalize the axis vector.

		// Compute quaternion components.
		float w = cos(halfAngle);
		float x = axis.x * sinHalfAngle;
		float y = axis.y * sinHalfAngle;
		float z = axis.z * sinHalfAngle;

		// Create and normalize the quaternion.
		DreamEngine::Vector4f result = DreamEngine::Vector4f{ w, x, y, z };
		result.Normalize();
		return result;
	}

	DreamEngine::Vector3f Rotate3D(DreamEngine::Vector3f& aVector, const DreamEngine::Vector4f& aQuaternion)
	{
		DreamEngine::Vector4f vectorQuaternion = { 0, aVector.x, aVector.y, aVector.z };
		DreamEngine::Vector4f quaternionConjugate = { aQuaternion.x, -aQuaternion.y, -aQuaternion.z, -aQuaternion.w };
		DreamEngine::Vector4f rotatedVector = QuaternionMultiplication(aQuaternion, QuaternionMultiplication(vectorQuaternion, quaternionConjugate));

		return { rotatedVector.y, rotatedVector.z, rotatedVector.w };
	}

	DreamEngine::Vector3f Rotate3D(DreamEngine::Vector3f& aVector, const DreamEngine::Vector3f& aNose, const float aAngle)
	{
		DreamEngine::Vector4f quaternion = Quaternion(aNose, aAngle);
		return Rotate3D(aVector, quaternion);
	}

	DreamEngine::Vector3f TransformGetPosition(DreamEngine::Matrix4x4f aTransform)
	{
		return DreamEngine::Vector3f{ aTransform(4,1), aTransform(4,2),aTransform(4,3) };
	}

	DreamEngine::Vector3f TransformGetUp(DreamEngine::Matrix4x4f aTransform)
	{
		return DreamEngine::Vector3f{ aTransform(2,1), aTransform(2,2),aTransform(2,3) };
	}

	DreamEngine::Vector3f TransformGetRight(DreamEngine::Matrix4x4f aTransform)
	{
		return DreamEngine::Vector3f{ aTransform(3,1), aTransform(3,2),aTransform(3,3) };
	}

	DreamEngine::Vector3f TransformGetForward(DreamEngine::Matrix4x4f aTransform)
	{
		return DreamEngine::Vector3f{ aTransform(1,1), aTransform(1,2),aTransform(1,3) };
	}

	void TransformSetPosition(DreamEngine::Matrix4x4f& aTransform, DreamEngine::Vector3f aPosition)
	{
		aTransform(4, 1) = aPosition.x;
		aTransform(4, 2) = aPosition.y;
		aTransform(4, 3) = aPosition.z;
	}

	void TransformSetForward(DreamEngine::Matrix4x4f& aTransform, DreamEngine::Vector3f aPosition)
	{
		aTransform(1, 1) = aPosition.x;
		aTransform(1, 2) = aPosition.y;
		aTransform(1, 3) = aPosition.z;
	}

	void TransformSetUp(DreamEngine::Matrix4x4f& aTransform, DreamEngine::Vector3f aPosition)
	{
		aTransform(2, 1) = aPosition.x;
		aTransform(2, 2) = aPosition.y;
		aTransform(2, 3) = aPosition.z;
	}

	void TransformSetRight(DreamEngine::Matrix4x4f& aTransform, DreamEngine::Vector3f aPosition)
	{
		aTransform(3, 1) = aPosition.x;
		aTransform(3, 2) = aPosition.y;
		aTransform(3, 3) = aPosition.z;
	}

	void RotateTransformAroundPivot(DreamEngine::Matrix4x4f& aTransform, DreamEngine::Vector3f aRotationAxis, float aRotationAngle, DreamEngine::Vector3f aPivot)
	{
		// we set them each
		DreamEngine::Vector3f temp = TransformGetRight(aTransform);
		TransformSetRight(aTransform, Rotate3D(temp, aRotationAxis, aRotationAngle));
		temp = TransformGetUp(aTransform);
		TransformSetUp(aTransform, Rotate3D(temp, aRotationAxis, aRotationAngle));
		temp = TransformGetForward(aTransform);
		TransformSetForward(aTransform, Rotate3D(temp, aRotationAxis, aRotationAngle));
		DreamEngine::Vector3f vector = TransformGetPosition(aTransform) - aPivot;
		TransformSetPosition(aTransform, Rotate3D(vector, aRotationAxis, aRotationAngle) + aPivot);
	}
}


Particle3D::Particle3D()
{
	myDecayTime = 1.0f;
	myDir = { 1,1,1 };
	myParticle3DInstance.myTransform.SetPosition(DreamEngine::Vector3f{ 0.f,0.f,0.f });
	myParticleOffset.SetPosition(DreamEngine::Vector3f{ 0.5f,0.5f,0.5f });
	myParticle3DInstance.myColor = { 1,1,1,1 };
	myScale = myParticle3DInstance.myTransform.CreateScaleMatrix(1000.0f);
	myRotationSpeed = { 1.0f,1.0f,1.0f };
	myForce = { 0, 0, 0 };
	mySpawnRadius = 5;
	myMinSpawnSquare = { 0,0,0 };
	myMaxSpawnSquare = { 3,3,3 };
	mySpawnInCircle = false;
	mySpawnInSquare = false;
	myShouldMove = true;
	myShouldRotate = false;
	myShouldShrink = false;
	myShouldGrow = false;
	myShouldFade = true;
	myIndex = 0;
	myTimer = 0;
	myStartIndex = 0;
	myDebugTempRot = { 0,0,1 };
	myDebugRotInDegrees = 1;
	myWorldScale = 1.f;
	myTimeUntillNextSpriteSheet = 0.01f;
}

Particle3D::~Particle3D()
{
}

void Particle3D::Init(DreamEngine::Vector3f aPos, DreamEngine::Color aColor, DreamEngine::Vector3f aSizeScale, float aDecayTime, DreamEngine::Vector4f aShouldDoStuff, int aDirMin, int aDirMax, DreamEngine::Transform* aCameraTransform, bool aActionCamera, bool aShouldGrow)
{
	if (aCameraTransform != NULL)
	{
		myDebugTempRot = aCameraTransform->GetMatrix().GetForward();

	}
	myDecayTime = aDecayTime;
	myRespawnPos = aPos;
	myParticle3DInstance.myColor = aColor;
	if (myParticle3DInstance.myColor.myA > 0.5f)
	{
		int temp = 1;
		temp++;
	}
	DreamEngine::Matrix4x4f tempScale = myScale.CreateScaleMatrix(aSizeScale);
	myParticle3DInstance.myTransform = tempScale;
	myShouldMove = aShouldDoStuff.x;
	myShouldRotate = aShouldDoStuff.y;
	myShouldShrink = aShouldDoStuff.z;
	myShouldGrow = aShouldGrow;
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
		myParticle3DInstance.myTransform.SetPosition(myRespawnPos);
	}
	if (myShouldRotate == true)
	{
		/*myRotationSpeed = ((float)Random::GetRandomNum(1, 100) / 10) - 5.0f;
		myParticle3DInstance.myTransform.SetRotation(DreamEngine::Vector3f{ (float)Random::GetRandomNum(1, 360), 0, (float)Random::GetRandomNum(1, 360) });
		DreamEngine::Vector3f rot;
		DreamEngine::Vector3f pos;
		DreamEngine::Vector3f scale;
		myParticle3DInstance.myTransform.DecomposeMatrix(pos, rot, scale);
		myParticleOffset.SetRotation(rot);
		myParticle3DInstance.myTransform *= myParticleOffset;*/
	}
	if (myShouldMove == true)
	{
		float offset = (((float)aDirMax + (float)aDirMin) / 2) / 100.f;
		myDir = { ((float)UtilityFunctions::GetRandomFloat(static_cast<float>(aDirMin), static_cast<float>(aDirMax)) / 100.f) - offset, ((float)UtilityFunctions::GetRandomFloat(static_cast<float>(aDirMin), static_cast<float>(aDirMax)) / 100) - offset, ((float)UtilityFunctions::GetRandomFloat(static_cast<float>(aDirMin), static_cast<float>(aDirMax)) / 100.f) - offset };
	}
	//myIndex = myStartIndex;

	if (aActionCamera == true)
	{
		DreamEngine::Vector3f tempPos = myParticle3DInstance.myTransform.GetPosition();
		/*DreamEngine::Vector3f unused;
		myParticle3DInstance.myTransform.DecomposeMatrix(tempPos, unused, unused);*/
		myParticle3DInstance.myTransform.SetPosition(0);
		
		myParticle3DInstance.myTransform.SetForward(aCameraTransform->GetMatrix(true).GetForward());
		myParticle3DInstance.myTransform.SetUp(aCameraTransform->GetMatrix(true).GetUp());
		myParticle3DInstance.myTransform.SetRight(aCameraTransform->GetMatrix(true).GetRight());
		
		myParticle3DInstance.myTransform(1, 4) *= myWorldScale;
		myParticle3DInstance.myTransform(2, 4) *= myWorldScale;
		myParticle3DInstance.myTransform(3, 4) *= myWorldScale;
		myParticle3DInstance.myTransform(1, 1) *= myWorldScale;
		myParticle3DInstance.myTransform(1, 2) *= myWorldScale;
		myParticle3DInstance.myTransform(1, 3) *= myWorldScale;
		myParticle3DInstance.myTransform(2, 1) *= myWorldScale;
		myParticle3DInstance.myTransform(2, 2) *= myWorldScale;
		myParticle3DInstance.myTransform(2, 3) *= myWorldScale;
		myParticle3DInstance.myTransform(3, 1) *= myWorldScale;
		myParticle3DInstance.myTransform(3, 2) *= myWorldScale;
		myParticle3DInstance.myTransform(3, 3) *= myWorldScale;

		myParticle3DInstance.myTransform.SetPosition(tempPos);
	}
	else
	{
		DreamEngine::Vector3f tempPos = myParticle3DInstance.myTransform.GetPosition();
		/*DreamEngine::Vector3f unused;
		myParticle3DInstance.myTransform.DecomposeMatrix(tempPos, unused, unused);*/
		myParticle3DInstance.myTransform.SetPosition(0);
		myParticle3DInstance.myTransform.SetForward(aCameraTransform->GetMatrix(true).GetForward());
		myParticle3DInstance.myTransform.SetUp(aCameraTransform->GetMatrix(true).GetUp());
		myParticle3DInstance.myTransform.SetRight(aCameraTransform->GetMatrix(true).GetRight());
		
		myParticle3DInstance.myTransform(1, 4) *= myWorldScale;
		myParticle3DInstance.myTransform(2, 4) *= myWorldScale;
		myParticle3DInstance.myTransform(3, 4) *= myWorldScale;
		myParticle3DInstance.myTransform(1, 1) *= myWorldScale;
		myParticle3DInstance.myTransform(1, 2) *= myWorldScale;
		myParticle3DInstance.myTransform(1, 3) *= myWorldScale;
		myParticle3DInstance.myTransform(2, 1) *= myWorldScale;
		myParticle3DInstance.myTransform(2, 2) *= myWorldScale;
		myParticle3DInstance.myTransform(2, 3) *= myWorldScale;
		myParticle3DInstance.myTransform(3, 1) *= myWorldScale;
		myParticle3DInstance.myTransform(3, 2) *= myWorldScale;
		myParticle3DInstance.myTransform(3, 3) *= myWorldScale;

		myParticle3DInstance.myTransform.SetPosition(tempPos);
	}
}

void Particle3D::SetDecayTime(float aValue)
{
	myDecayTime = aValue;
}

void Particle3D::SetPos(DreamEngine::Vector3f aPos)
{
	myParticle3DInstance.myTransform.SetPosition({ aPos.x + myParticleSize.x / 2.f, aPos.y + myParticleSize.y / 2.f, aPos.z });
}

void Particle3D::SetDir(DreamEngine::Vector3f aDir)
{
	myDir = aDir;
}

void Particle3D::SetSize(DreamEngine::Vector4f aSizeScale)
{
	myScale = myParticle3DInstance.myTransform.CreateScaleMatrix(aSizeScale);
	myParticle3DInstance.myTransform *= myScale;
}

void Particle3D::SetColor(DreamEngine::Vector4f aColor)
{
	myParticle3DInstance.myColor = { aColor.x, aColor.y, aColor.z, aColor.y };
}

void Particle3D::SetColor(DreamEngine::Color aColor)
{
	myParticle3DInstance.myColor = aColor;
}

DreamEngine::Color Particle3D::GetColor()
{
	return myParticle3DInstance.myColor;
}

float Particle3D::GetAlpha()
{
	return myParticle3DInstance.myColor.myA;
}

void Particle3D::SetAlpha(float aValue)
{
	myParticle3DInstance.myColor.myA = aValue;
}

void Particle3D::SetRotationSpeed(DreamEngine::Vector3f aRotationSpeed)
{
	myRotationSpeed = aRotationSpeed;
}

void Particle3D::SetForce(DreamEngine::Vector3f aForce)
{
	myForce = aForce;
}

void Particle3D::SetOrigin(DreamEngine::Vector3f aPos)
{
	myParticle3DInstance.myTransform.SetPosition(aPos);
}

void Particle3D::SetRotation(DreamEngine::Vector3f aRotation)
{
	myParticle3DInstance.myTransform.SetRotation(aRotation);
}

void Particle3D::SpawnRandomInSquare(DreamEngine::Vector3f aMin, DreamEngine::Vector3f aMax)
{
	myParticle3DInstance.myTransform.SetPosition({ (myRespawnPos.x + (UtilityFunctions::GetRandomFloat(aMin.x * 100.f, aMax.x * 100.f)) / 100.f), (myRespawnPos.y + (UtilityFunctions::GetRandomFloat(aMin.y * 100.f, aMax.y * 100)) / 100.f), (myRespawnPos.z + (UtilityFunctions::GetRandomFloat(aMin.z * 100.f, aMax.z * 100.f)) / 100.f) });
}

void Particle3D::SpawnRandomInCircle(float aRadius)
{
	myDir = { ((float)UtilityFunctions::GetRandomFloat(1, 100) / 10) - 5.0f, ((float)UtilityFunctions::GetRandomFloat(1, 100) / 10) - 5.f,((float)UtilityFunctions::GetRandomFloat(1, 100) / 10) - 5.f };
	myDir.Normalize();
	float randomNum = UtilityFunctions::GetRandomFloat(0, aRadius);
	DreamEngine::Vector3f tempPos;
	tempPos.x = myRespawnPos.x + (myDir.x * randomNum);
	tempPos.y = myRespawnPos.y + (myDir.y * randomNum);
	tempPos.z = myRespawnPos.z + (myDir.z * randomNum);
	myParticle3DInstance.myTransform.SetPosition(tempPos);
}

void Particle3D::Update(float aDeltaTime)
{
	DreamEngine::Vector3f pivot = { 0.5f,0.5f,0 };
	//DreamEngine::Vector3f pos;
	//DreamEngine::Vector3f rot;
	//DreamEngine::Vector3f scale;

	//DreamEngine::Matrix4x4f N = myParticle3DInstance.myTransform;
	//N.SetPosition(DreamEngine::Vector3f{ 0,0,0 });
	//DreamEngine::Vector3f result = N * pivot;
	//myParticle3DInstance.myTransform.DecomposeMatrix(pos, rot, scale);
	//pos += result/10.f;
	if (myParticle3DInstance.myColor.myA < 0)
	{
		return;
	}
	if (myShouldFade == true)
	{
		myParticle3DInstance.myColor.myA -= aDeltaTime / myDecayTime;
	}

	if (myShouldMove == true)
	{
		//pos += (myDir + myForce) * aDeltaTime;
		
		DE::Vector3f tempPosition = myParticle3DInstance.myTransform.GetPosition();
		tempPosition += (myDir + myForce) * aDeltaTime;
		myParticle3DInstance.myTransform.SetPosition(tempPosition);
	}
	if (myShouldRotate == true)
	{

		/*ImGui::Begin("RotationTest");

		ImGui::DragFloat3("RotationAxis", &myDebugTempRot.x, 1, 0, 1);
		ImGui::DragInt("Degree of Rotation", &myDebugRotInDegrees, 1, 0, 259);*/
		float radianDegree = (float)(myDebugRotInDegrees * 3.1415f / 180.f);
		RotationMathBS::RotateTransformAroundPivot(myParticle3DInstance.myTransform, myDebugTempRot, radianDegree, GetPivotPoint());
		//ImGui::End();

	}
	if (myShouldShrink == true)
	{
		DE::Vector4f scaleX = { myParticle3DInstance.myTransform(1,1),myParticle3DInstance.myTransform(1,2) ,myParticle3DInstance.myTransform(1,3) ,myParticle3DInstance.myTransform(1,4) };
		if (scaleX.Length() >= 0)
		{
			float decay = 0.99f;
			myParticle3DInstance.myTransform(1,1) *= decay;
			myParticle3DInstance.myTransform(1,2) *= decay;
			myParticle3DInstance.myTransform(1,3) *= decay;
			myParticle3DInstance.myTransform(2, 1) *= decay;
			myParticle3DInstance.myTransform(2, 2) *= decay;
			myParticle3DInstance.myTransform(2, 3) *= decay;
			myParticle3DInstance.myTransform(3, 1) *= decay;
			myParticle3DInstance.myTransform(3, 2) *= decay;
			myParticle3DInstance.myTransform(3, 3) *= decay;
		}
	}
	if (myShouldGrow == true)
	{
		float decay = 1.01f;
		myParticle3DInstance.myTransform(1, 1) *= decay;
		myParticle3DInstance.myTransform(1, 2) *= decay;
		myParticle3DInstance.myTransform(1, 3) *= decay;
		myParticle3DInstance.myTransform(2, 1) *= decay;
		myParticle3DInstance.myTransform(2, 2) *= decay;
		myParticle3DInstance.myTransform(2, 3) *= decay;
		myParticle3DInstance.myTransform(3, 1) *= decay;
		myParticle3DInstance.myTransform(3, 2) *= decay;
		myParticle3DInstance.myTransform(3, 3) *= decay;
	}
}

void Particle3D::PlaySpriteSheet(float aDeltaTime, int aSpiteSheetPictureAmount, int myStartUV, bool shouldStop, bool myShouldSwapUV)
{
	myTimer += aDeltaTime;
	//myDir = { 0,0,0 };

	if (shouldStop == true && myIndex == aSpiteSheetPictureAmount)
	{
		myParticle3DInstance.myColor.myA = 0;
		return;
	}
	// Cycle the sheet
	if (myShouldSwapUV == true)
	{
		if (myTimer >= myTimeUntillNextSpriteSheet)
		{
			myIndex++;
			if (myIndex > aSpiteSheetPictureAmount || myIndex < myStartUV)
			{
				myIndex = static_cast<unsigned short>(myStartUV);
			}
			myTimer = 0.0f;
		}
	}
}

void Particle3D::RotateTowardsCamera(DreamEngine::Transform* aCameraTransform, DE::Vector3f aPosition)
{
	// Convert offset to a PhysX vector
	physx::PxVec3 localOffset = physx::PxVec3((myParticleSize.x / 9)*-1.f, myParticleSize.y / 9, 0.0f);
	// Get rotation quaternion
	physx::PxQuat rotation = physx::PxQuat(aCameraTransform->GetQuaternion().x, aCameraTransform->GetQuaternion().y, aCameraTransform->GetQuaternion().z, aCameraTransform->GetQuaternion().w);
	// Rotate offset into world space
	physx::PxVec3 rotatedOffset = rotation.rotate(localOffset);
	// Apply rotated offset
	physx::PxVec3 finalPosition = physx::PxVec3(aPosition.x, aPosition.y, aPosition.z) + rotatedOffset;

	DreamEngine::Vector3f tempPos = { finalPosition.x, finalPosition.y, finalPosition.z};
	myParticle3DInstance.myTransform.SetPosition(tempPos);
}

unsigned short Particle3D::GetUVIndex()
{
	return myIndex;
}

DreamEngine::Vector3f Particle3D::GetDirection()
{
	return myDir;
}

DreamEngine::Sprite3DInstanceData* Particle3D::GetSpriteInstance()
{
	return &myParticle3DInstance;
}

void Particle3D::SetSpawnRadius(float aRadius)
{
	mySpawnRadius = aRadius;
}

void Particle3D::SetMinSpawnSquare(DreamEngine::Vector3f aMinPos)
{
	myMinSpawnSquare = aMinPos;
}

void Particle3D::SetMaxSpawnSquare(DreamEngine::Vector3f aMaxPos)
{
	myMaxSpawnSquare = aMaxPos;
}

void Particle3D::SetSpawnInCircle(bool aBool)
{
	mySpawnInCircle = aBool;
}

void Particle3D::SetSpawnInSquare(bool aBool)
{
	mySpawnInSquare = aBool;
}

bool* Particle3D::GetShouldRotatePtr()
{
	return &myShouldRotate;
}

bool* Particle3D::GetShouldMovePtr()
{
	return &myShouldMove;
}

bool* Particle3D::GetShouldShrinkPtr()
{
	return &myShouldShrink;
}

bool* Particle3D::GetShouldGrowPtr()
{
	return &myShouldGrow;
}

bool* Particle3D::GetShouldFadePtr()
{
	return &myShouldFade;
}

void Particle3D::SetShouldMove(bool aBool)
{
	myShouldMove = aBool;
}

void Particle3D::SetShouldRotate(bool aBool)
{
	myShouldRotate = aBool;
}

void Particle3D::SetShouldShrink(bool aBool)
{
	myShouldShrink = aBool;
}

void Particle3D::SetShouldGrow(bool aBool)
{
	myShouldGrow = aBool;
}

void Particle3D::SetShouldFade(bool aBool)
{
	myShouldFade = aBool;
}

void Particle3D::SetUVIndex(int aNumber)
{
	myIndex = (unsigned short)aNumber;
}

void Particle3D::SetTimeUntillNextSpriteSheet(float aTime)
{
	myTimeUntillNextSpriteSheet = aTime;
}

float* Particle3D::GetDecayTimePtr()
{
	return &myDecayTime;
}

void Particle3D::SetParticleSize(DreamEngine::Vector2ui aSize)
{
	myParticleSize = aSize;
}

DreamEngine::Vector2f Particle3D::GetParticleSize()
{
	return myParticleSize;
}

DreamEngine::Vector3f Particle3D::GetPivotPoint()
{
	DreamEngine::Vector3f pivot = myParticle3DInstance.myTransform.GetPosition();
	DreamEngine::Vector3f forward = myParticle3DInstance.myTransform.GetForward();
	DreamEngine::Vector3f up = myParticle3DInstance.myTransform.GetUp();
	DreamEngine::Vector3f right = myParticle3DInstance.myTransform.GetRight();
	forward; up; right;

	pivot += right * 0.5f;
	pivot -= up * 0.5f;

	return pivot;
}

void Particle3D::SetWorldScale(float aScale)
{
	myWorldScale = aScale;
}

#ifndef _RETAIL

bool* Particle3D::GetSpawnInCirclePtr()
{
	return &mySpawnInCircle;
}

bool* Particle3D::GetSpawnInSquarePtr()
{
	return &mySpawnInSquare;
}

float* Particle3D::GetSpawnRadiusPtr()
{
	return &mySpawnRadius;
}

DreamEngine::Vector3f* Particle3D::GetRotationSpeedPtr()
{
	return &myRotationSpeed;
}

DreamEngine::Vector3f* Particle3D::GetSpawnSquareMinPtr()
{
	return &myMinSpawnSquare;
}

DreamEngine::Vector3f* Particle3D::GetSpawnSquareMaxPtr()
{
	return &myMaxSpawnSquare;
}

DreamEngine::Vector3f* Particle3D::GetRespawnPosPtr()
{
	return &myRespawnPos;
}

DreamEngine::Vector3f* Particle3D::GetDirPtr()
{
	return &myDir;
}

void Particle3D::SetSprite3DInstance(DreamEngine::Sprite3DInstanceData& aSpriteData)
{
	myParticle3DInstance = aSpriteData;
}
#endif // !_RETAIL
