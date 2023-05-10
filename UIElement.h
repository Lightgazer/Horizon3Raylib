#pragma once
#include "raylib.h"
#include "Geometry.h"

class UIElement
{
private:
	Texture2D _texture;
	IntRect _rectangle;
	bool _wasPressed = false;

public:
	UIElement(Texture2D texture, Point position);
	void Update();
	void Draw();
	bool WasPressed();
};

