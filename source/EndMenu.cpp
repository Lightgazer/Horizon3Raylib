#include "EndMenu.h"

using namespace std;

EndMenu::EndMenu()
{
	Point center = Point(GameSettings::Width / 2, GameSettings::Height / 2);
	Point someSpace = Point(0, center.Y / 4);
	_button = UIElement(LoadTexture("resources/buttons/ok.png"), center + someSpace);
	_message = UIElement(LoadTexture("resources/gameover.png"), center - someSpace);
}

void EndMenu::ReStart()
{
}

void EndMenu::Update(const float delta)
{
	if (_button->WasPressed()) {
		RequestNextState();
	}
	_message->Update();
	_button->Update();
}

void EndMenu::Draw()
{
	_message->Draw();
	_button->Draw();
}
