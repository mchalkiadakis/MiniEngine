#include "Terrain.h"
#include <gtc/type_ptr.hpp>

Terrain::Terrain(const TerrainConfig& config, std::shared_ptr<Material> material)
    : m_Config(config), m_Material(std::move(material))
{
    TerrainGenerator::GeneratePerlin(m_Mesh, m_Config);
}

void Terrain::Regenerate(const TerrainConfig& config) {
    m_Config = config;
    TerrainGenerator::GeneratePerlin(m_Mesh, m_Config);
}

void Terrain::Render(const RenderContext& ctx) const {
    if (!m_Material) return;

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(m_ModelMatrix)));

    auto shader = m_Material->GetShader();
    m_Material->Bind();
    ctx.ApplyToShader(*shader, m_ModelMatrix);

    m_Mesh.Draw();
}