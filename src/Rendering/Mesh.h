#pragma once

#include <vector>
#include <glm.hpp>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    void Draw() const;

    // utility — computes tangents for a triangle soup and writes into vertices
    static void ComputeTangents(std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices);

private:
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;
    size_t m_IndexCount = 0;
};