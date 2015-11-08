#pragma once

#include "Core/GameObjects/IGameObject.h"

using namespace engine;

class GoBall: virtual public IGameObject
{
public:
	static IGameObject::IdType TypeId();
};

