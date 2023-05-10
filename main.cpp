#include "raylib.h"
#include "GlobalStateContext.h"
#include "GameSettings.h"

int main(void)
{
	InitWindow(GameSettings::Width, GameSettings::Height, "Horizon3Raylib");
	GlobalStateContext game;
	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
		// Update
		//----------------------------------------------------------------------------------
		const float deltaTime = GetFrameTime();
		game.Update(deltaTime);
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();
		ClearBackground(LIGHTGRAY);
		game.Draw();
		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	CloseWindow();

	return 0;
}