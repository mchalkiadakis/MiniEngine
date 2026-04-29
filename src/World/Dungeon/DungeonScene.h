#pragma once
#include "DungeonRoom.h"
#include "RoomData.h"
#include "World/Scene.h"
#include "Rendering/RenderContext.h"
#include "Rendering/Material.h"
#include "Rendering/DynamicMesh.h"
#include "Camera/Camera.h"
#include <vector>
#include <memory>
#include <string>
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

private:
    std::vector<DungeonRoom>  m_Rooms;
    DynamicMesh               m_CorridorMesh;
    std::shared_ptr<Material> m_WallMaterial;
    std::shared_ptr<Material> m_FloorMaterial;
    std::unique_ptr<IScene>   m_NextScene;
    DungeonData               m_Data;
};