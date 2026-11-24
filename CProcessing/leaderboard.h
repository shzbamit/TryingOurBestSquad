#pragma once

#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#define MAX_SCORES 5
#define MAX_NAME_LENGTH 10
#define NAME_BUFFER 12

void UpdateLeaderboard(int currentScore);
void DrawLeaderboard(void);
void NameInput(int *enteredName);

#endif