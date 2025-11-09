#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>

#include "glm/vec2.hpp"

struct TextureParameter {
    GLenum pname;
    GLint value;
};

class Texture2D {
public:
    explicit Texture2D(const std::string& imageFilepath, const std::vector<TextureParameter>& textureParameters);
    explicit Texture2D(int width, int height, GLint internalFormat, GLenum format, GLenum type, const std::vector<TextureParameter>& textureParameters);
    ~Texture2D();

    void Bind(int TextureSlot = 0);
    void Unbind() const;

    [[nodiscard]] GLuint GetTextureID() const {
        return m_texture;
    }

    [[nodiscard]] glm::ivec2 GetSize() const {
        return {m_width, m_height};
    }

private:
    GLuint m_texture;

    int m_width;
    int m_height;
    GLint m_internalFormat;
    GLenum m_format;
    GLenum m_type;

    GLenum m_textureSlot;
};
