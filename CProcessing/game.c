#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cprocessing.h"
#include "game.h"
#include "math.h"

// Assets
CP_Font myFont;
CP_Image pbread;
CP_Image tomatoS;
CP_Image lettuce;
CP_Image cheese;
CP_Image ham;

// Colours
#define BG_COLOUR CP_Color_Create(247, 252, 252, 255) // Background

#define SANDWICH_SIZE 5

int sandwichOrder = 0; // order in which sandwich will be drawn
int sandwich[SANDWICH_SIZE] = { 0, 0, 0, 0, 0}; // for storing which ingredient value

char sandwich_ingredients[5][20] = { "Bread", "Tomato", "Lettuce", "Cheese", "Ham" };
int randomOrder[SANDWICH_SIZE] = { 0, 0, 0, 0, 0 };

void GenerateRandomOrder(void)
{
	int min = 1;
	int max = 4; 

	for (int i = 0; i < SANDWICH_SIZE; i++)
	{
		if (i == 0 || i == SANDWICH_SIZE - 1) {
			randomOrder[i] = 0;
		}
		else {
			randomOrder[i] = (rand() % (max - min + 1)) + min; 
		}
	}
}

int CheckSandwich(void)
{
	if (sandwichOrder != SANDWICH_SIZE)
	{
		return 0;
	}
	for (int i = 0; i < SANDWICH_SIZE; i++)
	{
		if (sandwich[i] != randomOrder[i])
		{
			return 0;
		}
	}
	return 1;
}

void Game_Init(void)
{
	// Load Assets
	pbread = CP_Image_Load("Assets/bread.png");
	tomatoS = CP_Image_Load("Assets/tomato.png");
	lettuce = CP_Image_Load("Assets/lettuce.png");
	cheese = CP_Image_Load("Assets/cheese.png");
	ham = CP_Image_Load("Assets/ham.png");
	myFont = CP_Font_Load("Assets/Exo2-Regular.ttf");

	// Random order generator
	srand(time(NULL));
	int min = 1;
	int max = 4;
	int randomNum = 0;

	for (int i = 0; i < SANDWICH_SIZE; i++)
	{
		if (i == 0 || i == SANDWICH_SIZE - 1) {
			randomOrder[i] = 0;
			continue;
		}
		else {
			randomNum = (rand() % (max - min + 1)) + min;
			randomOrder[i] = randomNum;
		}
	}
}

void Game_Update(void)
{
	CP_Graphics_ClearBackground(BG_COLOUR); // Don't move this, this is for clearing the canvas at the start of every update

	CP_Image ingredients[SANDWICH_SIZE] = { pbread, tomatoS, lettuce, cheese, ham}; // array for storing the ref to actual ingredient images

	// Render regular game assets here
	//CP_Image_Draw(pbread, (float)CP_System_GetWindowWidth() / 5.0f, (float)CP_System_GetWindowHeight() / 2.0f, 200.0f, 200.0f, 255);

	// Random order generator
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(30.0f);
	CP_Font_Set(myFont);

	CP_Font_DrawText("Customer Order:", 100.0f, 100.0f);

	for (int i = 0; i < SANDWICH_SIZE; i++)
	{
		CP_Image_Draw(ingredients[randomOrder[i]], 100.0f, 150.0f + i * 70.0f, 50.0f, 50.0f, 255);
		CP_Font_DrawText(sandwich_ingredients[randomOrder[i]], 100.0f, 150.0f + i * 70.0f);
	}

	// Controls
	if (sandwichOrder < SANDWICH_SIZE)
	{
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
		if (CP_Input_KeyTriggered(KEY_L)) // L for lettuce
		{
			sandwich[sandwichOrder] = 2;
			sandwichOrder++;
		}
		if (CP_Input_KeyTriggered(KEY_C)) // C for cheese
		{
			sandwich[sandwichOrder] = 3;
			sandwichOrder++;
		}
		if (CP_Input_KeyTriggered(KEY_H)) // H for ham
		{
			sandwich[sandwichOrder] = 4;
			sandwichOrder++;
		}
	}
	
	if (CP_Input_KeyTriggered(KEY_BACKSPACE)) // Remove ingredient
	{
		sandwichOrder--;
	}

	sandwichOrder = CP_Math_ClampInt(sandwichOrder, 0, SANDWICH_SIZE); // This clamps sandwich size to SANDWICH_SIZE

	if (CheckSandwich())
	{
		sandwichOrder = 0;
		GenerateRandomOrder();
	}

	for (int i = 0; i < sandwichOrder; i++) // Render Work in Progress sandwich 
	{
		CP_Image_Draw(ingredients[sandwich[i]], (float)CP_System_GetWindowWidth() / 2.0f, (float)CP_System_GetWindowHeight() * 0.75f - (20.0f * i), 200.0f, 200.0f, 255);
	}
}

void Game_Exit(void)
{
	// Off load assets here
	CP_Image_Free(pbread);
	CP_Image_Free(tomatoS);
	CP_Image_Free(lettuce);
	CP_Image_Free(cheese);
	CP_Image_Free(ham);
	CP_Font_Free(myFont);
}