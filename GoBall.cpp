#include "Precompiled.h"

#include "GoBall.h"
#include "Core/GameObjects/IGameObject.h"
#include "Core/GameObjects/IGameObjectData.h"
#include "Assets/Models/MeshModel.h"
#include "Core/GameManagers/ISimulationManager.h"
#include "Core/Game/Game.h"
#include "Core/GameManagers/IRenderManager.h"
#include "Core/GameObjects/IBehaviour.h"
#include "Core/GameManagers/ITimeManager.h"
#include "Core/GameManagers/ISpawnManager.h"
#include "Engine/GameObjects/GameObject.h"
#include "Engine/Components/RenderComponent.h"
#include "Assets/Models/ModelRendering.h"
#include "Engine/Components/SimulationComponent.h"
#include "Engine/Components/AIComponent.h"
#include "Core/GameObjects/GameObjectFactory.h"
#include "Loader/Models/ModelFactory.h"
#include "CollisionTypes.h"
#include "PxPhysicsAPI.h"

#include <d3dx9.h>

using namespace physx;
using namespace engine;
using namespace std::chrono;

class GoBallData: public IGameObjectData
{
public:
	std::unique_ptr<MeshModel> _model;
	physx::unique_ptr<PxMaterial> _material;
	float _radius = 0.0f;

public:
	int load()
	{
		const float kRadius = 0.25f;
		_radius = kRadius;

		// create the physic object
		ISimulationManagerRef simulation = Game<ISimulationManager>();
		_material = physx::unique_ptr<PxMaterial>(simulation->physics().createMaterial(0.5f, 0.5f, 0.1f));    //static friction, dynamic friction, restitution

		//----------------------------------------------------------
		// create the render object
		_model = ModelFactory::createSphere(kRadius);

		return 0;
	}
};

class GoBallState
{
public:
	ITimeManager::time_point _spawnTime;
};

class BallBehaviour: virtual public IBehaviour
{
private:
	GoBallState _state;

public:
	BallBehaviour()
	{
		_state._spawnTime = Game<ITimeManager>()->currentTime();
	}

	virtual void update(const GameObjectRef &iGameObject) override
	{
		if (Game<ITimeManager>()->currentTime() - _state._spawnTime > 10s)
			Game<ISpawnManager>()->unspawn(iGameObject);
	}
};

class GoBallImp: public GoBall, public GameObject<GoBallImp, GoBallData>
{

public:
	virtual IdType typeId() const
	{ return GoBall::TypeId; }

	//-------------------------------------------------------------------------
	//
	static GameObjectRef createInstance(const GameObjectDataRef &aDataRef)
	{
		return GameObject<GoBallImp, GoBallData>::createInstance(aDataRef);
	}

	//-------------------------------------------------------------------------
	//
	static std::shared_ptr<IGameObjectData> loadData()
	{
		std::shared_ptr<GoBallData> data = std::make_shared<GoBallData>();
		data->load();
		return data;
	}

	//-------------------------------------------------------------------------
	//
	GoBallImp(const GameObjectDataRef &aDataRef)
		: GameObject(aDataRef)
	{}

public:
	virtual void onSpawn(const PxTransform &aPose) override
	{
		createComponent<RenderComponent>()->setRenderPrimitive(IRenderPrimitiveRef(new ModelRendering(*_data->_model)));

		// setup simulation component
		auto simulationComponent = createComponent<DynamicSimulationComponent>();
		PxRigidDynamic &pxActor = simulationComponent->pxActor();
		pxActor.setGlobalPose(aPose);
		
		PxShape *actorShape = pxActor.createShape(PxSphereGeometry(_data->_radius), *_data->_material);

		PxFilterData filterData;
		filterData.word0 = eACTOR_BULLET;
		filterData.word1 = eACTOR_ENEMY | eACTOR_TERRAIN | eACTOR_BULLET;
		actorShape->setSimulationFilterData(filterData);

		createComponent<AIComponent>()->setBehaviour(IBehaviourRef(new BallBehaviour));
	}

	//-------------------------------------------------------------------------
	//
	virtual void onUnspawn() override
	{}
};

//-----------------------------------------------------------------------------
//
GameObjectDataRef GoBall::loadData()
{
	return GoBallImp::loadData();
}

//-----------------------------------------------------------------------------
//
GameObjectRef GoBall::createInstance(const GameObjectDataRef &aDataRef)
{
	return GoBallImp::createInstance(aDataRef);
}


