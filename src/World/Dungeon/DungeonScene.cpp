#include "DungeonScene.h"
#include "DungeonMeshBuilder.h"
#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>

DungeonScene::DungeonScene(const DungeonData& data,
    std::shared_ptr<Material> wallMaterial,
    std::shared_ptr<Material> floorMaterial)
    : m_Data(data)
    , m_WallMaterial(std::move(wallMaterial))
    , m_FloorMaterial(std::move(floorMaterial))
{

    
    for (const auto& roomData : data.Rooms)
        m_Rooms.emplace_back(roomData, m_WallMaterial);

    std::cout << "Built " << m_Rooms.size() << " room meshes\n";
    std::cout << "Built " << m_CorridorMeshes.size() << " corridor meshes\n";

    for (const auto& corridor : data.Corridors) {
        DynamicMesh mesh;
        DungeonMeshBuilder::BuildCorridor(mesh, corridor);
        m_CorridorMeshes.push_back(std::move(mesh));
    }
    std::cout << "Built " << m_Rooms.size() << " room meshes\n";
    std::cout << "Built " << m_CorridorMeshes.size() << " corridor meshes\n";

    // place torches in every room based on room type
    float margin = 8.0f;
    for (const auto& room : data.Rooms) {
        float intensity = 8.0f;
        glm::vec3 color = glm::vec3(1.0f, 0.4f, 0.05f); // default warm orange

        if (room.Type == RoomType::Boss) {
            intensity = 3.0f;
            color = glm::vec3(0.5f, 0.0f, 0.8f); // purple ominous
        }
        else if (room.Type == RoomType::Treasure) {
            intensity = 10.0f;
            color = glm::vec3(1.0f, 0.8f, 0.2f); // warm gold
        }
        else if (room.Type == RoomType::Start) {
            intensity = 10.0f;
            color = glm::vec3(1.0f, 0.5f, 0.1f); // brighter start room
        }

        m_Torches.push_back({ room.X + margin,                  4.0f, room.Z + margin });
        m_Torches.push_back({ room.X + room.Width - margin,     4.0f, room.Z + margin });
        m_Torches.push_back({ room.X + margin,                  4.0f, room.Z + room.Depth - margin });
        //m_Torches.push_back({ room.X + room.Width - margin,     4.0f, room.Z + room.Depth - margin });
    }
}

void DungeonScene::Update(float deltaTime, const Camera& camera) {
}

void DungeonScene::Render(const RenderContext& ctx) const {
    for (const auto& room : m_Rooms)
        room.Render(ctx);

    if (m_WallMaterial) {
        auto shader = m_WallMaterial->GetShader();
        for (const auto& corridorMesh : m_CorridorMeshes) {
            m_WallMaterial->Bind();
            ctx.ApplyToShader(*shader, glm::mat4(1.0f));
            corridorMesh.Draw();
        }
    }
}