#include "SceneManager.h"
#include <iostream>
#include "World/Entity.h"

void SceneManager::LoadScene(std::unique_ptr<Scene> scene) {
    if (m_CurrentScene) {
        // cleanup current scene before swapping
        m_CurrentScene.reset();
    }
    m_CurrentScene = std::move(scene);
    std::cout << "Scene loaded\n";
}

void SceneManager::Update(float deltaTime, const Camera& camera) {
    if (m_CurrentScene)
        m_CurrentScene->Update(deltaTime, camera);
}

void SceneManager::Render(const RenderContext& ctx) {
    if (m_CurrentScene)
        m_CurrentScene->Render(ctx);
}