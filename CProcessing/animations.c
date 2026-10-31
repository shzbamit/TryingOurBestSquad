#include <stdio.h>
#include <cprocessing.h>

#include "animations.h"

// The core Lerp function
static float Lerp(float start, float target, float progress) // Progress represents how complete the animation is
{
    // Clamp t between 0.0 and 1.0 to prevent overshooting
    progress = CP_Math_ClampFloat(progress, 0.0f, 1.0f);

    return start + (target - start) * progress;
}

// Lerp for 2D coordinates
static CP_Vector Vector2Lerp(CP_Vector start, CP_Vector target, float progress)
{
    CP_Vector result;
    result.x = Lerp(start.x, target.x, progress);
    result.y = Lerp(start.y, target.y, progress);
    return result;
}

CP_Vector MoveImage(float speed, CP_Vector startPosition, CP_Vector targetPosition, float* moveProgress)
{
    *moveProgress += speed;

    return Vector2Lerp(startPosition, targetPosition, *moveProgress);
}
