#pragma once
#include "Core/GameObjects/IGameObject.h"
#include "Core/GameObjects/IGameObject.h"
#include "Engine/GameObjects/GameObject.h"

using namespace engine;

class GoTriggerZone : virtual public IGameObject
{
public:
	static constexpr IdType TypeId = "TriggerActor";
	static GameObjectDataRef loadData();
	static GameObjectRef createInstance(const GameObjectDataRef &aDataRef);
};

