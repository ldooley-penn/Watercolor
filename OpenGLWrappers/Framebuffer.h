#pragma once

#include <glad/glad.h>

#include "glm/vec2.hpp"

class Framebuffer {
    public:
    explicit Framebuffer(glm::ivec2 resolution, GLint colorTextureFormat = GL_RGB);
    ~Framebuffer();

    void Bind() const;

    static void Unbind();

    void BindColorTexture(int TextureSlot = 0);
    void UnbindColorTexture() const;

    void Resize(glm::ivec2 resolution);

private:
    void Destroy() const;
    void Generate();

    glm::ivec2 m_resolution;
    GLint m_colorTextureFormat;

    GLuint m_fbo;
    GLuint m_colorTexture;
    int m_colorTextureSlot;
};

