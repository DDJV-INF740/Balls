#include "Precompiled.h"

#include "GoTriggerZone.h"
#include "Import/DotX/DotXModel.h"
#include "Core/Game/Game.h"
#include "PxPhysicsAPI.h"
#include "Core/GameObjects/IGameObject.h"
#include "Engine/GameObjects/GameObject.h"
#include "Engine/Components/SimulationComponent.h"
#include "Engine/Components/RenderComponent.h"
#include "Core/Rendering/IRenderPrimitive.h"
#include "Assets/Models/ModelRendering.h"
#include "Core/Components/ICollisionHandler.h"
#include "Core/GameManagers/IGameRendering.h"
#include "Core/GameManagers/IGameSimulation.h"
#include "Loader/Models/ModelFactory.h"
#include "CollisionTypes.h"
#include "Core/GameObjects/GameObjectFactory.h"

using namespace physx;

class TriggerCollider: public ICollisionHandler
{
	virtual void onContact(const physx::PxContactPair &aContactPair) {}
	virtual void onTrigger(bool triggerEnter, physx::PxShape *actorShape, physx::PxShape *contactShape)
	{
		IGameObject *actor = nullptr;
		if (contactShape)
		{
			PxRigidActor *intruder = contactShape->getActor();
			actor = static_cast<IGameObject*>(intruder->userData);
		}
		else
		{
			// object was deleted, this is a trigger to let us know the deleted
			// object is not in the trigger area anymore
		}

		if (triggerEnter)
			printf("%s entered in the zone\n", actor ? actor->debugName() : "<deleted>");
		else
			printf("%s left in the zone\n", actor ? actor->debugName() : "<deleted>");
	}
};



class TriggerActorData: public IGameObjectData
{
public:
	DotXModel *_model;
	PxMaterial *_material;
	PxConvexMesh *_convexMesh;
public:
	TriggerActorData()
		: _model(nullptr)
		, _material(nullptr)
		, _convexMesh(nullptr)
	{}

	//-------------------------------------------------------------------------
	//
	int load()
	{
		_model = new DotXModel(Game<IGameRendering>()->d3dDevice(), "assets/area.x", "assets/");

		// create the physic object
		PxPhysics &physics = Game<IGameSimulation>()->physics();

		_convexMesh = ModelFactory::convexMeshFromModel(*_model);

		_material = physics.createMaterial(0.5f, 0.5f, 0.1f);    //static friction, dynamic friction, restitution
		return 0;
	}

	//-------------------------------------------------------------------------
	//
	~TriggerActorData()
	{
		delete _model;
		_model = nullptr;

		if (_material)
		{
			_material->release();
			_material = nullptr;
		}

		if (_convexMesh)
		{
			_convexMesh->release();
			_convexMesh = nullptr;
		}
	}
};

class GoTriggerZoneImp: public GameObject<GoTriggerZoneImp, TriggerActorData>
{
public:
	GoTriggerZoneImp(const IGameObjectDataRef &aDataRef)
		: GameObject(aDataRef)
	{}

	//-------------------------------------------------------------------------
	//
	static IdType TypeId()
	{	return "TriggerActor"; }

	//-------------------------------------------------------------------------
	//
	virtual void onSpawn(const PxTransform &aPose) override
	{
		addComponent<RenderComponent>()->setRenderPrimitive(IRenderPrimitiveRef(new ModelRendering(*_data->_model)));

		auto simulationComponent = addComponent<StaticSimulationComponent>();
		physx::PxRigidStatic &pxActor = simulationComponent->pxActor();
		pxActor.setGlobalPose(aPose);
		PxShape *actorShape = pxActor.createShape(PxConvexMeshGeometry(_data->_convexMesh), *_data->_material);
		actorShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		actorShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

		PxFilterData filterData;
		filterData.word0 = eACTOR_PLAYER;
		filterData.word1 = eACTOR_ENEMY | eACTOR_TERRAIN;
		actorShape->setSimulationFilterData(filterData);

		simulationComponent->setHandler(ICollisionHandlerRef(new TriggerCollider()));
	}

	//-------------------------------------------------------------------------
	//
	virtual void onUnspawn() override
	{}

};

//-----------------------------------------------------------------------------
//
IGameObject::IdType GoTriggerZone::TypeId()
{
	return GoTriggerZoneImp::TypeId();
}

RegisterGameObjectType<GoTriggerZoneImp> gRegisterActor;

