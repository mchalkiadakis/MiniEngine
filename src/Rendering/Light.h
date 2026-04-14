#pragma once

#include <glm.hpp>
#include "Camera/Camera.h"  

struct DirectionalLight {
    glm::vec3 Direction = glm::normalize(glm::vec3(2.5f, 1.0f, 0.3f));
    glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);
};

struct RenderContext {
    const Camera& camera;
    const DirectionalLight& light;
};