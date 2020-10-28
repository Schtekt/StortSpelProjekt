#include "Engine.h"
#include "EnemyFactory.h"
#include "GameNetwork.h"
#include "Player.h"
#include "Shop.h"

Scene* JacobsTestScene(SceneManager* sm);
Scene* LeosTestScene(SceneManager* sm);
Scene* TimScene(SceneManager* sm);
Scene* JockesTestScene(SceneManager* sm);
Scene* FloppipTestScene(SceneManager* sm);
Scene* FredriksTestScene(SceneManager* sm);
Scene* WilliamsTestScene(SceneManager* sm);
Scene* AndresTestScene(SceneManager* sm);
Scene* AntonTestScene(SceneManager* sm);
Scene* BjornsTestScene(SceneManager* sm);
Scene* ShopScene(SceneManager* sm);

void LeoUpdateScene(SceneManager* sm, double dt);
void TimUpdateScene(SceneManager* sm, double dt);
void JockeUpdateScene(SceneManager* sm, double dt);
void FredriksUpdateScene(SceneManager* sm, double dt);
void AndresUpdateScene(SceneManager* sm, double dt);
void ShopUpdateScene(SceneManager* sm, double dt);

EnemyFactory enemyFactory;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    /*------ Load Option Variables ------*/
    Option* option = &Option::GetInstance();
    option->ReadFile();
    float updateRate = 1.0f / std::atof(option->GetVariable("f_updateRate").c_str());
    float networkUpdateRate = 1.0f / std::atof(option->GetVariable("f_networkUpdateRate").c_str());

    /* ------ Engine  ------ */
    Engine engine;
    engine.Init(hInstance, nCmdShow);

    /*  ------ Get references from engine  ------ */
    Window* const window = engine.GetWindow();
    Timer* const timer = engine.GetTimer();
    ThreadPool* const threadPool = engine.GetThreadPool();
    SceneManager* const sceneManager = engine.GetSceneHandler();
    Renderer* const renderer = engine.GetRenderer();
    Physics* const physics = engine.GetPhysics();
    AudioEngine* const audioEngine = engine.GetAudioEngine();


    /*------ AssetLoader to load models / textures ------*/
    AssetLoader* al = AssetLoader::Get();

    //Scene* jacobScene = JacobsTestScene(sceneManager);
    //Scene* activeScenes[] = { jacobScene };
    //Scene* leoScene = LeosTestScene(sceneManager);
    //Scene* activeScenes[] = { leoScene };
    //Scene* timScene = TimScene(sceneManager);
    //Scene* activeScenes[] = { timScene };
    //Scene* jockeScene = JockesTestScene(sceneManager);
    //Scene* activeScenes[] = { jockeScene };
    //Scene* filipScene = FloppipTestScene(sceneManager);
    //Scene* activeScenes[] = { filipScene };
	//Scene* fredrikScene = FredriksTestScene(sceneManager);
    //Scene* activeScenes[] = { fredrikScene };
    //Scene* williamScene = WilliamsTestScene(sceneManager);
    //Scene* activeScenes[] = { williamScene };
    //Scene* bjornScene = BjornsTestScene(sceneManager);
    //Scene* activeScenes[] = { bjornScene };
    //Scene* antonScene = AntonTestScene(sceneManager);
    //Scene* activeScenes[] = { antonScene };
    //Scene* shopScene = ShopScene(sceneManager);
    //Scene* activeScenes[] = { shopScene };
    Scene* andresScene = AndresTestScene(sceneManager);
    Scene* activeScenes[] = { andresScene };

    // Set scene
    sceneManager->SetScenes(1, activeScenes);

    GameNetwork gameNetwork;

    if (std::atoi(option->GetVariable("i_network").c_str()) == 1)
    {
        gameNetwork.SetScenes(sceneManager->GetActiveScenes());
        gameNetwork.SetSceneManager(sceneManager);
        gameNetwork.SetEnemies(enemyFactory.GetAllEnemies());
    }

    double networkTimer = 0;
    double logicTimer = 0;
    int count = 0;

    while (!window->ExitWindow())
    {
        /* ------ Update ------ */

        timer->Update();
        logicTimer += timer->GetDeltaTime();
        if (gameNetwork.IsConnected())
        {
            networkTimer += timer->GetDeltaTime();
        }

        sceneManager->RenderUpdate(timer->GetDeltaTime());
        if (logicTimer >= updateRate)
        {
            logicTimer = 0;
            sceneManager->Update(updateRate);
            physics->Update(updateRate);
        }

        /* ---- Network ---- */
        if (gameNetwork.IsConnected())
        {
            if (networkTimer >= networkUpdateRate) 
			{
                networkTimer = 0;

                gameNetwork.Update(networkUpdateRate);
            }
        }

        /* ------ Sort ------ */
        renderer->SortObjects();

        /* ------ Draw ------ */
        renderer->Execute();
    }
    sceneManager->ResetScene();
    return 0;
}

Scene* JacobsTestScene(SceneManager* sm)
{
    Scene* scene = sm->CreateScene("JacobsScene");

    AssetLoader* al = AssetLoader::Get();

    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* rockModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
    Model* barbModel = al->LoadModel(L"../Vendor/Resources/Models/Barb/conan_obj.obj");

    AudioBuffer* bruhVoice = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");
    AudioBuffer* projectileSound = al->LoadAudio(L"../Vendor/Resources/Audio/fireball.wav", L"Fireball");
    AudioBuffer* swordSwing = al->LoadAudio(L"../Vendor/Resources/Audio/swing_sword.wav", L"SwordSwing");
    /*--------------------- Assets ---------------------*/

    /*--------------------- Component declarations ---------------------*/
    Entity* entity = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    component::Audio3DListenerComponent* avc2 = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::CameraComponent* cc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PlayerInputComponent* pic = nullptr;
    component::CollisionComponent* bcc = nullptr;
    component::MeleeComponent* melc = nullptr;
    component::RangeComponent* ranc = nullptr;
    component::CurrencyComponent* currc = nullptr;

    scene->CreateNavMesh("Quads");
    NavMesh* nav = scene->GetNavMesh();
    nav->AddNavQuad({ 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f });
    nav->CreateQuadGrid();

    /*--------------------- Player ---------------------*/
    // entity
    entity = scene->AddEntity("player");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    pic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    avc = entity->AddComponent<component::Audio2DVoiceComponent>();
    avc2 = entity->AddComponent<component::Audio3DListenerComponent>();
    bbc = entity->AddComponent<component::BoundingBoxComponent>();
    melc = entity->AddComponent<component::MeleeComponent>();
    ranc = entity->AddComponent<component::RangeComponent>(sm, scene, sphereModel, 0.3, 1, 20);
    currc = entity->AddComponent<component::CurrencyComponent>();

    Transform* t = tc->GetTransform();

    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(-15.0f, 10.0f, 0.0f);

    bcc = entity->AddComponent<component::CubeCollisionComponent>(1.0f, 1.0f, 1.0f, 1.0f, 0.01f, 0.0f, false);
    pic->Init();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);

    avc->AddVoice(L"Bruh");

    enemyFactory.SetScene(scene);

	EnemyComps conan = {};
	conan.model = barbModel;
	conan.hp = 20;
	conan.sound3D = L"Bruh";
	conan.compFlags = F_COMP_FLAGS::OBB | F_COMP_FLAGS::CAPSULE_COLLISION;
	conan.aiFlags = 0;
	conan.meleeAttackDmg = 10.0f;
	conan.attackInterval = 1.0f;
	conan.movementSpeed = 30.0f;
	conan.attackingDist = 0.5f;
	conan.rot = { 0.0, 0.0, 0.0 };
	conan.targetName = "player";
	conan.scale = 0.3;
	conan.detectionRad = 50.0f;

    enemyFactory.AddEnemy("enemyConan", &conan);
    enemyFactory.AddExistingEnemy("enemyConan", float3{ 50.0, 1.0, 0.0 });
    enemyFactory.AddExistingEnemy("enemyConan", float3{ 50.0, 1.0, 10.0 });
    enemyFactory.AddExistingEnemy("enemyConan", float3{ 50.0, 1.0, 20.0 });

    /*--------------------- Box ---------------------*/
    // entity
    entity = scene->AddEntity("Box");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(5.0f, 1.0f, 4.0f);
    bcc = entity->AddComponent<component::CubeCollisionComponent>(1000.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);

    mc->SetModel(cubeModel);

    /*--------------------- Floor ---------------------*/
    // entity
    entity = scene->AddEntity("floor");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 350.0, 0.0, 350.0);


    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(350.0f, 1.0f, 350.0f);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);

    /* ---------------------- PointLight1 ---------------------- */
    entity = scene->AddEntity("pointLight1");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0, 4.0f, 15.0f);

    /*--------------------- DirectionalLight ---------------------*/
    // entity
    entity = scene->AddEntity("sun");

    // components
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
    dlc->SetDirection({ 1.0f, -1.0f, -1.0f });
    dlc->SetColor({ 0.5f, 0.5f, 0.5f });

    return scene;
}

