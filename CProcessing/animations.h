#pragma once
#include <cprocessing.h>

struct AnimationState
{
	float animationProgress;				// Set to 0.0f under spawn initial customers, used as animation tracker
	CP_Vector startPos;						// Remembers where the customer started moving from
	CP_Vector targetPos;					// Remembers where the customer is moving to
	CP_Vector currentPos;
	int isAnimating;						// 1 if customer is currently exiting. This is for the leaving animation
	float animSpeed;
};

CP_Vector MoveImage(float speed, CP_Vector startPosition, CP_Vector targetPosition, float* moveProgress);
float ShakeCustomer(float patience, float maxPatience, float* shakeTimer);
CP_Vector TossImage(CP_Vector start, CP_Vector target, float height, float progress);
