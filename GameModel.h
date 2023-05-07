#pragma once

#include <array>
#include <vector>
#include "GameSettings.h"
#include "Geometry.h"

using namespace std;

struct BlockData
{
	int Type;
	bool Alive = true;
};

struct SwapInfo
{
	Point First;
	Point Second;

	SwapInfo(Point first, Point second) : First(first), Second(second) { }
};

class ITurn {
public:
	virtual ~ITurn() = 0;
};

inline ITurn::~ITurn() {}

class IdleTurn : virtual public ITurn
{
public:
	array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks> Blocks;
	IdleTurn(array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& blocks);
};

class MatchChain {
public:
	vector<shared_ptr<BlockData>> Blocks;
	bool Vertical;

	MatchChain(bool vertical);
	void Add(shared_ptr<BlockData> block);
	bool HasEnoughLinks();
	int Execute();
};

enum ModelState {
	Matches,
	Drop,
	Idle,
	Swap,
};

/// <summary>
/// ѕассивна€ модель. ћодель отвечает исполнение правил игры. »гра представлена 
/// поледовательностью раундов разного типа, генерируемых моделью.
/// </summary>
class GameModel
{
private:
	array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks> _blocks;
	unique_ptr<SwapInfo> _swap;
	ModelState _state;
	int _score = 0;

public:
	GameModel();
	unique_ptr<ITurn> GetNextTurn();
	bool SwapBlocks(Point& first, Point& second);

private:
	static bool IsSwapAllowed(Point& first, Point& second);
	vector<shared_ptr<MatchChain>> FindMatches();
	vector<shared_ptr<Point>> CollectDead();
	void FindMatches(vector<shared_ptr<MatchChain>>& result, bool vertical);
	///<summary>Ёлементы папавшие в матч тер€ют статус живых</summary>
	///<returns>ќчки за исполнение матчей, с учЄтом их возможных пересечений</returns>
	int ExecuteMatches(vector<shared_ptr<MatchChain>>& matches);
};