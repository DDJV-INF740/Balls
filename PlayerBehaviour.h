#include "Core/GameObjects/IBehaviour.h"
#include "Core/GameManagers/ITimeManager.h"

using namespace engine;

class PlayerActorState
{
public:
	PlayerActorState()
		: _lastFireTime()
	{}

	ITimeManager::time_point _lastFireTime;
};


class PlayerBehaviour: public IBehaviour
{
private:
	PlayerActorState _state;

public:
	virtual void update(const GameObjectRef &iGameObject) override;
};
