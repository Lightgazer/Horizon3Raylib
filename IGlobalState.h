#pragma once

class IGlobalState
{
public:
	virtual void Start() = 0;
	virtual void Update(const float delta) = 0;
	virtual void Draw() = 0;
};
