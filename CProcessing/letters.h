#pragma once
#include <cprocessing.h>
struct letterState
{
	char letter;
	int r;
	int g;
	int b;
	CP_KEY keybind;
};

enum { START = 5, QUIT = 4, RESTART = 7, RESUME = 6, CREDITS = 7, BACK = 4 };

extern struct letterState start[START];
extern struct letterState quit[QUIT];
extern struct letterState restart[RESTART];
struct letterState resume[RESUME];
extern struct letterState credits[CREDITS];
extern struct letterState back[BACK];

void gameStructInitialization(void);
void initializeLetters(void);
void CreditBack(float x, float y);
void endScreen(void);
void pauseScreen(void);
void GenerateLetters(struct letterState arr[], int size, float position_x, float position_y);
void resetWordsBlack(struct letterState arr[], int size, int* progress);
void resetWordsWhite(struct letterState arr[], int size, int* progress);
void wrongKey(struct letterState arr[], int size, int* progress, int gameState);