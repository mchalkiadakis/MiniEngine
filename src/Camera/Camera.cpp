#define GLM_ENABLE_EXPERIMENTAL
#include "Camera.h"

#include <gtc/matrix_transform.hpp>
#include <gtx/normalize_dot.hpp>
#include <iostream>

Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
    : m_FOV(fov), m_Aspect(aspectRatio), m_Near(nearClip), m_Far(farClip),
    m_Position(0.0f, 0.0f, 3.0f),
    m_Target(0.0f, 0.0f, 0.0f),
    m_Up(0.0f, 1.0f, 0.0f),
    m_Front(0.0f, 0.0f, -1.0f)
{
    m_Right = glm::normalize(glm::cross(m_Front, m_Up));
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

glm::mat4 Camera::GetProjectionMatrix() const {
    return glm::perspective(glm::radians(m_FOV), m_Aspect, m_Near, m_Far);
}

void Camera::SetPosition(const glm::vec3& position) {
    m_Position = position;
    m_Target = m_Position + m_Front;
}

void Camera::SetTarget(const glm::vec3& target) {
     m_Target = target;
     m_Front = glm::normalize(target - m_Position);
     m_Right = glm::normalize(glm::cross(m_Front, m_Up));
}

void Camera::ProcessKeyboard(float deltaTime, bool forward, bool backward, bool left, bool right, bool down, bool up) {
    float velocity = m_Speed * deltaTime;

    if (forward)  m_Position += m_Front * velocity;
    if (backward) m_Position -= m_Front * velocity;
    if (left)     m_Position -= m_Right * velocity;
    if (right)    m_Position += m_Right * velocity;
    if (up)       m_Position += m_Up * velocity;
    if (down)     m_Position -= m_Up * velocity;

    m_Target = m_Position + m_Front;
}
