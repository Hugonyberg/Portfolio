#pragma once
#include <functional>

struct GridLocation
{
    int x, y;
    bool operator==(const GridLocation& other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const GridLocation& other) const
    {
        return !(*this == other);
    }

    Tga::Vector2f ToPosition(float aTileSize)
    {
        return { x * aTileSize + aTileSize / 2.0f, y * aTileSize + aTileSize / 2.0f };
    }

    static int ManhattanDistance(const GridLocation& a, const GridLocation& b)
    {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }

};

namespace std   
{
    template <>
    struct hash<GridLocation>
    {
        std::size_t operator()(const GridLocation& loc) const
        {
            return std::hash<int>()(loc.x) ^ (std::hash<int>()(loc.y) << 1);
        }
    };
}

