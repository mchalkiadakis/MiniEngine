// MiniEngine.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>       // Always include GLAD before GLFW
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Renderer.h"
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Mesh.h"
#include "stb_image.h"
#include "Texture.h"
#include "Material.h"
#include "Camera/Camera.h"
#include <gtx/string_cast.hpp>
// TODO: Reference additional headers your program requires here.
