#include "Precompiled.h"

//=============================================================================
// EXTERNAL DECLARATIONS
//=============================================================================
#include "GoSkybox.h"

#include "Core/Game/Game.h"
#include "Core/Rendering/IRenderPrimitive.h"
#include "Engine/Components/RenderComponent.h"
#include "Engine/Rendering/SkyboxRenderData.h"
#include "Engine/Rendering/SkyboxRendering.h"
#include "Core/GameManagers/IRenderManager.h"
#include "Core/GameObjects/IGameObject.h"
#include "Engine/GameObjects/GameObject.h"
#include "Core/Components/IPoseInterface.h"
#include "Core/GameManagers/ICameraManager.h"
#include "Engine/Components/FollowPoseComponent.h"

using namespace physx;

//=============================================================================
// CLASS SkyboxData
//=============================================================================
class SkyboxData: public IGameObjectData
{
public:
	SkyboxRenderData _renderData;

public:
	int load()
	{
		static const TCHAR *kTextureFileName = _T("assets/skybox.jpg");
		LPDIRECT3DDEVICE9 d3ddev = Game<IRenderManager>()->d3dDevice();
		_renderData.load(kTextureFileName, d3ddev);
		return 0;
	}
};

//=============================================================================
// CLASS Skybox
//=============================================================================
class GoSkyBoxImp: public GameObject<GoSkyBoxImp, SkyboxData>
{
public: // IPoseInterface

public:
	static IGameObject::IdType TypeId()
	{
		return "Skybox";
	}

	//-------------------------------------------------------------------------
	//
	GoSkyBoxImp(const IGameObjectDataRef  &aDataRef)
		: GameObject(aDataRef)
	{}

	//-------------------------------------------------------------------------
	//
	virtual void onSpawn(const PxTransform &aPose) override
	{
		addComponent<FollowPoseComponent>()->follow(Game<ICameraManager>()->mainCamera());

		IRenderPrimitiveRef renderPrimitive(new SkyboxRendering(_data->_renderData));
		addComponent<RenderComponent>()->setRenderPrimitive(renderPrimitive);
	}

	//-------------------------------------------------------------------------
	//
	virtual void onUnspawn() override
	{}
};

//=============================================================================
// CLASS SkyboxDesc
//=============================================================================
IGameObject::IdType GoSkyBox::TypeId()
{
	return GoSkyBoxImp::TypeId();
}


//=============================================================================
// REGISTER "Skybox"
//=============================================================================
RegisterGameObjectType<GoSkyBoxImp> gRegisterActor;
