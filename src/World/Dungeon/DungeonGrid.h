#pragma once
#include <vector>
#include <glm.hpp>

enum class CellType {
    Wall,
    Floor,
    Corridor
};

struct GridCell {
    CellType Type = CellType::Wall;
};

class DungeonGrid {
public:
    DungeonGrid(float worldWidth, float worldDepth, float cellSize);

    // world space to grid coordinates
    int WorldToGridX(float worldX) const;
    int WorldToGridZ(float worldZ) const;

    // grid coordinates to world space (cell center)
    float GridToWorldX(int gridX) const;
    float GridToWorldZ(int gridZ) const;

    // cell access
    GridCell& GetCell(int x, int z);
    const GridCell& GetCell(int x, int z) const;

    bool InBounds(int x, int z) const;

    // mark a world-space rectangle as a given type
    void MarkRect(float worldX, float worldZ,
        float worldWidth, float worldDepth,
        CellType type);

    int GetWidth()    const { return m_Width; }
    int GetDepth()    const { return m_Depth; }
    float GetCellSize() const { return m_CellSize; }

private:
    int   m_Width;
    int   m_Depth;
    float m_CellSize;
    float m_WorldWidth;
    float m_WorldDepth;
    std::vector<GridCell> m_Cells;
};