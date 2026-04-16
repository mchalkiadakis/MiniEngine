#pragma once
#include "Rendering/RenderContext.h"
#include "Camera/Camera.h"

class IScene {
public:
    virtual ~IScene() = default;
    virtual void Update(float deltaTime, const Camera& camera) = 0;
    virtual void Render(const RenderContext& ctx) const = 0;
};