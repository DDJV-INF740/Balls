#pragma once

#include "Core/GameObjects/IGameObject.h"

class GoBall: virtual public IGameObject
{
public:
	static IGameObject::IdType TypeId();
};

