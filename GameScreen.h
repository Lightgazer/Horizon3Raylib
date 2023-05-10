#pragma once

#include "GameTimer.h"
#include "Board.h"
#include "ScoreWidget.h"

class GameScreen
{
private:
	BoardContext _grid;
	ScoreWidget _scoreWidget;
	GameTimer _timer;

public:
	void ReStart();
	void Update(const float delta);
	void Draw();
};