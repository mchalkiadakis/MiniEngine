#pragma once
#include "RoomData.h"
#include "World/Scene.h"
#include "Rendering/RenderContext.h"
#include "Rendering/Material.h"
#include "Rendering/DynamicMesh.h"
#include "Camera/Camera.h"
#include <vector>
#include <memory>
#include <glm.hpp>

class DungeonScene : public Scene {
public:
    DungeonScene(const DungeonData& data,
        std::shared_ptr<Material> wallMaterial,
        std::shared_ptr<Material> floorMaterial);

    void Update(float deltaTime, const Camera& camera) override;
    void Render(const RenderContext& ctx) const override;
    void RenderDepth(Shader& depthShader,
        const glm::mat4& lightSpaceMatrix) const override;
    void RequestTransition(std::unique_ptr<IScene> nextScene);
    std::unique_ptr<IScene> GetNextScene() override;

    // returns room data for torch placement etc
    const std::vector<RoomData>& GetRooms() const { return m_Data.Rooms; }

private:
    DynamicMesh               m_WallMesh;
    DynamicMesh               m_FloorMesh;
    std::shared_ptr<Material> m_WallMaterial;
    std::shared_ptr<Material> m_FloorMaterial;
    std::unique_ptr<IScene>   m_NextScene;
    DungeonData               m_Data;
};