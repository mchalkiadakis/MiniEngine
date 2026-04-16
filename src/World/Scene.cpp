#include "Scene.h"
#include "Entity.h"
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

    for (auto& entity : m_Entities)
        entity->Render(ctx);
}