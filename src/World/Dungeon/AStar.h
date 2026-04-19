#pragma once
#include "DungeonGrid.h"
#include <vector>
#include <glm.hpp>

struct AStarNode {
    int   x, z;
    float gCost = 0.0f; // cost from start
    float hCost = 0.0f; // heuristic cost to end
    float fCost() const { return gCost + hCost; }
    int   parentX = -1;
    int   parentZ = -1;
};

class AStar {
public:
    // returns list of grid coordinates forming the path
    // empty if no path found
    static std::vector<glm::ivec2> FindPath(
        DungeonGrid& grid,
        int startX, int startZ,
        int endX, int endZ);

private:
    static float GetCost(const DungeonGrid& grid, int x, int z);
    static float Heuristic(int x0, int z0, int x1, int z1);
};