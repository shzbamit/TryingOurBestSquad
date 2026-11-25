#include "letters.h"
float x_position = 700;
float y_position = 580;
float endX_position = 1050;
float r_x_position = 740;
struct letterState start[START];
struct letterState quit[QUIT];
struct letterState credits[CREDITS];
struct letterState restart[RESTART];
struct letterState resume[RESUME];
struct letterState back[BACK];
struct letterState confirmYes[3];
struct letterState confirmNo[2];

//detects which key is being pressed if it does not match the struct array
void WrongKey(struct letterState arr[], int size, int* progress, int color)
{
	//loop to check which key was pressed
	for (int k = KEY_A; k <= KEY_Z; k++)
	{
		if (CP_Input_KeyTriggered(k))
		{
				ResetWordColor(arr, size, progress, color); //calls the function to reset the words to default state
				break;
		}
	}
}

//Reseting the wordsto default color if wrong key is pressed
void ResetWordColor(struct letterState arr[], int size, int* progress, int color)
{
	for (int i = 0; i < size; i++)
	{
		arr[i].r = color;
		arr[i].g = color;
		arr[i].b = color;
	}
	*progress = 0;
}

void InitializeLetters(void)
{
	//Start text
	CP_Settings_TextSize(100);
	GenerateLetters(start, START, x_position, y_position,47.0,100);

	//Quit text
	GenerateLetters(quit, QUIT, x_position + 30, y_position + 200, 50.0,100);

	// Credits text
	GenerateLetters(credits, CREDITS, x_position - 45, y_position + 100, 50.0,100);
}

void CreditBack(float x, float y)
{
	// Back text
	GenerateLetters(back, BACK, x, y, 15.0, 20);
}

void endScreen(void)
{
	//Restart text
	GenerateLetters(restart, RESTART, endX_position + 40, y_position, 35, 100);
	

	//Quit text
	CP_Settings_Fill(CP_Color_Create(quit[0].r, quit[0].g, quit[0].b, 255));
	CP_Font_DrawText("Q", endX_position + 100, y_position + 60);
	CP_Settings_Fill(CP_Color_Create(quit[1].r, quit[1].g, quit[1].b, 255));
	CP_Font_DrawText("U", endX_position + 140, y_position + 60);
	CP_Settings_Fill(CP_Color_Create(quit[2].r, quit[2].g, quit[2].b, 255));
	CP_Font_DrawText("I", endX_position + 170, y_position + 60);
	CP_Settings_Fill(CP_Color_Create(quit[3].r, quit[3].g, quit[3].b, 255));
	CP_Font_DrawText("T", endX_position + 195, y_position + 60);
}

void GenerateLetters(struct letterState arr[], int size, float position_x, float position_y, float spacing, float fontSize)
{
	for (int i = 0; i < size; i++)
	{
		char text[2] = { arr[i].letter, '\0' };   
		
		if (text[0] == 'I') //adjust x spacing when there is an I
		{
			if (fontSize >= 30)
			{
				CP_Settings_Fill(CP_Color_Create(arr[i].r, arr[i].g, arr[i].b, 255));
				CP_Font_DrawText(text, (float)(position_x + i * spacing), position_y);
				position_x -= spacing / 1.5;
			}

			else
			{
				position_x -= spacing / 2.5;
				CP_Settings_Fill(CP_Color_Create(arr[i].r, arr[i].g, arr[i].b, 255));
				CP_Font_DrawText(text, (float)(position_x + i * spacing), position_y);
				position_x -= spacing / 2.5;
			}
		}	

		else
		{
			CP_Settings_Fill(CP_Color_Create(arr[i].r, arr[i].g, arr[i].b, 255));
			CP_Font_DrawText(text, (float)(position_x + i * spacing), position_y);
		}
	}
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
	CP_Font_DrawText("Type                    to resume", 800.0f, 490.0f);
	CP_Settings_Fill(CP_Color_Create(resume[0].r, resume[0].g, resume[0].b, 255));
	GenerateLetters(resume, 6, r_x_position, 490.0f, 13.5, 25.0);

	CP_Settings_TextSize(25.0f);
	GenerateLetters(quit, 4, r_x_position + 28 , 530.0f, 14, 25.0);
	CP_Font_DrawText("Type            to quit", 790.0, 530.0f);

	// RESTART option: "type" + letters + "to restart", all left-aligned
	CP_Settings_TextSize(25.0f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);

	float lineX = 630.0f;   // left edge of the whole line
	float lineY = 570.0f;

	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Font_DrawText("Type", lineX, lineY);

	// Letters start after "type'
	float lettersX = lineX + 55.0f;
	for (int i = 0; i < 7; i++)
	{
		char text[2] = { restart[i].letter, '\0' };
		CP_Settings_Fill(CP_Color_Create(restart[i].r, restart[i].g, restart[i].b, 255));
		CP_Font_DrawText(text, lettersX + i * 18.0f, lineY);
	}

	// "to restart" starts after the letters, with a 20px gap
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Font_DrawText("to restart", lettersX + 7 * 18.0f + 10.0f, lineY);

	// Put the alignment back for anything drawn after this
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
}

