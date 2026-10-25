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

// Customer
#define MAX_CUSTOMERS 50
#define NUM_CHARACTER_ASSETS 6

struct Customer
{
	CP_Image asset;
	int order[SANDWICH_SIZE];
	int position;
	float x;
	float y;
	float width;
	float height;
	int orderCompleted;
};
struct Customer customers[MAX_CUSTOMERS];
CP_Image customerAssets[NUM_CHARACTER_ASSETS];
int customerCount = 0;

void GenerateRandomOrder(int order[])
{
	int min = 1;
	int max = 4; 

	for (int i = 0; i < SANDWICH_SIZE; i++)
	{
		if (i == 0 || i == SANDWICH_SIZE - 1) {
			order[i] = 0;
		}
		else {
			order[i] = (rand() % (max - min + 1)) + min; 
		}
	}
}

int CheckSandwich(void)
{
	if (customerCount == 0)
	{
		return 0;
	}

	if (sandwichOrder != SANDWICH_SIZE)
	{
		return 0;
	}

	// Compare the player sandwich with the front customer order
	for (int i = 0; i < SANDWICH_SIZE; i++)
	{
		if (sandwich[i] != customers[0].order[i])
		{
			return 0;
		}
	}
	return 1;
}

void UpdateCustomerPositions(void)
{
	for (int i = 0; i < customerCount; i++)
	{
		customers[i].position = i;

		customers[i].x = 650.0f + (i * 250.0f);
		customers[i].y = 400.0f;

		customers[i].width = 180.0f;
		customers[i].height = 220.0f;
	}
}

void CompleteFrontCustomer(void)
{
	if (customerCount == 0)
	{
		return;
	}

	customers[0].orderCompleted = 1;

	// Remove the front customer after completed order
	for (int i = 0; i < customerCount - 1; i++)
	{
		customers[i] = customers[i + 1];
	}

	customerCount--;

	sandwichOrder = 0;

	UpdateCustomerPositions();
}

void SpawnInitialCustomers(void)
{
	customerCount = MAX_CUSTOMERS;

	for (int i = 0; i < customerCount; i++)
	{
		customers[i].asset =
			customerAssets[rand() % NUM_CHARACTER_ASSETS];

		GenerateRandomOrder(customers[i].order);

		customers[i].orderCompleted = 0;
	}

	UpdateCustomerPositions();
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
	cheese.name = "Cheese";
	cheese.asset = CP_Image_Load("Assets/cheese.png");
	cheese.keybind = KEY_C;

	ham.itemID = 4;
	ham.name = "Ham";
	ham.asset = CP_Image_Load("Assets/ham.png");
	ham.keybind = KEY_H;

	// Load customer character assets
	customerAssets[0] = CP_Image_Load("Assets/Character 1.png");
	customerAssets[1] = CP_Image_Load("Assets/Character 2.png");
	customerAssets[2] = CP_Image_Load("Assets/Character 3.png");
	customerAssets[3] = CP_Image_Load("Assets/Character 4.png");
	customerAssets[4] = CP_Image_Load("Assets/Character 5.png");
	customerAssets[5] = CP_Image_Load("Assets/Character 6.png");

	myFont = CP_Font_Load("Assets/Exo2-Regular.ttf");

	srand((unsigned int)time(NULL));

	// Customer queue
	SpawnInitialCustomers();

	next_tick = clock() + CLOCKS_PER_SEC;   // Set the first tick target
	// Initialize the string with a safe placeholder
	snprintf(timerText, sizeof(timerText), "05:00");
}

void Game_Update(void)
{
	CP_Graphics_ClearBackground(BG_COLOUR); // Don't move this, this is for clearing the canvas at the start of every update

	// Dependencies
	CP_Image ingredients[SANDWICH_SIZE] = { bread.asset, tomato.asset, lettuce.asset, cheese.asset, ham.asset }; // array for storing the ref to actual ingredient images
	char sandwich_ingredients[5][20] = { bread.name, tomato.name, lettuce.name, cheese.name, ham.name };

	// Render game assets here ==========================================================================================================

	// Display only the front customer order
	if (customerCount > 0)
	{
		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
		CP_Settings_TextSize(30.0f);
		CP_Font_Set(myFont);

		CP_Font_DrawText("Customer Order:", 100.0f, 100.0f);

		for (int i = 0; i < SANDWICH_SIZE; i++)
		{
			int ingredientID = customers[0].order[i];

			CP_Image_Draw(
				ingredients[ingredientID],
				110.0f,
				165.0f + i * 70.0f,
				50.0f,
				50.0f,
				255
			);

			CP_Font_DrawText(
				sandwich_ingredients[ingredientID],
				160.0f,
				175.0f + i * 70.0f
			);
		}
	}

	//CP_Image_Draw(pbread, (float)CP_System_GetWindowWidth() / 5.0f, (float)CP_System_GetWindowHeight() / 2.0f, 200.0f, 200.0f, 255);

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

	// Draw customers in the queue
	for (int i = 0; i < customerCount; i++)
	{
		CP_Image_Draw(
			customers[i].asset,
			customers[i].x,
			customers[i].y,
			customers[i].width,
			customers[i].height,
			255
		);
	}

	// End of asset renders =====================================================================================================================


	// Controls
	if (customerCount > 0 && sandwichOrder < SANDWICH_SIZE)
	{
		if (CP_Input_KeyTriggered(bread.keybind))
		{
			sandwich[sandwichOrder] = bread.itemID;
			sandwichOrder++;
		}
		else if (CP_Input_KeyTriggered(tomato.keybind))
		{
			sandwich[sandwichOrder] = tomato.itemID;
			sandwichOrder++;
		}
		else if (CP_Input_KeyTriggered(lettuce.keybind))
		{
			sandwich[sandwichOrder] = lettuce.itemID;
			sandwichOrder++;
		}
		else if (CP_Input_KeyTriggered(cheese.keybind))
		{
			sandwich[sandwichOrder] = cheese.itemID;
			sandwichOrder++;
		}
		else if (CP_Input_KeyTriggered(ham.keybind))
		{
			sandwich[sandwichOrder] = ham.itemID;
			sandwichOrder++;
		}
	}

	// Removes ingredient by backspace
	if (customerCount > 0 &&
		CP_Input_KeyTriggered(KEY_BACKSPACE))
	{
		if (sandwichOrder > 0)
		{
			sandwichOrder--;
			score -= SCORE_REMOVE_INGREDIENT;
		}
	}

	sandwichOrder = CP_Math_ClampInt(
		sandwichOrder, 0, SANDWICH_SIZE
	);

	// Check whether the front customer order is complete
	if (CheckSandwich())
	{
		score += SCORE_ORDER;
		CompleteFrontCustomer();
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

	for (int i = 0; i < NUM_CHARACTER_ASSETS; i++)
	{
		CP_Image_Free(customerAssets[i]);
	}
}