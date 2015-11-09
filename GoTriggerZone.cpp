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
#include "Core/GameManagers/IRenderManager.h"
#include "Core/GameManagers/ISimulationManager.h"
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
	std::unique_ptr<DotXModel> _model;
	physx::unique_ptr<PxMaterial> _material;
	physx::unique_ptr<PxConvexMesh> _convexMesh;

public:
	//-------------------------------------------------------------------------
	//
	int load()
	{
		_model = std::make_unique<DotXModel>(Game<IRenderManager>()->d3dDevice(), "assets/area.x", "assets/");

		// create the physic object
		PxPhysics &physics = Game<ISimulationManager>()->physics();

		_convexMesh = ModelFactory::convexMeshFromModel(*_model);

		_material = physx::unique_ptr<PxMaterial>(physics.createMaterial(0.5f, 0.5f, 0.1f));    //static friction, dynamic friction, restitution
		return 0;
	}
};

class GoTriggerZoneImp: public GameObject<GoTriggerZoneImp, TriggerActorData>
{
public:
	GoTriggerZoneImp(const GameObjectDataRef &aDataRef)
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
		createComponent<RenderComponent>()->setRenderPrimitive(IRenderPrimitiveRef(new ModelRendering(*_data->_model)));

		auto simulationComponent = createComponent<StaticSimulationComponent>();
		physx::PxRigidStatic &pxActor = simulationComponent->pxActor();
		pxActor.setGlobalPose(aPose);
		PxShape *actorShape = pxActor.createShape(PxConvexMeshGeometry(_data->_convexMesh.get()), *_data->_material);
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

RegisterGameObjectType<GoTriggerZoneImp> gRegisterGameObject;