Scene* LeosTestScene(SceneManager* sm)
{
    // Create scene
    Scene* scene = sm->CreateScene("LeoScene");

#pragma region init
    component::Audio2DVoiceComponent* avc = nullptr;
    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::InputComponent* ic = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::CollisionComponent* ccc = nullptr;
    component::Audio3DListenerComponent* avc2 = nullptr;
    component::HealthComponent* hc = nullptr;
    component::MeleeComponent* melc = nullptr;
    component::RangeComponent* ranc = nullptr;
    component::UpgradeComponent* uc = nullptr;

    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Man/man.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
    Model* barbModel = al->LoadModel(L"../Vendor/Resources/Models/Barb/conan_obj.obj");
    AudioBuffer* bruhVoice = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");
    AudioBuffer* projectileSound = al->LoadAudio(L"../Vendor/Resources/Audio/fireball.wav", L"Fireball");
    AudioBuffer* swordSwing = al->LoadAudio(L"../Vendor/Resources/Audio/swing_sword.wav", L"SwordSwing");
    Model* zombieModel = al->LoadModel(L"../Vendor/Resources/Models/Zombie/zombie.obj");

#pragma endregion
 
#pragma region entities

    al->LoadMap(scene, "../Vendor/Resources/aiSceneTriangles.txt");

#pragma region player
    Entity* entity = (scene->AddEntity("player"));
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    avc = entity->AddComponent<component::Audio2DVoiceComponent>();
    avc2 = entity->AddComponent<component::Audio3DListenerComponent>();
    melc = entity->AddComponent<component::MeleeComponent>();
    ranc = entity->AddComponent<component::RangeComponent>(sm, scene, sphereModel, 0.3, 1, 20);
    uc = entity->AddComponent<component::UpgradeComponent>();


    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(-10.0, 20.0, 10.0);

    double3 playerDim = mc->GetModelDim();

    double rad = playerDim.z / 2.0;
    double cylHeight = playerDim.y - (rad * 2.0);
    ccc = entity->AddComponent<component::CapsuleCollisionComponent>(200.0, rad, cylHeight, 0.0, 0.0, false);
    ic->Init();

    Player::GetInstance().SetPlayer(entity);
    Player::GetInstance().GetShop()->RandomizeInventory();
#pragma endregion

#pragma region enemies
	EnemyComps zombie = {};
	zombie.model = zombieModel;
	zombie.hp = 10;
	zombie.sound3D = L"Bruh";
	zombie.compFlags = F_COMP_FLAGS::OBB | F_COMP_FLAGS::CAPSULE_COLLISION;
	zombie.aiFlags = 0;
	zombie.meleeAttackDmg = 10.0f;
	zombie.attackInterval = 1.0f;
	zombie.movementSpeed = 30.0f;
	zombie.attackingDist = 0.5f;
	zombie.rot = { 0.0, 0.0, 0.0 };
	zombie.targetName = "player";
	zombie.scale = 0.04;
	zombie.detectionRad = 50.0f;

    enemyFactory.SetScene(scene);

    enemyFactory.AddSpawnPoint({ -10.0, 10.0, 340.0 });
    enemyFactory.AddSpawnPoint({ -340.0, 10.0, 340.0 });
    enemyFactory.DefineEnemy("enemyZombie", &zombie);

    for (int i = 0; i < 75; i++)
    {
        entity = enemyFactory.SpawnEnemy("enemyZombie");
    }

    Log::Print("Zombie 16 HP: %d\n", scene->GetEntity("enemyZombie16")->GetComponent<component::HealthComponent>()->GetMaxHealth());

    enemyFactory.SetEnemyTypeMaxHealth("enemyZombie", 50);

    Log::Print("Zombie 16 HP: %d\n", scene->GetEntity("enemyZombie16")->GetComponent<component::HealthComponent>()->GetMaxHealth());

#pragma endregion

#pragma endregion

    

    /* ---------------------- Update Function ---------------------- */    
    scene->SetUpdateScene(&LeoUpdateScene);

    srand(time(NULL));

    return scene;
}

Scene* AntonTestScene(SceneManager* sm)
{
    Scene* scene = sm->CreateScene("AntonScene");

    AssetLoader* al = AssetLoader::Get();

    al->LoadMap(scene, "../Vendor/Resources/FirstMap.txt");
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Man/man.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* rockModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");

    AudioBuffer* bruhVoice = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");
    AudioBuffer* projectileSound = al->LoadAudio(L"../Vendor/Resources/Audio/fireball.wav", L"Fireball");
    AudioBuffer* swordSwing = al->LoadAudio(L"../Vendor/Resources/Audio/swing_sword.wav", L"SwordSwing");

    /*--------------------- Assets ---------------------*/

    /*--------------------- Component declarations ---------------------*/
    Entity* entity = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::CameraComponent* cc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PlayerInputComponent* pic = nullptr;
    component::GUI2DComponent* txc = nullptr;
    component::CollisionComponent* ccc = nullptr;
    component::SphereCollisionComponent* scc = nullptr;
    component::MeleeComponent* melc = nullptr;
    component::RangeComponent* ranc = nullptr;
    /*--------------------- Component declarations ---------------------*/

    /*--------------------- Player ---------------------*/
    // entity
    entity = scene->AddEntity("player");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    pic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    avc = entity->AddComponent<component::Audio2DVoiceComponent>();
    bbc = entity->AddComponent<component::BoundingBoxComponent>();
    melc = entity->AddComponent<component::MeleeComponent>();
    ranc = entity->AddComponent<component::RangeComponent>(sm, scene, sphereModel, 0.2, 1, 50);

    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0.0f, 1.0f, 0.0f);

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);

    ccc = entity->AddComponent<component::CubeCollisionComponent>(1, mc->GetModelDim().x / 2.0f, mc->GetModelDim().y / 2.0f, mc->GetModelDim().z / 2.0f, 0, 0, false);
    pic->Init();

    avc->AddVoice(L"Bruh");
    entity = scene->AddEntity("sun");

    // components
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
    dlc->SetDirection({ 0.05f, -0.3f, 0.5f });
    dlc->SetColor({ 252.0f / 256.0f, 156.0f / 256.0f, 84.0f / 256.0f });
    dlc->SetCameraTop(150.0f);
    dlc->SetCameraBot(-100.0f);
    dlc->SetCameraRight(130.0f);
    dlc->SetCameraLeft(-180.0f);
    dlc->SetCameraNearZ(-1000.0f);
    /*--------------------- DirectionalLight ---------------------*/

    scene->SetUpdateScene(&TimUpdateScene);

    return scene;
}

