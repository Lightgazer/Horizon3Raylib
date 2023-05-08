#pragma once

#include <memory>
#include <array>
#include "raylib.h"
#include "GameSettings.h"
#include "Geometry.h"
#include "GameModel.h"

using namespace std;

class BoardState;
class IdleState;
class BoardContext;

class BoardState
{
public:
	virtual ~BoardState() = 0;
	virtual void Update(const float delta, BoardContext* context) = 0;
	virtual void Draw(BoardContext* context) = 0;
};

inline BoardState::~BoardState() {}

/// <summary>
/// � ���� ��������� ���� ��� ����� ����� ������� ��� ����� ��� �����.
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
/// � ���� ��������� ���� ���������� �������� ���������� ������ �������� � ���� 
/// � �������� �������. 
/// </summary>
class CascadeState : virtual public BoardState
{
public:
	static constexpr float TargetSizeShrink = 0.8f;

private:
	unique_ptr<CascadeTurn> _turn;
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
/// � ���� ��������� ���� ���������� �������� ������� ������ �� ������ �����.
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
/// � ���� ��������� ���� ���������� �������� ��� ��� ����� �������� �������.
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

/// <summary>
/// ������� ���� ����������� ��������� ���������. ��������� ���� ������ ����� ���
/// ������������� ������� NextTurn � ���������. �������� ��������� ������
/// �������� ��������� ���������.
/// </summary>
class BoardContext
{
public:
	static Point Padding;
	static Point BlockOrigin;
	static IntRect GridRectangle;

	array<Texture2D, GameSettings::NumberOfMarbleTypes> MarbleTextures;
	Texture2D FrameTexture;

	GameModel Model;

private:
	static const int SideLength = GameSettings::GridSize * GameSettings::BlockSize;

	unique_ptr<BoardState> _state;
	Texture2D _bombTexture;
	Texture2D _lineTexture;

public:
	BoardContext();
	void NextTurn();
	void Update(const float delta);
	void Draw();
};

