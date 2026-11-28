#pragma once
#include <cprocessing.h>
struct LetterState
{
	char letter;
	int r;
	int g;
	int b;
	CP_KEY keybind;
};

enum { START = 5, QUIT = 4, RESTART = 7, RESUME = 6, CREDITS = 7, BACK = 4, PAUSE = 5, INFO = 4, MENU = 4 };

extern struct LetterState start[START];
extern struct LetterState quit[QUIT];
extern struct LetterState restart[RESTART];
struct LetterState resume[RESUME];
extern struct LetterState credits[CREDITS];
extern struct LetterState back[BACK];
extern struct LetterState confirmYes[3];
extern struct LetterState confirmNo[2];
extern struct LetterState pause[PAUSE];
extern struct LetterState info[INFO];
extern struct LetterState menu[MENU];

void confirmScreen(const char* message);
void gameStructInitialization(void);
void InitializeLetters(void);
void CreditBack(float x, float y);
void endScreen(void);
void pauseScreen(void);
void GameScreenOptions(void);
void GenerateLetters(struct LetterState arr[], int size, float position_x, float position_y, float spacing, float fontSize, int gameScreen);
void ResetWordColor(struct LetterState arr[], int size, int* progress, int color);
void WrongKey(struct LetterState arr[], int size, int* progress, int color);