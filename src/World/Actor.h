#pragma once
#include "World/Entity.h"
#include "Core/Components/TransformComponent.h"
#include "Core/Components/PhysicsComponent.h"
#include "Core/Components/HealthComponent.h"

class Actor : public Entity {
public:
    Actor(const std::string& name, float maxHealth = 100.0f)
        : Entity(name)
    {
        AddComponent<TransformComponent>();
        AddComponent<PhysicsComponent>();
        AddComponent<HealthComponent>(maxHealth);
    }

    // convenience accessors
    TransformComponent* GetTransform() { return GetComponent<TransformComponent>(); }
    PhysicsComponent* GetPhysics() { return GetComponent<PhysicsComponent>(); }
    HealthComponent* GetHealth() { return GetComponent<HealthComponent>(); }
};