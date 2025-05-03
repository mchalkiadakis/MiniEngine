
#include "Core/MiniEngineApp.h"


bool MiniEngineApp::Init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(800, 600, "MiniEngine", nullptr, nullptr);
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

    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Load assets
    m_Shader = std::make_shared<Shader>("Assets/Shaders/basic.vert", "Assets/Shaders/basic.frag");
    m_Texture = std::make_shared<Texture>("Assets/Textures/sand.jpg");
    m_Material = std::make_unique<Material>(m_Shader, m_Texture);

    std::vector<float> vertices = {
        0.0f,  0.5f,  0.0f,   0.5f, 1.0f,
       -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   1.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,   0.0f, 1.0f
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1,
        1, 4, 3, 1, 3, 2
    };

    m_Mesh = std::make_unique<Mesh>(vertices, indices, sizeof(float) * 5);

    m_Camera = std::make_unique<Camera>(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    m_Camera->SetTarget(glm::vec3(0.0f, 0.0f, -1.0f));

    return true;
}

void MiniEngineApp::Run() {
    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(m_Window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        PollInput(deltaTime);
        Render(currentFrame);

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }

    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void MiniEngineApp::PollInput(float deltaTime) {
    bool w = glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS;
    bool s = glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS;
    bool a = glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS;
    bool d = glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS;
    bool q = glfwGetKey(m_Window, GLFW_KEY_Q) == GLFW_PRESS;
    bool e = glfwGetKey(m_Window, GLFW_KEY_E) == GLFW_PRESS;

    m_Camera->ProcessKeyboard(deltaTime, w, s, a, d, q, e);
}

void MiniEngineApp::Render(float time) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 1.0f, 0.0f));

    m_Material->Bind();
    m_Shader->Use();
    m_Shader->SetUniformMat4("u_Model", glm::value_ptr(model));
    m_Shader->SetUniformMat4("u_View", glm::value_ptr(m_Camera->GetViewMatrix()));
    m_Shader->SetUniformMat4("u_Projection", glm::value_ptr(m_Camera->GetProjectionMatrix()));

    m_Mesh->Draw();
}
