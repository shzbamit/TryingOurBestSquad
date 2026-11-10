#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include "cprocessing.h"
#include "game.h"
#include "math.h"
#include "animations.h"
#include "ingredients.h"
#include "sandwich.h"


// Assets
CP_Font myFont;
CP_Sound bgm;
CP_Sound sfxServe;
CP_Sound sfxDiscard; 
CP_Sound sfxLeave; 
CP_Sound sfxTick;
CP_Sound sfxCustomerEnter;

// Colours
#define BG_COLOUR					CP_Color_Create(247, 252, 252, 255) // Background
#define SCORE_TEXT_COLOR			CP_Color_Create(128, 128, 128, 255) 

// Score
int score =							0;
int lastScore =						-1;		// This tracks when the score is updated by comparing lastScore to score
char scoreText[12];
#define SCORE_TEXT_SIZE				100.0f

#define SCORE_ORDER					100		// For completing orders
#define SCORE_SPEED_BONUS_MAX		100		// points gained when order is completed faster
#define SCORE_REMOVE_INGREDIENT		-10		// For discarding ingredients
#define SCORE_CUSTOMER_LEFT			-50		// points lost when a customer leaves 
#define CUSTOMER_PLACEMENT_START_X	700.0f  // must be right of the order columns (~540)
#define CUSTOMER_PLACEMENT_INTERVAL 260.0f

// Timer
int total_seconds =					300;	// 5 minutes

int minutes =						0;
int seconds =						0;
char timerText[6];

clock_t next_tick;

// Customer
#define MAX_CUSTOMERS				50
#define CUSTOMERS_SHOWN				3
#define NUM_CHARACTER_ASSETS		9
#define CUSTOMER_PLACEMENT_INTERVAL 260.0f

struct Customer
{
	CP_Image asset;
	int order[MAX_SANDWICH_SIZE];			// Generates a sandwich array of maximum size
	int orderSize;							// For storing the actual size variable of order sandwich
	int position;
	//CP_Vector currentPos;
	float width;
	float height;
	int orderCompleted;
	float patience;
	float maxPatience;
	float shakeTimer;
	int shakeSoundPlayed;
	struct AnimationState animState;
};

// Animations
#define CUSTOMER_MOVE_SPEED			5.0f

struct Customer customers[MAX_CUSTOMERS];
CP_Image customerAssets[NUM_CHARACTER_ASSETS];
int customerCount =					0;

// Patience 
#define MIN_PATIENCE				10.0f
#define MAX_PATIENCE				25.0f
#define PATIENCE_SPEED				0.5f

// For leaving customers
struct Customer leavingCustomers[MAX_CUSTOMERS];
int leavingCustomerCount =			0;


// Array cleaner
static void ShiftCustomerArray(struct Customer arr[], int* count, int indexToRemove)
{
	for (int i = indexToRemove; i < (*count) - 1; i++)
	{
		arr[i] = arr[i + 1];
	}
	(*count)--;
}

static void GenerateRandomOrder(int order[], int orderSize)
{
	int min = 1;
	int max = INGREDIENT_COUNT - 1; 

	for (int i = 0; i < orderSize; i++)
	{
		if (i == 0 || i == orderSize - 1) {
			order[i] = 0; // Assigns first and last ingredient as bread
		}
		else {
			order[i] = (rand() % (max - min + 1)) + min; // For deciding ingredients
		}
	}
}

static void UpdateCustomerPositions(void)
{
	for (int i = 0; i < customerCount; i++) // i checks all customers (both leaving and waiting state customers)
	{

		customers[i].position = i;

		// Set coordinates for moving animation
		customers[i].animState.startPos = customers[i].animState.currentPos;
		customers[i].animState.targetPos = CP_Vector_Set(CUSTOMER_PLACEMENT_START_X + (i * CUSTOMER_PLACEMENT_INTERVAL), 550.0f);
		customers[i].animState.animationProgress = 0.0f; // Set animation progress at 0%

		customers[i].width = 180.0f;
		customers[i].height = 220.0f;
	}
}

