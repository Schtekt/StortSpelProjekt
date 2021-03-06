#include "UpgradeRangeVelocity.h"
#include "EngineMath.h"
#include "ECS/Entity.h"
#include "Components/RangeComponent.h"

UpgradeRangeVelocity::UpgradeRangeVelocity(Entity* parent)
	:Upgrade(parent)
{
	// name the upgrade!
	SetName("UpgradeRangeVelocity");
	// set upgrade type/types!
	SetType(F_UpgradeType::PLAYER);		//The range velocity is set on a rangecomponent which gones on the player, not the projectile entity
	// set the price of this upgrade 
	m_Price = 100;
	m_StartingPrice = m_Price;

	// this upgrade will have a max level
	m_MaxLevel = 5;

	m_ImageName = "RangeUpgrade.png";
}

UpgradeRangeVelocity::~UpgradeRangeVelocity()
{
}

void UpgradeRangeVelocity::IncreaseLevel()
{
	if (m_Level < m_MaxLevel)
	{
		m_Level++;
		float oldVelocity = m_pParentEntity->GetComponent<component::RangeComponent>()->GetVelocity();
		// double the velocity of the shots
		float newVelocity = oldVelocity + 30;
		m_pParentEntity->GetComponent<component::RangeComponent>()->SetVelocity(newVelocity);
		m_Price = m_StartingPrice * (m_Level + 1);
	}
}

void UpgradeRangeVelocity::ApplyBoughtUpgrade()
{
}

std::string UpgradeRangeVelocity::GetDescription(unsigned int level)
{
	return "Range Velocity: Increases velocity of the range attacks with 30 per level. Max velocity is 300, at upgrade level 5";
}
