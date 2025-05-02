cmake_minimum_required(VERSION 3.16)
project(MiniEngine)

set(CMAKE_CXX_STANDARD 17)

# Add GLFW (must be CMake-compatible version)
add_subdirectory(external/glfw)

# Include paths
include_directories(
    external/glad/include
    external/glfw/include
    external/glm           # For glm/glm.hpp (your structure)
    src
    src/Core
    src/Rendering   
    src/Gameplay
    src/World
    src/Engine
)

# Copy Assets/ to build directory (shaders, textures, etc.)
file(COPY Assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Gather source files
file(GLOB_RECURSE SOURCES
    src/Core/*.cpp
    src/Rendering/*.cpp
    src/Gameplay/*.cpp
    src/World/*.cpp
    src/Engine/*.cpp
    src/*.cpp
    external/glad/src/glad.c
)
include_directories(external/stb)
# Create executable
add_executable(MiniEngine ${SOURCES} "src/Rendering/Mesh.h" "src/Rendering/Mesh.cpp" "src/Rendering/Texture.cpp" "src/Rendering/Texture.h" "src/Rendering/Material.h" "src/Rendering/Material.cpp" "src/Camera/Camera.cpp" "src/Camera/Camera.h")

# Link libraries
target_link_libraries(MiniEngine glfw opengl32)
