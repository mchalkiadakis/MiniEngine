#include "MiniEngine.h"

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
    dungeonCfg.RoomHeight = 50.0f;
    dungeonCfg.CellSize = 2.0f;
    dungeonCfg.NodePadding = 50.0f;
    dungeonCfg.MinRooms = 6;
    dungeonCfg.MaxRooms = 10;
    dungeonCfg.Seed = 424209;
    DungeonData dungeon = DungeonGenerator::Generate(dungeonCfg);

    const RoomData& startRoom = dungeon.Rooms[dungeon.StartRoomIndex];
    glm::vec2 startCenter = startRoom.Center();
    m_Camera->SetPosition(glm::vec3(startCenter.x, 2.0f, startCenter.y));
    m_Camera->SetTarget(glm::vec3(startCenter.x + 1.0f, 2.0f, startCenter.y));

    std::cout << "Start room: x=" << startRoom.X << " z=" << startRoom.Z
        << " w=" << startRoom.Width << " d=" << startRoom.Depth << "\n";
    std::cout << "Camera position: "
        << m_Camera->GetPosition().x << ", "
        << m_Camera->GetPosition().y << ", "
        << m_Camera->GetPosition().z << "\n";

    m_Light.Color = glm::vec3(0.3f, 0.3f, 0.3f);
    m_Light.Direction = glm::normalize(glm::vec3(0.5f, 1.0f, 0.3f));

    auto rockNormal = assets.LoadTexture("Assets/Textures/Rock035_1K-JPG_NormalGL.jpg");

    auto wallMat = std::make_shared<Material>(
        basicShader,
        assets.LoadTexture("Assets/Textures/gold.jpg"),
        rockNormal
    );
    auto floorMat = std::make_shared<Material>(
        basicShader,
        assets.LoadTexture("Assets/Textures/DungeonFloor.jpg"),
        rockNormal
    );

    m_SceneManager.LoadScene(
        std::make_unique<DungeonScene>(dungeon, wallMat, floorMat)
    );

    auto* scene = m_SceneManager.GetCurrentScene();

    float margin = 8.0f;
    for (const auto& room : dungeon.Rooms) {
        auto addTorch = [&](float x, float z) {
            PointLight torch;
            torch.Position = glm::vec3(x, 4.0f, z);
            torch.Color = glm::vec3(1.0f, 0.4f, 0.05f);
            torch.Radius = 35.0f;
            torch.Intensity = 8.0f;
            scene->AddPointLight(torch);
            };
        addTorch(room.X + margin, room.Z + margin);
        addTorch(room.X + room.Width - margin, room.Z + margin);
        addTorch(room.X + margin, room.Z + room.Depth - margin);
        addTorch(room.X + room.Width - margin, room.Z + room.Depth - margin);
    }

    m_Fog.Enabled = true;
    m_Fog.Density = 0.009f;
    m_Fog.Color = glm::vec3(0.15f, 0.15f, 0.15f);

    std::cout << "Registered " << scene->GetPointLights().size() << " torches\n";

    // sphere
    const int sphereStacks = 16;
    const int sphereSlices = 32;
    const float sphereRadius = 2.0f;

    std::vector<Vertex> sphereVertices;
    std::vector<unsigned int> sphereIndices;

    for (int i = 0; i <= sphereStacks; ++i) {
        float stackAngle = glm::pi<float>() / 2.0f - i * glm::pi<float>() / sphereStacks;
        float xy = sphereRadius * cosf(stackAngle);
        float z = sphereRadius * sinf(stackAngle);

        for (int j = 0; j <= sphereSlices; ++j) {
            float sliceAngle = 2.0f * glm::pi<float>() * j / sphereSlices;
            float x = xy * cosf(sliceAngle);
            float y = xy * sinf(sliceAngle);

            glm::vec3 pos(x, z, y);
            glm::vec3 normal = glm::normalize(pos);
            glm::vec2 texCoord(static_cast<float>(j) / sphereSlices,
                static_cast<float>(i) / sphereStacks);
            sphereVertices.push_back({ pos, normal, texCoord });
        }
    }

    for (int i = 0; i < sphereStacks; ++i) {
        unsigned int k1 = i * (sphereSlices + 1);
        unsigned int k2 = k1 + sphereSlices + 1;
        for (int j = 0; j < sphereSlices; ++j) {
            if (i != 0) {
                sphereIndices.push_back(k1);
                sphereIndices.push_back(k2);
                sphereIndices.push_back(k1 + 1);
            }
            if (i != sphereStacks - 1) {
                sphereIndices.push_back(k1 + 1);
                sphereIndices.push_back(k2);
                sphereIndices.push_back(k2 + 1);
            }
            k1++;
            k2++;
        }
    }

    m_TestSphere = &scene->CreateEntity("TestSphere");
    m_TestSphere->SetMesh(std::make_unique<Mesh>(sphereVertices, sphereIndices));
    m_TestSphere->SetMaterial(std::make_unique<Material>(
        basicShader,
        assets.LoadTexture("Assets/Textures/Rock.jpg")
    ));
    m_SphereCenterPos = glm::vec3(startCenter.x + 20.0f, 3.0f, startCenter.y);
    m_TestSphere->SetPosition(m_SphereCenterPos);

    // cube
    std::vector<Vertex> cubeVertices = {
        { { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f } },
        { {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f } },
        { {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f } },
        { { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f } },
        { { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f } },
        { {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f } },
        { {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f } },
        { { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f } },
        { {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f } },
        { {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } },
        { {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f } },
        { {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } },
        { { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f } },
        { { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } },
        { { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f } },
        { { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } },
        { { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f } },
        { {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f } },
        { {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f } },
        { { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f } },
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

    auto& testBox = scene->CreateEntity("TestBox");
    testBox.SetMesh(std::make_unique<Mesh>(cubeVertices, cubeIndices));
    testBox.SetMaterial(std::make_unique<Material>(
        basicShader,
        assets.LoadTexture("Assets/Textures/Rock.jpg")
    ));
    testBox.AddComponent<TransformComponent>(
        glm::vec3(startCenter.x, 3000.0f, startCenter.y + 3.0f),
        glm::vec3(0.0f),
        glm::vec3(23.0f)
    );
    testBox.AddComponent<PhysicsComponent>(true);

    auto playerActor = std::make_unique<PlayerActor>("Player", 100.0f);
    playerActor->GetTransform()->SetPosition(
        glm::vec3(startCenter.x, 0.0f, startCenter.y));
    m_Player = playerActor.get();

    AABB playerBounds;
    playerBounds.Min = glm::vec3(-2.5f, 0.0f, -2.5f);
    playerBounds.Max = glm::vec3(2.5f, 3.6f, 2.5f);
    playerActor->AddComponent<ColliderComponent>(dungeon.Grid, playerBounds);
    scene->AddActor(std::move(playerActor));

    std::cout << "Player created at: "
        << startCenter.x << ", 0, " << startCenter.y << "\n";

    // shadow map
    m_ShadowMap.Init();
    auto depthShaderPtr = assets.LoadShader(
        "Assets/Shaders/depth.vert",
        "Assets/Shaders/depth.frag"
    );
    if (!depthShaderPtr) {
        std::cerr << "Failed to load depth shader!\n";
        return false;
    }
    m_DepthShader = depthShaderPtr.get();

    // offscreen buffer — change first two values to control resolution
    // 320x240 = PS1 look, 1280x720 = full res
    // PS1 look
    m_OffscreenBuffer.Init(320, 240, 1280, 720);

    // Full resolution — no pixelation
    //m_OffscreenBuffer.Init(1280, 720, 1280, 720);

    m_ScreenShader = assets.LoadShader(
        "Assets/Shaders/retro.vert",
        "Assets/Shaders/retro.frag"
    ).get();

    m_QuadMesh = std::make_unique<Mesh>(Mesh::CreateFullscreenQuad());

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
    bool forward = glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS;
    bool backward = glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS;
    bool left = glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS;
    bool right = glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS;

    if (glfwGetKey(m_Window, GLFW_KEY_F) == GLFW_PRESS && !m_FKeyPressed) {
        m_FKeyPressed = true;
        m_FreeRoam = !m_FreeRoam;
        std::cout << (m_FreeRoam ? "Free roam ON\n" : "Free roam OFF\n");
    }
    if (glfwGetKey(m_Window, GLFW_KEY_F) == GLFW_RELEASE)
        m_FKeyPressed = false;

    if (m_FreeRoam || !m_Player) {
        bool down = glfwGetKey(m_Window, GLFW_KEY_Q) == GLFW_PRESS;
        bool up = glfwGetKey(m_Window, GLFW_KEY_E) == GLFW_PRESS;
        m_Camera->ProcessKeyboard(deltaTime, forward, backward, left, right, down, up);
    }
    else {
        m_Player->ProcessInput(forward, backward, left, right, *m_Camera);
        if (glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_PRESS)
            m_Player->TriggerAttack();
    }
}

