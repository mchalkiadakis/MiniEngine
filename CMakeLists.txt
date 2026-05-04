cmake_minimum_required(VERSION 3.16)
project(MiniEngine)

set(CMAKE_CXX_STANDARD 17)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
add_subdirectory(external/assimp)

include_directories(
    external/glad/include
    external/glfw/include
    external/glm
    external/stb
    external/assimp/include
    src
    src/Core
    src/Rendering
    src/Gameplay
    src/World
    src/Engine
    src/Camera
    src/Effects
)

file(GLOB_RECURSE SOURCES
    src/Core/*.cpp
    src/Rendering/*.cpp
    src/Gameplay/*.cpp
    src/World/*.cpp
    src/Engine/*.cpp
    src/Camera/*.cpp
    src/Effects/*.cpp
    src/*.cpp
    external/glad/src/glad.c
)

add_executable(MiniEngine ${SOURCES})

add_custom_target(CopyAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/Assets
    ${CMAKE_CURRENT_BINARY_DIR}/Assets
    COMMENT "Copying assets to build directory"
)
add_dependencies(MiniEngine CopyAssets)

target_link_libraries(MiniEngine glfw opengl32 assimp)