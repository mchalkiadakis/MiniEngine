#pragma once
#include <glm.hpp>
#include <vector>
#include <string>

enum class RoomType {
    Start,
    Combat,
    Treasure,
    Boss,
    Corridor
};


enum class WallSide {
    North, // +Z wall
    South, // -Z wall
    East,  // +X wall
    West   // -X wall
};

struct DoorData {
    glm::vec2 Position;   // center of door in world space
    glm::vec2 Direction;  // normalized direction
    int       ConnectsTo; // index of connected room
    WallSide  Side;       // which wall this door is on
    float     CenterAlong; // position along the wall (0-1)
    float     Width = 6.0f; // door opening width
};

struct RoomData {
    int       Index;
    RoomType  Type = RoomType::Combat;
    float     X = 0.0f; // world space position
    float     Z = 0.0f;
    float     Width = 20.0f;
    float     Depth = 20.0f;
    float     Height = 8.0f;
    std::vector<DoorData> Doors;

    glm::vec2 Center() const {
        return glm::vec2(X + Width * 0.5f, Z + Depth * 0.5f);
    }

    bool Overlaps(const RoomData& other, float padding = 2.0f) const {
        return X < other.X + other.Width + padding &&
            X + Width + padding > other.X &&
            Z < other.Z + other.Depth + padding &&
            Z + Depth + padding > other.Z;
    }
};

struct CorridorData {
    int   RoomAIndex;
    int   RoomBIndex;
    float X, Z;
    float Width, Depth;
    float Height = 8.0f;
};

struct DungeonData {
    std::vector<RoomData>     Rooms;
    std::vector<CorridorData> Corridors;
    int StartRoomIndex = 0;
    int BossRoomIndex = 0;
};