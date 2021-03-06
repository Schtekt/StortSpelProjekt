#ifndef UPGRADE_MELEE_ATTACK_SPEED_H
#define UPGRADE_MELEE_ATTACK_SPEED_H
#include "Upgrade.h"

// Upgrade description: This upgrade increases melee attack speed with 20% of base attack speed per level. Price doubles per level.
class UpgradeMeleeAttackSpeed : public Upgrade
{
public:
	UpgradeMeleeAttackSpeed(Entity* parent);
	~UpgradeMeleeAttackSpeed();

	void ApplyStat();
	void ApplyBoughtUpgrade();
	void IncreaseLevel();

	std::string GetDescription(unsigned int level);

private:
	float m_UpgradeFactor;
	float m_BaseAttackSpeed;
	float m_AttackPerSecond;
	int m_Percent;
};

#endif
