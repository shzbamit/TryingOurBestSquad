#pragma once
#include <cprocessing.h>
struct letterState
{
	int r;
	int g;
	int b;
	CP_KEY keybind;
};
extern struct letterState start[5];
extern struct letterState quit[4];
//struct letterState start_S,start_T,start_A,start_R,start_T2,quit_Q,quit_U,quit_I,quit_T;
void initializeLetters(void);
void resetWords(struct letterState arr[], int size, int* progress);
void wrongKey(struct letterState arr[], int size, int* progress);