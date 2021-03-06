#include "Engine.h"
#include "RangeComponent.h"
#include "Player.h"
#include "UpgradeManager.h"

#include "Events/Events.h"
#include "ProjectileComponent.h"

#include <Bullet/btBulletCollisionCommon.h>

#include "ECS/Scene.h"
#include "../ECS/SceneManager.h"
#include "../ECS/Components/Audio2DVoiceComponent.h"

#include "UpgradeComponents/UpgradeComponent.h"
#include "../Physics/CollisionCategories/PlayerProjectileCollisionCategory.h"
#include "../Memory/PoolAllocator.h"

component::RangeComponent::RangeComponent(Entity* parent, SceneManager* sm, Scene* scene, Model* model, float scale, int damage, float velocity) : Component(parent)
{
	m_pSceneMan = sm;
	m_pScene = scene;
	m_pModel = model;
	m_Scale = scale;
	m_Damage = damage;
	m_Velocity = velocity;
	m_pVoiceComponent = nullptr;
	m_AttackInterval = 0.5;
	m_TimeAccumulator = 0.0;
	m_NrOfProjectiles = 0;
	m_ProjectileRestitution = 0.0f;

	AssetLoader* al = AssetLoader::Get();
	m_pParticleTexture = static_cast<Texture2DGUI*>(al->LoadTexture2D(L"../Vendor/Resources/Textures/Particles/player_magic.png"));

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

component::RangeComponent::~RangeComponent()
{
	
}

void component::RangeComponent::OnInitScene()
{
}

void component::RangeComponent::OnUnInitScene()
{
}

void component::RangeComponent::Update(double dt)
{
	m_TimeAccumulator += dt;
}

void component::RangeComponent::SetVelocity(float velocity)
{
	m_Velocity = velocity;
}

float component::RangeComponent::GetVelocity()
{
	return m_Velocity;
}

void component::RangeComponent::SetAttackInterval(float interval)
{
	m_AttackInterval = interval;
}

void component::RangeComponent::SetDamage(int damage)
{
	m_Damage = damage;
}

void component::RangeComponent::ChangeDamage(int change)
{
	m_Damage += change;
}

float component::RangeComponent::GetAttackInterval() const
{
	return m_AttackInterval;
}

void component::RangeComponent::MakeProjectileBouncy(bool bouncy)
{
	m_ProjectileRestitution = 0.5f * static_cast<float>(bouncy);
}

void component::RangeComponent::Attack()
{
	if (m_TimeAccumulator >= m_AttackInterval)
	{
		m_pParent->GetComponent<component::AnimationComponent>()->PlayAnimation("Attack_Swing_Left", false);

		Entity* ent = m_pScene->AddEntity("RangeAttack" + std::to_string(++m_NrOfProjectiles));
		component::ModelComponent* mc = nullptr;
		component::TransformComponent* tc = nullptr;
		component::BoundingBoxComponent* bbc = nullptr;
		component::ProjectileComponent* pc = nullptr;
		component::UpgradeComponent* uc = nullptr;
		component::PointLightComponent* plc = nullptr;
		component::ParticleEmitterComponent* pec = nullptr;

		mc = ent->AddComponent<component::ModelComponent>();
		tc = ent->AddComponent<component::TransformComponent>();
		pc = ent->AddComponent<component::ProjectileComponent>(m_Damage);
		uc = ent->AddComponent<component::UpgradeComponent>();
		plc = ent->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::FLICKER);
		plc->SetFlickerRate(3);
		plc->SetFlickerAmplitude(0.16);
		
		//tlc = ent->AddComponent<component::TemporaryLifeComponent>(duration);

		// Applying all range uppgrades to the new projectile entity "RangeAttack"
		if (m_pParent->HasComponent<component::UpgradeComponent>())
		{
			Player::GetInstance().GetUpgradeManager()->ApplyRangeUpgrades(ent);
		}

		// get the pos of parent object and forward of camera 
		// so we know where to spawn and in which direction
		float3 ParentPos = m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
		float3 forward = m_pScene->GetMainCamera()->GetDirectionFloat3();
		float length = forward.length();

		double3 dim = m_pParent->GetComponent<component::ModelComponent>()->GetModelDim();
		float3 scale = {
						m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetScale().x,
						m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetScale().y,
						m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetScale().z
		};
		// add the forward vector to parent pos 
		// so the projectile doesn't spawn inside of us
		float3 pos;
		bool t_pose = m_pParent->GetComponent<component::BoundingBoxComponent>()->GetFlagOBB() & F_OBBFlags::T_POSE;
		pos.x = ParentPos.x + (forward.x / length) * ((static_cast<float>(!t_pose)* dim.x + static_cast<float>(t_pose)* dim.z)* scale.x / 2.0) * 1.1f;
		pos.y = ParentPos.y + (forward.y / length);
		pos.z = ParentPos.z + (forward.z / length) * (dim.z * scale.z / 2.0) * 1.1f;

		// Raytrace from the middle of the screen
		DirectX::XMVECTOR rayInWorldSpacePos = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR rayInWorldSpaceDir = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		// Transform a ray to worldSpace by taking inverse of the view matrix
		rayInWorldSpacePos = DirectX::XMVector3TransformCoord(rayInWorldSpacePos, *m_pScene->GetMainCamera()->GetViewMatrixInverse());
		rayInWorldSpaceDir = DirectX::XMVector3TransformNormal(rayInWorldSpaceDir, *m_pScene->GetMainCamera()->GetViewMatrixInverse());
		DirectX::XMFLOAT4 rayInWorldSpacePosFloat4;
		DirectX::XMStoreFloat4(&rayInWorldSpacePosFloat4, rayInWorldSpacePos);
		DirectX::XMFLOAT4 rayInWorldSpaceDirFloat4;
		DirectX::XMStoreFloat4(&rayInWorldSpaceDirFloat4, rayInWorldSpaceDir);

		// Send a ray from the middle of the screen towards the world
		// The search distance is the length the projectile will travel before disappearing
		float searchDist = pc->GetTimeToLive() * m_Velocity;
		double dist = m_pParent->GetComponent<component::CollisionComponent>()->CastRay(double3{
			rayInWorldSpaceDirFloat4.x, 
			rayInWorldSpaceDirFloat4.y, 
			rayInWorldSpaceDirFloat4.z
			}, searchDist);

		// Normalize the camera forward vector
		float vecLen = sqrtf(powf(rayInWorldSpaceDirFloat4.x, 2) + powf(rayInWorldSpaceDirFloat4.y, 2) + powf(rayInWorldSpaceDirFloat4.z, 2));
		float3 rayInWorldSpaceDirNorm = { rayInWorldSpaceDirFloat4.x / vecLen, rayInWorldSpaceDirFloat4.y / vecLen, rayInWorldSpaceDirFloat4.z / vecLen };

		// If it hits something before a certain length, make the projectile move towards that point
		float3 hitDir;
		if (dist != -1)
		{
			hitDir = rayInWorldSpaceDirNorm * dist;
		}
		// Else set the point at the end of the length
		else
		{
			hitDir = rayInWorldSpaceDirNorm * searchDist;
		}

		// Compensate for the low spawnpoint of the projectile compared to the height of the crosshair
		double dirAngle = rayInWorldSpaceDirNorm.y;
		if (dirAngle > -0.1f)
		{
			hitDir.y += 6.0f;
		}
		else
		{
			hitDir.y += 3.0f;
		}

		hitDir.normalize();
		hitDir *= m_Velocity;

		createParticleEffect(ent, hitDir);

		// initialize the components
		mc->SetModel(m_pModel);
		mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW); // hidden because we just want to see the particle effect
		tc->GetTransform()->SetPosition(pos.x, pos.y, pos.z);
		tc->GetTransform()->SetScale(m_Scale);
		tc->GetTransform()->SetVelocity(m_Velocity);
		tc->Update(0.02);
		bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
		bbc->Init();
		bbc->AddCollisionCategory<PlayerProjectileCollisionCategory>();
		if (m_AudioPlay)
		{
			m_pVoiceComponent->Play(L"Fireball");
		}

		component::CollisionComponent* cc = nullptr;
		double3 projectileDim = mc->GetModelDim();
		cc = ent->AddComponent<component::SphereCollisionComponent>(500.0f, projectileDim.z / 2.0f, 1.0f, m_ProjectileRestitution, false);
		cc->SetGravity(0.0f);

		// Light color
		plc->SetColor({ (75.0f * 3.0f) / 255.0f, 0.0f, (130.0f * 3.0f) /255.0f });

		// add the entity to the sceneManager so it can be spawned in in run time
		m_pScene->InitDynamicEntity(ent);
		ent->Update(0);	// Init, so that the light doesn't spawn in origo first frame;
		tc->RenderUpdate(0);
		m_TimeAccumulator = 0.0;
		
		cc->SetVelVector(hitDir.x, hitDir.y, hitDir.z);
		component::CollisionComponent* ccParent = m_pParent->GetComponent<component::CollisionComponent>();
		cc->SetCollidesWith(ccParent, false);

		// Makes player turn in direction of camera to attack
		m_pParent->GetComponent<component::PlayerInputComponent>()->SetAttacking(false);
	}
}

void component::RangeComponent::createParticleEffect(Entity* entity, float3 velocityDir) const
{
	// Create particleEffect
	ParticleEffectSettings settings = {};
	// Important settings
	settings.texture = m_pParticleTexture;
	settings.maxParticleCount = 38;
	settings.spawnInterval = 0.007;
	settings.isLooping = true;

	// Start values
	settings.startValues.position = { 0, 0, 0 };
	settings.startValues.acceleration = { 0, 0, 0 };
	settings.startValues.color = { 1, 1, 1, 1 };
	settings.startValues.lifetime = 0.12;
	settings.startValues.size = 2.5;

	// End values
	settings.endValues.size = 1.4;
	settings.endValues.color = { 1, 1, 1, 0 };

	// Randomize values
	settings.randPosition = { -0.5, 0.5, -0.5, 0.5, -0.5, 0.5 };
	settings.randVelocity = { -6, 6, -6, 6, -6, 6 };
	settings.randSize = { 0, 0 };
	settings.randRotation = { 0, 2 * PI };
	settings.randRotationSpeed = { -PI * 2, PI * 2 };

	// Create the component
	entity->AddComponent<component::ParticleEmitterComponent>(&settings, true);
}
