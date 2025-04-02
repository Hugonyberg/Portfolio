#include "AStar.h"

double PathFinding::Heuristic(GridLocation a, GridLocation b)
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

bool PathFinding::AStarSearch(Grid graph, GridLocation start, GridLocation goal, std::unordered_map<GridLocation, GridLocation>& came_from, std::unordered_map<GridLocation, double>& cost_so_far)
{
    PriorityQueue<GridLocation, double> frontier;
    frontier.Put(start, 0);

    came_from[start] = start;
    cost_so_far[start] = 0;

    if (!graph.IsTileWalkable(start))
    {
        std::cout << "Start is not Walkable" << std::endl;
    }

    while (!frontier.Empty())
    {
        GridLocation current = frontier.Get();

        if (current == goal)
        {
            break;
        }

        for (GridLocation next : graph.Neighbors(current))
        {
            if (!graph.IsTileWalkable(next)) // Skip Non-walkable
            {
                continue;
            }

            double new_cost = cost_so_far[current] + graph.Cost(current, next);
            if (cost_so_far.find(next) == cost_so_far.end() || new_cost < cost_so_far[next])
            {
                cost_so_far[next] = new_cost;
                double priority = new_cost + Heuristic(next, goal);
                frontier.Put(next, priority);
                came_from[next] = current;
            }
        }
    }

    if (!came_from.count(goal))
    {
        return false;
    }
    return true;
}

std::vector<GridLocation> PathFinding::ReconstructPath(GridLocation start, GridLocation goal, std::unordered_map<GridLocation, GridLocation>& came_from)
{
    std::vector<GridLocation> path;
    GridLocation current = goal;
    while (current != start)
    {
        path.push_back(current);
        current = came_from[current];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<GridLocation> PathFinding::GetPathFromAStar(GridLocation& aStart, GridLocation& aGoal, Grid& aGrid)
{
    std::unordered_map<GridLocation, GridLocation> came_from;
    std::unordered_map<GridLocation, double> cost_so_far;

    std::vector<GridLocation> path;

    if (AStarSearch(aGrid, aStart, aGoal, came_from, cost_so_far)) // If found path
    {
        path = ReconstructPath(aStart, aGoal, came_from);
    }
    return path;
}
