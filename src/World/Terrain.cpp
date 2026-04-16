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

    shader->SetUniformMat4("u_Model", glm::value_ptr(m_ModelMatrix));
    shader->SetUniformMat4("u_View", glm::value_ptr(ctx.camera.GetViewMatrix()));
    shader->SetUniformMat4("u_Projection", glm::value_ptr(ctx.camera.GetProjectionMatrix()));
    shader->SetUniformMat3("u_NormalMatrix", glm::value_ptr(normalMatrix));
    shader->SetUniform3f("u_LightDir", ctx.light.Direction);
    shader->SetUniform3f("u_LightColor", ctx.light.Color);
    shader->SetUniform3f("u_ViewPos", ctx.camera.GetPosition());

    m_Mesh.Draw();
}