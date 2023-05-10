#pragma once

#include <string>

using namespace std;

class ScoreWidget
{
private:
	int _score = 0;

public:
	ScoreWidget() {}
	void SetScore(int score);
	void Draw();
};

