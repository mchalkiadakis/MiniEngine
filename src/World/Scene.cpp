#include "Scene.h"
#include "Entity.h"
#include <algorithm>
#include "MiniEngine.h"

Entity& Scene::CreateEntity(const std::string& name) {
    m_Entities.push_back(std::make_unique<Entity>(name));
    return *m_Entities.back();
}


void Scene::SetSkybox(std::unique_ptr<Skybox> skybox) {
    m_Skybox = std::move(skybox);
}


void Scene::SetChunkManager(std::unique_ptr<ChunkManager> chunkManager) {
    m_ChunkManager = std::move(chunkManager);
}

void Scene::Update(float deltaTime, const Camera& camera) {
    for (auto& entity : m_Entities)
        entity->Update(deltaTime);

    if (m_ChunkManager)
        m_ChunkManager->Update(camera);
}

void Scene::Render(const RenderContext& ctx) {
    if (m_Skybox)
        m_Skybox->Render(ctx);

    if (m_ChunkManager)
        m_ChunkManager->Render(ctx);

    // collect and sort draw calls by shader to minimize rebinds
    struct DrawCall {
        Entity* entity;
        unsigned int shaderID;
    };

    std::vector<DrawCall> drawCalls;
    drawCalls.reserve(m_Entities.size());

    for (auto& entity : m_Entities)
        drawCalls.push_back({ entity.get(), entity->GetShaderID() });

    std::sort(drawCalls.begin(), drawCalls.end(),
        [](const DrawCall& a, const DrawCall& b) {
            return a.shaderID < b.shaderID;
        });

    for (auto& dc : drawCalls)
        dc.entity->Render(ctx);
}