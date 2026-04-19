#include "DungeonGrid.h"
#include <algorithm>
#include <cmath>

DungeonGrid::DungeonGrid(float worldWidth, float worldDepth, float cellSize)
    : m_CellSize(cellSize)
    , m_WorldWidth(worldWidth)
    , m_WorldDepth(worldDepth)
{
    m_Width = static_cast<int>(std::ceil(worldWidth / cellSize));
    m_Depth = static_cast<int>(std::ceil(worldDepth / cellSize));
    m_Cells.resize(m_Width * m_Depth);
}

int DungeonGrid::WorldToGridX(float worldX) const {
    return static_cast<int>(worldX / m_CellSize);
}

int DungeonGrid::WorldToGridZ(float worldZ) const {
    return static_cast<int>(worldZ / m_CellSize);
}

float DungeonGrid::GridToWorldX(int gridX) const {
    return gridX * m_CellSize + m_CellSize * 0.5f;
}

float DungeonGrid::GridToWorldZ(int gridZ) const {
    return gridZ * m_CellSize + m_CellSize * 0.5f;
}

GridCell& DungeonGrid::GetCell(int x, int z) {
    return m_Cells[z * m_Width + x];
}

const GridCell& DungeonGrid::GetCell(int x, int z) const {
    return m_Cells[z * m_Width + x];
}

bool DungeonGrid::InBounds(int x, int z) const {
    return x >= 0 && x < m_Width && z >= 0 && z < m_Depth;
}

void DungeonGrid::MarkRect(float worldX, float worldZ,
    float worldWidth, float worldDepth,
    CellType type)
{
    int x0 = std::max(0, WorldToGridX(worldX));
    int z0 = std::max(0, WorldToGridZ(worldZ));
    int x1 = std::min(m_Width - 1, WorldToGridX(worldX + worldWidth));
    int z1 = std::min(m_Depth - 1, WorldToGridZ(worldZ + worldDepth));

    for (int z = z0; z <= z1; z++)
        for (int x = x0; x <= x1; x++)
            GetCell(x, z).Type = type;
}