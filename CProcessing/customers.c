#include <stdlib.h>
#include "customers.h"
#include "ingredients.h"
#include "sandwich.h"

CP_Sound sfxCustomerEnter;

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
		customers[i].animState.targetPos = CP_Vector_Set(CUSTOMER_PLACEMENT_START_X + (i * CUSTOMER_PLACEMENT_INTERVAL), 470.0f);
		customers[i].animState.animationProgress = 0.0f; // Set animation progress at 0%

		customers[i].width = 210.0f;
		customers[i].height = 250.0f;
	}
}

void SpawnCustomer(void)
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
	customers[i].animState.currentPos.y = 470.0f;

	customerCount++;

	CP_Sound_PlayAdvanced(sfxCustomerEnter, 1.0f, 1.0f, FALSE, CP_SOUND_GROUP_SFX);

	UpdateCustomerPositions();
}

#define SPAWN_INTERVAL_MIN		5.0f	// shortest wait before the next customer arrives (seconds)
#define SPAWN_INTERVAL_MAX		7.0f	// longest wait
#define DOUBLE_ARRIVAL_CHANCE	40		// % chance that 2 customers walk in together

static float spawnTimer = 0.0f;			// counts down to the next arrival

// Picks a random wait between MIN and MAX
static float RandomSpawnInterval(void)
{
	return SPAWN_INTERVAL_MIN + ((float)rand() / (float)RAND_MAX) * (SPAWN_INTERVAL_MAX - SPAWN_INTERVAL_MIN);
}

void UpdateCustomerSpawning(void)
{
	// Shop is full, so the timer waits until a slot opens up
	if (customerCount >= CUSTOMERS_SHOWN)
	{
		return;
	}

	spawnTimer -= CP_System_GetDt();

	if (spawnTimer <= 0.0f)
	{
		int arrivals = 1; // default: one customer walks in

		// Only when exactly 1 customer is inside can a pair walk in together
		if (customerCount == 1 && (rand() % 100) < DOUBLE_ARRIVAL_CHANCE)
		{
			arrivals = 2;
		}

		// Spawn them, but never go over the maximum
		for (int n = 0; n < arrivals && customerCount < CUSTOMERS_SHOWN; n++)
		{
			SpawnCustomer();
		}

		spawnTimer = RandomSpawnInterval();
	}
}

void CompleteCustomer(int index)
{
	if (customerCount == 0) return;

	// Copy the front customer into the leaving array
	leavingCustomers[leavingCustomerCount] = customers[index];

	// Set up their exit path
	leavingCustomers[leavingCustomerCount].animState.startPos = customers[index].animState.currentPos;
	leavingCustomers[leavingCustomerCount].animState.targetPos = CUSTOMER_LEAVE_POS; // Move up off-screen
	leavingCustomers[leavingCustomerCount].animState.animationProgress = 0.0f;
	leavingCustomers[leavingCustomerCount].animState.isAnimating = 1;
	leavingCustomerCount++;

	// Shift the main queue immediately so the next customer takes over index 0
	ShiftCustomerArray(customers, &customerCount, index);

	// Tell the remaining active line queue to slide forward step slots
	UpdateCustomerPositions();

	// Tell the remaining active line queue to slide forward step slots 
	UpdateCustomerPositions();

	// Never leave the shop empty: if the last customer left, someone new arrives
	if (customerCount == 0)
	{
		SpawnCustomer();
		spawnTimer = RandomSpawnInterval(); // restart the wait for the next arrival
	}
}

void SpawnInitialCustomers(void)
{
	customerCount = 0;

	SpawnCustomer(); // Only 1 customer at the start of the game
	spawnTimer = RandomSpawnInterval(); 
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
