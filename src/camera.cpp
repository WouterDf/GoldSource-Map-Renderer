#include "camera.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 forward)
     : m_position( position ),
       m_forward( forward )
{}

void Camera::Move(glm::vec3 direction, float deltaTime, bool boost)
{
    const static float BOOST_MULTIPLIER = 3.0F;

    auto m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 velocity =
          direction.z * m_forward +
          direction.x * m_right +
          direction.y * m_up;

    float boostFactor = 1.0F;
    if( boost )
    {
        boostFactor = BOOST_MULTIPLIER;
    }

    m_position += velocity * boostFactor * m_moveSpeed * deltaTime;
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

