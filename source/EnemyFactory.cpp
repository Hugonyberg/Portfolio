#include "EnemyFactory.h"
#include "EnemyMele.h"
#include "EnemyShooting.h"
#include "Leviathan.h"

EnemyFactory::EnemyFactory()
{
	myEnemyTypes[static_cast<int>(eEnemyType::eLeviathan)].attackDamage = 1;
	myEnemyTypes[static_cast<int>(eEnemyType::eLeviathan)].baseSpeed = 1;
	myEnemyTypes[static_cast<int>(eEnemyType::eLeviathan)].enemyType = eEnemyType::eLeviathan;
	myEnemyTypes[static_cast<int>(eEnemyType::eLeviathan)].maxHealth = 1;

	myEnemyTypes[static_cast<int>(eEnemyType::eMele)].attackDamage = 1;
	myEnemyTypes[static_cast<int>(eEnemyType::eMele)].baseSpeed = 1;
	myEnemyTypes[static_cast<int>(eEnemyType::eMele)].enemyType = eEnemyType::eMele;
	myEnemyTypes[static_cast<int>(eEnemyType::eMele)].maxHealth = 1;

	myEnemyTypes[static_cast<int>(eEnemyType::eShooting)].attackDamage = 1;
	myEnemyTypes[static_cast<int>(eEnemyType::eShooting)].baseSpeed = 1;
	myEnemyTypes[static_cast<int>(eEnemyType::eShooting)].enemyType = eEnemyType::eShooting;
	myEnemyTypes[static_cast<int>(eEnemyType::eShooting)].maxHealth = 1;
}

std::shared_ptr<EnemyMele> EnemyFactory::CreateEnemyMelee() const
{
	return std::make_shared<EnemyMele>(EnemyMele(myEnemyTypes[static_cast<int>(eEnemyType::eMele)]));
}

std::shared_ptr<EnemyShooting> EnemyFactory::CreateEnemyShooting() const
{
	return std::make_shared<EnemyShooting>(EnemyShooting(myEnemyTypes[static_cast<int>(eEnemyType::eShooting)]));
}

std::shared_ptr<Leviathan> EnemyFactory::CreateLeviathan() const
{
	return std::make_shared<Leviathan>(Leviathan(myEnemyTypes[static_cast<int>(eEnemyType::eLeviathan)]));
}


const EnemyType& EnemyFactory::GetEnemyType(const eEnemyType anEnemyType) const
{
	return myEnemyTypes[static_cast<int>(anEnemyType)];
}