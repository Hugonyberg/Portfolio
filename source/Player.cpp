#include "Player.h"
#include "HealthComponent.h"
#include "RigidBodyComponent.h"
#include "Message.h"
#include "MainSingleton.h"
#include <DreamEngine/engine.h>
#include <DreamEngine/graphics/ModelFactory.h>
#include <DreamEngine/graphics/GraphicsEngine.h>
#include <DreamEngine/utilities/UtilityFunctions.h>

#include <DreamEngine/graphics/ModelDrawer.h>

Player::~Player()
{
	MainSingleton::GetInstance()->GetPostMaster().Unsubscribe(eMessageType::PlayerLookAround, this);
	MainSingleton::GetInstance()->GetPostMaster().Unsubscribe(eMessageType::PlayerMove, this);
	MainSingleton::GetInstance()->GetPostMaster().Unsubscribe(eMessageType::PlayerSprint, this);
	MainSingleton::GetInstance()->GetPostMaster().Unsubscribe(eMessageType::PlayerJump, this);
	MainSingleton::GetInstance()->GetPostMaster().Unsubscribe(eMessageType::PlayerInteract, this);
}

void Player::Deallocate()
{
	MainSingleton::GetInstance()->GetPostMaster().Unsubscribe(eMessageType::PlayerLookAround, this);
	MainSingleton::GetInstance()->GetPostMaster().Unsubscribe(eMessageType::PlayerMove, this);
	MainSingleton::GetInstance()->GetPostMaster().Unsubscribe(eMessageType::PlayerSprint, this);
	MainSingleton::GetInstance()->GetPostMaster().Unsubscribe(eMessageType::PlayerJump, this);
	MainSingleton::GetInstance()->GetPostMaster().Unsubscribe(eMessageType::PlayerInteract, this);
}

void Player::Init()
{
	myModelInstance = std::make_shared<DreamEngine::ModelInstance>(DE::ModelFactory::GetInstance().GetModelInstance(L"3D/SM_Ch_Dog.fbx"));

	myTransform.SetPosition({ myBoat->GetTransform()->GetPosition().x, myBoat->GetTransform()->GetPosition().y + 200.0f, myBoat->GetTransform()->GetPosition().z });
	myTransform.SetScale(1.0f);
	myModelInstance->SetTransform(myTransform);
	myModelInstance->SetScale(1.0f);

	myAmmoModel = std::make_shared<DreamEngine::ModelInstance>(DE::ModelFactory::GetInstance().GetModelInstance(L"3D/spherePrimitive.fbx"));
	myAmmoModel->SetTransform(myModelInstance->GetTransform());
	myAmmoModel->SetScale(15.0f);

	myRepairModel = std::make_shared<DreamEngine::ModelInstance>(DE::ModelFactory::GetInstance().GetModelInstance(L"3D/cubePrimitive.fbx"));
	myRepairModel->SetTransform(myModelInstance->GetTransform());
	myRepairModel->SetScale(15.0f);

	CaptureLocalTransform(*myBoat->GetTransform());
	SetUpSubscriptions();
}

void Player::SetUpComponents()
{

}

void Player::SetUpSubscriptions()
{
	MainSingleton::GetInstance()->GetPostMaster().Subscribe(eMessageType::PlayerLookAround, this);
	MainSingleton::GetInstance()->GetPostMaster().Subscribe(eMessageType::PlayerMove, this);
	MainSingleton::GetInstance()->GetPostMaster().Subscribe(eMessageType::PlayerSprint, this);
	MainSingleton::GetInstance()->GetPostMaster().Subscribe(eMessageType::PlayerJump, this);
	MainSingleton::GetInstance()->GetPostMaster().Subscribe(eMessageType::PlayerInteract, this);
	MainSingleton::GetInstance()->GetPostMaster().Subscribe(eMessageType::WaveNumberChanged, this);
}

void Player::SetUpModel()
{

}

void Player::InitializeMemberVariables()
{
}

void Player::Update(const float aDeltaTime)
{
	SyncTransformWithBoat();

	if (myStats.shouldMove)
	{
		MovePlayer(aDeltaTime);
	}

	if (myStats.isLocked)
	{
		HandleInteractingObject(aDeltaTime);
	}

	Interact();
	ResetInputMessages();
}

