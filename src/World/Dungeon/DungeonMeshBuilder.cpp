#include "DungeonMeshBuilder.h"
#include "Rendering/Mesh.h"
#include <algorithm>
#include <iostream>

void DungeonMeshBuilder::AddQuad(std::vector<Vertex>& vertices,
    std::vector<unsigned int>& indices,
    const glm::vec3& p0, const glm::vec3& p1,
    const glm::vec3& p2, const glm::vec3& p3,
    const glm::vec3& normal,
    float uScale, float vScale)
{
    unsigned int base = static_cast<unsigned int>(vertices.size());

    vertices.push_back({ p0, normal, { 0.0f,   0.0f   } });
    vertices.push_back({ p1, normal, { uScale, 0.0f   } });
    vertices.push_back({ p2, normal, { uScale, vScale } });
    vertices.push_back({ p3, normal, { 0.0f,   vScale } });

    indices.push_back(base + 0);
    indices.push_back(base + 1);
    indices.push_back(base + 2);
    indices.push_back(base + 2);
    indices.push_back(base + 3);
    indices.push_back(base + 0);
}

// builds a wall along one side of a room, cutting gaps where corridor cells are adjacent
void DungeonMeshBuilder::AddWallWithGridDoors(
    std::vector<Vertex>& vertices,
    std::vector<unsigned int>& indices,
    const RoomData& room,
    WallSide side,
    float doorHeight,
    const DungeonGrid& grid)
{
    float h = room.Height;
    float cs = grid.GetCellSize();

    float x0 = room.X;
    float x1 = room.X + room.Width;
    float z0 = room.Z;
    float z1 = room.Z + room.Depth;

    glm::vec3 normal;
    bool      isNS;

    switch (side) {
    case WallSide::North: normal = { 0.0f, 0.0f, -1.0f }; isNS = true;  break;
    case WallSide::South: normal = { 0.0f, 0.0f,  1.0f }; isNS = true;  break;
    case WallSide::East:  normal = { -1.0f, 0.0f,  0.0f }; isNS = false; break;
    case WallSide::West:  normal = { 1.0f, 0.0f,  0.0f }; isNS = false; break;
    }

    float totalLength = isNS ? (x1 - x0) : (z1 - z0);

    // find gap ranges by scanning adjacent corridor cells
    std::vector<std::pair<float, float>> gaps;

    if (isNS) {
        int gz = (side == WallSide::North)
            ? grid.WorldToGridZ(z1 + cs * 1.5f)
            : grid.WorldToGridZ(z0 - cs * 1.5f);

        int gxStart = grid.WorldToGridX(x0);
        int gxEnd = grid.WorldToGridX(x1);

        float gapStart = -1.0f;
        for (int gx = gxStart; gx <= gxEnd; gx++) {
            bool isCorridor = grid.InBounds(gx, gz) &&
                grid.GetCell(gx, gz).Type == CellType::Corridor;
            if (isCorridor && gapStart < 0.0f)
                gapStart = gx * cs;
            else if (!isCorridor && gapStart >= 0.0f) {
                gaps.push_back({ gapStart - x0, gx * cs - x0 });
                gapStart = -1.0f;
            }
        }
        if (gapStart >= 0.0f)
            gaps.push_back({ gapStart - x0, x1 - x0 });

    }
    else {
        int gx = (side == WallSide::East)
            ? grid.WorldToGridX(x1 + cs * 1.5f)
            : grid.WorldToGridX(x0 - cs * 1.5f);

        int gzStart = grid.WorldToGridZ(z0);
        int gzEnd = grid.WorldToGridZ(z1);

        float gapStart = -1.0f;
        for (int gz = gzStart; gz <= gzEnd; gz++) {
            bool isCorridor = grid.InBounds(gx, gz) &&
                grid.GetCell(gx, gz).Type == CellType::Corridor;
            if (isCorridor && gapStart < 0.0f)
                gapStart = gz * cs;
            else if (!isCorridor && gapStart >= 0.0f) {
                gaps.push_back({ gapStart - z0, gz * cs - z0 });
                gapStart = -1.0f;
            }
        }
        if (gapStart >= 0.0f)
            gaps.push_back({ gapStart - z0, z1 - z0 });
    }

    // build solid wall segments between gaps
    std::vector<std::pair<float, float>> segments;
    float cursor = 0.0f;
    for (auto& gap : gaps) {
        if (cursor < gap.first)
            segments.push_back({ cursor, gap.first });
        cursor = gap.second;
    }
    if (cursor < totalLength)
        segments.push_back({ cursor, totalLength });

    for (auto& [start, end] : segments) {
        float segLen = end - start;
        if (segLen <= 0.0f) continue;

        glm::vec3 p0, p1, p2, p3;

        switch (side) {
        case WallSide::North:
            p0 = { x0 + start, 0.0f, z1 };
            p1 = { x0 + end,   0.0f, z1 };
            p2 = { x0 + end,   h,    z1 };
            p3 = { x0 + start, h,    z1 };
            break;
        case WallSide::South:
            p0 = { x0 + end,   0.0f, z0 };
            p1 = { x0 + start, 0.0f, z0 };
            p2 = { x0 + start, h,    z0 };
            p3 = { x0 + end,   h,    z0 };
            break;
        case WallSide::East:
            p0 = { x1, 0.0f, z0 + end };
            p1 = { x1, 0.0f, z0 + start };
            p2 = { x1, h,    z0 + start };
            p3 = { x1, h,    z0 + end };
            break;
        case WallSide::West:
            p0 = { x0, 0.0f, z0 + start };
            p1 = { x0, 0.0f, z0 + end };
            p2 = { x0, h,    z0 + end };
            p3 = { x0, h,    z0 + start };
            break;
        }

        AddQuad(vertices, indices, p0, p1, p2, p3, normal,
            segLen / h, 1.0f);
    }

    // emit above-door strips for each gap
    for (auto& gap : gaps) {
        float gapStart = gap.first;
        float gapEnd = gap.second;
        float gapLen = gapEnd - gapStart;
        if (gapLen <= 0.0f) continue;

        float stripH = h - doorHeight;
        if (stripH <= 0.0f) continue;

        glm::vec3 q0, q1, q2, q3;

        switch (side) {
        case WallSide::North:
            q0 = { x0 + gapStart, doorHeight, z1 };
            q1 = { x0 + gapEnd,   doorHeight, z1 };
            q2 = { x0 + gapEnd,   h,          z1 };
            q3 = { x0 + gapStart, h,          z1 };
            break;
        case WallSide::South:
            q0 = { x0 + gapEnd,   doorHeight, z0 };
            q1 = { x0 + gapStart, doorHeight, z0 };
            q2 = { x0 + gapStart, h,          z0 };
            q3 = { x0 + gapEnd,   h,          z0 };
            break;
        case WallSide::East:
            q0 = { x1, doorHeight, z0 + gapEnd };
            q1 = { x1, doorHeight, z0 + gapStart };
            q2 = { x1, h,          z0 + gapStart };
            q3 = { x1, h,          z0 + gapEnd };
            break;
        case WallSide::West:
            q0 = { x0, doorHeight, z0 + gapStart };
            q1 = { x0, doorHeight, z0 + gapEnd };
            q2 = { x0, h,          z0 + gapEnd };
            q3 = { x0, h,          z0 + gapStart };
            break;
        }

        AddQuad(vertices, indices, q0, q1, q2, q3, normal,
            gapLen / stripH, 1.0f);
    }
}

