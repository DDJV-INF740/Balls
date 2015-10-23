//=============================================================================
// EXTERNAL DECLARATIONS
//=============================================================================
#include "Precompiled.h"

#include "GoCow.h"

#include "PxPhysicsAPI.h"
#include <d3dx9.h>

#include "Engine/Components/InputComponent.h"
#include "Engine/Components/RenderComponent.h"
#include "Engine/Components/AIComponent.h"
#include "Engine/Components/SimulationComponent.h"
#include "Import/DotX/DotXModel.h"
#include "Assets/Models/ModelRendering.h"
#include "Core//GameManagers/IGameRendering.h"
#include "Core/GameObjects/IGameObjectData.h"
#include "Engine/GameObjects/GameObject.h"
#include "CollisionTypes.h"
#include "Core/GameObjects/GameObjectFactory.h"

using namespace physx;

//=============================================================================
// CLASS GoCowData
//=============================================================================
class GoCowData: public IGameObjectData
{
public:
	PxMaterial *_material;
	IModel *_model;

public:
	//-------------------------------------------------------------------------
	//
	GoCowData()
		: _material(nullptr)
		, _model(nullptr)
	{}

	//-------------------------------------------------------------------------
	//
	int load()
	{
		// load the .x model
		const char *modelFile = "assets/cow.x";
		_model = new DotXModel(Game<IGameRendering>()->d3dDevice(), modelFile, "assets/");

		// create the physic object
		PxPhysics &physics = Game<IGameSimulation>()->physics();
		_material = physics.createMaterial(0.5f, 0.5f, 0.1f);    //static friction, dynamic friction, restitution

		return 0;
	}

	//-------------------------------------------------------------------------
	//
	~GoCowData()
	{
		// cleanup render objects
	    if (_model)
		{
			delete _model;
		}

		// cleanup physics objects
		if (_material)
		{
			_material->release();
			_material = nullptr;
		}
	}
};

//=============================================================================
// CLASS GoCow
//=============================================================================
class GoCowImp: public GameObject<GoCowImp, GoCowData>
{
public:
	PxShape *_actorShape;

public:
	GoCowImp(const IGameObjectDataRef &aDataRef)
		: GameObject(aDataRef)
		, _actorShape(nullptr)
	{}

	//-------------------------------------------------------------------------
	//
	static IGameObject::IdType	TypeId() { return "GoCow"; }

public:
	//-------------------------------------------------------------------------
	//
	virtual void onSpawn(const PxTransform &aPose) override
	{
		addComponent<RenderComponent>()->setRenderPrimitive(IRenderPrimitiveRef(new ModelRendering(*_data->_model)));

		PxVec3 origPos(0, 0, 0);
		PxTransform origTransform(origPos);

		std::shared_ptr<DynamicSimulationComponent> simulationComponent = addComponent<DynamicSimulationComponent>();
		PxRigidDynamic &pxActor = simulationComponent->pxActor();
		pxActor.setGlobalPose(origTransform);

		PxShape *actorShape = pxActor.createShape(PxBoxGeometry(PxVec3(3, 2, 1)), *_data->_material);

		PxFilterData filterData;
		filterData.word0 = 0;
		filterData.word1 = eACTOR_TERRAIN | eACTOR_BULLET;
		actorShape->setSimulationFilterData(filterData);

	}

	//-------------------------------------------------------------------------
	//
	virtual void onUnspawn() override
	{}
};

//-----------------------------------------------------------------------------
//
IGameObject::IdType	GoCow::TypeId()
{
	return GoCowImp::TypeId();

}

RegisterGameObjectType<GoCowImp> gRegisterActor;

