#pragma once
#include "Component.h"

class HealthComponent : public Component
{
public:
	HealthComponent(GameObject& aObject) : Component(aObject)
	{
		myCurrentHealth = myMaxHealth;
	};
	void Init() { myIsActive = true; };
	const int& GetCurrentHealth() const;
	void SetCurrentHealth(const int aHealthAmount);
	const int& GetMaxHealth() const;
	void SetMaxHealth(const int aHealthAmount);
	const bool IsAlive() const;
	void TakeDamage(const int aDamageAmount);
	void Heal(const int aHealingAmount);
	void HealToMax();
private:
	int myMaxHealth = 3;
	int myCurrentHealth;
};