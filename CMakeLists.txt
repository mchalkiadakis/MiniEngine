cmake_minimum_required(VERSION 3.16)
project(MiniEngine)

set(CMAKE_CXX_STANDARD 17)

# GLFW (CMake-compatible)
add_subdirectory(external/glfw)

# Include paths
include_directories(
    external/glad/include
    external/glfw/include
    src
    src/Core
    src/Rendering   
    src/Gameplay
    src/World
    src/Engine
)

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

# Executable
add_executable(MiniEngine ${SOURCES})

# Link libraries
target_link_libraries(MiniEngine glfw opengl32)
