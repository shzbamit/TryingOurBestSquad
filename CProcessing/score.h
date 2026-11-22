#pragma once

// Score
extern int score;							// Tracks actual score
extern char scoreText[12];
#define SCORE_TEXT_SIZE				100.0f
#define SCORE_TEXT_COLOR			CP_Color_Create(128, 128, 128, 255) 

#define SCORE_ORDER					100		// For completing orders
#define SCORE_SPEED_BONUS_MAX		100		// points gained when order is completed faster
#define SCORE_REMOVE_INGREDIENT		-10		// For discarding ingredients
#define SCORE_CUSTOMER_LEFT			-50		// points lost when a customer leaves 

typedef struct {
    char text[12];
    CP_Color color;
    float timeAlive;  // Tracks how long (in seconds) this specific line has been visible
    float alpha;      // Handles the smooth visibility scale (1.0f fully visible to 0.0f hidden)
} ScoreLogEntry;

void PushScoreList(int amount);
void RenderScoreList(void);
void UpdateScoreCounter(void);
void RenderScore(void);
