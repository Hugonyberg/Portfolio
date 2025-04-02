#pragma once

#include <unordered_map>
#include <vector>
#include <cmath>
#include "Grid.h"
#include "PriorityQueue.h"
#include "GridLocation.h"

namespace PathFinding
{
    inline double Heuristic(GridLocation a, GridLocation b);

    bool AStarSearch(Grid graph, GridLocation start, GridLocation goal,
        std::unordered_map<GridLocation, GridLocation>& came_from,
        std::unordered_map<GridLocation, double>& cost_so_far);

    std::vector<GridLocation> ReconstructPath(GridLocation start, GridLocation goal,
        std::unordered_map<GridLocation, GridLocation>& came_from);

    std::vector<GridLocation> GetPathFromAStar(GridLocation& aStart, GridLocation& aGoal, Grid& aGrid);
}