void DungeonMeshBuilder::BuildRoom(DynamicMesh& mesh,
    const RoomData& room,
    const DungeonGrid& grid)
{
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    float x0 = room.X;
    float x1 = room.X + room.Width;
    float z0 = room.Z;
    float z1 = room.Z + room.Depth;
    float h = room.Height;
    float doorHeight = h * 0.75f;

    // floor
    AddQuad(vertices, indices,
        { x0, 0.0f, z1 }, { x1, 0.0f, z1 },
        { x1, 0.0f, z0 }, { x0, 0.0f, z0 },
        { 0.0f, 1.0f, 0.0f },
        room.Width, room.Depth);

    // ceiling
    AddQuad(vertices, indices,
        { x0, h, z0 }, { x1, h, z0 },
        { x1, h, z1 }, { x0, h, z1 },
        { 0.0f, -1.0f, 0.0f },
        room.Width, room.Depth);

    // walls with grid-derived door gaps
    AddWallWithGridDoors(vertices, indices, room, WallSide::North, doorHeight, grid);
    AddWallWithGridDoors(vertices, indices, room, WallSide::South, doorHeight, grid);
    AddWallWithGridDoors(vertices, indices, room, WallSide::East, doorHeight, grid);
    AddWallWithGridDoors(vertices, indices, room, WallSide::West, doorHeight, grid);

    mesh.Upload(vertices, indices);
}

