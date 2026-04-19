#pragma once
#include "Core/Component.h"
#include "Core/Components/TransformComponent.h"
#include <glm.hpp>

class Entity;

class PhysicsComponent : public Component {
public:
    PhysicsComponent(bool useGravity = true)
        : m_UseGravity(useGravity) {
    }

    void Start(Entity& owner) override;
    void Update(float deltaTime) override;

    // velocity
    void             SetVelocity(const glm::vec3& vel) { m_Velocity = vel; }
    void             AddVelocity(const glm::vec3& vel) { m_Velocity += vel; }
    const glm::vec3& GetVelocity() const { return m_Velocity; }

    // movement intent — set by player input or AI each frame
    void SetMovementIntent(const glm::vec3& intent) { m_MovementIntent = intent; }

    // state
    bool IsGrounded() const { return m_IsGrounded; }
    void SetGrounded(bool grounded) { m_IsGrounded = grounded; }

    // config
    void SetGravity(float gravity) { m_Gravity = gravity; }
    void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }
    void SetUseGravity(bool use) { m_UseGravity = use; }

private:
    Entity* m_Owner = nullptr;
    TransformComponent* m_Transform = nullptr;

    glm::vec3         m_Velocity = glm::vec3(0.0f);
    glm::vec3         m_MovementIntent = glm::vec3(0.0f);

    float             m_Gravity = -20.0f;
    float             m_MoveSpeed = 10.0f;
    bool              m_UseGravity = true;
    bool              m_IsGrounded = false;

    float             m_FloorY = 0.0f; // ground level
};