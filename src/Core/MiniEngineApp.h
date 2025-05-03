#pragma once

#include "MiniEngine.h"
#include "World/Scene.h"
#include "Camera/Camera.h"
class MiniEngineApp {
public:
    bool Init();
    void Run();

private:
    void PollInput(float deltaTime);
    void Update(float deltaTime);
    void Render();

    GLFWwindow* m_Window = nullptr;

    std::shared_ptr<Camera> m_Camera;
    std::unique_ptr<Scene> m_Scene;
};