#include "HealthComponent.h"
#include <DreamEngine\utilities\UtilityFunctions.h>

const int& HealthComponent::GetCurrentHealth() const
{
    return myCurrentHealth;
}

void HealthComponent::SetCurrentHealth(const int aHealthAmount)
{
    myCurrentHealth = UtilityFunctions::Min(aHealthAmount, myMaxHealth);
}

const int& HealthComponent::GetMaxHealth() const
{
    return myMaxHealth;
}

void HealthComponent::SetMaxHealth(const int aHealthAmount)
{
    myMaxHealth = aHealthAmount;
}

const bool HealthComponent::IsAlive() const
{
    return myCurrentHealth > 0;
}

void HealthComponent::TakeDamage(const int aDamageAmount)
{
    myCurrentHealth = UtilityFunctions::Max(myCurrentHealth - aDamageAmount, 0);
}

void HealthComponent::Heal(const int aHealingAmount)
{
    myCurrentHealth = UtilityFunctions::Min(myCurrentHealth + aHealingAmount, myMaxHealth);
}

void HealthComponent::HealToMax()
{
    myCurrentHealth = myMaxHealth;
}
