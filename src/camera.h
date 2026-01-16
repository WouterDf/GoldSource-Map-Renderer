#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>

class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 forward);
    void Move(glm::vec3 direction, float deltaTime, bool boost);
    void Rotate(float pitch, float yaw, float deltaTime);
    glm::mat4 GetViewMatrix();
    glm::vec3 GetPosition() { return m_position; };

private:
    glm::vec3 m_position;
    glm::vec3 m_forward;
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    float m_pitch = 0.0f;
    float m_yaw = -90.0f;
    float m_moveSpeed = 0.25f;
    float m_rotSpeed = 0.5f;
};

