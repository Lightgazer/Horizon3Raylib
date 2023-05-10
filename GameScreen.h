#pragma once

#include <functional>
#include "GameTimer.h"
#include "Board.h"
#include "ScoreWidget.h"
#include "GlobalState.h"

class GameScreen : public GlobalState
{
private:
	BoardContext _grid;
	ScoreWidget _scoreWidget;
	GameTimer _timer;

public:
	GameScreen();
	void ReStart() override;
	void Update(const float delta) override;
	void Draw() override;
};