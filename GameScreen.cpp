#pragma once

#include "GameScreen.h"

void GameScreen::ReStart()
{
	_grid = BoardContext();
	_scoreWidget = ScoreWidget();
	_timer = GameTimer(60.0f);
}

void GameScreen::Update(const float delta)
{
	_grid.Update(delta);
	_scoreWidget.SetScore(_grid.Model.GetScore());
	_timer.Update(delta);
}

void GameScreen::Draw()
{
	_grid.Draw();
	_scoreWidget.Draw();
	_timer.Draw();
}


