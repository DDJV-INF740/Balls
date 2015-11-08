#pragma once
#include "Core/GameObjects/IGameObject.h"

using namespace engine;

class GoTriggerZone: public IGameObject
{
public:
	static IGameObject::IdType	TypeId();
};
