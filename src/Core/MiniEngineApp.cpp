
#include "MiniEngine.h"

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

    m_Camera = std::make_shared<Camera>(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    m_Camera->SetTarget(glm::vec3(0.0f, 0.0f, -1.0f));

    m_Scene = std::make_unique<Scene>();


    //CREATING PYRAMID ENTITY
    Entity& entity = m_Scene->CreateEntity("Pyramid");
    entity.SetMesh(std::make_unique<Mesh>(
        std::vector<float>{
        0.0f, 0.5f, 0.0f, 0.5f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f
    },
        std::vector<unsigned int>{
        0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1,
            1, 4, 3, 1, 3, 2
    },
        sizeof(float) * 5
    ));
    entity.SetMaterial(std::make_unique<Material>(
        std::make_shared<Shader>("Assets/Shaders/basic.vert", "Assets/Shaders/basic.frag"),
        std::make_shared<Texture>("Assets/Textures/sand.jpg")
    ));
    entity.EnableRotation(true);

    //CRTEATING CUBE 
    auto& goldCube = m_Scene->CreateEntity("Cube");
    // Cube vertex data (positions + UVs)
    std::vector<float> cubeVertices = {
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    };

    // Index data
    std::vector<unsigned int> cubeIndices = {
        // Front
        0, 1, 2, 2, 3, 0,
        // Right
        1, 5, 6, 6, 2, 1,
        // Back
        5, 4, 7, 7, 6, 5,
        // Left
        4, 0, 3, 3, 7, 4,
        // Top
        3, 2, 6, 6, 7, 3,
        // Bottom
        4, 5, 1, 1, 0, 4
    };

    goldCube.SetMesh(std::make_unique<Mesh>(cubeVertices, cubeIndices, sizeof(float) * 5));
    goldCube.SetMaterial(std::make_unique<Material>(
        std::make_shared<Shader>("Assets/Shaders/basic.vert", "Assets/Shaders/basic.frag"),
        std::make_shared<Texture>("Assets/Textures/gold.jpg")
    ));
   
    goldCube.SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));  // above the pyramid
    goldCube.EnableRotation(true);                   
   


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

void MiniEngineApp::Update(float deltaTime) {
    m_Scene->Update(deltaTime);
}

void MiniEngineApp::Render() {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_Scene->Render(*m_Camera);
}
