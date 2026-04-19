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
        m_Rooms.emplace_back(roomData, m_WallMaterial, *data.Grid);

    if (data.Grid)
        DungeonMeshBuilder::BuildCorridorMesh(m_CorridorMesh,
            *data.Grid,
            data.RoomHeight);

    std::cout << "Built " << m_Rooms.size() << " room meshes\n";
    std::cout << "Built corridor mesh from grid\n";
}

void DungeonScene::Update(float deltaTime, const Camera& camera) {
    Scene::Update(deltaTime, camera); // call parent — updates entities
}

void DungeonScene::Render(const RenderContext& ctx) const {
    Scene::Render(ctx); // call parent — renders entities

    for (const auto& room : m_Rooms)
        room.Render(ctx);

    if (m_WallMaterial) {
        auto shader = m_WallMaterial->GetShader();
        m_WallMaterial->Bind();
        ctx.ApplyToShader(*shader, glm::mat4(1.0f));
        m_CorridorMesh.Draw();
    }
}

void DungeonScene::RequestTransition(std::unique_ptr<IScene> nextScene) {
    m_NextScene = std::move(nextScene);
}

std::unique_ptr<IScene> DungeonScene::GetNextScene() {
    return std::move(m_NextScene);
}