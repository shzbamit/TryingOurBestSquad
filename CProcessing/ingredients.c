#include "ingredients.h"

// Allocate the actual variables exactly once here:
struct Ingredient bread, tomato, lettuce, cheese, ham;
struct Ingredient ingredients[INGREDIENT_COUNT];

void InitializeIngredientList(void)
{
	bread.itemID = 0;
	bread.name = "Bread";
	bread.asset = CP_Image_Load("Assets/bread.png");
	bread.keybind = KEY_B;
	bread.animState.animationProgress = 0.0f;
	bread.animState.isAnimating = 0;

	tomato.itemID = 1;
	tomato.name = "Tomato";
	tomato.asset = CP_Image_Load("Assets/tomato.png");
	tomato.keybind = KEY_T;
	tomato.animState.animationProgress = 0.0f;
	tomato.animState.isAnimating = 0;

	lettuce.itemID = 2;
	lettuce.name = "Lettuce";
	lettuce.asset = CP_Image_Load("Assets/lettuce.png");
	lettuce.keybind = KEY_L;
	lettuce.animState.animationProgress = 0.0f;
	lettuce.animState.isAnimating = 0;

	cheese.itemID = 3;
	cheese.name = "Cheese";
	cheese.asset = CP_Image_Load("Assets/cheese.png");
	cheese.keybind = KEY_C;
	cheese.animState.animationProgress = 0.0f;
	cheese.animState.isAnimating = 0;

	ham.itemID = 4;
	ham.name = "Ham";
	ham.asset = CP_Image_Load("Assets/ham.png");
	ham.keybind = KEY_H;
	ham.animState.animationProgress = 0.0f;
	ham.animState.isAnimating = 0;

	ingredients[0] = bread;
	ingredients[1] = tomato;
	ingredients[2] = lettuce;
	ingredients[3] = cheese;
	ingredients[4] = ham;
}
