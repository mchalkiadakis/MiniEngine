#pragma once
#include "World/Actor.h"
#include "World/Dungeon/DungeonGrid.h"
#include "World/Dungeon/RoomData.h"
#include "World/Dungeon/AStar.h"
#include "Rendering/Model.h"
#include "Rendering/Animator.h"
#include <vector>
#include <memory>
#include <random>

class PlayerActor;

enum class EnemyState {
    Patrol,
    Chase
};

class EnemyActor : public Actor {
public:
    EnemyActor(const std::string& name,
        std::shared_ptr<DungeonGrid> grid,
        const std::vector<RoomData>& rooms,
        float detectionRadius = 40.0f);

    void Update(float deltaTime, const PlayerActor* player);

    
    void SetSkinnedModel(std::unique_ptr<SkinnedModel> model);
    void Render(const RenderContext& ctx, const glm::mat4& modelMatrix) const;

private:
    void UpdatePatrol(float deltaTime);
    void UpdateChase(float deltaTime, const glm::vec3& playerPos);
    void MoveAlongPath(float deltaTime);
    void SetNewPatrolTarget();
    void RebuildPathTo(const glm::vec3& worldTarget);

    std::shared_ptr<DungeonGrid>        m_Grid;
    const std::vector<RoomData>& m_Rooms;

    EnemyState                          m_State = EnemyState::Patrol;
    std::vector<glm::ivec2>             m_Path;
    int                                 m_PathIndex = 0;

    float                               m_DetectionRadius = 40.0f;
    float                               m_MoveSpeed = 15.0f;
    float                               m_RePathTimer = 0.0f;
    float                               m_RePathInterval = 0.5f;

    std::mt19937                        m_Rng;

    // animation
    std::unique_ptr<SkinnedModel>       m_SkinnedModel;
    Animator                            m_Animator;
};