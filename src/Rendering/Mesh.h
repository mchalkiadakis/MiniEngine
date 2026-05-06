#pragma once

#include <vector>
#include <glm.hpp>

#define MAX_BONES 128
#define MAX_BONE_INFLUENCE 4

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
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void Draw() const;
    static Mesh CreateFullscreenQuad();
    // utility — computes tangents for a triangle soup and writes into vertices
    static void ComputeTangents(std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices);

private:
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;
    size_t m_IndexCount = 0;
};


struct SkinnedVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    int       BoneIds[MAX_BONE_INFLUENCE] = { 0, 0, 0, 0 };
    float     BoneWeights[MAX_BONE_INFLUENCE] = { 0.0f, 0.0f, 0.0f, 0.0f };

    void AddBoneData(int boneId, float weight) {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            if (BoneWeights[i] == 0.0f) {
                BoneIds[i] = boneId;
                BoneWeights[i] = weight;
                return;
            }
        }
    }
};

class SkinnedMesh {
public:
    SkinnedMesh(const std::vector<SkinnedVertex>& vertices,
        const std::vector<unsigned int>& indices);
    ~SkinnedMesh();
    SkinnedMesh(const SkinnedMesh&) = delete;
    SkinnedMesh& operator=(const SkinnedMesh&) = delete;
    SkinnedMesh(SkinnedMesh&& other) noexcept;
    SkinnedMesh& operator=(SkinnedMesh&& other) noexcept;

    void Draw() const;

private:
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;
    size_t       m_IndexCount = 0;
};