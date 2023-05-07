#pragma once

#include <memory>
#include "raylib.h"
#include "Board.h"

using namespace std;

bool IdleState::IsBlockSelected()
{
	return _selectedIndex.X != -1 || _selectedIndex.Y != -1;
}

IdleState::IdleState(unique_ptr<IdleTurn>&& turn) : _turn(move(turn)) {}

void IdleState::Update(const float& delta, BoardContext* context)
{
	//private void ManagePlayerInput(GameContext context)
	//{
	//    if (GetCellClick() is { } click)
	//    {
	//        if (_selectedIndex.HasValue && context.Model.SwapBlocks(_selectedIndex.Value, click))
	//            context.NextTurn();

	//        _selectedIndex = click;
	//    }
	//}

	bool buttonPressed = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
	Point mousePosition{ GetMousePosition() };
	if (!_wasButtonPressed && buttonPressed && BoardContext::GridRectangle.Contains(mousePosition)) {
		_selectedIndex = (mousePosition - BoardContext::Padding) / GameSettings::BlockSize;
	}

	_wasButtonPressed = buttonPressed;
}

void IdleState::Draw(BoardContext* context) {
	for (int i = 0; i < _turn->Blocks.size(); i++) {
		Point position = Point::FromIndex(i, GameSettings::GridSize) * GameSettings::BlockSize + BoardContext::Padding;
		int& type = _turn->Blocks[i]->Type;
		DrawTexture(context->MarbleTextures[type], position.X, position.Y, WHITE);
		// DrawBonusIcon(spriteBatch, block, position);
	}

	if (!IsBlockSelected()) {
		return;
	}
	Point postion = _selectedIndex * GameSettings::BlockSize + BoardContext::Padding;
	DrawTexture(context->FrameTexture, postion.X, postion.Y, WHITE);
}

Point BoardContext::Padding{ (GameSettings::Width - SideLength) / 2, (GameSettings::Height - SideLength) / 2 };
Point BoardContext::BlockOrigin{ GameSettings::BlockSize / 2 };
IntRect BoardContext::GridRectangle{ Padding, Point(SideLength) };

BoardContext::BoardContext() :
	MarbleTextures({
		LoadTexture("resources/marbles/block0.png"),
		LoadTexture("resources/marbles/block1.png"),
		LoadTexture("resources/marbles/block2.png"),
		LoadTexture("resources/marbles/block3.png"),
		LoadTexture("resources/marbles/block4.png")
	}),
	FrameTexture(LoadTexture("resources/frame.png")),
	_state(nullptr),
	_bombTexture(LoadTexture("resources/bonuses/bomb.png")),
	_lineTexture(LoadTexture("resources/bonuses/line.png"))
{
	NextTurn();
}

void BoardContext::NextTurn()
{
	unique_ptr<ITurn> turn = Model.GetNextTurn();
	if (IdleTurn* idleTurn = dynamic_cast<IdleTurn*>(turn.get())) {
		turn.release();
		unique_ptr<IdleTurn> idleTurnPtr(idleTurn);
		_state.reset(new IdleState(move(idleTurnPtr)));
	}
}

void BoardContext::Update(const float& delta)
{
	_state->Update(delta, this);
}

void BoardContext::Draw()
{
	_state->Draw(this);
}