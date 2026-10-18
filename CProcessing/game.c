#include "cprocessing.h"
#include "game.h"
#include "math.h"

// Colours
#define BG_COLOUR CP_Color_Create(247, 252, 252, 255) // Background

#define SANDWICH_SIZE 5

CP_Image pbread;
CP_Image tomatoS;
int sandwichOrder = 0; // order in which sandwich will be drawn
int sandwich[] = { 0, 0, 0, 0, 0}; // for storing which ingredient value

void Game_Init(void)
{
	pbread = CP_Image_Load("Assets/bread.png");
	tomatoS = CP_Image_Load("Assets/tomato.png");
}

void Game_Update(void)
{
	CP_Graphics_ClearBackground(BG_COLOUR);
	CP_Image_Draw(pbread, (float)CP_System_GetWindowWidth() / 5.0f, (float)CP_System_GetWindowHeight() / 2.0f, 200.0f, 200.0f, 255);


	CP_Image ingredients[] = { pbread, tomatoS }; // array for storing the ref to actual ingredient images

	if (CP_Input_KeyTriggered(KEY_B)) // B for bread
	{
		sandwich[sandwichOrder] = 0;
		sandwichOrder++;
	}

	if (CP_Input_KeyTriggered(KEY_T)) // T for tomato
	{
		sandwich[sandwichOrder] = 1;
		sandwichOrder++;
	}

	if (CP_Input_KeyTriggered(KEY_BACKSPACE)) // Remove ingredient
	{
		sandwichOrder--;
	}

	sandwichOrder = CP_Math_ClampInt(sandwichOrder, 0, SANDWICH_SIZE); // This clamps sandwich size to 5 for now
	for (int i = 0; i < sandwichOrder; i++)
	{
		CP_Image_Draw(ingredients[sandwich[i]], (float)CP_System_GetWindowWidth() / 2.0f, (float)CP_System_GetWindowHeight() * 0.75f - (20.0f * i), 200.0f, 200.0f, 255);
	}
}

void Game_Exit(void)
{
	CP_Image_Free(pbread);
	CP_Image_Free(tomatoS);
}