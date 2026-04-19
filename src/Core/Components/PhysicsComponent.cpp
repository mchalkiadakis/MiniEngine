/*Note — PhysicsComponent::Start requires TransformComponent to already be added before PhysicsComponent.The developer must add them in order :
cppentity.AddComponent<TransformComponent>(...); // first
entity.AddComponent<PhysicsComponent>();      // second — hooks into transform */


#include "PhysicsComponent.h"
#include "World/Entity.h"
#include <algorithm>

void PhysicsComponent::Start(Entity& owner) {
    m_Owner = &owner;
    m_Transform = owner.GetComponent<TransformComponent>();
}

void PhysicsComponent::Update(float deltaTime) {
    if (!m_Transform) return;

    // apply gravity
    if (m_UseGravity && !m_IsGrounded)
        m_Velocity.y += m_Gravity * deltaTime;

    // apply movement intent — horizontal only
    m_Velocity.x = m_MovementIntent.x * m_MoveSpeed;
    m_Velocity.z = m_MovementIntent.z * m_MoveSpeed;

    // integrate position
    glm::vec3 pos = m_Transform->GetPosition();
    pos += m_Velocity * deltaTime;

    // simple floor check — don't go below floor level
    if (pos.y <= m_FloorY) {
        pos.y = m_FloorY;
        m_Velocity.y = 0.0f;
        m_IsGrounded = true;
    }
    else {
        m_IsGrounded = false;
    }

    // reset movement intent — must be set every frame by input/AI
    m_MovementIntent = glm::vec3(0.0f);

    m_Transform->SetPosition(pos);
}