Scene* TimScene(SceneManager* sm)
{
    Scene* scene = sm->CreateScene("TimScene");

    AssetLoader* al = AssetLoader::Get();

    al->LoadMap(scene,"../Vendor/Resources/TestScene.txt" );
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* rockModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    //HeightmapModel* heightMapModel = al->LoadHeightmap(L"../Vendor/Resources/Textures/HeightMaps/hm.hm");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
    
    AudioBuffer* bruhVoice = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");
    
    /*--------------------- Assets ---------------------*/
    
    /*--------------------- Component declarations ---------------------*/
    Entity* entity = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::CameraComponent* cc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PlayerInputComponent* pic = nullptr;
    component::GUI2DComponent* txc = nullptr;
    component::CollisionComponent* ccc = nullptr;
    component::SphereCollisionComponent* scc = nullptr;
    /*--------------------- Component declarations ---------------------*/

    /*--------------------- Player ---------------------*/
    // entity
    entity = scene->AddEntity("player");
    
    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    pic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    avc = entity->AddComponent<component::Audio2DVoiceComponent>();
    
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0.0f, 1.0f, 0.0f);
    
    ccc = entity->AddComponent<component::CubeCollisionComponent>(1,1,1,1,0,0);
    pic->Init();
    
    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);
    
    avc->AddVoice(L"Bruh");
    /*--------------------- Player ---------------------*/
    //
    /*--------------------- Box1 ---------------------*/
    //// entity
    //entity = scene->AddEntity("Box1");
    //
    //// components
    //mc = entity->AddComponent<component::ModelComponent>();
    //tc = entity->AddComponent<component::TransformComponent>();
    //
    //tc->GetTransform()->SetScale(1.0f);
    //tc->GetTransform()->SetPosition(0.0f, 1.0f, 1.0f);
    //
    //mc->SetModel(cubeModel);
    //mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    
    /*--------------------- Box1 ---------------------*/
    //
    ///*--------------------- Box2 ---------------------*/
    //// entity
    //entity = scene->AddEntity("Box2");
    //
    //// components
    //mc = entity->AddComponent<component::ModelComponent>();
    //mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    //tc = entity->AddComponent<component::TransformComponent>();
    //tc->GetTransform()->SetScale(1.0f);
    //tc->GetTransform()->SetPosition(1.0f, 1.0f, 10.0f);
    ////ccc = entity->AddComponent<component::CubeCollisionComponent>(1000.0);
    //
    //mc->SetModel(cubeModel);
    //
    ///*--------------------- Box2 ---------------------*/
    //
    /*--------------------- Floor ---------------------*/
    //// entity
    //entity = scene->AddEntity("floor");
    //
    //// components
    //mc = entity->AddComponent<component::ModelComponent>();
    //tc = entity->AddComponent<component::TransformComponent>();
    //ccc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0, 0.0, 1.0);
    //
    //mc->SetModel(floorModel);
    //mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);
    //tc->GetTransform()->SetScale(35.f, 1.f, 35.f);
    //tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /*--------------------- Floor ---------------------*/
    //
    ///* ---------------------- PointLight1 ---------------------- */
    //entity = scene->AddEntity("pointLight1");
    //mc = entity->AddComponent<component::ModelComponent>();
    //tc = entity->AddComponent<component::TransformComponent>();
    //plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);
    //
    //mc->SetModel(cubeModel);
    //mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    //tc->GetTransform()->SetScale(0.5f);
    //tc->GetTransform()->SetPosition(0, 4.0f, 15.0f);
    ///* ---------------------- PointLight1 ---------------------- */
    //
    ///* ---------------------- PointLight ---------------------- */
    //entity = scene->AddEntity("pointLight");
    //mc = entity->AddComponent<component::ModelComponent>();
    //tc = entity->AddComponent<component::TransformComponent>();
    //plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);
    //
    //mc->SetModel(sphereModel);
    //mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    //tc->GetTransform()->SetScale(0.3f);
    //tc->GetTransform()->SetPosition(0.0f, 4.0f, 0.0f);
    //
    //plc->SetColor({ 2.0f, 0.0f, 2.0f });
    //plc->SetAttenuation({ 1.0, 0.09f, 0.032f });
    ///* ---------------------- PointLight ---------------------- */
    //
    /*--------------------- DirectionalLight ---------------------*/
    // entity
    entity = scene->AddEntity("sun");
    
    // components
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
    dlc->SetDirection({ 1.0f, -1.0f, -1.0f });
    dlc->SetColor({ 0.5f, 0.5f, 0.5f });
    /*--------------------- DirectionalLight ---------------------*/

    scene->SetUpdateScene(&TimUpdateScene);

    return scene;
}

Scene* JockesTestScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("jockesScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::InputComponent* ic = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::SpotLightComponent* slc = nullptr;
    component::CollisionComponent* bcc = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    Model* shopModel = al->LoadModel(L"../Vendor/Resources/Models/Shop/shop.obj");
    Model* posterModel = al->LoadModel(L"../Vendor/Resources/Models/Poster/Poster.obj");

    TextureCubeMap* skyboxCubemap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/skymap.dds");

    scene->CreateNavMesh("Quads");
    NavMesh* nav = scene->GetNavMesh();
    nav->AddNavQuad({ 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f });
    nav->CreateQuadGrid();
    /* ---------------------- Player ---------------------- */
    Entity* entity = (scene->AddEntity("player"));
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    bcc = entity->AddComponent<component::SphereCollisionComponent>(1, 1.5, 0.0);
    scene->SetPrimaryCamera(cc->GetCamera());
    ic->Init();

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);
    /* ---------------------- Player ---------------------- */

    /* ---------------------- Skybox ---------------------- */
    entity = scene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    sbc->SetTexture(skyboxCubemap);
    /* ---------------------- Skybox ---------------------- */

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0, 0.0, 1.0);

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(50, 1, 50);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /* ---------------------- Floor ---------------------- */

    /* ---------------------- Poster ---------------------- */
    entity = scene->AddEntity("poster");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0, 0.0, 1.0);

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(posterModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(2, 1, 2);
    tc->GetTransform()->SetRotationZ(-PI / 2);
    tc->GetTransform()->SetPosition(28.5f, 2.0f, 34.0f);
    /* ---------------------- Poster ---------------------- */

    /* ---------------------- Shop ---------------------- */
    entity = scene->AddEntity("shop");
    mc = entity->AddComponent<component::ModelComponent>();
    mc->SetModel(shopModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);

    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(30.0f, 0.0f, 30.0f);
    tc->GetTransform()->SetRotationY(PI + PI / 4);

    double3 shopDim = mc->GetModelDim();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(10000000.0, shopDim.x / 2.0f, shopDim.y / 2.0f, shopDim.z / 2.0f, 1000.0, 0.0, false);
    /* ---------------------- Shop ---------------------- */

    /* ---------------------- SpotLightDynamic ---------------------- */
    entity = scene->AddEntity("pointLightDynamic");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::CAST_SHADOW | FLAG_LIGHT::STATIC);
    
    float3 pos = { 5.0f, 20.0f, 5.0f };
    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(pos.x, pos.y, pos.z);
    
    slc->SetColor({ 5.0f, 0.0f, 0.0f });
    slc->SetAttenuation({ 1.0, 0.09f, 0.032f });
    slc->SetPosition(pos);
    slc->SetDirection({ 1.0f, -1.0f, 1.0f });
    slc->SetOuterCutOff(50.0f);
    /* ---------------------- SpotLightDynamic ---------------------- */

    /* ---------------------- dirLight ---------------------- */
    entity = scene->AddEntity("dirLight");
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::STATIC | FLAG_LIGHT::CAST_SHADOW);
    dlc->SetColor({ 0.8f, 0.8f, 0.8f });
    dlc->SetDirection({ -2.0f, -1.0f, -1.0f });
    dlc->SetCameraTop(30.0f);
    dlc->SetCameraBot(-30.0f);
    dlc->SetCameraLeft(-70.0f);
    dlc->SetCameraRight(70.0f);
    /* ---------------------- dirLight ---------------------- */

    /* ---------------------- Update Function ---------------------- */
    scene->SetUpdateScene(&JockeUpdateScene);
    return scene;
}

