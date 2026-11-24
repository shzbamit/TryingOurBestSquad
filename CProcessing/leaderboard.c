#include <stdio.h>
#include <string.h>
#include "leaderboard.h"
#include "cprocessing.h"


static int player_position = -1;
static int leaderboard[MAX_SCORES];
static char leaderboardNames[MAX_SCORES][NAME_BUFFER]; // Array to store player names
static int total_score_count = 0;
static char playerName[NAME_BUFFER] = "";
static int nameLength = 0; // Tracks the length of the player's name input


void UpdateLeaderboard(int currentScore)
{
    player_position = -1;
    FILE* leaderboardTXT = NULL;
    fopen_s(&leaderboardTXT, "leaderboardName.txt", "r");

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
                if (fscanf_s(leaderboardTXT, "%10s %d", leaderboardNames[i], (unsigned)NAME_BUFFER, &leaderboard[i]) == 2){
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

            strcpy_s(leaderboardNames[i], NAME_BUFFER, leaderboardNames[i - 1]);
        }

        // insert current score
        leaderboard[position] = currentScore;
        strcpy_s(leaderboardNames[position], NAME_BUFFER, playerName);

        total_score_count = new_count;
        player_position = position;
    }

	// store the updated leaderboard back to the file
    leaderboardTXT = NULL;
    fopen_s(&leaderboardTXT, "leaderboardName.txt", "w");

    if (leaderboardTXT != NULL)
    {
        fprintf(leaderboardTXT, "%d\n", total_score_count);

        for (int i = 0; i < total_score_count; ++i)
        {
            fprintf(leaderboardTXT, "%s %d\n", leaderboardNames[i], leaderboard[i]);
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
                sprintf_s(scoreText, sizeof(scoreText), "%d: %s %d (You!)", i + 1, leaderboardNames[i], leaderboard[i]);
            }
            else {
                sprintf_s(scoreText, sizeof(scoreText), "%d: %s %d", i + 1, leaderboardNames[i], leaderboard[i]);
            }

            CP_Font_DrawText(scoreText, 400.0f, 450.0f + i * 50.0f);
        }
    }
}

void NameInput(int *enteredName) 
{
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
    CP_Settings_TextSize(75.0f);
    CP_Font_DrawText(playerName, 800, 485);

    for (int key = 'A'; key <= 'Z'; key++)
    {
        if (CP_Input_KeyTriggered(key))
        {
            if (nameLength < MAX_NAME_LENGTH)
            {
                playerName[nameLength] = (char)key;
                nameLength++;

                playerName[nameLength] = '\0';
            }
        }
    }

    // Check backspace
    if (CP_Input_KeyTriggered(KEY_BACKSPACE))
    {
        if (nameLength > 0)
        {
            nameLength--;
            playerName[nameLength] = '\0';
        }
    }

    // Check Enter
    if (CP_Input_KeyTriggered(KEY_ENTER) && nameLength > 0)
    {
        *enteredName = 1;
    }
}
