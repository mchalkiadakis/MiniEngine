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
add_subdirectory(external/assimp)   # ← was missing

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
)

file(COPY Assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE SOURCES
    src/Core/*.cpp
    src/Gameplay/*.cpp
    src/World/*.cpp
    src/Engine/*.cpp
    src/Camera/*.cpp
    src/*.cpp
    external/glad/src/glad.c
)

add_executable(MiniEngine ${SOURCES} "src/World/Scene.h" "src/World/Entity.cpp" "src/World/Entity.h" "src/World/Scene.cpp" "src/Rendering/Model.cpp" "src/Rendering/Model.h" "src/Rendering/ModelLoader.cpp" "src/Rendering/ModelLoader.h" "src/Rendering/Light.h" "src/Rendering/DynamicMesh.cpp" "src/Rendering/DynamicMesh.h" "src/World/TerrainGenerator.cpp" "src/World/TerrainGenerator.h" "src/World/Terrain.cpp" "src/Rendering/CubemapTexture.h" "src/Rendering/CubemapTexture.cpp" "src/World/Skybox.h" "src/World/Skybox.cpp" "src/World/TerrainChunk.h" "src/World/ChunkManager.h" "src/Core/AssetManager.h" "src/Rendering/PointLight.h" "src/Rendering/RenderContext.h" "src/World/Dungeon/RoomData.h" "src/World/Dungeon/DungeonGenerator.h" "src/World/Dungeon/DungeonGenerator.cpp" "src/Core/SceneManager.h" "src/Core/SceneManager.cpp" "src/World/Dungeon/DungeonRoom.h" "src/World/Dungeon/DungeonRoom.cpp" "src/World/Dungeon/DungeonMeshBuilder.h" "src/World/Dungeon/DungeonMeshBuilder.cpp" "src/World/IScene.h" "src/World/Dungeon/DungeonScene.h" "src/World/Dungeon/DungeonScene.cpp" "src/World/Dungeon/DungeonGrid.h" "src/World/Dungeon/DungeonGrid.cpp" "src/World/Dungeon/AStar.h" "src/World/Dungeon/AStar.cpp")

target_link_libraries(MiniEngine glfw opengl32 assimp)