#include "MiniEngine.h"
#include "World/MainMenu.h"

bool MiniEngineApp::Init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(1280, 720, "MiniEngine", nullptr, nullptr);
    if (!m_Window) {
        std::cerr << "Failed to create GLFW window!\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!\n";
        return false;
    }

    glViewport(0, 0, 1280, 720);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    m_Camera = std::make_shared<Camera>(45.0f, 1280.0f / 720.0f, 0.1f, 5000.0f);

    InputManager::Init(m_Window);

    s_Instance = this;
    glfwSetCursorPosCallback(m_Window, MouseCallback);
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    m_Light.Color = glm::vec3(0.3f, 0.3f, 0.3f);
    m_Light.Direction = glm::normalize(glm::vec3(0.5f, 1.0f, 0.3f));

    m_Fog.Enabled = true;
    m_Fog.Density = 0.009f;
    m_Fog.Color = glm::vec3(0.15f, 0.15f, 0.15f);

    m_ShadowMap.Init();
    auto& assets = AssetManager::Get();
    auto depthShaderPtr = assets.LoadShader(
        "Assets/Shaders/depth.vert",
        "Assets/Shaders/depth.frag"
    );
    if (!depthShaderPtr) {
        std::cerr << "Failed to load depth shader!\n";
        return false;
    }
    m_DepthShader = depthShaderPtr.get();

    m_OffscreenBuffer.Init(320, 240, 1280, 720);
    m_ScreenShader = assets.LoadShader(
        "Assets/Shaders/retro.vert",
        "Assets/Shaders/retro.frag"
    ).get();
    m_QuadMesh = std::make_unique<Mesh>(Mesh::CreateFullscreenQuad());

    m_SceneManager.LoadScene(std::make_unique<MainMenu>());

    return true;
}

void MiniEngineApp::Run() {
    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(m_Window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        PollInput(deltaTime);
        Update(deltaTime);
        Render();

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void MiniEngineApp::PollInput(float deltaTime) {
    // Tab toggles debug UI
    if (glfwGetKey(m_Window, GLFW_KEY_TAB) == GLFW_PRESS && !m_TabKeyPressed) {
        m_TabKeyPressed = true;
        m_ShowDebugUI = !m_ShowDebugUI;
        if (m_ShowDebugUI)
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (glfwGetKey(m_Window, GLFW_KEY_TAB) == GLFW_RELEASE)
        m_TabKeyPressed = false;
}

void MiniEngineApp::Update(float deltaTime) {
    m_SceneManager.Update(deltaTime, *m_Camera);

    auto* scene = m_SceneManager.GetCurrentScene();
    if (scene) {
        auto next = scene->GetNextScene();
        if (next)
            m_SceneManager.LoadScene(std::move(next));
    }
}

void MiniEngineApp::RenderDebugOverlay() {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Always);
    ImGui::Begin("MiniEngine Debug", nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Separator();

    glm::vec3 camPos = m_Camera->GetPosition();
    ImGui::Text("Camera: %.1f, %.1f, %.1f",
        camPos.x, camPos.y, camPos.z);
    ImGui::Separator();

    ImGui::Text("Fog");
    ImGui::Checkbox("Enabled##fog", &m_Fog.Enabled);
    ImGui::SliderFloat("Density", &m_Fog.Density, 0.0f, 0.05f);
    ImGui::ColorEdit3("Fog Color", &m_Fog.Color.x);
    ImGui::Separator();

    ImGui::Text("Directional Light");
    ImGui::ColorEdit3("Light Color", &m_Light.Color.x);
    ImGui::SliderFloat3("Direction", &m_Light.Direction.x, -1.0f, 1.0f);
    ImGui::Separator();

    ImGui::SliderFloat("Vertex Snap", &m_SnapStrength, 0.0f, 300.0f);
    ImGui::Separator();

    auto* scene = m_SceneManager.GetCurrentScene();
    if (scene)
        ImGui::Text("Point lights: %d",
            (int)scene->GetPointLights().size());

    ImGui::End();
}

void MiniEngineApp::Render() {
    auto* scene = m_SceneManager.GetCurrentScene();
    if (!scene || !m_DepthShader || !m_ScreenShader) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    const glm::vec3 sceneCenter(500.0f, 0.0f, 500.0f);
    const float sceneRadius = 750.0f;
    m_LightSpaceMatrix = m_ShadowMap.GetLightSpaceMatrix(
        m_Light.Direction,
        sceneCenter,
        sceneRadius
    );

    // --- Pass 1: Shadow depth pass ---
    m_ShadowMap.BindForWriting();
    scene->RenderDepth(*m_DepthShader, m_LightSpaceMatrix);
    m_ShadowMap.FinishWriting();

    // --- Pass 2: Render scene to offscreen buffer ---
    m_OffscreenBuffer.BindForWriting();
    m_ShadowMap.BindForReading(4);

    RenderContext ctx{
        *m_Camera,
        m_Light,
        scene->GetPointLights(),
        m_Fog,
        m_LightSpaceMatrix,
        4,
        m_SnapStrength
    };
    m_SceneManager.Render(ctx);

    // --- Pass 3: Present to screen ---
    m_OffscreenBuffer.Present(*m_ScreenShader, *m_QuadMesh);

    // --- Pass 4: ImGui ---
    if (m_ShowDebugUI)
        RenderDebugOverlay();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

MiniEngineApp* MiniEngineApp::s_Instance = nullptr;

void MiniEngineApp::MouseCallback(GLFWwindow*, double xpos, double ypos) {
    if (s_Instance->m_ShowDebugUI) return;

    if (s_Instance->m_FirstMouse) {
        s_Instance->m_LastMouseX = static_cast<float>(xpos);
        s_Instance->m_LastMouseY = static_cast<float>(ypos);
        s_Instance->m_FirstMouse = false;
        return;
    }

    float xOffset = static_cast<float>(xpos) - s_Instance->m_LastMouseX;
    float yOffset = -(static_cast<float>(ypos) - s_Instance->m_LastMouseY);

    s_Instance->m_LastMouseX = static_cast<float>(xpos);
    s_Instance->m_LastMouseY = static_cast<float>(ypos);

    s_Instance->m_Camera->ProcessMouseMovement(xOffset, yOffset);
}