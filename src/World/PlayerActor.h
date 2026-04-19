#pragma once
#include "World/Actor.h"
#include "Camera/Camera.h"

class PlayerActor : public Actor {
public:
    PlayerActor(const std::string& name, float maxHealth = 100.0f)
        : Actor(name, maxHealth)
    {
        // player has lower gravity feel
        GetPhysics()->SetGravity(-20.0f);
        GetPhysics()->SetMoveSpeed(100.0f);
    }

    void Update(float deltaTime, const Camera& camera);

    void ProcessInput(bool forward, bool backward,
        bool left, bool right,
        const Camera& camera);

    bool IsAttacking()     const { return m_IsAttacking; }
    void TriggerAttack() { m_IsAttacking = true; m_AttackTimer = m_AttackDuration; }

private:
    bool  m_IsAttacking = false;
    float m_AttackTimer = 0.0f;
    float m_AttackDuration = 0.3f;
};