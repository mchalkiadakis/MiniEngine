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

class DungeonScene : public IScene {
public:
    DungeonScene(const DungeonData& data,
        std::shared_ptr<Material> wallMaterial,
        std::shared_ptr<Material> floorMaterial);

    void Update(float deltaTime, const Camera& camera) override;
    void Render(const RenderContext& ctx) const override;

private:
    std::vector<DungeonRoom>  m_Rooms;
    std::vector<DynamicMesh>  m_CorridorMeshes;
    std::shared_ptr<Material> m_WallMaterial;
    std::shared_ptr<Material> m_FloorMaterial;
    DungeonData               m_Data;
};