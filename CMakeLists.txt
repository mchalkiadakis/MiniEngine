cmake_minimum_required(VERSION 3.16)
project(MiniEngine)

set(CMAKE_CXX_STANDARD 17)

# Add GLFW (must be CMake-compatible version)
add_subdirectory(external/glfw)

# Include paths
include_directories(
    external/glad/include
    external/glfw/include
    external/glm           # For glm/glm.hpp
    external/stb           # For stb_image.h if used
    src
    src/Core
    src/Rendering   
    src/Gameplay
    src/World
    src/Engine
    src/Camera
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
    src/Camera/*.cpp
    src/*.cpp
    external/glad/src/glad.c
)

# Create executable
add_executable(MiniEngine ${SOURCES})

# Link libraries
target_link_libraries(MiniEngine glfw opengl32)
