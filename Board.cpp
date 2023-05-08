#pragma once

#include <memory>
#include "raylib.h"
#include "Board.h"
#include "MyMath.h"

using namespace std;

IdleState::IdleState(unique_ptr<IdleTurn>&& turn) : _turn(move(turn)) {}

void IdleState::Update(const float delta, BoardContext* context)
{
	bool buttonPressed = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
	Point mousePosition(GetMousePosition());
	if (!_wasButtonPressed && buttonPressed && BoardContext::GridRectangle.Contains(mousePosition)) 
	{
		Point click = (mousePosition - BoardContext::Padding) / GameSettings::BlockSize;
		if (
			IsBlockSelected() && context->Model.SwapBlocks(
				_selectedIndex.ToIndex(GameSettings::GridSize), 
				click.ToIndex(GameSettings::GridSize)
			)
		)
		{
			context->NextTurn();
		}
		_selectedIndex = click;
	}

	_wasButtonPressed = buttonPressed;
}

void IdleState::Draw(BoardContext* context) 
{
	for (int i = 0; i < _turn->Blocks.size(); i++) 
	{
		Point position = Point::FromIndex(i, GameSettings::GridSize) * GameSettings::BlockSize + BoardContext::Padding;
		int type = _turn->Blocks[i]->Type;
		DrawTexture(context->MarbleTextures[type], position.X, position.Y, WHITE);
		// DrawBonusIcon(spriteBatch, block, position);
	}

	if (!IsBlockSelected()) return;
	Point position = _selectedIndex * GameSettings::BlockSize + BoardContext::Padding;
	DrawTexture(context->FrameTexture, position.X, position.Y, WHITE);
}

bool IdleState::IsBlockSelected()
{
	return _selectedIndex.X != -1 || _selectedIndex.Y != -1;
}

CascadeState::CascadeState(unique_ptr<CascadeTurn>&& turn) : _turn(move(turn)) {}

void CascadeState::Update(const float time, BoardContext* context)
{
	float delta = GameSettings::AnimationSpeed * time;
	_localShrink = MoveTowards(_localShrink, TargetSizeShrink, delta);
	for (const int index : _turn->Dead) 
	{
		_shrinkGrid[index] = _localShrink;
	}
	//_bonuses.ForEach(bonus = > bonus.Update(gameTime, _shrinkGrid));

	if (IsOver()) context->NextTurn();
}

void CascadeState::Draw(BoardContext* context)
{
	for (int i = 0; i < _turn->Blocks.size(); i++)
	{
		Point position = Point::FromIndex(i, GameSettings::GridSize) * GameSettings::BlockSize + BoardContext::Padding;
		int type = _turn->Blocks[i]->Type;
		float size = 1.0f - _shrinkGrid[i];
		Texture2D& texture = context->MarbleTextures[type];
		float width = static_cast<float>(texture.width);
		float height = static_cast<float>(texture.height);
		Rectangle dest = 
		{ 
			static_cast<float>(position.X + width) - width * size / 2,
			static_cast<float>(position.Y + height) - height * size / 2,
			width * size, 
			height * size 
		};
		DrawTexturePro(texture,	{ 0, 0, width, height }, dest, context->BlockOrigin.ToVector2(), 0.0f, WHITE);
		//DrawBonusIcon(spriteBatch, block, position);
	}
}

bool CascadeState::IsOver()
{
	return abs(_localShrink - TargetSizeShrink) < FLT_EPSILON; // && _bonuses.All(bonus => bonus.Over);
}

DropState::DropState(unique_ptr<DropTurn>&& turn) : _turn(move(turn)) {}

void DropState::Update(const float time, BoardContext* context)
{
	float target = GameSettings::BlockSize;
	if (abs(_displacement - target) < FLT_EPSILON)
	{
		context->NextTurn();
		return;
	}
	float delta = time * target * 2.0f * GameSettings::AnimationSpeed;
	_displacement = MoveTowards(_displacement, target, delta);
}

void DropState::Draw(BoardContext* context)
{
	for (int i = 0; i < _turn->Blocks.size(); i++)
	{
		if (!_turn->Blocks[i]->Alive) continue;
		int type = _turn->Blocks[i]->Type;
		Vector2 position = (Point::FromIndex(i, GameSettings::GridSize) * 
			GameSettings::BlockSize + BoardContext::Padding).ToVector2();
		if (_turn->Drop.contains(i)) position += Vector2(0, _displacement);
		DrawTextureV(context->MarbleTextures[type], position, WHITE);
		//DrawBonusIcon(spriteBatch, block, position);
	}
}

SwapState::SwapState(unique_ptr<SwapTurn>&& turn) : 
	_turn(move(turn)), 
	_direction(
		Point::FromIndex(turn->First, GameSettings::GridSize).ToVector2() -
		Point::FromIndex(turn->Second, GameSettings::GridSize).ToVector2()
	) {}

void SwapState::Update(const float time, BoardContext* context)
{
	float target = 0;
	float delta = time * GameSettings::BlockSize * 2.0f * GameSettings::AnimationSpeed;
	_displacement = MoveTowards(_displacement, target, delta);
	if (abs(_displacement - target) < FLT_EPSILON) context->NextTurn();
}

void SwapState::Draw(BoardContext* context)
{
	for (int i = 0; i < _turn->Blocks.size(); i++)
	{
		int type = _turn->Blocks[i]->Type;
		Vector2 position = (Point::FromIndex(i, GameSettings::GridSize) *
			GameSettings::BlockSize + BoardContext::Padding).ToVector2();
		if (_turn->First == i) position += _direction * _displacement;
		if (_turn->Second == i) position += -_direction * _displacement;
		DrawTextureV(context->MarbleTextures[type], position, WHITE);
		//DrawBonusIcon(spriteBatch, block, position);
	}
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
	if (IdleTurn* idleTurn = dynamic_cast<IdleTurn*>(turn.get())) 
	{
		turn.release();
		unique_ptr<IdleTurn> idleTurnPtr(idleTurn);
		_state.reset(new IdleState(move(idleTurnPtr)));
	} 
	else if (CascadeTurn* cascadeTurn = dynamic_cast<CascadeTurn*>(turn.get())) 
	{
		turn.release();
		unique_ptr<CascadeTurn> cascadeTurnPtr(cascadeTurn);
		_state.reset(new CascadeState(move(cascadeTurnPtr)));
	}
	else if (DropTurn* dropTurn = dynamic_cast<DropTurn*>(turn.get()))
	{
		turn.release();
		unique_ptr<DropTurn> dropTurnPtr(dropTurn);
		_state.reset(new DropState(move(dropTurnPtr)));
	}
	else if (SwapTurn* swapTurn = dynamic_cast<SwapTurn*>(turn.get()))
	{
		turn.release();
		unique_ptr<SwapTurn> swapTurnPtr(swapTurn);
		_state.reset(new SwapState(move(swapTurnPtr)));
	}
	else 
	{
		throw "No state for this turn";
	}
}

void BoardContext::Update(const float delta)
{
	_state->Update(delta, this);
}

void BoardContext::Draw()
{
	_state->Draw(this);
}
