#include "Precompiled.h"

//=============================================================================
// EXTERNAL DECLARATIONS
//=============================================================================
#include "GoTerrain.h"
#include "Core/Rendering/IRenderPrimitive.h"

#include "Core/Game/Game.h"
#include "Core/GameObjects/IGameObject.h"
#include "Import/Images/OLEImageImporter.h"
#include "Engine/Components/SimulationComponent.h"
#include "Engine/Components/RenderComponent.h"
#include "Core/GameObjects/IGameObjectData.h"
#include "Core/GameObjects/IGameObject.h"
#include "Engine/GameObjects/GameObject.h"
#include "Core/GameManagers/IRenderManager.h"
#include "Core/GameObjects/GameObjectFactory.h"

#include "Engine/Terrain/HeightMapData.h"
#include "Engine/Terrain/HeightMapRendering.h"
#include "Engine/Terrain/TerrainData.h"

#include "CollisionTypes.h"
#include <windows.h>
#include <PxPhysicsAPI.h>
#include <d3d9.h>
using namespace physx;


//=============================================================================
// CLASS Terrain
//=============================================================================
class GoTerrainImp: public GameObject<GoTerrainImp, TerrainData>
{
public:
	PxShape *_heightMapShape;

public:
	//-------------------------------------------------------------------------
	//
	GoTerrainImp(const GameObjectDataRef &aDataRef)
		: GameObject(aDataRef)
		, _heightMapShape(nullptr)
	{}

	//-------------------------------------------------------------------------
	//
	~GoTerrainImp()
	{}


	//-------------------------------------------------------------------------
	//
	static constexpr IdType TypeId = GoTerrain::TypeId;

	//-------------------------------------------------------------------------
	//
	virtual void onSpawn(const PxTransform &/*aPose*/) override
	{
		// create the physic object
		PxVec3 origPos(-_data->_renderData._scale.x/2.0f, 0.0f, -_data->_renderData._scale.z/2.0f);
		PxTransform origTransform(origPos);
		auto simulationComponent = createComponent<StaticSimulationComponent>();
		PxRigidStatic &pxActor = simulationComponent->pxActor();
		pxActor.setGlobalPose(origTransform);
		_heightMapShape = pxActor.createShape(PxHeightFieldGeometry(_data->_heightField.get(), PxMeshGeometryFlag::eDOUBLE_SIDED, _data->_renderData._scale.y / 65535.0f, 1.0f*_data->_renderData._scale.x/_data->_renderData._width, 1.0f*_data->_renderData._scale.z/_data->_renderData._height), *_data->_material);

		PxFilterData filterData;
		filterData.word0 = eACTOR_TERRAIN;
 		filterData.word1 = 0;
 		_heightMapShape->setSimulationFilterData(filterData);


		// create the vertices using the CUSTOMVERTEX struct
		createComponent<RenderComponent>()->setRenderPrimitive(IRenderPrimitiveRef(new HeightMapRendering(&_data->_renderData)));

	}

	//-------------------------------------------------------------------------
	//
	virtual void onUnspawn() override
	{}
};


//-----------------------------------------------------------------------------
//
GameObjectDataRef GoTerrain::loadData()
{
	return GoTerrainImp::loadData();
}

//-----------------------------------------------------------------------------
//
GameObjectRef GoTerrain::createInstance(const GameObjectDataRef &aDataRef)
{
	return GoTerrainImp::createInstance(aDataRef);
}


