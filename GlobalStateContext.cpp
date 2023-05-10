#include "GlobalStateContext.h"

GlobalStateContext::GlobalStateContext()
{
	_states.push_back(make_unique<MainMenu>());
	_states.push_back(make_unique<GameScreen>());
	_states.push_back(make_unique<EndMenu>());
}

void GlobalStateContext::Update(const float delta)
{
	if (_states[_currentState]->ConsumeStateFlag()) {
		_currentState = (_currentState + 1) % _states.size();
		_states[_currentState]->ReStart();
	}
	_states[_currentState]->Update(delta);
}

void GlobalStateContext::Draw()
{
	_states[_currentState]->Draw();
}
