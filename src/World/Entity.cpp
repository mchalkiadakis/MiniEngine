#include "Entity.h"
#include "Rendering/Model.h"
#include "Core/Components/TransformComponent.h"

Entity::Entity(const std::string& name) : m_Name(name) {}

void Entity::SetMesh(std::unique_ptr<Mesh> mesh) {
    m_Mesh = std::move(mesh);
}

void Entity::SetMaterial(std::unique_ptr<Material> material) {
    m_Material = std::move(material);
}

void Entity::Update(float deltaTime) {
    for (auto& [type, component] : m_Components)
        component->Update(deltaTime);

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Position);
    if (m_ShouldRotate) {
        m_RotationAngle += deltaTime;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f),
            m_RotationAngle,
            glm::vec3(0.0f, 1.0f, 0.0f));
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
    glm::mat4 matrix = m_ModelMatrix;

    auto* transform = GetComponent<TransformComponent>();
    if (transform)
        matrix = transform->GetMatrix();

    if (m_Model) {
        m_Model->Draw(ctx, matrix);
        return;
    }

    if (!m_Material || !m_Mesh) return;
    auto shader = m_Material->GetShader();
    m_Material->Bind();
    ctx.ApplyToShader(*shader, matrix);
    m_Mesh->Draw();
}

void Entity::SetTransform(const glm::mat4& transform) {
    m_ModelMatrix = transform;
    m_ManualTransform = true;
}

void Entity::DrawGeometry() const {
    glm::mat4 matrix = m_ModelMatrix;

    auto* transform = const_cast<Entity*>(this)->GetComponent<TransformComponent>();
    if (transform)
        matrix = transform->GetMatrix();

    // Model path — draw each mesh's VAO directly
    if (m_Model) {
        m_Model->DrawGeometry();
        return;
    }

    // Mesh path
    if (!m_Mesh) return;
    m_Mesh->Draw();
}