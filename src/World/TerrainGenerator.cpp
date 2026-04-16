#include "TerrainGenerator.h"
#include <cmath>
#include <algorithm>

// ---- Value noise implementation ----

float TerrainGenerator::Noise(int x, int z) {
    int n = x + z * 57;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float TerrainGenerator::SmoothNoise(int x, int z) {
    float corners = (Noise(x - 1, z - 1) + Noise(x + 1, z - 1) + Noise(x - 1, z + 1) + Noise(x + 1, z + 1)) / 16.0f;
    float sides = (Noise(x - 1, z) + Noise(x + 1, z) + Noise(x, z - 1) + Noise(x, z + 1)) / 8.0f;
    float center = Noise(x, z) / 4.0f;
    return corners + sides + center;
}

float TerrainGenerator::Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float TerrainGenerator::Fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float TerrainGenerator::InterpolatedNoise(float x, float z) {
    int ix = (int)x;
    int iz = (int)z;
    float fx = x - ix;
    float fz = z - iz;

    float v1 = SmoothNoise(ix, iz);
    float v2 = SmoothNoise(ix + 1, iz);
    float v3 = SmoothNoise(ix, iz + 1);
    float v4 = SmoothNoise(ix + 1, iz + 1);

    float i1 = Lerp(v1, v2, Fade(fx));
    float i2 = Lerp(v3, v4, Fade(fx));
    return Lerp(i1, i2, Fade(fz));
}

float TerrainGenerator::GetHeight(float x, float z, float scale) {
    float height = 0.0f;
    height += InterpolatedNoise(x * scale, z * scale) * 1.0f;
    height += InterpolatedNoise(x * scale * 2.0f, z * scale * 2.0f) * 0.5f;
    height += InterpolatedNoise(x * scale * 4.0f, z * scale * 4.0f) * 0.25f;
    return height;
}

// ---- Shared helpers ----

void TerrainGenerator::CalculateNormals(std::vector<Vertex>& vertices, const TerrainConfig& cfg) {
    for (int z = 0; z < cfg.Depth; z++) {
        for (int x = 0; x < cfg.Width; x++) {
            int xL = std::max(x - 1, 0);
            int xR = std::min(x + 1, cfg.Width - 1);
            int zD = std::max(z - 1, 0);
            int zU = std::min(z + 1, cfg.Depth - 1);

            float hL = vertices[z * cfg.Width + xL].Position.y;
            float hR = vertices[z * cfg.Width + xR].Position.y;
            float hD = vertices[zD * cfg.Width + x].Position.y;
            float hU = vertices[zU * cfg.Width + x].Position.y;

            glm::vec3 normal = glm::normalize(glm::vec3(hL - hR, 2.0f * cfg.TileSize, hD - hU));
            vertices[z * cfg.Width + x].Normal = normal;
        }
    }
}

void TerrainGenerator::BuildIndices(std::vector<unsigned int>& indices, const TerrainConfig& cfg) {
    for (int z = 0; z < cfg.Depth - 1; z++) {
        for (int x = 0; x < cfg.Width - 1; x++) {
            unsigned int tl = z * cfg.Width + x;
            unsigned int tr = z * cfg.Width + x + 1;
            unsigned int bl = (z + 1) * cfg.Width + x;
            unsigned int br = (z + 1) * cfg.Width + x + 1;

            indices.push_back(tl);
            indices.push_back(bl);
            indices.push_back(tr);

            indices.push_back(tr);
            indices.push_back(bl);
            indices.push_back(br);
        }
    }
}

// ---- Value noise Generate ----

void TerrainGenerator::Generate(DynamicMesh& mesh, const TerrainConfig& cfg) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    vertices.reserve(cfg.Width * cfg.Depth);

    for (int z = 0; z < cfg.Depth; z++) {
        for (int x = 0; x < cfg.Width; x++) {
            Vertex v;
            v.Position = { x * cfg.TileSize,
                            GetHeight((float)x, (float)z, cfg.NoiseScale) * cfg.HeightScale,
                            z * cfg.TileSize };
            v.TexCoords = { (float)x / cfg.Width, (float)z / cfg.Depth };
            v.Normal = { 0.0f, 1.0f, 0.0f };
            vertices.push_back(v);
        }
    }

    CalculateNormals(vertices, cfg);
    BuildIndices(indices, cfg);
    mesh.Upload(vertices, indices);
}

