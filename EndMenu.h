#pragma once

#include <optional>
#include "GlobalState.h"
#include "UIElement.h"
#include "GameSettings.h"

class EndMenu : public GlobalState
{
private:
	std::optional<UIElement> _button;
	std::optional<UIElement> _message;

public:
	EndMenu();
	void ReStart() override;
	void Update(const float delta) override;
	void Draw() override;
};

