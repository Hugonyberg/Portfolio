
DE::Vector3f ReturnSteeringPosition(float aDeltaTime)
{
	mySteering.SetUpdateParameters(aDeltaTime, myTransform.GetPosition(), myVelocity, myTarget, mySpeed);

	DE::Vector3f steering = mySteering.UpdateSteering();
	steering = TruncateSteering(steering, myMaxForce);
	steering /= myMass;
	DE::Vector3f desiredVelocity = TruncateSteering(myVelocity + steering, mySpeed);

	desiredVelocity = ReturnAngularSmoothing(aDeltaTime, desiredVelocity);
	desiredVelocity = ReturnLerpWithPreviousVelocity(desiredVelocity);

	myVelocity = desiredVelocity;
	myPreviousVelocity = myVelocity;

	TurnTowardsLerped(myTarget, myTurnLerpFactor);

	return (myTransform.GetPosition() + myVelocity * aDeltaTime * mySpeed);
}

void TurnTowardsLerped(const DE::Vector3f aPos, float aLerpFactor)
{
	const float maxDeltaDegrees = myMaxTurnDegree;
	const DE::Vector3f toTarget = aPos - myTransform.GetPosition();

	const float horizontalDist = sqrtf(toTarget.x * toTarget.x + toTarget.z * toTarget.z);
	const float targetYaw = atan2f(toTarget.x, toTarget.z) * (180.f / DE::PI);
	const float targetPitch = -atan2f(toTarget.y, horizontalDist) * (180.f / DE::PI);

	const DE::Vector3f targetRotation{ targetPitch, targetYaw, 0.0f };
	const DE::Vector3f currentRotation = myTransform.GetRotation();

	const DE::Vector3f deltaRotation = 
	{
		NormalizeAngle(targetRotation.x - currentRotation.x),
		NormalizeAngle(targetRotation.y - currentRotation.y),
		NormalizeAngle(targetRotation.z - currentRotation.z)
	};

	const DE::Vector3f newRotation = currentRotation + deltaRotation * aLerpFactor;

	myTransform.SetRotation(newRotation);
	myModelInstance->SetRotation(newRotation);
}

DE::Vector3f ReturnLerpWithPreviousVelocity(DE::Vector3f aVelocity)
{
	DE::Vector3f blendedSteering = myPreviousVelocity + (aVelocity - myPreviousVelocity) * mySteeringSmoothingFactor;
	return blendedSteering;
}

DE::Vector3f ReturnAngularSmoothing(float aDeltaTime, DE::Vector3f aVelocity)
{
	const float maxTurnAngle = myMaxTurnRate * aDeltaTime;
	const DE::Vector3f currentDir = myVelocity.GetNormalized();
	const DE::Vector3f desiredDir = aVelocity.GetNormalized();

	if (myVelocity.LengthSqr() > myMinDistance && aVelocity.LengthSqr() > myMinDistance)
	{
		float dot = std::clamp(currentDir.Dot(desiredDir), -1.0f, 1.0f);
		const float angleBetween = acos(dot);

		if (angleBetween > maxTurnAngle)
		{
			const float t = maxTurnAngle / angleBetween;
			return Slerp(currentDir, desiredDir, t) * aVelocity.Length();
		}
	}

	return desiredDir;
}


void Enemy::Update(float aDeltaTime)
{
	if (myRecentlySpawned)
	{
		EnterGameArea(aDeltaTime); // Enters game area in a pattern ignoring collision
		return;
	}

	if (!HandleDeath(aDeltaTime)) return; // Returns early if dead

	if (myProjectilePool) myProjectilePool->Update(aDeltaTime);

	if (myBehavior) myBehavior->Update();

	DE::Vector3f newPosition = ReturnSteeringPosition(aDeltaTime);
	myTransform.SetPosition(newPosition);
	myModelInstance->SetLocation(newPosition);

	UpdatePhysXBody(newPosition);
	UpdateTimers(aDeltaTime);
}

