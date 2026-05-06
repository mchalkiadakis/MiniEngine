#include "Model.h"
#include <glad/glad.h>
#include <gtc/type_ptr.hpp>

void Model::AddSubMesh(std::unique_ptr<Mesh> mesh,
    std::unique_ptr<Material> material) {
    m_SubMeshes.push_back({ std::move(mesh), std::move(material) });
}

void Model::Draw(const RenderContext& ctx,
    const glm::mat4& modelMatrix) const {
    for (const auto& sub : m_SubMeshes) {
        if (!sub.mesh || !sub.material) continue;
        auto shader = sub.material->GetShader();
        sub.material->Bind();
        ctx.ApplyToShader(*shader, modelMatrix);
        sub.mesh->Draw();
    }
}

void Model::DrawGeometry() const {
    for (const auto& sub : m_SubMeshes) {
        if (!sub.mesh) continue;
        sub.mesh->Draw();
    }
}

// --- SkinnedModel ---

void SkinnedModel::AddSubMesh(std::unique_ptr<SkinnedMesh> mesh,
    std::unique_ptr<Material> material) {
    m_SubMeshes.push_back({ std::move(mesh), std::move(material) });
}

void SkinnedModel::Draw(const RenderContext& ctx,
    const glm::mat4& modelMatrix,
    const std::vector<glm::mat4>& boneMatrices) const {
    for (const auto& sub : m_SubMeshes) {
        if (!sub.mesh || !sub.material) continue;

        auto shader = sub.material->GetShader();
        if (!shader) continue;

        sub.material->Bind();
        ctx.ApplyToShader(*shader, modelMatrix);

        int boneCount = (int)std::min(boneMatrices.size(), (size_t)128);
        for (int i = 0; i < boneCount; i++) {
            std::string uniform = "u_BoneMatrices[" + std::to_string(i) + "]";
            shader->SetUniformMat4(uniform, glm::value_ptr(boneMatrices[i]));
        }

        sub.mesh->Draw();
    }
}

void SkinnedModel::DrawGeometry() const {
    for (const auto& sub : m_SubMeshes) {
        if (!sub.mesh) continue;
        sub.mesh->Draw();
    }
}