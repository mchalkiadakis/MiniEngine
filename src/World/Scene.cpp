
#include "Scene.h"
#include "Entity.h"

Entity& Scene::CreateEntity(const std::string& name) {
    m_Entities.push_back(std::make_unique<Entity>(name));
    return *m_Entities.back();
}

void Scene::Update(float deltaTime) {
    for (auto& entity : m_Entities) {
        entity->Update(deltaTime);
    }
}

void Scene::Render(const Camera& camera) {
    for (auto& entity : m_Entities) {
        entity->Render(camera);
    }
}