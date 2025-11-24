//
// Created by logan on 11/24/2025.
//

#include "Camera.h"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

Camera::Camera(glm::ivec2 size, glm::vec3 position, glm::vec3 look, glm::vec3 up, float moveSpeed, float rotateSpeed):
    m_aspectRatio(static_cast<float>(size.x) / static_cast<float>(size.y)),
    m_position(position),
    m_look(look),
    m_up(up),
    m_moveSpeed(moveSpeed),
    m_rotateSpeed(rotateSpeed),
    m_view(glm::mat4(1.0)),
    m_projection(glm::mat4(1.0))
{
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

void Camera::Resize(glm::ivec2 newSize) {
    m_aspectRatio = static_cast<float>(newSize.x) / static_cast<float>(newSize.y);
    UpdateProjectionMatrix();
}

void Camera::MoveCamera(glm::vec3 forwardRightUpInput, float dt) {
    if (forwardRightUpInput == glm::vec3(0.0)) {
        return;
    }

    glm::vec3 right = glm::cross(m_look, m_up);

    glm::vec3 moveDirection = m_look * forwardRightUpInput.x + right * forwardRightUpInput.y + m_up * forwardRightUpInput.z;

    m_position += moveDirection * m_moveSpeed * dt;
    UpdateViewMatrix();
}

void Camera::RotateCamera(glm::vec2 xyInput) {
}

void Camera::UpdateViewMatrix() {
    m_view = glm::lookAt(m_position, m_position + m_look, m_up);
}

void Camera::UpdateProjectionMatrix() {
    m_projection = glm::perspective(1.f, m_aspectRatio, 0.01f, 1000.f);
}
