#pragma once
#include "Core/GameObjects/IGameObject.h"
#include "Core/GameObjects/IGameObject.h"
#include "Engine/GameObjects/GameObject.h"

using namespace engine;

class GoPlayer : virtual public IGameObject
{
public:
	static constexpr IdType TypeId = "GoPlayer";
	static GameObjectDataRef loadData();
	static GameObjectRef createInstance(const GameObjectDataRef &aDataRef);
};
