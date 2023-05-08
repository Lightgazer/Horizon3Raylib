#pragma once
#include <cmath>

float MoveTowards(float current, float target, float maxDelta)
{
    if (abs(target - current) <= maxDelta)
    {
        return target;
    }
    float delta = copysign(maxDelta, target - current);
    return current + delta;
}