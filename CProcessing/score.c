#include <cprocessing.h>
#include "score.h"

int score = 0;								// Tracks actual score
int displayScore = 0;						// For setting the display score
int lastScore = -1;
char scoreText[12];

#define MAX_SCORE_POP_INDICATORS	5		// Floating score effect system
#define SCORE_POP_LIFETIME          5.0f    // How long floating score effects stay in seconds

#define SCORE_TEXT_POS_X            1050.0f
#define SCORE_TEXT_POS_Y            100.0f
#define SCORE_POP_TEXT_POS_X        1175.0f
#define SCORE_POP_TEXT_POS_Y        10.0f

#define STREAK_BONUS_PER_LEVEL   10     // extra points per streak level
#define STREAK_BONUS_MAX_LEVEL   10     // cap so the bonus stops growing
#define STREAK_TEXT_POS_X        600.0f
#define STREAK_TEXT_POS_Y        100.0f

int streak = 0;
int bestStreak = 0;

int targetScore = 0;
ScoreLogEntry scoreList[MAX_SCORE_POP_INDICATORS];
int scoreListCount = 0;

void OnCustomerServed(int basePoints)
{
    streak++;
    if (streak > bestStreak)
    {
        bestStreak = streak;
    }

    // Streak of 1 gives no bonus, 2 gives +10, 3 gives +20, ...
    int level = streak - 1;
    if (level > STREAK_BONUS_MAX_LEVEL)
    {
        level = STREAK_BONUS_MAX_LEVEL;
    }

    int bonus = level * STREAK_BONUS_PER_LEVEL;

    score += basePoints;   // UpdateScoreCounter will pop up the total
    targetScore += basePoints;
    PushScoreList(basePoints);      // shows first

    if (bonus > 0)
    {
        score += bonus;
        targetScore += bonus;
        PushScoreList(bonus);       // shows second

        scoreList[scoreListCount - 1].color = CP_Color_Create(255, 180, 0, 255);   // orange
    }
}

void OnCustomerLeft(void)
{
    streak = 0;
    score += SCORE_CUSTOMER_LEFT;
}

void OnIngredientDiscarded(void)
{
    streak = 0;
    score += SCORE_REMOVE_INGREDIENT;
}

int GetStreak(void) { return streak; }
void ResetStreak(void) { streak = 0; }

/* !
@brief  Pushes a new score adjustment entry with initialized visibility states.
@param  int - The value of the change (e.g. 100, -10)
*//*______________________________________________________________*/
void PushScoreList(int amount)
{
    if (scoreListCount >= MAX_SCORE_POP_INDICATORS)
    {
        for (int i = 0; i < MAX_SCORE_POP_INDICATORS - 1; i++)
        {
            scoreList[i] = scoreList[i + 1];
        }
        scoreListCount = MAX_SCORE_POP_INDICATORS - 1;
    }

    int targetIndex = scoreListCount;

    if (amount > 0) // For positive gain
    {
        snprintf(scoreList[targetIndex].text, sizeof(scoreList[targetIndex].text), "+%d", amount);
        scoreList[targetIndex].color = CP_Color_Create(50, 200, 50, 255);
    }
    else // Negative gain
    {
        snprintf(scoreList[targetIndex].text, sizeof(scoreList[targetIndex].text), "%d", amount);
        scoreList[targetIndex].color = CP_Color_Create(220, 50, 50, 255);
    }

    // Initialize the lifetime attributes
    scoreList[targetIndex].timeAlive = 0.0f; // Starts at 0 seconds old
    scoreList[targetIndex].alpha = 1.0f;     // Starts completely opaque
    scoreListCount++;
}

/* !
@brief  Updates lifetime trackers, modifies alpha transparency, and renders the list.
*//*______________________________________________________________*/
void RenderScoreList(void)
{
    float dt = CP_System_GetDt();

    for (int i = 0; i < scoreListCount; i++)
    {
        // Advance aging clocks independently for each visible item frame
        scoreList[i].timeAlive += dt;

        // If the entry has been on screen longer than pop_lifetime, begin smooth alpha decay
        if (scoreList[i].timeAlive >= SCORE_POP_LIFETIME)
        {
            scoreList[i].alpha -= 2.0f * dt; // Adjust '2.0f' multiplier to make the fade faster/slower
            if (scoreList[i].alpha < 0.0f)
            {
                scoreList[i].alpha = 0.0f;
            }
        }
    }

    // Clean up completely transparent elements from index 0
    while (scoreListCount > 0 && scoreList[0].alpha <= 0.0f)
    {
        for (int j = 0; j < scoreListCount - 1; j++)
        {
            scoreList[j] = scoreList[j + 1];
        }
        scoreListCount--;
    }

    // Set text properties
    CP_Settings_TextSize(SCORE_TEXT_SIZE / 2);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_RIGHT, CP_TEXT_ALIGN_V_TOP);

    float startY = SCORE_POP_TEXT_POS_Y;
    float lineSpacing = 35.0f;

    for (int i = 0; i < scoreListCount; i++)
    {
        // Extract original structural tone colors and tint them via the changing alpha matrix
        CP_Color dynamicTint = scoreList[i].color;
        dynamicTint.a = (int)(scoreList[i].alpha * 255.0f);

        CP_Settings_Fill(dynamicTint);

        float currentY = startY + (i * lineSpacing);
        CP_Font_DrawText(scoreList[i].text, SCORE_POP_TEXT_POS_X, currentY);
    }
}


/* !
@brief  Ticks the display score closer to the actual score over time
        and updates the string asset buffer only when changes happen.
@param  none
@return none
*//*______________________________________________________________*/
void UpdateScoreCounter(void)
{
    // Detects the exact moment a point event happens
    if (score != targetScore)
    {
        int delta = score - targetScore;

        PushScoreList(delta);

        targetScore = score;
    }

    // If the visual score doesn't match the actual backend score
    if (displayScore != score)
    {
        // Calculate the difference
        int diff = score - displayScore;

        // Ticks faster if the difference is more than 19, 
        // but moves by at least 1 or 2 points so small additions feel snappy.
        int step = diff / 10;
        if (step == 0) // Accounts for score diff of 19 or lesser or negative
        {
            step = (diff > 0) ? 1 : -1;
        }

        // Ticks closer
        displayScore += step;
    }

    // Re-generate string text matching the moving display score
    if (displayScore != lastScore)
    {
        snprintf(scoreText, sizeof(scoreText), "%d", displayScore);
        lastScore = displayScore;
    }
}

void RenderScore(void)
{
    // Score text
    CP_Settings_Fill(SCORE_TEXT_COLOR);
    CP_Settings_TextSize(SCORE_TEXT_SIZE);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_RIGHT, CP_TEXT_ALIGN_V_BASELINE);

    CP_Font_DrawText(scoreText, SCORE_TEXT_POS_X, SCORE_TEXT_POS_Y);

    // Streak text
    if (streak >= 2)
    {
        char streakText[24];
        snprintf(streakText, sizeof(streakText), "Streak x%d", streak);

        CP_Settings_Fill(CP_Color_Create(255, 180, 0, 255));
        CP_Settings_TextSize(SCORE_TEXT_SIZE / 2);
        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_BASELINE);
        CP_Font_DrawText(streakText, STREAK_TEXT_POS_X, STREAK_TEXT_POS_Y);
    }

    // Renders the history list array from top to bottom
    RenderScoreList();
}
