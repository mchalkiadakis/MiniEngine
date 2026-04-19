#pragma once

class Entity;

class Component {
public:
    virtual ~Component() = default;
    virtual void Start(Entity& owner) {}
    virtual void Update(float deltaTime) {}
};