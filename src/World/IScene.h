#pragma once
#include "Rendering/RenderContext.h"
#include "Rendering/PointLight.h"
#include "Camera/Camera.h"
#include <vector>
#include <string>
#include <memory>

class Entity;

class IScene {
public:
    virtual ~IScene() = default;
    virtual void Update(float deltaTime, const Camera& camera) = 0;
    virtual void Render(const RenderContext& ctx) const = 0;

    // lighting
    virtual void AddPointLight(const PointLight& light) = 0;
    virtual const std::vector<PointLight>& GetPointLights() const = 0;

    // entities
    virtual Entity& CreateEntity(const std::string& name) = 0;
    virtual Entity* GetEntity(const std::string& name) = 0;

    // scene transitions
    virtual std::unique_ptr<IScene> GetNextScene() { return nullptr; }
};