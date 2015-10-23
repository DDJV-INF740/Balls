#pragma once

#include "Core/GameObjects/IGameObject.h"

class GoPlayer: virtual public IGameObject
{
public:
	static IGameObject::IdType TypeId();
};

