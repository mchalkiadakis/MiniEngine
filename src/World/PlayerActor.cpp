#include "PlayerActor.h"

void PlayerActor::Update(float deltaTime, const Camera& camera) {
    // update attack timer
    if (m_IsAttacking) {
        m_AttackTimer -= deltaTime;
        if (m_AttackTimer <= 0.0f)
            m_IsAttacking = false;
    }

    // sync transform position with camera
    auto* transform = GetTransform();
    auto* physics = GetPhysics();
    if (!transform || !physics) return;

    // camera follows player
    // player Y is physics controlled, X/Z follow camera
}

void PlayerActor::ProcessInput(bool forward, bool backward,
    bool left, bool right,
    const Camera& camera) {
    auto* physics = GetPhysics();
    if (!physics) return;

    // get camera horizontal directions
    glm::vec3 camForward = camera.GetFront();
    camForward.y = 0.0f;
    if (glm::length(camForward) > 0.0f)
        camForward = glm::normalize(camForward);

    glm::vec3 camRight = camera.GetRight();
    camRight.y = 0.0f;
    if (glm::length(camRight) > 0.0f)
        camRight = glm::normalize(camRight);

    glm::vec3 intent = glm::vec3(0.0f);
    if (forward)  intent += camForward;
    if (backward) intent -= camForward;
    if (right)    intent += camRight;
    if (left)     intent -= camRight;

    if (glm::length(intent) > 0.0f)
        intent = glm::normalize(intent);

    physics->SetMovementIntent(intent);
}