#pragma once

#include <cmath>
#include <tge\math\vector2.h>

namespace MATH
{

    inline float CalculateDistance(const Tga::Vector2f& a, const Tga::Vector2f& b)
    {
        Tga::Vector2f delta = b - a;
        return std::sqrt(delta.x * delta.x + delta.y * delta.y);
    }
}