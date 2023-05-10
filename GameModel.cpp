#include <memory>
#include "lib/boolinq/include/boolinq/boolinq.h"
#include "GameModel.h"

GameModel::GameModel()
{
	for (shared_ptr<BlockData>& block : _blocks)
	{
		if (!block)
		{
			block.reset(new BlockData{ GetRandomValue(0, GameSettings::NumberOfMarbleTypes - 1) });
		}
	}
	FindMatches(_matches);
}

unique_ptr<ITurn> GameModel::GetNextTurn()
{
	switch (_state)
	{
	case Matches:
		if (_matches.size() > 0)
		{
			vector<shared_ptr<Bonus>>&& bonuses = CollectBonuses(_matches);
			_score += ExecuteMatches(_matches);
			_matches.clear();
			CollectDead(_deadFromMatch);
			_score += ExecuteBonuses(bonuses);
			_state = Drop;
			return make_unique<CascadeTurn>(_blocks, bonuses, _deadFromMatch);
		}
		_swap.reset();
		_state = Idle;
		return GetNextTurn();
	case Drop:
		RestoreBonusBlocks(_deadFromMatch);
		_deadFromMatch.clear();
		DropBlocks(_dropList);
		CreateBlocksInFirstRow();
		_dropList.clear();
		MakeDropList(_dropList);
		if (_dropList.size() > 0) 
		{
			return make_unique<DropTurn>(_blocks, _dropList);
		}
		FindMatches(_matches);
		_swap.reset();
		_state = _matches.size() > 0 ? Matches : Idle;
		return GetNextTurn();
	case Idle:
		if (_swap) 
		{
			_state = Swap;
			return GetNextTurn();
		}
		ReleaseSuspects();
		FindMatches(_matches);
		if (_matches.size() > 0) {
			_state = Matches;
			return GetNextTurn();
		}
		return make_unique<IdleTurn>(_blocks);
	case Swap:
		_state = BackSwap;
		return make_unique<SwapTurn>(_blocks, _swap->First, _swap->Second);
	case BackSwap:
		FindMatches(_matches);
		_state = _matches.size() > 0 ? Matches : Idle;
		if (_state == Matches) 
		{
			_swap.reset();
			return GetNextTurn();
		}
		ReturnSwapedBlocks();
		int first = _swap->First;
		int second = _swap->Second;
		_swap.reset();
		return make_unique<SwapTurn>(_blocks, second, first);
	}

	return make_unique<IdleTurn>(_blocks);
}

bool GameModel::SwapBlocks(const int first, const int second)
{
	if (IsSwapAllowed(first, second))
	{
		shared_ptr<BlockData> block1 = _blocks[first];
		shared_ptr<BlockData> block2 = _blocks[second];
		block1->Suspect = true;
		block2->Suspect = true;
		_blocks[first] = block2;
		_blocks[second] = block1;
		_swap.reset(new SwapInfo(first, second));
		return true;
	}
	return false;
}

int GameModel::GetScore()
{
	return _score;
}

bool GameModel::IsSwapAllowed(const int index1, const int index2)
{
	Point first = Point::FromIndex(index1, GameSettings::GridSize);
	Point second = Point::FromIndex(index2, GameSettings::GridSize);
	if (first.X == second.X)
		return abs(first.Y - second.Y) == 1;
	if (first.Y == second.Y)
		return abs(first.X - second.X) == 1;
	return false;
}

void GameModel::FindMatches(vector<shared_ptr<MatchChain>>& matches)
{
	FindMatches(matches, true);
	FindMatches(matches, false);
}

void GameModel::CollectDead(vector<int>& result)
{
	for (int i = 0; i < _blocks.size(); i++)
	{
		if (!_blocks[i]->Alive)
		{
			result.push_back(i);
		}
	}
}

void GameModel::FindMatches(vector<shared_ptr<MatchChain>>& result, const bool vertical)
{
	for (int k = 0; k < GameSettings::GridSize; k++)
	{
		int currentType = -1;
		shared_ptr<MatchChain> matchChain = make_shared<MatchChain>(vertical);
		for (int j = 0; j < GameSettings::GridSize; j++)
		{
			Point point = vertical ? Point(k, j) : Point(j, k);
			shared_ptr<BlockData> block = _blocks[point.ToIndex(GameSettings::GridSize)];
			if (block->Bonus) block->Bonus->Target = point;
			if (currentType == block->Type)
			{
				matchChain->Add(block);
			}
			else
			{
				if (matchChain->HasEnoughLinks()) result.push_back(matchChain);
				currentType = block->Type;
				matchChain = make_shared<MatchChain>(vertical);
				matchChain->Add(block);
			}
		}

		if (matchChain->HasEnoughLinks()) result.push_back(matchChain);
	}
}

