#pragma once
#include "Core/Component.h"
#include <functional>

class Entity;

class HealthComponent : public Component {
public:
    HealthComponent(float maxHealth = 100.0f)
        : m_Health(maxHealth)
        , m_MaxHealth(maxHealth)
    {
    }

    void Start(Entity& owner) override { m_Owner = &owner; }
    void Update(float deltaTime) override {}

    // health management
    void  SetHealth(float health) { m_Health = glm::clamp(health, 0.0f, m_MaxHealth); }
    void  SetMaxHealth(float max) { m_MaxHealth = max; m_Health = glm::min(m_Health, max); }
    float GetHealth()    const { return m_Health; }
    float GetMaxHealth() const { return m_MaxHealth; }
    float GetHealthPercent() const { return m_Health / m_MaxHealth; }
    bool  IsAlive()      const { return m_Health > 0.0f; }

    void TakeDamage(float amount) {
        if (!IsAlive()) return;
        m_Health = glm::clamp(m_Health - amount, 0.0f, m_MaxHealth);
        if (m_OnDamage) m_OnDamage(amount);
        if (!IsAlive() && m_OnDeath) m_OnDeath();
    }

    void Heal(float amount) {
        m_Health = glm::clamp(m_Health + amount, 0.0f, m_MaxHealth);
    }

    // callbacks — developer sets these
    void SetOnDamage(std::function<void(float)> callback) { m_OnDamage = callback; }
    void SetOnDeath(std::function<void()>       callback) { m_OnDeath = callback; }

private:
    Entity* m_Owner = nullptr;
    float   m_Health = 100.0f;
    float   m_MaxHealth = 100.0f;

    std::function<void(float)> m_OnDamage;
    std::function<void()>      m_OnDeath;
};