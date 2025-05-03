#pragma once

#include "MiniEngine.h"


class MiniEngineApp {
public:
    bool Init();
    void Run();
    void Shutdown();

private:
    void PollInput(float deltaTime);
    void Render(float time);

    GLFWwindow* m_Window = nullptr;

    std::shared_ptr<Shader> m_Shader;
    std::shared_ptr<Texture> m_Texture;
    std::unique_ptr<Material> m_Material;
    std::unique_ptr<Mesh> m_Mesh;
    std::unique_ptr<Camera> m_Camera;
};
