#include "EnemyActor.h"
#include "World/PlayerActor.h"
#include "Core/Components/TransformComponent.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>

EnemyActor::EnemyActor(const std::string& name,
    std::shared_ptr<DungeonGrid> grid,
    const std::vector<RoomData>& rooms,
    float detectionRadius)
    : Actor(name, 100.0f)
    , m_Grid(std::move(grid))
    , m_Rooms(rooms)
    , m_DetectionRadius(detectionRadius)
    , m_Rng(std::random_device{}())
{
    
    GetPhysics()->SetUseGravity(false);
    SetNewPatrolTarget();
}

void EnemyActor::Update(float deltaTime, const PlayerActor* player) {
    m_Animator.Update(deltaTime);
    if (!player) {
        UpdatePatrol(deltaTime);
        return;
    }

    auto* myTransform = GetTransform();
    auto* playerTransform = const_cast<PlayerActor*>(player)->GetTransform();
    if (!myTransform || !playerTransform) return;

    glm::vec3 myPos = myTransform->GetPosition();
    glm::vec3 playerPos = playerTransform->GetPosition();
    float     dist = glm::length(playerPos - myPos);

    if (dist < m_DetectionRadius) {
        if (m_State != EnemyState::Chase) {
            m_State = EnemyState::Chase;
            std::cout << "Enemy detected player\n";
        }
        UpdateChase(deltaTime, playerPos);
    }
    else {
        if (m_State != EnemyState::Patrol) {
            m_State = EnemyState::Patrol;
            SetNewPatrolTarget();
            std::cout << "Enemy lost player\n";
        }
        UpdatePatrol(deltaTime);
    }
}

void EnemyActor::UpdatePatrol(float deltaTime) {
    if (m_Path.empty()) {
        SetNewPatrolTarget();
        return;
    }
    MoveAlongPath(deltaTime);
}

void EnemyActor::UpdateChase(float deltaTime, const glm::vec3& playerPos) {
    m_RePathTimer -= deltaTime;
    if (m_RePathTimer <= 0.0f) {
        m_RePathTimer = m_RePathInterval;
        RebuildPathTo(playerPos);
    }
    MoveAlongPath(deltaTime);
}

void EnemyActor::MoveAlongPath(float deltaTime) {
    if (m_Path.empty() || m_PathIndex >= (int)m_Path.size()) return;

    auto* transform = GetTransform();
    if (!transform) return;

    glm::vec3 currentPos = transform->GetPosition();
    float cs = m_Grid->GetCellSize();

    glm::ivec2 targetCell = m_Path[m_PathIndex];
    glm::vec3 targetPos = glm::vec3(
        targetCell.x * cs + cs * 0.5f,
        currentPos.y,
        targetCell.y * cs + cs * 0.5f
    );

    glm::vec3 dir = targetPos - currentPos;
    float     dist = glm::length(dir);

    if (dist < 1.0f) {
        m_PathIndex++;
        if (m_PathIndex >= (int)m_Path.size()) {
            m_Path.clear();
            m_PathIndex = 0;
        }
        return;
    }

    // face movement direction
    glm::vec3 normDir = glm::normalize(dir);
    float yaw = glm::degrees(atan2f(normDir.x, normDir.z));
    glm::vec3 rot = transform->GetRotation();
    transform->SetRotation(glm::vec3(rot.x, yaw, rot.z));

    glm::vec3 move = normDir * m_MoveSpeed * deltaTime;
    transform->SetPosition(currentPos + move);
}

void EnemyActor::SetNewPatrolTarget() {
    if (m_Rooms.empty()) return;

    std::uniform_int_distribution<int> dist(0, (int)m_Rooms.size() - 1);
    int targetRoom = dist(m_Rng);

    glm::vec2 center = m_Rooms[targetRoom].Center();
    RebuildPathTo(glm::vec3(center.x, 0.0f, center.y));
}

void EnemyActor::RebuildPathTo(const glm::vec3& worldTarget) {
    auto* transform = GetTransform();
    if (!transform) return;

    glm::vec3 myPos = transform->GetPosition();

    int startX = m_Grid->WorldToGridX(myPos.x);
    int startZ = m_Grid->WorldToGridZ(myPos.z);
    int endX = m_Grid->WorldToGridX(worldTarget.x);
    int endZ = m_Grid->WorldToGridZ(worldTarget.z);

    m_Path = AStar::FindPath(*m_Grid, startX, startZ, endX, endZ);
    m_PathIndex = 0;
}


void EnemyActor::SetSkinnedModel(std::unique_ptr<SkinnedModel> model) {
    m_SkinnedModel = std::move(model);

    if (m_SkinnedModel && m_SkinnedModel->GetSkeleton()) {
        m_Animator.SetSkeleton(m_SkinnedModel->GetSkeleton());
        m_Animator.SetGlobalInverseTransform(
            m_SkinnedModel->GetGlobalInverseTransform());

        // resize to accommodate original bone IDs
        m_Animator.ResizeBoneMatrices(256);

        for (const auto& clip : m_SkinnedModel->GetClips())
            m_Animator.AddClip(clip);

        if (!m_SkinnedModel->GetClips().empty())
            m_Animator.Play(m_SkinnedModel->GetClips()[0].Name);

        std::cout << "Skeleton bone count: "
            << m_SkinnedModel->GetSkeleton()->GetBoneCount() << "\n";
    }
}

void EnemyActor::Render(const RenderContext& ctx,
    const glm::mat4& modelMatrix) const {
    if (!m_SkinnedModel) return;
    m_SkinnedModel->Draw(ctx, modelMatrix, m_Animator.GetBoneMatrices());
}