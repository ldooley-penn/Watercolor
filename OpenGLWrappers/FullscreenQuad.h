#pragma once

#include <glad/glad.h>

class FullscreenQuad {
public:
    FullscreenQuad();
    ~FullscreenQuad();

    void Draw() const;

private:
    GLuint m_vbo;
    GLuint m_vao;
    GLuint m_ebo;
};
