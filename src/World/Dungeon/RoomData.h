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

struct DoorData {
    glm::vec2 Position;  // center of door in world space
    glm::vec2 Direction; // which wall it's on (normalized)
    int       ConnectsTo; // index of connected room
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