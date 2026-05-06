#include "Scene.h"
#include "Entity.h"
#include "Core/Components/TransformComponent.h"
#include <algorithm>
#include <gtc/type_ptr.hpp>
#include <iostream>

Entity& Scene::CreateEntity(const std::string& name) {
    m_Entities.push_back(std::make_unique<Entity>(name));
    return *m_Entities.back();
}

void Scene::SetChunkManager(std::unique_ptr<ChunkManager> chunkManager) {
    m_ChunkManager = std::move(chunkManager);
}

void Scene::SetSkybox(std::unique_ptr<Skybox> skybox) {
    m_Skybox = std::move(skybox);
}

void Scene::Update(float deltaTime, Camera& camera) {
    for (auto& entity : m_Entities)
        entity->Update(deltaTime);

    if (m_ChunkManager)
        m_ChunkManager->Update(camera);
}

void Scene::Render(const RenderContext& ctx) const {
    if (m_Skybox)
        m_Skybox->Render(ctx);

    if (m_ChunkManager)
        m_ChunkManager->Render(ctx);

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

void Scene::RenderDepth(Shader& depthShader,
    const glm::mat4& lightSpaceMatrix) const {
    depthShader.Use();
    depthShader.SetUniformMat4("u_LightSpaceMatrix",
        glm::value_ptr(lightSpaceMatrix));

    for (auto& entity : m_Entities) {
        if (!entity->HasMesh()) continue;
        if (entity->IsSkinnedActor()) continue; // check BEFORE drawing

        auto* transform = const_cast<Entity*>(entity.get())
            ->GetComponent<TransformComponent>();

        glm::mat4 model = transform
            ? transform->GetMatrix()
            : entity->GetModelMatrix();

        depthShader.SetUniformMat4("u_Model", glm::value_ptr(model));
        entity->DrawGeometry();

        std::cout << "Entity: " << entity->GetName()
            << " HasMesh=" << entity->HasMesh()
            << " IsSkinned=" << entity->IsSkinnedActor() << "\n";
        if (!entity->HasMesh()) continue;
        if (entity->IsSkinnedActor()) continue;
    }
}

void Scene::AddPointLight(const PointLight& light) {
    m_PointLights.push_back(light);
}

const std::vector<PointLight>& Scene::GetPointLights() const {
    return m_PointLights;
}

Entity* Scene::GetEntity(const std::string& name) {
    for (auto& entity : m_Entities)
        if (entity->GetName() == name)
            return entity.get();
    return nullptr;
}

Actor& Scene::CreateActor(const std::string& name, float maxHealth) {
    m_Entities.push_back(std::make_unique<Actor>(name, maxHealth));
    return static_cast<Actor&>(*m_Entities.back());
}

void Scene::AddActor(std::unique_ptr<Actor> actor) {
    m_Entities.push_back(std::unique_ptr<Entity>(actor.release()));
}