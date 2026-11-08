#pragma once
#include "ingredients.h"

#define MAX_SANDWICH_SIZE			8
#define MIN_SANDWICH_SIZE			5
extern int sandwichOrder;		// order in which sandwich ingredients will be drawn
extern struct Ingredient sandwich[MAX_SANDWICH_SIZE];

#define MAX_DISCARDED_ITEMS 16
extern struct DiscardedIngredient discardedItems[MAX_DISCARDED_ITEMS]; // Queue to track discarded ingredients mid animation
extern int discardedCount;

struct DiscardedIngredient
{
    struct Ingredient item;  // Packs the original asset and existing animation paths
    float tossSpeed;         // Persistent speed scalar
    float tossHeight;        // Persistent peak arc height
    float rotationSpeed;     // Persistent spin speed and direction
};

void RenderSandwich(void);
void MakeSandwich(int customerCount);
void RemoveIngredient(void);
void UpdateAndRenderDiscards(void);