Scene* FloppipTestScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("floppipScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::InputComponent* ic = nullptr;
    component::CollisionComponent* bcc = nullptr;

    Entity* entity = nullptr;

    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");

    scene->CreateNavMesh("Quads");
    NavMesh* nav = scene->GetNavMesh();
    nav->AddNavQuad({ 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f });
    nav->CreateQuadGrid();
    /* ---------------------- Player ---------------------- */
    entity = scene->AddEntity("player");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::SphereCollisionComponent>(1, 1.5, 0.0);
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    ic->Init();

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);
    /* ---------------------- Player ---------------------- */

    /* ---------------------- Skybox ---------------------- */

    // Skybox
    TextureCubeMap* skyboxCubemap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/cubemap.dds");
    entity = scene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    sbc->SetTexture(skyboxCubemap);

    /* ---------------------- Skybox ---------------------- */

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 35.0, 0.0, 35.0);

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 1, 35);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /* ---------------------- Floor ---------------------- */

    
    /* ---------------------- PointLight1 ---------------------- */
    entity = scene->AddEntity("pointLight1");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(-30.0f, 4.0f, 15.0f);

    plc->SetColor({ 2.0f, 0.0f, 2.0f });
    plc->SetAttenuation({ 1.0, 0.09f, 0.032f });

    component::BoundingBoxComponent* bbc = entity->AddComponent<component::BoundingBoxComponent>();
    bbc->Init();

    /* ---------------------- PointLight1 ---------------------- */

    /* ---------------------- The Sun ---------------------- */
    entity = scene->AddEntity("sun");
    component::DirectionalLightComponent* dlc;
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
    
    dlc->SetColor({ 2.0f, 2.0f, 2.0f });

    dlc->SetDirection({ -1.0f, -1.0f, -1.0f });
    /* ---------------------- The Sun ---------------------- */

    return scene;
}

