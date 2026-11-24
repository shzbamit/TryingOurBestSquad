#include <cprocessing.h>
#include <string.h>

#include "debug.h"

#include "customers.h"
#include "timer.h"
#include "score.h"

// Panel Layout Configurations
#define PANEL_W 500.0f
#define PANEL_X 20.0f
#define PANEL_Y 20.0f

// Colors
#define PANEL_COLOUR CP_Color_Create(0, 0, 0, 200)       // Semi-transparent Black
#define BUTTON_COLOUR CP_Color_Create(255, 255, 255, 255) // Solid White
#define TEXT_COLOUR CP_Color_Create(0, 0, 0, 255)         // Solid Black
#define BUTTON_HOVER_COLOUR CP_Color_Create(144, 238, 144, 255) // Light Green highlight

// Button & Typography Metrics
#define STARTING_UI_Y 40.0f
#define TEXT_SIZE 30.0f
#define BUTTON_PADDING_H 20.0f  // Added to TEXT_SIZE to calculate total button height
#define BUTTON_MARGIN_X 20.0f   // Horizontal padding from the edge of the panel
#define BUTTON_SPACING_Y 15.0f  // Vertical gap between adjacent buttons
#define PANEL_PADDING_BOTTOM 40.0f // Padding at the bottom of the menu panel

int debugActive = 0; // Tracks whether debug menu is open or not

const char* debugContent[] = {
	"Debug Menu",
	"SpawnCustomer()",
	"RemoveCustomer()",
	"Timer +60s",
	"Timer -60s",
	"Score +100",
	"Score +1000",
	"Score -100",
	"Score -1000"
};
int debugLines = sizeof(debugContent) / sizeof(debugContent[0]);

static void RenderPanel(void)
{
	// Calculate element heights
	float btnH = TEXT_SIZE + BUTTON_PADDING_H;

	// Calculate button heights and gaps between each button
	float totalButtonsHeight = (float)debugLines * btnH;
	float totalGapsHeight = (float)(debugLines - 1) * BUTTON_SPACING_Y;
	if (totalGapsHeight < 0.0f) totalGapsHeight = 0.0f;

	// Calculate panel height according to number of elements
	float dynamicPanelH = STARTING_UI_Y + totalButtonsHeight + totalGapsHeight + PANEL_PADDING_BOTTOM;

	// Render Background Panel
	CP_Settings_Fill(PANEL_COLOUR);
	CP_Graphics_DrawRect(PANEL_X, PANEL_Y, PANEL_W, dynamicPanelH);

	// Get current frame mouse coordinates once before running the loop
	float mouseX = CP_Input_GetMouseX();
	float mouseY = CP_Input_GetMouseY();

	for (int i = 0; i < debugLines; i++)
	{
		// Calculate static layout bounds relative to the panel configuration macros
		float btnW = PANEL_W - (BUTTON_MARGIN_X * 2.0f);
		float btnX = PANEL_X + BUTTON_MARGIN_X;
		float btnY = STARTING_UI_Y + (i * (btnH + BUTTON_SPACING_Y));

		// Determine if the cursor is hovering inside this specific button container
		int isHovering = (mouseX >= btnX && mouseX <= (btnX + btnW) &&
			mouseY >= btnY && mouseY <= (btnY + btnH));

		if (i != 0)
		{
			// Render button background and highlight if hovered
			if (isHovering)
			{
				CP_Settings_Fill(BUTTON_HOVER_COLOUR); // Use hover highlight color
			}
			else
			{
				CP_Settings_Fill(BUTTON_COLOUR);       // Fall back to default white color
			}
			CP_Graphics_DrawRect(btnX, btnY, btnW, btnH);

			// Render Button Text
			CP_Settings_Fill(TEXT_COLOUR);
			CP_Settings_TextSize(TEXT_SIZE);
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

			// Calculate midpoint coordinates and render label text
			float textX = btnX + (btnW / 2.0f);
			float textY = btnY + (btnH / 2.0f);
			CP_Font_DrawText(debugContent[i], textX, textY);

			// Handle clicks using the exact same bounding box variables
			if (CP_Input_MouseClicked())
			{
				if (isHovering)
				{
					switch (i) // Cases start from 1 because 0 is debug menu text
					{
					case 1:
						SpawnCustomer();
						break;
					case 2:
						CompleteCustomer(0);
						break;
					case 3:
						ApplyTimeChanges(60);
						break;
					case 4:
						ApplyTimeChanges(-60);
						break;
					case 5:
						score += 100;
						break;
					case 6:
						score += 1000;
						break;
					case 7:
						score -= 100;
						break;
					case 8:
						score -= 1000;
						break;
					default:
						break;
					}
				}
			}
		}
		else
		{
			// Render "Debug Menu" Text
			CP_Settings_Fill(BUTTON_COLOUR); // Use white
			CP_Settings_TextSize(TEXT_SIZE);
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
			CP_Font_DrawText(debugContent[i], PANEL_X + BUTTON_MARGIN_X, STARTING_UI_Y + (i * (btnH + BUTTON_SPACING_Y)));
		}
	}
}

int AllowDebug(void)
{
	if (CP_Input_KeyTriggered(KEY_GRAVE_ACCENT)) // ` key for debugging menu
	{
		debugActive = debugActive == 0 ? 1 : 0;
	}
	if (debugActive)
	{
		RenderPanel();
		return 1;
	}
	return 0;
}
