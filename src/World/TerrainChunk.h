#pragma once
#include "Rendering/DynamicMesh.h"
#include "Rendering/Material.h"
#include "Rendering/Light.h"
#include "TerrainGenerator.h"
#include <memory>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Rendering/RenderContext.h"

class TerrainChunk {
public:
    TerrainChunk(int chunkX, int chunkZ, const TerrainConfig& config,
        std::shared_ptr<Material> material, const FastNoiseLite& noise);

    void Render(const RenderContext& ctx) const;

    int GetChunkX() const { return m_ChunkX; }
    int GetChunkZ() const { return m_ChunkZ; }

private:
    int                       m_ChunkX;
    int                       m_ChunkZ;
    DynamicMesh               m_Mesh;
    std::shared_ptr<Material> m_Material;
    glm::mat4                 m_ModelMatrix;
};