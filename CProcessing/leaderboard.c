#include <stdio.h>
#include "leaderboard.h"
#include "cprocessing.h"


static int player_position = -1;
static int leaderboard[MAX_SCORES];
static int total_score_count = 0;


void UpdateLeaderboard(int currentScore)
{
    player_position = -1;
    FILE* leaderboardTXT = NULL;
    fopen_s(&leaderboardTXT, "leaderboard.txt", "r");

    total_score_count = 0;

	// read leaderboard from file
    if (leaderboardTXT != NULL)
    {
        int saved_count = 0;

        if (fscanf_s(leaderboardTXT, "%d", &saved_count) == 1)
        {
            // Prevent reading more than MAX SCORES
            if (saved_count > MAX_SCORES) {
                saved_count = MAX_SCORES;
            }

            for (int i = 0; i < saved_count; ++i)
            {
                if (fscanf_s(leaderboardTXT, "%d", &leaderboard[i]) == 1){
                    total_score_count++;
                }
                else {
                    break;
                }
            }
        }

        fclose(leaderboardTXT);
    }

	// find the position to insert the new score
    int position = total_score_count;

    for (int i = 0; i < total_score_count; ++i)
    {
        if (currentScore > leaderboard[i]){
            position = i;
            break;
        }
    }

	// check if the current score is greater than 0 and if it can be added to the leaderboard
    if (currentScore > 0 && (total_score_count < MAX_SCORES || position < MAX_SCORES)){

        // Shift existing scores to the right
        int new_count = total_score_count;

        if (new_count < MAX_SCORES) {
            new_count++;
        }

        for (int i = new_count - 1; i > position; --i){
            leaderboard[i] = leaderboard[i - 1];
        }

        // insert current score
        leaderboard[position] = currentScore;
        total_score_count = new_count;

        player_position = position;
    }

	// store the updated leaderboard back to the file
    leaderboardTXT = NULL;
    fopen_s(&leaderboardTXT, "leaderboard.txt", "w");

    if (leaderboardTXT != NULL)
    {
        fprintf(leaderboardTXT, "%d\n", total_score_count);

        for (int i = 0; i < total_score_count; ++i)
        {
            fprintf(leaderboardTXT, "%d\n", leaderboard[i]);
        }

        fclose(leaderboardTXT);
    }
}


void DrawLeaderboard(void)
{
    CP_Settings_TextSize(100.0f);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Font_DrawText("LEADERBOARD", 400.0f, 350.0f);

    CP_Settings_TextSize(70.0f);

    if (total_score_count == 0){
        CP_Font_DrawText("No leaderboard yet!", 400.0f, 450.0f);
    }
    else {
        char scoreText[50];

        for (int i = 0; i < total_score_count; ++i)
        {
            if (i == player_position){
                sprintf_s(scoreText, sizeof(scoreText), "%d. %d (You!)", i + 1, leaderboard[i]);
            }
            else {
                sprintf_s(scoreText, sizeof(scoreText), "%d. %d", i + 1, leaderboard[i]);
            }

            CP_Font_DrawText(scoreText, 400.0f, 450.0f + i * 50.0f);
        }
    }
}

