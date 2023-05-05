#pragma once

#include "GameTimer.h"
#include "Board.h"

class GameScreen
{
private:
	BoardContext _grid;
	//ScoreWidget _scoreWidget;
	GameTimer _timer;

public:
	GameScreen() {}
	void ReStart();
	void Update(const float& delta);
	void Draw();
};