void DungeonMeshBuilder::BuildCorridorMesh(DynamicMesh& mesh,
    const DungeonGrid& grid,
    float roomHeight)
{
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    float cs = grid.GetCellSize();
    float h = roomHeight;

    const int dx[] = { 0,  0,  1, -1 };
    const int dz[] = { 1, -1,  0,  0 };

    for (int z = 0; z < grid.GetDepth(); z++) {
        for (int x = 0; x < grid.GetWidth(); x++) {
            const GridCell& cell = grid.GetCell(x, z);
            if (cell.Type != CellType::Corridor) continue;

            float wx = x * cs;
            float wz = z * cs;

            // check if any neighbor is a floor cell (room interior)
            // extend floor/ceiling toward room to close the gap
            float extN = 0.0f, extS = 0.0f, extE = 0.0f, extW = 0.0f;

            if (grid.InBounds(x, z + 1) && grid.GetCell(x, z + 1).Type == CellType::Floor) extN = cs;
            if (grid.InBounds(x, z - 1) && grid.GetCell(x, z - 1).Type == CellType::Floor) extS = cs;
            if (grid.InBounds(x + 1, z) && grid.GetCell(x + 1, z).Type == CellType::Floor) extE = cs;
            if (grid.InBounds(x - 1, z) && grid.GetCell(x - 1, z).Type == CellType::Floor) extW = cs;

            // floor — extended toward room on sides that touch room cells
            AddQuad(vertices, indices,
                { wx - extW,      0.0f, wz - extS },
                { wx + cs + extE, 0.0f, wz - extS },
                { wx + cs + extE, 0.0f, wz + cs + extN },
                { wx - extW,      0.0f, wz + cs + extN },
                { 0.0f, 1.0f, 0.0f },
                cs, cs);

            // ceiling — extended toward room
            AddQuad(vertices, indices,
                { wx - extW,      h, wz + cs + extN },
                { wx + cs + extE, h, wz + cs + extN },
                { wx + cs + extE, h, wz - extS },
                { wx - extW,      h, wz - extS },
                { 0.0f, -1.0f, 0.0f },
                cs, cs);

            // wall faces where neighbor is wall
            for (int d = 0; d < 4; d++) {
                int nx = x + dx[d];
                int nz = z + dz[d];

                bool isWall = !grid.InBounds(nx, nz) ||
                    grid.GetCell(nx, nz).Type == CellType::Wall;
                if (!isWall) continue;

                glm::vec3 p0, p1, p2, p3, normal;

                if (d == 0) {
                    normal = { 0.0f, 0.0f, -1.0f };
                    p0 = { wx + cs, 0.0f, wz + cs };
                    p1 = { wx,      0.0f, wz + cs };
                    p2 = { wx,      h,    wz + cs };
                    p3 = { wx + cs, h,    wz + cs };
                }
                else if (d == 1) {
                    normal = { 0.0f, 0.0f, 1.0f };
                    p0 = { wx,      0.0f, wz };
                    p1 = { wx + cs, 0.0f, wz };
                    p2 = { wx + cs, h,    wz };
                    p3 = { wx,      h,    wz };
                }
                else if (d == 2) {
                    normal = { -1.0f, 0.0f, 0.0f };
                    p0 = { wx + cs, 0.0f, wz };
                    p1 = { wx + cs, 0.0f, wz + cs };
                    p2 = { wx + cs, h,    wz + cs };
                    p3 = { wx + cs, h,    wz };
                }
                else {
                    normal = { 1.0f, 0.0f, 0.0f };
                    p0 = { wx, 0.0f, wz + cs };
                    p1 = { wx, 0.0f, wz };
                    p2 = { wx, h,    wz };
                    p3 = { wx, h,    wz + cs };
                }

                AddQuad(vertices, indices, p0, p1, p2, p3, normal,
                    1.0f, h / cs);
            }
        }
    }

    if (!vertices.empty())
        mesh.Upload(vertices, indices);
}