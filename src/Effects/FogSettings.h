#pragma once
#include <glm.hpp>

struct FogSettings {
    glm::vec3 Color = glm::vec3(0.15f, 0.15f, 0.15f); // dark grey
    float     Density = 0.003f;
    bool      Enabled = true;
};