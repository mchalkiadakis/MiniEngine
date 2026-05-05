#include "DungeonScene.h"
#include "DungeonMeshBuilder.h"
#include "Core/AssetManager.h"
#include "Core/Components/TransformComponent.h"
#include "Core/Components/PhysicsComponent.h"
#include "Core/Components/ColliderComponent.h"
#include "World/PlayerActor.h"
#include "Rendering/Mesh.h"
#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "Core/InputManager.h"

static DungeonConfig DefaultConfig() {
    DungeonConfig cfg;
    cfg.FloorWidth = 1000.0f;
    cfg.FloorDepth = 1000.0f;
    cfg.MinRoomWidth = 60.0f;
    cfg.MaxRoomWidth = 120.0f;
    cfg.MinRoomDepth = 60.0f;
    cfg.MaxRoomDepth = 120.0f;
    cfg.RoomHeight = 50.0f;
    cfg.CellSize = 2.0f;
    cfg.NodePadding = 50.0f;
    cfg.MinRooms = 6;
    cfg.MaxRooms = 10;
    cfg.Seed = 424209;
    return cfg;
}

DungeonScene::DungeonScene()
    : DungeonScene(DefaultConfig()) {
}

DungeonScene::DungeonScene(const DungeonConfig& config) {
    Build(config);
}

void DungeonScene::Build(const DungeonConfig& config) {
    auto& assets = AssetManager::Get();

    auto basicShader = assets.LoadShader(
        "Assets/Shaders/basic.vert",
        "Assets/Shaders/basic.frag"
    );
    auto rockNormal = assets.LoadTexture(
        "Assets/Textures/Rock035_1K-JPG_NormalGL.jpg"
    );

    m_WallMaterial = std::make_shared<Material>(
        basicShader,
        assets.LoadTexture("Assets/Textures/gold.jpg"),
        rockNormal
    );
    m_FloorMaterial = std::make_shared<Material>(
        basicShader,
        assets.LoadTexture("Assets/Textures/DungeonFloor.jpg"),
        rockNormal
    );

    m_Data = DungeonGenerator::Generate(config);

    if (m_Data.Grid) {
        DungeonMeshBuilder::BuildDungeonWallMesh(m_WallMesh,
            *m_Data.Grid, m_Data.RoomHeight);
        DungeonMeshBuilder::BuildDungeonFloorMesh(m_FloorMesh,
            *m_Data.Grid, m_Data.RoomHeight);
    }

    std::cout << "Built dungeon wall mesh\n";
    std::cout << "Built dungeon floor mesh\n";

    // place torches
    float margin = 8.0f;
    for (const auto& room : m_Data.Rooms) {
        auto addTorch = [&](float x, float z) {
            PointLight torch;
            torch.Position = glm::vec3(x, 4.0f, z);
            torch.Color = glm::vec3(1.0f, 0.4f, 0.05f);
            torch.Radius = 35.0f;
            torch.Intensity = 8.0f;
            AddPointLight(torch);
            };
        addTorch(room.X + margin, room.Z + margin);
        addTorch(room.X + room.Width - margin, room.Z + margin);
        addTorch(room.X + margin, room.Z + room.Depth - margin);
        addTorch(room.X + room.Width - margin, room.Z + room.Depth - margin);
    }

    std::cout << "Registered " << GetPointLights().size() << " torches\n";

    // spawn player at start room
    const RoomData& startRoom = m_Data.Rooms[m_Data.StartRoomIndex];
    glm::vec2 startCenter = startRoom.Center();

    auto playerActor = std::make_unique<PlayerActor>("Player", 100.0f);
    playerActor->GetTransform()->SetPosition(
        glm::vec3(startCenter.x, 0.0f, startCenter.y));

    m_Player = playerActor.get(); // save before move

    AABB playerBounds;
    playerBounds.Min = glm::vec3(-2.5f, 0.0f, -2.5f);
    playerBounds.Max = glm::vec3(2.5f, 3.6f, 2.5f);
    playerActor->AddComponent<ColliderComponent>(m_Data.Grid, playerBounds);

    AddActor(std::move(playerActor));

    std::cout << "Player spawned at: "
        << startCenter.x << ", 0, " << startCenter.y << "\n";
}

void DungeonScene::Update(float deltaTime, Camera& camera) {
    // free roam toggle
    if (InputManager::IsKeyDown(GLFW_KEY_F) && !m_FKeyPressed) {
        m_FKeyPressed = true;
        m_FreeRoam = !m_FreeRoam;
        std::cout << (m_FreeRoam ? "Free roam ON\n" : "Free roam OFF\n");
    }
    if (InputManager::IsKeyUp(GLFW_KEY_F))
        m_FKeyPressed = false;

    if (m_FreeRoam) {
        bool forward = InputManager::IsKeyDown(GLFW_KEY_W);
        bool backward = InputManager::IsKeyDown(GLFW_KEY_S);
        bool left = InputManager::IsKeyDown(GLFW_KEY_A);
        bool right = InputManager::IsKeyDown(GLFW_KEY_D);
        bool down = InputManager::IsKeyDown(GLFW_KEY_Q);
        bool up = InputManager::IsKeyDown(GLFW_KEY_E);
        camera.ProcessKeyboard(deltaTime, forward, backward, left, right, down, up);
    }
    else if (m_Player) {
        bool forward = InputManager::IsKeyDown(GLFW_KEY_W);
        bool backward = InputManager::IsKeyDown(GLFW_KEY_S);
        bool left = InputManager::IsKeyDown(GLFW_KEY_A);
        bool right = InputManager::IsKeyDown(GLFW_KEY_D);

        m_Player->ProcessInput(forward, backward, left, right, camera);

        if (InputManager::IsKeyDown(GLFW_KEY_SPACE))
            m_Player->TriggerAttack();

        auto* transform = m_Player->GetTransform();
        if (transform) {
            glm::vec3 pos = transform->GetPosition();
            camera.SetPosition(pos + glm::vec3(0.0f, 1.8f, 0.0f));
        }
    }

    Scene::Update(deltaTime, camera);
}

void DungeonScene::Render(const RenderContext& ctx) const {
    Scene::Render(ctx);

    if (m_WallMaterial) {
        auto shader = m_WallMaterial->GetShader();
        m_WallMaterial->Bind();
        ctx.ApplyToShader(*shader, glm::mat4(1.0f));
        m_WallMesh.Draw();
    }

    if (m_FloorMaterial) {
        auto shader = m_FloorMaterial->GetShader();
        m_FloorMaterial->Bind();
        ctx.ApplyToShader(*shader, glm::mat4(1.0f));
        m_FloorMesh.Draw();
    }
}

void DungeonScene::RenderDepth(Shader& depthShader,
    const glm::mat4& lightSpaceMatrix) const {
    Scene::RenderDepth(depthShader, lightSpaceMatrix);

    depthShader.Use();
    depthShader.SetUniformMat4("u_LightSpaceMatrix",
        glm::value_ptr(lightSpaceMatrix));

    glm::mat4 identity(1.0f);
    depthShader.SetUniformMat4("u_Model", glm::value_ptr(identity));

    m_WallMesh.Draw();
    m_FloorMesh.Draw();
}

void DungeonScene::RequestTransition(std::unique_ptr<IScene> nextScene) {
    m_NextScene = std::move(nextScene);
}

std::unique_ptr<IScene> DungeonScene::GetNextScene() {
    return std::move(m_NextScene);
}