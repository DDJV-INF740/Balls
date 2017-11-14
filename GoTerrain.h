#pragma once
#include "Core/GameObjects/IGameObject.h"
#include "Core/GameObjects/IGameObject.h"
#include "Engine/GameObjects/GameObject.h"


using namespace engine;

class GoTerrain : virtual public IGameObject
{
public:
	static constexpr IdType TypeId = "GoTerrain";
	static GameObjectDataRef loadData();
	static GameObjectRef createInstance(const GameObjectDataRef &aDataRef);
};
