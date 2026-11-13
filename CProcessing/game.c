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
#include "customers.h"
#include "letters.h"


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

// Timer
int total_seconds =					300;	// 5 minutes
//int total_seconds =					5;	// remove after testing
int minutes =						0;
int seconds =						0;
char timerText[6];
int initialTime =					300; //used for reseting the time when restarting the game
clock_t next_tick;

// Pause
int isPaused = 0;

//gameEnd
int gameEnd = 0;
int restartProgress = 0;
int resumeProgress = 0;
static int quitProgress = 0;

//game state
static int gameState = 1;

void Game_Init(void)
{
	resume[0] = (struct letterState){ 255,255,255,KEY_R };
	resume[1] = (struct letterState){ 255,255,255,KEY_E };
	resume[2] = (struct letterState){ 255,255,255,KEY_S };
	resume[3] = (struct letterState){ 255,255,255,KEY_U };
	resume[4] = (struct letterState){ 255,255,255,KEY_M };
	resume[5] = (struct letterState){ 255,255,255,KEY_E };

	quit[0] = (struct letterState){ 255,255,255,KEY_Q };
	quit[1] = (struct letterState){ 255,255,255,KEY_U };
	quit[2] = (struct letterState){ 255,255,255,KEY_I };
	quit[3] = (struct letterState){ 255,255,255,KEY_T };

	restart[0] = (struct letterState){ 255,255,255,KEY_R };
	restart[1] = (struct letterState){ 255,255,255,KEY_E };
	restart[2] = (struct letterState){ 255,255,255,KEY_S };
	restart[3] = (struct letterState){ 255,255,255,KEY_T };
	restart[4] = (struct letterState){ 255,255,255,KEY_A };
	restart[5] = (struct letterState){ 255,255,255,KEY_R };
	restart[6] = (struct letterState){ 255,255,255,KEY_T };

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
	// Check if space is pressed to pause game
	if (CP_Input_KeyTriggered(KEY_SPACE))
	{
		isPaused = !isPaused;
	}

	if (total_seconds < 0)
	{
		gameEnd = 1;
	}

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
		if (!isPaused) {
			// Interpolate coordinates via your custom function
			CP_Vector positionFrame = MoveImage(
				customers[i].animState.animSpeed * CP_System_GetDt(),
				customers[i].animState.startPos,
				customers[i].animState.targetPos,
				&customers[i].animState.animationProgress
			);

			// Store frame updates so downstream math remains fully synced
			customers[i].animState.currentPos = positionFrame;
		}

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
		if (!customers[i].animState.isAnimating && customers[i].animState.currentPos.x + customers[i].width / 2.0f <= (float)CP_System_GetWindowWidth()) // When the customer is not leaving & customer's right edge is fully on screen
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
		// CP_System_GetSeconds() = seconds since the game started (e.g. 12.37)
		// * 4.0     -> the number goes up by 1 every 0.25 seconds
		// (int)     -> removes the decimal part (49.48 becomes 49)
		// % 2       -> remainder after dividing by 2, so it alternates 0, 1, 0, 1...
		// == 0      -> true when the remainder is 0, false when it is 1
		// Result: flashOn switches between true and false every 0.25 seconds
		int flashOn = ((int)(CP_System_GetSeconds() * 4.0) % 2) == 0;

		if (flashOn)
		{
			// Change the drawing colour to red (red, green, blue, opacity)
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

	if (!isPaused && gameEnd == 0) {
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

			while (sandwichOrder > 0) // Repeats until it is no longer true
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
		for (int i = 0; i < customerCount; i++) // Go through each customer in the queue, one by one
		{
			// CP_System_GetDt() = time since the last frame (e.g. 0.016 seconds)
			// PATIENCE_SPEED = 0.5, so patience goes down by 0.5 every real second
			// Using dt keeps the speed the same on fast and slow computers
			customers[i].patience -= CP_System_GetDt() * PATIENCE_SPEED;

			// Play the warning sound once, when the customer has 4 secs left
			// Patience drains at 0.5 per second, so patience is NOT the same as seconds.
			// Dividing by PATIENCE_SPEED converts it into real seconds left.
			// Example: patience 1.5 / 0.5 = 3 seconds left
			float secondsLeft = customers[i].patience / PATIENCE_SPEED;

			// Play the warning sound only if BOTH are true:
			// 1) the sound has not been played yet for this customer (! means "not")
			// 2) the customer has 3 seconds or less left
			if (!customers[i].shakeSoundPlayed && secondsLeft <= 3.0f)
			{
				// Mark the sound as played FIRST, so this block never runs again for this customer.
				// Without this, the sound would replay every frame (about 60 times a second).
				customers[i].shakeSoundPlayed = 1;

				// Play the warning sound once (volume, pitch, looping off, sound group)
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

			// Render the update
			minutes = total_seconds / 60;
			seconds = total_seconds % 60;
			snprintf(timerText, sizeof(timerText), "%02d:%02d", minutes, seconds); // Converts min/sec int to char
			fflush(stdout); // Force engine to instantly display timer text
		}
	}
	
	// Exit game if timer ends
	if (gameEnd == 1)
	{
		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
		CP_Graphics_DrawRect(0.0f, 0.0f, 1600.0f, 900.0f);

		CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
		CP_Settings_TextSize(150.0f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_TOP);
		CP_Font_DrawText("GAME OVER", 800.0f, 100.0f);

		CP_Settings_TextSize(40.0f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_TOP);
		if (score >= 10000)
		{
			CP_Font_DrawText("You definitely cheated", 800.0f, 230.0f);
		}

		else if (500 <= score && score < 10000)
		{
			CP_Font_DrawText("Good Job!", 800.0f, 230.0f);
		}

		else
		{
			CP_Font_DrawText("Skill Issue lol", 800.0f, 230.0f);
		}

		CP_Settings_TextSize(100.0f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText("Your Score:", 800.0f, 350.0f);
		CP_Font_DrawText(scoreText, 800.0f, 430.0f);

		endScreen();
	}

	if (CP_Input_KeyTriggered(restart[restartProgress].keybind))
	{
		resetWordsWhite(quit, 4, &quitProgress); //resets quit 
		restart[restartProgress].r = 0;
		restart[restartProgress].b = 0;
		restartProgress++;
	}

	else if (CP_Input_KeyTriggered(quit[quitProgress].keybind))
	{
		resetWordsWhite(restart, 7, &restartProgress);
		quit[quitProgress].g = 0;
		quit[quitProgress].b = 0;
		quitProgress++;
	}
	else
	{
		wrongKey(restart, 7, &restartProgress, gameState);
		wrongKey(quit, 4, &quitProgress, gameState);
	}

	if (restartProgress == 7)
	{
		while (sandwichOrder > 0)
		{
			RemoveIngredient();
			sandwichOrder--;
		}
		gameEnd = 0;
		restartProgress = 0;
		quitProgress = 0;
		score = 0;
		total_seconds = initialTime;
		CP_Engine_SetNextGameStateForced(Game_Init, Game_Update, Game_Exit);
	}

	if (quitProgress == 4)
	{
		CP_Engine_Terminate();
	}

	if (resumeProgress == 6)
	{
		resumeProgress = 0;
		isPaused = !isPaused;
	}

	if (isPaused && gameEnd == 0)
	{
		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 150));
		CP_Graphics_DrawRect(0.0f, 0.0f, 1600.0f, 900.0f);

		CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
		CP_Settings_TextSize(60.0f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText("PAUSED", 800.0f, 420.0f);

		CP_Settings_TextSize(25.0f);
		CP_Font_DrawText("Press SPACE to resume", 800.0f, 490.0f);

		CP_Settings_TextSize(25.0f);
		CP_Font_DrawText("Press ESC to quit", 800.0f, 530.0f);

		if (CP_Input_KeyTriggered(KEY_ESCAPE)) {
			CP_Engine_Terminate();
		}
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