#pragma once
#include "MiniEngine.h"
#include "Core/SceneManager.h"
#include "Effects/FogSettings.h"
#include "Rendering/ShadowMap.h"
#include "Rendering/OffscreenBuffer.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class MiniEngineApp {
public:
    bool Init();
    void Run();
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);

private:
    void PollInput(float deltaTime);
    void Update(float deltaTime);
    void Render();
    void RenderDebugOverlay();

    // rendering
    OffscreenBuffer         m_OffscreenBuffer;
    Shader* m_ScreenShader = nullptr;
    std::unique_ptr<Mesh>   m_QuadMesh;
    ShadowMap               m_ShadowMap;
    Shader* m_DepthShader = nullptr;
    glm::mat4               m_LightSpaceMatrix{ 1.0f };

    // window
    GLFWwindow* m_Window = nullptr;
    std::shared_ptr<Camera> m_Camera;

    // scene
    SceneManager            m_SceneManager;

    // world settings
    DirectionalLight        m_Light;
    FogSettings             m_Fog;

    // debug UI
    bool                    m_ShowDebugUI = false;
    bool                    m_TabKeyPressed = false;
    float                   m_SnapStrength = 0.0f;

    // mouse
    static MiniEngineApp* s_Instance;
    float                   m_LastMouseX = 400.0f;
    float                   m_LastMouseY = 300.0f;
    bool                    m_FirstMouse = true;
};