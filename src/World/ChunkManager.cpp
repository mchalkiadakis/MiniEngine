#include "ChunkManager.h"
#include <cmath>

ChunkManager::ChunkManager(const TerrainConfig& config, std::shared_ptr<Material> material)
    : m_Config(config), m_Material(std::move(material))
{
    m_Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_Noise.SetFrequency(0.003f);       // very large features
    m_Noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_Noise.SetFractalOctaves(6);       // more detail layers
    m_Noise.SetFractalLacunarity(2.0f); // standard
    m_Noise.SetFractalGain(0.4f);       // lower = smoother, rounder shapes
    m_Noise.SetSeed(m_Config.Seed);
}

std::string ChunkManager::MakeKey(int x, int z) const {
    return std::to_string(x) + "," + std::to_string(z);
}

void ChunkManager::LoadChunk(int chunkX, int chunkZ) {
    std::string key = MakeKey(chunkX, chunkZ);
    if (m_Chunks.find(key) != m_Chunks.end()) return; // already loaded

    m_Chunks[key] = std::make_unique<TerrainChunk>(
        chunkX, chunkZ, m_Config, m_Material, m_Noise
    );
}

void ChunkManager::UnloadChunks(int centerX, int centerZ) {
    auto it = m_Chunks.begin();
    while (it != m_Chunks.end()) {
        int cx = it->second->GetChunkX();
        int cz = it->second->GetChunkZ();

        if (std::abs(cx - centerX) > m_ViewDistance ||
            std::abs(cz - centerZ) > m_ViewDistance) {
            it = m_Chunks.erase(it);
        }
        else {
            ++it;
        }
    }
}

void ChunkManager::Update(const Camera& camera) {
    glm::vec3 pos = camera.GetPosition();

    float chunkWorldSize = (m_Config.Width - 1) * m_Config.TileSize;
    int centerX = static_cast<int>(std::floor(pos.x / chunkWorldSize));
    int centerZ = static_cast<int>(std::floor(pos.z / chunkWorldSize));

    // load chunks in view distance
    for (int z = centerZ - m_ViewDistance; z <= centerZ + m_ViewDistance; z++) {
        for (int x = centerX - m_ViewDistance; x <= centerX + m_ViewDistance; x++) {
            LoadChunk(x, z);
        }
    }

    UnloadChunks(centerX, centerZ);
}

void ChunkManager::Render(const RenderContext& ctx) const {
    for (const auto& [key, chunk] : m_Chunks) {
        chunk->Render(ctx);
    }
}