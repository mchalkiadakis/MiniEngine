#include "Model.h"

void Model::AddSubMesh(std::unique_ptr<Mesh> mesh, std::unique_ptr<Material> material) {
    m_SubMeshes.push_back({ std::move(mesh), std::move(material) });
}

void Model::Draw(const Camera& camera, const glm::mat4& modelMatrix) const {
    for (const auto& sub : m_SubMeshes) {
        if (!sub.mesh || !sub.material) continue;

        sub.material->Bind();
        auto shader = sub.material->GetShader();
        shader->SetUniformMat4("u_Model", glm::value_ptr(modelMatrix));
        shader->SetUniformMat4("u_View", glm::value_ptr(camera.GetViewMatrix()));
        shader->SetUniformMat4("u_Projection", glm::value_ptr(camera.GetProjectionMatrix()));

        sub.mesh->Draw();
    }
}