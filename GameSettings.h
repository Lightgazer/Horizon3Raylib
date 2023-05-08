#pragma once
class GameSettings
{
public:
	static const int Width = 800;
	static const int Height = 600;
	static const int BlockSize = 64;
	static const int NumberOfMarbleTypes = 5;
	static const int GridSize = 8;
	static const int NumberOfBlocks = GridSize * GridSize;
	static constexpr float AnimationSpeed = 3.0f;
};

