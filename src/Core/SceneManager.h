#pragma once
#include <memory>
#include "World/Scene.h"
#include "Rendering/RenderContext.h"
#include "Camera/Camera.h"

class SceneManager {
public:
    void LoadScene(std::unique_ptr<Scene> scene);
    void Update(float deltaTime, const Camera& camera);
    void Render(const RenderContext& ctx);

    Scene* GetCurrentScene() const { return m_CurrentScene.get(); }
    bool   HasScene() const { return m_CurrentScene != nullptr; }

private:
    std::unique_ptr<Scene> m_CurrentScene;
};