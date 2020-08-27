#ifndef SCENEHANDLER_H
#define SCENEHANDLER_H

#include "Scene.h"
#include "..\Renderer\Renderer.h"

class SceneHandler 
{
public:
	SceneHandler(Renderer *r);
	~SceneHandler();

	Scene* CreateScene(std::string sceneName);

	Scene* GetScene(std::string sceneName) const;
	
	// Input is either an Entity or a Scene
	template<class T>
	void ManageScene(T* input);
private:
	Renderer* renderer;

	std::map<std::string, Scene*> scenes;

	bool SceneExists(std::string sceneName) const;
	void HandleSceneComponents(Scene* scene);
	void ManageComponent(Entity* entity);
	void ResetScene();
};

template<class T>
void SceneHandler::ManageScene(T* input)
{
	Scene* scene = dynamic_cast<Scene*>(input);
	
	if (scene != nullptr)
	{
		Log::Print("New Scene \n");

		ResetScene();
		HandleSceneComponents(scene);

		this->renderer->ConstantBufferPreparation();

		// -------------------- DEBUG STUFF --------------------
		// Test to change camera to the shadow casting lights cameras
		//auto& tuple = this->renderer->lights[LIGHT_TYPE::SPOT_LIGHT].at(0);
		//BaseCamera* tempCam = std::get<0>(tuple)->GetCamera();
		//this->renderer->ScenePrimaryCamera = tempCam;
		if (this->renderer->ScenePrimaryCamera == nullptr)
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "No primary camera was set in scene: %s\n", scene->GetName());
		}
		this->renderer->mousePicker->SetPrimaryCamera(renderer->ScenePrimaryCamera);
		scene->SetPrimaryCamera(renderer->ScenePrimaryCamera);
		this->renderer->SetRenderTasksRenderComponents();
		this->renderer->SetRenderTasksPrimaryCamera();

		this->renderer->currActiveScene = scene;

		return;
	}

	Entity* entity = dynamic_cast<Entity*>(input);

	if (entity != nullptr)
	{
		Log::Print("New Entity \n");
		ManageComponent(entity);
		return;
	}
	
	Log::PrintSeverity(Log::Severity::CRITICAL, "The pointer sent to ManageScene is neither an Entity or a Scene!\n");
}

#endif
