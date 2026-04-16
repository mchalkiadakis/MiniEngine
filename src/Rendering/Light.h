#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include "Camera/Camera.h"
#include "PointLight.h"
#include "Shader.h"

struct DirectionalLight {
    glm::vec3 Direction = glm::normalize(glm::vec3(2.5f, 1.0f, 0.3f));
    glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);
};
