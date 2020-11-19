#ifndef UPGRADEPOISONATTACK_H
#define UPGRADEPOISONATTACK_H
#include "Upgrade.h"

class Entity;

class UpgradePoisonAttack : public Upgrade
{
public:
	UpgradePoisonAttack(Entity* parent);
	~UpgradePoisonAttack();

	// Inherited functions that are needed for this upgrade
	void IncreaseLevel();
	void OnRangedHit(Entity* target);
	void ApplyBoughtUpgrade();

	std::string GetDescription(unsigned int level);

private:
	int m_Damage;
	int m_NrOfTicks;
	float m_Slow;
	double m_TickDuration;

};

#endif