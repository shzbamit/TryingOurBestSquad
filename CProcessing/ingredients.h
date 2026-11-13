#pragma once

#ifndef INGREDIENTS_H
#define INGREDIENTS_H

#include "animations.h"

struct Ingredient
{
	int itemID;
	const char* name;
	CP_Image asset;
	CP_KEY keybind;
	struct AnimationState animState;
};

// extern stops multiple copies from existing when multiple scripts reference ingredients.h
// Ingredients
extern struct Ingredient bread, tomato, lettuce, cheese, ham;

#define INGREDIENT_COUNT	5
#define INGREDIENT_SIZE		200.0f

// For target position (where the sandwich will be assembled)
#define INGREDIENT_POSX		(float)CP_System_GetWindowWidth() / 2.0f
#define INGREDIENT_POSY		(float)CP_System_GetWindowHeight() * 0.75f
#define INGREDIENT_Y_OFFSET 20.0f // How far up each ingredient will be offset from the previous

#define INGREDIENT_SPEED	10.0f		// For animations

#define INGREDIENT_TOSS_SPEED 1.0f
#define INGREDIENT_TOSS_SPEED_MAX 3.0f

#define INGREDIENT_TOSS_SPINS 360.0f // How many degrees it spins during flight (e.g., 360 = 1 full spin)
#define INGREDIENT_TOSS_SPINS_MAX 360.0f * 5.0f

#define INGREDIENT_TOSS_HEIGHT 200.0f // Height of the throw arc
#define INGREDIENT_TOSS_HEIGHT_MAX 700.0f 

extern struct Ingredient ingredients[INGREDIENT_COUNT]; // array for storing the ref to actual ingredients at runtime

void InitializeIngredientList(void); //ran in game_init() to initialize ingredient properties

#endif // INGREDIENTS_H