#include <memory>
#include "lib/boolinq/include/boolinq/boolinq.h"
#include "GameModel.h"

GameModel::GameModel() : _state(Matches)
{
	for (shared_ptr<BlockData>& block : _blocks) {
		if (!block) {
			block.reset(new BlockData{ GetRandomValue(0, GameSettings::NumberOfMarbleTypes - 1) });
		}
	}
}

unique_ptr<ITurn> GameModel::GetNextTurn()
{
	switch (_state)
	{
	case Matches:
		vector<shared_ptr<MatchChain>>&& matches = FindMatches();
		if (matches.size() > 0) {
			//var bonuses = CollectBonuses(matches);
			_score += ExecuteMatches(matches);
			vector<shared_ptr<Point>>&& deadFromMatch = CollectDead();
			//Score += ExecuteBonuses(bonuses);
		}
		break;
	}

	return make_unique<IdleTurn>(_blocks);
}

bool GameModel::SwapBlocks(Point& first, Point& second)
{
	if (IsSwapAllowed(first, second)) {
		const int& index1 = first.ToIndex(GameSettings::GridSize);
		const int& index2 = second.ToIndex(GameSettings::GridSize);
		shared_ptr<BlockData> block1 = _blocks[index1];
		shared_ptr<BlockData> block2 = _blocks[index2];
		_blocks[index1] = block2;
		_blocks[index2] = block1;
		_swap.reset(new SwapInfo(first, second));
		return true;
	}
	return false;
}

bool GameModel::IsSwapAllowed(Point& first, Point& second)
{
	if (first.X == second.X)
		return abs(first.Y - second.Y) == 1;
	if (first.Y == second.Y)
		return abs(first.X - second.X) == 1;
	return false;
}

vector<shared_ptr<MatchChain>> GameModel::FindMatches()
{
	vector<shared_ptr<MatchChain>> result;
	FindMatches(result, true);
	FindMatches(result, false);
	return result;
}

vector<shared_ptr<Point>> GameModel::CollectDead()
{
	vector<shared_ptr<Point>> result;
	for (int i = 0; i < _blocks.size(); i++) {
		if (!_blocks[i]->Alive) {
			result.push_back(make_shared<Point>(Point::FromIndex(i, GameSettings::GridSize)));
		}
	}
	return result;
}

void GameModel::FindMatches(vector<shared_ptr<MatchChain>>& result, bool vertical)
{
	for (int k = 0; k < GameSettings::GridSize; k++)
	{
		int currentType = -1;
		shared_ptr<MatchChain> matchChain = make_shared<MatchChain>(vertical);
		for (int j = 0; j < GameSettings::GridSize; j++)
		{
			Point point = vertical ? Point(k, j) : Point(j, k);
			shared_ptr<BlockData> block = _blocks[point.ToIndex(GameSettings::GridSize)];
			// if (block.Bonus is { }) block.Bonus.Target = new Point(x, y);
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

			if (matchChain->HasEnoughLinks()) result.push_back(matchChain);
		}
	}
}

int GameModel::ExecuteMatches(vector<shared_ptr<MatchChain>>& matches)
{
	return boolinq::from(matches).sum([](shared_ptr<MatchChain> match) { return match->Execute(); });
}

IdleTurn::IdleTurn(array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& blocks)
{
	Blocks = blocks;
}

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
	for (shared_ptr<BlockData> block : Blocks) {
		//Bonus bonus = null;
		//if (!block.Alive)
		//{
		//	bonus = new BombBonus();
		//}
		//else if (block.Suspect && Blocks.Count > 3)
		//{
		//	if (Blocks.Count > 4)
		//		bonus = new BombBonus();
		//	else
		//		bonus = new LineBonus(Vertical);
		//}

		block->Alive = false;
		//block.Bonus = bonus;
	}
	return 0;
}
