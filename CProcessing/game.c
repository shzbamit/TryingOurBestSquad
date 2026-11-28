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
#include "leaderboard.h"
#include "score.h"
#include "timer.h"
#include "debug.h"
#include "mainmenu.h"

// Assets
CP_Font myFont;
CP_Sound bgm;
CP_Sound sfxServe;
CP_Sound sfxDiscard; 
CP_Sound sfxLeave; 
CP_Sound sfxTick;
CP_Sound sfxCustomerEnter;

CP_Image background;
CP_Image foreground;
CP_Image scoreBoard;

CP_Image nameInputField;
CP_Image infoPage;

// Colours
#define BG_COLOUR					CP_Color_Create(247, 252, 252, 255) // Background
#define TIMER_COLOUR				CP_Color_Create(128, 128, 128, 255)
static int black = 0;
static int white = 255;
static int grey = 128;

// Pause
int isPaused = 0;

int enteredName = 0; // Tracks whether the player has entered their name for the leaderboard

//gameEnd
int gameEnd = 0;
int restartProgress = 0;
int resumeProgress = 0;
static int quitProgress = 0;
int pauseProgress = 0;
static int infoProgress = 0;
int menuProgress = 0;


static int leaderboardUpdated = 0;
static int infoVisible = 0;

// Tracks whether the debug menu has been turned on or not
// Can be used to disable certain features later, such as disabling highscore 
static int debugEnabled = 0;

enum { CONFIRM_NONE, CONFIRM_RESTART, CONFIRM_QUIT, CONFIRM_MENU };
static int confirmAction = CONFIRM_NONE;
static int yesProgress = 0;
static int noProgress = 0;

void Game_Init(void)
{
	debugActive = 0; // Reset debug menu state
	enteredName = 0; // Reset name input state
	leaderboardUpdated = 0; // Reset leaderboard update state

	// Load game menu structs
	gameStructInitialization();

	// Load Assets
	InitializeIngredientList();

	// Load customer character assets
	customerAssets[0] = CP_Image_Load("Assets/Customer 1.png");
	customerAssets[1] = CP_Image_Load("Assets/Customer 2.png");
	customerAssets[2] = CP_Image_Load("Assets/Customer 3.png");
	customerAssets[3] = CP_Image_Load("Assets/Customer 4.png");
	customerAssets[4] = CP_Image_Load("Assets/Customer 5.png");
	customerAssets[5] = CP_Image_Load("Assets/Customer 6.png");
	customerAssets[6] = CP_Image_Load("Assets/Customer 7.png");
	customerAssets[7] = CP_Image_Load("Assets/Customer 8.png");
	customerAssets[8] = CP_Image_Load("Assets/Customer 9.png");

	// Load foreground and background assets
	background = CP_Image_Load("Assets/Backgroud.png");
	foreground = CP_Image_Load("Assets/foreground2.png");
	scoreBoard = CP_Image_Load("Assets/Scoreboard .png");

	nameInputField = CP_Image_Load("Assets/name_input_field.png");

	infoPage = CP_Image_Load("Assets/info.png");

	myFont = CP_Font_Load("Assets/Exo2-Regular.ttf");

	// Load sounds 
	bgm = CP_Sound_LoadMusic("Assets/bgm.mp3");
	sfxServe = CP_Sound_Load("Assets/serve.mp3");
	sfxDiscard = CP_Sound_Load("Assets/discard.mp3");
	sfxLeave = CP_Sound_Load("Assets/leave.mp3");
	sfxTick = CP_Sound_Load("Assets/tick.mp3");
	sfxCustomerEnter = CP_Sound_Load("Assets/CustomerEnter.mp3");

	InitializeScore();

	srand((unsigned int)time(NULL));

	// Customer queue
	SpawnInitialCustomers();

	InitializeTimer();

	ResetWordColor(info, INFO, &infoProgress, white);

	// Play music on loop: volume, pitch, looping, group
	CP_Sound_PlayAdvanced(bgm, 0.4f, 1.0f, TRUE, CP_SOUND_GROUP_MUSIC);
}

