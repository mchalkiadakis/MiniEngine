#include "AStar.h"
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <cmath>

float AStar::GetCost(const DungeonGrid& grid, int x, int z) {
    if (!grid.InBounds(x, z)) return 9999.0f;
    switch (grid.GetCell(x, z).Type) {
    case CellType::Floor:    return 1.0f;
    case CellType::Corridor: return 1.0f;
    case CellType::Wall:     return 100.0f;
    default:                 return 100.0f;
    }
}

float AStar::Heuristic(int x0, int z0, int x1, int z1) {
    // Manhattan distance
    return static_cast<float>(std::abs(x1 - x0) + std::abs(z1 - z0));
}

std::vector<glm::ivec2> AStar::FindPath(
    DungeonGrid& grid,
    int startX, int startZ,
    int endX, int endZ)
{
    // key: z * width + x
    auto Key = [&](int x, int z) { return z * grid.GetWidth() + x; };

    std::unordered_map<int, AStarNode> allNodes;
    std::unordered_map<int, bool>      closedSet;

    // open set — min heap by fCost
    auto Compare = [](const AStarNode& a, const AStarNode& b) {
        return a.fCost() > b.fCost();
        };
    std::priority_queue<AStarNode,
        std::vector<AStarNode>,
        decltype(Compare)> openSet(Compare);

    AStarNode startNode;
    startNode.x = startX;
    startNode.z = startZ;
    startNode.gCost = 0.0f;
    startNode.hCost = Heuristic(startX, startZ, endX, endZ);
    allNodes[Key(startX, startZ)] = startNode;
    openSet.push(startNode);

    // 4-directional movement
    const int dx[] = { 0, 0, 1, -1 };
    const int dz[] = { 1,-1, 0,  0 };

    while (!openSet.empty()) {
        AStarNode current = openSet.top();
        openSet.pop();

        int currentKey = Key(current.x, current.z);

        if (closedSet.count(currentKey)) continue;
        closedSet[currentKey] = true;

        // reached destination
        if (current.x == endX && current.z == endZ) {
            // reconstruct path
            std::vector<glm::ivec2> path;
            int cx = current.x;
            int cz = current.z;

            while (cx != startX || cz != startZ) {
                path.push_back({ cx, cz });
                AStarNode& node = allNodes[Key(cx, cz)];
                int px = node.parentX;
                int pz = node.parentZ;
                cx = px;
                cz = pz;
            }
            path.push_back({ startX, startZ });
            std::reverse(path.begin(), path.end());
            return path;
        }

        // explore neighbors
        for (int d = 0; d < 4; d++) {
            int nx = current.x + dx[d];
            int nz = current.z + dz[d];

            if (!grid.InBounds(nx, nz)) continue;
            if (closedSet.count(Key(nx, nz))) continue;

            float moveCost = GetCost(grid, nx, nz);
            float newG = current.gCost + moveCost;

            int nKey = Key(nx, nz);
            if (!allNodes.count(nKey) || newG < allNodes[nKey].gCost) {
                AStarNode neighbor;
                neighbor.x = nx;
                neighbor.z = nz;
                neighbor.gCost = newG;
                neighbor.hCost = Heuristic(nx, nz, endX, endZ);
                neighbor.parentX = current.x;
                neighbor.parentZ = current.z;
                allNodes[nKey] = neighbor;
                openSet.push(neighbor);
            }
        }
    }

    // no path found
    return {};
}