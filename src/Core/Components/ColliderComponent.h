#pragma once
#include "Core/Component.h"
#include "Core/Components/TransformComponent.h"
#include "World/Dungeon/DungeonGrid.h"
#include <memory>
#include <glm.hpp>

class Entity;

struct AABB {
    glm::vec3 Min;
    glm::vec3 Max;
};

class ColliderComponent : public Component {
public:
    ColliderComponent(std::shared_ptr<DungeonGrid> grid, const AABB& localBounds)
        : m_Grid(std::move(grid))
        , m_Bounds(localBounds)
    {
    }

    void Start(Entity& owner) override;
    void Update(float deltaTime) override;

private:
    bool IsWallAt(float worldX, float worldZ) const;

    std::shared_ptr<DungeonGrid> m_Grid;
    AABB                         m_Bounds;
    TransformComponent* m_Transform = nullptr;
    glm::vec3                    m_PreviousPosition = glm::vec3(0.0f);
};