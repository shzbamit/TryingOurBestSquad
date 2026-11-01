#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include "cprocessing.h"
#include "game.h"
#include "math.h"
#include "animations.h"

// Assets
CP_Font myFont;

struct Ingredient
{
	int itemID;
	const char* name;
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

// Animations
#define CUSTOMER_MOVE_SPEED 5.0f

// Score
int score = 0;
// This tracks when the score is updated by comparing lastScore to score
int lastScore = -1;
char scoreText[12];
#define SCORE_ORDER 100 // For completing orders

#define SCORE_REMOVE_INGREDIENT 10 // For discarding ingredients
#define MIN_PATIENCE 10.0f
#define MAX_PATIENCE 25.0f
#define PATIENCE_SPEED 0.5f
#define SCORE_CUSTOMER_LEFT 50 // points lost when a customer leaves 
#define SCORE_SPEED_BONUS_MAX 100 // points gained when order is completed faster

// Timer
int total_seconds = 300; // 5 minutes

int minutes = 0;
int seconds = 0;
char timerText[6];

clock_t next_tick;

// Customer
#define MAX_CUSTOMERS 50
#define CUSTOMERS_SHOWN 3
#define NUM_CHARACTER_ASSETS 9

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
	float patience;
	float maxPatience;
	float animationProgress; // Set to 0.0f under spawn initial customers, used as animation tracker
	CP_Vector startPos;      // Remembers where the customer started moving from
	CP_Vector targetPos;     // Remembers where the customer is moving to
	int isLeaving;			 // 1 if customer is currently exiting. This is for the leaving animation
	float shakeTimer;
};
struct Customer customers[MAX_CUSTOMERS];
CP_Image customerAssets[NUM_CHARACTER_ASSETS];
int customerCount = 0;

// For leaving customers
struct Customer leavingCustomers[MAX_CUSTOMERS];
int leavingCustomerCount = 0;


// Array cleaner
static void ShiftCustomerArray(struct Customer arr[], int* count, int indexToRemove)
{
	for (int i = indexToRemove; i < (*count) - 1; i++)
	{
		arr[i] = arr[i + 1];
	}
	(*count)--;
}

static void GenerateRandomOrder(int order[])
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

static int CheckSandwich(void)
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

static void UpdateCustomerPositions(void)
{
	for (int i = 0; i < customerCount; i++) // i checks all customers (both leaving and waiting state customers)
	{

		customers[i].position = i;

		// Set coordinates for moving animation
		customers[i].startPos = CP_Vector_Set(customers[i].x, customers[i].y);
		customers[i].targetPos = CP_Vector_Set(450.0f + (i * 240.0f), 550.0f);
		customers[i].animationProgress = 0.0f; // Set animation progress at 0%

		customers[i].width = 180.0f;
		customers[i].height = 220.0f;
	}
}

static void SpawnCustomer(void) // spwan just one customer at a time, this is called when a customer leaves the queue
{
	int i = customerCount;

	customers[i].asset = customerAssets[rand() % NUM_CHARACTER_ASSETS];

	GenerateRandomOrder(customers[i].order);

	customers[i].orderCompleted = 0;
	customers[i].isLeaving = 0;
	float randomPatience = MIN_PATIENCE +
		((float)rand() / (float)RAND_MAX) *
		(MAX_PATIENCE - MIN_PATIENCE);

	customers[i].patience = randomPatience;
	customers[i].maxPatience = randomPatience;
	customers[i].animationProgress = 0.0f;

	// Spawn at the right side of the screen
	customers[i].x = 1900.0f;
	customers[i].y = 400.0f;

	customerCount++;

	UpdateCustomerPositions();
}

static void CompleteCustomer(int index)
{
	if (customerCount == 0) return;

	// Copy the front customer into the leaving array
	leavingCustomers[leavingCustomerCount] = customers[index];

	// Set up their exit path
	leavingCustomers[leavingCustomerCount].startPos = CP_Vector_Set(customers[index].x, customers[index].y);
	leavingCustomers[leavingCustomerCount].targetPos = CP_Vector_Set(customers[index].x, -100.0f); // Move up off-screen
	leavingCustomers[leavingCustomerCount].animationProgress = 0.0f;
	leavingCustomers[leavingCustomerCount].isLeaving = 1;
	leavingCustomerCount++;

	// Shift the main queue immediately so the next customer takes over index 0
	ShiftCustomerArray(customers, &customerCount, index);

	// Tell the remaining active line queue to slide forward step slots
	UpdateCustomerPositions();

	// Spawn a new customer if the queue is below the maximum shown
	if (customerCount < 3)
	{
		SpawnCustomer();
	}
}

