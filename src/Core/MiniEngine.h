
#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/string_cast.hpp>

#include "World/IScene.h"
#include "World/Dungeon/DungeonScene.h"
#include "World/Dungeon/DungeonMeshBuilder.h"
#include "World/Dungeon/DungeonRoom.h"
#include "World/Dungeon/RoomData.h"
#include "World/Dungeon/DungeonGenerator.h"
#include "Camera/Camera.h"
#include "Rendering/Light.h"
#include "Rendering/RenderContext.h"
#include "Rendering/PointLight.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "Model.h"
#include "ModelLoader.h"
#include "World/Scene.h"
#include "World/Entity.h"
#include "World/Terrain.h"
#include "World/TerrainGenerator.h"
#include "World/ChunkManager.h"
#include "World/TerrainChunk.h"
#include "World/Skybox.h"
#include "Core/AssetManager.h"
#include "Core/MiniEngineApp.h"
#include "Core/Component.h"
#include "Core/Components/TransformComponent.h"
#include "Core/Components/PhysicsComponent.h"
#include "Effects/FogSettings.h"
#include "World/Dungeon/DungeonGrid.h"
#include "World/Dungeon/AStar.h"
