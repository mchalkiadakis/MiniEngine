#include "Entity.h"

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

void Entity::Render(const Camera& camera) {
    if (!m_Material || !m_Mesh)
        return;

    m_Material->Bind();
    m_Material->GetShader()->Use();
    m_Material->GetShader()->SetUniformMat4("u_Model", glm::value_ptr(m_ModelMatrix));
    m_Material->GetShader()->SetUniformMat4("u_View", glm::value_ptr(camera.GetViewMatrix()));
    m_Material->GetShader()->SetUniformMat4("u_Projection", glm::value_ptr(camera.GetProjectionMatrix()));

    m_Mesh->Draw();
}
void Entity::SetTransform(const glm::mat4& transform) {
    m_ModelMatrix = transform;
    m_ManualTransform = true;
}