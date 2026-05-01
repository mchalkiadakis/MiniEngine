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
    if (data.Grid) {
        DungeonMeshBuilder::BuildDungeonWallMesh(m_WallMesh,
            *data.Grid, data.RoomHeight);
        DungeonMeshBuilder::BuildDungeonFloorMesh(m_FloorMesh,
            *data.Grid, data.RoomHeight);
    }

    std::cout << "Built dungeon wall mesh\n";
    std::cout << "Built dungeon floor mesh\n";
}

void DungeonScene::Update(float deltaTime, const Camera& camera) {
    Scene::Update(deltaTime, camera);
}

void DungeonScene::Render(const RenderContext& ctx) const {
    Scene::Render(ctx);

    // walls
    if (m_WallMaterial) {
        auto shader = m_WallMaterial->GetShader();
        m_WallMaterial->Bind();
        ctx.ApplyToShader(*shader, glm::mat4(1.0f));
        m_WallMesh.Draw();
    }

    // floor and ceiling
    if (m_FloorMaterial) {
        auto shader = m_FloorMaterial->GetShader();
        m_FloorMaterial->Bind();
        ctx.ApplyToShader(*shader, glm::mat4(1.0f));
        m_FloorMesh.Draw();
    }
}

void DungeonScene::RenderDepth(Shader& depthShader,
    const glm::mat4& lightSpaceMatrix) const {
    Scene::RenderDepth(depthShader, lightSpaceMatrix);

    depthShader.Use();
    depthShader.SetUniformMat4("u_LightSpaceMatrix",
        glm::value_ptr(lightSpaceMatrix));

    glm::mat4 identity(1.0f);
    depthShader.SetUniformMat4("u_Model", glm::value_ptr(identity));

    m_WallMesh.Draw();
    m_FloorMesh.Draw();
}

void DungeonScene::RequestTransition(std::unique_ptr<IScene> nextScene) {
    m_NextScene = std::move(nextScene);
}

std::unique_ptr<IScene> DungeonScene::GetNextScene() {
    return std::move(m_NextScene);
}