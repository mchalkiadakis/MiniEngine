#include "DungeonGenerator.h"
#include <random>
#include <algorithm>
#include <cmath>
#include <climits>
#include <cfloat>

DungeonData DungeonGenerator::Generate(const DungeonConfig& config) {
    std::mt19937 rng(config.Seed);
    DungeonData data;

    std::vector<BSPNode> nodes;
    BSPNode root;
    root.X = 0; root.Z = 0;
    root.Width = config.FloorWidth;
    root.Depth = config.FloorDepth;
    nodes.push_back(root);

    int minSize = static_cast<int>(std::max(config.MaxRoomWidth,
        config.MaxRoomDepth) + config.NodePadding);

    // keep splitting until no more splits are possible
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
    ConnectRooms(data, config);
    AssignRoomTypes(data, rng);

    return data;
}

void DungeonGenerator::Split(std::vector<BSPNode>& nodes, int idx,
    int minSize, std::mt19937& rng)
{
    BSPNode& node = nodes[idx];
    if (node.Width < minSize * 2 && node.Depth < minSize * 2) return;

    bool splitH; // true = horizontal split (divide along Z)
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

        // skip if node is too small for minimum room size
        if (node.Width < cfg.MinRoomWidth + 4.0f ||
            node.Depth < cfg.MinRoomDepth + 4.0f) continue;

        float maxW = std::min(cfg.MaxRoomWidth, node.Width - 4.0f);
        float maxD = std::min(cfg.MaxRoomDepth, node.Depth - 4.0f);

        // skip if max is smaller than min after clamping
        if (maxW < cfg.MinRoomWidth || maxD < cfg.MinRoomDepth) continue;

        std::uniform_real_distribution<float> rw(cfg.MinRoomWidth, maxW);
        std::uniform_real_distribution<float> rd(cfg.MinRoomDepth, maxD);

        float rWidth = rw(rng);
        float rDepth = rd(rng);

        float rxMax = node.X + node.Width - rWidth - 2.0f;
        float rzMax = node.Z + node.Depth - rDepth - 2.0f;

        // skip if placement range is invalid
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

void DungeonGenerator::ConnectRooms(DungeonData& data,
    const DungeonConfig& cfg)
{
    // connect each room to its nearest unconnected neighbor
    for (int i = 0; i < (int)data.Rooms.size(); i++) {
        float minDist = FLT_MAX;
        int   nearest = -1;

        for (int j = 0; j < (int)data.Rooms.size(); j++) {
            if (i == j) continue;

            // check if already connected
            bool connected = false;
            for (auto& d : data.Rooms[i].Doors)
                if (d.ConnectsTo == j) { connected = true; break; }
            if (connected) continue;

            glm::vec2 ci = data.Rooms[i].Center();
            glm::vec2 cj = data.Rooms[j].Center();
            float dist = glm::length(cj - ci);

            if (dist < minDist) {
                minDist = dist;
                nearest = j;
            }
        }

        if (nearest == -1) continue;

        // create corridor between centers
        glm::vec2 ci = data.Rooms[i].Center();
        glm::vec2 cj = data.Rooms[nearest].Center();

        // add doors
        DoorData doorI;
        doorI.ConnectsTo = nearest;
        doorI.Position = ci;
        data.Rooms[i].Doors.push_back(doorI);

        DoorData doorJ;
        doorJ.ConnectsTo = i;
        doorJ.Position = cj;
        data.Rooms[nearest].Doors.push_back(doorJ);

        // L-shaped corridor — horizontal then vertical
        float hw = cfg.CorridorWidth * 0.5f;

        CorridorData hCorridor;
        hCorridor.RoomAIndex = i;
        hCorridor.RoomBIndex = nearest;
        hCorridor.X = std::min(ci.x, cj.x) - hw;
        hCorridor.Z = ci.y - hw;
        hCorridor.Width = std::abs(cj.x - ci.x) + cfg.CorridorWidth;
        hCorridor.Depth = cfg.CorridorWidth;
        hCorridor.Height = cfg.RoomHeight;
        data.Corridors.push_back(hCorridor);

        CorridorData vCorridor;
        vCorridor.RoomAIndex = i;
        vCorridor.RoomBIndex = nearest;
        vCorridor.X = cj.x - hw;
        vCorridor.Z = std::min(ci.y, cj.y) - hw;
        vCorridor.Width = cfg.CorridorWidth;
        vCorridor.Depth = std::abs(cj.y - ci.y) + cfg.CorridorWidth;
        vCorridor.Height = cfg.RoomHeight;
        data.Corridors.push_back(vCorridor);
    }
}

void DungeonGenerator::AssignRoomTypes(DungeonData& data,
    std::mt19937& rng)
{
    if (data.Rooms.empty()) return;

    // start room — first room
    data.Rooms[0].Type = RoomType::Start;
    data.StartRoomIndex = 0;

    // boss room — furthest from start
    float maxDist = 0.0f;
    int   bossIdx = 0;
    for (int i = 1; i < (int)data.Rooms.size(); i++) {
        float dist = glm::length(data.Rooms[i].Center() -
            data.Rooms[0].Center());
        if (dist > maxDist) { maxDist = dist; bossIdx = i; }
    }
    data.Rooms[bossIdx].Type = RoomType::Boss;
    data.BossRoomIndex = bossIdx;

    // randomly assign treasure rooms (~20% chance)
    std::uniform_real_distribution<float> chance(0.0f, 1.0f);
    for (int i = 1; i < (int)data.Rooms.size(); i++) {
        if (i == bossIdx) continue;
        if (chance(rng) < 0.2f)
            data.Rooms[i].Type = RoomType::Treasure;
    }
}