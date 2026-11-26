#include "mainmenu.h"
#include "cprocessing.h"
#include "game.h"
#include "animations.h"
#include "letters.h"

CP_Font myFont;
CP_Color red;
CP_Color green;

//Title position
float text_TypeOff_x = 550;
float text_TypeOff_y = 300;

int startProgress = 0; // To help check against current key triggered for start
static int quitProgress = 0; //To help check against current key triggered for quit
int creditsProgress = 0;
int backProgress = 0; // For back function on credit screen

static int black = 0;
static int white = 255;

#define SPLASH_SCREEN_TIME 4.0f // Total splash screen time: 2 seconds for digipen logo and 2s for team logo
float splashScreenTimer = 0.0f; // For tracking how much time has passed in splash screen

#define BLACK_COLOUR CP_Color_Create(0, 0, 0, 255)
#define WHITE_COLOUR CP_Color_Create(255, 255, 255, 255)
#define CENTER_X 800.0f
#define CENTER_Y 450.0f

int isSplashScreen = 1; // Boolean

CP_Image digipenLogo;
#define DIGIPEN_LOGO_W 500.0f
#define DIGIPEN_LOGO_H 225.0f

const char copyrightText[] = { "All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved." };
#define COPYRIGHT_TEXT_POS_Y (CENTER_Y + 200.0f)
#define COPYRIGHT_TEXT_SIZE 30.0f

char teamLogo[] = { "TryingOurBestSquad!" }; // Replace with logo if we make one
#define TEAM_LOGO_SIZE 150.0f

int creditsVisible = 0;
#define CREDITS_BOX_W 1400.0f
#define CREDITS_BOX_H 800.0f

#define CREDITS_DIGIPEN_LOGO_W 250.0f
#define CREDITS_DIGIPEN_LOGO_H 95.0f
#define CREDITS_DIGIPEN_LOGO_POS_X ((1600.0f - CREDITS_BOX_W) + (CREDITS_DIGIPEN_LOGO_W / 4.0f))
#define CREDITS_DIGIPEN_LOGO_POS_Y ((900.0f - CREDITS_BOX_H) + (CREDITS_DIGIPEN_LOGO_H / 2.0f))

#define CREDITS_COPYRIGHT_POS_X (CREDITS_DIGIPEN_LOGO_POS_X + CREDITS_DIGIPEN_LOGO_W - 50.0f)
#define CREDITS_COPYRIGHT_POS_Y (CREDITS_DIGIPEN_LOGO_POS_Y - (CREDITS_DIGIPEN_LOGO_H / 2.0f))

// Array of lines to display in the credits panel
const char* creditsContent[] = {
	"TEAM MEMBERS",
	"+++++++++++++",
	"Tan Bao Yi",
	"Eliza Siew",
	"Nur Shazana",
	"Guo Qi"
};
int creditsLines = sizeof(creditsContent) / sizeof(creditsContent[0]);

#define CREDITS_TEXT_STARTING_X (CREDITS_DIGIPEN_LOGO_POS_X - (CREDITS_DIGIPEN_LOGO_W / 2.0f))
#define CREDITS_TEXT_STARTING_Y (CREDITS_DIGIPEN_LOGO_POS_Y + CREDITS_DIGIPEN_LOGO_H)

const char* creditsContent2[] = {
	"INSTRUCTORS",
	"=====================================",
	"Gerald Han Feng Wong",
	"Parminder Singh",
	"Tommy Tan",
	"Soroor Malekmohammadi Faradounbeh",
	"Richard Tsao",
	"",
	"PRESIDENT",
	"=====================================",
	"Claude Comair",
	"",
	"EXECUTIVES",
	"=====================================",
	"CHU Jason Yeu Tat",
	"Michael GATS",
	"TAN Chek Ming",
	"Prasanna Kumar GHALI",
	"Mandy WONG",
	"Johnny DEEK",
	"",
	"~~Type BACK to go back!"
};
int creditsLines2 = sizeof(creditsContent2) / sizeof(creditsContent2[0]);

#define CREDITS_TEXT2_STARTING_X CREDITS_COPYRIGHT_POS_X
#define CREDITS_TEXT2_STARTING_Y (CREDITS_COPYRIGHT_POS_Y + (COPYRIGHT_TEXT_SIZE * 2.0f))

