#include "Engine.h"
#include "RangeEnemyComponent.h"
#include "ECS/Scene.h"
#include "../ECS/SceneManager.h"
#include "../ECS/Components/Audio2DVoiceComponent.h"
#include "ProjectileComponent.h"


component::RangeEnemyComponent::RangeEnemyComponent(Entity* parent, SceneManager* sm, Scene* scene, Model* model, float scale, int damage, float velocity) : Component(parent)
{
	m_pSceneMan = sm;
	m_pScene = scene;
	m_pModel = model;
	m_Scale = scale;
	m_Damage = damage;
	m_Velocity = velocity;
	m_pVoiceComponent = nullptr;
	m_AttackInterval = 1.5;
	m_TimeAccumulator = 0.0;
	m_ParentName = m_pParent->GetName();
	m_NrOfProjectiles = 0;

	// this component will not have a 2d voice, but probably a 3dvoice so i'll leave this here for now as a "template"
	if (parent->GetComponent<component::Audio2DVoiceComponent>())
	{
		m_AudioPlay = true;
		// Fetch the player audio component (if one exists)
		m_pVoiceComponent = parent->GetComponent<component::Audio2DVoiceComponent>();
		m_pVoiceComponent->AddVoice(L"Fireball");
	}
	else
	{
		m_AudioPlay = false;
	}
}

component::RangeEnemyComponent::~RangeEnemyComponent()
{
}

void component::RangeEnemyComponent::OnInitScene()
{
}

void component::RangeEnemyComponent::OnUnInitScene()
{
}

void component::RangeEnemyComponent::Update(double dt)
{
	m_TimeAccumulator += dt;
}

void component::RangeEnemyComponent::SetVelocity(float velocity)
{
	m_Velocity = velocity;
}

float component::RangeEnemyComponent::GetVelocity()
{
	return m_Velocity;
}

void component::RangeEnemyComponent::SetAttackInterval(float interval)
{
	m_AttackInterval = interval;
}

float component::RangeEnemyComponent::GetAttackInterval() const
{
	return m_AttackInterval;
}

void component::RangeEnemyComponent::Attack(float3 direction)
{
	if (m_TimeAccumulator >= m_AttackInterval)
	{
		Entity* ent = m_pScene->AddEntity(m_ParentName + "RangeAttack" + std::to_string(++m_NrOfProjectiles));
		component::ModelComponent* mc = nullptr;
		component::TransformComponent* tc = nullptr;
		component::BoundingBoxComponent* bbc = nullptr;
		component::ProjectileComponent* pc = nullptr;
		component::AccelerationComponent* ac = nullptr;

		mc = ent->AddComponent<component::ModelComponent>();
		tc = ent->AddComponent<component::TransformComponent>();
		pc = ent->AddComponent<component::ProjectileComponent>(m_Damage);
		//ac = ent->AddComponent<component::AccelerationComponent>(98.2);	// no drop

		// get the position of parent entity
		float3 ParentPos = m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
		float3 forward = m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetForwardFloat3();
		float length = forward.length();

		double3 dim = m_pParent->GetComponent<component::ModelComponent>()->GetModelDim();
		float3 scale = {
						m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetScale().x,
						m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetScale().y,
						m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetScale().z
		};
		// add the forward vector to parent pos 
		// so the projectile doesn't spawn inside of enemy
		float3 pos;
		bool t_pose = m_pParent->GetComponent<component::BoundingBoxComponent>()->GetFlagOBB() & F_OBBFlags::T_POSE;
		pos.x = ParentPos.x + (forward.x / length) * ((static_cast<float>(!t_pose) * dim.x + static_cast<float>(t_pose) * dim.z) * scale.x / 2.0);
		pos.y = ParentPos.y + (forward.y / length);
		pos.z = ParentPos.z + (forward.z / length) * (dim.z * scale.z / 2.0);

		// initialize the components
		mc->SetModel(m_pModel);
		mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
		tc->GetTransform()->SetPosition(pos.x, pos.y, pos.z);
		tc->GetTransform()->SetMovement(direction.x * m_Velocity, direction.y * m_Velocity, direction.z * m_Velocity);
		tc->GetTransform()->SetScale(m_Scale);
		tc->GetTransform()->SetVelocity(m_Velocity);
		tc->Update(0.02);
		bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
		bbc->Init();
		Physics::GetInstance().AddCollisionEntity(ent);

		if (m_AudioPlay)
		{
			m_pVoiceComponent->Play(L"Fireball");
		}

		ent->Update(0);	// Init, so that the light doesn't spawn in origo first frame;
		tc->RenderUpdate(0);

		// add the entity to the sceneManager so it can be spawned in in run time
		ent->SetEntityState(true);
		m_pSceneMan->AddEntity(ent, m_pScene);

		m_TimeAccumulator = 0.0;
	}
}