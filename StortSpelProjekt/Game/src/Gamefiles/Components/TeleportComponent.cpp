#include "TeleportComponent.h"

#include "Events/EventBus.h"
#include "ECS/SceneManager.h"
#include "Player.h"
#include "Shop.h"

component::TeleportComponent::TeleportComponent(Entity* parent, Entity* player, std::string newSceneName)
	:Component(parent)
{
	m_NewSceneName = newSceneName;
	m_pPlayerInstance = player;
	EventBus::GetInstance().Subscribe(this, &TeleportComponent::OnCollision);
}

component::TeleportComponent::~TeleportComponent()
{
	EventBus::GetInstance().Unsubscribe(this, &TeleportComponent::OnCollision);
}

void component::TeleportComponent::OnInitScene()
{
}

void component::TeleportComponent::OnUnInitScene()
{
}

void component::TeleportComponent::OnCollision(Collision* collisionEvent)
{
	auto func = [](const std::string newSceneName)
	{
		EventBus::GetInstance().Publish(&SceneChange(newSceneName));
		if (newSceneName == "ShopScene")
		{
			Player::GetInstance().IsInShop(true);
		}
		else
		{
			Player::GetInstance().IsInShop(false);
			EventBus::GetInstance().Publish(&RoundStart());
		}
	};

	if (collisionEvent->ent1 == m_pPlayerInstance && collisionEvent->ent2 == m_pParent)
	{
		func(m_NewSceneName);
	}
	else if (collisionEvent->ent2 == m_pPlayerInstance && collisionEvent->ent1 == m_pParent)
	{
		func(m_NewSceneName);
	}
}
