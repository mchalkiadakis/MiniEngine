#include "DungeonMeshBuilder.h"
#include "Rendering/Mesh.h"
#include <algorithm>

// helper to add a quad as two triangles
void DungeonMeshBuilder::AddQuad(std::vector<Vertex>& vertices,
    std::vector<unsigned int>& indices,
    const glm::vec3& p0, const glm::vec3& p1,
    const glm::vec3& p2, const glm::vec3& p3,
    const glm::vec3& normal,
    float uScale, float vScale)
{
    unsigned int base = static_cast<unsigned int>(vertices.size());

    vertices.push_back({ p0, normal, { 0.0f,    0.0f    } });
    vertices.push_back({ p1, normal, { uScale,  0.0f    } });
    vertices.push_back({ p2, normal, { uScale,  vScale  } });
    vertices.push_back({ p3, normal, { 0.0f,    vScale  } });

    indices.push_back(base + 0);
    indices.push_back(base + 1);
    indices.push_back(base + 2);
    indices.push_back(base + 2);
    indices.push_back(base + 3);
    indices.push_back(base + 0);
}

void DungeonMeshBuilder::AddWallWithDoors(std::vector<Vertex>& vertices,
    std::vector<unsigned int>& indices,
    const RoomData& room,
    WallSide side,
    float doorHeight)
{
    float h = room.Height;

    // collect doors on this wall, sorted by position along wall
    std::vector<const DoorData*> wallDoors;
    for (const auto& door : room.Doors)
        if (door.Side == side)
            wallDoors.push_back(&door);

    // sort doors left to right along the wall
    std::sort(wallDoors.begin(), wallDoors.end(),
        [&](const DoorData* a, const DoorData* b) {
            return a->CenterAlong < b->CenterAlong;
        });

    // wall start/end in world space
    float wallStart, wallEnd;
    glm::vec3 normal;
    bool isEastWest = (side == WallSide::East || side == WallSide::West);

    float x0 = room.X;
    float x1 = room.X + room.Width;
    float z0 = room.Z;
    float z1 = room.Z + room.Depth;

    // build wall segments around door gaps
    // segments track position along the wall axis
    std::vector<std::pair<float, float>> segments; // (start, end) pairs with no gap
    float cursor = 0.0f;
    float totalLength = isEastWest ? room.Depth : room.Width;

    for (const auto* door : wallDoors) {
        float doorCenter = door->CenterAlong * totalLength;
        float halfW = door->Width * 0.5f;
        float gapStart = glm::clamp(doorCenter - halfW, 0.0f, totalLength);
        float gapEnd = glm::clamp(doorCenter + halfW, 0.0f, totalLength);

        if (cursor < gapStart)
            segments.push_back({ cursor, gapStart });
        cursor = gapEnd;
    }
    if (cursor < totalLength)
        segments.push_back({ cursor, totalLength });

    // emit geometry for each segment
    for (auto& [start, end] : segments) {
        float segLen = end - start;
        if (segLen <= 0.0f) continue;

        glm::vec3 p0, p1, p2, p3;

        switch (side) {
        case WallSide::North: // +Z wall, normal points -Z (inward)
            normal = glm::vec3(0.0f, 0.0f, -1.0f);
            p0 = { x0 + start, 0.0f, z1 };
            p1 = { x0 + end,   0.0f, z1 };
            p2 = { x0 + end,   h,    z1 };
            p3 = { x0 + start, h,    z1 };
            break;
        case WallSide::South: // -Z wall, normal points +Z (inward)
            normal = glm::vec3(0.0f, 0.0f, 1.0f);
            p0 = { x0 + end,   0.0f, z0 };
            p1 = { x0 + start, 0.0f, z0 };
            p2 = { x0 + start, h,    z0 };
            p3 = { x0 + end,   h,    z0 };
            break;
        case WallSide::East: // +X wall, normal points -X (inward)
            normal = glm::vec3(-1.0f, 0.0f, 0.0f);
            p0 = { x1, 0.0f, z0 + end };
            p1 = { x1, 0.0f, z0 + start };
            p2 = { x1, h,    z0 + start };
            p3 = { x1, h,    z0 + end };
            break;
        case WallSide::West: // -X wall, normal points +X (inward)
            normal = glm::vec3(1.0f, 0.0f, 0.0f);
            p0 = { x0, 0.0f, z0 + start };
            p1 = { x0, 0.0f, z0 + end };
            p2 = { x0, h,    z0 + end };
            p3 = { x0, h,    z0 + start };
            break;
        }

        // above door segment if there are doors
        // full height wall if no doors on this side
        AddQuad(vertices, indices, p0, p1, p2, p3, normal,
            segLen / h, 1.0f);

        // add strip above each door gap
        for (const auto* door : wallDoors) {
            float doorCenter = door->CenterAlong * totalLength;
            float halfW = door->Width * 0.5f;
            float gapStart = glm::clamp(doorCenter - halfW, 0.0f, totalLength);
            float gapEnd = glm::clamp(doorCenter + halfW, 0.0f, totalLength);
            float gapLen = gapEnd - gapStart;

            glm::vec3 q0, q1, q2, q3;
            float stripH = h - doorHeight;

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
}

void DungeonMeshBuilder::BuildRoom(DynamicMesh& mesh, const RoomData& room)
{
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    float x0 = room.X;
    float x1 = room.X + room.Width;
    float z0 = room.Z;
    float z1 = room.Z + room.Depth;
    float h = room.Height;
    float doorHeight = h * 0.75f; // door is 75% of room height

    // floor — normal up, CCW from above
    AddQuad(vertices, indices,
        { x0, 0.0f, z1 }, { x1, 0.0f, z1 },
        { x1, 0.0f, z0 }, { x0, 0.0f, z0 },
        { 0.0f, 1.0f, 0.0f },
        room.Width, room.Depth);

    // ceiling — normal down, CCW from below
    AddQuad(vertices, indices,
        { x0, h, z0 }, { x1, h, z0 },
        { x1, h, z1 }, { x0, h, z1 },
        { 0.0f, -1.0f, 0.0f },
        room.Width, room.Depth);

    // four walls with door gaps
    AddWallWithDoors(vertices, indices, room, WallSide::North, doorHeight);
    AddWallWithDoors(vertices, indices, room, WallSide::South, doorHeight);
    AddWallWithDoors(vertices, indices, room, WallSide::East, doorHeight);
    AddWallWithDoors(vertices, indices, room, WallSide::West, doorHeight);

    mesh.Upload(vertices, indices);
}

void DungeonMeshBuilder::BuildCorridor(DynamicMesh& mesh,
    const CorridorData& corridor)
{
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    float x0 = corridor.X;
    float x1 = corridor.X + corridor.Width;
    float z0 = corridor.Z;
    float z1 = corridor.Z + corridor.Depth;
    float h = corridor.Height;

    // floor
    AddQuad(vertices, indices,
        { x0, 0.0f, z0 }, { x1, 0.0f, z0 },
        { x1, 0.0f, z1 }, { x0, 0.0f, z1 },
        { 0.0f, 1.0f, 0.0f },
        corridor.Width, corridor.Depth);

    // ceiling
    AddQuad(vertices, indices,
        { x0, h, z1 }, { x1, h, z1 },
        { x1, h, z0 }, { x0, h, z0 },
        { 0.0f, -1.0f, 0.0f },
        corridor.Width, corridor.Depth);

    // north wall
    AddQuad(vertices, indices,
        { x1, 0.0f, z1 }, { x0, 0.0f, z1 },
        { x0, h,    z1 }, { x1, h,    z1 },
        { 0.0f, 0.0f, -1.0f },
        corridor.Width, h);

    // south wall
    AddQuad(vertices, indices,
        { x0, 0.0f, z0 }, { x1, 0.0f, z0 },
        { x1, h,    z0 }, { x0, h,    z0 },
        { 0.0f, 0.0f, 1.0f },
        corridor.Width, h);

    // east wall
    AddQuad(vertices, indices,
        { x1, 0.0f, z0 }, { x1, 0.0f, z1 },
        { x1, h,    z1 }, { x1, h,    z0 },
        { -1.0f, 0.0f, 0.0f },
        corridor.Depth, h);

    // west wall
    AddQuad(vertices, indices,
        { x0, 0.0f, z1 }, { x0, 0.0f, z0 },
        { x0, h,    z0 }, { x0, h,    z1 },
        { 1.0f, 0.0f, 0.0f },
        corridor.Depth, h);

    mesh.Upload(vertices, indices);
}