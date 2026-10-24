#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cprocessing.h"
#include "game.h"
#include "math.h"

// Assets
CP_Font myFont;

struct Ingredient
{
	int itemID;
	char name;
	CP_Image asset;
	CP_KEY keybind;
};
struct Ingredient bread;
struct Ingredient tomato;
struct Ingredient lettuce;
struct Ingredient cheese;
struct Ingredient ham;

// Colours
#define BG_COLOUR CP_Color_Create(247, 252, 252, 255) // Background
#define SCORE_TEXT_COLOR CP_Color_Create(128, 128, 128, 255) 

#define SCORE_TEXT_SIZE 100.0f
#define SANDWICH_SIZE 5

int sandwichOrder = 0; // order in which sandwich will be drawn
int sandwich[SANDWICH_SIZE] = { 0, 0, 0, 0, 0}; // for storing which ingredient value
int randomOrder[SANDWICH_SIZE] = { 0, 0, 0, 0, 0 };

// Score
int score = 0;
char scoreText[12];
#define SCORE_ORDER 100 // For completing orders

#define SCORE_REMOVE_INGREDIENT 5 // For discarding ingredients

// Timer
int total_seconds = 300; // 5 minutes

int minutes = 0;
int seconds = 0;
char timerText[6];

clock_t next_tick;

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
	bread.itemID = 0;
	bread.name = "Bread";
	bread.asset = CP_Image_Load("Assets/bread.png");
	bread.keybind = KEY_B;
	
	tomato.itemID = 1;
	tomato.name = "Tomato";
	tomato.asset = CP_Image_Load("Assets/tomato.png");
	tomato.keybind = KEY_T;
	
	lettuce.itemID = 2;
	lettuce.name = "Lettuce";
	lettuce.asset = CP_Image_Load("Assets/lettuce.png");
	lettuce.keybind = KEY_L;

	cheese.itemID = 3;
	cheese.name = "Lettuce";
	cheese.asset = CP_Image_Load("Assets/cheese.png");
	cheese.keybind = KEY_C;

	ham.itemID = 4;
	ham.name = "Ham";
	ham.asset = CP_Image_Load("Assets/ham.png");
	ham.keybind = KEY_H;

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

	next_tick = clock() + CLOCKS_PER_SEC;   // Set the first tick target
	// Initialize the string with a safe placeholder
	snprintf(timerText, sizeof(timerText), "05:00");
}

void Game_Update(void)
{
	// Dependencies
	CP_Image ingredients[SANDWICH_SIZE] = { bread.asset, tomato.asset, lettuce.asset, cheese.asset, ham.asset }; // array for storing the ref to actual ingredient images
	char sandwich_ingredients[5][20] = { bread.name, tomato.name, lettuce.name, cheese.name, ham.name };

	// Render game assets here ==========================================================================================================

	CP_Graphics_ClearBackground(BG_COLOUR); // Don't move this, this is for clearing the canvas at the start of every update
	//CP_Image_Draw(pbread, (float)CP_System_GetWindowWidth() / 5.0f, (float)CP_System_GetWindowHeight() / 2.0f, 200.0f, 200.0f, 255);

	// Random order text generator
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(30.0f);
	CP_Font_Set(myFont);

	CP_Font_DrawText("Customer Order:", 100.0f, 100.0f);

	// Score text
	CP_Settings_Fill(SCORE_TEXT_COLOR);
	CP_Settings_TextSize(SCORE_TEXT_SIZE);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_BASELINE);

	CP_Font_DrawText(scoreText, 800.0f, 100.0f);

	// Timer text
	CP_Font_DrawText(timerText, 1450.0f, 100.0f);

	// Render sandwich 
	for (int i = 0; i < sandwichOrder; i++)
	{
		CP_Image_Draw(ingredients[sandwich[i]], (float)CP_System_GetWindowWidth() / 2.0f, (float)CP_System_GetWindowHeight() * 0.75f - (20.0f * i), 200.0f, 200.0f, 255);
	}

	// Random order generator
	for (int i = 0; i < SANDWICH_SIZE; i++)
	{
		CP_Image_Draw(ingredients[randomOrder[i]], 100.0f, 150.0f + i * 70.0f, 50.0f, 50.0f, 255);
		CP_Font_DrawText(sandwich_ingredients[randomOrder[i]], 100.0f, 150.0f + i * 70.0f);
	}

	// End of asset renders =====================================================================================================================


	// Controls
	if (sandwichOrder < SANDWICH_SIZE)
	{
		if (CP_Input_KeyTriggered(bread.keybind)) // B for bread
		{
			sandwich[sandwichOrder] = bread.itemID;
			sandwichOrder++;
		}
		if (CP_Input_KeyTriggered(tomato.keybind)) // T for tomato
		{
			sandwich[sandwichOrder] = tomato.itemID;
			sandwichOrder++;
		}
		if (CP_Input_KeyTriggered(lettuce.keybind)) // L for lettuce
		{
			sandwich[sandwichOrder] = lettuce.itemID;
			sandwichOrder++;
		}
		if (CP_Input_KeyTriggered(cheese.keybind)) // C for cheese
		{
			sandwich[sandwichOrder] = cheese.itemID;
			sandwichOrder++;
		}
		if (CP_Input_KeyTriggered(ham.keybind)) // H for ham
		{
			sandwich[sandwichOrder] = ham.itemID;
			sandwichOrder++;
		}
	}
	if (CP_Input_KeyTriggered(KEY_BACKSPACE)) // Remove ingredient
	{
		if (sandwichOrder > 0)
		{
			score -= SCORE_REMOVE_INGREDIENT;
		}
		sandwichOrder--;
	}

	sandwichOrder = CP_Math_ClampInt(sandwichOrder, 0, SANDWICH_SIZE); // This clamps sandwich size to SANDWICH_SIZE

	// Order completion
	if (CheckSandwich())
	{
		sandwichOrder = 0;
		score += SCORE_ORDER;
		GenerateRandomOrder();
	}

	snprintf(scoreText, sizeof(scoreText), "%d", score); // Converts score int to char

	// Timer check
	if (clock() >= next_tick && total_seconds >= 0)
	{
		total_seconds--;
		next_tick = clock() + CLOCKS_PER_SEC; // Set target for next second

		// Exit game if timer ends
		if (total_seconds < 0) {
			CP_Engine_Terminate();
		}

		// Render the update
		minutes = total_seconds / 60;
		seconds = total_seconds % 60;
		snprintf(timerText, sizeof(timerText), "%02d:%02d", minutes, seconds); // Converts min/sec int to char
		fflush(stdout); // Force engine to instantly display timer text
	}

}

void Game_Exit(void)
{
	// Off load assets here
	CP_Image_Free(bread.asset);
	CP_Image_Free(tomato.asset);
	CP_Image_Free(lettuce.asset);
	CP_Image_Free(cheese.asset);
	CP_Image_Free(ham.asset);
	CP_Font_Free(myFont);
}