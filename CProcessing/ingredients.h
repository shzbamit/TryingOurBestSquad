#pragma once
#include <cprocessing.h>

struct Ingredient
{
	int itemID;
	const char* name;
	CP_Image asset;
	CP_KEY keybind;
};

// Ingredients
struct Ingredient bread, tomato, lettuce, cheese, ham;

#define INGREDIENT_COUNT	5
