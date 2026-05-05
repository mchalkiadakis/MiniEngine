#include "SceneManager.h"
#include "World/Entity.h"
#include "World/Scene.h"
#include <iostream>

void SceneManager::LoadScene(std::unique_ptr<IScene> scene) {
    if (m_CurrentScene)
        m_CurrentScene.reset();
    m_CurrentScene = std::move(scene);
    std::cout << "Scene loaded\n";
}

void SceneManager::Update(float deltaTime, Camera& camera) {
    if (m_CurrentScene)
        m_CurrentScene->Update(deltaTime, camera);
}

void SceneManager::Render(const RenderContext& ctx) const {
    if (m_CurrentScene)
        m_CurrentScene->Render(ctx);
}