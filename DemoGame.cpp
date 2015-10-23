#include "Precompiled.h"
#include "DemoGame.h"
//=============================================================================
// EXTERNAL DECLARATIONS
//=============================================================================
#include "Engine/Game/GameEngine.h"
#include "Engine/Managers/SpawnManager.h"
#include "Engine/Tasks/SpawnTask.h"
#include "Core/Game/Game.h"
#include "Core/GameManagers/IGameSpawner.h"
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

using namespace physx;
//=============================================================================
// CLASS DemoGame
//=============================================================================
class DemoGame: public Engine
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


	DemoGame()
	{
	}

	virtual bool init() override
	{
		Engine::init();

		addComponent<WindowManager>();
		addComponent<RenderManager>();
		addComponent<SimulationManager>();
		addComponent<SpawnManager>();
		addComponent<AIManager>();
		addComponent<PlayerManager>();
		addComponent<CameraManager>();
		addComponent<GameLoopManager>();
		addComponent<TimeManager>();

		addTask(new SpawnTask, SPAWNTASK);
		addTask(new RenderTask, RENDERTASK);
		addTask(new PlayerTask, PLAYERTASK);
		addTask(new InputTask, INPUTTASK);
		addTask(new AITask, AITASK);
		addTask(new GameRulesTask, GAMERULESTASK);
		addTask(new PhysicsTask, PHYSICSTASK);
		addTask(new TimeTask, TIMERTASK);

		loadLevel();
		return true;
	}

	virtual bool cleanup() override
	{
		unloadLevel();

		return Engine::cleanup();
	}


	~DemoGame()
	{}

	void loadLevel();

	void unloadLevel()
	{
		// unspawn every remaining objects
		Game<IGameSpawner>()->unspawnAll();
		Game<IGameSpawner>()->update();
	}
};



EngineRef IEngine::Instance()
{
	static EngineRef sGame(new DemoGame);
	return sGame;
}





void DemoGame::loadLevel()
{
	Game<IGameSpawner>()->spawn<GoSkyBox>(PxTransform(PxVec3(0, 0, 0)));
	Game<IGameSpawner>()->spawn<GoTerrain>(PxTransform(PxVec3(0, 0, 0)));

	// spawn the player
	GameObjectRef _player = Game<IGameSpawner>()->spawn<GoPlayer>(PxTransform(PxVec3(0, 0, 0)));
	_player->addComponent<KeyboardInputComponent>();
	_player->addComponent<PlayerComponent>()->setBehaviour(IBehaviourRef(new PlayerBehaviour));

	Game<IGameSpawner>()->spawn<GoCow>(PxTransform(PxVec3(0, 0, 0)));
	Game<IGameSpawner>()->spawn<GoTriggerZone>(PxTransform(PxVec3(20, 0, 20)));

	Game<IGameSpawner>()->spawn<ICamera>(PxTransform(PxVec3(0, 0, 0)));
}