#define BACK_TEXT_POS_X CREDITS_BOX_W
#define BACK_TEXT_POS_Y CREDITS_BOX_H

void Main_Menu_Init(void)
{
	myFont = CP_Font_Load("Assets/Exo2-Regular.ttf");
	digipenLogo = CP_Image_Load("Assets/DigiPen_Singapore_WEB_RED.png");

	start[0] = (struct LetterState){ 'S',0,0,0,KEY_S};
	start[1] = (struct LetterState){ 'T',0,0,0,KEY_T};
	start[2] = (struct LetterState){ 'A',0,0,0,KEY_A};
	start[3] = (struct LetterState){ 'R',0,0,0,KEY_R };
	start[4] = (struct LetterState){ 'T',0,0,0,KEY_T };

	quit[0] = (struct LetterState){ 'Q',0,0,0,KEY_Q };
	quit[1] = (struct LetterState){ 'U',0,0,0,KEY_U };
	quit[2] = (struct LetterState){ 'I',0,0,0,KEY_I };
	quit[3] = (struct LetterState){ 'T',0,0,0,KEY_T };

	credits[0] = (struct LetterState){ 'C',0,0,0,KEY_C };
	credits[1] = (struct LetterState){ 'R',0,0,0,KEY_R };
	credits[2] = (struct LetterState){ 'E',0,0,0,KEY_E };
	credits[3] = (struct LetterState){ 'D',0,0,0,KEY_D };
	credits[4] = (struct LetterState){ 'I',0,0,0,KEY_I };
	credits[5] = (struct LetterState){ 'T',0,0,0,KEY_T };
	credits[6] = (struct LetterState){ 'S',0,0,0,KEY_S };

	back[0] = (struct LetterState){ 'B',255,255,255,KEY_B };
	back[1] = (struct LetterState){ 'A',255,255,255,KEY_A };
	back[2] = (struct LetterState){ 'C',255,255,255,KEY_C };
	back[3] = (struct LetterState){ 'K',255,255,255,KEY_K };
}

