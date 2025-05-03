#pragma once

#include <memory>
#include <vector>
#include <string>

// Forward declarations
class Entity;
class Camera;

class Scene {
public:
    Entity& CreateEntity(const std::string& name);
    void Update(float deltaTime);
    void Render(const Camera& camera);

private:
    std::vector<std::unique_ptr<Entity>> m_Entities;
};
