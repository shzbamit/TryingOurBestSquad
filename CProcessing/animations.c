#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "animations.h"

// Lerp for Vectors
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

// Shake customer when patience reaches red level
float ShakeCustomer(float patience, float maxPatience, float* shakeTimer)
{
    if (patience <= maxPatience * 0.25f)
    {
        // Increase animation time
        *shakeTimer += CP_System_GetDt();

        // Calculate side-to-side movement
        return sinf(*shakeTimer * 35.0f) * 8.0f;
    }

    // Stop shaking when patience is above red level
    *shakeTimer = 0.0f;

    return 0.0f;
}

CP_Vector TossImage(CP_Vector start, CP_Vector target, float height, float progress)
{
    CP_Vector result;

    // 1. Linear horizontal travel (steady movement left/right)
    result.x = CP_Math_LerpFloat(start.x, target.x, progress);

    // 2. Linear vertical travel base
    float baselineY = CP_Math_LerpFloat(start.y, target.y, progress);

    // 3. Parabolic arc offset: progress goes 0.0 -> 0.5 -> 1.0
    // (4 * progress * (1 - progress)) creates a perfect curve peaking at 1.0 in the middle
    float arcOffset = 4.0f * progress * (1.0f - progress) * height;

    // Subtract from Y because in screen space, lower numbers/negative numbers are higher up
    result.y = baselineY - arcOffset;

    return result;
}
