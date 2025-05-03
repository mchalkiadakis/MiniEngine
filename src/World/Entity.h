#pragma once
#include "MiniEngine.h"

class Entity {
public:
    Entity(const std::string& name);

    void SetMesh(std::unique_ptr<Mesh> mesh);
    void SetMaterial(std::unique_ptr<Material> material);
    void Update(float deltaTime);
    void Render(const Camera& camera);
    void SetTransform(const glm::mat4& transform);
    void SetPosition(const glm::vec3& pos) { m_Position = pos; }
    void EnableRotation(bool rotate) { m_ShouldRotate = rotate; }
private:
    bool m_ManualTransform = false;
    std::string m_Name;
    std::unique_ptr<Mesh> m_Mesh;
    std::unique_ptr<Material> m_Material;
    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
    float m_RotationAngle = 0.0f;
    bool m_ShouldRotate = false;
    
    glm::vec3 m_Position = glm::vec3(0.0f);
};