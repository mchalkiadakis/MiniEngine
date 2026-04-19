#include "DungeonGenerator.h"
#include "AStar.h"
#include <random>
#include <algorithm>
#include <cmath>
#include <climits>
#include <cfloat>
#include <iostream>

DungeonData DungeonGenerator::Generate(const DungeonConfig& config) {
    std::mt19937 rng(config.Seed);
    DungeonData data;
    data.RoomHeight = config.RoomHeight;

    // create grid
    data.Grid = std::make_shared<DungeonGrid>(
        config.FloorWidth, config.FloorDepth, config.CellSize);

    std::vector<BSPNode> nodes;
    BSPNode root;
    root.X = 0; root.Z = 0;
    root.Width = config.FloorWidth;
    root.Depth = config.FloorDepth;
    nodes.push_back(root);

    int minSize = static_cast<int>(std::max(config.MaxRoomWidth,
        config.MaxRoomDepth)
        + config.NodePadding);

    bool splitHappened = true;
    while (splitHappened) {
        splitHappened = false;
        int count = static_cast<int>(nodes.size());
        for (int i = 0; i < count; i++) {
            if (nodes[i].IsLeaf() &&
                (nodes[i].Width >= minSize * 2 ||
                    nodes[i].Depth >= minSize * 2)) {
                Split(nodes, i, minSize, rng);
                splitHappened = true;
            }
        }
    }

    PlaceRooms(nodes, data, config, rng);

    // mark rooms on grid
    for (const auto& room : data.Rooms)
        data.Grid->MarkRect(room.X, room.Z,
            room.Width, room.Depth,
            CellType::Floor);

    // connect rooms using A*
    ConnectRooms(data, config);

    AssignRoomTypes(data, rng);

    return data;
}

void DungeonGenerator::Split(std::vector<BSPNode>& nodes, int idx,
    int minSize, std::mt19937& rng)
{
    BSPNode& node = nodes[idx];
    if (node.Width < minSize * 2 && node.Depth < minSize * 2) return;

    bool splitH;
    if (node.Width > node.Depth * 1.25f)      splitH = false;
    else if (node.Depth > node.Width * 1.25f) splitH = true;
    else {
        std::uniform_int_distribution<int> coin(0, 1);
        splitH = coin(rng);
    }

    BSPNode left, right;
    if (splitH) {
        std::uniform_real_distribution<float> split(
            node.Z + minSize, node.Z + node.Depth - minSize);
        float splitZ = split(rng);
        left = { node.X, node.Z, node.Width, splitZ - node.Z };
        right = { node.X, splitZ, node.Width, node.Z + node.Depth - splitZ };
    }
    else {
        std::uniform_real_distribution<float> split(
            node.X + minSize, node.X + node.Width - minSize);
        float splitX = split(rng);
        left = { node.X, node.Z, splitX - node.X, node.Depth };
        right = { splitX, node.Z, node.X + node.Width - splitX, node.Depth };
    }

    node.Left = static_cast<int>(nodes.size());
    nodes.push_back(left);
    node.Right = static_cast<int>(nodes.size());
    nodes.push_back(right);
}

