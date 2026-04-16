#pragma once
#include "RoomData.h"
#include "Rendering/DynamicMesh.h"

class DungeonMeshBuilder {
public:
    DungeonMeshBuilder() = delete;

    static void BuildRoom(DynamicMesh& mesh, const RoomData& room);
    static void BuildCorridor(DynamicMesh& mesh, const CorridorData& corridor);

private:
    static void AddQuad(std::vector<Vertex>& vertices,
        std::vector<unsigned int>& indices,
        const glm::vec3& p0, const glm::vec3& p1,
        const glm::vec3& p2, const glm::vec3& p3,
        const glm::vec3& normal,
        float uScale = 1.0f, float vScale = 1.0f);

    static void AddWallWithDoors(std::vector<Vertex>& vertices,
        std::vector<unsigned int>& indices,
        const RoomData& room,
        WallSide side,
        float doorHeight);
};