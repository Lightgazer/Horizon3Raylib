#pragma once

#include <memory>
#include <vector>
#include <functional>
#include "Geometry.h"
#include "GameSettings.h"

using namespace std;

struct BlockData;
class Bonus;
class LineBonus;
class BombBonus;

struct BlockData
{
	int Type;
	bool Alive = true;
	shared_ptr<Bonus> Bonus;
	/// <summary>
	/// Флаг которым пемечается последний передвинутый блок, на его месте может возникнуть бонус.
	/// </summary>
	bool Suspect;
};

class Bonus
{
public:
	Point Target;
	vector<int> Dead;
	vector<shared_ptr<Bonus>> Chain;

private:
	int _score;

public:
	virtual ~Bonus() = 0;
	int Execute(const array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& blocks);

protected:
	virtual void FindTarget(const function<void(Point)>& callback) = 0;

private:
	void OnEachTarget(Point target, const array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& blocks);
	bool CheckTarget(Point index, const array<shared_ptr<BlockData>, GameSettings::NumberOfBlocks>& blocks);
};

inline Bonus::~Bonus() {}

class LineBonus : public Bonus
{
public:
	const bool Vertical;

public:
	LineBonus(bool vertical);

protected:
	void FindTarget(const function<void(Point)>& callback) override;
};

class BombBonus : public Bonus
{
protected:
	void FindTarget(const function<void(Point)>& callback) override;
};