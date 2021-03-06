#ifndef UPGRADE_H
#define UPGRADE_H
#include <string>
#include "Core.h"
// forward declaration
class Entity;

// Types of upgrades, for sorting/call purposes
enum F_UpgradeType
{
	RANGE = BIT(1),			// Range weapon/attack upgrades which go on a PROJECTILE entity's UpgradeComponent
	PLAYER = BIT(2),		// Upgrades that go on the player entity's UpgradeComponent
	ENEMYSPECIFIC = BIT(3)	// Upgrades only relevant for enemy entities. example: enemy explode on death
};

class Upgrade
{
public:
	Upgrade(Entity* parentEntity);
	virtual ~Upgrade();
	
	void SetParentEntity(Entity* parentEntity);
	// setters and getters for private member variables
	void SetName(std::string name);
	std::string GetName() const;
	void SetImage(std::string* imageName);
	std::string GetImage() const;
	int GetLevel() const;
	int GetMaxLevel() const;
	void SetType(unsigned int type);
	unsigned int GetType() const;
	void SetID(int id);
	int GetID();
	virtual std::string GetDescription(unsigned int level);
	int GetPrice();

	// If dt is needed for an upgrade
	virtual void Update(double dt);
	virtual void RenderUpdate(double dt);

	void SetLevel(unsigned int lvl);
	// Below are all functions needed by upgrades. Some will be used by several upgrades and others might be unique.
	// This way you can call functions that range over several "types" but might be called on in similar situations (such as melee/range OnHit).
	// Definitions are to be implemented in the separate upgrade classes. Add more as needed.
	

	// This function is supposed to increase the level of the upgrade, and to make sure that appropriate changes are made in the upgrade to reflect the level of the upgrade.
	virtual void IncreaseLevel();
	// This function should only be used when we want to increase just the level and not apply any stats.
	// Exist to fix a bug made by increasing the level of an upgrade in AllAvailableUpgrades map in upgradeManager.
	// So don't use this when you actually want to increase a level. This should not be overwritten!
	void IncreaseLevelOnly();
	// upgrades that are triggered on hit (ex: explosive, poison)
	virtual void OnHit(Entity* target);
	// specific for ranged hits
	virtual void OnRangedHit(Entity* target, Entity* projectile);
	// specific for melee hits
	virtual void OnMeleeHit(Entity* target);
	// upgrades that are triggered when taking damage
	virtual void OnDamage();
	// upgrades that are applied immediately, for example apply stat when bought in shop
	virtual void OnPickUp();
	// upgrades triggered on death
	virtual void OnDeath();
	// upgrades that affect the flight pattern (ex: gravity, speed)
	virtual void RangedFlight();
	// upgrades that modify the ranged attack (ex: multiple projectiles)
	virtual void RangedModifier();
	// upgrades that applies/adds stats (ex: more hp, more stamina)
	virtual void ApplyStat();
	// upgrades that need to be applied once when bought to take effect
	virtual void ApplyBoughtUpgrade();

protected:
	Entity* m_pParentEntity;
	// Name of the upgrade, for ease of access in shop or upgrade handlers
	std::string m_Name = "";
	// Image associated with the upgrade for shop
	std::string m_ImageName;
	// Price is used in shop when buying upgrades
	int m_Price;
	// Price that will not be increased by level
	int m_StartingPrice;
	// ID used in networking for sending which upgrades are applied.
	// Is set in UpgradeManagers fillUpgradeMap() function.
	int m_ID;
	// What level the upgrade is, for keeping track of stacking of upgrades and price
	int m_Level = 0;
	// Some upgrade might have a maximum upgrade level. Set in upgrade class, else "unlimited" levels.
	int m_MaxLevel = INT_MAX;
	// What type of upgrade this is, for ease of sorting or making calls to certain types
	unsigned int m_Type = 0;
};

#endif
