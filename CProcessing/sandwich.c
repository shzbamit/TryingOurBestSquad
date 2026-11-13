#include <stdlib.h>
#include "sandwich.h"

int sandwichOrder = 0; // Used to track the current sandwich index and sandwich size
struct Ingredient sandwich[MAX_SANDWICH_SIZE]; // Contains the sandwich being made currently

struct DiscardedIngredient discardedItems[MAX_DISCARDED_ITEMS];
int discardedCount = 0;

/* !
@brief  Updates currenPos of each sandwich ingredient in sandwichOrder 
		based on animationProgress.
		CP_Image_Draw() then places the ingredient based on currentPos.
@param  none
@return none
*//*______________________________________________________________*/
void RenderSandwich(void)
{
	for (int i = 0; i < sandwichOrder; i++)
	{
		// Update the animation properties first
		sandwich[i].animState.currentPos = MoveImage(
			INGREDIENT_SPEED * CP_System_GetDt(),
			sandwich[i].animState.startPos,
			sandwich[i].animState.targetPos,
			&sandwich[i].animState.animationProgress
		);

		// Draw based on currentPos
		CP_Image_Draw(
			sandwich[i].asset,
			sandwich[i].animState.currentPos.x,
			sandwich[i].animState.currentPos.y,
			INGREDIENT_SIZE,
			INGREDIENT_SIZE,
			255
		);
	}
}

/* !
@brief  Assigns selectedIngredient according to CP_Input_Key.
		
		If any key is pressed, keyPressed is set to true,
		and set the current sandwichOrder index of sandwich[] to the selectedIngredient,
		then set the positions of the animationState of the corresponding sandwich[sandwichOrder] struct.

		sandwichOrder is then incremented before next ingredient.
@param  int
@return none
*//*______________________________________________________________*/
void MakeSandwich(int customerCount)
{
	// Only allow adding ingredients if there are customers and sandwichOrder isn't already at max size
	if (customerCount > 0 && sandwichOrder < MAX_SANDWICH_SIZE)
	{
		struct Ingredient selectedIngredient;
		int keyPressed = 0;

		if (CP_Input_KeyTriggered(bread.keybind)) { selectedIngredient = bread; keyPressed = 1; }
		else if (CP_Input_KeyTriggered(tomato.keybind)) { selectedIngredient = tomato; keyPressed = 1; }
		else if (CP_Input_KeyTriggered(lettuce.keybind)) { selectedIngredient = lettuce; keyPressed = 1; }
		else if (CP_Input_KeyTriggered(cheese.keybind)) { selectedIngredient = cheese; keyPressed = 1; }
		else if (CP_Input_KeyTriggered(ham.keybind)) { selectedIngredient = ham; keyPressed = 1; }

		if (keyPressed)
		{
			// Copy the entire baseline struct into this slot
			sandwich[sandwichOrder] = selectedIngredient;

			// Set up animation state positions
			sandwich[sandwichOrder].animState.animationProgress = 0.0f;
			sandwich[sandwichOrder].animState.startPos = CP_Vector_Set(INGREDIENT_POSX, -100.0f); // Top of screen
			// Place this ingredient higher based on the current sandwichOrder
			sandwich[sandwichOrder].animState.targetPos = CP_Vector_Set(INGREDIENT_POSX, INGREDIENT_POSY - (INGREDIENT_Y_OFFSET * sandwichOrder)); 
			sandwich[sandwichOrder].animState.currentPos = sandwich[sandwichOrder].animState.startPos;

			sandwichOrder++;
		}
	}
}

