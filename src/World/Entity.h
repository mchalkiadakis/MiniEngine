#pragma once
#include "MiniEngine.h"

class Entity {
public:
    Entity(const std::string& name);

    void SetModel(std::unique_ptr<Model> model);
    void SetMesh(std::unique_ptr<Mesh> mesh);
    void SetMaterial(std::unique_ptr<Material> material);
    void Update(float deltaTime);
    void Render(const RenderContext& ctx);
    void SetTransform(const glm::mat4& transform);
    void SetPosition(const glm::vec3& pos) { m_Position = pos; }
    void EnableRotation(bool rotate) { m_ShouldRotate = rotate; }
    unsigned int GetShaderID() const {
        return m_Material ? m_Material->GetShader()->GetID() : 0;
    }
private:
    bool m_ManualTransform = false;
    std::string m_Name;
    std::unique_ptr<Mesh> m_Mesh;
    std::unique_ptr<Material> m_Material;
    std::unique_ptr<Model> m_Model;
    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
    float m_RotationAngle = 0.0f;
    bool m_ShouldRotate = false;
    
    glm::vec3 m_Position = glm::vec3(0.0f);
};