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

    s_Instance = this;
    glfwSetCursorPosCallback(m_Window, MouseCallback);
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    auto& assets = AssetManager::Get();
    auto basicShader = assets.LoadShader("Assets/Shaders/basic.vert",
        "Assets/Shaders/basic.frag");

    // generate dungeon
    DungeonConfig dungeonCfg;
    dungeonCfg.FloorWidth = 1000.0f;
    dungeonCfg.FloorDepth = 1000.0f;
    dungeonCfg.MinRoomWidth = 60.0f;
    dungeonCfg.MaxRoomWidth = 120.0f;
    dungeonCfg.MinRoomDepth = 60.0f;
    dungeonCfg.MaxRoomDepth = 120.0f;
    dungeonCfg.RoomHeight = 25.0f;
    dungeonCfg.CorridorWidth = 12.0f;
    dungeonCfg.NodePadding = 50.0f;
    dungeonCfg.MinRooms = 6;
    dungeonCfg.MaxRooms = 10;
    DungeonData dungeon = DungeonGenerator::Generate(dungeonCfg);

    std::cout << "Generated " << dungeon.Rooms.size() << " rooms and "
        << dungeon.Corridors.size() << " corridors\n";

    // place camera in start room
    const RoomData& startRoom = dungeon.Rooms[dungeon.StartRoomIndex];
    glm::vec2 startCenter = startRoom.Center();
    m_Camera->SetPosition(glm::vec3(startCenter.x, 2.0f, startCenter.y));
    m_Camera->SetTarget(glm::vec3(startCenter.x, 2.0f, startCenter.y - 1.0f));

    std::cout << "Start room: x=" << startRoom.X << " z=" << startRoom.Z
        << " w=" << startRoom.Width << " d=" << startRoom.Depth << "\n";
    std::cout << "Start room center: " << startCenter.x << ", " << startCenter.y << "\n";
    std::cout << "Camera position: "
        << m_Camera->GetPosition().x << ", "
        << m_Camera->GetPosition().y << ", "
        << m_Camera->GetPosition().z << "\n";

    // place 4 torches near the corners of the start room
    float margin = 8.0f;
    std::vector<glm::vec3> torchPositions = {
        { startRoom.X + margin,                       4.0f, startRoom.Z + margin },
        { startRoom.X + startRoom.Width - margin,     4.0f, startRoom.Z + margin },
        { startRoom.X + margin,                       4.0f, startRoom.Z + startRoom.Depth - margin },
        { startRoom.X + startRoom.Width - margin,     4.0f, startRoom.Z + startRoom.Depth - margin }
    };

    for (auto& pos : torchPositions) {
        PointLight torch;
        torch.Position = pos;
        torch.Color = glm::vec3(0.5f, 0.5f, 0.5f);
        torch.Radius = 35.0f;
        torch.Intensity = 8.0f;
        m_PointLights.push_back(torch);
    }

    m_Light.Color = glm::vec3(1.0f, 1.0f, 1.0f);
    m_Light.Direction = glm::normalize(glm::vec3(0.5f, 1.0f, 0.3f));

    auto wallMat = std::make_shared<Material>(
        basicShader,
        assets.LoadTexture("Assets/Textures/Rock.jpg")
    );

    auto floorMat = std::make_shared<Material>(
        basicShader,
        assets.LoadTexture("Assets/Textures/Cobblestone.jpg")
    );

    m_SceneManager.LoadScene(
        std::make_unique<DungeonScene>(dungeon, wallMat, floorMat)
    );

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
    m_SceneManager.Update(deltaTime, *m_Camera);
}

void MiniEngineApp::Render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderContext ctx{ *m_Camera, m_Light, m_PointLights };
    m_SceneManager.Render(ctx);
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