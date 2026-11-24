#pragma once

#ifndef TIMER_H
#define TIMER_H

#include <time.h>

// Universal Sizing Configurations
#define TIMER_TEXT_SIZE 100.0f

// Public global variables exposed via extern
extern int total_seconds;
extern int minutes;
extern int seconds;
extern char timerText[6];
extern const int initialTime;
extern clock_t next_tick;

// Function Prototypes
void InitializeTimer(void);
void UpdateTimer(void);
void ApplyTimeChanges(int addedSeconds);

#endif // TIMER_H