/* !
@brief  Updates the animation progress of discardedItems[] and then calls Draw().

		Checks through each active member of discardedItems[] and updates their animationProgress
		before drawing. This function also discards any members who have completed their animation.
@param  none
@return none
*//*______________________________________________________________*/
void UpdateAndRenderDiscards(void)
{
	for (int i = 0; i < discardedCount; i++) // Repeat for each item in discardedItems[]
	{
		// anim points to discardedItem's animState for quick reference
		struct AnimationState* anim = &discardedItems[i].item.animState;

		// Progress the animationProgress using the tossSpeed
		anim->animationProgress += discardedItems[i].tossSpeed * CP_System_GetDt();

		if (anim->animationProgress >= 1.0f) // If animation is done
		{
			// Remove the current slot and shift the array up
			for (int j = i; j < discardedCount - 1; j++)
			{
				discardedItems[j] = discardedItems[j + 1]; // Shift all items forward in the queue (removed item gets placed at the end)
			}
			discardedCount--; // Array size gets 1 size smaller
			i--; // force for() to look at current i element again (as a new item has been placed in [i])
			continue;
		}

		// Update currentPos using newly updated animationProgress
		anim->currentPos = TossImage(
			anim->startPos,
			anim->targetPos,
			discardedItems[i].tossHeight,
			anim->animationProgress
		);

		// currentRotation = progress * total rotations
		float currentRotation = anim->animationProgress * discardedItems[i].rotationSpeed;

		CP_Settings_ImageMode(CP_POSITION_CENTER); // Important for allowing the image to spin from center origin
		CP_Image_DrawAdvanced(
			discardedItems[i].item.asset, // Fetch the texture out of the inner item struct
			anim->currentPos.x,
			anim->currentPos.y,
			INGREDIENT_SIZE,
			INGREDIENT_SIZE,
			255,
			currentRotation
		);
	}
}

/* !
@brief  Adds discarded ingredients to discardedItems[] and sets animation properties.

		Sets the starting position to current position and then
		generate a set of randomized ratios for speed, height and spin amount for the tossing animation.

@param  none
@return none
*//*______________________________________________________________*/
void RemoveIngredient(void)
{
	// Get the index of the ingredient we are tossing away
	int removeIndex = sandwichOrder - 1; // -1 because sandwichOrder is an int that counts from 1 while the sandwich[] array counts from 0

	if (discardedCount < MAX_DISCARDED_ITEMS) // only allow up to MAX_DISCARDED_ITEMS in the queue
	{
		// Copy removed ingredient to struct discardedItems[]
		discardedItems[discardedCount].item = sandwich[removeIndex];

		// Set pointer shortcut for discardedItems[] properties
		struct AnimationState* anim = &discardedItems[discardedCount].item.animState;

		anim->animationProgress = 0.0f;
		anim->startPos = sandwich[removeIndex].animState.currentPos;

		// Set window boundaries
		float windowWidth = (float)CP_System_GetWindowWidth();
		float windowHeight = (float)CP_System_GetWindowHeight();

		int randDir = rand() % 2; // Returns exactly 1 or 0

		// Set target x coordinate to either the right side or the left side of x-axis (200.0f off the screen)
		float targetX = (randDir == 1) ? (windowWidth + 200.0f) : -200.0f;
		anim->targetPos = CP_Vector_Set(targetX, windowHeight + 200.0f);

		// Generate and use unique randoms for toss speed, height and rotation
		// animation speed = min_speed + (ratio * (max_speed - min_speed))
		float ratioSpeed = (float)rand() / (float)RAND_MAX;
		discardedItems[discardedCount].tossSpeed = INGREDIENT_TOSS_SPEED + ratioSpeed * (INGREDIENT_TOSS_SPEED_MAX - INGREDIENT_TOSS_SPEED);

		float ratioHeight = (float)rand() / (float)RAND_MAX;
		discardedItems[discardedCount].tossHeight = INGREDIENT_TOSS_HEIGHT + ratioHeight * (INGREDIENT_TOSS_HEIGHT_MAX - INGREDIENT_TOSS_HEIGHT);

		float ratioSpins = (float)rand() / (float)RAND_MAX;
		float spinAmount = INGREDIENT_TOSS_SPINS + ratioSpins * (INGREDIENT_TOSS_SPINS_MAX - INGREDIENT_TOSS_SPINS);
		discardedItems[discardedCount].rotationSpeed = (rand() % 2 == 0) ? spinAmount : -spinAmount;

		discardedCount++; // Incremented to prepare for next discard
	}
}
