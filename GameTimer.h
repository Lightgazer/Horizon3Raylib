#pragma once

#include <string>

using namespace std;

class GameTimer
{
private:
	float _timeLeft;
	string _labelText;

public:
	GameTimer(const float seconds);
	void Update(const float delta);
	void Draw();
	bool IsTimeout();
};
