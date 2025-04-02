void Projectile::SetDamage(const int aDamageAmount)
{
	myDamage = aDamageAmount;
}

void Projectile::Update(float aDeltaTime)
{
	if (myIsActive)
	{
		HandleCollision();
		
		if (myTimer < myTimeLimit)
		{
			myTimer += aDeltaTime;

			DreamEngine::Vector3f position = myTransform.GetPosition();
			position += myVelocity.GetNormalized() * mySpeed * aDeltaTime;

			myTransform.SetPosition(position);
			myModelInstance->SetTransform(myTransform);
		}
		else
		{
			myTimer = 0.0f;
			Deactivate();
		}
	}
}

// Ignoring Roll
void Projectile::TurnTowards(DE::Vector3f aPos)
{
	const DE::Vector3f toTarget = myTransform.GetPosition() - aPos;
	const float horizontalDist = sqrtf(toTarget.x * toTarget.x + toTarget.z * toTarget.z);
	const float yaw = atan2f(toTarget.x, toTarget.z) * (180.f / DE::PI); // Rotation around Y-axis
	const float pitch = -atan2f(toTarget.y, horizontalDist) * (180.f / DE::PI); // Rotation around X-axis

	const DE::Vector3f rotation(pitch, yaw, 0.f);
	myTransform.SetRotation(rotation);
	myModelInstance->SetRotation(rotation);
}

void Projectile::Activate(DreamEngine::Vector3f aPosition, DreamEngine::Vector3f aDirection, DreamEngine::Vector3f aTargetPosition)
{
	myIsActive = true;
	myTransform.SetPosition(aPosition);
	TurnTowards(aTargetPosition);
	myTargetPosition = aTargetPosition;
	myVelocity = myTargetPosition - myTransform.GetPosition();
}

void Projectile::Deactivate()
{
	myIsActive = false;
}

const int Projectile::GetDamage() const
{
	return myDamage;
}

void Projectile::HandleCollision()
{
	const physx::PxSphereGeometry sphereGeometry(myScale);
	const DE::Vector3f position = myTransform.GetPosition();
	const physx::PxTransform pose({ position.x, position.y, position.z });

	physx::PxQueryFilterData filter;
	filter.data.word0 = MainSingleton::GetInstance()->GetCollisionFiltering().Player |
		MainSingleton::GetInstance()->GetCollisionFiltering().DefensePoint;
	filter.flags = physx::PxQueryFlag::eANY_HIT | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC;

	physx::PxOverlapBuffer overlap;
	bool status = MainSingleton::GetInstance()->GetPhysXScene()->overlap(sphereGeometry, pose, overlap, filter);

	if (!status || !overlap.hasAnyHits())
		return;

	for (physx::PxU32 i = 0; i < overlap.getNbAnyHits(); ++i)
	{
		const auto& hit = overlap.getAnyHit(i);
		const char* name = hit.actor ? hit.actor->getName() : nullptr;

		if (!name)
			continue;

		if (strcmp(name, "Player") == 0)
		{
			MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &myDamage, eMessageType::PlayerTakeDamage });
			Deactivate();
			return;
		}
		else if (strcmp(name, "Drill") == 0)
		{
			std::pair<int, char> msg = { myDamage, targetID };
			MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &msg, eMessageType::DrillTakeDamage });
			Deactivate();
			return;
		}
	}
}

void Projectile::Render(DreamEngine::GraphicsEngine& aGraphicsEngine)
{
	if (myIsActive)
	{
		aGraphicsEngine.GetModelDrawer().DrawGBCalc(*myModelInstance);
	}
}