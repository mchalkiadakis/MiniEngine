#pragma once
#include "RoomData.h"
#include "World/Scene.h"
#include "Rendering/RenderContext.h"
#include "Rendering/Material.h"
#include "Rendering/DynamicMesh.h"
#include "World/Dungeon/DungeonGenerator.h"
#include "Camera/Camera.h"
#include <vector>
#include <memory>
#include <glm.hpp>
#include "World/PlayerActor.h"
#include "World/EnemyActor.h"
#include "Rendering/ParticleEmitter.h"

class DungeonScene : public Scene {
public:
    // default constructor — generates dungeon with default config
    DungeonScene();

    // custom config constructor
    explicit DungeonScene(const DungeonConfig& config);

    void Update(float deltaTime, Camera& camera) override;
    void Render(const RenderContext& ctx) const override;
    void RenderDepth(Shader& depthShader,
        const glm::mat4& lightSpaceMatrix) const override;
    void RequestTransition(std::unique_ptr<IScene> nextScene);
    std::unique_ptr<IScene> GetNextScene() override;
    void RenderTransparent(const RenderContext& ctx) const override;


    const std::vector<RoomData>& GetRooms() const { return m_Data.Rooms; }

private:
    void Build(const DungeonConfig& config);
    PlayerActor* m_Player = nullptr;
    DynamicMesh               m_WallMesh;
    DynamicMesh               m_FloorMesh;
    std::shared_ptr<Material> m_WallMaterial;
    std::shared_ptr<Material> m_FloorMaterial;
    std::unique_ptr<IScene>   m_NextScene;
    DungeonData               m_Data;
    EnemyActor* m_Enemy = nullptr;
    std::vector<ParticleEmitter> m_TorchEmitters;
    Shader* m_ParticleShader = nullptr;
    Shader* m_SkinnedDepthShader = nullptr;
    bool m_FreeRoam = false;
    bool m_FKeyPressed = false;
    bool m_FirstUpdate = true;
};