#pragma once
#include <vector>
#include "Rendering/DynamicMesh.h"
#include "FastNoiseLite.h"

struct TerrainConfig {
    int   Seed = 42;
    int   Width = 128;
    int   Depth = 128;
    float TileSize = 1.0f;
    float HeightScale = 8.0f;
    float NoiseScale = 0.003f;
};

class TerrainGenerator {
public:
    // Value noise version
    static void Generate(DynamicMesh& mesh, const TerrainConfig& config);

    // FastNoiseLite version
    static void GeneratePerlin(DynamicMesh& mesh, const TerrainConfig& config);
    static void GeneratePerlin(DynamicMesh& mesh, const TerrainConfig& config,const FastNoiseLite& noise, float offsetX = 0.0f, float offsetZ = 0.0f);
    static void CalculateNormalsWithOffset(std::vector<Vertex>& vertices,
        const TerrainConfig& cfg, const FastNoiseLite& noise, float offsetX, float offsetZ);

private: 
    // Value noise helpers
    static float GetHeight(float x, float z, float scale);
    static float Noise(int x, int z);
    static float SmoothNoise(int x, int z);
    static float InterpolatedNoise(float x, float z);
    static float Lerp(float a, float b, float t);
    static float Fade(float t);

    // Shared helper for both versions
    static void CalculateNormals(std::vector<Vertex>& vertices, const TerrainConfig& cfg);
    static void BuildIndices(std::vector<unsigned int>& indices, const TerrainConfig& cfg);
};