bool FlyingEnemy::RaycastTowardsPlayer()
{
	if (myShouldAttackDefensePoint)
		return false;

	const auto playerPos = MainSingleton::GetInstance()->GetPlayer()->GetTransform()->GetPosition() + DE::Vector3f(0, 50.f, 0);
	const auto enemyPos = myTransform.GetPosition();
	const DE::Vector3f toPlayer = playerPos - enemyPos;
	const float distance = toPlayer.Length();

	if (distance < myMinRayDistance || distance >= myMaxRayDistance)
		return false;

	const DE::Vector3f rayDir = toPlayer / distance;
	const float dotProduct = myTransform.GetMatrix().GetForward().Dot(rayDir);

	if (distance >= myViewDistance && dotProduct < cosf(DegreesToRadians(myViewAngle)))
		return false;

	physx::PxRaycastBufferN<64> hitInfo;
	physx::PxQueryFilterData filterData;
	filterData.data.word0 = MainSingleton::GetInstance()->GetCollisionFiltering().Player |
		MainSingleton::GetInstance()->GetCollisionFiltering().Environment;

	physx::PxVec3 origin(enemyPos.x, enemyPos.y, enemyPos.z);
	physx::PxVec3 direction(toPlayer.x, toPlayer.y, toPlayer.z);
	auto* scene = MainSingleton::GetInstance()->GetPhysXScene();

	if (!scene->raycast(origin, direction, distance, hitInfo, physx::PxHitFlag::ePOSITION, filterData))
		return false;

	for (physx::PxU32 i = 0; i < hitInfo.nbTouches; ++i)
	{
		const auto& hit = hitInfo.touches[i];
		const char* hitName = hit.actor ? hit.actor->getName() : nullptr;

		if (hitName && strcmp(hitName, "Player") == 0 && hit.distance < myMaxRayDistance)
		{
			myTarget = myLastSeenPlayer = playerPos;
			myShouldAttackPlayer = true;
			myShouldSeekPlayer = false;
			myShouldOrbitPlayer = false;
			return true;
		}
		else
		{
			myShouldAttackPlayer = false;
			myShouldOrbitPlayer = false;
			if (!myShouldSeekPlayerTimer.IsDone())
				myShouldSeekPlayer = true;
		}
	}

	return false;
}

void FlyingEnemy::QueryExplosionHitPlayer()
{
	RemovePhysXBody();
	myExplodedThisFrame = true;

	const DE::Vector3f position = myTransform.GetPosition();
	const physx::PxTransform pose({ position.x, position.y, position.z });
	const physx::PxSphereGeometry explosionSphere(myExplosionRadius);

	physx::PxQueryFilterData filter;
	filter.data.word0 = MainSingleton::GetInstance()->GetCollisionFiltering().Player;

	physx::PxOverlapBufferN<64> hits;
	bool status = MainSingleton::GetInstance()->GetPhysXScene()->overlap(explosionSphere, pose, hits, filter);

	if (!status || !hits.hasAnyHits())
		return;

	for (physx::PxU32 i = 0; i < hits.nbTouches; ++i)
	{
		const auto& hit = hits.touches[i];
		MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ &myDamage, eMessageType::PlayerTakeDamage });
		break;
	}
}

void FlyingEnemy::Shoot()
{
	if (!myBurstCoolDownTimer.IsDone())
	{
		myBurstAmount = 5;
		myShootCoolDownTimer.Reset();
		return;
	}

	if (!myShootCoolDownTimer.IsDone() || myBurstAmount <= 0)
		return;

	myShootCoolDownTimer.Reset();

	const DE::Vector3f enemyPos = myTransform.GetPosition();
	const DE::Vector3f toTarget = myTarget - enemyPos;
	const float distance = toTarget.Length();

	if (distance < myMinDistance)
		return;

	const DE::Vector3f dir = toTarget / distance;
	const float dot = myTransform.GetMatrix().GetForward().Dot(dir);
	const float angleThreshold = cosf(DegreesToRadians(myShootAngle));

	if (dot < angleThreshold)
		return;

	// Compute gun barrel position
	DE::Vector3f gunPos = enemyPos + myTransform.GetMatrix().GetForward() * myGunForwardOffset;
	gunPos += myTransform.GetMatrix().GetRight() * (myShootFromRight ? myGunRightOffset : -myGunRightOffset);

	myProjectilePool->GetProjectile(gunPos, dir, myTarget, myDefensePointID);
	myShootFromRight = !myShootFromRight;

	if (--myBurstAmount <= 0)
		myBurstCoolDownTimer.Reset();
}