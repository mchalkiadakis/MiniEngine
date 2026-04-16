#pragma once
#include <vector>
#include "Mesh.h" // for Vertex struct

class DynamicMesh {
public:
    DynamicMesh();
    ~DynamicMesh();

    void Upload(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void Draw() const;

private:
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;
    size_t m_IndexCount = 0;
};