void Player::Receive(const Message& aMessage)
{
	switch (aMessage.messageType)
	{
		case eMessageType::PlayerLookAround:
		{
			if (MainSingleton::GetInstance()->GetWasActiveWindowLastFrame() && GetForegroundWindow() == *DE::Engine::GetInstance()->GetHWND())
			{
				myStats.shouldRotate = true;
				myStats.rotationDelta = *static_cast<DE::Vector2i*>(aMessage.messageData);
			}
			break;
		}
		case eMessageType::PlayerMove:
		{
			myStats.shouldMove = true;
			DE::Vector2i moveDir = *static_cast<DE::Vector2i*>(aMessage.messageData);
			myStats.moveInputDirection = DE::Vector2f(static_cast<float>(moveDir.x) / 100.0f, static_cast<float>(moveDir.y) / 100.0f);
			break;
		}
		case eMessageType::PlayerInteract:
		{
			myStats.shouldInteract = true;
			break;
		}		
	}
}

void Player::MovePlayer(float aDeltaTime)
{
	if (!myStats.shouldMove || myStats.isLocked) return;

	DE::Vector3f localPosition = myLocalTransform.GetPosition();
	const auto& boatTransform = myBoat->GetTransform();

	// Camera
	const auto& camera = MainSingleton::GetInstance()->GetActiveCamera();
	DE::Vector3f cameraForward = camera->GetTransform().GetMatrix().GetForward();
	DE::Vector3f cameraRight = camera->GetTransform().GetMatrix().GetRight();
	cameraForward.y = 0.0f;
	cameraRight.y = 0.0f;
	cameraForward.Normalize();
	cameraRight.Normalize();

	// Input direction
	DE::Vector3f moveDirection = (cameraForward * myStats.moveInputDirection.y) +
		(cameraRight * myStats.moveInputDirection.x);

	if (moveDirection.LengthSqr() > 0.0f)
	{
		moveDirection.Normalize();

		// Convert local -> world, apply movement, then convert back to local
		const DE::Matrix4x4f& boatMatrix = boatTransform->GetMatrix();
		DE::Vector3f worldPosition = boatMatrix * DE::Vector4f(localPosition, 1.0f);
		worldPosition += moveDirection * myStats.moveSpeed * aDeltaTime;
		DE::Vector3f newLocalPosition = boatMatrix.GetInverse() * DE::Vector4f(worldPosition, 1.0f);

		// Check boundaries
		if (newLocalPosition.x < 300.0f && newLocalPosition.x > -300.0f &&
			newLocalPosition.z < 800.0f && newLocalPosition.z > -800.0f)
		{
			myLocalTransform.SetPosition(newLocalPosition);
		}

		// Update rotation to face movement direction
		const float yawRadians = std::atan2(moveDirection.x, moveDirection.z);
		const DreamEngine::Quaternionf worldRotation({ 0.0f, yawRadians, 0.0f });
		const DreamEngine::Quaternionf localRotation = boatTransform->GetQuaternion().GetConjugate() * worldRotation;
		myLocalTransform.SetRotation(localRotation.GetEulerAnglesDegrees());
	}
}


bool Player::CheckIfGrounded()
{
	const physx::PxVec3 direction(0.0f, -1.0f, 0.0f);
	physx::PxQueryFilterData queryFilterData;
	queryFilterData.data.word0 = MainSingleton::GetInstance()->GetCollisionFiltering().Environment;
	physx::PxRaycastBuffer hitBuffer;

	if (MainSingleton::GetInstance()->GetPhysXScene()->raycast(GetComponent<RigidBodyComponent>()->GetBody()->getGlobalPose().p, direction, myStats.groundedTraceLength, hitBuffer, physx::PxHitFlag::eNORMAL, queryFilterData))
	{
		myStats.groundedDotProduct = hitBuffer.block.normal.dot(direction);
		if (myStats.groundedDotProduct < -0.8f)
		{
			myStats.groundedNormal = hitBuffer.block.normal;
			return true;
		}
	}
	myStats.groundedDotProduct = 0.0f;
	return false;
}


void Player::HandleInteractingObject(float aDeltaTime)
{
	if (myInteractedComponent)
	{
		myInteractedComponent->HandlePlayerInput(*this);
	}
}

void Player::Interact()
{
	// TODO: Gamepad
	auto& input = MainSingleton::GetInstance()->GetInputManager();
	bool pressedE = input.IsKeyDown(DE::eKeyCode::E);
	bool pressedR = input.IsKeyDown(DE::eKeyCode::R);
	bool pressedF = input.IsKeyDown(DE::eKeyCode::F);
	bool held = input.IsKeyHeld(DE::eKeyCode::E);
	bool released = input.IsKeyUp(DE::eKeyCode::E);

	if (pressedE || pressedR || pressedF)
	{
		auto& compPointer = myBoat->FindComponentInRange(myTransform.GetPosition());
		
		myInteractedComponent = compPointer; // TODO: Figure out the best implementation
		if (compPointer)
		{
			if (pressedE)
			{
				compPointer->OnInteractSouthButton(*this);
			}
			else if (pressedR)
			{
				compPointer->OnInteractEastButton(*this);
			}
			else if (pressedF)
			{
				compPointer->OnInteractNorthButton(*this);
			}
		}
	}

	if (pressedR && myStats.isCarryingObject)
	{
		myBoat->GiveObject(myStats.inventory, 1);
		RemoveObject();
	}

	auto& closestComp = myBoat->FindComponentInRange(myTransform.GetPosition());
	if (closestComp)
	{
		closestComp->TriggerContact(*this);
	}
}