static void SpawnCustomer(void)
{
	if (customerCount >= MAX_CUSTOMERS) return; // guard against overflowing the array

	int i = customerCount;

	customers[i].asset = customerAssets[rand() % NUM_CHARACTER_ASSETS]; // Randomly selects a character asset for the customer

	customers[i].orderSize = (rand() % (MAX_SANDWICH_SIZE - MIN_SANDWICH_SIZE + 1)) + MIN_SANDWICH_SIZE; // Randomly selects a sandwich size for the customer order
	GenerateRandomOrder(customers[i].order, customers[i].orderSize); // Generates a random sandwich order for the customer

	customers[i].orderCompleted = 0; // Initializes the order completion status
	customers[i].shakeSoundPlayed = 0; // Initializes the shake sound played status
	customers[i].shakeTimer = 0.0f; // Initializes the shake timer for the customer

	float randomPatience = MIN_PATIENCE + ((float)rand() / (float)RAND_MAX) * (MAX_PATIENCE - MIN_PATIENCE); // Generates a random patience value for the customer within the defined range
	customers[i].patience = randomPatience;
	customers[i].maxPatience = randomPatience; 

	customers[i].animState.animSpeed = CUSTOMER_MOVE_SPEED;
	customers[i].animState.animationProgress = 0.0f;
	customers[i].animState.isAnimating = 0;

	// Spawn at the right side of the screen
	customers[i].animState.currentPos.x = 1900.0f;
	customers[i].animState.currentPos.y = 400.0f;

	customerCount++;

	CP_Sound_PlayAdvanced(sfxCustomerEnter, 1.0f, 1.0f, FALSE, CP_SOUND_GROUP_SFX);

	UpdateCustomerPositions();
}

static void CompleteCustomer(int index)
{
	if (customerCount == 0) return;

	// Copy the front customer into the leaving array
	leavingCustomers[leavingCustomerCount] = customers[index];

	// Set up their exit path
	leavingCustomers[leavingCustomerCount].animState.startPos = customers[index].animState.currentPos;
	leavingCustomers[leavingCustomerCount].animState.targetPos = CP_Vector_Set(customers[index].animState.currentPos.x, -100.0f); // Move up off-screen
	leavingCustomers[leavingCustomerCount].animState.animationProgress = 0.0f;
	leavingCustomers[leavingCustomerCount].animState.isAnimating = 1;
	leavingCustomerCount++;

	// Shift the main queue immediately so the next customer takes over index 0
	ShiftCustomerArray(customers, &customerCount, index);

	// Tell the remaining active line queue to slide forward step slots
	UpdateCustomerPositions();

	// Spawn a new customer if the queue is below the maximum shown
	if (customerCount < CUSTOMERS_SHOWN)
	{
		SpawnCustomer();
	}
}

static void SpawnInitialCustomers(void) 
{
	customerCount = 0;

	for (int i = 0; i < CUSTOMERS_SHOWN; i++) // Spawn the initial customers to fill the queue
	{
		SpawnCustomer();
	}
}