Scene* FredriksTestScene(SceneManager* sm)
{
	// Create Scene
	Scene* scene = sm->CreateScene("FredriksTestScene");

	component::CameraComponent* cc = nullptr;
	component::ModelComponent* mc = nullptr;
	component::TransformComponent* tc = nullptr;
	component::DirectionalLightComponent* dlc = nullptr;
	component::PointLightComponent* plc = nullptr;
	component::SpotLightComponent* slc = nullptr;
	component::InputComponent* ic = nullptr;
	component::Audio3DListenerComponent* audioListener = nullptr;
	component::Audio3DEmitterComponent* audioEmitter = nullptr;
	component::Audio2DVoiceComponent* avc = nullptr;
	component::HealthComponent* hc = nullptr;
	component::BoundingBoxComponent* bbc = nullptr;
	component::CollisionComponent* bcc = nullptr;
	component::RangeComponent* rc = nullptr;
    component::GUI2DComponent* gui = nullptr;

	AssetLoader* al = AssetLoader::Get();

	// Get the models needed
	Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
	Model* enemyModel = al->LoadModel(L"../Vendor/Resources/Models/Barb/conan_obj.obj");
	Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
	Model* stoneModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
	Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
	Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
	Model* posterModel = al->LoadModel(L"../Vendor/Resources/Models/Poster/Poster.obj");
	
	// Get textures
	Texture* buttonTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/greenButton.png");
	Texture* headTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/stefanHuvud.png");
	Texture* mapTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/minimap.png");
	Texture* transTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/transparent.png");

	// Get the audio needed and add settings to it.
	AudioBuffer* melodySound = al->LoadAudio(L"../Vendor/Resources/Audio/melody.wav", L"melody");
	AudioBuffer* bruhSound = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");
	AudioBuffer* horseSound = al->LoadAudio(L"../Vendor/Resources/Audio/AGameWithNoName.wav", L"horse");
	AudioBuffer* attackSound = al->LoadAudio(L"../Vendor/Resources/Audio/attack.wav", L"attack");

	// Audio may loop infinetly (0) once (1) or otherwise specified amount of times!
	bruhSound->SetAudioLoop(0);
	melodySound->SetAudioLoop(0);
	horseSound->SetAudioLoop(0);
	attackSound->SetAudioLoop(1);

    scene->CreateNavMesh("Quads");
    NavMesh* nav = scene->GetNavMesh();
    nav->AddNavQuad({ 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f });
    nav->CreateQuadGrid();
	/* ---------------------- Player ---------------------- */
	Entity* entity = scene->AddEntity("player");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();
	ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
	cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
	bcc = entity->AddComponent<component::CubeCollisionComponent>(1, 1, 1, 1, 0.01);
	audioListener = entity->AddComponent<component::Audio3DListenerComponent>();
	ic->Init();
	hc = entity->AddComponent<component::HealthComponent>(15);
	rc = entity->AddComponent<component::RangeComponent>(sm, scene, sphereModel, 0.3, 1, 10);
	// adding OBB with collision
	bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);

	mc->SetModel(playerModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	tc->GetTransform()->SetScale(1.0f);
	tc->GetTransform()->SetPosition(0, 1, -40);

	// initialize OBB after we have the transform info
	bbc->Init();
	Physics::GetInstance().AddCollisionEntity(entity);

	/* ---------------------- Player ---------------------- */

	/* ---------------------- Floor ---------------------- */
	entity = scene->AddEntity("floor");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();
	bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 60.0, 0.0, 60.0);
	//backgroundAudio = entity->AddComponent<component::Audio2DVoiceComponent>();
	//backgroundAudio->AddVoice(L"bruh");
	//backgroundAudio->Play(L"bruh");

	mc = entity->GetComponent<component::ModelComponent>();
	mc->SetModel(floorModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	tc = entity->GetComponent<component::TransformComponent>();
	tc->GetTransform()->SetScale(60, 1, 60);
	tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
	/* ---------------------- Floor ---------------------- */

	/* ---------------------- dirLight ---------------------- */
	entity = scene->AddEntity("dirLight");
	dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
	dlc->SetColor({ 1.0f, 1.0f, 1.0f });
	dlc->SetDirection({ -1.0f, -1.0f, -1.0f });
	/* ---------------------- dirLight ---------------------- */

	/* ---------------------- Spotlights ---------------------- */
	entity = scene->AddEntity("SpotlightRed");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();
	slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);

	mc->SetModel(sphereModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	tc->GetTransform()->SetScale(0.3f);
	tc->GetTransform()->SetPosition(-50.0f, 3.0f, 50.0f);

	slc->SetColor({ 10.0f, 0.0f, 0.0f });
	slc->SetAttenuation({ 1.0f, 0.14, 0.07f });
	slc->SetDirection({ 1.0, -0.5, -1.0f });

	entity = scene->AddEntity("SpotlightGreen");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();
	slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);

	mc->SetModel(sphereModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	tc->GetTransform()->SetScale(0.3f);
	tc->GetTransform()->SetPosition(50.0f, 3.0f, 50.0f);

	slc->SetColor({ 0.0f, 10.0f, 0.0f });
	slc->SetAttenuation({ 1.0f, 0.14, 0.07f });
	slc->SetDirection({ -1.0, -0.5, -1.0f });

	entity = scene->AddEntity("SpotlightBlue");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();
	slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);

	mc->SetModel(sphereModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	tc->GetTransform()->SetScale(0.3f);
	tc->GetTransform()->SetPosition(50.0f, 3.0f, -50.0f);

	slc->SetColor({ 0.0f, 0.0f, 10.0f });
	slc->SetAttenuation({ 1.0f, 0.14, 0.07f });
	slc->SetDirection({ -1.0, -0.5, 1.0f });

	entity = scene->AddEntity("SpotlightYellow");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();
	slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);

	mc->SetModel(sphereModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	tc->GetTransform()->SetScale(0.3f);
	tc->GetTransform()->SetPosition(-50.0f, 3.0f, -50.0f);

	slc->SetColor({ 10.0f, 10.0f, 0.0f });
	slc->SetAttenuation({ 1.0f, 0.14, 0.07f });
	slc->SetDirection({ 1.0, -0.5, 1.0f });
	/* ---------------------- Spotlights ---------------------- */

	/*--------------------- Sphere1 ---------------------*/
	// entity
	entity = scene->AddEntity("Sphere1");

	// components
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();

	tc->GetTransform()->SetScale(1.0f);
	tc->GetTransform()->SetPosition(1.0f, 1.0f, 1.0f);
	bcc = entity->AddComponent<component::SphereCollisionComponent>(1.0f, 1.5f, 1.0f, 1.0f);

	mc->SetModel(sphereModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
	/*--------------------- Sphere1 ---------------------*/

	/* ---------------------- Stefan ---------------------- */
	entity = scene->AddEntity("stefan");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();

	mc->SetModel(posterModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	tc = entity->GetComponent<component::TransformComponent>();
	tc->GetTransform()->SetScale(50, 50, 1);
	tc->GetTransform()->SetPosition(0.0f, 30.0f, 500.0f);
	tc->GetTransform()->SetRotationX(3 * 3.1415 / 2);
	tc->GetTransform()->SetRotationZ(3 * 3.1415 / 2);
	/* ---------------------- Stefan ---------------------- */

	/* ------------------------- BUTTON 1 --------------------------- */
	std::string textToRender = "TEST";
	float2 textPos = { 0.095f, 0.031f };
	float2 textPadding = { 0.5f, 0.0f };
	float4 textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float2 textScale = { 0.5f, 0.5f };
	float4 textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

    entity = scene->AddEntity("health");
	gui = entity->AddComponent<component::GUI2DComponent>();
	gui->GetTextManager()->AddText("health");
	gui->GetTextManager()->SetColor(textColor, "health");
	gui->GetTextManager()->SetPadding(textPadding, "health");
	gui->GetTextManager()->SetPos(textPos, "health");
	gui->GetTextManager()->SetScale(textScale, "health");
	gui->GetTextManager()->SetText(textToRender, "health");
	gui->GetTextManager()->SetBlend(textBlend, "health");

	float2 quadPos = { 0.0f, 0.0f };
	float2 quadScale = { 0.25f, 0.1f };
	float4 blended = { 1.0, 1.0, 1.0, 0.99 };
	float4 notBlended = { 1.0, 1.0, 1.0, 1.0 };
	gui->GetQuadManager()->CreateQuad(
		"health",
		quadPos, quadScale,
		false, false,
		1,
		notBlended,
		buttonTexture);
	/* ---------------------------------------------------------- */

	/* ------------------------- head --------------------------- */
	entity = scene->AddEntity("head");
	gui = entity->AddComponent<component::GUI2DComponent>();
	quadPos = { 0.009f, 0.014f };
	quadScale = { 0.07f, 0.07f };
	gui->GetQuadManager()->CreateQuad(
		"head",
		quadPos, quadScale,
		true, true,
		2,
		notBlended,
		headTexture);
	/* ---------------------------------------------------------- */

    /* ------------------------- overlay --------------------------- */
    entity = scene->AddEntity("overlay");
    gui = entity->AddComponent<component::GUI2DComponent>();
    quadPos = { 0.0f, 0.0f };
    quadScale = { 0.3f, 0.1f };
    gui->GetQuadManager()->CreateQuad(
		"overlay",
		quadPos, quadScale,
		false, false,
		0, 
		blended,
		transTexture);
    /* ---------------------------------------------------------- */

	/* ------------------------- minimap --------------------------- */
	entity = scene->AddEntity("minimap");
	gui = entity->AddComponent<component::GUI2DComponent>();
	quadPos = { 0.85f, 0.0f };
	quadScale = { 0.15f, 0.15f };
	gui->GetQuadManager()->CreateQuad(
		"minimap",
		quadPos, quadScale,
		false, false,
		2,
		notBlended,
		mapTexture);
	/* ---------------------------------------------------------- */

	/* ---------------------- Skybox ---------------------- */
	TextureCubeMap* skyboxCubeMap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/skymap.dds");
	entity = scene->AddEntity("skybox");
	component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
	/* ---------------------- Skybox ---------------------- */


	/* ---------------------- Update Function ---------------------- */
	scene->SetUpdateScene(&FredriksUpdateScene);
	srand(time(NULL));
	/* ---------------------- Update Function ---------------------- */

    return scene;
}

Scene* WilliamsTestScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("williamScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::CollisionComponent* bcc = nullptr;

    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* dragonModel = al->LoadModel(L"../Vendor/Resources/Models/Dragon/Dragon 2.5_fbx.fbx");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    Model* aniTest = al->LoadModel(L"../Vendor/Resources/Models/aniTest/Standard_Walk.fbx");
    Model* amongUsModel = al->LoadModel(L"../Vendor/Resources/Models/amongus/AmongUs.fbx");

    Entity* entity = scene->AddEntity("player");
    bcc = entity->AddComponent<component::CubeCollisionComponent>(1, 1, 1, 1, 0.1);
    mc = entity->AddComponent<component::ModelComponent>();
    component::PlayerInputComponent* ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    tc = entity->AddComponent<component::TransformComponent>();
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    ic->Init();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);

    scene->CreateNavMesh("Quads");
    NavMesh* nav = scene->GetNavMesh();
    nav->AddNavQuad({ 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f });
    nav->CreateQuadGrid();
    /* ---------------------- Skybox ---------------------- */

    // Skybox
    TextureCubeMap* skyboxCubemap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/skymap.dds");
    entity = scene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    sbc->SetTexture(skyboxCubemap);

    /* ---------------------- Skybox ---------------------- */

    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 35.0, 0.0, 35.0);

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 1, 35);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);


    entity = scene->AddEntity("dragon");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(dragonModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(0.0f, -20.0f, 70.0f);
    tc->GetTransform()->SetRotationX(1.5708);


    entity = scene->AddEntity("amongUs");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(aniTest);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(0.0f, 5.0f, 40.0f);



    entity = scene->AddEntity("pointLight1");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(-30.0f, 4.0f, 15.0f);

    entity = scene->AddEntity("pointLight2");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0.0f, 4.0f, 15.0f);

    plc->SetAttenuation({ 1.0f, 0.045f, 0.0075 });


    entity = scene->AddEntity("pointLight3");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(30.0f, 4.0f, 15.0f);

    entity = scene->AddEntity("pointLight4");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(-30.0f, 4.0f, -15.0f);

    entity = scene->AddEntity("pointLight5");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0.0f, 4.0f, -15.0f);

    plc->SetAttenuation({ 1.0f, 0.045f, 0.0075 });

    entity = scene->AddEntity("pointLight6");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(30.0f, 4.0f, -15.0f);

    return scene;
}

