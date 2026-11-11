#pragma once

#ifndef CUSTOMERS_H
#define CUSTOMERS_H

#include "cprocessing.h"
#include "animations.h"
#include "sandwich.h"

// Customer macros:
// Customer
#define MAX_CUSTOMERS				50
#define CUSTOMERS_SHOWN				3
#define NUM_CHARACTER_ASSETS		9

// customer placement
#define CUSTOMER_PLACEMENT_START_X	700.0f  // must be right of the order columns (~540)
#define CUSTOMER_PLACEMENT_INTERVAL 260.0f

// Animations
#define CUSTOMER_MOVE_SPEED			5.0f

// Patience 
#define MIN_PATIENCE				10.0f
#define MAX_PATIENCE				25.0f
#define PATIENCE_SPEED				0.5f

// Customer struct:
struct Customer
{
	CP_Image asset;
	int order[MAX_SANDWICH_SIZE];			// Generates a sandwich array of maximum size
	int orderSize;							// For storing the actual size variable of order sandwich
	int position;
	//CP_Vector currentPos;
	float width;
	float height;
	int orderCompleted;
	float patience;
	float maxPatience;
	float shakeTimer;
	int shakeSoundPlayed;
	struct AnimationState animState;
};

// Global variables:
extern struct Customer customers[MAX_CUSTOMERS];
extern CP_Image customerAssets[NUM_CHARACTER_ASSETS];
extern int customerCount;

extern struct Customer leavingCustomers[MAX_CUSTOMERS];
extern int leavingCustomerCount;

// Customer functions:
void SpawnInitialCustomers(void);
void CompleteCustomer(int index);
int FindMatchingCustomer(void);

#endif // CUSTOMERS_H