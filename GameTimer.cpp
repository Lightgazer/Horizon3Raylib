#pragma once
#include <string>
#include "raylib.h"
#include "GameTimer.h"
#include "GameSettings.h"

using namespace std;

GameTimer::GameTimer() : _timeLeft(60.0f) { }

GameTimer::GameTimer(const float seconds) : _timeLeft(seconds) { }

void GameTimer::Update(const float delta)
{
	_timeLeft -= delta;
	_labelText = "Time Left: " + to_string(static_cast<int>(_timeLeft));
	//if (_timeLeft < 0) GlobalStateContext.ChangeState<EndMenu>();
}

void GameTimer::Draw()
{
	DrawText(_labelText.c_str(), GameSettings::Width * 0.1, 10, 20, DARKGRAY);
}
