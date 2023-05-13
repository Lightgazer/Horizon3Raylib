#pragma once

#include <optional>
#include "GlobalState.h"
#include "UIElement.h"
#include "GameSettings.h"

class MainMenu : public GlobalState
{
private:
	std::optional<UIElement> _button;

public:
	MainMenu();
	void ReStart() override;
	void Update(const float delta) override;
	void Draw() override;
};