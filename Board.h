#pragma once

#include <memory>
#include <array>
#include "raylib.h"
#include "GameSettings.h"
#include "Point.h"

using namespace std;

class IdleTurn;
class BoardState;
class IdleState;
class BoardContext;

struct BlockData
{
	int Type;
};

class IdleTurn
{
public:
	array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks> Blocks;

	//Temp constructor
	IdleTurn();
};

class BoardState
{
public:
	virtual void Update(const float& delta, BoardContext* context) = 0;
	virtual void Draw(BoardContext* context) = 0;
};

/// <summary>
/// В этом состоянии игра ждёт когда игрок выберет два блока для свапа.
/// </summary>
class IdleState : public BoardState
{
private:
	IdleTurn _turn;
	bool _wasButtonPressed = false;
	Point _selectedIndex = Point(-1, -1);

public:
	void Update(const float& delta, BoardContext* context) override;
	void Draw(BoardContext* context) override;

private:
	bool IsBlockSelected();
};


/// <summary>
/// Игровое поле реализовано паттерном состояние. Состояния сами решают когда они
/// заканчиваются вызывая NextTurn у контекста. Контекст используя модель
/// выбирает следующие состояние.
/// </summary>
class BoardContext
{
private:
	static const int SideLength = GameSettings::GridSize * GameSettings::BlockSize;

	unique_ptr<BoardState> _state;
	Texture2D _bombTexture;
	Texture2D _lineTexture;

public:
	static Point Padding;
	static Point BlockOrigin;
	static IntRect GridRectangle;

	array<Texture2D, GameSettings::NumberOfMarbleTypes> MarbleTextures;
	Texture2D FrameTexture;

	BoardContext();
	void NextTurn();
	void Update(const float& delta);
	void Draw();
};