Scene* AndresTestScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("AndresTestScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::SpotLightComponent* slc = nullptr;
    component::InputComponent* ic = nullptr;
    component::Audio3DListenerComponent* audioListener = nullptr;
    component::Audio3DEmitterComponent* audioEmitter = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    component::HealthComponent* hc = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::CollisionComponent* bcc = nullptr;
    component::RangeComponent* rc = nullptr;
    component::UpgradeComponent* upgradeComp = nullptr;
    component::MeleeComponent* melc = nullptr;

    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* enemyModel = al->LoadModel(L"../Vendor/Resources/Models/Barb/conan_obj.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
    Model* posterModel = al->LoadModel(L"../Vendor/Resources/Models/Poster/Poster.obj");

    // audio stuff
    //// Get the audio needed and add settings to it.
    //AudioBuffer* melodySound = al->LoadAudio(L"../Vendor/Resources/Audio/melody.wav", L"melody");
    //AudioBuffer* bruhSound = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");
    //AudioBuffer* horseSound = al->LoadAudio(L"../Vendor/Resources/Audio/AGameWithNoName.wav", L"horse");
    //AudioBuffer* attackSound = al->LoadAudio(L"../Vendor/Resources/Audio/attack.wav", L"attack");
    //// Audio may loop infinetly (0) once (1) or otherwise specified amount of times!
    //bruhSound->SetAudioLoop(0);
    //melodySound->SetAudioLoop(0);
    //horseSound->SetAudioLoop(0);
    //attackSound->SetAudioLoop(1);

    scene->CreateNavMesh("Quads");
    NavMesh* nav = scene->GetNavMesh();
    nav->AddNavQuad({ 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f });
    nav->CreateQuadGrid();
    /* ---------------------- Player ---------------------- */
    Entity* entity = scene->AddEntity("player");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    bcc = entity->AddComponent<component::CubeCollisionComponent>(1, 1, 1, 1, 0.01);
    audioListener = entity->AddComponent<component::Audio3DListenerComponent>();
    ic->Init();
    hc = entity->AddComponent<component::HealthComponent>(100);
    rc = entity->AddComponent<component::RangeComponent>(sm, scene, sphereModel, 0.3, 10, 40);
    // adding OBB with collision
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    melc = entity->AddComponent<component::MeleeComponent>();
    upgradeComp = entity->AddComponent<component::UpgradeComponent>();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -40);
    // initialize OBB after we have the transform info
    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);
    
    // Set the player entity in Player
    Player::GetInstance().SetPlayer(entity);

    /* ---------------------- Player ---------------------- */

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 60.0, 0.0, 60.0);

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(60, 1, 60);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /* ---------------------- Floor ---------------------- */

    /* ---------------------- dirLight ---------------------- */
    entity = scene->AddEntity("dirLight");
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
    dlc->SetColor({ 0.3f, 0.3f, 0.3f });
    dlc->SetDirection({ -0.01f, -1.0f, 0.01f });
    /* ---------------------- dirLight ---------------------- */

    /* ---------------------- Spotlights ---------------------- */
    entity = scene->AddEntity("SpotlightRed");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(-50.0f, 3.0f, 50.0f);

    slc->SetColor({ 10.0f, 0.0f, 0.0f });
    slc->SetAttenuation({ 1.0f, 0.027f, 0.0028f });
    slc->SetDirection({ 1.0, -0.5, -1.0f });

    entity = scene->AddEntity("SpotlightGreen");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(50.0f, 3.0f, 50.0f);

    slc->SetColor({ 0.0f, 10.0f, 0.0f });
    slc->SetAttenuation({ 1.0f, 0.027f, 0.0028f });
    slc->SetDirection({ -1.0, -0.5, -1.0f });

    entity = scene->AddEntity("SpotlightBlue");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(50.0f, 3.0f, -50.0f);

    slc->SetColor({ 0.0f, 0.0f, 10.0f });
    slc->SetAttenuation({ 1.0f, 0.027f, 0.0028f });
    slc->SetDirection({ -1.0, -0.5, 1.0f });

    entity = scene->AddEntity("SpotlightYellow");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(-50.0f, 3.0f, -50.0f);

    slc->SetColor({ 10.0f, 10.0f, 0.0f });
    slc->SetAttenuation({ 1.0f, 0.027f, 0.0028f });
    slc->SetDirection({ 1.0, -0.5, 1.0f });
    /* ---------------------- Spotlights ---------------------- */

    /*--------------------- Sphere1 ---------------------*/
    // entity
    entity = scene->AddEntity("Sphere1");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(1.0f, 1.0f, 1.0f);
    bcc = entity->AddComponent<component::SphereCollisionComponent>(1.0f, 1.5f, 1.0f, 1.0f);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    /*--------------------- Sphere1 ---------------------*/

    /* ---------------------- Stefan ---------------------- */
    entity = scene->AddEntity("stefan");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc->SetModel(posterModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(50, 1, 50);
    tc->GetTransform()->SetPosition(0.0f, 30.0f, 500.0f);
    tc->GetTransform()->SetRotationX(3 * 3.1415 / 2);
    tc->GetTransform()->SetRotationZ(3 * 3.1415 / 2);
    /* ---------------------- Stefan ---------------------- */


    /* ---------------------- Enemy -------------------------------- */
    // Enemyfactory used in the wrong way. 
    //EnemyFactory enH(scene);
    //entity = enH.AddEnemy("enemy", enemyModel, 1000, float3{ 0, 10, 20 }, L"Bruh", F_COMP_FLAGS::OBB | F_COMP_FLAGS::CAPSULE_COLLISION, 0, 0.5, float3{ 0, 0, 0 }, "player", 25.0f, 7.0f, 0.5f, 10.0f);
    /* ---------------------- Enemy -------------------------------- */


    /* ---------------------- Skybox ---------------------- */
    TextureCubeMap* skyboxCubeMap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/skymap.dds");
    entity = scene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    /* ---------------------- Skybox ---------------------- */


    /* ---------------------- Update Function ---------------------- */
    scene->SetUpdateScene(&AndresUpdateScene);
    srand(time(NULL));
    /* ---------------------- Update Function ---------------------- */

    return scene;
}

