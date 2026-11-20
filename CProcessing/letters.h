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
extern struct letterState start[5];
extern struct letterState quit[4];
extern struct letterState restart[7];
struct letterState resume[6];

void gameStructInitialization(void);
void initializeLetters(void);
void endScreen(void);
void pauseScreen(void);
void GenerateLetters(struct letterState arr[], int size, float position_x, float position_y);
void resetWordsBlack(struct letterState arr[], int size, int* progress);
void resetWordsWhite(struct letterState arr[], int size, int* progress);
void wrongKey(struct letterState arr[], int size, int* progress, int gameState);