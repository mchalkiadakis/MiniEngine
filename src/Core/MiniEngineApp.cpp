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

    m_Camera = std::make_shared<Camera>(45.0f, 800.0f / 600.0f, 0.1f, 5000.0f);
    m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    m_Camera->SetTarget(glm::vec3(0.0f, 0.0f, -1.0f));

    s_Instance = this;
    glfwSetCursorPosCallback(m_Window, MouseCallback);
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    m_Scene = std::make_unique<Scene>();

    m_Scene->SetSkybox(std::make_unique<Skybox>(std::vector<std::string>{
        "Assets/Skybox/right.jpg",
            "Assets/Skybox/left.jpg",
            "Assets/Skybox/top.jpg",
            "Assets/Skybox/bottom.jpg",
            "Assets/Skybox/front.jpg",
            "Assets/Skybox/back.jpg"
    }));

    auto& assets = AssetManager::Get();
    auto basicShader = assets.LoadShader("Assets/Shaders/basic.vert", "Assets/Shaders/basic.frag");

    // CREATING PYRAMID ENTITY
    std::vector<Vertex> pyramidVertices = {
        { {  0.0f,  0.5f,  0.0f }, {  0.0f,  1.0f,  0.0f }, { 0.5f, 1.0f } },
        { { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f } },
        { {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f } },
        { {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f } },
        { { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f } },
    };

    std::vector<unsigned int> pyramidIndices = {
        0, 1, 2,  0, 2, 3,  0, 3, 4,  0, 4, 1,
        1, 4, 3,  1, 3, 2
    };

    Entity& entity = m_Scene->CreateEntity("Pyramid");
    entity.SetMesh(std::make_unique<Mesh>(pyramidVertices, pyramidIndices));
    entity.SetMaterial(std::make_unique<Material>(
        basicShader,
        assets.LoadTexture("Assets/Textures/sand.jpg")
    ));
    entity.EnableRotation(true);

    // CREATING CUBE
    std::vector<Vertex> cubeVertices = {
        // Front face
        { { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f } },
        { {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f } },
        { {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f } },
        { { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f } },
        // Back face
        { { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f } },
        { {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f } },
        { {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f } },
        { { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f } },
        // Right face
        { {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f } },
        { {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } },
        { {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f } },
        { {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } },
        // Left face
        { { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f } },
        { { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } },
        { { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f } },
        { { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } },
        // Top face
        { { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f } },
        { {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f } },
        { {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f } },
        { { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f } },
        // Bottom face
        { { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 0.0f } },
        { {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f } },
        { {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 1.0f } },
        { { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f } },
    };

    std::vector<unsigned int> cubeIndices = {
         0,  1,  2,   2,  3,  0,
         4,  7,  6,   6,  5,  4,
         8,  9, 10,  10, 11,  8,
        12, 13, 14,  14, 15, 12,
        16, 17, 18,  18, 19, 16,
        20, 21, 22,  22, 23, 20,
    };

    auto model = ModelLoader::Load("Assets/Models/backpack/backpack.obj", basicShader);
    if (model) {
        Entity& e = m_Scene->CreateEntity("Backpack");
        e.SetModel(std::move(model));
        e.SetPosition(glm::vec3(3.0f, 0.0f, 0.0f));
        e.EnableRotation(true);
    }
    else {
        std::cerr << "Failed to load backpack model\n";
    }

    auto& goldCube = m_Scene->CreateEntity("Cube");
    goldCube.SetMesh(std::make_unique<Mesh>(cubeVertices, cubeIndices));
    goldCube.SetMaterial(std::make_unique<Material>(
        basicShader,
        assets.LoadTexture("Assets/Textures/gold.jpg")
    ));
    goldCube.SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));
    goldCube.EnableRotation(true);

    TerrainConfig config;
    config.Width = 64;
    config.Depth = 64;
    config.TileSize = 4.0f;
    config.HeightScale = 90.0f;
    config.NoiseScale = 0.02f;

    auto terrainMat = std::make_shared<Material>(
        basicShader,
        assets.LoadTexture("Assets/Textures/sand.jpg")
    );

    auto chunkManager = std::make_unique<ChunkManager>(config, terrainMat);
    chunkManager->SetViewDistance(4);
    m_Scene->SetChunkManager(std::move(chunkManager));

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
    m_Scene->Update(deltaTime, *m_Camera);
}

void MiniEngineApp::Render() {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderContext ctx{ *m_Camera, m_Light };
    m_Scene->Render(ctx);
}

MiniEngineApp* MiniEngineApp::s_Instance = nullptr;

void MiniEngineApp::MouseCallback(GLFWwindow*, double xpos, double ypos) {
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