#pragma once

enum class eEnemyType
{
	eLeviathan,
	eMele,
	eShooting,
	DefaultTypeAndCount
};

struct EnemyType
{
	eEnemyType enemyType;
	int attackDamage;
	int maxHealth;
	float baseSpeed;
};