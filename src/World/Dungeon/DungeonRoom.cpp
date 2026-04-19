#include "DungeonRoom.h"
#include "DungeonMeshBuilder.h"

DungeonRoom::DungeonRoom(const RoomData& data,
    std::shared_ptr<Material> material,
    const DungeonGrid& grid)
    : m_Data(data), m_Material(std::move(material))
{
    DungeonMeshBuilder::BuildRoom(m_Mesh, data, grid);
}

void DungeonRoom::Render(const RenderContext& ctx) const {
    if (!m_Material) return;
    auto shader = m_Material->GetShader();
    m_Material->Bind();
    ctx.ApplyToShader(*shader, m_ModelMatrix);
    m_Mesh.Draw();
}