static void SpawnInitialCustomers(void)
{
	customerCount = CUSTOMERS_SHOWN;

	for (int i = 0; i < customerCount; i++)
	{
		customers[i].asset =
			customerAssets[rand() % NUM_CHARACTER_ASSETS];

		GenerateRandomOrder(customers[i].order);

		customers[i].orderCompleted = 0;
		customers[i].isLeaving = 0;
		float randomPatience = MIN_PATIENCE +
			((float)rand() / (float)RAND_MAX) *
			(MAX_PATIENCE - MIN_PATIENCE);
		customers[i].patience = randomPatience;
		customers[i].maxPatience = randomPatience;
		customers[i].animationProgress = 0.0f; // Set animation progress to 0. This variable is used to track individual animation progress.

		// Initial spawn coordinates
		customers[i].x = 1900.0f;
		customers[i].y = 400.0f;
	}

	UpdateCustomerPositions();
}

static int FindMatchingCustomer(void)
{
	// Only check for matching orders when the sandwich is complete
	if (sandwichOrder != SANDWICH_SIZE)
	{
		return -1;
	}

	int selectedCustomer = -1;

	for (int i = 0; i < customerCount; i++)
	{
		int match = 1;

		for (int j = 0; j < SANDWICH_SIZE; j++)
		{
			if (sandwich[j] != customers[i].order[j])
			{
				match = 0;
				break;
			}
		}

		if (match)
		{
			if (selectedCustomer == -1 ||
				customers[i].patience < customers[selectedCustomer].patience)
			{
				selectedCustomer = i;
			}
		}
	}

	return selectedCustomer;
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
	customerAssets[6] = CP_Image_Load("Assets/Character 7.png");
	customerAssets[7] = CP_Image_Load("Assets/Character 8.png");
	customerAssets[8] = CP_Image_Load("Assets/Character 9.png");

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
	static CP_Image ingredients[SANDWICH_SIZE];// array for storing the ref to actual ingredient images
	static const char* sandwich_ingredients[5];
	static int arrays_initialized = 0;

	if (!arrays_initialized) // initializes the arrays only once
	{
		ingredients[0] = bread.asset; ingredients[1] = tomato.asset;
		ingredients[2] = lettuce.asset; ingredients[3] = cheese.asset;
		ingredients[4] = ham.asset;

		sandwich_ingredients[0] = bread.name; sandwich_ingredients[1] = tomato.name;
		sandwich_ingredients[2] = lettuce.name; sandwich_ingredients[3] = cheese.name;
		sandwich_ingredients[4] = ham.name;

		arrays_initialized = 1;
	}

	// Render game assets here ==========================================================================================================


	// Display all customer orders
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(25.0f);
	CP_Font_Set(myFont);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_BASELINE);

	for (int i = 0; i < customerCount; i++)
	{
		char customerText[30];

		snprintf(customerText, sizeof(customerText), "Customer %d", i + 1);

		float startX = 30.0f + (i * 200.0f);
		float startY = 55.0f;

		CP_Font_DrawText(customerText, startX, startY);

		for (int j = 0; j < SANDWICH_SIZE; j++)
		{
			int ingredientID = customers[i].order[j];
			char keyText[2];

			switch (ingredientID)
			{
			case 0: keyText[0] = 'B'; break;
			case 1: keyText[0] = 'T'; break;
			case 2: keyText[0] = 'L'; break;
			case 3: keyText[0] = 'C'; break;
			case 4: keyText[0] = 'H'; break;
			default: keyText[0] = '?'; break;
			}

			keyText[1] = '\0';

			float keyX = startX;
			float keyY = startY + 20.0f + (j * 55.0f);

			CP_Settings_RectMode(CP_POSITION_CORNER);
			CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
			CP_Settings_Stroke(CP_Color_Create(150, 150, 150, 255));
			CP_Graphics_DrawRect(keyX, keyY, 40.0f, 40.0f);

			CP_Settings_NoStroke();
			CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
			CP_Settings_TextSize(25.0f);
			CP_Font_DrawText(keyText, keyX + 12.0f, keyY + 29.0f);

			CP_Image_Draw(
				ingredients[ingredientID],
				keyX + 72.0f,
				keyY + 20.0f,
				55.0f,
				55.0f,
				255
			);
		}
	}


	// Score text
	CP_Settings_Fill(SCORE_TEXT_COLOR);
	CP_Settings_TextSize(SCORE_TEXT_SIZE);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_BASELINE);

	CP_Font_DrawText(scoreText, 800.0f, 100.0f);

	// Timer text
	CP_Font_DrawText(timerText, 1450.0f, 100.0f);

	// Calculate once before the loop begins
	float centerX = (float)CP_System_GetWindowWidth() / 2.0f;
	float centerYBase = (float)CP_System_GetWindowHeight() * 0.75f;

	// Render sandwich 
	for (int i = 0; i < sandwichOrder; i++)
	{
		CP_Image_Draw(ingredients[sandwich[i]], centerX, centerYBase - (20.0f * i), 200.0f, 200.0f, 255);
	}

	// Draw customers in the queue
	for (int i = 0; i < customerCount; i++)
	{
		// Interpolate coordinates via your custom function
		CP_Vector positionFrame = MoveImage(
			CUSTOMER_MOVE_SPEED * CP_System_GetDt(),
			customers[i].startPos,
			customers[i].targetPos,
			&customers[i].animationProgress
		);

		// Store frame updates so downstream math remains fully synced
		customers[i].x = positionFrame.x;
		customers[i].y = positionFrame.y;

		// Calculate shaking movement
		float shakeOffset = 0.0f;

		if (!customers[i].isLeaving)
		{
			shakeOffset = ShakeCustomer(
				customers[i].patience,
				customers[i].maxPatience,
				&customers[i].shakeTimer
			);
		}

		CP_Image_Draw(
			customers[i].asset,
			customers[i].x + shakeOffset,
			customers[i].y,
			customers[i].width,
			customers[i].height,
			255
		);

		// Patience timer bar above the customer 
		if (!customers[i].isLeaving && customers[i].x + customers[i].width / 2.0f <= (float)CP_System_GetWindowWidth())
		{
			float ratio = customers[i].patience / customers[i].maxPatience;
			float barWidth = customers[i].width;
			float barHeight = 15.0f;
			float barX = customers[i].x - barWidth / 2.0f;
			float barY = customers[i].y - customers[i].height / 2.0f - 25.0f;

			CP_Settings_RectMode(CP_POSITION_CORNER);
			CP_Settings_NoStroke();

			CP_Settings_Fill(CP_Color_Create(200, 200, 200, 255));
			CP_Graphics_DrawRect(barX, barY, barWidth, barHeight);

			if (ratio > 0.5f)
			{
				CP_Settings_Fill(CP_Color_Create(0, 200, 0, 255));
			}
			else if (ratio > 0.25f)
			{
				CP_Settings_Fill(CP_Color_Create(255, 180, 0, 255));
			}
			else
			{
				CP_Settings_Fill(CP_Color_Create(220, 50, 50, 255));
			}
			CP_Graphics_DrawRect(barX, barY, barWidth * ratio, barHeight);
		}
	}

	// If a customer finished their animation path and was marked as leaving, erase them from memory
	for (int i = 0; i < leavingCustomerCount; i++)
	{
		// Animate leaving progress independent of queue steps
		CP_Vector positionFrame = MoveImage(CUSTOMER_MOVE_SPEED * CP_System_GetDt(), leavingCustomers[i].startPos, leavingCustomers[i].targetPos, &leavingCustomers[i].animationProgress);
		leavingCustomers[i].x = positionFrame.x;
		leavingCustomers[i].y = positionFrame.y;

		// Draw the walking-away customer (No patience bar drawn here!)
		CP_Image_Draw(leavingCustomers[i].asset, leavingCustomers[i].x, leavingCustomers[i].y, leavingCustomers[i].width, leavingCustomers[i].height, 255);

		// Clean them up once they completely vanish off screen
		if (leavingCustomers[i].animationProgress >= 1.0f)
		{
			ShiftCustomerArray(leavingCustomers, &leavingCustomerCount, i);
			i--;
		}
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
			score = CP_Math_ClampInt(score, 0, INT_MAX); // clamps score to 0
		}
	}

	sandwichOrder = CP_Math_ClampInt(
		sandwichOrder, 0, SANDWICH_SIZE
	);

	int matchedCustomer = FindMatchingCustomer();

	if (matchedCustomer != -1)
	{
		score += SCORE_ORDER;

		// For bonus points
		float patienceRatio =
			customers[matchedCustomer].patience /
			customers[matchedCustomer].maxPatience;
		int speedBonus = (int)(SCORE_SPEED_BONUS_MAX * patienceRatio);
		score += speedBonus;

		score = CP_Math_ClampInt(score, 0, INT_MAX);

		CompleteCustomer(matchedCustomer);

		sandwichOrder = 0;
	}

	// Patience countdown
	for (int i = 0; i < customerCount; i++)
	{
		customers[i].patience -= CP_System_GetDt() * PATIENCE_SPEED;
	}

	// Remove customers whose patience has expired
	for (int i = customerCount - 1; i >= 0; i--)
	{
		if (customers[i].patience <= 0.0f)
		{
			score -= SCORE_CUSTOMER_LEFT;
			score = CP_Math_ClampInt(score, 0, INT_MAX);

			CompleteCustomer(i);
		}
	}

	// This only updates when score is changed
	if (score != lastScore) 
	{
		snprintf(scoreText, sizeof(scoreText), "%d", score); // Converts score int to char
		lastScore = score;
	}

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