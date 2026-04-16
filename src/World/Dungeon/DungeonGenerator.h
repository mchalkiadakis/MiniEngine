#pragma once
#include "RoomData.h"
#include <random>

struct DungeonConfig {
    float FloorWidth = 200.0f;
    float FloorDepth = 200.0f;
    int   MinRooms = 8;
    int   MaxRooms = 15;
    float MinRoomWidth = 16.0f;
    float MaxRoomWidth = 40.0f;
    float MinRoomDepth = 16.0f;
    float MaxRoomDepth = 40.0f;
    float RoomHeight = 8.0f;
    float CorridorWidth = 6.0f;
    int   Seed = 42;
};

class DungeonGenerator {
public:
    DungeonGenerator() = delete;
    static DungeonData Generate(const DungeonConfig& config);

private:
    struct BSPNode {
        float X, Z, Width, Depth;
        int   RoomIndex = -1;
        int   Left = -1;
        int   Right = -1;

        bool IsLeaf() const { return Left == -1 && Right == -1; }
    };

    static void Split(std::vector<BSPNode>& nodes, int nodeIndex,
        int minSize, std::mt19937& rng);
    static void PlaceRooms(std::vector<BSPNode>& nodes,
        DungeonData& data,
        const DungeonConfig& cfg,
        std::mt19937& rng);
    static void ConnectRooms(DungeonData& data,
        const DungeonConfig& cfg);
    static void AssignRoomTypes(DungeonData& data, std::mt19937& rng);
};