#include "letters.h"
float x_position = 700;
float y_position = 580;
float endX_position = 1050;
float r_x_position = 800;
struct letterState start[5];
struct letterState quit[4];
struct letterState restart[7];
struct letterState resume[6];

//detects which key is being pressed if it does not match the struct array
void wrongKey(struct letterState arr[], int size, int* progress, int gameState)
{
	//loop to check which key was pressed
	for (int k = KEY_A; k <= KEY_Z; k++)
	{
		if (CP_Input_KeyTriggered(k))
		{
			if(gameState == 0) // If its from main menu
			{
				resetWordsBlack(arr, size, progress); //calls the function to reset the words to default state
				break;
			}
			else //If it came from game
			{
				resetWordsWhite(arr, size, progress); //calls the function to reset the words to default state
				break;
			}
		}
	}
}

//Reseting the wordsto default color if wrong key is pressed
void resetWordsBlack(struct letterState arr[], int size, int* progress)
{
	for (int i = 0; i < size; i++)
	{
		arr[i].r = 0;
		arr[i].g = 0;
		arr[i].b = 0;
	}
	*progress = 0;
}

void resetWordsWhite(struct letterState arr[], int size, int* progress)
{
	for (int i = 0; i < size; i++)
	{
		arr[i].r = 255;
		arr[i].g = 255;
		arr[i].b = 255;
	}
	*progress = 0;
}
void initializeLetters(void)
{
	//Start text
	CP_Settings_TextSize(100);
	CP_Settings_Fill(CP_Color_Create(start[0].r, start[0].g, start[0].b, 255));
	CP_Font_DrawText("S", x_position, y_position);
	CP_Settings_Fill(CP_Color_Create(start[1].r, start[1].g, start[1].b, 255));
	CP_Font_DrawText("T", x_position + 50, y_position);
	CP_Settings_Fill(CP_Color_Create(start[2].r, start[2].g, start[2].b, 255));
	CP_Font_DrawText("A", x_position + 100, y_position);
	CP_Settings_Fill(CP_Color_Create(start[3].r, start[3].g, start[3].b, 255));
	CP_Font_DrawText("R", x_position + 150, y_position);
	CP_Settings_Fill(CP_Color_Create(start[4].r, start[4].g, start[4].b, 255));
	CP_Font_DrawText("T", x_position + 200, y_position);

	//Quit text
	CP_Settings_Fill(CP_Color_Create(quit[0].r, quit[0].g, quit[0].b, 255));
	CP_Font_DrawText("Q", x_position + 30, y_position + 100);
	CP_Settings_Fill(CP_Color_Create(quit[1].r, quit[1].g, quit[1].b, 255));
	CP_Font_DrawText("U", x_position + 80, y_position + 100);
	CP_Settings_Fill(CP_Color_Create(quit[2].r, quit[2].g, quit[2].b, 255));
	CP_Font_DrawText("I", x_position + 130, y_position + 100);
	CP_Settings_Fill(CP_Color_Create(quit[3].r, quit[3].g, quit[3].b, 255));
	CP_Font_DrawText("T", x_position + 150, y_position + 100);
}

void endScreen(void)
{
	//Restart text
	CP_Settings_Fill(CP_Color_Create(restart[0].r, restart[0].g, restart[0].b, 255));
	CP_Font_DrawText("R", endX_position , y_position);
	CP_Settings_Fill(CP_Color_Create(restart[1].r, restart[1].g, restart[1].b, 255));
	CP_Font_DrawText("E", endX_position + 50, y_position);
	CP_Settings_Fill(CP_Color_Create(restart[2].r, restart[2].g, restart[2].b, 255));
	CP_Font_DrawText("S", endX_position + 100, y_position);
	CP_Settings_Fill(CP_Color_Create(restart[3].r, restart[3].g, restart[3].b, 255));
	CP_Font_DrawText("T", endX_position + 150, y_position);
	CP_Settings_Fill(CP_Color_Create(restart[4].r, restart[4].g, restart[4].b, 255));
	CP_Font_DrawText("A", endX_position + 200, y_position);
	CP_Settings_Fill(CP_Color_Create(restart[5].r, restart[5].g, restart[5].b, 255));
	CP_Font_DrawText("R", endX_position + 250, y_position);
	CP_Settings_Fill(CP_Color_Create(restart[6].r, restart[6].g, restart[6].b, 255));
	CP_Font_DrawText("T", endX_position + 300, y_position);

	//Quit text
	CP_Settings_Fill(CP_Color_Create(quit[0].r, quit[0].g, quit[0].b, 255));
	CP_Font_DrawText("Q", endX_position + 50, y_position + 100);
	CP_Settings_Fill(CP_Color_Create(quit[1].r, quit[1].g, quit[1].b, 255));
	CP_Font_DrawText("U", endX_position + 100, y_position + 100);
	CP_Settings_Fill(CP_Color_Create(quit[2].r, quit[2].g, quit[2].b, 255));
	CP_Font_DrawText("I", endX_position + 140, y_position + 100);
	CP_Settings_Fill(CP_Color_Create(quit[3].r, quit[3].g, quit[3].b, 255));
	CP_Font_DrawText("T", endX_position + 170, y_position + 100);
}

void pauseScreen(void)
{
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 150));
	CP_Graphics_DrawRect(0.0f, 0.0f, 1600.0f, 900.0f);

	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Settings_TextSize(60.0f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText("PAUSED", 800.0f, 420.0f);

	CP_Settings_TextSize(25.0f);
	CP_Font_DrawText("Type        to resume", 800.0f, 490.0f);
	CP_Settings_Fill(CP_Color_Create(resume[0].r, resume[0].g, resume[0].b, 255));
	CP_Font_DrawText("R", r_x_position, 490.0f);

	CP_Settings_TextSize(25.0f);
	CP_Font_DrawText("type      to quit", 800.0f, 530.0f);
}

void gameStructInitialization()
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
}