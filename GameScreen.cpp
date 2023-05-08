#pragma once

#include "GameScreen.h"

void GameScreen::ReStart()
{
	_grid = BoardContext();
	_timer = GameTimer(60.0f);
}

void GameScreen::Update(const float delta)
{
	_grid.Update(delta);
	_timer.Update(delta);
}

void GameScreen::Draw()
{
	_grid.Draw();
	_timer.Draw();
}


