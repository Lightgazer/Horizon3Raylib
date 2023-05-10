#include "ScoreWidget.h"
#include "GameSettings.h"
#include "raylib.h"

using namespace std;

void ScoreWidget::SetScore(int score)
{
	_score = score;
}

void ScoreWidget::Draw()
{
	DrawText(("Score: " + to_string(_score)).c_str(), GameSettings::Width * 0.8, 10, 20, DARKGRAY);
}
