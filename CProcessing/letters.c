#include "letters.h"
int x_position = 700;
int y_position = 580;
struct letterState start[5];
struct letterState quit[4];

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