void Player::ResetInputMessages()
{
	myStats.shouldMove = false;
	myStats.shouldInteract = false;
}

void Player::Render(DE::GraphicsEngine& aGraphicsEngine)
{
	aGraphicsEngine.GetModelDrawer().DrawGBCalc(*myModelInstance.get());

	if (myStats.isCarryingObject) // TODO : Prata med matilda om finare lösning
	{
		myInventoryModel->SetTransform(myModelInstance->GetTransform());
		myInventoryModel->SetScale(50.0f);
		myInventoryModel->SetLocation(myModelInstance->GetTransform().GetPosition() 
			+ myInventoryModel->GetTransform().GetMatrix().GetForward() * 1.5f 
			+ myInventoryModel->GetTransform().GetMatrix().GetUp() * 1.5f);
		aGraphicsEngine.GetModelDrawer().DrawGBCalc(*myInventoryModel.get());
	}
}

void Player::SetLockPosition(DE::Vector3f aPosition, DE::Vector3f aDirection)
{
	SyncTransformWithBoat();
	const auto& boatTransform = myBoat->GetTransform();

	// Convert local -> world, apply movement, then convert back to local
	const DE::Matrix4x4f& boatMatrix = boatTransform->GetMatrix();
	DE::Vector3f worldPosition = aPosition;
	DE::Vector3f newLocalPosition = boatMatrix.GetInverse() * DE::Vector4f(worldPosition, 1.0f);

	// Check boundaries
	if (newLocalPosition.x < 300.0f && newLocalPosition.x > -300.0f &&
		newLocalPosition.z < 800.0f && newLocalPosition.z > -800.0f)
	{
		myLocalTransform.SetPosition(newLocalPosition);
	}

	// Update rotation to face movement direction
	const float yawRadians = std::atan2(aDirection.x, aDirection.z);
	const DreamEngine::Quaternionf worldRotation({ 0.0f, yawRadians, 0.0f });
	const DreamEngine::Quaternionf localRotation = boatTransform->GetQuaternion().GetConjugate() * worldRotation;
	myLocalTransform.SetRotation(localRotation.GetEulerAnglesDegrees());

}

void Player::GiveObject(eObjectType aType)
{
	switch (aType)
	{
	case eObjectType::Ammo:
		myInventoryModel = myAmmoModel;
		break;
	case eObjectType::Repair:
		myInventoryModel = myRepairModel;
		break;
	}
	myStats.inventory = aType;
	myStats.isCarryingObject = true;
}

void Player::RemoveObject()
{
	myStats.inventory = eObjectType::None;
	myStats.isCarryingObject = false;
}

void Player::CaptureLocalTransform(const DreamEngine::Transform& boatXform)
{
	DreamEngine::Transform worldXform = myModelInstance->GetTransform();

	DE::Vector3f worldPosition = worldXform.GetPosition();
	DE::Vector3f localPosition = boatXform.InverseTransformPosition(worldPosition);

	DE::Quaternionf boatQuat = boatXform.GetQuaternion();
	DE::Quaternionf inverseBoatQuat = boatQuat.GetConjugate();
	DE::Quaternionf worldQuat = worldXform.GetQuaternion();
	DE::Quaternionf localQuat = inverseBoatQuat * worldQuat;

	DE::Vector3f localScale = worldXform.GetScale();

	myLocalTransform = DreamEngine::Transform
	{
		localPosition,
		localQuat,
		localScale
	};
}

void Player::SyncTransformWithBoat()
{
	const DreamEngine::Transform& boatXform = *myBoat->GetTransform();

	DE::Vector3f localPosition = myLocalTransform.GetPosition();
	DE::Vector3f worldPosition = boatXform.TransformPosition(localPosition);

	DE::Quaternionf boatQuat = boatXform.GetQuaternion();
	DE::Quaternionf localQuat = myLocalTransform.GetQuaternion();
	DE::Quaternionf worldQuat = boatQuat * localQuat;

	DE::Vector3f worldScale = myLocalTransform.GetScale();

	DreamEngine::Transform newXform{ worldPosition, worldQuat, worldScale };
	myModelInstance->SetTransform(newXform);
	myTransform = myModelInstance->GetTransform();
}
