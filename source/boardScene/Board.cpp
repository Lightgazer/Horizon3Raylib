#pragma once

#include <stdexcept>
#include <memory>
#include <optional>
#include "raylib.h"
#include "Board.h"
#include "utils/MyMath.h"
#include "boolinq/boolinq.h"

using namespace std;

Point BoardContext::Padding{ (GameSettings::Width - SideLength) / 2, (GameSettings::Height - SideLength) / 2 };
Vector2 BoardContext::BlockOrigin{ GameSettings::BlockSize / 2, GameSettings::BlockSize / 2 };
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
	BombTexture(LoadTexture("resources/bonuses/bomb.png")),
	LineTexture(LoadTexture("resources/bonuses/line.png")),
	_state(nullptr)
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
		throw std::logic_error("No state for this turn");
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

void BoardState::DrawBonusIcon(BoardContext* context, shared_ptr<BlockData> block, Vector2 position)
{
	std::optional<Texture2D> texture = std::nullopt;
	float rotation = 0.0f;
	if (shared_ptr<LineBonus> line = dynamic_pointer_cast<LineBonus>(block->Bonus))
	{
		texture = context->LineTexture;
		if (!line->Vertical)
		{
			rotation = 90.0f;
		}
	} 
	else if (shared_ptr<BombBonus> line = dynamic_pointer_cast<BombBonus>(block->Bonus))
	{
		texture = context->BombTexture;
	}

	if (!texture.has_value()) return;

	float width = static_cast<float>(texture->width);
	float height = static_cast<float>(texture->height);
	Rectangle dest =
	{
		position.x + context->BlockOrigin.x,
		position.y + context->BlockOrigin.y,
		width,
		height
	};
	DrawTexturePro(texture.value(), {0, 0, width, height}, dest, context->BlockOrigin, rotation, WHITE);
}

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
		Vector2 position = (Point::FromIndex(i, GameSettings::GridSize) * 
			GameSettings::BlockSize + BoardContext::Padding).ToVector2();
		int type = _turn->Blocks[i]->Type;
		DrawTextureV(context->MarbleTextures[type], position, WHITE);
		DrawBonusIcon(context, _turn->Blocks[i], position);
	}

	if (!IsBlockSelected()) return;
	Point position = _selectedIndex * GameSettings::BlockSize + BoardContext::Padding;
	DrawTexture(context->FrameTexture, position.X, position.Y, WHITE);
}

bool IdleState::IsBlockSelected()
{
	return _selectedIndex.X != -1 || _selectedIndex.Y != -1;
}

CascadeState::CascadeState(unique_ptr<CascadeTurn>&& turn) : _turn(move(turn)) 
{
	for (const shared_ptr<Bonus>& bonus : _turn->Bonuses)
	{
		_bonuses.push_back(BonusAnimation::Create(bonus));
	}
}

void CascadeState::Update(const float time, BoardContext* context)
{
	float delta = GameSettings::AnimationSpeed * time;
	_localShrink = MoveTowards(_localShrink, TargetSizeShrink, delta);
	for (const int index : _turn->Dead)
	{
		_shrinkGrid[index] = _localShrink;
	}
	for (unique_ptr<BonusAnimation>& bonus : _bonuses) 
	{
		bonus->Update(time, _shrinkGrid);
	}

	if (IsOver()) context->NextTurn();
}

void CascadeState::Draw(BoardContext* context)
{
	for (int i = 0; i < _turn->Blocks.size(); i++)
	{
		Vector2 position = (Point::FromIndex(i, GameSettings::GridSize) * 
			GameSettings::BlockSize + BoardContext::Padding).ToVector2();
		int type = _turn->Blocks[i]->Type;
		float size = 1.0f - _shrinkGrid[i];
		Texture2D& texture = context->MarbleTextures[type];
		float width = static_cast<float>(texture.width);
		float height = static_cast<float>(texture.height);
		Rectangle dest = 
		{ 
			position.x + width - width * size / 2,
			position.y + height - height * size / 2,
			width * size, 
			height * size 
		};
		DrawTexturePro(texture,	{ 0, 0, width, height }, dest, context->BlockOrigin, 0.0f, WHITE);
		DrawBonusIcon(context, _turn->Blocks[i], position);
	}
	for (unique_ptr<BonusAnimation>& bonus : _bonuses)
	{
		bonus->Draw(context);
	}
}

bool CascadeState::IsOver()
{
	for (unique_ptr<BonusAnimation>& bonus : _bonuses)
	{
		if (!bonus->IsOver()) return false;
	}
	return abs(_localShrink - TargetSizeShrink) < FLT_EPSILON;
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
		DrawBonusIcon(context, _turn->Blocks[i], position);
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
		DrawBonusIcon(context, _turn->Blocks[i], position);
	}
}

bool BonusAnimation::IsOver()
{
	return _over;
}

Point BonusAnimation::GetTarget()
{
	return _target;
}

unique_ptr<BonusAnimation> BonusAnimation::Create(const shared_ptr<Bonus> bonus)
{
	if (const shared_ptr<LineBonus> line = dynamic_pointer_cast<LineBonus>(bonus)) {
		return make_unique<LineBonusAnimation>(line);
	}
	if (const shared_ptr<BombBonus> bomb = dynamic_pointer_cast<BombBonus>(bonus)) {
		return make_unique<BombBonusAnimation>(bomb);
	}
	throw std::logic_error("Bonus animation missing");
}

LineBonusAnimation::LineBonusAnimation(const shared_ptr<LineBonus>& bonus) : _data(bonus)
{
	for (shared_ptr<Bonus> child : _data->Chain)
	{
		_childs.push_back(BonusAnimation::Create(child));
	}
	_target = _data->Target;
}

