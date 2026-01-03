#include "camera.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 forward)
     : m_position( position ),
       m_forward( forward )
{}

void Camera::Move(glm::vec3 direction, float deltaTime)
{
     m_position += m_moveSpeed * deltaTime * direction;
}

void Camera::Rotate(float pitch, float yaw, float deltaTime)
{
     m_pitch += pitch * m_rotSpeed * deltaTime;
     m_yaw += yaw * m_rotSpeed * deltaTime;
}

glm::mat4 Camera::GetViewMatrix() {
    m_forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_forward.y = sin(glm::radians(m_pitch));
    m_forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_forward = glm::normalize(m_forward);

    return glm::lookAt(m_position, m_position + m_forward, m_up);
}

