#ifndef ENEMY_FACTORY_H
#define ENEMY_FACTORY_H

#include "EngineMath.h"
#include "Core.h"
#include "Misc/EngineRand.h"
#include <map>
#include <vector>

class Scene;
class Component;
class Model;
class Entity;

static int s_EnemyId = 0;

enum F_COMP_FLAGS
{
	OBB = BIT(1),
	CUBE_COLLISION = BIT(2),
	SPHERE_COLLISION = BIT(3),
	CAPSULE_COLLISION = BIT(4),
};

// saves the info about a type of enemy
// so that it can be recreated easier
struct EnemyComps
{
	unsigned int compFlags = 0;
	unsigned int aiFlags = 0;
	Model* model = nullptr;
	float3 rot = { 0, 0, 0 };
	float3 pos = { 0, 1, 0 };
	float scale = 1.0;
	int hp = 10;
	std::string targetName = "player";
	int enemiesOfThisType = 0;
	std::wstring sound3D = L"Bruh";
	double3 dim = double3({ 0.0, 0.0, 0.0 });
	float detectionRad = 25.0f;
	float attackingDist = 3.5f;
	float attackInterval = 1.0f;
	float meleeAttackDmg = 10.0f;
	float movementSpeed = 30.0f;
};

class EnemyFactory
{
public:
	EnemyFactory();
	EnemyFactory(Scene* scene);
	~EnemyFactory();

	void SetScene(Scene* scene);

	// Used when you want to add a new type of enemy to the scene .
	// Flag is used for selecting which specific components are needed for this enemy e.g. OBB. 
	// Components that enemies have as default won't have a flag.
	// Will allways draw_opaque and give shadows.
	Entity* AddEnemy(const std::string& entityName, EnemyComps* comps);
	// used if you want to add an already existing enemy type to the scene with only the pos being different
	Entity* AddExistingEnemy(const std::string& entityName, float3 pos);
	// Used if you want to add an already existing enemy but change some of the values; flag, scale, rot or hp.
	// If any of those values are not written in they will use the values stored in the struct of this enemy type.
	// If you want to for example only change the scale then write UINT_MAX as flag, then normal values for scale.
	// To skipp scale and write to rot the equvalent would be FLT_MAX.
	Entity* AddExistingEnemyWithChanges(const std::string& entityName, float3 pos, unsigned int compFlags = UINT_MAX, unsigned int aiFlags = UINT_MAX, float scale = FLT_MAX, float3 rot = { FLT_MAX , FLT_MAX , FLT_MAX }, int hp = INT_MAX);
	// Helper function for adding an enemy
	Entity* Add(const std::string& entityName, EnemyComps* comps);

	std::vector<Entity*>* GetAllEnemies();
	
	// Adds a spawnpoint.
	void AddSpawnPoint(const float3& point);
	// Adds an enemy at the indicated spawnpoint.
	Entity* SpawnEnemy(std::string entityName, unsigned int spawnPoint);
	// Spawns an enemy at random spawnpoint. Assumes that there are atleast two spawnpoints!
	Entity* SpawnEnemy(std::string entityName);
	// Defines an enemy without adding it to the scene.
	EnemyComps* DefineEnemy(const std::string& entityName, EnemyComps* comps);

private:
	Scene* m_pScene = nullptr;
	std::map<std::string, EnemyComps*> m_EnemyComps;
	std::vector<Entity*> m_Enemies;
	std::vector<float3> m_SpawnPoints;
	EngineRand m_RandGen;
};

#endif