void LineBonusAnimation::Update(float delta, array<float, GameSettings::NumberOfBlocks>& shrink)
{
	if (_over) return;
	if (MoveDestructor(delta))
	{
		Point target = GetTarget();
		Point destructor1 = target + _destructorPosition;
		Point destructor2 = target - _destructorPosition;
		ActivateChainedBonus(destructor1);
		ActivateChainedBonus(destructor2);
		StartShrinkingBlock(destructor1);
		StartShrinkingBlock(destructor2);
	}

	ShrinkBlocks(delta, shrink);
	for (int index : _activeChilds) 
	{
		_childs[index]->Update(delta, shrink);
	}
	_over = CalculateOver(shrink);
}

void LineBonusAnimation::Draw(BoardContext* context)
{
	if (_over) return;
	DrawDestructor(context, _destructorPosition, _destructorDisplacement);
	DrawDestructor(context, -_destructorPosition, -_destructorDisplacement);

	for (int index : _activeChilds)
	{
		_childs[index]->Draw(context);
	}
}

void LineBonusAnimation::DrawDestructor(BoardContext* context, Point position, float displacement)
{
	float rotation = _data->Vertical ? 0.0f : 90.0f;
	Point blockIndex = GetTarget() + position;
	Vector2 screenPosition = (blockIndex * GameSettings::BlockSize + BoardContext::Padding).ToVector2();
	screenPosition += _data->Vertical ? Vector2(0, displacement) : Vector2(displacement, 0);
	Texture2D& texture = context->LineTexture;
	float width = static_cast<float>(texture.width);
	float height = static_cast<float>(texture.height);
	Rectangle dest =
	{
		screenPosition.x + context->BlockOrigin.x,
		screenPosition.y + context->BlockOrigin.y,
		width,
		height
	};
	DrawTexturePro(texture, { 0, 0, width, height }, dest, context->BlockOrigin, rotation, WHITE);
}

bool LineBonusAnimation::MoveDestructor(float delta)
{
	_destructorDisplacement += delta * GameSettings::BlockSize * 2.0f * GameSettings::AnimationSpeed;
	if (_destructorDisplacement > GameSettings::BlockSize)
	{
		_destructorDisplacement -= GameSettings::BlockSize;
		if (_data->Vertical)
			_destructorPosition.Y++;
		else
			_destructorPosition.X++;
		return true;
	}
	return false;
}

void LineBonusAnimation::ActivateChainedBonus(Point target)
{
	for (int i = 0; i < _childs.size(); i++) {
		if (_childs[i]->GetTarget() == target) {
			_activeChilds.push_back(i);
			return;
		}
	}
}

void LineBonusAnimation::StartShrinkingBlock(Point point)
{
	if (std::count(_data->Dead.begin(), _data->Dead.end(), point.ToIndex(GameSettings::GridSize)))
		_activeDead.push_back(point.ToIndex(GameSettings::GridSize));
}

void LineBonusAnimation::ShrinkBlocks(float time, array<float, GameSettings::NumberOfBlocks>& shrink)
{
	float delta = time * GameSettings::AnimationSpeed;

	for (int index : _activeDead)
	{
		float value = shrink[index];
		shrink[index] = MoveTowards(value, CascadeState::TargetSizeShrink, delta);
	}
}

bool LineBonusAnimation::CalculateOver(const array<float, GameSettings::NumberOfBlocks>& shrink)
{
	for (const unique_ptr<BonusAnimation>& child : _childs) {
		if (!child->IsOver()) return false;
	}
	for (int index : _data->Dead)
	{
		if (!(abs(shrink[index] - CascadeState::TargetSizeShrink) < FLT_EPSILON)) return false;
	}
	return true;
}

BombBonusAnimation::BombBonusAnimation(const shared_ptr<BombBonus>& bonus) : _data(bonus)
{
	for (shared_ptr<Bonus> child : _data->Chain)
	{
		_childs.push_back(BonusAnimation::Create(child));
	}
	_target = _data->Target;
}

void BombBonusAnimation::Update(float delta, array<float, GameSettings::NumberOfBlocks>& shrink)
{
	if (_over) return;
	if (_detonateTime > 0)
	{
		_detonateTime -= delta;
	}
	else
	{
		ShrinkBlocks(delta, shrink);
		for (unique_ptr<BonusAnimation>& animation : _childs)
		{
			animation->Update(delta, shrink);
		}
		_over = CalculateOver(shrink);
	}
}

void BombBonusAnimation::Draw(BoardContext* context)
{
	for (unique_ptr<BonusAnimation>& animation : _childs)
	{
		animation->Draw(context);
	}
}

void BombBonusAnimation::ShrinkBlocks(float time, array<float, GameSettings::NumberOfBlocks>& shrink)
{
	float delta = time * GameSettings::AnimationSpeed;
	_shrinkSize = MoveTowards(_shrinkSize, CascadeState::TargetSizeShrink, delta);
	for (int index : _data->Dead) 
	{
		shrink[index] = _shrinkSize;
	}
}

bool BombBonusAnimation::CalculateOver(const array<float, GameSettings::NumberOfBlocks>& shrink)
{
	for (const unique_ptr<BonusAnimation>& child : _childs) {
		if (!child->IsOver()) return false;
	}
	for (int index : _data->Dead)
	{
		if (!(abs(shrink[index] - CascadeState::TargetSizeShrink) < FLT_EPSILON)) return false;
	}
	return true;
}
