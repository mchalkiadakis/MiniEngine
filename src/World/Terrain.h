#pragma once
#include "Rendering/DynamicMesh.h"
#include "Rendering/Material.h"
#include "Rendering/Light.h"
#include "TerrainGenerator.h"
#include <memory>

class Terrain {
public:
    Terrain(const TerrainConfig& config, std::shared_ptr<Material> material);

    void Regenerate(const TerrainConfig& config);
    void Render(const RenderContext& ctx) const;

private:
    DynamicMesh                m_Mesh;
    std::shared_ptr<Material>  m_Material;
    TerrainConfig              m_Config;
    glm::mat4                  m_ModelMatrix = glm::mat4(1.0f);
};