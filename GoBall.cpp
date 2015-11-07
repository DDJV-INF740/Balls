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

class GoBallData: public IGameObjectData
{
public:
	typedef std::auto_ptr<MeshModel> MeshModelRef;
	MeshModelRef _model;
	PxMaterial *_material;
	float _radius;

public:
	GoBallData()
		: _material(nullptr)

	{}

	int load()
	{
		const float kRadius = 0.25f;
		_radius = kRadius;

		// create the physic object

		_material =  Game<ISimulationManager>()->physics().createMaterial(0.5f, 0.5f, 0.1f);    //static friction, dynamic friction, restitution

		//----------------------------------------------------------
		// create the render object
		_model = MeshModelRef(ModelFactory::createSphere(kRadius));

		return 0;
	}

	~GoBallData()
	{
		// cleanup physics objects
		if (_material)
		{
			_material->release();
			_material = nullptr;
		}
	}
};

class GoBallState
{
public:
	GoBallState()
		: _spawnTime(0)
	{}

public:
	double _spawnTime;
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
		if (Game<ITimeManager>()->currentTime() - _state._spawnTime > 10)
			Game<ISpawnManager>()->unspawn(iGameObject);
	}
};

class GoBallImp: public GoBall, public GameObject<GoBallImp, GoBallData>
{

public:
	static IGameObject::IdType TypeId()
	{ return "GoBall"; }

	//-------------------------------------------------------------------------
	//
	GoBallImp(const IGameObjectDataRef &aDataRef)
		: GameObject(aDataRef)
	{}

public:
	virtual void onSpawn(const PxTransform &aPose) override
	{
		addComponent<RenderComponent>()->setRenderPrimitive(IRenderPrimitiveRef(new ModelRendering(*_data->_model)));

		// setup simulation component
		auto simulationComponent = addComponent<DynamicSimulationComponent>();
		PxRigidDynamic &pxActor = simulationComponent->pxActor();
		pxActor.setGlobalPose(aPose);
		
		PxShape *actorShape = pxActor.createShape(PxSphereGeometry(_data->_radius), *_data->_material);

		PxFilterData filterData;
		filterData.word0 = eACTOR_BULLET;
		filterData.word1 = eACTOR_ENEMY | eACTOR_TERRAIN | eACTOR_BULLET;
		actorShape->setSimulationFilterData(filterData);

		addComponent<AIComponent>()->setBehaviour(IBehaviourRef(new BallBehaviour));
	}

	//-------------------------------------------------------------------------
	//
	virtual void onUnspawn() override
	{}
};

//-----------------------------------------------------------------------------
//
IGameObject::IdType GoBall::TypeId()
{	return GoBallImp::TypeId(); }

RegisterGameObjectType<GoBallImp> gRegisterActor;

