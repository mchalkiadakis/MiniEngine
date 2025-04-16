#pragma once
#include "Shader.h"
class Renderer {
public:
    static void Clear();
    static void Draw(unsigned int vaoID); // Placeholder for future draw logic
    static unsigned int Renderer::CreateMesh(const float* vertices, size_t vertexCount);
    static void Renderer::Draw(unsigned int vaoID, size_t vertexCount, Shader& shader);
};