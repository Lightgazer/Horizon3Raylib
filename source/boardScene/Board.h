#pragma once

#include <memory>
#include <array>
#include "raylib.h"
#include "GameSettings.h"
#include "utils/Geometry.h"
#include "model/GameModel.h"
#include "model/BlockData.h"

using namespace std;

class BoardContext;
class BoardState;
class IdleState;
class CascadeState;
class DropState;
class SwapState;
class BonusAnimation;

/// <summary>
/// Игровое поле реализовано паттерном состояние. Состояния сами решают когда они
/// заканчиваются вызывая NextTurn у контекста. Контекст используя модель
/// выбирает следующие состояние.
/// </summary>
class BoardContext
{
public:
	static Point Padding;
	static Vector2 BlockOrigin;
	static IntRect GridRectangle;

	array<Texture2D, GameSettings::NumberOfMarbleTypes> MarbleTextures;
	Texture2D FrameTexture;
	Texture2D BombTexture;
	Texture2D LineTexture;

	GameModel Model;

private:
	static const int SideLength = GameSettings::GridSize * GameSettings::BlockSize;

	unique_ptr<BoardState> _state;

public:
	BoardContext();
	void NextTurn();
	void Update(const float delta);
	void Draw();
};

class BoardState
{
public:
	virtual ~BoardState() = 0;
	virtual void Update(const float delta, BoardContext* context) = 0;
	virtual void Draw(BoardContext* context) = 0;

protected:
	void DrawBonusIcon(BoardContext* context, shared_ptr<BlockData> block, Vector2 position);
};

inline BoardState::~BoardState() {}

/// <summary>
/// В этом состоянии игра ждёт когда игрок выберет два блока для свапа.
/// </summary>
class IdleState : virtual public BoardState
{
private:
	unique_ptr<IdleTurn> _turn;
	bool _wasButtonPressed = false;
	Point _selectedIndex = Point(-1, -1);

public:
	IdleState(unique_ptr<IdleTurn>&& turn);
	void Update(const float delta, BoardContext* context) override;
	void Draw(BoardContext* context) override;

private:
	bool IsBlockSelected();
};

/// <summary>
/// В этом состоянии игра отображает анимацию уменьшения блоков попавших в матч 
/// и анимацию бонусов. 
/// </summary>
class CascadeState : virtual public BoardState
{
public:
	static constexpr float TargetSizeShrink = 0.8f;

private:
	unique_ptr<CascadeTurn> _turn;
	vector<unique_ptr<BonusAnimation>> _bonuses = vector<unique_ptr<BonusAnimation>>();
	array<float, GameSettings::NumberOfBlocks> _shrinkGrid = array<float, GameSettings::NumberOfBlocks>();
	float _localShrink = 0;

public:
	CascadeState(unique_ptr<CascadeTurn>&& turn);
	void Update(const float delta, BoardContext* context) override;
	void Draw(BoardContext* context) override;

private:
	bool IsOver();
};

/// <summary>
/// В этом состоянии игра отображает анимацию падения блоков на пустые места.
/// </summary>
class DropState : virtual public BoardState
{
private:
	unique_ptr<DropTurn> _turn;
	float _displacement = 0;

public:
	DropState(unique_ptr<DropTurn>&& turn);
	void Update(const float delta, BoardContext* context) override;
	void Draw(BoardContext* context) override;
};

/// <summary>
/// В этом состоянии игра показывает анимацию где два блока меняются местами.
/// </summary>
class SwapState : virtual public BoardState
{
private:
	Vector2 _direction;
	unique_ptr<SwapTurn> _turn;
	float _displacement = -GameSettings::BlockSize;

public:
	SwapState(unique_ptr<SwapTurn>&& turn);
	void Update(const float delta, BoardContext* context) override;
	void Draw(BoardContext* context) override;
};

class BonusAnimation
{
protected:
	bool _over = false;
	Point _target;

public:
	virtual ~BonusAnimation() = 0;
	virtual void Update(float delta, array<float, GameSettings::NumberOfBlocks>& shrink) = 0;
	virtual void Draw(BoardContext* context) = 0;
	bool IsOver();
	Point GetTarget();

	static unique_ptr<BonusAnimation> Create(const shared_ptr<Bonus> bonus);
};

inline BonusAnimation::~BonusAnimation() { }

class LineBonusAnimation : public BonusAnimation
{
private:
	const shared_ptr<LineBonus> _data;
	vector<unique_ptr<BonusAnimation>> _childs = vector<unique_ptr<BonusAnimation>>();
	vector<int> _activeChilds = vector<int>();
	vector<int> _activeDead = vector<int>();
	float _destructorDisplacement = 0;
	Point _destructorPosition{ 0 };

public:
	LineBonusAnimation(const shared_ptr<LineBonus>& bonus);
	void Update(float delta, array<float, GameSettings::NumberOfBlocks>& shrink) override;
	void Draw(BoardContext* context) override;

private:
	void DrawDestructor(BoardContext* context, Point position, float displacement);
	bool MoveDestructor(float delta);
	void ActivateChainedBonus(Point index);
	void StartShrinkingBlock(Point index);
	void ShrinkBlocks(float delta, array<float, GameSettings::NumberOfBlocks>& shrink);
	bool CalculateOver(const array<float, GameSettings::NumberOfBlocks>& shrink);
};

class BombBonusAnimation : public BonusAnimation
{
private:
	const shared_ptr<BombBonus> _data;
	vector<unique_ptr<BonusAnimation>> _childs = vector<unique_ptr<BonusAnimation>>();
	float _detonateTime = 0.25f;
	float _shrinkSize = 0.0f;

public:
	BombBonusAnimation(const shared_ptr<BombBonus>& bonus);
	void Update(float delta, array<float, GameSettings::NumberOfBlocks>& shrink) override;
	void Draw(BoardContext* context) override;
	void ShrinkBlocks(float delta, array<float, GameSettings::NumberOfBlocks>& shrink);

private:
	bool CalculateOver(const array<float, GameSettings::NumberOfBlocks>& shrink);
};
