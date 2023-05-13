#include "GlobalState.h"

void GlobalState::RequestNextState()
{
	_isNextStateRequired = true;
}

bool GlobalState::ConsumeStateFlag()
{
	bool result = _isNextStateRequired;
	_isNextStateRequired = false;
	return result;
}
