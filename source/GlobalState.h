#pragma once

class GlobalState
{
private:
	bool _isNextStateRequired = false;

public:
	virtual ~GlobalState() = 0;
	virtual void ReStart() = 0;
	virtual void Update(const float delta) = 0;
	virtual void Draw() = 0;
	void RequestNextState();
	bool ConsumeStateFlag();
};

inline GlobalState::~GlobalState() {}
