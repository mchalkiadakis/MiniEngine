#pragma once
#include "DungeonRoom.h"
#include "RoomData.h"
#include "World/IScene.h"
#include "Rendering/RenderContext.h"
#include "Rendering/Material.h"
#include "Rendering/DynamicMesh.h"
#include "Rendering/PointLight.h"
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
    void AddPointLight(const PointLight& light) override;
    const std::vector<PointLight>& GetPointLights() const override;

private:
    std::vector<DungeonRoom>  m_Rooms;
    DynamicMesh               m_CorridorMesh;
    std::shared_ptr<Material> m_WallMaterial;
    std::shared_ptr<Material> m_FloorMaterial;
    std::vector<PointLight>   m_PointLights;
    DungeonData               m_Data;
};