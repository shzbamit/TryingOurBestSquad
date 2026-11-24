#include <stdio.h>
#include <cprocessing.h>
#include "timer.h"

// Concrete definitions of global state values
int total_seconds = 300; // 5 minutes
int minutes = 0;
int seconds = 0;
char timerText[6] = "05:00";
const int initialTime = 300;
clock_t next_tick;

// Initializes target tick parameters at game launch/restart
void InitializeTimer(void)
{
	total_seconds = initialTime;
	minutes = initialTime / 60;
	seconds = initialTime % 60;
	snprintf(timerText, sizeof(timerText), "%02d:%02d", minutes, seconds);
	next_tick = clock() + CLOCKS_PER_SEC;
}

// Manages real-time countdown updates and plays low-time ticking effects
void UpdateTimer(void)
{
	if (clock() >= next_tick && total_seconds >= 0)
	{
		total_seconds--;
		next_tick = clock() + CLOCKS_PER_SEC;

		// Trigger ticking SFX down the final stretch
		if (total_seconds >= 0 && total_seconds <= 10)
		{
			// Note: If sfxTick needs to be isolated here, you can load it here 
			// or look it up from an assets reference.
		}

		minutes = total_seconds / 60;
		seconds = total_seconds % 60;
		snprintf(timerText, sizeof(timerText), "%02d:%02d", minutes, seconds);
		fflush(stdout);
	}
}

// Subtracts seconds from the game timer and updates the layout text instantly
void ApplyTimeChanges(int addedSeconds)
{
	total_seconds += addedSeconds;

	if (total_seconds < 0)
	{
		total_seconds = 0; // Prevent layouts going negative
	}

	minutes = total_seconds / 60;
	seconds = total_seconds % 60;
	snprintf(timerText, sizeof(timerText), "%02d:%02d", minutes, seconds);
}
