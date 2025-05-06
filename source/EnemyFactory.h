#pragma once
#include <array>
#include <memory>
#include "EnemyType.h"

class EnemyMele;
class EnemyShooting;
class Leviathan;

class EnemyFactory
{
public:
	EnemyFactory();

	std::shared_ptr<EnemyMele> CreateEnemyMelee() const;
	std::shared_ptr<EnemyShooting> CreateEnemyShooting() const;
	std::shared_ptr<Leviathan> CreateLeviathan() const;

	const EnemyType& GetEnemyType(const eEnemyType anEnemyType) const;

private:
	std::array<EnemyType, static_cast<int>(eEnemyType::DefaultTypeAndCount)> myEnemyTypes;
};