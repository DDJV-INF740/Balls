#pragma once

#include "Core/GameObjects/IGameObject.h"

using namespace engine;

class GoCow : public IGameObject
{
public:
	static IGameObject::IdType TypeId();
};

