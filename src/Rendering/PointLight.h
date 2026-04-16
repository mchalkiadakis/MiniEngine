#pragma once
#include <glm.hpp>

struct PointLight {
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Color = glm::vec3(1.0f, 0.8f, 0.4f);
    float     Intensity = 1.0f;
    float     Radius = 10.0f;
};