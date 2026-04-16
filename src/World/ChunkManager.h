#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "TerrainChunk.h"
#include "Rendering/Light.h"
#include "Rendering/Material.h"
#include "Camera/Camera.h"
#include "FastNoiseLite.h"

class ChunkManager {
public:
    ChunkManager(const TerrainConfig& config, std::shared_ptr<Material> material);

    void Update(const Camera& camera);
    void Render(const RenderContext& ctx) const;

    void SetViewDistance(int chunks) { m_ViewDistance = chunks; }

private:
    std::string MakeKey(int x, int z) const;
    void LoadChunk(int chunkX, int chunkZ);
    void UnloadChunks(int centerX, int centerZ);

    TerrainConfig                                          m_Config;
    std::shared_ptr<Material>                              m_Material;
    FastNoiseLite                                          m_Noise;
    std::unordered_map<std::string, std::unique_ptr<TerrainChunk>> m_Chunks;
    int                                                    m_ViewDistance = 4;
};