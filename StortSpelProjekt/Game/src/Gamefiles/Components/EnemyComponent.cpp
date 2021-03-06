#include "EnemyComponent.h"
#include "EnemyFactory.h"

#include "EngineMath.h"

// Engine includes
#include "ECS/SceneManager.h"
#include "ECS/Entity.h"
#include "Misc/AssetLoader.h"
#include "Renderer/Texture/Texture2DGUI.h"
#include "ECS/Components/ParticleEmitterComponent.h"
#include "ECS/Components/TemporaryLifeComponent.h"
#include "../Events/EventBus.h"


constexpr float g_timeToLive = 1.0f;

component::EnemyComponent::EnemyComponent(Entity* parent, EnemyFactory* factory): Component(parent), m_pFactory(factory)
{
}

component::EnemyComponent::~EnemyComponent()
{
}

void component::EnemyComponent::Update(double dt)
{
    // Sound
    m_TimeSinceLastGrunt += dt;
    if (static_cast<double>(m_Rand.Randu(10,100)) < m_TimeSinceLastGrunt)
    {
        m_TimeSinceLastGrunt = 0.0;
        m_pParent->GetComponent<component::Audio3DEmitterComponent>()->UpdateEmitter(L"OnGrunt");
        m_pParent->GetComponent<component::Audio3DEmitterComponent>()->Play(L"OnGrunt");
    }

    // Move ProgressBar with the enemy
    component::ProgressBarComponent* pc = m_pParent->GetComponent<component::ProgressBarComponent>();
    component::TransformComponent* tc = m_pParent->GetComponent<component::TransformComponent>();
    Transform* trans = tc->GetTransform();
    component::ModelComponent* mc = m_pParent->GetComponent<component::ModelComponent>();

    float3 positionAboveHead = trans->GetPositionFloat3();

    positionAboveHead.y = positionAboveHead.y + mc->GetModelDim().y * trans->GetScale().y / 2.0f + 1.0f;

    pc->SetPosition(positionAboveHead);
}

void component::EnemyComponent::OnInitScene()
{
    m_Alive = true;
	m_pFactory->AddEnemyToList(m_pParent);
    
    m_pParent->GetComponent<component::Audio3DEmitterComponent>()->UpdateEmitter(L"Walk");
    m_pParent->GetComponent<component::Audio3DEmitterComponent>()->Play(L"Walk");    
    EventBus::GetInstance().Subscribe(this, &EnemyComponent::death);
    EventBus::GetInstance().Subscribe(this, &EnemyComponent::sceneChange);
}

void component::EnemyComponent::OnUnInitScene()
{
    if (!m_Alive)
    {
        // Create particle effect
        SceneManager& sm = SceneManager::GetInstance();
        Scene* scene = sm.GetActiveScene();
        Entity* entity = nullptr;

        static int counter = 0;
        counter++;
        std::string entityName = "onDeathParticleEffect" + std::to_string(counter);
        entity = scene->AddEntity(entityName);

        std::vector<ParticleEffectSettings> vec;

        static ParticleEffectSettings particleEffectSettings =
        {
            static_cast<Texture2DGUI*>(AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/Particles/fire_particle0.png")),
            35,         // Max particles
            0.000001f,  // spawnInterval
            false,      // IsLooping
            {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, -5.0f, 0.0f}, {1, 1, 1, 1}, 0.7, 0.0f, 0.0f, g_timeToLive}, // start ParticleAttributes
            {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, -5.0f, 0.0f}, {1, 1, 1, 0}, 0.0, 0.0f, 0.0f, g_timeToLive}, // end ParticleAttributes
            {-1, 1, -1, 1, -1, 1},          // RandPosition
            {-20, 20, -20, 20, -20, 20},    // RandVelocity
            {0, 0.6 },                  // RandSize
            {0.0f, 2*PI},                   // RandRotation
            {0.0f, 3.0f},                   // RandRotationSpeed
            {0.0f, 0.0f}                    // RandLifetime
        };


        vec.push_back(particleEffectSettings);

        entity->AddComponent<component::ParticleEmitterComponent>(&vec, true);
        component::TransformComponent* tc = entity->AddComponent<component::TransformComponent>();
        entity->AddComponent<component::TemporaryLifeComponent>(g_timeToLive);

        // Use the enemies current position as a start position for the partile effect
        float3 parentPos = m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
        tc->GetTransform()->SetPosition(parentPos.x, parentPos.y, parentPos.z);

        scene->InitDynamicEntity(entity);
        /* ---------------------- Particle ---------------------- */
    }

    EventBus::GetInstance().Unsubscribe(this, &EnemyComponent::death);
    EventBus::GetInstance().Unsubscribe(this, &EnemyComponent::sceneChange);

    // Remove the enemy
	m_pFactory->RemoveEnemyFromList(m_pParent);
}

void component::EnemyComponent::SetRandSeed(unsigned long seed)
{
    m_Rand.SetSeed(seed);
}

void component::EnemyComponent::death(Death* evnt)
{
    m_Alive = false;
    if (evnt->ent->GetComponent<component::Audio3DEmitterComponent>())
    {
        evnt->ent->GetComponent<component::Audio3DEmitterComponent>()->Stop(L"Walk");
    }
}

void component::EnemyComponent::sceneChange(SceneChange* evnt)
{
    //If the enemy is still present when a scene change then they are likely still alive from a time round or they are dying.
    //Either way by setting alive to true we prevent deathParticleSpawning
    m_Alive = true;
}
