#pragma once

#include <array>
#include <vector>
#include <unordered_set>
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
	int First;
	int Second;

	SwapInfo(const int first = 0, const int second = 0) : First(first), Second(second) { }
};

class ITurn {
public:
	virtual ~ITurn() = 0;
};

inline ITurn::~ITurn() {}

class IdleTurn : virtual public ITurn
{
public:
	array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& Blocks;

	IdleTurn(array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& blocks);
};

class CascadeTurn : virtual public ITurn
{
public:
	array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& Blocks;
	vector<int>& Dead;

	CascadeTurn(array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& blocks, vector<int>& dead);
};

class DropTurn : virtual public ITurn
{
public:
	array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& Blocks;
	/// <summary> —писок блоков которые падают в текущем раунде. </summary>
	unordered_set<int>& Drop;

	DropTurn(array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& blocks, unordered_set<int>& drop);
};

class SwapTurn : virtual public ITurn
{
public:
	array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& Blocks;
	int First;
	int Second;

	SwapTurn(array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& blocks, int first, int second);
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
	BackSwap
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
	ModelState _state = Matches;
	int _score = 0;
	vector<shared_ptr<MatchChain>> _matches;
	vector<int> _deadFromMatch;
	unordered_set<int> _dropList;

public:
	GameModel();
	unique_ptr<ITurn> GetNextTurn();
	bool SwapBlocks(const int first, const int second);

private:
	static bool IsSwapAllowed(const int first, const int second);
	void FindMatches(vector<shared_ptr<MatchChain>>& matches);
	void CollectDead(vector<int>&);
	void FindMatches(vector<shared_ptr<MatchChain>>& result, const bool vertical);
	///<summary>Ёлементы папавшие в матч тер€ют статус живых</summary>
	///<returns>ќчки за исполнение матчей, с учЄтом их возможных пересечений</returns>
	int ExecuteMatches(vector<shared_ptr<MatchChain>>& matches);
	void CreateBlocksInFirstRow();
	void MakeDropList(unordered_set<int>& dropList);
	void DropBlocks(unordered_set<int>& dropList);
	void ReleaseSuspects();
	void ReturnSwapedBlocks();
};