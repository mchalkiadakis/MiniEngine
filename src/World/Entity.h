#pragma once
#include <string>
#include <memory>
#include <vector>
#include <typeindex>
#include <unordered_map>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Rendering/Mesh.h"
#include "Rendering/Material.h"
#include "Rendering/Model.h"
#include "Rendering/RenderContext.h"
#include "Core/Component.h"

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

    const std::string& GetName()     const { return m_Name; }
    glm::vec3          GetPosition() const { return m_Position; }

    unsigned int GetShaderID() const {
        return m_Material ? m_Material->GetShader()->GetID() : 0;
    }

    // component system
    template<typename T, typename... Args>
    T& AddComponent(Args&&... args) {
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *component;
        ref.Start(*this);
        m_Components[std::type_index(typeid(T))] = std::move(component);
        return ref;
    }

    template<typename T>
    T* GetComponent() {
        auto it = m_Components.find(std::type_index(typeid(T)));
        if (it != m_Components.end())
            return static_cast<T*>(it->second.get());
        return nullptr;
    }

    template<typename T>
    bool HasComponent() const {
        return m_Components.count(std::type_index(typeid(T))) > 0;
    }

    template<typename T>
    void RemoveComponent() {
        m_Components.erase(std::type_index(typeid(T)));
    }

private:
    bool                      m_ManualTransform = false;
    std::string               m_Name;
    std::unique_ptr<Mesh>     m_Mesh;
    std::unique_ptr<Material> m_Material;
    std::unique_ptr<Model>    m_Model;
    glm::mat4                 m_ModelMatrix = glm::mat4(1.0f);
    float                     m_RotationAngle = 0.0f;
    bool                      m_ShouldRotate = false;
    glm::vec3                 m_Position = glm::vec3(0.0f);

    std::unordered_map<std::type_index, std::unique_ptr<Component>> m_Components;
};