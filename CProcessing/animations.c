#include <stdio.h>
#include <cprocessing.h>

#include "animations.h"

// Lerp for 2D coordinates
static CP_Vector Vector2Lerp(CP_Vector start, CP_Vector target, float progress)
{
    CP_Vector result;
    result.x = CP_Math_LerpFloat(start.x, target.x, progress);
    result.y = CP_Math_LerpFloat(start.y, target.y, progress);
    return result;
}

CP_Vector MoveImage(float speed, CP_Vector startPosition, CP_Vector targetPosition, float* moveProgress)
{
    *moveProgress += speed;

    return Vector2Lerp(startPosition, targetPosition, *moveProgress);
}