void Game_Update(void)
{
	if (total_seconds < 0)
	{
		gameEnd = 1;
	}

	// check if the player typed the whole pause
	if (confirmAction == CONFIRM_NONE && pauseProgress == PAUSE)
	{
		isPaused = !isPaused;
		ResetWordColor(pause, PAUSE, &pauseProgress, grey);
	}

	score = CP_Math_ClampInt(score, 0, MAX_SCORE);

	CP_Graphics_ClearBackground(BG_COLOUR); // Don't move this, this is for clearing the canvas at the start of every update

	// Render game assets here ==========================================================================================================

	CP_Image_Draw(background, CP_System_GetWindowWidth() / 2, CP_System_GetWindowHeight() / 2, CP_System_GetWindowWidth(), CP_System_GetWindowHeight(), 255);

	// Display all customer orders
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(25.0f);
	CP_Font_Set(myFont);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_BASELINE);

	


	// Draw customers in the queue
	for (int i = 0; i < customerCount; i++)
	{
		if (!isPaused && enteredName && confirmAction == CONFIRM_NONE) {
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
			// ratio = fraction of patience left (1.0 = full, 0.0 = empty)
			float ratio = customers[i].patience / customers[i].maxPatience;

			// The bar is as wide as the customer image and 15 pixels tall
			float barWidth = customers[i].width;
			float barHeight = 15.0f;

			// Customers are drawn from their centre, but the bar is drawn from its top - left corner, so subtract half the width to find the left edge
			float barX = customers[i].animState.currentPos.x - barWidth / 2.0f;

			// Start at the customer's centre, go up half their height to reach the top of the image, then go up 25 more pixels so the bar floats just above their head
			float barY = customers[i].animState.currentPos.y - customers[i].height / 2.0f - 25.0f;

			// Draw rectangles from their top-left corner, with no outline
			CP_Settings_RectMode(CP_POSITION_CORNER);
			CP_Settings_NoStroke();

			// Draw the grey background track at full width (this is the "empty" part of the bar)
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

			// Draw the coloured bar on top of the grey track
			// Width = full width * ratio, so it gets shorter as patience runs out
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

		// Only draw the layers if this leaving customer is the one who was successfully served
		if (leavingCustomers[i].animState.isAnimating)
		{
			RenderLeavingSandwich(positionFrame);
		}

		// Clean them up once they completely vanish off screen
		if (leavingCustomers[i].animState.animationProgress >= 1.0f)
		{
			ClearLeavingSandwich();

			ShiftCustomerArray(leavingCustomers, &leavingCustomerCount, i);
			i--;
		}
	}
	CP_Image_Draw(foreground, CP_System_GetWindowWidth() / 2, CP_System_GetWindowHeight() / 2, CP_System_GetWindowWidth(), CP_System_GetWindowHeight(), 255);
	CP_Image_Draw(scoreBoard, (CP_System_GetWindowWidth() / 2) - 100, CP_System_GetWindowHeight() / 2, CP_System_GetWindowWidth(), CP_System_GetWindowHeight(), 255);

	GameScreenOptions();
	// Draw the customer orders
	for (int i = 0; i < customerCount; i++)
	{
		// Get customer's current position
		float customerX = customers[i].animState.currentPos.x;
		float customerY = customers[i].animState.currentPos.y;

		float startX = customerX - customers[i].width / 2.0f - 115.0f;

		// Calculate background dimensions
		float backgroundWidth = 100.0f;
		float backgroundHeight = customers[i].orderSize * 45.0f + 10.0f;

		// To start from the same Y-axis
		float bottomY = customerY + 100.0f;
		float startY = bottomY - backgroundHeight;

		// White background
		CP_Settings_RectMode(CP_POSITION_CORNER);
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
		CP_Settings_NoStroke();

		CP_Graphics_DrawRect(
			startX,
			startY,
			backgroundWidth,
			backgroundHeight
		);

		// Draw ingredient images
		for (int j = 0; j < customers[i].orderSize; j++)
		{
			int ingredientID = customers[i].order[j];

			float ingredientX = startX + backgroundWidth / 2.0f;
			float ingredientY = startY + (j * 45.0f) + 27.5f;

			// Dynamic green highlight tracker
			int isRowCorrect = 0;

			// Only evaluate if the player has actually stacked up to this row depth
			if (j < sandwichOrder)
			{
				// Check if EVERY SINGLE ingredient up to this point matches the customer's order perfectly
				int perfectStreak = 1;
				for (int k = 0; k <= j; k++)
				{
					if (sandwich[k].itemID != customers[i].order[k])
					{
						perfectStreak = 0; // The chain was broken by a wrong ingredient!
						break;
					}
				}
				isRowCorrect = perfectStreak;
			}

			// Draw green highlight behind matching ingredient
			if (isRowCorrect)
			{
				CP_Settings_Fill(CP_Color_Create(120, 230, 120, 255));
				CP_Settings_NoStroke();

				CP_Graphics_DrawRect(
					startX,
					ingredientY - 22.5f,
					backgroundWidth,
					45.0f
				);
			}

			CP_Image_Draw(
				ingredients[ingredientID].asset,
				ingredientX,
				ingredientY,
				45.0f,
				45.0f,
				255
			);
		}
	}

	RenderScore();

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
	CP_Settings_TextSize(TIMER_TEXT_SIZE);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_BASELINE);
	CP_Settings_Fill(TIMER_COLOUR);
	CP_Font_DrawText(timerText, 1450.0f, 100.0f);

	RenderSandwich();
	// Updates physics positions and prints all falling discarded ingredients
	UpdateAndRenderDiscards();

	debugEnabled = AllowDebug(); // Allow capturing debug key input to debug

	// End of asset renders =====================================================================================================================
	if (CP_Input_KeyTriggered(pause[pauseProgress].keybind) && !isPaused && gameEnd == 0 && enteredName == 1)
	{
		pause[pauseProgress].r = 195;
		pause[pauseProgress].g = 92;
		pause[pauseProgress].b = 250;
		pauseProgress++;
	}

	else
	{
		WrongKey(pause, PAUSE, &pauseProgress, grey);
	}

	if(!isPaused && gameEnd == 0 && enteredName && confirmAction == CONFIRM_NONE && pauseProgress == 0) {
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
				OnIngredientDiscarded();
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
				OnIngredientDiscarded();
			}
		}


		sandwichOrder = CP_Math_ClampInt(sandwichOrder, 0, MAX_SANDWICH_SIZE);

		int matchedCustomer = FindMatchingCustomer();

		// If a match is found, complete the order and update the score
		if (matchedCustomer != -1)
		{

			// For bonus points
			float patienceRatio = customers[matchedCustomer].patience / customers[matchedCustomer].maxPatience; // Calculate the ratio of remaining patience to maximum patience
			int speedBonus = (int)(SCORE_SPEED_BONUS_MAX * patienceRatio); // Calculate the speed bonus based on the patience ratio
			
			OnCustomerServed(SCORE_ORDER + speedBonus);

			CaptureLeavingSandwich();

			CompleteCustomer(matchedCustomer); // Remove the customer from the queue and mark them as leaving
			CP_Sound_PlayAdvanced(sfxServe, 1.0f, 1.0f, FALSE, CP_SOUND_GROUP_SFX);

			sandwichOrder = 0;
		}

		UpdateCustomerSpawning();

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
				OnCustomerLeft();

				CompleteCustomer(i);
			}
		}

		// Updates the ticker interpolator every single frame automatically
		UpdateScoreCounter();

		// Timer check
		UpdateTimer();
	}

	if (!enteredName) {
		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 150));
		CP_Graphics_DrawRect(0.0f, 0.0f, 1600.0f, 900.0f);

		CP_Image_Draw(nameInputField, 800.0f, 450.0f, 600.0f, 600.0f, 255);

		NameInput(&enteredName);
	}

	if (gameEnd == 1 && leaderboardUpdated == 0)
	{
		UpdateLeaderboard(score);
		leaderboardUpdated = 1;
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
			CP_Font_DrawText("You definitely cheated", 800.0f, 240.0f);
		}

		else if (500 <= score && score < 10000)
		{
			CP_Font_DrawText("Good Job!", 800.0f, 240.0f);
		}

		else
		{
			CP_Font_DrawText("Skill Issue lol", 800.0f, 240.0f);
		}

		CP_Settings_TextSize(100.0f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText("Your Score:", 1200.0f, 350.0f);
		CP_Font_DrawText(scoreText, 1200.0f, 450.0f);

		DrawLeaderboard();

		endScreen();
	}

	if (confirmAction == CONFIRM_NONE && enteredName && (isPaused || gameEnd == 1))
	{
		if ((restartProgress <= 3 && resumeProgress <= 3) && (CP_Input_KeyTriggered(resume[resumeProgress].keybind) || CP_Input_KeyTriggered(restart[restartProgress].keybind))) {
			ResetWordColor(quit, 4, &quitProgress, white); //resets quit 
			ResetWordColor(menu, MENU, &menuProgress, white);
			ResetWordColor(info, INFO, &infoProgress, white);
			resume[resumeProgress].r = 0;
			resume[resumeProgress].b = 0;
			restart[restartProgress].r = 0;
			restart[restartProgress].b = 0;
			restartProgress++;
			resumeProgress++;
		}

		else if (quitProgress == 0 && CP_Input_KeyTriggered(info[infoProgress].keybind))
		{
			ResetWordColor(resume, RESUME, &resumeProgress, white);
			ResetWordColor(restart, RESTART, &restartProgress, white);
			ResetWordColor(quit, QUIT, &quitProgress, white);
			ResetWordColor(menu, MENU, &menuProgress, white);
			info[infoProgress].r = 0;
			info[infoProgress].b = 0;
			infoProgress++;
		}

		else if (CP_Input_KeyTriggered(quit[quitProgress].keybind))
		{
			ResetWordColor(resume, RESUME, &resumeProgress, white); 
			ResetWordColor(restart, RESTART, &restartProgress, white);
			ResetWordColor(info, INFO, &infoProgress, white);
			ResetWordColor(menu, MENU, &menuProgress, white);
			quit[quitProgress].g = 0;
			quit[quitProgress].b = 0;
			quitProgress++;
		}

		else if (resumeProgress > 3 && CP_Input_KeyTriggered(resume[resumeProgress].keybind))
		{
			ResetWordColor(restart, RESTART, &restartProgress, white);
			resume[resumeProgress].r = 0;
			resume[resumeProgress].b = 0;
			resumeProgress++;
		}

		else if (restartProgress > 3 && CP_Input_KeyTriggered(restart[restartProgress].keybind))
		{
			ResetWordColor(resume, RESUME, &resumeProgress, white);
			restart[restartProgress].r = 0;
			restart[restartProgress].b = 0;
			restartProgress++;
		}

		else if (CP_Input_KeyTriggered(menu[menuProgress].keybind) && gameEnd == 0)
		{
			ResetWordColor(resume, RESUME, &resumeProgress, white);
			ResetWordColor(restart, RESTART, &restartProgress, white);
			ResetWordColor(quit, QUIT, &quitProgress, white);
			ResetWordColor(info, INFO, &infoProgress, white);
			menu[menuProgress].r = 0;
			menu[menuProgress].b = 0;
			menuProgress++;
		}

		else
		{
			WrongKey(menu, MENU, &menuProgress, white);
			WrongKey(restart, RESTART, &restartProgress, white);
			WrongKey(quit, QUIT, &quitProgress, white);
			WrongKey(resume, RESUME, &resumeProgress, white);
			WrongKey(info, INFO, &infoProgress, white);
		}
	}

	if (restartProgress == RESTART)
	{
		ResetWordColor(restart, RESTART, &restartProgress, white);
		confirmAction = CONFIRM_RESTART;   // ask first
	}

	if (quitProgress == QUIT)
	{
		ResetWordColor(quit, QUIT, &quitProgress, white);
		confirmAction = CONFIRM_QUIT;      // ask first
	}

	if (resumeProgress == RESUME)
	{
		ResetWordColor(resume, RESUME, &resumeProgress, white);
		isPaused = !isPaused;
	}

	if (infoProgress == INFO)
	{
		infoVisible = 1;
	}

	if (menuProgress == MENU)
	{
		ResetWordColor(menu, MENU, &menuProgress, white);
		confirmAction = CONFIRM_MENU;
	}

	if (isPaused && gameEnd == 0)
	{
		pauseScreen();

		if (confirmAction == CONFIRM_NONE && CP_Input_KeyTriggered(KEY_ESCAPE))
		{
			confirmAction = CONFIRM_QUIT;
		}
	}

	if (confirmAction != CONFIRM_NONE)
	{
		const char* msg = "Are you sure?";
		if (confirmAction == CONFIRM_RESTART)   msg = "Restart the game?";
		else if (confirmAction == CONFIRM_QUIT) msg = "Exit the game?";
		else if (confirmAction == CONFIRM_MENU) msg = "Go back to main menu?";

		confirmScreen(msg);

		if (CP_Input_KeyTriggered(confirmYes[yesProgress].keybind))
		{
			ResetWordColor(confirmNo, 2, &noProgress, white);
			confirmYes[yesProgress].r = 0;
			confirmYes[yesProgress].b = 0;
			yesProgress++;
		}
		else if (CP_Input_KeyTriggered(confirmNo[noProgress].keybind))
		{
			ResetWordColor(confirmYes, 3, &yesProgress, white);
			confirmNo[noProgress].g = 0;
			confirmNo[noProgress].b = 0;
			noProgress++;
		}
		else
		{
			WrongKey(confirmYes, 3, &yesProgress, white);
			WrongKey(confirmNo, 2, &noProgress, white);
		}

		if (noProgress == 2)   // NO typed: cancel
		{
			ResetWordColor(confirmNo, 2, &noProgress, white);
			confirmAction = CONFIRM_NONE;
		}

		if (yesProgress == 3)  // YES typed: do the action
		{
			int action = confirmAction;
			ResetWordColor(confirmYes, 3, &yesProgress, white);
			confirmAction = CONFIRM_NONE;

			if (action == CONFIRM_RESTART)
			{
				while (sandwichOrder > 0)
				{
					RemoveIngredient();
					sandwichOrder--;
				}
				gameEnd = 0;
				isPaused = 0;
				score = 0;
				ResetStreak();
				CP_Engine_SetNextGameStateForced(Game_Init, Game_Update, Game_Exit);
			}
			else if (action == CONFIRM_QUIT)
			{
				CP_Engine_Terminate();
			}
			else if (action == CONFIRM_MENU)
			{
				while (sandwichOrder > 0)
				{
					RemoveIngredient();
					sandwichOrder--;
				}
				gameEnd = 0;
				isPaused = 0;
				score = 0;
				ResetStreak();
				CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
			}
		}
	}

	if (infoVisible)
	{
		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 100));
		CP_Graphics_DrawRect(800.0f, 450.0f, 1600.0f, 900.0f);
		CP_Image_Draw(infoPage, 800.0f, 450.0f, 850.0f, 850.0f, 255);
		if (CP_Input_KeyTriggered(KEY_X)) {
			infoVisible = 0;
			ResetWordColor(info, INFO, &infoProgress, white);
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
	CP_Image_Free(egg.asset);
	CP_Image_Free(sausage.asset);
	CP_Image_Free(mayo.asset);
	CP_Font_Free(myFont);
	CP_Sound_Free(bgm);
	CP_Sound_Free(sfxServe);
	CP_Sound_Free(sfxDiscard);
	CP_Sound_Free(sfxLeave);
	CP_Sound_Free(sfxTick);
	CP_Sound_Free(sfxCustomerEnter);

	CP_Image_Free(background);
	CP_Image_Free(foreground);
	CP_Image_Free(scoreBoard);
	CP_Image_Free(nameInputField);
	CP_Image_Free(infoPage);

	for (int i = 0; i < NUM_CHARACTER_ASSETS; i++)
	{
		CP_Image_Free(customerAssets[i]);
	}
}