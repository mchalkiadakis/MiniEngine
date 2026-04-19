#pragma once
#include "Rendering/RenderContext.h"
#include "Rendering/PointLight.h"
#include "Camera/Camera.h"
#include <vector>

class IScene {
public:
    virtual ~IScene() = default;
    virtual void Update(float deltaTime, const Camera& camera) = 0;
    virtual void Render(const RenderContext& ctx) const = 0;
    virtual void AddPointLight(const PointLight& light) = 0;
    virtual const std::vector<PointLight>& GetPointLights() const = 0;
};