// Draws a word letter by letter so each letter can have its own colour
static void DrawTypedWord(struct letterState arr[], int size, float x, float y)
{
	for (int i = 0; i < size; i++)
	{
		char text[2] = { arr[i].letter, '\0' };
		CP_Settings_Fill(CP_Color_Create(arr[i].r, arr[i].g, arr[i].b, 255));
		CP_Font_DrawText(text, x + i * 30.0f, y);
	}
}

// Confirmation dialog: dark box, a question, and YES / NO to type
void confirmScreen(const char* message)
{
	CP_Settings_RectMode(CP_POSITION_CORNER);
	CP_Settings_NoStroke();

	// Dim the whole screen, then draw the box
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 170));
	CP_Graphics_DrawRect(0.0f, 0.0f, 1600.0f, 900.0f);
	CP_Settings_Fill(CP_Color_Create(40, 40, 40, 255));
	CP_Graphics_DrawRect(500.0f, 320.0f, 600.0f, 260.0f);

	// The question
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Settings_TextSize(45.0f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText(message, 800.0f, 390.0f);

	// The two typed options
	CP_Settings_TextSize(35.0f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);

	DrawTypedWord(confirmYes, 3, 600.0f, 470.0f);
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Font_DrawText("- yes, continue", 720.0f, 470.0f);

	DrawTypedWord(confirmNo, 2, 600.0f, 530.0f);
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Font_DrawText("- no, go back", 720.0f, 530.0f);
}

void gameStructInitialization()
{
	resume[0] = (struct letterState){'R', 255,255,255,KEY_R};
	resume[1] = (struct letterState){ 'E',255,255,255,KEY_E};
	resume[2] = (struct letterState){ 'S',255,255,255,KEY_S};
	resume[3] = (struct letterState){ 'U',255,255,255,KEY_U};
	resume[4] = (struct letterState){ 'M',255,255,255,KEY_M};
	resume[5] = (struct letterState){ 'E',255,255,255,KEY_E};

	quit[0] = (struct letterState){ 'Q',255,255,255,KEY_Q};
	quit[1] = (struct letterState){ 'U',255,255,255,KEY_U};
	quit[2] = (struct letterState){ 'I',255,255,255,KEY_I};
	quit[3] = (struct letterState){ 'T',255,255,255,KEY_T};

	confirmYes[0] = (struct letterState){ 'Y',255,255,255,KEY_Y };
	confirmYes[1] = (struct letterState){ 'E',255,255,255,KEY_E };
	confirmYes[2] = (struct letterState){ 'S',255,255,255,KEY_S };

	confirmNo[0] = (struct letterState){ 'N',255,255,255,KEY_N };
	confirmNo[1] = (struct letterState){ 'O',255,255,255,KEY_O };

	restart[0] = (struct letterState){ 'R',255,255,255,KEY_R};
	restart[1] = (struct letterState){ 'E',255,255,255,KEY_E};
	restart[2] = (struct letterState){ 'S',255,255,255,KEY_S};
	restart[3] = (struct letterState){ 'T',255,255,255,KEY_T};
	restart[4] = (struct letterState){ 'A',255,255,255,KEY_A};
	restart[5] = (struct letterState){ 'R',255,255,255,KEY_R};
	restart[6] = (struct letterState){ 'T',255,255,255,KEY_T};
}