void MiniEngineApp::Update(float deltaTime) {
    m_SceneManager.Update(deltaTime, *m_Camera);

    if (m_TestSphere) {
        m_SpherePhase += deltaTime * 0.5f;
        float offsetX = sinf(m_SpherePhase) * 8.0f;
        m_TestSphere->SetPosition(m_SphereCenterPos +
            glm::vec3(offsetX + 20.0f, 20.0f, 20.0f));
    }

    if (m_Player && !m_FreeRoam) {
        auto* transform = m_Player->GetTransform();
        if (transform) {
            glm::vec3 playerPos = transform->GetPosition();
            m_Camera->SetPosition(playerPos + glm::vec3(0.0f, 1.8f, 0.0f));
        }
    }

    auto* scene = m_SceneManager.GetCurrentScene();
    if (scene) {
        auto next = scene->GetNextScene();
        if (next)
            m_SceneManager.LoadScene(std::move(next));
    }
}

void MiniEngineApp::Render() {
    auto* scene = m_SceneManager.GetCurrentScene();
    if (!scene || !m_DepthShader || !m_ScreenShader) return;

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
        m_LightSpaceMatrix
    };
    m_SceneManager.Render(ctx);

    // --- Pass 3: Present to screen ---
    m_OffscreenBuffer.Present(*m_ScreenShader, *m_QuadMesh);
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