void DungeonGenerator::PlaceRooms(std::vector<BSPNode>& nodes,
    DungeonData& data,
    const DungeonConfig& cfg,
    std::mt19937& rng)
{
    for (auto& node : nodes) {
        if (!node.IsLeaf()) continue;

        if (node.Width < cfg.MinRoomWidth + 4.0f ||
            node.Depth < cfg.MinRoomDepth + 4.0f) continue;

        float maxW = std::min(cfg.MaxRoomWidth, node.Width - 4.0f);
        float maxD = std::min(cfg.MaxRoomDepth, node.Depth - 4.0f);

        if (maxW < cfg.MinRoomWidth || maxD < cfg.MinRoomDepth) continue;

        std::uniform_real_distribution<float> rw(cfg.MinRoomWidth, maxW);
        std::uniform_real_distribution<float> rd(cfg.MinRoomDepth, maxD);

        float rWidth = rw(rng);
        float rDepth = rd(rng);

        float rxMax = node.X + node.Width - rWidth - 2.0f;
        float rzMax = node.Z + node.Depth - rDepth - 2.0f;

        if (rxMax < node.X + 2.0f || rzMax < node.Z + 2.0f) continue;

        std::uniform_real_distribution<float> rx(node.X + 2.0f, rxMax);
        std::uniform_real_distribution<float> rz(node.Z + 2.0f, rzMax);

        RoomData room;
        room.Index = static_cast<int>(data.Rooms.size());
        room.X = rx(rng);
        room.Z = rz(rng);
        room.Width = rWidth;
        room.Depth = rDepth;
        room.Height = cfg.RoomHeight;

        node.RoomIndex = room.Index;
        data.Rooms.push_back(room);
    }
}

void DungeonGenerator::ConnectRooms(DungeonData& data, const DungeonConfig& cfg)
{
    if (data.Rooms.size() < 2) return;

    for (int i = 0; i < (int)data.Rooms.size() - 1; i++) {
        glm::vec2 ci = data.Rooms[i].Center();
        glm::vec2 cj = data.Rooms[i + 1].Center();

        int startX = data.Grid->WorldToGridX(ci.x);
        int startZ = data.Grid->WorldToGridZ(ci.y);
        int endX = data.Grid->WorldToGridX(cj.x);
        int endZ = data.Grid->WorldToGridZ(cj.y);

        std::vector<glm::ivec2> path = AStar::FindPath(
            *data.Grid, startX, startZ, endX, endZ);

        if (path.empty()) {
            std::cout << "Warning: no path found between room "
                << i << " and " << i + 1 << "\n";
            continue;
        }

        int hw = std::max(3, static_cast<int>(
            cfg.CorridorWidth * 0.5f / cfg.CellSize));

        auto carveWidth = [&](glm::ivec2 cell) {
            for (int ddx = -hw; ddx <= hw; ddx++) {
                for (int ddz = -hw; ddz <= hw; ddz++) {
                    int nx = cell.x + ddx;
                    int nz = cell.y + ddz;
                    if (!data.Grid->InBounds(nx, nz)) continue;
                    if (data.Grid->GetCell(nx, nz).Type == CellType::Wall)
                        data.Grid->GetCell(nx, nz).Type = CellType::Corridor;
                }
            }
            };

        // carve full width along entire path
        for (auto& cell : path)
            carveWidth(cell);

        // force extra width at start and end to guarantee
        // connection with room walls
        int endZone = std::min(8, (int)path.size());
        for (int p = 0; p < endZone; p++)
            carveWidth(path[p]);
        for (int p = (int)path.size() - endZone; p < (int)path.size(); p++)
            carveWidth(path[p]);
    }
}

void DungeonGenerator::AssignRoomTypes(DungeonData& data, std::mt19937& rng)
{
    if (data.Rooms.empty()) return;

    data.Rooms[0].Type = RoomType::Start;
    data.StartRoomIndex = 0;

    float maxDist = 0.0f;
    int   bossIdx = 0;
    for (int i = 1; i < (int)data.Rooms.size(); i++) {
        float dist = glm::length(data.Rooms[i].Center() -
            data.Rooms[0].Center());
        if (dist > maxDist) { maxDist = dist; bossIdx = i; }
    }
    data.Rooms[bossIdx].Type = RoomType::Boss;
    data.BossRoomIndex = bossIdx;

    std::uniform_real_distribution<float> chance(0.0f, 1.0f);
    for (int i = 1; i < (int)data.Rooms.size(); i++) {
        if (i == bossIdx) continue;
        if (chance(rng) < 0.2f)
            data.Rooms[i].Type = RoomType::Treasure;
    }
}