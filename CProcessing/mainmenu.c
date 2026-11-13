#include "mainmenu.h"
#include "cprocessing.h"
#include "game.h"
#include "animations.h"
#include "letters.h"
CP_Font myFont;
CP_Color red;
CP_Color black;
CP_Color green;

//Title position
float text_TypeOff_x = 550;
float text_TypeOff_y = 300;

int startProgress = 0; // To help check against current key triggered for start
static int quitProgress = 0; //To help check against current key triggered for quit

static int gameState = 0;

void Main_Menu_Init(void)
{
	myFont = CP_Font_Load("Assets/Exo2-Regular.ttf");

	start[0] = (struct letterState){ 0,0,0,KEY_S };
	start[1] = (struct letterState){ 0,0,0,KEY_T };
	start[2] = (struct letterState){ 0,0,0,KEY_A };
	start[3] = (struct letterState){ 0,0,0,KEY_R };
	start[4] = (struct letterState){ 0,0,0,KEY_T };

	quit[0] = (struct letterState){ 0,0,0,KEY_Q };
	quit[1] = (struct letterState){ 0,0,0,KEY_U };
	quit[2] = (struct letterState){ 0,0,0,KEY_I };
	quit[3] = (struct letterState){ 0,0,0,KEY_T };
}

void Main_Menu_Update(void)
{

	black = CP_Color_Create(0, 0, 0, 255);
	
	CP_Graphics_ClearBackground(CP_Color_Create(255, 255, 255, 255));
	initializeLetters(); //Initialize start and quit

	//Render Title of game
	CP_Settings_Fill(black);
	CP_Settings_TextSize(150.0f);
	CP_Font_DrawText("Type Off!:", text_TypeOff_x, text_TypeOff_y);
	CP_Font_DrawText("Kitchen Rush", text_TypeOff_x - 100, text_TypeOff_y + 150);

	
	if (CP_Input_KeyTriggered(start[startProgress].keybind))
	{
		resetWordsBlack(quit, 4, &quitProgress); //resets quit 
		start[startProgress].g = 255;
		startProgress++;
	}
	else if (CP_Input_KeyTriggered(quit[quitProgress].keybind))
	{
		resetWordsBlack(start, 5, &startProgress);
		quit[quitProgress].r = 255;
		quitProgress++;
	}
	else
	{
		wrongKey(start, 5, &startProgress, gameState);
		wrongKey(quit, 4, &quitProgress, gameState);
	}

	if (startProgress == 5)
	{
		CP_Engine_SetNextGameState(Game_Init, Game_Update, Game_Exit);
	}

	if (quitProgress == 4)
	{
		CP_Engine_Terminate();
	}

}

void Main_Menu_Exit(void)
{
	CP_Font_Free(myFont);
}