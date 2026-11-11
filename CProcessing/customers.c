#include <stdlib.h>

#include "customers.h"
#include "ingredients.h"
#include "sandwich.h"

extern CP_Sound sfxCustomerEnter;

// Global variables:
struct Customer customers[MAX_CUSTOMERS];
CP_Image customerAssets[NUM_CHARACTER_ASSETS];
int customerCount = 0;

struct Customer leavingCustomers[MAX_CUSTOMERS];
int leavingCustomerCount = 0;

// Customer functions definitions:

// Array cleaner
void ShiftCustomerArray(struct Customer arr[], int* count, int indexToRemove)
{
	for (int i = indexToRemove; i < (*count) - 1; i++)
	{
		arr[i] = arr[i + 1];
	}
	(*count)--;
}

static void GenerateRandomOrder(int order[], int orderSize)
{
	int min = 1;
	int max = INGREDIENT_COUNT - 1;

	for (int i = 0; i < orderSize; i++)
	{
		if (i == 0 || i == orderSize - 1) {
			order[i] = 0; // Assigns first and last ingredient as bread
		}
		else {
			order[i] = (rand() % (max - min + 1)) + min; // For deciding ingredients
		}
	}
}

static void UpdateCustomerPositions(void)
{
	for (int i = 0; i < customerCount; i++) // i checks all customers (both leaving and waiting state customers)
	{

		customers[i].position = i;

		// Set coordinates for moving animation
		customers[i].animState.startPos = customers[i].animState.currentPos;
		customers[i].animState.targetPos = CP_Vector_Set(CUSTOMER_PLACEMENT_START_X + (i * CUSTOMER_PLACEMENT_INTERVAL), 550.0f);
		customers[i].animState.animationProgress = 0.0f; // Set animation progress at 0%

		customers[i].width = 180.0f;
		customers[i].height = 220.0f;
	}
}

static void SpawnCustomer(void)
{
	if (customerCount >= MAX_CUSTOMERS) return; // guard against overflowing the array

	int i = customerCount;

	customers[i].asset = customerAssets[rand() % NUM_CHARACTER_ASSETS]; // Randomly selects a character asset for the customer

	customers[i].orderSize = (rand() % (MAX_SANDWICH_SIZE - MIN_SANDWICH_SIZE + 1)) + MIN_SANDWICH_SIZE; // Randomly selects a sandwich size for the customer order
	GenerateRandomOrder(customers[i].order, customers[i].orderSize); // Generates a random sandwich order for the customer

	customers[i].orderCompleted = 0; // Initializes the order completion status
	customers[i].shakeSoundPlayed = 0; // Initializes the shake sound played status
	customers[i].shakeTimer = 0.0f; // Initializes the shake timer for the customer

	float randomPatience = MIN_PATIENCE + ((float)rand() / (float)RAND_MAX) * (MAX_PATIENCE - MIN_PATIENCE); // Generates a random patience value for the customer within the defined range
	customers[i].patience = randomPatience;
	customers[i].maxPatience = randomPatience;

	customers[i].animState.animSpeed = CUSTOMER_MOVE_SPEED;
	customers[i].animState.animationProgress = 0.0f;
	customers[i].animState.isAnimating = 0;

	// Spawn at the right side of the screen
	customers[i].animState.currentPos.x = 1900.0f;
	customers[i].animState.currentPos.y = 400.0f;

	customerCount++;

	CP_Sound_PlayAdvanced(sfxCustomerEnter, 1.0f, 1.0f, FALSE, CP_SOUND_GROUP_SFX);

	UpdateCustomerPositions();
}

void CompleteCustomer(int index)
{
	if (customerCount == 0) return;

	// Copy the front customer into the leaving array
	leavingCustomers[leavingCustomerCount] = customers[index];

	// Set up their exit path
	leavingCustomers[leavingCustomerCount].animState.startPos = customers[index].animState.currentPos;
	leavingCustomers[leavingCustomerCount].animState.targetPos = CP_Vector_Set(customers[index].animState.currentPos.x, -100.0f); // Move up off-screen
	leavingCustomers[leavingCustomerCount].animState.animationProgress = 0.0f;
	leavingCustomers[leavingCustomerCount].animState.isAnimating = 1;
	leavingCustomerCount++;

	// Shift the main queue immediately so the next customer takes over index 0
	ShiftCustomerArray(customers, &customerCount, index);

	// Tell the remaining active line queue to slide forward step slots
	UpdateCustomerPositions();

	// Spawn a new customer if the queue is below the maximum shown
	if (customerCount < CUSTOMERS_SHOWN)
	{
		SpawnCustomer();
	}
}

void SpawnInitialCustomers(void)
{
	customerCount = 0;

	for (int i = 0; i < CUSTOMERS_SHOWN; i++) // Spawn the initial customers to fill the queue
	{
		SpawnCustomer();
	}
}

int FindMatchingCustomer(void)
{
	// Check further only if there is a sandwich
	if (sandwichOrder == 0)
	{
		return -1;
	}

	int selectedCustomer = -1;

	for (int i = 0; i < customerCount; i++)
	{
		// Size of sandwich must first be identical
		if (sandwichOrder != customers[i].orderSize)
		{
			continue; // If sizes don't match, skip to the next customer
		}

		int match = 1;

		// Compare each ingredient between customer[i].order and sandwich[j]
		for (int j = 0; j < customers[i].orderSize; j++)
		{
			if (sandwich[j].itemID != customers[i].order[j])
			{
				match = 0; // Found mismatched ingredient
				break;
			}
		}

		// If a match is found, track the customer with lowest patience
		// This is to enforce a tie-breaking rule where lowest patience customer gets their order first
		if (match)
		{
			if (selectedCustomer == -1 || customers[i].patience < customers[selectedCustomer].patience)
			{
				selectedCustomer = i;
			}
		}
	}

	return selectedCustomer;
}
