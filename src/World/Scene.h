#pragma once

#include <memory>
#include <vector>
#include <string>
#include "ChunkManager.h"
#include "Skybox.h"
#include "Rendering/Light.h"
#include "Rendering/RenderContext.h"

class Entity;

class Scene {
public:
    Entity& CreateEntity(const std::string& name);
    void SetChunkManager(std::unique_ptr<ChunkManager> chunkManager);
    void SetSkybox(std::unique_ptr<Skybox> skybox);
    void Update(float deltaTime, const Camera& camera);
    void Render(const RenderContext& ctx);

private:
    std::vector<std::unique_ptr<Entity>> m_Entities;
    std::unique_ptr<ChunkManager>        m_ChunkManager;
    std::unique_ptr<Skybox>              m_Skybox;
};