#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Scene.h"
#include "../Renderer/MousePicker.h"
#include <set>
#include <unordered_map>

class Entity;
class Renderer;
class AudioEngine;

// Event
struct Death;
struct RemoveMe;
struct SceneChange;

class SceneManager 
{
public:
	static SceneManager& GetInstance();
	~SceneManager();
	void EraseSceneManager();
	// Update
	void Update(double dt);
	void RenderUpdate(double dt);

	// Scene
	Scene* CreateScene(std::string sceneName);
	void SetScenes(unsigned int numScenes, Scene** scene);
	std::vector<Scene*>* GetActiveScenes();
	Scene* GetScene(std::string sceneName) const;

	void ChangeSceneIfTeleported();
	void ChangeSceneIfPlayerDied();

	void ResetScene();

	// Special scenes
	void SetGameOverScene(Scene* scene);

	// Entity
	void RemoveEntity(Entity* entity, Scene* scene);
	void AddEntity(Entity* entity, Scene* scene);

private:
	SceneManager();
	std::map<std::string, Scene*> m_Scenes;
	std::vector<Scene*> m_ActiveScenes;

	struct EntityScene
	{
		Entity* ent;
		Scene* scene;
	};
	std::vector<EntityScene> m_ToRemove;

	Scene* m_pGameOverScene = nullptr;

	bool sceneExists(std::string sceneName) const;

	// Entity events
	bool m_ChangeSceneNextFrameToDeathScene = false;
	void onEntityDeath(Death* evnt);

	void onEntityRemove(RemoveMe* evnt);

	std::string m_SceneToChangeToWhenTeleported = "";
	bool m_ChangeSceneNextFrame = false;
	void changeSceneNextFrame(SceneChange* sceneChangeEvent);
};

#endif
