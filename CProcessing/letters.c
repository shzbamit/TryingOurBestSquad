#include "letters.h"
int x_position = 700;
int y_position = 580;
struct letterState start[5];
struct letterState quit[4];

//detects which key is being pressed if it does not match the struct array
void wrongKey(struct letterState arr[], int size, int* progress)
{
	//loop to check which key was pressed
	for (int k = KEY_A; k <= KEY_Z; k++)
	{
		if (CP_Input_KeyTriggered(k))
		{
			resetWords(arr, size, progress); //calls the function to reset the words to default state
			break;
		}
	}
}

//Reseting the wordsto default color if wrong key is pressed
void resetWords(struct letterState arr[], int size, int* progress)
{
	for (int i = 0; i < size; i++)
	{
		arr[i].r = 0;
		arr[i].g = 0;
		arr[i].b = 0;
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