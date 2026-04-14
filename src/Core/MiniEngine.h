#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/string_cast.hpp>

#include "Camera/Camera.h"
#include "Rendering/Light.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "Model.h"
#include "ModelLoader.h"
#include "World/Scene.h"
#include "World/Entity.h"
#include "Core/MiniEngineApp.h"