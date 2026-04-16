#include "TerrainChunk.h"

TerrainChunk::TerrainChunk(int chunkX, int chunkZ, const TerrainConfig& config,
    std::shared_ptr<Material> material, const FastNoiseLite& noise)
    : m_ChunkX(chunkX), m_ChunkZ(chunkZ), m_Material(std::move(material))
{
    float worldX = chunkX * (config.Width - 1) * config.TileSize;
    float worldZ = chunkZ * (config.Depth - 1) * config.TileSize;

    m_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(worldX, 0.0f, worldZ));

    TerrainGenerator::GeneratePerlin(m_Mesh, config, noise, worldX, worldZ);
}

void TerrainChunk::Render(const RenderContext& ctx) const {
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