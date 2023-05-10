#pragma once
#include <string>
#include "raylib.h"
#include "GameTimer.h"
#include "GameSettings.h"

using namespace std;

GameTimer::GameTimer(const float seconds) : _timeLeft(seconds) { }

void GameTimer::Update(const float delta)
{
	_timeLeft -= delta;
	_labelText = "Time Left: " + to_string(static_cast<int>(_timeLeft));
}

void GameTimer::Draw()
{
	DrawText(_labelText.c_str(), static_cast<int>(GameSettings::Width * 0.1), 10, 20, DARKGRAY);
}

bool GameTimer::IsTimeout()
{
	return _timeLeft < 0;
}
