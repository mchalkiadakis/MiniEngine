#pragma once
#include <glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include "DungeonGrid.h"

enum class RoomType {
    Start,
    Combat,
    Treasure,
    Boss,
    Corridor
};

enum class WallSide {
    North,
    South,
    East,
    West
};

struct DoorData {
    glm::vec2 Position;
    glm::vec2 Direction;
    int       ConnectsTo;
    WallSide  Side;
    float     CenterAlong;
    float     Width = 6.0f;
};

struct RoomData {
    int      Index;
    RoomType Type = RoomType::Combat;
    float    X = 0.0f;
    float    Z = 0.0f;
    float    Width = 20.0f;
    float    Depth = 20.0f;
    float    Height = 8.0f;
    std::vector<DoorData> Doors;

    glm::vec2 Center() const {
        return glm::vec2(X + Width * 0.5f, Z + Depth * 0.5f);
    }
};

struct DungeonData {
    std::vector<RoomData>        Rooms;
    std::shared_ptr<DungeonGrid> Grid;
    float                        RoomHeight = 8.0f;
    int                          StartRoomIndex = 0;
    int                          BossRoomIndex = 0;
};