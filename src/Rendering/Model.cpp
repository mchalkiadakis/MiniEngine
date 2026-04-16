#include "Model.h"

void Model::AddSubMesh(std::unique_ptr<Mesh> mesh, std::unique_ptr<Material> material) {
    m_SubMeshes.push_back({ std::move(mesh), std::move(material) });
}
//should be optimized
void Model::Draw(const RenderContext& ctx, const glm::mat4& modelMatrix) const {
    for (const auto& sub : m_SubMeshes) {
        if (!sub.mesh || !sub.material) continue;

        auto shader = sub.material->GetShader();
        sub.material->Bind();
        ctx.ApplyToShader(*shader, modelMatrix);
        sub.mesh->Draw();
    }
}