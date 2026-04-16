// Camera.h
#pragma once
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

class Camera {
public:
    Camera(float fov, float aspectRatio, float nearClip, float farClip);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    void ProcessKeyboard(float deltaTime, bool forward, bool backward, bool left, bool right, bool down, bool up);
    void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
    void SetPosition(const glm::vec3& position);
    void SetTarget(const glm::vec3& target);

    glm::vec3 GetPosition() const { return m_Position; }

private:
    glm::vec3 m_Position;
    glm::vec3 m_Target;
    glm::vec3 m_Up;
    glm::vec3 m_Front;
    glm::vec3 m_Right;

    float m_FOV;
    float m_Aspect;
    float m_Near;
    float m_Far;

    float m_Speed = 50.5f;
    float m_MouseSensitivity = 0.1f;
    float m_Yaw = -90.0f;
    float m_Pitch = 0.0f;
};