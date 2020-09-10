#include "Engine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    /* ------ Engine  ------ */
    Engine engine = Engine();
    engine.Init(hInstance, nCmdShow);

	/*  ------ Get references from engine  ------ */
	Window* const window = engine.GetWindow();
	Timer* const timer = engine.GetTimer();
	ThreadPool* const threadPool = engine.GetThreadPool();
	SceneManager* const sceneManager = engine.GetSceneHandler();
	Renderer* const renderer = engine.GetRenderer();

    /*------ AssetLoader to load models / textures ------*/
    AssetLoader* al = AssetLoader::Get();

    // This will be loaded once from disk, then the next time the same function is called (with the same filepath),
    // the function will just return the same pointer to the model that was loaded earlier.

    std::vector<Mesh*>* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    std::vector<Mesh*>* stoneModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    std::vector<Mesh*>* cubeModel  = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    std::vector<Mesh*>* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    std::vector<Mesh*>* dragonModel = al->LoadModel(L"../Vendor/Resources/Models/Dragon/Dragon 2.5_fbx.fbx");

#pragma region CreateScene0
    // Create Scene
    Scene* scene = sceneManager->CreateScene("scene0");
    
    // Add Entity to Scene
    scene->AddEntity("player");
    scene->AddEntity("floor");
    scene->AddEntity("box");
    scene->AddEntity("stone");
    scene->AddEntity("transparentTestObject");
    scene->AddEntity("Dragon");
    scene->AddEntity("directionalLight");
    scene->AddEntity("spotLight");
    scene->AddEntity("spotLight2");

    // Add Components to Entities
    Entity* entity;

    entity = scene->GetEntity("player");
    component::CameraComponent* cc = entity->AddComponent<component::CameraComponent>(hInstance, *window->GetHwnd(), true);
    entity->AddComponent<component::MeshComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>(true);

    entity = scene->GetEntity("floor");
    entity->AddComponent<component::MeshComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>(false);

    entity = scene->GetEntity("box");
    entity->AddComponent<component::MeshComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>(true);

    entity = scene->GetEntity("stone");
    entity->AddComponent<component::MeshComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>(true);

    entity = scene->GetEntity("transparentTestObject");
    entity->AddComponent<component::MeshComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>(false);

    entity = scene->GetEntity("Dragon");
    entity->AddComponent<component::MeshComponent>();
    entity->AddComponent<component::TransformComponent>();

    entity = scene->GetEntity("directionalLight");
    entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW_ULTRA_RESOLUTION);

    entity = scene->GetEntity("spotLight");
    entity->AddComponent<component::MeshComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>(false);
    entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::CAST_SHADOW_ULTRA_RESOLUTION | FLAG_LIGHT::USE_TRANSFORM_POSITION);

    // Set the m_Components

    component::MeshComponent* mc = scene->GetEntity("player")->GetComponent<component::MeshComponent>();
    mc->SetMeshes(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    mc->GetMesh(0)->GetMaterial()->SetShininess(300);
    component::TransformComponent* tc = scene->GetEntity("player")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(
        cc->GetCamera()->GetPositionFloat3().x + cc->GetCamera()->GetDirection().x,
        cc->GetCamera()->GetPositionFloat3().y + cc->GetCamera()->GetDirection().y - 1,
        cc->GetCamera()->GetPositionFloat3().z + cc->GetCamera()->GetDirection().z
    );
    scene->GetEntity("player")->GetComponent<component::BoundingBoxComponent>()->Init();

    mc = scene->GetEntity("floor")->GetComponent<component::MeshComponent>();
    mc->SetMeshes(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    mc->GetMesh(0)->GetMaterial()->SetShininess(300);
    mc->GetMesh(0)->GetMaterial()->SetUVScale(2.0f, 2.0f);
    tc = scene->GetEntity("floor")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(20, 1, 20);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    scene->GetEntity("floor")->GetComponent<component::BoundingBoxComponent>()->Init();

    mc = scene->GetEntity("box")->GetComponent<component::MeshComponent>();
    mc->SetMeshes(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    tc = scene->GetEntity("box")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(-10.0f, 0.5f, 14.0f);
    scene->GetEntity("box")->GetComponent<component::BoundingBoxComponent>()->Init();

    mc = scene->GetEntity("stone")->GetComponent<component::MeshComponent>();
    mc->SetMeshes(stoneModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    mc->GetMesh(0)->GetMaterial()->SetShininess(300);
    mc->GetMesh(0)->GetMaterial()->SetColorMul(COLOR_TYPE::LIGHT_SPECULAR, { 0.4f, 0.4f, 0.4f, 1.0f });
    tc = scene->GetEntity("stone")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(0.01f);
    tc->GetTransform()->SetPosition(-8.0f, 0.0f, 0.0f);
    scene->GetEntity("stone")->GetComponent<component::BoundingBoxComponent>()->Init();

    mc = scene->GetEntity("transparentTestObject")->GetComponent<component::MeshComponent>();
    mc->SetMeshes(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::Blend);

    mc = scene->GetEntity("Dragon")->GetComponent<component::MeshComponent>();
    mc->SetMeshes(dragonModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    tc = scene->GetEntity("Dragon")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 40.0f);
    tc->GetTransform()->RotateX(3.1415f / 2);

    Texture* ambientDefault = al->LoadTexture(L"../Vendor/Resources/Textures/Default/default_ambient.png");
    Texture* normalDefault  = al->LoadTexture(L"../Vendor/Resources/Textures/Default/default_normal.png");
    mc->GetMesh(0)->GetMaterial()->SetTexture(TEXTURE_TYPE::AMBIENT , ambientDefault);
    mc->GetMesh(0)->GetMaterial()->SetTexture(TEXTURE_TYPE::DIFFUSE , ambientDefault);
    mc->GetMesh(0)->GetMaterial()->SetTexture(TEXTURE_TYPE::SPECULAR, ambientDefault);
    mc->GetMesh(0)->GetMaterial()->SetTexture(TEXTURE_TYPE::NORMAL  , normalDefault);
    
    tc = scene->GetEntity("transparentTestObject")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(5.0f);
    tc->GetTransform()->SetPosition(0.0f, 5.0f, 1.0f);
    tc->GetTransform()->RotateZ(3.141572f / 2.0f);
    tc->GetTransform()->RotateX(3.141572f / 2.0f);

    entity = scene->GetEntity("transparentTestObject");
    entity->GetComponent<component::BoundingBoxComponent>()->Init();
    
    component::DirectionalLightComponent* dl = scene->GetEntity("directionalLight")->GetComponent<component::DirectionalLightComponent>();
    dl->SetDirection({ -1.0f, -1.0f, -1.0f });
    dl->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.05f, 0.05f, 1.0f });
    dl->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.4f, 0.4f, 0.4f, 1.0f });
    dl->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.4f, 0.4f, 0.4f, 1.0f });

    // Spotlight settings
    entity = scene->GetEntity("spotLight");
    mc = entity->GetComponent<component::MeshComponent>();
    mc->SetMeshes(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);

    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(-20.0f, 6.0f, -3.0f);

    entity->GetComponent<component::BoundingBoxComponent>()->Init();

    component::SpotLightComponent* sl = scene->GetEntity("spotLight")->GetComponent<component::SpotLightComponent>();
    sl->SetPosition({ -20.0f, 6.0f, -3.0f });
    sl->SetDirection({ 2.0, -1.0, 0.0f });
    sl->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.00f, 0.05f, 1.0f });
    sl->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 1.0f, 0.00f, 1.0f, 1.0f });
    sl->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 1.0f, 0.00f, 1.0f, 1.0f });

#pragma endregion CreateScene0

	char sceneName[10] = "scene0";
	sceneManager->SetSceneToDraw(sceneManager->GetScene(sceneName));

    while (!window->ExitWindow())
    {
        // ONLY HERE FOR TESTING
		if (window->WasTabPressed())
		{
			// Test to change scene during runtime
			//static int sceneSwapper = 1;
			//sceneSwapper %= 2;
			//sprintf(sceneName, "scene%d", sceneSwapper);
			//Log::Print("Scene: %s\n", sceneName);
			//sceneManager->SetSceneToDraw(sceneManager->GetScene(sceneName));
			//sceneSwapper++;

            // Test to remove picked object
            /*Entity* pickedEnt = renderer->GetPickedEntity();
            if (pickedEnt != nullptr)
            {
				sceneManager->RemoveEntity(pickedEnt);
				scene->RemoveEntity(pickedEnt->GetName());
            }*/


		}
		if (window->WasSpacePressed())
		{
        }

        /* ------ Update ------ */
        timer->Update();
        renderer->Update(timer->GetDeltaTime());

        /* ------ Sort ------ */
        renderer->SortObjectsByDistance();

        /* ------ Draw ------ */
        renderer->Execute();
    }

    return 0;
}
