#include "HealthComponent.h"
#include "../Events/EventBus.h"
#include "../Events/Events.h"
#include "ECS/Entity.h"

component::HealthComponent::HealthComponent(Entity* parent, int hp) : Component(parent)
{
	m_Health = hp;

	// temp so that we can print when health = 0
	EventBus::GetInstance().Subscribe(this, &HealthComponent::printDeath);
	
}

component::HealthComponent::~HealthComponent()
{

}

void component::HealthComponent::SetHealth(int hp)
{
	m_Health = hp;
	if (m_Health <= 0 && m_Dead == false)
	{
		EventBus::GetInstance().Publish(&Death(m_pParent));
		m_Dead = true;
	}
}

void component::HealthComponent::ChangeHealth(int hpChange)
{
	m_Health += hpChange;
	if (m_Health <= 0 && m_Dead == false)
	{
		EventBus::GetInstance().Publish(&Death(m_pParent));
		m_Dead = true;
	}
}

int component::HealthComponent::GetHealth()
{
	return m_Health;
}


void component::HealthComponent::printDeath(Death* event)
{
	if (event->ent == m_pParent)
	{
		Log::Print("%s died!\n", event->ent->GetName().c_str());
		//if (m_pParent->HasComponent<component::AiComponent>())
		//{
		//	m_pParent->GetComponent<component::AiComponent>()
		//}
	}
}
