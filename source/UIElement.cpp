#include "UIElement.h"

UIElement::UIElement(Texture2D texture, Point position) : _texture(texture) 
{
	Point size = Point(texture.width, texture.height);
	Point location = position - size / 2;
	_rectangle = IntRect(location, size);
}

void UIElement::Update()
{
	_wasPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && _rectangle.Contains(Point(GetMousePosition()));
}

void UIElement::Draw()
{
	DrawTexture(_texture, _rectangle.Location.X, _rectangle.Location.Y, WHITE);
}

bool UIElement::WasPressed()
{
	return _wasPressed;
}
