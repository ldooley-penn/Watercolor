#pragma once

#include <memory>
#include <glad/glad.h>

#include "Texture2D.h"
#include "glm/vec2.hpp"

class Texture2D;

class Framebuffer {
    public:
    explicit Framebuffer(glm::ivec2 resolution, GLint colorTextureInternalFormat, GLint colorTextureFormat, GLint colorTextureType, const std::vector<TextureParameter>& colorTextureParameters);
    ~Framebuffer();

    void Bind() const;

    static void Unbind();

    [[nodiscard]] const std::unique_ptr<Texture2D>& GetColorTexture() const {
        return m_colorTexture;
    }

    void Resize(glm::ivec2 resolution);

private:
    void Destroy() const;
    void Generate();

    glm::ivec2 m_resolution;
    GLint m_colorTextureInternalFormat;
    GLenum m_colorTextureFormat;
    GLenum m_colorTextureType;

    GLuint m_fbo;
    std::unique_ptr<Texture2D> m_colorTexture;
    int m_colorTextureSlot;
    std::vector<TextureParameter> m_colorTextureParameters;
    GLuint m_rbo;
};

