#pragma once
#include "MiniEngine.h"
#include "Core/SceneManager.h"

class MiniEngineApp {
public:
    bool Init();
    void Run();

    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);

private:
    void PollInput(float deltaTime);
    void Update(float deltaTime);
    void Render();

    GLFWwindow* m_Window = nullptr;
    std::shared_ptr<Camera> m_Camera;
    SceneManager            m_SceneManager;
    DirectionalLight        m_Light;

    static MiniEngineApp* s_Instance;
    float                   m_LastMouseX = 400.0f;
    float                   m_LastMouseY = 300.0f;
    bool                    m_FirstMouse = true;
};