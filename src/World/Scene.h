#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Rendering/Light.h"


// Forward declarations
class Entity;
class Camera;

class Scene {
public:
   // void Render(const RenderContext& ctx);
    Entity& CreateEntity(const std::string& name);
    void Update(float deltaTime);
    void Render(const RenderContext& ctx);

private:
    std::vector<std::unique_ptr<Entity>> m_Entities;
};
