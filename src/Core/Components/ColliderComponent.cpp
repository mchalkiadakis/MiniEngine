#include "ColliderComponent.h"
#include "World/Entity.h"

void ColliderComponent::Start(Entity& owner) {
    m_Transform = owner.GetComponent<TransformComponent>();
    if (m_Transform)
        m_PreviousPosition = m_Transform->GetPosition();
}

void ColliderComponent::Update(float deltaTime) {
    if (!m_Transform) return;

    glm::vec3 pos = m_Transform->GetPosition();

    // test X axis with old Z
    float minX = pos.x + m_Bounds.Min.x;
    float maxX = pos.x + m_Bounds.Max.x;
    float minZ = m_PreviousPosition.z + m_Bounds.Min.z;
    float maxZ = m_PreviousPosition.z + m_Bounds.Max.z;

    bool blockedX = IsWallAt(minX, minZ) || IsWallAt(maxX, minZ) ||
        IsWallAt(minX, maxZ) || IsWallAt(maxX, maxZ);
    if (blockedX)
        pos.x = m_PreviousPosition.x;

    // test Z axis with resolved X
    float minX2 = pos.x + m_Bounds.Min.x;
    float maxX2 = pos.x + m_Bounds.Max.x;
    float minZ2 = pos.z + m_Bounds.Min.z;
    float maxZ2 = pos.z + m_Bounds.Max.z;

    bool blockedZ = IsWallAt(minX2, minZ2) || IsWallAt(maxX2, minZ2) ||
        IsWallAt(minX2, maxZ2) || IsWallAt(maxX2, maxZ2);
    if (blockedZ)
        pos.z = m_PreviousPosition.z;

    m_Transform->SetPosition(pos);
    m_PreviousPosition = pos;
}

bool ColliderComponent::IsWallAt(float worldX, float worldZ) const {
    int gx = m_Grid->WorldToGridX(worldX);
    int gz = m_Grid->WorldToGridZ(worldZ);

    if (!m_Grid->InBounds(gx, gz)) return true;

    return m_Grid->GetCell(gx, gz).Type == CellType::Wall;
}