Scene* BjornsTestScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("bjornScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::SpotLightComponent* slc = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    component::HealthComponent* hc = nullptr;
    component::RangeComponent* rc = nullptr;
    component::CollisionComponent* bcc = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* dragonModel = al->LoadModel(L"../Vendor/Resources/Models/Dragon/Dragon 2.5_fbx.fbx");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    Model* stoneModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");

    // Get the audio needed and add settings to it.
    AudioBuffer* bruhSound = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");

    // Audio may loop infinetly (0) once (1) or otherwise specified amount of times!
    bruhSound->SetAudioLoop(1);

    scene->CreateNavMesh("Quads");
    NavMesh* nav = scene->GetNavMesh();
    nav->AddNavQuad({ 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f });
    nav->CreateQuadGrid();

    Entity* entity = scene->AddEntity("player");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(1, 1, 1, 1, 0.1);
    component::PlayerInputComponent* ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    ic->Init();
    hc = entity->AddComponent<component::HealthComponent>(15);
    // adding OBB with collision
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    avc = entity->AddComponent<component::Audio2DVoiceComponent>();
    avc->AddVoice(L"Bruh");
    rc = entity->AddComponent<component::RangeComponent>(sm, scene, sphereModel, 0.3);

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);
    // initialize OBB after we have the transform info
    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);


    entity = scene->AddEntity("floor");
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 35.0, 0.0, 35.0);
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 1, 35);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);


    entity = scene->AddEntity("dragon");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(dragonModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    bbc->Init();
    tc->GetTransform()->SetPosition(0.0f, -20.0f, 70.0f);
    tc->GetTransform()->SetRotationX(1.5708);

    Physics::GetInstance().AddCollisionEntity(entity);

    //entity = scene->AddEntity("stone");
    //mc = entity->AddComponent<component::ModelComponent>();
    //tc = entity->AddComponent<component::TransformComponent>();
    //bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);

    //mc = entity->GetComponent<component::ModelComponent>();
    //mc->SetModel(stoneModel);
    //mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    //tc = entity->GetComponent<component::TransformComponent>();
    //tc->GetTransform()->SetScale(0.01f);
    //tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    //bbc->Init();
    //Physics::GetInstance().AddCollisionEntity(entity);

    // Adding enemy example
    EnemyFactory enH(scene);
    //enH.AddEnemy("rock", stoneModel, 5, float3{ 1, 0, 1 }, F_COMP_FLAGS::OBB, 0.01, float3{ 1.578, 0, 0 });
    // showing that using the wrong overload will send Warning to Log. 
    // and then automaticly use the correct overloaded function 
   // enH.AddEnemy("rock", stoneModel, float3{ -10, 0, -10 }, F_COMP_FLAGS::OBB, 0.01);

    // adding an already existing enemy type but changing the scale of it
    //enH.AddExistingEnemyWithChanges("rock", float3{ 20, 0, 4 }, UINT_MAX, 0.005f);

    // looping through and adding already existing enemy type with only new position
    float xVal = 8;
    float zVal = 0;
    for (int i = 0; i < 50; i++)
    {
        zVal += 8;
        enH.AddExistingEnemy("enemy", float3{ xVal, 0, zVal });
        if ((i + 1) % 5 == 0)
        {
            xVal += 8;
            zVal = 0;
        }
    }

    /* ---------------------- PointLight ---------------------- */
    entity = scene->AddEntity("pointLight");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(0.0f, 4.0f, 0.0f);

    plc->SetColor({ 2.0f, 0.0f, 2.0f });
    plc->SetAttenuation({ 1.0, 0.09f, 0.032f });
    /* ---------------------- PointLight ---------------------- */

    /* ---------------------- dirLight ---------------------- */
    entity = scene->AddEntity("dirLight");
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
    dlc->SetColor({ 1.0f, 1.0f, 1.0f });
    dlc->SetDirection({ -1.0f, -1.0f, -1.0f });
    /* ---------------------- dirLight ---------------------- */

    /* ---------------------- Spotlight ---------------------- */
    entity = scene->AddEntity("Spotlight");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(30.0f, 4.0f, 10.0f);

    slc->SetColor({ 0.0f, 0.0f, 4.0f });
    slc->SetAttenuation({ 1.0f, 0.027f, 0.0028f });
    slc->SetDirection({ -2.0, -1.0, 0.0f });
    /* ---------------------- Spotlight ---------------------- */
    return scene;
}

Scene* ShopScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("shopScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::InputComponent* ic = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::SpotLightComponent* slc = nullptr;
    component::CollisionComponent* bcc = nullptr;
    component::MeleeComponent* mac = nullptr;
    component::RangeComponent* rc = nullptr;
    component::UpgradeComponent* uc = nullptr;
    component::CapsuleCollisionComponent* ccc = nullptr;
    component::HealthComponent* hc = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Man/man.obj");
    Model* shopModel = al->LoadModel(L"../Vendor/Resources/Models/Shop/shop.obj");
    Model* posterModel = al->LoadModel(L"../Vendor/Resources/Models/Poster/Poster.obj");
    Model* fenceModel = al->LoadModel(L"../Vendor/Resources/Models/FencePBR/fence.obj");

    TextureCubeMap* skyboxCubemap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/skymap.dds");

    scene->CreateNavMesh("Quads");
    NavMesh* nav = scene->GetNavMesh();
    nav->AddNavQuad({ 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f });
    nav->CreateQuadGrid();

#pragma region player
    Entity* entity = (scene->AddEntity("player"));
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    mac = entity->AddComponent<component::MeleeComponent>();
    rc = entity->AddComponent<component::RangeComponent>(sm, scene, sphereModel, 0.3, 1, 20);
    uc = entity->AddComponent<component::UpgradeComponent>();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0.0, 20.0, 0.0);

    double3 playerDim = mc->GetModelDim();

    double rad = playerDim.z / 2.0;
    double cylHeight = playerDim.y - (rad * 2.0);
    ccc = entity->AddComponent<component::CapsuleCollisionComponent>(200.0, rad, cylHeight, 0.0, 0.0, false);
    hc = entity->AddComponent<component::HealthComponent>(10000000);
    ic->Init();
    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);

    Player::GetInstance().SetPlayer(entity);
    Player::GetInstance().GetShop()->RandomizeInventory();
#pragma endregion player
    /* ---------------------- Skybox ---------------------- */
    entity = scene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    sbc->SetTexture(skyboxCubemap);
    /* ---------------------- Skybox ---------------------- */

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0, 0.0, 1.0);

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(50, 1, 50);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /* ---------------------- Floor ---------------------- */

    /* ---------------------- Poster ---------------------- */
    entity = scene->AddEntity("poster");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(posterModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(1, 1, 1);
    tc->GetTransform()->SetRotationZ(-PI / 2);
    tc->GetTransform()->SetPosition(27.8f, 1.0f, 34.0f);
    /* ---------------------- Poster ---------------------- */

    /* ---------------------- Shop ---------------------- */
    entity = scene->AddEntity("shop");
    mc = entity->AddComponent<component::ModelComponent>();
    mc->SetModel(shopModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);

    double3 shopDim = mc->GetModelDim();
    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(30.0f, shopDim.y/2, 30.0f);
    tc->GetTransform()->SetRotationY(PI + PI / 4);

    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, shopDim.x / 2.0f, shopDim.y / 2.0f, shopDim.z / 2.0f, 1000.0, 0.0, false);
    /* ---------------------- Shop ---------------------- */