void Main_Menu_Update(void)
{
	if (isSplashScreen && !creditsVisible) // Splash Screen Segment
	{
		// Accumulate time passed since the last frame using CProcessing's delta time
		splashScreenTimer += CP_System_GetDt();

		// Keep the screen completely black
		CP_Graphics_ClearBackground(BLACK_COLOUR);

		// If 4 seconds have elapsed, turn off the splash screen mode
		if (splashScreenTimer >= SPLASH_SCREEN_TIME)
		{
			isSplashScreen = 0;
		}
		else if (splashScreenTimer >= (SPLASH_SCREEN_TIME / 2.0f)) // 2 Seconds
		{
			CP_Settings_Fill(CP_Color_Create(255, 255, 255, (int)(255.0f * (splashScreenTimer - (SPLASH_SCREEN_TIME / 2.0f))))); // White, fades in
			CP_Settings_TextSize(TEAM_LOGO_SIZE);
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
			CP_Font_DrawText(teamLogo, CENTER_X, CENTER_Y);
		}
		else
		{
			// Fades in
			CP_Image_Draw(digipenLogo, CENTER_X, CENTER_Y, DIGIPEN_LOGO_W, DIGIPEN_LOGO_H, (int)(255.0f * splashScreenTimer));
			CP_Settings_Fill(CP_Color_Create(255, 255, 255, (int)(255.0f * splashScreenTimer)));
			CP_Settings_TextSize(COPYRIGHT_TEXT_SIZE);
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
			CP_Font_DrawText(copyrightText, CENTER_X, COPYRIGHT_TEXT_POS_Y);
		}

		if (CP_Input_KeyTriggered(KEY_ENTER) && splashScreenTimer < 2.0f)
		{
			splashScreenTimer = 2.0f;
		}
		if (CP_Input_KeyTriggered(KEY_ENTER) && splashScreenTimer > 2.0f)
		{
			splashScreenTimer = 4.0f;
		}
	}

	if (!isSplashScreen && creditsVisible) // Credit Panel Open
	{
		CP_Settings_Fill(BLACK_COLOUR);
		CP_Graphics_DrawRect(CENTER_X, CENTER_Y, CREDITS_BOX_W, CREDITS_BOX_H);

		CP_Image_Draw(digipenLogo, CREDITS_DIGIPEN_LOGO_POS_X, CREDITS_DIGIPEN_LOGO_POS_Y, CREDITS_DIGIPEN_LOGO_W, CREDITS_DIGIPEN_LOGO_H, 255);

		CP_Settings_Fill(WHITE_COLOUR);
		CP_Settings_TextSize(COPYRIGHT_TEXT_SIZE);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
		CP_Font_DrawText(copyrightText, CREDITS_COPYRIGHT_POS_X, CREDITS_COPYRIGHT_POS_Y);


		for (int i = 0; i < creditsLines; i++)
		{
			// Calculate dynamic line position base height
			float currentLineY = CREDITS_TEXT_STARTING_Y + (i * COPYRIGHT_TEXT_SIZE);
			CP_Font_DrawText(creditsContent[i], CREDITS_TEXT_STARTING_X, currentLineY);
		}

		for (int i = 0; i < creditsLines2; i++)
		{
			// Calculate dynamic line position base height
			float currentLineY = CREDITS_TEXT2_STARTING_Y + (i * COPYRIGHT_TEXT_SIZE);
			CP_Font_DrawText(creditsContent2[i], CREDITS_TEXT2_STARTING_X, currentLineY);
		}

		CreditBack(BACK_TEXT_POS_X, BACK_TEXT_POS_Y);

		if (CP_Input_KeyTriggered(back[backProgress].keybind))
		{
			back[backProgress].r = 0;
			back[backProgress].g = 255;
			back[backProgress].b = 0;
			backProgress++;
		}
		else
		{
			WrongKey(back, BACK, &backProgress, white); // gameState forced set to correct mode here
		}

		if (backProgress == BACK)
		{
			WrongKey(credits, CREDITS, &creditsProgress, white); // Reset CREDITS text
			creditsVisible = 0;
		}
	}

	if (!isSplashScreen && !creditsVisible) // Main Menu Screen
	{
		CP_Graphics_ClearBackground(WHITE_COLOUR);
		InitializeLetters(); //Initialize start and quit

		//Render Title of game
		CP_Settings_Fill(BLACK_COLOUR);
		CP_Settings_TextSize(150.0f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText("Type Off!:", text_TypeOff_x, text_TypeOff_y);
		CP_Font_DrawText("Kitchen Rush", text_TypeOff_x - 100, text_TypeOff_y + 150);

		CP_Settings_TextSize(60.0f);
		CP_Font_DrawText("Type :)", 500, 580);

		if (CP_Input_KeyTriggered(start[startProgress].keybind) && !CP_Input_KeyTriggered(credits[creditsProgress].keybind))
		{
			ResetWordColor(quit, QUIT, &quitProgress, black); //resets quit 
			ResetWordColor(credits, CREDITS, &creditsProgress, black);
			start[startProgress].g = 255;
			startProgress++;
		}
		else if (CP_Input_KeyTriggered(quit[quitProgress].keybind))
		{
			ResetWordColor(start, START, &startProgress, black);
			ResetWordColor(credits, CREDITS, &creditsProgress, black);
			quit[quitProgress].r = 255;
			quitProgress++;
		}
		else if (CP_Input_KeyTriggered(credits[creditsProgress].keybind))
		{
			ResetWordColor(start, START, &startProgress, black);
			ResetWordColor(quit, QUIT, &quitProgress, black);
			credits[creditsProgress].g = 255;
			creditsProgress++;
		}
		else
		{
			WrongKey(start, START, &startProgress, black);
			WrongKey(quit, QUIT, &quitProgress, black);
			WrongKey(credits, CREDITS, &creditsProgress, black);
		}

		if (startProgress == START)
		{
			CP_Settings_TextSize(60.0f);
			CP_Font_DrawText("Entering game ...", 950, 600);
			CP_Engine_SetNextGameState(Game_Init, Game_Update, Game_Exit);
		}

		if (quitProgress == QUIT)
		{
			CP_Engine_Terminate();
		}

		if (creditsProgress == CREDITS)
		{
			// Reset BACK text
			WrongKey(back, BACK, &backProgress, white); // gameState forced set to correct mode here
			creditsVisible = 1;
		}
	}

}

void Main_Menu_Exit(void)
{
	CP_Font_Free(myFont);
	CP_Image_Free(digipenLogo);
}