#pragma once
#include "DungeonRoom.h"
#include "RoomData.h"
#include "World/IScene.h"
#include "Rendering/RenderContext.h"
#include "Rendering/Material.h"
#include "Rendering/DynamicMesh.h"
#include "Camera/Camera.h"
#include <vector>
#include <memory>
#include <glm.hpp>

class DungeonScene : public IScene {
public:
    DungeonScene(const DungeonData& data,
        std::shared_ptr<Material> wallMaterial,
        std::shared_ptr<Material> floorMaterial);

    void Update(float deltaTime, const Camera& camera) override;
    void Render(const RenderContext& ctx) const override;

    const std::vector<glm::vec3>& GetTorchPositions() const { return m_Torches; }

private:
    std::vector<DungeonRoom>  m_Rooms;
    std::vector<DynamicMesh>  m_CorridorMeshes;
    std::shared_ptr<Material> m_WallMaterial;
    std::shared_ptr<Material> m_FloorMaterial;
    std::vector<glm::vec3>    m_Torches;
    DungeonData               m_Data;
};