int GameModel::ExecuteMatches(vector<shared_ptr<MatchChain>>& matches)
{
	int result = 0;
	for (shared_ptr<MatchChain>& match : matches)
	{
		result += match->Execute();
	}
	return result;
}

void GameModel::CreateBlocksInFirstRow()
{
	for (int x = 0; x < GameSettings::GridSize; x++)
	{
		if (!_blocks[x] || _blocks[x]->Alive == false)
		{
			_blocks[x].reset(new BlockData{ GetRandomValue(0, GameSettings::NumberOfMarbleTypes - 1) });
		}
	}
}

void GameModel::MakeDropList(unordered_set<int>& dropList)
{
	for (int i = 0; i < _blocks.size(); i++)
	{
		if (!_blocks[i]->Alive) {
			int dropIndex = i;
			while ((dropIndex -= GameSettings::GridSize) >= 0)
			{
				dropList.insert(dropIndex);
			}
		}
	}
}

void GameModel::DropBlocks(unordered_set<int>& dropList)
{
	unordered_map<int, shared_ptr<BlockData>> dropMap(dropList.size());
	for (const int index : dropList)
	{
		dropMap[index] = move(_blocks[index]);
		if (!dropMap[index]) {
			auto x = 1;
		}
	}

	for (auto& [index, block] : dropMap) 
	{
		int lowerIndex = index + GameSettings::GridSize;
		_blocks[lowerIndex] = move(block);
	}
}

void GameModel::ReleaseSuspects()
{
	for (shared_ptr<BlockData>& block : _blocks) {
		block->Suspect = false;
	}
}

void GameModel::ReturnSwapedBlocks()
{
	shared_ptr<BlockData> block1 = _blocks[_swap->First];
	shared_ptr<BlockData> block2 = _blocks[_swap->Second];
	_blocks[_swap->First] = block2;
	_blocks[_swap->Second] = block1;
}

vector<shared_ptr<Bonus>> GameModel::CollectBonuses(vector<shared_ptr<MatchChain>>& matches)
{
	return boolinq::from(matches)
		.selectMany([](shared_ptr<MatchChain> chain) { return boolinq::from(chain->Blocks); })
		.where([](shared_ptr<BlockData> block) { return bool(block->Bonus); })
		.select([](shared_ptr<BlockData> block) { return block->Bonus; })
		.toStdVector();
}

int GameModel::ExecuteBonuses(vector<shared_ptr<Bonus>>& bonuses)
{
	int result = 0;
	for (shared_ptr<Bonus>& bonus : bonuses)
	{
		result += bonus->Execute(_blocks);
	}
	return result;
}

void GameModel::RestoreBonusBlocks(vector<int>& dead)
{
	for(int index : dead)
	{
		if (_blocks[index]->Bonus) _blocks[index]->Alive = true;
	}
}

IdleTurn::IdleTurn(array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& blocks) : Blocks(blocks) { }

CascadeTurn::CascadeTurn(
	array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& blocks, 
	vector<shared_ptr<Bonus>> bonuses,
	vector<int>& dead
) : Blocks(blocks), Bonuses(bonuses), Dead(dead) {}

DropTurn::DropTurn(array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& blocks, unordered_set<int>& drop)
	: Blocks(blocks), Drop(drop) {}

SwapTurn::SwapTurn(array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& blocks, int first, int second)
	: Blocks(blocks), First(first), Second(second) {}

MatchChain::MatchChain(bool vertical) : Vertical(vertical) {}

void MatchChain::Add(shared_ptr<BlockData> block)
{
	Blocks.push_back(block);
}

bool MatchChain::HasEnoughLinks()
{
	return Blocks.size() > 2;
}

int MatchChain::Execute()
{
	int score = boolinq::from(Blocks).count([](shared_ptr<BlockData> block) { return block->Alive; });
	for (shared_ptr<BlockData>& block : Blocks)
	{
		if (!block->Alive)
		{
			block->Bonus.reset(new BombBonus());
		}
		else if (block->Suspect && Blocks.size() > 3)
		{
			if (Blocks.size() > 4)
				block->Bonus.reset(new BombBonus());
			else
				block->Bonus.reset(new LineBonus(Vertical));
		}
		else 
		{
			block->Bonus.reset();
		}

		block->Alive = false;
	}
	return score;
}
