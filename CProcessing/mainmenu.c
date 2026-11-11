#include "mainmenu.h"
#include "cprocessing.h"
#include "game.h"
#include "animations.h"

CP_Font myFont;
CP_Color red;
CP_Color black;
CP_Color green;
int i = 0; //used in array later 
const char start[5] = { 'S', 'T', 'A', 'R', 'T' }; //start text array
const char quit[4] = { 'Q', 'U', 'I', 'T' }; // quit text array
float text_TypeOff_x = 550;
float text_TypeOff_y = 300;
float currentKey_x;
float currentKey_y;
float start_position_x;

void Main_Menu_Init(void)
{
	myFont = CP_Font_Load("Assets/Exo2-Regular.ttf");
}

void Main_Menu_Update(void)
{
	red = CP_Color_Create(255, 0, 0, 255);
	black = CP_Color_Create(0, 0, 0, 255);
	green = CP_Color_Create(0, 255, 0, 255);
	
	CP_Graphics_ClearBackground(CP_Color_Create(255, 255, 255, 255));

	//Render Title of game
	CP_Settings_Fill(black);
	CP_Settings_TextSize(150.0f);
	CP_Font_DrawText("Type Off!:", text_TypeOff_x, text_TypeOff_y);
	CP_Font_DrawText("Kitchen Rush", text_TypeOff_x - 100, text_TypeOff_y + 150);

	//rendering start and quit
	CP_Settings_Fill(black);
	CP_Settings_TextSize(100.0f);
	CP_Font_DrawText(start, text_TypeOff_x + 150, text_TypeOff_y + 280);
	CP_Font_DrawText(quit, text_TypeOff_x + 180, text_TypeOff_y + 380);


	if (CP_Input_KeyTriggered(KEY_S))
	{
		CP_Engine_SetNextGameState(Game_Init, Game_Update, Game_Exit);
	}

}

void Main_Menu_Exit(void)
{
	CP_Font_Free(myFont);
}