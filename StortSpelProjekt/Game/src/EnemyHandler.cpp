#include "EnemyHandler.h"
#include "ECS/Scene.h"
#include "Engine.h"

EnemyHandler::EnemyHandler(Scene* scene)
{
	m_pScene = scene;
}

EnemyHandler::~EnemyHandler()
{
	for (auto pair : m_enemyComps)
	{
		if (pair.second != nullptr)
		{
			delete pair.second;
		}
	}
	m_enemyComps.clear();
}

Entity* EnemyHandler::AddEnemy(std::string entityName, Model* model, float3 pos, unsigned int flag, float scale, float3 rot)
{
	for (auto pair : m_enemyComps)
	{
		// An entity with this m_Name already exists
		// so make use of the overloaded version of this function
		if (pair.first == entityName)
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Enemy of this type \"%s\" already exists! Overloaded funtion will be used instead!\n", entityName.c_str());
			return AddEnemy(pos, entityName, flag, scale, rot);
		}
	}
	Entity *ent = m_pScene->AddEntity(entityName);
	component::ModelComponent* mc = nullptr;
	component::TransformComponent* tc = nullptr;
	component::BoundingBoxComponent* bbc = nullptr;
	// TODO: Add more components when they are made such as HealthComponent
	m_enemyComps[entityName] = new EnemyComps;

	m_enemyComps[entityName]->enemiesOfThisType++;
	m_enemyComps[entityName]->compFlags = flag;
	m_enemyComps[entityName]->pos = pos;
	m_enemyComps[entityName]->scale = scale;
	m_enemyComps[entityName]->rot = rot;
	m_enemyComps[entityName]->model = model;

	mc = ent->AddComponent<component::ModelComponent>();
	tc = ent->AddComponent<component::TransformComponent>();	

	mc->SetModel(m_enemyComps[entityName]->model/*model*/);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	tc->GetTransform()->SetPosition(pos.x, pos.y, pos.z);
	tc->GetTransform()->SetScale(scale);
	tc->GetTransform()->SetRotationX(rot.x);
	tc->GetTransform()->SetRotationY(rot.y);
	tc->GetTransform()->SetRotationZ(rot.z);
	if (F_COMP_FLAGS::OBB & flag)
	{
		bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
		bbc->Init();
		//m_enemyComps[entityName]->s_Components.push_back("");
		Physics::GetInstance().AddCollisionEntity(ent);
	}
	return ent;
}

Entity* EnemyHandler::AddEnemy(std::string entityName, float3 pos)
{
	for (auto pair : m_enemyComps)
	{
		// An entity with this m_Name already exists
		// so create a new onen of the same type
		if (pair.first == entityName)
		{
			std::string name = entityName + std::to_string(m_enemyComps[entityName]->enemiesOfThisType);
			Entity* ent = m_pScene->AddEntity(name);
			m_enemyComps[entityName]->enemiesOfThisType++;
			component::ModelComponent* mc = nullptr;
			component::TransformComponent* tc = nullptr;
			component::BoundingBoxComponent* bbc = nullptr;
			// TODO: Add more components as they are made such as HealthComponent
			
			mc = ent->AddComponent<component::ModelComponent>();
			tc = ent->AddComponent<component::TransformComponent>();

			mc->SetModel(m_enemyComps[entityName]->model);
			mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
			tc->GetTransform()->SetPosition(pos.x, pos.y , pos.z);
			tc->GetTransform()->SetScale(m_enemyComps[entityName]->scale);
			tc->GetTransform()->SetRotationX(m_enemyComps[entityName]->rot.x);
			tc->GetTransform()->SetRotationY(m_enemyComps[entityName]->rot.y);
			tc->GetTransform()->SetRotationZ(m_enemyComps[entityName]->rot.z);

			if (F_COMP_FLAGS::OBB & m_enemyComps[entityName]->compFlags)
			{
				bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
				bbc->Init();
				Physics::GetInstance().AddCollisionEntity(ent);
			}
			return ent;
		}
		else
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Inssuficient input in parameters to add new type of enemy!\n");
			return nullptr;
		}
	}
}

Entity* EnemyHandler::AddEnemy(float3 pos, std::string entityName, unsigned int flag, float scale, float3 rot)
{
	for (auto pair : m_enemyComps)
	{
		// An entity with this m_Name already exists
		// so create a new onen of the same type
		if (pair.first == entityName)
		{
			// if any of the inputs are not default values use them
			// otherwise use the values from the struct
			unsigned int newFlag;
			if (flag != UINT_MAX)
			{
				newFlag = flag;
			}
			else
			{
				newFlag = m_enemyComps[entityName]->compFlags;
			}
			float newScale;
			if (scale != FLT_MAX)
			{
				newScale = scale;
			}
			else
			{
				newScale = m_enemyComps[entityName]->scale;
			}
			float3 newRot;
			if (rot.x != FLT_MAX)
			{
				newRot = rot;
			}
			else
			{
				newRot = m_enemyComps[entityName]->rot;
			}

			std::string name = entityName + std::to_string(m_enemyComps[entityName]->enemiesOfThisType);
			Entity* ent = m_pScene->AddEntity(name);
			m_enemyComps[entityName]->enemiesOfThisType++;
			component::ModelComponent* mc = nullptr;
			component::TransformComponent* tc = nullptr;
			component::BoundingBoxComponent* bbc = nullptr;
			// TODO: Add more components as they are made such as HealthComponent

			mc = ent->AddComponent<component::ModelComponent>();
			tc = ent->AddComponent<component::TransformComponent>();

			mc->SetModel(m_enemyComps[entityName]->model);
			mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
			tc->GetTransform()->SetPosition(pos.x, pos.y, pos.z);
			tc->GetTransform()->SetScale(newScale);
			tc->GetTransform()->SetRotationX(newRot.x);
			tc->GetTransform()->SetRotationY(newRot.y);
			tc->GetTransform()->SetRotationZ(newRot.z);

			if (F_COMP_FLAGS::OBB & newFlag)
			{
				bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
				bbc->Init();
				Physics::GetInstance().AddCollisionEntity(ent);
			}
			return ent;
		}
		else
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Inssuficient input in parameters to add new type of enemy!\n");
			return nullptr;
		}
	}
}

