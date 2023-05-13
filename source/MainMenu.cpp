#include "MainMenu.h"

MainMenu::MainMenu()
{
	Point center = Point(GameSettings::Width / 2, GameSettings::Height / 2);
	_button = UIElement(LoadTexture("resources/buttons/play.png"), center);
}

void MainMenu::ReStart()
{
}

void MainMenu::Update(const float delta)
{
	if (_button->WasPressed()) {
		RequestNextState();
	}
	_button->Update();
}

void MainMenu::Draw()
{
	_button->Draw();
}