static int FindMatchingCustomer(void)
{
	// Check further only if there is a sandwich
	if (sandwichOrder == 0)
	{
		return -1;
	}

	int selectedCustomer = -1;

	for (int i = 0; i < customerCount; i++)
	{
		// Size of sandwich must first be identical
		if (sandwichOrder != customers[i].orderSize)
		{
			continue; // If sizes don't match, skip to the next customer
		}

		int match = 1;

		// Compare each ingredient between customer[i].order and sandwich[j]
		for (int j = 0; j < customers[i].orderSize; j++)
		{
			if (sandwich[j].itemID != customers[i].order[j])
			{
				match = 0; // Found mismatched ingredient
				break;
			}
		}

		// If a match is found, track the customer with lowest patience
		// This is to enforce a tie-breaking rule where lowest patience customer gets their order first
		if (match)
		{
			if (selectedCustomer == -1 || customers[i].patience < customers[selectedCustomer].patience)
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
	InitializeIngredientList();

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

	// Load sounds 
	bgm = CP_Sound_LoadMusic("Assets/bgm.mp3");
	sfxServe = CP_Sound_Load("Assets/serve.mp3");
	sfxDiscard = CP_Sound_Load("Assets/discard.mp3");
	sfxLeave = CP_Sound_Load("Assets/leave.mp3");
	sfxTick = CP_Sound_Load("Assets/tick.mp3");
	sfxCustomerEnter = CP_Sound_Load("Assets/CustomerEnter.mp3");

	srand((unsigned int)time(NULL));

	// Customer queue
	SpawnInitialCustomers();

	next_tick = clock() + CLOCKS_PER_SEC;   // Set the first tick target
	// Initialize the string with a safe placeholder
	snprintf(timerText, sizeof(timerText), "05:00");

	// Play music on loop: volume, pitch, looping, group
	CP_Sound_PlayAdvanced(bgm, 0.4f, 1.0f, TRUE, CP_SOUND_GROUP_MUSIC);
}

void Game_Update(void)
{
	CP_Graphics_ClearBackground(BG_COLOUR); // Don't move this, this is for clearing the canvas at the start of every update

	// Render game assets here ==========================================================================================================

	// Display all customer orders
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(25.0f);
	CP_Font_Set(myFont);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_BASELINE);

	// Draw customers in the queue
	for (int i = 0; i < customerCount; i++)
	{
		// Interpolate coordinates via your custom function
		CP_Vector positionFrame = MoveImage(
			customers[i].animState.animSpeed * CP_System_GetDt(),
			customers[i].animState.startPos,
			customers[i].animState.targetPos,
			&customers[i].animState.animationProgress
		);

		// Store frame updates so downstream math remains fully synced
		customers[i].animState.currentPos = positionFrame;

		// Calculate shaking movement
		float shakeOffset = 0.0f;

		if (!customers[i].animState.isAnimating)
		{
			shakeOffset = ShakeCustomer(
				customers[i].patience,
				customers[i].maxPatience,
				&customers[i].shakeTimer
			);
		}

		CP_Image_Draw(
			customers[i].asset,
			customers[i].animState.currentPos.x + shakeOffset,
			customers[i].animState.currentPos.y,
			customers[i].width,
			customers[i].height,
			255
		);

		// Patience timer bar above the customer 
		if (!customers[i].animState.isAnimating && customers[i].animState.currentPos.x + customers[i].width / 2.0f <= (float)CP_System_GetWindowWidth())
		{
			float ratio = customers[i].patience / customers[i].maxPatience;
			float barWidth = customers[i].width;
			float barHeight = 15.0f;
			float barX = customers[i].animState.currentPos.x - barWidth / 2.0f;
			float barY = customers[i].animState.currentPos.y - customers[i].height / 2.0f - 25.0f;

			CP_Settings_RectMode(CP_POSITION_CORNER);
			CP_Settings_NoStroke();

			CP_Settings_Fill(CP_Color_Create(200, 200, 200, 255));
			CP_Graphics_DrawRect(barX, barY, barWidth, barHeight);

			if (ratio > 0.5f) // Green when patience is above 50%
			{
				CP_Settings_Fill(CP_Color_Create(0, 200, 0, 255));
			}
			else if (ratio > 0.25f) // Yellow when patience is between 25% and 50%
			{
				CP_Settings_Fill(CP_Color_Create(255, 180, 0, 255));
			}
			else // Red when patience is below 25%
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
		CP_Vector positionFrame = MoveImage(leavingCustomers[i].animState.animSpeed * CP_System_GetDt(), leavingCustomers[i].animState.startPos, leavingCustomers[i].animState.targetPos, &leavingCustomers[i].animState.animationProgress);
		leavingCustomers[i].animState.currentPos = positionFrame;

		// Draw the walking-away customer (No patience bar drawn here!)
		CP_Image_Draw(leavingCustomers[i].asset, leavingCustomers[i].animState.currentPos.x, leavingCustomers[i].animState.currentPos.y, leavingCustomers[i].width, leavingCustomers[i].height, 255);

		// Clean them up once they completely vanish off screen
		if (leavingCustomers[i].animState.animationProgress >= 1.0f)
		{
			ShiftCustomerArray(leavingCustomers, &leavingCustomerCount, i);
			i--;
		}
	}

	// Draw the customer orders
	for (int i = 0; i < customerCount; i++)
	{
		char customerText[30];

		snprintf(customerText, sizeof(customerText), "Customer %d", i + 1); 

		float startX = 30.0f + (i * 200.0f);
		float startY = 55.0f;

		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
		CP_Font_DrawText(customerText, startX, startY); // Draws the customer label above their order

		for (int j = 0; j < customers[i].orderSize; j++) // Draws the order ingredients and their corresponding keys
		{
			int ingredientID = customers[i].order[j];
			char keyText[2];

			switch (ingredientID) // Assigns the corresponding key to the ingredient
			{
			case 0: keyText[0] = 'B'; break;
			case 1: keyText[0] = 'T'; break;
			case 2: keyText[0] = 'L'; break;
			case 3: keyText[0] = 'C'; break;
			case 4: keyText[0] = 'H'; break;
			default: keyText[0] = '?'; break;
			}

			keyText[1] = '\0';

			// Calculate the position for each ingredient and its corresponding key
			float keyX = startX;
			float keyY = startY + 20.0f + (j * 55.0f);

			// Draw the key box and ingredient image
			CP_Settings_RectMode(CP_POSITION_CORNER);
			CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
			CP_Settings_Stroke(CP_Color_Create(150, 150, 150, 255));
			CP_Graphics_DrawRect(keyX, keyY, 40.0f, 40.0f);

			// Draw the key text and ingredient image
			CP_Settings_NoStroke();
			CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
			CP_Settings_TextSize(25.0f);
			CP_Font_DrawText(keyText, keyX + 12.0f, keyY + 29.0f);

			// Draw the ingredient image next to the key box
			CP_Image_Draw(
				ingredients[ingredientID].asset,
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
	if (total_seconds <= 30)
	{
		// Toggles every 0.25s, so it blinks about twice per second
		int flashOn = ((int)(CP_System_GetSeconds() * 4.0) % 2) == 0;

		if (flashOn)
		{
			CP_Settings_Fill(CP_Color_Create(220, 50, 50, 255));
		}
		// else: stays SCORE_TEXT_COLOR (grey), which creates the flash
	}

	// Timer text
	CP_Font_DrawText(timerText, 1450.0f, 100.0f);

	RenderSandwich();
	// Updates physics positions and prints all falling discarded ingredients
	UpdateAndRenderDiscards();

	// End of asset renders =====================================================================================================================

	// Controls for making the sandwich
	MakeSandwich(customerCount);

	// Removes ingredient by backspace
	if (customerCount > 0 && CP_Input_KeyTriggered(KEY_BACKSPACE))
	{
		if (sandwichOrder > 0)
		{
			RemoveIngredient();
			CP_Sound_PlayAdvanced(sfxDiscard, 1.0f, 1.0f, FALSE, CP_SOUND_GROUP_SFX);

			// Remove ingredient from the active sandwich stack instantly
			sandwichOrder--;
			score += SCORE_REMOVE_INGREDIENT;
			score = CP_Math_ClampInt(score, 0, INT_MAX); // clamps score to 0
		}
	}

	// Removes all ingredients from the current sandwich 
	if (customerCount > 0 && CP_Input_KeyTriggered(KEY_DELETE))
	{
		if (sandwichOrder > 0)
		{
			CP_Sound_PlayAdvanced(sfxDiscard, 1.0f, 1.0f, FALSE, CP_SOUND_GROUP_SFX);
		}

		while (sandwichOrder > 0)
		{
			RemoveIngredient();

			sandwichOrder--;
			score += SCORE_REMOVE_INGREDIENT;
		}
		score = CP_Math_ClampInt(score, 0, INT_MAX); // clamps score to 0
	}


	sandwichOrder = CP_Math_ClampInt(sandwichOrder, 0, MAX_SANDWICH_SIZE);

	int matchedCustomer = FindMatchingCustomer();

	// If a match is found, complete the order and update the score
	if (matchedCustomer != -1)
	{
		score += SCORE_ORDER;

		// For bonus points
		float patienceRatio = customers[matchedCustomer].patience / customers[matchedCustomer].maxPatience; // Calculate the ratio of remaining patience to maximum patience
		int speedBonus = (int)(SCORE_SPEED_BONUS_MAX * patienceRatio); // Calculate the speed bonus based on the patience ratio
		score += speedBonus;

		score = CP_Math_ClampInt(score, 0, INT_MAX);

		CompleteCustomer(matchedCustomer); // Remove the customer from the queue and mark them as leaving
		CP_Sound_PlayAdvanced(sfxServe, 1.0f, 1.0f, FALSE, CP_SOUND_GROUP_SFX);

		sandwichOrder = 0;
	}

	// Patience countdown
	for (int i = 0; i < customerCount; i++)
	{
		customers[i].patience -= CP_System_GetDt() * PATIENCE_SPEED;

		// Play the warning sound once, when the customer has 4 secs left
		float secondsLeft = customers[i].patience / PATIENCE_SPEED;

		if (!customers[i].shakeSoundPlayed && secondsLeft <= 3.0f)
		{
			customers[i].shakeSoundPlayed = 1;
			CP_Sound_PlayAdvanced(sfxLeave, 1.0f, 1.0f, FALSE, CP_SOUND_GROUP_SFX);
		}
	}

	// Remove customers whose patience has expired
	for (int i = customerCount - 1; i >= 0; i--)
	{
		if (customers[i].patience <= 0.0f)
		{
			score += SCORE_CUSTOMER_LEFT;
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

		// timer countdown sfx plays 
		if (total_seconds >= 0 && total_seconds <= 10)
		{
			CP_Sound_PlayAdvanced(sfxTick, 1.0f, 1.0f, FALSE, CP_SOUND_GROUP_SFX);
		}

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
	CP_Sound_Free(bgm);
	CP_Sound_Free(sfxServe);
	CP_Sound_Free(sfxDiscard);
	CP_Sound_Free(sfxLeave);
	CP_Sound_Free(sfxTick);
	CP_Sound_Free(sfxCustomerEnter);

	for (int i = 0; i < NUM_CHARACTER_ASSETS; i++)
	{
		CP_Image_Free(customerAssets[i]);
	}
}