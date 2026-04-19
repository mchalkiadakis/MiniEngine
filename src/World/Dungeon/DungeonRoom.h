#pragma once
#include "RoomData.h"
#include "DungeonGrid.h"
#include "Rendering/DynamicMesh.h"
#include "Rendering/Material.h"
#include "Rendering/RenderContext.h"
#include <memory>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

enum class RoomState {
    Unexplored,
    Active,
    Cleared
};

class DungeonRoom {
public:
    DungeonRoom(const RoomData& data,
        std::shared_ptr<Material> material,
        const DungeonGrid& grid);

    void Render(const RenderContext& ctx) const;

    const RoomData& GetData()  const { return m_Data; }
    RoomState       GetState() const { return m_State; }
    void            SetState(RoomState state) { m_State = state; }

private:
    RoomData                  m_Data;
    DynamicMesh               m_Mesh;
    std::shared_ptr<Material> m_Material;
    RoomState                 m_State = RoomState::Unexplored;
    glm::mat4                 m_ModelMatrix = glm::mat4(1.0f);
};