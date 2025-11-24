#pragma once

#include "glm/glm.hpp"

class Camera {
public:
    Camera(glm::ivec2 size, glm::vec3 position, glm::vec3 look, glm::vec3 up, float moveSpeed, float rotateSpeed);
    ~Camera() = default;

    void Resize(glm::ivec2 newSize);

    void MoveCamera(glm::vec3 forwardRightUpInput, float dt);
    void RotateCamera(glm::vec2 xyInput);

    [[nodiscard]] glm::mat4 GetViewMatrix() const {
        return m_view;
    }

    [[nodiscard]] glm::mat4 GetProjectionMatrix() const {
        return m_projection;
    }

private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

    float m_aspectRatio;

    glm::vec3 m_position;
    glm::vec3 m_look;
    glm::vec3 m_up;

    float m_moveSpeed;
    float m_rotateSpeed;

    glm::mat4 m_view;
    glm::mat4 m_projection;

};
