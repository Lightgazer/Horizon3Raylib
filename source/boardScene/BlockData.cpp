#include <array>
#include "BlockData.h"
#include "GameSettings.h"
#include "boolinq/boolinq.h"

int Bonus::Execute(const array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& blocks)
{
	Dead.clear();
	Chain.clear();
	_score = 0;
	FindTarget([&](Point target) { OnEachTarget(target, blocks); });

	_score += boolinq::from(Chain)
		.select([&blocks](shared_ptr<Bonus> bonus) { return bonus->Execute(blocks); }).sum();
	return _score;
}

void Bonus::OnEachTarget(Point target, const array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& blocks)
{
	auto x = [](Point point) {};
	if (CheckTarget(target, blocks))
	{
		_score++;
		int i = target.ToIndex(GameSettings::GridSize);
		blocks[i]->Alive = false;
		Dead.push_back(i);
		if (blocks[i]->Bonus) Chain.push_back(blocks[i]->Bonus);
	}
}

bool Bonus::CheckTarget(Point index, const array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& blocks)
{
	return index.X >= 0
		&& index.X < GameSettings::GridSize
		&& index.Y >= 0
		&& index.Y < GameSettings::GridSize
		&& blocks[index.ToIndex(GameSettings::GridSize)]->Alive;
}

LineBonus::LineBonus(bool vertical) : Vertical(vertical) { }

void LineBonus::FindTarget(const function<void(Point)>& callback)
{
	int line = Vertical ? Target.X : Target.Y;
	for (int i = 0; i < GameSettings::GridSize; i++)
	{
		Point target = Vertical ? Point(line, i) : Point(i, line);
		callback(target);
	}
}

void BombBonus::FindTarget(const function<void(Point)>& callback)
{
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			Point target(Target.X + x, Target.Y + y);
			callback(target);
		}
	}
}
