#include "Entity.h"
#include "Rendering/Model.h"

Entity::Entity(const std::string& name) : m_Name(name) {}

void Entity::SetMesh(std::unique_ptr<Mesh> mesh) {
    m_Mesh = std::move(mesh);
}

void Entity::SetMaterial(std::unique_ptr<Material> material) {
    m_Material = std::move(material);
}

void Entity::Update(float deltaTime) {
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Position);

    if (m_ShouldRotate) {
        m_RotationAngle += deltaTime;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), m_RotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        m_ModelMatrix = translation * rotation;
    }
    else {
        m_ModelMatrix = translation;
    }
}

void Entity::SetModel(std::unique_ptr<Model> model) {
    m_Model = std::move(model);
}

void Entity::Render(const RenderContext& ctx) {
    if (m_Model) {
        m_Model->Draw(ctx, m_ModelMatrix);
        return;
    }

    if (!m_Material || !m_Mesh) return;

    auto shader = m_Material->GetShader();
    m_Material->Bind();
    shader->SetUniformMat4("u_Model", glm::value_ptr(m_ModelMatrix));
    shader->SetUniformMat4("u_View", glm::value_ptr(ctx.camera.GetViewMatrix()));
    shader->SetUniformMat4("u_Projection", glm::value_ptr(ctx.camera.GetProjectionMatrix()));
    shader->SetUniform3f("u_LightDir", ctx.light.Direction);
    shader->SetUniform3f("u_LightColor", ctx.light.Color);
    shader->SetUniform3f("u_ViewPos", ctx.camera.GetPosition());
    m_Mesh->Draw();
}
void Entity::SetTransform(const glm::mat4& transform) {
    m_ModelMatrix = transform;
    m_ManualTransform = true;
}