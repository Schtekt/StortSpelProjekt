#include "ProjectileComponent.h"
#include "Engine.h"
#include "Events/Events.h"
#include "../ECS/Entity.h"
#include "HealthComponent.h"
#include "UpgradeComponents/UpgradeComponent.h"

component::ProjectileComponent::ProjectileComponent(Entity* parent, int damage) : Component(parent)
{
	m_Damage = damage;

	EventBus::GetInstance().Subscribe(this, &ProjectileComponent::hit);
}

component::ProjectileComponent::~ProjectileComponent()
{

}

void component::ProjectileComponent::Update(float dt)
{
	
}

void component::ProjectileComponent::OnInitScene()
{
}

void component::ProjectileComponent::OnUnInitScene()
{
}

void component::ProjectileComponent::hit(Collision* event)
{
	// if we are the one that collided then make 
	// the other object lose health (if it has health)
	if (event->ent1 == m_pParent)
	{
		if (event->ent2->HasComponent<component::HealthComponent>())
		{
			event->ent2->GetComponent<component::HealthComponent>()->ChangeHealth(-m_Damage);
		}
		// Call on upgrade on hit functions
		if (m_pParent->HasComponent<component::UpgradeComponent>())
		{
			m_pParent->GetComponent<component::UpgradeComponent>()->OnHit();
			m_pParent->GetComponent<component::UpgradeComponent>()->OnRangedHit();
		}
		Log::Print("Collision!\n");
		EventBus::GetInstance().Unsubscribe(this, &ProjectileComponent::hit);
	}
	else if (event->ent2 == m_pParent)
	{
		if (event->ent1->HasComponent<component::HealthComponent>())
		{
			event->ent1->GetComponent<component::HealthComponent>()->ChangeHealth(-m_Damage);
		}
		// Call on upgrade on hit functions
		if (m_pParent->HasComponent<component::UpgradeComponent>())
		{
			m_pParent->GetComponent<component::UpgradeComponent>()->OnHit();
			m_pParent->GetComponent<component::UpgradeComponent>()->OnRangedHit();
		}
		Log::Print("Collision!\n");
		EventBus::GetInstance().Unsubscribe(this, &ProjectileComponent::hit);
	}
}
