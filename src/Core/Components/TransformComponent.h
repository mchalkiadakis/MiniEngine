#pragma once
#include "Core/Component.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

class TransformComponent : public Component {
public:
    TransformComponent() = default;
    TransformComponent(const glm::vec3& position,
        const glm::vec3& rotation = glm::vec3(0.0f),
        const glm::vec3& scale = glm::vec3(1.0f))
        : m_Position(position)
        , m_Rotation(rotation)
        , m_Scale(scale)
    {
    }

    void Update(float deltaTime) override {}

    // position
    void            SetPosition(const glm::vec3& pos) { m_Position = pos; m_Dirty = true; }
    void            Translate(const glm::vec3& delta) { m_Position += delta; m_Dirty = true; }
    const glm::vec3& GetPosition() const { return m_Position; }

    // rotation (euler angles in degrees)
    void            SetRotation(const glm::vec3& rot) { m_Rotation = rot; m_Dirty = true; }
    void            Rotate(const glm::vec3& delta) { m_Rotation += delta; m_Dirty = true; }
    const glm::vec3& GetRotation() const { return m_Rotation; }

    // scale
    void            SetScale(const glm::vec3& scale) { m_Scale = scale; m_Dirty = true; }
    const glm::vec3& GetScale() const { return m_Scale; }

    // matrix
    const glm::mat4& GetMatrix() {
        if (m_Dirty) Recalculate();
        return m_Matrix;
    }

    // forward/right/up vectors
    glm::vec3 Forward() {
        if (m_Dirty) Recalculate();
        return glm::normalize(glm::vec3(m_Matrix[2]));
    }
    glm::vec3 Right() {
        if (m_Dirty) Recalculate();
        return glm::normalize(glm::vec3(m_Matrix[0]));
    }
    glm::vec3 Up() {
        if (m_Dirty) Recalculate();
        return glm::normalize(glm::vec3(m_Matrix[1]));
    }

private:
    void Recalculate() {
        glm::mat4 t = glm::translate(glm::mat4(1.0f), m_Position);
        glm::mat4 rx = glm::rotate(glm::mat4(1.0f),
            glm::radians(m_Rotation.x),
            glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 ry = glm::rotate(glm::mat4(1.0f),
            glm::radians(m_Rotation.y),
            glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rz = glm::rotate(glm::mat4(1.0f),
            glm::radians(m_Rotation.z),
            glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 s = glm::scale(glm::mat4(1.0f), m_Scale);
        m_Matrix = t * ry * rx * rz * s;
        m_Dirty = false;
    }

    glm::vec3 m_Position = glm::vec3(0.0f);
    glm::vec3 m_Rotation = glm::vec3(0.0f);
    glm::vec3 m_Scale = glm::vec3(1.0f);
    glm::mat4 m_Matrix = glm::mat4(1.0f);
    bool      m_Dirty = true;
};