#include "Precompiled.h"
#include "DemoGame.h"
//=============================================================================
// EXTERNAL DECLARATIONS
//=============================================================================
#include "Engine/Game/GameEngine.h"
#include "Engine/Managers/SpawnManager.h"
#include "Engine/Tasks/SpawnTask.h"
#include "Core/Game/Game.h"
#include "Core/GameManagers/ISpawnManager.h"
#include "GoSkybox.h"
#include "GoTerrain.h"
#include "GoPlayer.h"
#include "GoCow.h"
#include "GoTriggerZone.h"
#include "Engine/Components/InputComponent.h"
#include "Engine/Components/PlayerComponent.h"
#include "PxPhysicsAPI.h"
#include "PlayerBehaviour.h"
#include "Core/GameObjects/GameObjectFactory.h"

#include "Engine/Managers/RenderManager.h"
#include "Engine/Tasks/RenderTask.h"
#include "Engine/Tasks/PlayerTask.h"
#include "Engine/Tasks/InputTask.h"
#include "Engine/Tasks/AITask.h"
#include "Engine/Tasks/PhysicsTask.h"
#include "Engine/Tasks/GameRulesTask.h"
#include "Engine/Tasks/TimeTask.h"

#include "Engine/Components/ComponentFactory.h"
#include "Engine/Components/SimulationComponent.h"
#include "Engine/Components/AIComponent.h"
#include "Engine/Components/FollowPoseComponent.h"
#include "Engine/Components/RenderComponent.h"
#include "Engine/Components/RenderViewComponent.h"

#include "Engine/Managers/WindowManager.h"
#include "Engine/Managers/CameraManager.h"
#include "Engine/Managers/SimulationManager.h"
#include "Engine/Managers/PlayerManager.h"
#include "Engine/Managers/AIManager.h"
#include "Engine/Managers/GameLoopManager.h"
#include "Engine/Managers/TimeManager.h"
#include "Engine/Rendering/Camera.h"

#include "GoCow.h"
#include "GoBall.h"

#include <memory>

using namespace physx;
using namespace engine;

//=============================================================================
// CLASS DemoGame
//=============================================================================
class DemoGame: public GameEngine
{
public:

	enum
	{
		TIMERTASK,
		INPUTTASK,
		PLAYERTASK,
		AITASK,
		PHYSICSTASK,
		GAMERULESTASK,
		RENDERTASK,
		SPAWNTASK,
	};

	DemoGame() = default;

	virtual bool init() override
	{
		GameEngine::init();

		{
			GameManagerFactory& factory = GameManagerFactory::Instance();
			factory.registerType<WindowManager>();
			factory.registerType<RenderManager>();
			factory.registerType<SimulationManager>();
			factory.registerType<SpawnManager>();
			factory.registerType<AIManager>();
			factory.registerType<PlayerManager>();
			factory.registerType<CameraManager>();
			factory.registerType<GameLoopManager>();
			factory.registerType<TimeManager>();
		}

		{
			ComponentFactory& factory = ComponentFactory::Instance();
			factory.registerType<DynamicSimulationComponent>();
			factory.registerType<StaticSimulationComponent>();
			factory.registerType<AIComponent>();
			factory.registerType<FollowPoseComponent>();
			factory.registerType<KeyboardInputComponent>();
			factory.registerType<PlayerComponent>();
			factory.registerType<RenderComponent>();
			factory.registerType<RenderViewComponent>();
		}

		{
			GameObjectFactory::registerGameObjectType(Camera::TypeId, &Camera::createInstance, &Camera::loadData);
			GameObjectFactory::registerGameObjectType(GoBall::TypeId, &GoBall::createInstance, &GoBall::loadData);
			GameObjectFactory::registerGameObjectType(GoCow::TypeId, &GoCow::createInstance, &GoCow::loadData);
			GameObjectFactory::registerGameObjectType(GoPlayer::TypeId, &GoPlayer::createInstance, &GoPlayer::loadData);
			GameObjectFactory::registerGameObjectType(GoSkyBox::TypeId, &GoSkyBox::createInstance, &GoSkyBox::loadData);
			GameObjectFactory::registerGameObjectType(GoTerrain::TypeId, &GoTerrain::createInstance, &GoTerrain::loadData);
			GameObjectFactory::registerGameObjectType(GoTriggerZone::TypeId, &GoTriggerZone::createInstance, &GoTriggerZone::loadData);
		}

		ensureManager<WindowManager>();
		ensureManager<RenderManager>();
		ensureManager<SimulationManager>();
		ensureManager<SpawnManager>();
		ensureManager<AIManager>();
		ensureManager<PlayerManager>();
		ensureManager<CameraManager>();
		ensureManager<GameLoopManager>();
		ensureManager<TimeManager>();

		addTask<SpawnTask>(SPAWNTASK);
		addTask<RenderTask>(RENDERTASK);
		addTask<PlayerTask>(PLAYERTASK);
		addTask<InputTask>(INPUTTASK);
		addTask<AITask>(AITASK);
		addTask<GameRulesTask>(GAMERULESTASK);
		addTask<PhysicsTask>(PHYSICSTASK);
		addTask<TimeTask>(TIMERTASK);

		loadLevel();
		return true;
	}

	virtual bool cleanup() override
	{
		unloadLevel();

		return GameEngine::cleanup();
	}


	~DemoGame() = default;

	void loadLevel();

	void unloadLevel()
	{
		// unspawn every remaining objects
		Game<ISpawnManager>()->unspawnAll();
		Game<ISpawnManager>()->update();
	}
};

GameEngineRef& IGameEngine::Instance()
{
	static GameEngineRef sGame = std::make_shared<DemoGame>();
	return sGame;
}


void DemoGame::loadLevel()
{
	// Skybox
	Game<ISpawnManager>()->spawn<GoSkyBox>(PxTransform(PxVec3(0, 0, 0)));

	// Terrain
	Game<ISpawnManager>()->spawn<GoTerrain>(PxTransform(PxVec3(0, 0, 0)));
	
	// Player
	GameObjectRef player = Game<ISpawnManager>()->spawn<GoPlayer>(PxTransform(PxVec3(0, 0, 0)));
	player->createComponent<KeyboardInputComponent>();
	player->createComponent<PlayerComponent>()->setBehaviour(IBehaviourRef(new PlayerBehaviour));

	// Cow
	Game<ISpawnManager>()->spawn<GoCow>(PxTransform(PxVec3(0, 0, 0)));

	// Trigger zone
	Game<ISpawnManager>()->spawn<GoTriggerZone>(PxTransform(PxVec3(20, 0, 20)));

	// Camera
	Game<ISpawnManager>()->spawn<Camera>(PxTransform(PxVec3(0, 0, 0)));
}