// ---- FastNoiseLite Generate ----

void TerrainGenerator::GeneratePerlin(DynamicMesh& mesh, const TerrainConfig& cfg) {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin); // Perlin noise
    noise.SetFrequency(cfg.NoiseScale);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(4);
    noise.SetFractalLacunarity(2.0f);
    noise.SetFractalGain(0.5f);
    noise.SetSeed(cfg.Seed);
    //noise.SetSeed(std::rand());

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    vertices.reserve(cfg.Width * cfg.Depth);

    for (int z = 0; z < cfg.Depth; z++) {
        for (int x = 0; x < cfg.Width; x++) {
            float n = (noise.GetNoise((float)x, (float)z) + 1.0f) * 0.5f;

            Vertex v;
            v.Position = { x * cfg.TileSize,
                            n * cfg.HeightScale,
                            z * cfg.TileSize };
            v.TexCoords = { (float)x / cfg.Width, (float)z / cfg.Depth };
            v.Normal = { 0.0f, 1.0f, 0.0f };
            vertices.push_back(v);
        }
    }

    CalculateNormals(vertices, cfg);
    BuildIndices(indices, cfg);
    mesh.Upload(vertices, indices);
}

void TerrainGenerator::CalculateNormalsWithOffset(std::vector<Vertex>& vertices,
    const TerrainConfig& cfg, const FastNoiseLite& noise, float offsetX, float offsetZ) {

    for (int z = 0; z < cfg.Depth; z++) {
        for (int x = 0; x < cfg.Width; x++) {
            // sample neighboring heights directly from noise
            // so edge vertices use correct cross-chunk neighbors
            auto sampleHeight = [&](int sx, int sz) -> float {
                float wx = offsetX + sx * cfg.TileSize;
                float wz = offsetZ + sz * cfg.TileSize;
                return (noise.GetNoise(wx, wz) + 1.0f) * 0.5f * cfg.HeightScale;
                };

            float hL = sampleHeight(x - 1, z);
            float hR = sampleHeight(x + 1, z);
            float hD = sampleHeight(x, z - 1);
            float hU = sampleHeight(x, z + 1);

            glm::vec3 normal = glm::normalize(glm::vec3(hL - hR, 2.0f * cfg.TileSize, hD - hU));
            vertices[z * cfg.Width + x].Normal = normal;
        }
    }
}
void TerrainGenerator::GeneratePerlin(DynamicMesh& mesh, const TerrainConfig& cfg,
    const FastNoiseLite& noise, float offsetX, float offsetZ) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    vertices.reserve(cfg.Width * cfg.Depth);

    for (int z = 0; z < cfg.Depth; z++) {
        for (int x = 0; x < cfg.Width; x++) {
            float worldX = offsetX + x * cfg.TileSize;
            float worldZ = offsetZ + z * cfg.TileSize;

            // sample noise at world coordinates — continuous across chunks
            float n = (noise.GetNoise(worldX, worldZ) + 1.0f) * 0.5f;

            Vertex v;
            v.Position = { x * cfg.TileSize, n * cfg.HeightScale, z * cfg.TileSize };
            v.TexCoords = { (float)x / cfg.Width, (float)z / cfg.Depth };
            v.Normal = { 0.0f, 1.0f, 0.0f };
            vertices.push_back(v);
        }
    }

    CalculateNormalsWithOffset(vertices, cfg, noise, offsetX, offsetZ);
    BuildIndices(indices, cfg);
    mesh.Upload(vertices, indices);
}