#pragma region walls
    // Left wall
    entity = scene->AddEntity("wallLeft");
    mc = entity->AddComponent<component::ModelComponent>();
    mc->SetModel(fenceModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_TRANSPARENT_TEXTURE | FLAG_DRAW::NO_DEPTH | FLAG_DRAW::GIVE_SHADOW);
    

    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(-50.0f, 10.0f, 0.0f);
    tc->GetTransform()->SetScale(10, 1, 50);
    tc->GetTransform()->SetRotationZ(-PI/2);
    tc->GetTransform()->SetRotationY(PI);
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0f, 0.0f, 1.0f);

    // Right wall
    entity = scene->AddEntity("wallRight");
    mc = entity->AddComponent<component::ModelComponent>();
    mc->SetModel(fenceModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_TRANSPARENT_TEXTURE | FLAG_DRAW::NO_DEPTH);


    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(50.0f, 10.0f, 0.0f);
    tc->GetTransform()->SetScale(10, 1, 50);
    tc->GetTransform()->SetRotationZ(-PI / 2);
    tc->GetTransform()->SetRotationY(PI);
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0f, 0.0f, 1.0f);

    // Top Wall
    entity = scene->AddEntity("wallTop");
    mc = entity->AddComponent<component::ModelComponent>();
    mc->SetModel(fenceModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_TRANSPARENT_TEXTURE | FLAG_DRAW::NO_DEPTH);


    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(0.0f, 10.0f, 50.0f);
    tc->GetTransform()->SetScale(10, 1, 50);
    tc->GetTransform()->SetRotationZ(PI / 2);
    tc->GetTransform()->SetRotationX(PI / 2);
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0f, 0.0f, 1.0f);

    // Bot Wall
    entity = scene->AddEntity("wallBot");
    mc = entity->AddComponent<component::ModelComponent>();
    mc->SetModel(fenceModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_TRANSPARENT_TEXTURE | FLAG_DRAW::NO_DEPTH | FLAG_DRAW::GIVE_SHADOW);


    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(0.0f, 10.0f, -50.0f);
    tc->GetTransform()->SetScale(10, 1, 50);
    tc->GetTransform()->SetRotationZ(PI / 2);
    tc->GetTransform()->SetRotationX(PI / 2);
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0f, 0.0f, 1.0f);
    
#pragma endregion walls
    /* ---------------------- SpotLightDynamic ---------------------- */
    entity = scene->AddEntity("spotLightDynamic");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::CAST_SHADOW | FLAG_LIGHT::STATIC);

    float3 pos = { 5.0f, 20.0f, 5.0f };
    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(pos.x, pos.y, pos.z);

    slc->SetColor({ 5.0f, 0.0f, 0.0f });
    slc->SetAttenuation({ 1.0, 0.09f, 0.032f });
    slc->SetPosition(pos);
    slc->SetDirection({ 1.0f, -1.0f, 1.0f });
    slc->SetOuterCutOff(50.0f);
    /* ---------------------- SpotLightDynamic ---------------------- */

    /* ---------------------- dirLight ---------------------- */
    entity = scene->AddEntity("dirLight");
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::STATIC | FLAG_LIGHT::CAST_SHADOW);
    dlc->SetColor({ 0.8f, 0.8f, 0.8f });
    dlc->SetDirection({ -2.0f, -1.0f, -1.0f });
    dlc->SetCameraTop(50.0f);
    dlc->SetCameraBot(-30.0f);
    dlc->SetCameraLeft(-70.0f);
    dlc->SetCameraRight(70.0f);
    /* ---------------------- dirLight ---------------------- */

    /* ---------------------- Update Function ---------------------- */
    scene->SetUpdateScene(&ShopUpdateScene);
    return scene;
}

void LeoUpdateScene(SceneManager* sm, double dt)
{
}

void TimUpdateScene(SceneManager* sm, double dt)
{
    if (ImGuiHandler::GetInstance().GetBool("reset"))
    {
        ImGuiHandler::GetInstance().SetBool("reset", false);

        sm->GetScene("TimScene")->GetEntity("player")->GetComponent<component::CollisionComponent>()->SetVelVector(0, 0, 0);
        sm->GetScene("TimScene")->GetEntity("player")->GetComponent<component::CollisionComponent>()->SetAngularVelocity(0, 0, 0);
        sm->GetScene("TimScene")->GetEntity("player")->GetComponent<component::CollisionComponent>()->SetRotation(0, 0, 0);
        sm->GetScene("TimScene")->GetEntity("player")->GetComponent<component::CollisionComponent>()->SetPosition(0, 10, 0);

        sm->GetScene("TimScene")->GetEntity("Box1")->GetComponent<component::CollisionComponent>()->SetVelVector(0, 0, 0);
        sm->GetScene("TimScene")->GetEntity("Box1")->GetComponent<component::CollisionComponent>()->SetAngularVelocity(0, 0, 0);
        sm->GetScene("TimScene")->GetEntity("Box1")->GetComponent<component::CollisionComponent>()->SetRotation(0, 0, 0);
        sm->GetScene("TimScene")->GetEntity("Box1")->GetComponent<component::CollisionComponent>()->SetPosition(1, 1, 1);

        sm->GetScene("TimScene")->GetEntity("Box2")->GetComponent<component::CollisionComponent>()->SetAngularVelocity(0, 3.14, 0);
        sm->GetScene("TimScene")->GetEntity("Box2")->GetComponent<component::CollisionComponent>()->SetFriction(0);

    }
}

void JockeUpdateScene(SceneManager* sm, double dt)
{
    //static float intensity = 0.0f;
    //component::PointLightComponent* plc = sm->GetScene("jockesScene")->GetEntity("pointLightDynamic")->GetComponent<component::PointLightComponent>();
    //float col = abs(sinf(intensity)) * 10;
    //plc->SetColor({ col, col, 0.0f });
    //
    //intensity += 0.005f;
}

void FredriksUpdateScene(SceneManager* sm, double dt)
{
	/*AssetLoader* al = AssetLoader::Get();
	component::HealthComponent* hc = sm->GetScene("FredriksTestScene")->GetEntity("player")->GetComponent<component::HealthComponent>();
	tx->GetTextManager()->SetText("HP: " + std::to_string(hc->GetHealth()), "health");
	tx->GetTextManager()->UploadAndExecuteTextData("health");*/
}

void AndresUpdateScene(SceneManager* sm, double dt)
{
    //component::Audio3DEmitterComponent* ec = sm->GetScene("AndresTestScene")->GetEntity("enemy")->GetComponent<component::Audio3DEmitterComponent>();
    //ec->UpdateEmitter(L"Bruh");
}

void ShopUpdateScene(SceneManager* sm, double dt)
{
    static float rotValue = 0.0f;
    Transform* trans = sm->GetScene("shopScene")->GetEntity("poster")->GetComponent<component::TransformComponent>()->GetTransform();
    trans->SetRotationX(rotValue);
    
    rotValue += 0.005f;
}
