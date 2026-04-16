#include "DungeonScene.h"
#include "DungeonMeshBuilder.h"
#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

DungeonScene::DungeonScene(const DungeonData& data,
    std::shared_ptr<Material> wallMaterial,
    std::shared_ptr<Material> floorMaterial)
    : m_Data(data)
    , m_WallMaterial(std::move(wallMaterial))
    , m_FloorMaterial(std::move(floorMaterial))
{
    for (const auto& roomData : data.Rooms)
        m_Rooms.emplace_back(roomData, m_WallMaterial);

    for (const auto& corridor : data.Corridors) {
        DynamicMesh mesh;
        DungeonMeshBuilder::BuildCorridor(mesh, corridor);
        m_CorridorMeshes.push_back(std::move(mesh));
    }
}

void DungeonScene::Update(float deltaTime, const Camera& camera) {
    // room state updates go here later
}

void DungeonScene::Render(const RenderContext& ctx) const {
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);

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

    //glDisable(GL_CULL_FACE);
}