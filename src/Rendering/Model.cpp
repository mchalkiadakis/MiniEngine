#include "Model.h"

void Model::AddSubMesh(std::unique_ptr<Mesh> mesh, std::unique_ptr<Material> material) {
    m_SubMeshes.push_back({ std::move(mesh), std::move(material) });
}
//should be optimized
void Model::Draw(const RenderContext& ctx, const glm::mat4& modelMatrix) const {
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

    for (const auto& sub : m_SubMeshes) {
        if (!sub.mesh || !sub.material) continue;

        auto shader = sub.material->GetShader();
        sub.material->Bind();

        shader->SetUniformMat4("u_Model", glm::value_ptr(modelMatrix));
        shader->SetUniformMat4("u_View", glm::value_ptr(ctx.camera.GetViewMatrix()));
        shader->SetUniformMat4("u_Projection", glm::value_ptr(ctx.camera.GetProjectionMatrix()));
        shader->SetUniformMat3("u_NormalMatrix", glm::value_ptr(normalMatrix));
        shader->SetUniform3f("u_LightDir", ctx.light.Direction);
        shader->SetUniform3f("u_LightColor", ctx.light.Color);
        shader->SetUniform3f("u_ViewPos", ctx.camera.GetPosition());

        sub.mesh->Draw();
    }
}