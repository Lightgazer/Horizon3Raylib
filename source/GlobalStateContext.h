#pragma once
#include <array>
#include <memory>
#include "GlobalState.h"
#include "GameScreen.h"
#include "EndMenu.h"
#include "MainMenu.h"

using namespace std;

class GlobalStateContext
{
private:
	vector<unique_ptr<GlobalState>> _states;
	int _currentState = 0;

public:
	GlobalStateContext();
	void Update(const float delta);
	void Draw();
};


