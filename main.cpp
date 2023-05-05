#include "raylib.h"
#include "GameScreen.h"
#include "GameSettings.h"

int main(void)
{
	InitWindow(GameSettings::Width, GameSettings::Height, "Horizon3Raylib");
	GameScreen gameScreen;
	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
		// Update
		//----------------------------------------------------------------------------------
		const float deltaTime = GetFrameTime();
		gameScreen.Update(deltaTime);
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();
		ClearBackground(LIGHTGRAY);
		gameScreen.Draw();
		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	CloseWindow();

	return 0;
}