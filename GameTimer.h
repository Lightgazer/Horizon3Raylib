#pragma once

#include <string>

using namespace std;

class GameTimer
{
private:
	float _timeLeft;
	string _labelText;

public:
	GameTimer();
	GameTimer(const float& seconds);
	void Update(const float& delta);
	void Draw();
};
