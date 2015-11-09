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

#include "Engine/Managers/RenderManager.h"
#include "Engine/Tasks/RenderTask.h"
#include "Engine/Tasks/PlayerTask.h"
#include "Engine/Tasks/InputTask.h"
#include "Engine/Tasks/AITask.h"
#include "Engine/Tasks/PhysicsTask.h"
#include "Engine/Tasks/GameRulesTask.h"
#include "Engine/Tasks/TimeTask.h"

#include "Engine/Managers/WindowManager.h"
#include "Engine/Managers/CameraManager.h"
#include "Engine/Managers/SimulationManager.h"
#include "Engine/Managers/PlayerManager.h"
#include "Engine/Managers/AIManager.h"
#include "Engine/Managers/GameLoopManager.h"
#include "Engine/Managers/TimeManager.h"
#include "Engine/Rendering/Camera.h"

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

		createComponent<WindowManager>();
		createComponent<RenderManager>();
		createComponent<SimulationManager>();
		createComponent<SpawnManager>();
		createComponent<AIManager>();
		createComponent<PlayerManager>();
		createComponent<CameraManager>();
		createComponent<GameLoopManager>();
		createComponent<TimeManager>();

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
	Game<ISpawnManager>()->spawn<ICamera>(PxTransform(PxVec3(0, 0, 0)));
}

